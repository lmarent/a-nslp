/// ----------------------------------------*- mode: C++; -*--
/// @file nr_session.cpp
/// The session for an NSIS Responder.
/// ----------------------------------------------------------
/// $Id: nr_session.cpp 3165 2015-08-31 11:07:00 amarentes $
/// $HeadURL: https://./src/nr_session.cpp $
// ===========================================================
//                      
// Copyright (C) 2012-2014, all rights reserved by
// - System and Computing Engineering, Universidad de los Andes
//
// More information and contact:
// https://www.uniandes.edu.co/
//                      
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; version 2 of the License
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//
// ===========================================================
#include <openssl/rand.h>

#include "logfile.h"

#include "mri.h"	// from NTLP

#include "anslp_config.h"
#include "events.h"
#include "msg/anslp_msg.h"
#include "dispatcher.h"


using namespace anslp;
using namespace anslp::msg;
using namespace protlib::log;
using protlib::uint32;


#define LogError(msg) ERRLog("nr_session", msg)
#define LogWarn(msg) WLog("nr_session", msg)
#define LogInfo(msg) ILog("nr_session", msg)
#define LogDebug(msg) DLog("nr_session", msg)


/**
 * Constructor.
 *
 * Use this if the session ID is known in advance.
 */
nr_session::nr_session(const session_id &id, anslp_config *conf)
		: session(id), state(STATE_ANSLP_CLOSE), config(conf),
		  lifetime(0), max_lifetime(0), state_timer(this) 
{

	set_session_type(st_receiver);
	assert( conf != NULL );

	set_max_lifetime(conf->get_nr_max_session_lifetime());
}


/**
 * Constructor for test cases.
 *
 * @param s the state to start in
 * @param msn the initial message sequence number
 */
nr_session::nr_session(nr_session::state_t s, uint32 msn)
		: session(), state(s), config(NULL),
		  lifetime(0), max_lifetime(60), state_timer(this) 
{
	set_session_type(st_receiver);
	set_msg_sequence_number(msn);
}


/**
 * Destructor.
 */
nr_session::~nr_session() 
{
	// nothing to do
}


std::ostream &anslp::operator<<(std::ostream &out, const nr_session &s) 
{
	static const char *const names[] = { "CLOSE", 
										 "PENDING", 
										 "AUCTIONING"};

	return out << "[nr_session: id=" << s.get_id()
		<< ", state=" << names[s.get_state()] << "]";
}

/****************************************************************************
 *
 * Utilities
 *
 ****************************************************************************/
/**
 * Return a copy of the saved auction rule if we have one.
 */
auction_rule *
nr_session::get_auction_rule_copy() const {
	if ( get_auction_rule() == NULL )
		return NULL;
	else
		return get_auction_rule()->copy();
}


/**
 * Create a auction rule from the given event and save it.
 *
 * If we receive a successful response later, we will activate the rule.
 * In case we don't support the requested policy rule, an exception is thrown.
 */
void 
nr_session::save_auction_rule(dispatcher *d, 
   							    msg_event *evt,
								std::vector<msg::anslp_mspec_object *> &missing_objects) 
			throw (request_error)
{
	
	using ntlp::mri_pathcoupled;

	LogDebug("Starting save_mt_policy_rule ");
	
	string session_id;

	std::vector<msg::anslp_mspec_object *> objects;

	assert( evt != NULL );

	anslp_create *create = evt->get_create();
	assert( create != NULL );

	mri_pathcoupled *pc_mri
		= dynamic_cast<mri_pathcoupled *>(evt->get_mri());

	if ( pc_mri == NULL ) {
		LogError("no path-coupled MRI found");
		throw request_error("no path-coupled MRI found",
			information_code::sc_permanent_failure,
			information_code::fail_configuration_failed); // failure
	}
	
	session_id = get_id().to_string();
	create->get_mspec_objects(objects);
	
	// Check which metering object could be installed in this node.
	std::vector<msg::anslp_mspec_object *>::const_iterator it_objects;
	for ( it_objects = objects.begin(); it_objects != objects.end(); it_objects++)
	{
		const anslp_mspec_object *object = *it_objects;
		if (check_participating(create->get_selection_auctioning_entities())){
			if (d->check(session_id, object)){
			   rule->set_request_object(object->copy());
			   missing_objects.push_back(object->copy());
			}
			else{
			   missing_objects.push_back(object->copy());
			}
		}
		else{
			missing_objects.push_back(object->copy());
		}
	}
	
	LogDebug("The auction rule has been established: ");
}


