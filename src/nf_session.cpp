/// ----------------------------------------*- mode: C++; -*--
/// @file nf_session.cpp
/// The session for an NSIS Forwarder.
/// ----------------------------------------------------------
/// $Id: nf_session.cpp 3165 2015-08-31 11:10:00 amarentes $
/// $HeadURL: https://./src/nf_session.cpp $
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
#include "logfile.h"

#include "mri.h"	// from NTLP
#include "anslp_config.h"
#include "events.h"
#include "msg/anslp_msg.h"
#include "dispatcher.h"
#include "session.h"
#include "msg/information_code.h"
#include <iostream>


using namespace anslp;
using namespace protlib::log;
using protlib::uint32;


#define LogError(msg) ERRLog("nf_session", msg)
#define LogWarn(msg) WLog("nf_session", msg)
#define LogInfo(msg) ILog("nf_session", msg)
#define LogDebug(msg) DLog("nf_session", msg)

#define LogUnimp(msg) Log(ERROR_LOG, LOG_UNIMP, "nf_session", \
	msg << " at " << __FILE__ << ":" << __LINE__)


/**
 * Constructor.
 *
 * Use this if the session ID is known in advance.
 */
nf_session::nf_session(const session_id &id, const anslp_config *conf)
		: session(id), state(nf_session::STATE_ANSLP_CLOSE), config(conf),
		  proxy_mode(false), lifetime(0), max_lifetime(0),
		  response_timeout(0), state_timer(this), response_timer(this),
		  ni_mri(NULL), nr_mri(NULL), create_message(NULL), 
		  refresh_message(NULL)
{
	set_session_type(st_forwarder);
	assert( config != NULL );
}


/**
 * Constructor for test cases.
 *
 * @param s the state to start in
 */
nf_session::nf_session(nf_session::state_t s, const anslp_config *conf)
		: session(), state(s), config(conf),
		  proxy_mode(false), lifetime(0), max_lifetime(60),
		  response_timeout(0), state_timer(this), response_timer(this),
		  ni_mri(NULL), nr_mri(NULL), create_message(NULL),
		  refresh_message(NULL)
{
	set_session_type(st_forwarder);
}


/**
 * Destructor.
 */
nf_session::~nf_session() 
{
	
	if (ni_mri != NULL)
		delete ni_mri;
	
	if (nr_mri != NULL)
		delete nr_mri;
	
	if (create_message != NULL)
		delete create_message;
	
	if (refresh_message != NULL)
		delete refresh_message;
		
}


std::ostream &anslp::operator<<(std::ostream &out, const nf_session &s) {
	static const char *const names[] = { "CLOSE", 
										 "PENDING", 
										 "METERING" };

	return out << "[nf_session: id=" << s.get_id()
		<< ", state=" << names[s.get_state()] << "]";
}


/****************************************************************************
 *
 * Utilities
 *
 ****************************************************************************/

/**
 * Copy the given message and adjust it for forwarding to the NR.
 *
 */
ntlp_msg *nf_session::create_msg_for_nr(ntlp_msg *msg) const 
{
	return msg->copy_for_forwarding();
}


/**
 * Copy the given message and adjust it for forwarding to the NI.
 *
 */
ntlp_msg *nf_session::create_msg_for_ni(ntlp_msg *msg) const 
{
	return msg->copy_for_forwarding();
}


void nf_session::set_pc_mri(msg_event *evt) throw (request_error)
{
	using ntlp::mri_pathcoupled;
	
	LogDebug( "Begin set_pc_mri()");

	assert( evt != NULL );

	mri_pathcoupled *pc_mri
		= dynamic_cast<mri_pathcoupled *>(evt->get_mri());

	if ( pc_mri == NULL ) 
	{
		LogError("no path-coupled MRI found");
		throw request_error("no path-coupled MRI found",
			information_code::sc_permanent_failure,
			information_code::fail_configuration_failed); // failure
	}

	assert( pc_mri->get_downstream() == true );

	// store the rewritten MRIs for later use
	set_nr_mri( create_mri_with_dest(pc_mri) );
	set_ni_mri( create_mri_inverted(pc_mri) );

	LogDebug( "End set_pc_mri()");

}

