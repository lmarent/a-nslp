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
		: session(id), state(STATE_ANSLP_CLOSE),routing_info(NULL), config(conf),
		  msn_bidding(0), lifetime(0), max_lifetime(0), create_counter(0), state_timer(this), 
		  response_timer(this), act_rule(NULL), create_message(NULL)
{

	set_session_type(st_receiver);
	assert( conf != NULL );

	set_msg_bidding_sequence_number(create_random_number());
	set_max_lifetime(conf->get_nr_max_session_lifetime());
	set_max_retries(conf->get_nr_max_retries());
}


/**
 * Constructor for test cases.
 *
 * @param s the state to start in
 * @param msn the initial message sequence number
 */
nr_session::nr_session(nr_session::state_t s, anslp_config *conf, uint32 msn)
		: session(), state(s), routing_info(NULL), config(conf),
		  msn_bidding(0), lifetime(0), max_lifetime(60), create_counter(0), 
		  max_retries(3), state_timer(this),
		  response_timer(this), act_rule(NULL), create_message(NULL) 
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
	
	if (routing_info != NULL){
		delete routing_info;
	}	
	
	if (create_message != NULL){
		delete create_message;
	}
	
	if ( act_rule != NULL ){
		delete act_rule;
	}
	
}

/**
 * Generate a 32 Bit random number.
 */
uint32 nr_session::create_random_number() const 
{
	unsigned value;
	int ret = RAND_bytes((unsigned char *) &value, sizeof(value));
	assert( ret == 1 );

	return value;
}

std::ostream &anslp::operator<<(std::ostream &out, const nr_session &s) 
{
	static const char *const names[] = { "CLOSE", 
										 "PENDING",
										 "PENDING_INSTALLING",
										 "AUCTIONING",
										 "PENDING_TEARDOWN"};

	return out << "[nr_session: id=" << s.get_id()
		<< ", state=" << names[s.get_state()] << "]";
}

/**
 * Increment the Message Sequence Number.
 *
 * @return the new (incremented) MSN_BIDDING
 */