/**
 * Create an auctioning rule from the given event and return it.
 *
 */
auction_rule * 
nr_session::create_auction_rule(anslp_bidding *bidding) 
{
	
	LogDebug( "Begin create_auction_rule()");
	string session_id;

	assert( bidding != NULL );
	
	auction_rule *to_post = new auction_rule();
	
	std::vector<msg::anslp_mspec_object *> objects;

	bidding->get_mspec_objects(objects);
	
	LogDebug( "Nbr objects to check:" << objects.size() );
	
	// Check which metering object could be installed in this node.
	std::vector<msg::anslp_mspec_object *>::const_iterator it_objects;
	for ( it_objects = objects.begin(); it_objects != objects.end(); it_objects++)
	{
		const anslp_mspec_object *object = *it_objects;
		to_post->set_request_object(object->copy());
	}
	
	LogDebug("End create_auction_rule - objects inserted:" 
					<< to_post->get_request_objects()->size());
	
	return to_post;
}


/****************************************************************************
 *
 * The state machine.
 *
 ****************************************************************************/


/*
 * state: STATE_ANSLP_CLOSE
 */
nr_session::state_t 
nr_session::handle_state_close(dispatcher *d, event *evt) 
{
	using namespace msg;

	LogDebug("Starting handle_state_close ");
	
	string session_id = get_id().to_string();

	/*
	 * A msg_event arrived which contains a ANSLP create message.
	 */
	if ( is_anslp_create(evt) ) {
		msg_event *e = dynamic_cast<msg_event *>(evt);
		ntlp_msg *msg = e->get_ntlp_msg();
		anslp_create *c = e->get_create();

		uint32 lifetime;
		uint32 msn;
		lifetime = c->get_session_lifetime();
		msn = c->get_msg_sequence_number();

		// Before proceeding check several preconditions.
		try {
			check_lifetime(lifetime, get_max_lifetime());
			check_authorization(d, e);
		}
		catch ( request_error &exp ) {
			LogError(exp);
			d->send_message( msg->create_error_response(exp) );
			return STATE_ANSLP_CLOSE;
		}
		catch ( override_lifetime &exp) {
			lifetime = get_max_lifetime();
		}
		if ( lifetime > 0 ) {
			LogDebug("responder session initiated.");
			set_lifetime(lifetime);
			set_msg_sequence_number(msn);
			std::vector<msg::anslp_mspec_object *> missing_objects;
			save_auction_rule(d, e, missing_objects);
			
			auction_rule * result = d->install_auction_rules(session_id, rule);
			if (rule->get_number_mspec_request_objects() ==
					result->get_number_mspec_response_objects() ){
				// free the space allocated to the rule to be installed.
				delete(rule);
				// Assign the response as the rule installed.
				rule = result;
				ntlp_msg *resp = msg->create_success_response(lifetime);
				
				// Copy anslp messages into the response message.
				objectListIter_t iter;
				for (iter = result->get_response_objects()->begin(); 
						iter != result->get_response_objects()->end(); ++iter){
					dynamic_cast<anslp_response *>(msg->get_anslp_msg())->set_mspec_object((iter->second)->copy());
				}

				d->send_message(resp);
				state_timer.start(d, lifetime);
				return STATE_ANSLP_AUCTIONING;
			}	
			else
			{
				
				set_lifetime(0);
				delete(rule);
				// Assign the response as the rule installed.
				rule = result;
				// Uninstall the previous rules.
				if (rule->get_number_mspec_response_objects() > 0){
					d->remove_auction_rules(session_id, rule);
				}
					
				d->send_message( msg->create_response(
								information_code::sc_permanent_failure, 
								information_code::fail_internal_error) );
				return STATE_ANSLP_CLOSE;
			}				
		}
		else {
			LogWarn("invalid lifetime.");
			d->send_message( msg->create_response(
								 information_code::sc_permanent_failure, 
								 information_code::fail_configuration_failed) );
			return STATE_ANSLP_CLOSE;
		}
	}
	else {
		LogInfo("discarding unexpected event " << *evt);
		return STATE_ANSLP_CLOSE;
	}
}


/*
 * state: STATE_ANSLP_AUCTIONING
 */