/**
 * Create an auctioning policy rule from the given event and save it.
 *
 * If we receive a successful response later, we will activate the rule.
 * In case we don't support the requested policy rule, an exception is thrown.
 */
void nf_session::set_auction_rule(dispatcher *d, 
								 msg_event *evt,
								 std::vector<msg::anslp_mspec_object *> &missing_objects) 
{
	
	LogDebug( "Begin set_auction_rule()");
	string session_id;
	
	std::vector<msg::anslp_mspec_object *> objects;
	assert( evt != NULL );
	anslp_create *create = evt->get_create();
	assert( create != NULL );
	create->get_mspec_objects(objects);
	
	LogDebug( "Nbr objects to check:" << objects.size() 
			   << "sel auct entities:" << create->get_selection_auctioning_entities() );
	
	// Check which metering object could be installed in this node.
	std::vector<msg::anslp_mspec_object *>::const_iterator it_objects;
	for ( it_objects = objects.begin(); it_objects != objects.end(); it_objects++)
	{
		const anslp_mspec_object *object = *it_objects;
		if (check_participating( create->get_selection_auctioning_entities()))
		{
			try 
			{
				session_id = get_id().to_string();

				LogDebug( "Checking element:" );
				
				if (d->check(session_id, object)) {
				   rule->set_request_object(object->copy());
				   missing_objects.push_back(object->copy());
				} else {
				   missing_objects.push_back(object->copy());
				}
			} catch(std::invalid_argument &e) {
				missing_objects.push_back(object->copy());
			}
		}
		else {
			missing_objects.push_back(object->copy());
		}
	}
	
	LogDebug("End set_auction_rule - objects inserted:" 
					<< rule->get_request_objects()->size());
}


/**
 * Create an auctioning rule from the given event and return it.
 *
 */
auction_rule * 
nf_session::create_auction_rule(anslp_bidding *bidding) 
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


// For implementation of auctioning interface.


/****************************************************************************
 *
 * The state machine
 *
 ****************************************************************************/



msg::ntlp_msg *
nf_session::build_create_message(msg_event *e, 
								std::vector<msg::anslp_mspec_object *> & missing_objects) 
{
	using namespace anslp::msg;
	
	LogDebug( "Begin build_create_message");
		
	assert( get_nr_mri() != NULL );
	
	anslp_create *c = e->get_create();

	/*
	 * Build a ANSLP Create message.
	 */
	anslp_create *create = new anslp_create();
	
	if (get_lifetime() <= c->get_session_lifetime())
		create->set_session_lifetime(get_lifetime());
	else
		create->set_session_lifetime(c->get_session_lifetime());
		
	create->set_msg_sequence_number(c->get_msg_sequence_number());
	create->set_selection_auctioning_entities(c->get_selection_auctioning_entities());
	create->set_message_hop_count(c->get_message_hop_count());

	/*
	 * Insert missing objects to install.
	 */
	std::vector<msg::anslp_mspec_object *>::iterator it_objects;
	for ( it_objects = missing_objects.begin(); 
			it_objects != missing_objects.end(); it_objects++ )
	{
		LogDebug( "Adding object to message");
		create->set_mspec_object((*it_objects)->copy());
	}

	/*
	 * Wrap the Create inside an ntlp_msg and add session ID and MRI.
	 */
	ntlp_msg *msg = new ntlp_msg(get_id(), create, get_nr_mri()->copy(), 0);

	LogDebug( "End build_create_message");
	
	return msg;
}


/**
 * Build a A-NSLP Refresh message to teardown other configurations
 * This could happend when the something went wrong installing the policy rules
 *
 * This will fetch the MSN and also increment the MSN.
 */