uint32 nr_session::next_msg_bidding_sequence_number() 
{
	return ++msn_bidding;	// UINT_MAX+1 = 0, so wrap arounds as per RFC-1982 work
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
bool
nr_session::save_auction_rule(dispatcher *d, 
   							   anslp_create *create,
							   std::vector<msg::anslp_mspec_object *> &missing_objects) 
			throw (request_error)
{
	
	LogDebug("Starting save_mt_policy_rule ");
	
	string session_id;

	assert( create != NULL );
	
	session_id = get_id().to_string();
	create->get_mspec_objects(missing_objects);

	// Delete all posible request objects previously created by another message.
	if (rule->get_number_mspec_request_objects() > 0){
		objectListIter_t it;		
		for ( it = rule->get_request_objects()->begin(); it != rule->get_request_objects()->end(); it++)
		{
			if (it->second != NULL)
				delete(it->second);
		}		
		rule->get_request_objects()->clear();
	}
	
	// Assign objects as requests.
	std::vector<msg::anslp_mspec_object *>::iterator it;
	for (it = missing_objects.begin(); it != missing_objects.end(); ++it){
		mspec_rule_key key;
		anslp_mspec_object *object = *it;
		rule->set_request_object(key,object->copy());
	}
	
	if (check_participating(create->get_selection_auctioning_entities()))
	{
		return d->check(session_id, rule->get_request_objects());
	}
	
	return false;  // No participating.
	
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

/**
 * Build a A-NSLP bidding message based the api bidding event.
 *  This function makes a deep copy of the mspec_objects. So the session
 *  object can destroy them.
 *
 */
msg::ntlp_msg *nr_session::build_bidding_message(api_bidding_event *e ) 
{
	using namespace anslp::msg;

	assert( get_mri() != NULL );
	
	/*
	 * Create the message routing information (MRI) which we're going to
	 * use for all messages.
	 */
	ntlp::mri *nslp_mri = get_mri()->copy();
	nslp_mri->invertDirection();

	/*
	 * Build a A-NSLP bidding message.
	 */
	anslp_bidding *bidding = new anslp_bidding();
		
	bidding->set_msg_sequence_number(next_msg_bidding_sequence_number());

	// Set the objects to install.
	objectListConstIter_t it_objects;
	for ( it_objects = e->getObjects()->begin(); 
			it_objects != e->getObjects()->end(); it_objects++)
	{
		bidding->set_mspec_object((it_objects->second)->copy());
	}
	

	/*
	 * Wrap the bidding inside an ntlp_msg and add session ID and MRI.
	 */
	ntlp_msg *msg = new ntlp_msg(get_id(), bidding, nslp_mri->copy(), 0);
	
	saveDelete(nslp_mri);
	
	return msg;
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
	using ntlp::mri_pathcoupled;

	LogDebug("Starting handle_state_close ");
	
	string session_id = get_id().to_string();

	/*
	 * A msg_event arrived which contains a ANSLP create message.
	 */
	if ( is_anslp_create(evt) ) {
		msg_event *e = dynamic_cast<msg_event *>(evt);
		ntlp_msg *msg = e->get_ntlp_msg();
		anslp_create *c = e->get_create();
		
		set_mri((msg->get_mri())->copy());
			
		uint32 lifetime;
		uint32 msn;
		lifetime = c->get_session_lifetime();
		msn = c->get_msg_sequence_number();
				
		set_response_timeout(config->get_nr_response_timeout());
				
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

		// store one copy for further reference and pass one on
		set_last_create_message( msg->copy() );
		
		if ( lifetime > 0 ) {
			
			LogDebug("responder session initiated.");
			set_lifetime(lifetime);
			set_msg_sequence_number(msn);
			std::vector<msg::anslp_mspec_object *> missing_objects;
			
			try{

				mri_pathcoupled *pc_mri
					= dynamic_cast<mri_pathcoupled *>(e->get_mri());

				if ( pc_mri == NULL ) {
					LogError("no path-coupled MRI found");
					throw request_error("no path-coupled MRI found",
						information_code::sc_permanent_failure,
						information_code::fail_configuration_failed); // failure
				}
				
				bool participating = save_auction_rule(d, c, missing_objects);
				
				if ( participating ) 
				{
					
					inc_create_counter();
					state_timer.start(d, lifetime);
						
					return STATE_ANSLP_PENDING;
				} else {

					// Assign an empty rule as the router is not participating. 
					saveDelete(rule);
					rule = new auction_rule();
									
					ntlp_msg *resp = msg->create_success_response(lifetime);
					anslp_response *response = resp->get_anslp_response();
					assert( response != NULL );
									
					// Copy anslp messages into the response message.
					objectListIter_t iter;
					for (iter = rule->get_response_objects()->begin(); 
							iter != rule->get_response_objects()->end(); ++iter){
						response->set_mspec_object((iter->second)->copy());
					}

					d->send_message(resp);
					
					state_timer.start(d, lifetime);
								
					return STATE_ANSLP_AUCTIONING;
					
				}
			} 
			catch (auction_rule_installer_error &err)
			{
				
				d->send_message( msg->create_response(
								 information_code::sc_permanent_failure, 
								 information_code::fail_configuration_failed) );
								 
				return STATE_ANSLP_CLOSE;
			}	
		}	
		else
		{
				
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
 * state: STATE_ANSLP_PENDING
 */
nr_session::state_t 
nr_session::handle_state_pending(dispatcher *d, event *evt) 
{
	using namespace msg;

	LogDebug("Starting handle_state_pending");
	
	string session_id = get_id().to_string();

	/*
	 * A response timeout was triggered.
	 */
	if ( is_no_next_node_found_event(evt)
			|| is_timer(evt, state_timer) ) {

		// Retry. Send the Create message again and start a new timer.
		if ( get_create_counter() < get_max_retries() ) {
			
			LogWarn("response timeout, restarting timer.");
			
			inc_create_counter();
			
			anslp_create *c = get_last_create_message()->get_anslp_create();
			
			std::vector<msg::anslp_mspec_object *> missing_objects;

			// Resend the check event.
			save_auction_rule(d, c, missing_objects);

			state_timer.start(d, lifetime);

			return STATE_ANSLP_PENDING; // no change
		}
		else {
			// Timeout could not check for objets, abort
			
			ntlp_msg *msg = get_last_create_message();
			
			d->send_message( msg->create_response(
							information_code::sc_permanent_failure, 
							information_code::fail_internal_error) );

			return STATE_ANSLP_CLOSE;
		}
	}

	/*
	 * A msg_event arrived which contains a ANSLP api install message.
	 */
	else if ( is_api_check(evt) ) {
				
		api_check_event *e = dynamic_cast<api_check_event *>(evt);
						
		LogDebug("responder session installed.");
		
		// We loop through spec objects and remove those not included in the check message.
		objectListRevIter_t itc_objects;
		for ( itc_objects = rule->get_request_objects()->rbegin(); 
				itc_objects != rule->get_request_objects()->rend(); itc_objects++)
		{
			mspec_rule_key key = itc_objects->first;
			objectListIter_t itc_objects2;
			if ( e->getObjects()->find(key) == e->getObjects()->end()){
				delete itc_objects->second;
				rule->get_request_objects()->erase (key);
			}
		}

		// reinitiate create counter.
		create_counter = 0;
		d->install_auction_rules(session_id, rule);		
		
		state_timer.start(d, lifetime);
						
		return STATE_ANSLP_PENDING_INSTALLING;
	}
			
	else {
		LogInfo("discarding unexpected event " << *evt);
		return STATE_ANSLP_PENDING;
	}
}



/*
 * state: STATE_ANSLP_PENDING_INSTALLING
 */
nr_session::state_t 
nr_session::handle_state_pending_installing(dispatcher *d, event *evt) 
{
	using namespace msg;

	LogDebug("Starting handle_state_pending_installing ");
	
	string session_id = get_id().to_string();

	/*
	 * A response timeout was triggered.
	 */
	if ( is_timer(evt, state_timer) ) {

		// Retry. Send the Create message again and start a new timer.
		if ( get_create_counter() < get_max_retries() ) {
			
			LogWarn("response timeout, restarting timer.");
			
			inc_create_counter();
			
			d->install_auction_rules(session_id, rule);

			state_timer.start(d, lifetime);

			return STATE_ANSLP_PENDING_INSTALLING; // no change
		}
		// Retry count exceeded, abort.
		else {
			
			ntlp_msg *msg = get_last_create_message();
			
			d->send_message( msg->create_response(
							information_code::sc_permanent_failure, 
							information_code::fail_internal_error) );

			return STATE_ANSLP_CLOSE;
		}
	}	

	/*
	 * A msg_event arrived which contains an outdated ANSLP api check message.
	 */
	else if ( is_api_check(evt) ) {
		return STATE_ANSLP_PENDING_INSTALLING;
	}
	/*
	 * A msg_event arrived which contains a ANSLP api install message.
	 */
	else if ( is_api_install(evt) ) {
						
		api_install_event *e = dynamic_cast<api_install_event *>(evt);
				
		ntlp_msg *msg = get_last_create_message();
						
		LogDebug("responder session installed." << rule->get_number_mspec_request_objects());
				
		// Verify that every rule that passed the checking process could be installed.
		if (rule->get_number_mspec_request_objects() 
				== e->getObjects()->size() ){
						
			// Assign as the rule installed the response. 
			set_reponse_objects(e, rule);
							
			ntlp_msg *resp = msg->create_success_response(lifetime);
			anslp_response *response = resp->get_anslp_response();
			assert( response != NULL );
							
			// Copy anslp messages into the response message.
			objectListIter_t iter;
			for (iter = rule->get_response_objects()->begin(); 
					iter != rule->get_response_objects()->end(); ++iter){
				response->set_mspec_object((iter->second)->copy());
			}

			d->send_message(resp);
			
			state_timer.start(d, lifetime);
						
			return STATE_ANSLP_AUCTIONING;
		}	
		else
		{
			
			set_lifetime(0);

			// Uninstall the previous rules.
			if (rule->get_number_mspec_response_objects() > 0){
				d->remove_auction_rules(session_id, rule->copy());
			}
				
			d->send_message( msg->create_response(
							information_code::sc_permanent_failure, 
							information_code::fail_internal_error) );
							
			return STATE_ANSLP_CLOSE;
		}				
	}
		
	else {
		LogInfo("discarding unexpected event " << *evt);
		return STATE_ANSLP_PENDING_INSTALLING;
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
		LogDebug(" is_anslp_refresh " );
		
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
		
		LogDebug(" duplicate response received. " << "msn:" << msn << "messaSN:" << get_msg_sequence_number());
		
		if ( ! is_greater_than(msn, get_msg_sequence_number()) ) {
		
			LogWarn("duplicate response received.");
			return STATE_ANSLP_AUCTIONING; // no change
		
		} else if ( lifetime > 0 ) {
			
			LogInfo("authentication succesful.");

			set_lifetime(lifetime); // could be a new lifetime!
			set_msg_sequence_number(msn);

			ntlp_msg *resp = msg->create_success_response(lifetime);

			d->send_message(resp);

			state_timer.restart(d, lifetime);

			return STATE_ANSLP_AUCTIONING; // no change

		} else if ( lifetime == 0 ) {

			LogInfo("terminating session on NI request.");
					
			session_id = get_id().to_string();

			// Respond success for the requester.
			ntlp_msg *resp = msg->create_success_response(lifetime);
		
			d->send_message(resp);
			
			// Uninstall the previous rules.
			if (rule->get_number_mspec_response_objects() > 0){
				
				d->remove_auction_rules(session_id, rule);

				state_timer.start(d, lifetime);
																					
				return STATE_ANSLP_PENDING_TEARDOWN;
				
			} else {
							
				state_timer.stop();

				LogInfo("session terminated on NI request.");
			
				return STATE_ANSLP_CLOSE;
			}
		
		} else {
		
			LogWarn("invalid lifetime.");

			return STATE_ANSLP_AUCTIONING; // no change
		}
	}

	/*
	 * API bidding event received. The user wants to send an object to the auction server.
	 */
	else if ( is_anslp_bidding(evt) ) {
		
		LogDebug("received anslp bidding event");

		msg_event *e = dynamic_cast<msg_event *>(evt);
		ntlp_msg *msg = e->get_ntlp_msg();
		anslp_bidding *bidding = e->get_bidding();

		if (e->is_for_this_node()) {
			// The message is for us, so we send it to the install policy
			// This messages are without any response. 
			// As it is implemented, we delegate the upper layer to retry to send them again.
			
			session_id = get_id().to_string();
						
			auction_rule * to_post = create_auction_rule(bidding);
			
			auction_rule * result = d->auction_interaction(true, session_id, to_post);
			
			saveDelete(to_post);
			
			saveDelete(result);
			
		} 
		
		LogDebug("Ending state handle_state_auctioning - bidding event ");

		return STATE_ANSLP_AUCTIONING; // no change
		
	}

	/*
	 * API bidding event received. The user wants to send an object to the auction server.
	 */
	else if ( is_api_bidding(evt) ) {
		
		LogDebug("received API bidding event");
				
		api_bidding_event *e = dynamic_cast<api_bidding_event *>(evt);
		
		// Build the bidding message based on those objects not installed.
		d->send_message( build_bidding_message(e) );
		
		// Send the response saying that an event was processed for the
		// session id.
		if ( e->get_return_queue() != NULL ) {
			message *m = new anslp_event_msg(get_id(), NULL);
			e->get_return_queue()->enqueue(m);
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
		
		d->report_async_event("session timed out");

		// Uninstall the previous rules.
		if (rule->get_number_mspec_response_objects() > 0){

			d->remove_auction_rules(session_id, rule);

			state_timer.start(d, lifetime);
																	
			return STATE_ANSLP_PENDING_TEARDOWN;
		} else {
							
			state_timer.stop();
			
			return STATE_ANSLP_CLOSE;
		}
		
	}
	
	/*
	 * Outdated timer event, discard and don't log.
	 */
	else if ( is_timer(evt) ) 
	{
		LogDebug(" is_timer ");
		
		return STATE_ANSLP_AUCTIONING;// no change
	}
	else 
	{
		LogInfo("discarding unexpected event " << *evt);

		return STATE_ANSLP_AUCTIONING; // no change
	}
}



/*
 * state: STATE_ANSLP_PENDING_TEARDOWN
 */
nr_session::state_t 
nr_session::handle_state_pending_teardown(dispatcher *d, event *evt) 
{
	using namespace msg;

	LogDebug("Starting handle_state_pending_teardown ");
	
	string session_id = get_id().to_string();

	/*
	 * A response timeout was triggered.
	 */
	if ( is_timer(evt, state_timer) ) {

		// Retry. Send the Create message again and start a new timer.
		if ( get_create_counter() < get_max_retries() ) {
			
			LogWarn("response timeout, restarting timer.");
			
			inc_create_counter();
			
			d->remove_auction_rules(session_id, rule);

			state_timer.start(d, lifetime);

			return STATE_ANSLP_PENDING_TEARDOWN; // no change
		}
		// Retry count exceeded, abort.
		else {
			
			return STATE_ANSLP_CLOSE;
		}
	}	

	/*
	 * A msg_event arrived which contains an outdated ANSLP api check message.
	 */
	else if ( is_api_check(evt) ) {
		return STATE_ANSLP_PENDING_TEARDOWN;
	}
	
	/*
	 * A msg_event arrived which contains an outdated ANSLP api install message.
	 */	
	else if ( is_api_install(evt) ) {
		return STATE_ANSLP_PENDING_TEARDOWN;
	}

	/*
	 * API bidding event received. The user wants to send an object to the auction server.
	 * it is discarded.
	 */
	else if ( is_anslp_bidding(evt) ) {
		
		LogDebug("received anslp bidding event");

		return STATE_ANSLP_PENDING_TEARDOWN; // no change
		
	}

	/*
	 * API bidding event received. The user wants to send an object from the auction server.
	 */
	else if ( is_api_bidding(evt) ) {
		
		LogDebug("received API bidding event");
				
		api_bidding_event *e = dynamic_cast<api_bidding_event *>(evt);
		
		// Build the bidding message based on those objects not installed.
		d->send_message( build_bidding_message(e) );
		
		// Send the response saying that an event was processed for the
		// session id.
		if ( e->get_return_queue() != NULL ) {
			message *m = new anslp_event_msg(get_id(), NULL);
			e->get_return_queue()->enqueue(m);
		}
		
		LogDebug("Ending state handle_state_auctioning - bidding event ");
		
		return STATE_ANSLP_PENDING_TEARDOWN; // no change
		
	}

	/*
	 * A msg_event arrived which contains a ANSLP api install message.
	 */
	else if ( is_api_remove(evt) ) {
						
		api_remove_event *e = dynamic_cast<api_remove_event *>(evt);
																			
		state_timer.stop();
			
		return STATE_ANSLP_CLOSE;

	}	
		
	else {
		LogInfo("discarding unexpected event " << *evt);
		return STATE_ANSLP_PENDING_TEARDOWN;
	}
}


/**
 * Process an event.
 *
 * This method implements the transition function of the state machine.
 */
void nr_session::process_event(dispatcher *d, event *evt) 
{
	LogInfo("begin process_event(): " << *this << "SessionId:" << get_id().to_string());
	
	switch ( get_state() ) {

		case nr_session::STATE_ANSLP_CLOSE:
			state = handle_state_close(d, evt);
			break;

		case nr_session::STATE_ANSLP_PENDING:
			state = handle_state_pending(d, evt);
			break;

		case nr_session::STATE_ANSLP_PENDING_INSTALLING:
			state = handle_state_pending_installing(d, evt);
			break;

		case nr_session::STATE_ANSLP_AUCTIONING:
			state = handle_state_auctioning(d, evt);
			break;

		case nr_session::STATE_ANSLP_PENDING_TEARDOWN:
			state = handle_state_pending_teardown(d, evt);
			break;

		default:
			LogError("Invalid State(): " << get_state());
			assert( false ); // invalid state
	}

	LogInfo("end process_event(): " << *this << "SessionId:" << get_id().to_string());
}