nr_session::state_t nr_session::handle_state_auctioning(
		dispatcher *d, event *evt) 
{
	LogDebug("Starting handle_STATE_ANSLP_AUCTIONING ");

	string session_id;

	/*
	 * A msg_event arrived which contains a ANSLP REFRESH message.
	 */
	if ( is_anslp_refresh(evt) ) 
	{
		msg_event *e = dynamic_cast<msg_event *>(evt);
		ntlp_msg *msg = e->get_ntlp_msg();
		anslp_refresh *c = e->get_refresh();

		uint32 lifetime = c->get_session_lifetime();
		uint32 msn = c->get_msg_sequence_number();
	
		// Before proceeding check several preconditions.
		try {
			check_lifetime(lifetime, get_max_lifetime());
			check_authorization(d, e);
		}
		catch ( override_lifetime &exp) {
			lifetime = get_max_lifetime();
		}
		catch ( request_error &exp ) {
			LogError(exp);
			d->send_message( msg->create_error_response(exp) );
			return STATE_ANSLP_AUCTIONING;
		}

		if ( ! is_greater_than(msn, get_msg_sequence_number()) ) {
			LogWarn("duplicate response received.");
			return STATE_ANSLP_AUCTIONING; // no change
		}
		else if ( lifetime > 0 ) {
			LogDebug("authentication succesful.");

			set_lifetime(lifetime); // could be a new lifetime!
			set_msg_sequence_number(msn);

			ntlp_msg *resp = msg->create_success_response(lifetime);

			d->send_message(resp);

			state_timer.restart(d, lifetime);

			return STATE_ANSLP_AUCTIONING; // no change
		}
		else if ( lifetime == 0 ) 
		{
			LogInfo("terminating session on NI request.");
		
			session_id = get_id().to_string();
			
			// Uninstall the previous rules.
			if (rule->get_number_mspec_response_objects() > 0)
				d->remove_auction_rules(session_id, rule);
			
			ntlp_msg *resp = msg->create_success_response(lifetime);
		
			d->send_message(resp);
			
			state_timer.stop();

			return STATE_ANSLP_CLOSE;
		}
		else 
		{
			LogWarn("invalid lifetime.");

			return STATE_ANSLP_AUCTIONING; // no change
		}
	}

	/*
	 * API bidding event received. The user wants to send an object to the auction server.
	 */
	else if ( is_anslp_bidding(evt) ) {
		LogDebug("received API bidding event");

		msg_event *e = dynamic_cast<msg_event *>(evt);
		ntlp_msg *msg = e->get_ntlp_msg();
		anslp_bidding *bidding = e->get_bidding();

		if (e->is_for_this_node()) {
			// The message is for us, so we send it to the install policy
			// This messages are without any response. 
			// As it is implemented, we delegate the upper layer to retry to send them again.
			
			session_id = get_id().to_string();
						
			auction_rule * to_post = create_auction_rule(bidding);
			
			auction_rule * result = d->auction_interaction(session_id, to_post);
			
			saveDelete(to_post);
			
			saveDelete(result);
			
		} 
		
		LogDebug("Ending state handle_state_auctioning - bidding event ");

		return STATE_ANSLP_AUCTIONING; // no change
		
	}
	
	/*
	 * The session timeout was triggered.
	 */
	else if ( is_timer(evt, state_timer) ) {
		LogWarn("session timed out.");
		
		session_id = get_id().to_string();
		
		// Uninstall the previous rules.
		if (rule->get_number_mspec_response_objects() > 0)
			d->remove_auction_rules(session_id, rule);

		d->report_async_event("session timed out");
		return STATE_ANSLP_CLOSE;
	}
	
	/*
	 * Outdated timer event, discard and don't log.
	 */
	else if ( is_timer(evt) ) 
	{
		return STATE_ANSLP_AUCTIONING;// no change
	}
	else 
	{
		LogInfo("discarding unexpected event " << *evt);

		return STATE_ANSLP_AUCTIONING; // no change
	}
}


/**
 * Process an event.
 *
 * This method implements the transition function of the state machine.
 */
void nr_session::process_event(dispatcher *d, event *evt) 
{
	LogDebug("begin process_event(): " << *this);
	
	switch ( get_state() ) {

		case nr_session::STATE_ANSLP_CLOSE:
			state = handle_state_close(d, evt);
			break;

		case nr_session::STATE_ANSLP_AUCTIONING:
			state = handle_state_auctioning(d, evt);
			break;

		default:
			assert( false ); // invalid state
	}

	LogDebug("end process_event(): " << *this);
}