msg::ntlp_msg *nf_session::build_teardown_message() 
{
	using namespace anslp::msg;
	
	LogDebug( "Begin build_teardown_message");

	assert( get_nr_mri() != NULL );

	/*
	 * Build a ANSLP Refresh message.
	 */
	anslp_refresh *refresh = new anslp_refresh();
	refresh->set_session_lifetime(0);
	refresh->set_msg_sequence_number(next_msg_sequence_number());
	
	/*
	 * Wrap the Create inside an ntlp_msg and add session ID and MRI.
	 */
	ntlp_msg *msg = new ntlp_msg(get_id(), refresh, get_nr_mri()->copy(), 0);
	
	LogDebug( "Ending build_teardown_message");
	return msg;
}

nf_session::state_t
nf_session::process_state_close(dispatcher *d, event *evt)
{
	using ntlp::mri_pathcoupled;
	
	LogDebug( "Begin process State Close");
	
	msg_event *e = dynamic_cast<msg_event *>(evt);
	ntlp_msg *msg = e->get_ntlp_msg();

	// store one copy for further reference and pass one on
	anslp_create *c = e->get_create();
	set_last_create_message( msg->copy() );
	try 
	{
		std::vector<msg::anslp_mspec_object *> missing_objects;
				
		set_pc_mri(e);
		
		set_auction_rule(d, e, missing_objects);

		mri_pathcoupled *mri_pc
						= dynamic_cast<mri_pathcoupled *>(msg->get_mri());
		assert( mri_pc != NULL );

		hostaddress ni_address = e->get_mri()->get_sourceaddress();

		// Create the new message to send foreward.
		d->send_message( build_create_message(e, missing_objects) );

		state_timer.start(d, get_lifetime());

		// Release the memory allocated to the missing objects vector
		std::vector<msg::anslp_mspec_object *>::iterator it_objects;
		for ( it_objects = missing_objects.begin(); 
				it_objects != missing_objects.end(); it_objects++)
		{
			delete(*it_objects);
		}
		missing_objects.clear();
		
		LogDebug( "Ending process State Close New state PENDING");		
		return STATE_ANSLP_PENDING;
	}	
	catch ( auction_rule_installer_error &exc ) {
		LogError("auction rule not supported: " << exc);
		d->send_message( msg->create_error_response(exc) );
		return STATE_ANSLP_CLOSE;
	}
	catch ( request_error &exc ) {
		LogError("Mri information not found : " << exc);
		d->send_message( msg->create_error_response(exc) );
		return STATE_ANSLP_CLOSE;	
	}
	
}


nf_session::state_t 
nf_session::handle_state_close(dispatcher *d, event *evt) 
{

	using namespace msg;
	using ntlp::mri_pathcoupled;
	
	LogDebug("begin handle_state_close(): " << *this);

	/*
	 * A msg_event arrived which contains a ANSLP Create message.
	 */
	if ( is_anslp_create(evt) ) {
		msg_event *e = dynamic_cast<msg_event *>(evt);
		ntlp_msg *msg = e->get_ntlp_msg();

		// store one copy for further reference and pass one on
		anslp_create *c = e->get_create();
		uint32 lifetime = c->get_session_lifetime();
		uint32 msn = c->get_msg_sequence_number();

		set_max_lifetime(config->get_nf_max_session_lifetime());
		set_response_timeout(config->get_nf_max_session_lifetime());

		/*
		 * Before proceeding check several preconditions.
		 */
		try {
			check_lifetime(lifetime, get_max_lifetime());
			check_authorization(d, e);
		}
		catch ( override_lifetime &e) {
			lifetime = get_max_lifetime();
		}
		catch ( request_error &e ) {
			LogError(e);
			d->send_message( msg->create_error_response(e) );
			return STATE_ANSLP_CLOSE;
		}
		if ( lifetime == 0 ) {
			LogWarn("lifetime == 0, discarding message");
			return STATE_ANSLP_CLOSE;
		}
		/*
		 * All basic preconditions match, the CREATE seems to be valid.
		 * Now setup the session.
		 */
		LogDebug("forwarder session initiated, waiting for a response");

		set_lifetime(lifetime);
		set_msg_sequence_number(msn);
		
		return process_state_close(d, evt);

	}
	
}

/*
 * state: STATE_ANSLP_PENDING
 */
nf_session::state_t 
nf_session::handle_state_pending(dispatcher *d, event *evt) {

	using namespace anslp::msg;
	string session_id;

	LogDebug("begin handle_state_pending(): " << *this);
	
	/*
	 * Another CREATE from the upstream peer arrived.
	 *
	 * Either the NI wants to shut down the session even before it started,
	 * or we didn't respond fast enough and the NI resends its CREATE.
	 *
	 * TODO: What if this is a new CREATE with lifetime > 0? Discard?
	 * Accept and even save policy rules?
	 */
	if ( is_anslp_create(evt) ) {
		msg_event *e = dynamic_cast<msg_event *>(evt);
		ntlp_msg *msg = e->get_ntlp_msg();
		anslp_create *create = e->get_create();

		anslp_create *previous = dynamic_cast<anslp_create *>(
			get_last_create_message()->get_anslp_create());
		assert( previous != NULL );

		try {
			check_authorization(d, e);
		}
		catch ( request_error &e ) {
			LogError(e);
			d->send_message( msg->create_error_response(e) );
			return STATE_ANSLP_PENDING; // no change
		}

		if ( create->get_msg_sequence_number() >
				previous->get_msg_sequence_number()
				&& create->get_session_lifetime() == 0 ) {

			LogDebug("terminating session.");

			state_timer.stop();

			d->send_message( msg->copy_for_forwarding() );

			return STATE_ANSLP_CLOSE;
		}
		else if ( create->get_msg_sequence_number()
				== previous->get_msg_sequence_number() ) {

			LogWarn("NI resent the initial CREATE. Passing it on.");

			d->send_message( msg->copy_for_forwarding() );

			state_timer.start(d, get_response_timeout());

			return STATE_ANSLP_PENDING; // no change
		}
		else {
			LogWarn("Replacing CREATE message."); 
									
			set_lifetime(create->get_session_lifetime());
			set_msg_sequence_number(create->get_msg_sequence_number());

			return process_state_close(d, e);
		}
	}
	
	/*
	 * Either GIST can't reach the destination or we waited long enough
	 * for the downstream peer to respond. Anyway, send a RESPONSE.
	 */
	else if ( is_no_next_node_found_event(evt)
			|| is_timer(evt, state_timer) ) {

		LogInfo("cannot reach destination");

		ntlp_msg *latest = get_last_create_message();
		ntlp_msg *resp = latest->create_response(
			information_code::sc_permanent_failure,
			information_code::fail_nr_not_reached);

		d->send_message( resp );

		// TODO: ReportAsyncEvent() ?

		return STATE_ANSLP_CLOSE;
	}
	/*
	 * Outdated timer event, discard and don't log.
	 */
	else if ( is_timer(evt) ) {
		return STATE_ANSLP_PENDING; // no change
	}

	/*
	 * A msg_event arrived which contains a A-NSLP RESPONSE message.
	 */
	else if ( is_anslp_response(evt, get_last_create_message()) ) {
		msg_event *e = dynamic_cast<msg_event *>(evt);
		ntlp_msg *msg = e->get_ntlp_msg();
		anslp_response *resp = e->get_response();

		// Discard if this is no RESPONSE to our original Create.
		anslp_create *c = get_last_create_message()->get_anslp_create();
		if ( ! resp->is_response_to(c) ) {
			LogWarn("RESPONSE doesn't match CREATE, discarding");
			return STATE_ANSLP_PENDING; // no change
		}

		if ( resp->is_success() ) 
		{
			
			LogDebug("initiated session " << get_id());
			session_id = get_id().to_string();
			auction_rule * result = d->install_auction_rules(session_id, rule);

			LogDebug("After Session Installation, requested objects:" << rule->get_number_mspec_request_objects()
						<< "installed objects:" << result->get_number_mspec_response_objects());

			
			// Verify that every rule that passed the checking process could be installed.
			if (rule->get_number_mspec_request_objects() 
					== result->get_number_mspec_response_objects() )
			{
			
				// free the space allocated to the rule to be installed.
				delete(rule);
				// Assign the response as the rule installed.
				rule = result;
				
				anslp_response *response = resp->copy();
				
				// Copy the messages that comes from routers in the path.
				objectListIter_t iter;
				for (iter = result->get_response_objects()->begin(); 
						iter != result->get_response_objects()->end(); ++iter){
					response->set_mspec_object((iter->second)->copy());
				}
				
				/*
				* Wrap the Response inside an ntlp_msg and add session ID and MRI.
				*/
				
				ntlp_msg *msgRsp = new ntlp_msg(get_id(), response, msg->get_mri()->copy(), 0);
				
				LogDebug("Before sending response to previous hop " << get_id());
				LogDebug("response SourceAddress:" << msg->get_mri()->get_sourceaddress() 
									<< "DestinationAddress:" << msg->get_mri()->get_destaddress());
				
				d->send_message( msgRsp );
				state_timer.start(d, get_lifetime());
				return STATE_ANSLP_AUCTIONING;
			}
			else
			{
				set_lifetime(0);

				// Assign the response as the rule installed.
				delete(rule);
				rule = result;
				
				// Uninstall the previous rules.
				if (rule->get_number_mspec_response_objects() > 0){
					session_id = get_id().to_string();
					d->remove_auction_rules(session_id, rule);
				}
				// Create message towards nr to teardown what we have done before 
				ntlp_msg * tear_down = build_teardown_message();
				d->send_message( tear_down );

				// Send an error message to the ni.
				d->send_message( msg->create_response(
								 information_code::sc_permanent_failure, 
								 information_code::fail_internal_error));
				return STATE_ANSLP_CLOSE;
			}
		}
		else {
			LogDebug("forwarding error msg from upstream peer");

			// TODO: ReportAsyncEvent() ?
			state_timer.stop();

			d->send_message( create_msg_for_ni(msg) );

			return STATE_ANSLP_CLOSE;
		}
	}
	
	/*
	 * Unexpected event.
	 */
	else {
		LogInfo("discarding unexpected event " << *evt);
		return STATE_ANSLP_PENDING; // no change
	}

	LogDebug("Ending handle_state_pending(): " << *this);

}


/*
 * state: STATE_ANSLP_AUCTIONING
 */
nf_session::state_t nf_session::handle_state_auctioning(
		dispatcher *d, event *evt) {

	using namespace anslp::msg;
	string session_id;
  
	LogDebug("Begining handle_STATE_ANSLP_AUCTIONING(): " << *this);
  
	/*
	 * A msg_event arrived which contains a A-NSLP REFRESH message.
	 */
	if ( is_anslp_refresh(evt) ) 
	{
				
		msg_event *e = dynamic_cast<msg_event *>(evt);
		ntlp_msg *msg = e->get_ntlp_msg();
		anslp_refresh *refresh = e->get_refresh();

		uint32 lifetime = refresh->get_session_lifetime();
		uint32 msn = refresh->get_msg_sequence_number();

		/*
		 * Before proceeding check several preconditions.
		 */
		try {
			check_lifetime(lifetime, get_max_lifetime());
			check_authorization(d, e);
		}
		catch ( override_lifetime &e) {
			lifetime = get_max_lifetime();
		}
		catch ( request_error &e ) {
			LogError(e);
			d->send_message( msg->create_error_response(e) );
			return STATE_ANSLP_AUCTIONING; // no change!
		}

		if ( ! is_greater_than(msn, get_msg_sequence_number()) ) {
			LogWarn("discarding duplicate response.");
			return STATE_ANSLP_AUCTIONING; // no change
		}

		/*
		 * All preconditions have been checked.
		 */
		set_lifetime(lifetime); // could be a new lifetime!
		if ( lifetime > 0 ) {
			LogDebug("forwarder session refreshed.");

			response_timer.stop();

			set_msg_sequence_number(msn);

			// store one copy for further reference and pass one on
			set_last_refresh_message( msg->copy() );

			d->send_message( create_msg_for_nr(msg) );

			response_timer.start(d, get_response_timeout());

			return STATE_ANSLP_AUCTIONING; // no change
		}
		else {	// lifetime == 0
			LogDebug("forwarder session refreshed lifetime 0.");
			
			response_timer.stop();
			
			d->send_message( create_msg_for_nr(msg) );
						
			// store one copy for further reference and pass one on
			set_last_refresh_message( msg->copy() );

			response_timer.start(d, get_response_timeout());
						
			// Wait confirmation to close session
			return STATE_ANSLP_AUCTIONING; 

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
			// These messages are without any response. 
			// As it is implemented, we delegate the upper layer to retry to send them again.

			session_id = get_id().to_string();
			
			std::vector<msg::anslp_mspec_object *> missing_objects;
			
			auction_rule * to_post = create_auction_rule(bidding);
			
			auction_rule * result = d->auction_interaction(session_id, to_post);
			
			saveDelete(to_post);
			
			saveDelete(result);
			
			
		} else { // Not for this node, continue sending the message towards the following node.
			
			// Build the bidding message based on those objects not installed.
			d->send_message( create_msg_for_nr(msg) );
		
		}

		LogDebug("Ending state handle_state_auctioning - bidding event ");
		
		return STATE_ANSLP_AUCTIONING; // no change
			
	}

	
	/*
	 * Downstream peer didn't respond.
	 * 
	 * This is the synchronous case, because the upstream peer is still
	 * waiting for a response to its REFRESH message.
	 *
	 * Note: We can safely ignore this when we're in proxy mode.
	 */
	else if ( ( is_no_next_node_found_event(evt) || 
	            is_timer(evt, response_timer) ) ) {
		LogWarn("downstream peer did not respond");
		
		state_timer.stop();
		
		// Uninstall the previous rules.
		if (rule->get_number_mspec_response_objects() > 0){
			session_id = get_id().to_string();
			d->remove_auction_rules(session_id, rule);
		}

		// TODO: check the spec!
		ntlp_msg *response = get_last_refresh_message()->create_response(
			information_code::sc_permanent_failure,
			information_code::fail_nr_not_reached);

		d->send_message( response );

		return STATE_ANSLP_CLOSE; 
	}
	
	/*
	 * Upstream peer didn't send a refresh in time.
	 */
	else if ( is_timer(evt, state_timer) ) 
	{		
		LogWarn("session timed out");
		response_timer.stop();

		// Uninstall the previous rules.
		if (rule->get_number_mspec_response_objects() > 0){
			session_id = get_id().to_string();
			d->remove_auction_rules(session_id, rule);
		}
		// TODO: ReportAsyncEvent()

		return STATE_ANSLP_CLOSE;
	}
	/*
	 * GIST detected that one of our routes is no longer usable. This
	 * could be the route to the NI or to the NR.
	 */
	else if ( is_route_changed_bad_event(evt) ) {
		LogUnimp("route to the NI or to the NR is no longer usable");

		// Uninstall the previous rules.
		if (rule->get_number_mspec_response_objects() > 0){
			session_id = get_id().to_string();
			d->remove_auction_rules(session_id, rule);
		}

		return STATE_ANSLP_CLOSE;
	}	
	/*
	 * Outdated timer event, discard and don't log.
	 */
	else if ( is_timer(evt) ) {
		return STATE_ANSLP_AUCTIONING; // no change
	}
	/*
	 * A RESPONSE to a REFRESH arrived.
	 */
	else if ( is_anslp_response(evt, get_last_refresh_message()) ) 
	{
			
		msg_event *e = dynamic_cast<msg_event *>(evt);
		ntlp_msg *msg = e->get_ntlp_msg();
		anslp_response *response = e->get_response();
		
		// Discard if this is not a RESPONSE to our original REFRESH.
		anslp_refresh *c = get_last_refresh_message()->get_anslp_refresh();
		if ( ! response->is_response_to(c) ) 
		{			
			LogWarn("RESPONSE doesn't match REFRESH, discarding");
			return STATE_ANSLP_AUCTIONING;	// no change
		}

		
		if ( response->is_success() ) {
			
			LogDebug("upstream peer sent successful response.");
			d->send_message( create_msg_for_ni(msg) );
			if ( get_lifetime() == 0 )
			{
				state_timer.stop();	
				// Uninstall the previous rules.
				if (rule->get_number_mspec_response_objects() > 0){
					session_id = get_id().to_string();
					d->remove_auction_rules(session_id, rule);	
				}
				return STATE_ANSLP_CLOSE;
			}
			else
			{
				state_timer.start(d, get_lifetime());
				return STATE_ANSLP_AUCTIONING; // no change
			}
		}
		else {
			LogWarn("error message received.");
			state_timer.stop();
			// Uninstall the previous rules.
			if (rule->get_number_mspec_response_objects() > 0){
				session_id = get_id().to_string();
				d->remove_auction_rules(session_id, rule);
			}
			
			d->send_message( create_msg_for_ni(msg) );

			return STATE_ANSLP_CLOSE;
		}
	}
	/*
	 * Received unexpected event.
	 */
	else {
		LogInfo("discarding unexpected event " << *evt);
		return STATE_ANSLP_AUCTIONING; // no change
	}

	LogDebug("Ending handle_STATE_ANSLP_AUCTIONING(): " << *this);
	
}

/**
 * Process an event.
 *
 * This method implements the transition function of the state machine.
 */
void nf_session::process_event(dispatcher *d, event *evt) {

	LogDebug("begin process_event(): " << *this);

	switch ( get_state() ) {

		case nf_session::STATE_ANSLP_CLOSE:
			state = handle_state_close(d, evt);
			break;

		case nf_session::STATE_ANSLP_PENDING:
			state = handle_state_pending(d, evt);
			break;

		case nf_session::STATE_ANSLP_AUCTIONING:
			state = handle_state_auctioning(d, evt);
			break;

		default:
			assert( false ); // invalid state
	}

	LogDebug("end process_event(): " << *this);
}

/**
 * Create a new MRI but with a rewritten destination.
 * TODO AM: check how it works with NAT operation.
 * This is needed for NAT operation. 
 */
ntlp::mri_pathcoupled *nf_session::create_mri_with_dest(
		ntlp::mri_pathcoupled *orig_mri) const {

	LogDebug("begin create_mri_with_dest(): " << *this);

	assert( orig_mri != NULL );
	assert( orig_mri->get_downstream() == true );

	// create a new MRI with rewritten destination
	ntlp::mri_pathcoupled *new_mri = new ntlp::mri_pathcoupled(
		orig_mri->get_sourceaddress(),
		orig_mri->get_sourceprefix(),
		orig_mri->get_sourceport(),
		orig_mri->get_destaddress(),				
		orig_mri->get_destprefix(),
		orig_mri->get_destport(),
		orig_mri->get_protocol(),
		orig_mri->get_flowlabel(),
		orig_mri->get_ds_field(),
		orig_mri->get_spi(),
		orig_mri->get_downstream()
	);

	LogDebug("End create_mri_with_dest(): " << *this);

	return new_mri;
}

/**
 * Create a new MRI but with the direction flag inverted.
 */
ntlp::mri_pathcoupled *nf_session::create_mri_inverted(
		ntlp::mri_pathcoupled *orig_mri) const {

	LogDebug("begin create_mri_inverted(): " << *this);

	assert( orig_mri != NULL );
	assert( orig_mri->get_downstream() == true );

	ntlp::mri_pathcoupled *new_mri = orig_mri->copy();
	new_mri->invertDirection();

	LogDebug("end create_mri_inverted(): " << *this);

	return new_mri;
}
