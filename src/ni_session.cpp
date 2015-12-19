/// ----------------------------------------*- mode: C++; -*--
/// @file ni_session.cpp
/// The session for an NSIS Initiator.
/// ----------------------------------------------------------
/// $Id: ni_session.cpp 3165 2015-08-31 11:12:00 amarentes $
/// $HeadURL: https://./src/ni_session.cpp $
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
#include <cmath>
#include <sstream>
#include <openssl/rand.h>

#include "logfile.h"

#include "anslp_config.h"
#include "events.h"
#include "msg/anslp_msg.h"
#include "dispatcher.h"
#include "ni_session.h"
#include <iostream>


using namespace anslp;
using namespace protlib::log;
using protlib::uint32;


#define LogWarn(msg) Log(WARNING_LOG, LOG_NORMAL, "ni_session", msg)
#define LogInfo(msg) Log(INFO_LOG, LOG_NORMAL, "ni_session", msg)
#define LogDebug(msg) Log(DEBUG_LOG, LOG_NORMAL, "ni_session", msg)


/**
 * Constructor.
 *
 * Use this if the session ID is known in advance.
 */
ni_session::ni_session(const session_id &id, const anslp_config *conf)
		: session(id), state(STATE_ANSLP_CLOSE), routing_info(NULL),
		  last_create_msg(NULL), last_refresh_msg(NULL), lifetime(0),
		  refresh_interval(20), response_timeout(0), create_counter(0),
		  refresh_counter(0), max_retries(0), proxy_session(false),
		  response_timer(this), refresh_timer(this) {

	set_session_type(st_initiator);
	set_msg_sequence_number(create_random_number());

	assert( conf != NULL );
	set_response_timeout(conf->get_ni_response_timeout());
	set_max_retries(conf->get_ni_max_retries());
	set_msg_hop_count(conf->get_ni_msg_hop_count());
	
}


/**
 * Constructor for test cases.
 *
 * @param s the state to start in
 */
ni_session::ni_session(state_t s)
		: session(), state(s), routing_info(NULL),
		  last_create_msg(NULL), last_refresh_msg(NULL), lifetime(30),
		  refresh_interval(20), response_timeout(2), create_counter(0),
		  refresh_counter(0), max_retries(3), proxy_session(false),
		  response_timer(this), refresh_timer(this) {

	set_session_type(st_initiator);
	set_msg_sequence_number(create_random_number());

	// for testing, we create an empty MRI
	routing_info = new ntlp::mri_pathcoupled();
}


/**
 * Destructor.
 */
ni_session::~ni_session() 
{
	LogDebug("Starting destroy ni_session");
	
	if (routing_info != NULL){
		delete routing_info;
	}
	
	if (last_create_msg != NULL){
		delete last_create_msg;
	}
	
	if (last_refresh_msg != NULL){
		delete last_refresh_msg;
	}
	
	LogDebug("Ending destroy ni_session");
}


/**
 * Generate a 32 Bit random number.
 */
uint32 ni_session::create_random_number() const 
{
	unsigned value;
	int ret = RAND_bytes((unsigned char *) &value, sizeof(value));
	assert( ret == 1 );

	return value;
}


std::ostream &anslp::operator<<(std::ostream &out, const ni_session &s) 
{
	static const char *const names[] = { "CLOSE", "PENDING", "METERING" };

	return out << "[ni_session: id=" << s.get_id()
			   << ", state=" << names[s.get_state()] << "]";
}


/**
 * Build a ANSLP Create message based the api create event.
 *  This function makes a deep copy of the mspec_objects. So the session
 *  object can destroy them.
 *
 * This will fetch the session lifetime and also increment the MSN.
 */
msg::ntlp_msg *ni_session::build_create_message(api_create_event *e, 
   						   std::vector<msg::anslp_mspec_object *> & missing_objects) 
{
	using namespace anslp::msg;

	assert( get_mri() != NULL );

	/*
	 * Build a A-NSLP Create message.
	 */
	anslp_create *create = new anslp_create();

	if (get_lifetime() <= e->get_session_lifetime())
		create->set_session_lifetime(get_lifetime());
	else
		create->set_session_lifetime(e->get_session_lifetime());	
		
	create->set_msg_sequence_number(next_msg_sequence_number());
	create->set_selection_auctioning_entities(e->get_selection_auctioning_entities());
	create->set_message_hop_count(get_msg_hop_count());

	/*
	 * Insert missing objects to install.
	 */
	std::vector<msg::anslp_mspec_object *>::iterator it_objects;
	for ( it_objects = missing_objects.begin(); 
			it_objects != missing_objects.end(); it_objects++ )
	{
		create->set_mspec_object((*it_objects)->copy());
	}

	/*
	 * Wrap the Configure inside an ntlp_msg and add session ID and MRI.
	 */
	ntlp_msg *msg = new ntlp_msg(get_id(), create, get_mri()->copy(), 0);

	return msg;
}

/**
 * Build a A-NSLP bidding message based the api bidding event.
 *  This function makes a deep copy of the mspec_objects. So the session
 *  object can destroy them.
 *
 */
msg::ntlp_msg *ni_session::build_bidding_message(api_bidding_event *e ) 
{
	using namespace anslp::msg;

	/*
	 * Create the message routing information (MRI) which we're going to
	 * use for all messages.
	 */
	uint8 src_prefix = 32;
	uint8 dest_prefix = 32;
	uint16 flow_label = 0;
	uint16 traffic_class = 0;		// DiffServ CodePoint
	uint32 ipsec_spi = 0;			// IPsec SPI
	bool downstream = true;

	ntlp::mri *nslp_mri = get_mri()->copy();


	/*
	 * Build a A-NSLP bidding message.
	 */
	anslp_bidding *bidding = new anslp_bidding();
		
	bidding->set_msg_sequence_number(next_msg_sequence_number());

	// Set the objects to install.
	std::vector<msg::anslp_mspec_object *>::const_iterator it_objects;
	for ( it_objects = e->get_auctioning_objects().begin(); 
			it_objects != e->get_auctioning_objects().end(); it_objects++)
	{
		anslp_mspec_object *object = *it_objects;
		bidding->set_mspec_object(object->copy());
	}
	

	/*
	 * Wrap the bidding inside an ntlp_msg and add session ID and MRI.
	 */
	ntlp_msg *msg = new ntlp_msg(get_id(), bidding, nslp_mri->copy(), 0);
	
	saveDelete(nslp_mri);
	
	return msg;
}


/**
 * Build a A-NSLP Refresh message based on the session's state.
 *
 * This will fetch the session lifetime and also increment the MSN.
 */
msg::ntlp_msg *ni_session::build_refresh_message() 
{
	using namespace anslp::msg;

	assert( get_mri() != NULL );

	/*
	 * Build a A-NSLP Refresh message.
	 */
	anslp_refresh *refresh = new anslp_refresh();
	refresh->set_session_lifetime(get_lifetime());
	refresh->set_msg_sequence_number(next_msg_sequence_number());
	
	/*
	 * Wrap the Refresh inside an ntlp_msg and add session ID and MRI.
	 */
	ntlp_msg *msg = new ntlp_msg(get_id(), refresh, get_mri()->copy(), 0);

	return msg;
}


void 
ni_session::setup_session(dispatcher *d, api_create_event *e, 
			  std::vector<msg::anslp_mspec_object *> &missing_objects) 
{
	
	/*
	 * Create the message routing information (MRI) which we're going to
	 * use for all messages.
	 */
	uint8 src_prefix = 32;
	uint8 dest_prefix = 32;
	uint16 flow_label = 0;
	uint16 traffic_class = 0;		// DiffServ CodePoint
	uint32 ipsec_spi = 0;			// IPsec SPI
	bool downstream = true;

	ntlp::mri *nslp_mri = new ntlp::mri_pathcoupled(
		e->get_source_address(), src_prefix,
		e->get_source_port(),
		e->get_destination_address(), dest_prefix,
		e->get_destination_port(),
		e->get_protocol(), flow_label, traffic_class, ipsec_spi,
		downstream
	);

	set_mri(nslp_mri);

	if ( e->get_session_lifetime() != 0 )
		set_lifetime(e->get_session_lifetime());

	uint32 r = (uint32) ceil(
		get_lifetime() / ( (get_max_retries()+0.5) * 1.5 ) );
	assert( r > 0 );

	set_refresh_interval(r); 

	uint32 t = (int) ceil( ( ( (rand() % 1000) / 1000.0) + 0.5 ) * r );

	set_response_timeout( t );

	// In this node we do not install any policy.
	std::vector<msg::anslp_mspec_object *>::const_iterator it_objects;
	for ( it_objects = e->get_auctioning_objects().begin(); 
			it_objects != e->get_auctioning_objects().end(); it_objects++)
	{
		anslp_mspec_object *object = *it_objects;
		missing_objects.push_back(object->copy());
	}
	
	LogDebug("using lifetime: " << get_lifetime() << " seconds");
	LogDebug("using refresh interval: " << r << " seconds");
	LogDebug("using response timeout: " << t << " seconds");

}


/**
 * Create an auctioning rule from the given event and return it.
 *
 */
auction_rule * 
ni_session::create_auction_rule(anslp_bidding *bidding) 
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
 * state machine part
 *
 ****************************************************************************/


/*
 * state: HANDLE STATE_CLOSE
 */
ni_session::state_t ni_session::handle_state_close(dispatcher *d, event *evt) 
{
	using msg::anslp_create;
	std::vector<msg::anslp_mspec_object *> missing_objects;

	LogDebug("Initiating state handle_state_close ");

	/*
	 * API Create event received.
	 */	
	if ( is_api_create(evt) ) {
		api_create_event *e = dynamic_cast<api_create_event *>(evt);
		
		// Initialize the session based on user-provided parameters
		setup_session(d, e, missing_objects);

		// Build the new create message based on those objects not installed.
		set_last_create_message( build_create_message(e, missing_objects) );

		d->send_message( get_last_create_message()->copy() );
		
		response_timer.start(d, get_response_timeout());

		// Send the response saying that an event was processed for the
		// new session id.
		if ( e->get_return_queue() != NULL ) {
			message *m = new anslp_event_msg(get_id(), NULL);
			e->get_return_queue()->enqueue(m);
		}

		// Release the memory allocated to the missing objects vector
		std::vector<msg::anslp_mspec_object *>::iterator it_objects;
		for ( it_objects = missing_objects.begin(); 
			it_objects != missing_objects.end(); it_objects++)
		{
			delete(*it_objects);
		}
		missing_objects.clear();
		
		LogDebug("Ending state handle_state_close - New State PENDING ");
		return STATE_ANSLP_PENDING;
		
	}
	/*
	 * An invalid event has been received.
	 */
	else {
		LogInfo("discarding unexpected event " << *evt);
		return STATE_ANSLP_CLOSE;
	}
}


/*
 * state: STATE_ANSLP_PENDING
 */
ni_session::state_t ni_session::handle_state_pending(
		dispatcher *d, event *evt) {

	
	LogDebug("Initiating state pending ");
	using namespace anslp::msg;
	string session_id;

	/*
	 * A response timeout was triggered.
	 */
	if ( is_timer(evt, response_timer) ) {

		// Retry. Send the Create message again and start a new timer.
		if ( get_create_counter() < get_max_retries() ) {
			LogWarn("response timeout, restarting timer.");
			inc_create_counter();
			d->send_message( get_last_create_message()->copy() );

			response_timer.start(d, get_response_timeout());

			return STATE_ANSLP_PENDING; // no change
		}
		// Retry count exceeded, abort.
		else {
			if (rule->get_number_mspec_response_objects() > 0){
				session_id = get_id().to_string();
				d->remove_auction_rules(session_id, rule);
			}
			d->report_async_event("got no response for our Create Message");
			return STATE_ANSLP_CLOSE;
		}
	}
	/*
	 * The NTLP can't reach the destination.
	 */
	else if ( is_no_next_node_found_event(evt) ) {
		if (rule->get_number_mspec_response_objects() > 0){
			session_id = get_id().to_string();
			d->remove_auction_rules(session_id, rule);
		}
		
		LogInfo("cannot reach destination");

		return STATE_ANSLP_CLOSE;
	}
	
	/*
	 * API teardown event received.
	 */
	else if ( is_api_teardown(evt) ) {
		LogDebug("received API teardown event");

		// Send a Refresh message with a session lifetime of 0.
		set_lifetime(0);
		set_last_create_message(NULL);

		// Uninstall the previous rules.
		if (rule->get_number_mspec_response_objects() > 0){
			session_id = get_id().to_string();
			d->remove_auction_rules(session_id, rule);
		}
			
		d->send_message( build_refresh_message() );

		return STATE_ANSLP_CLOSE;
	}
	
	/*
	 * Outdated timer event, discard and don't log.
	 */
	else if ( is_timer(evt) ) {
		return STATE_ANSLP_PENDING; // no change
	}
	
	/*
	 * A msg_event arrived which contains a ANSLP RESPONSE message.
	 */
	else if ( is_anslp_response(evt, get_last_create_message()) ) {
		msg_event *e = dynamic_cast<msg_event *>(evt);
		anslp_response *resp = e->get_response();

		LogDebug("received response " << *resp);

		// Discard if this is no RESPONSE to our original CREATE.
		anslp_create *c = get_last_create_message()->get_anslp_create();
		if ( ! resp->is_response_to(c) ) {
			LogWarn("RESPONSE doesn't match CREATE, discarding");
			return STATE_ANSLP_PENDING; // no change
		}

		if ( resp->is_success() ) {
			
			LogDebug("initiated session " << get_id());
			d->report_async_event("CREATE session initiated");
			response_timer.stop();
			
			// Check whether someone in the path change the initial lifetime
			if (resp->get_session_lifetime() != get_lifetime())
			{
				
				LogDebug("Session lifetime changed in the path" << resp->get_session_lifetime());
				
				set_lifetime(resp->get_session_lifetime());
				cal_refresh_interval();
			}
			
			LogDebug("Refresh interval " << get_refresh_interval());
			refresh_timer.start(d, get_refresh_interval());
				
			set_create_counter(0);
				
			// Group response messages from respose and 
			// messages from policies locally installed
			//ntlp::mri mriData = e->get_mri();
			vector<anslp_mspec_object *> responseObjects;
				
			// Insert objects returned from other routers along the path into the rule.
			resp->get_mspec_objects(responseObjects);

			vector<anslp_mspec_object *>::iterator iter;
			for (iter = responseObjects.begin(); iter != responseObjects.end(); ++iter)
			{
				rule->set_request_object(*iter);
			}
			session_id = get_id().to_string();
			auction_rule * result = d->send_response(session_id, rule);
			
			if (rule->get_number_mspec_request_objects() 
					== result->get_number_mspec_response_objects() )
			{
				// Assign the response as the rule installed.
				delete(rule);
				rule = result;
						
				LogDebug("Ending state handle pending - New State AUCTIONING ");
				return STATE_ANSLP_AUCTIONING;
			
			} else {
				
				// Assign the response as the rule installed.
				delete(rule);
				rule = result;
				
				// Send a Refresh message with a session lifetime of 0.
				set_lifetime(0);
				set_last_create_message(NULL);

				// Uninstall the previous rules.
				if (rule->get_number_mspec_response_objects() > 0) {
					session_id = get_id().to_string();
					d->remove_auction_rules(session_id, result);
				}
			
				d->send_message( build_refresh_message() );

				return STATE_ANSLP_CLOSE;
			}
				  
		}
		else {
			d->report_async_event("cannot initiate Create session");
			return STATE_ANSLP_CLOSE;
		}
	}
	
	/*
	 * Some other, unexpected event arrived.
	 */
	else {
		LogInfo("discarding unexpected event " << *evt);
		return STATE_ANSLP_PENDING; // no change
	}
}


/*
 * state: STATE_ANSLP_AUCTIONING
 */
ni_session::state_t ni_session::handle_state_auctioning(
		dispatcher *d, event *evt) {

	using namespace anslp::msg;
	
	LogDebug("Initiating state auctioning");
	string session_id;
	
	/*
	 * A refresh timer was triggered.
	 */
	if ( is_timer(evt, refresh_timer) ) {

		LogDebug("received refresh timer");

		// Build a new REFRESH message, it stores a copy for refreshing.
		set_last_refresh_message( build_refresh_message() );

		// dispatcher will delete
		d->send_message( get_last_refresh_message()->copy() );

        // Set the refresh counter to zero
        set_refresh_counter(0);

		response_timer.start(d, get_response_timeout());

		return STATE_ANSLP_AUCTIONING; // no change

	}
	/*
	 * A response timout was triggered.
	 */
	else if ( is_timer(evt, response_timer) ) {

		LogDebug("received response timeout timer");

		// Retry. Send the refresh message again and start a new timer.
		if ( get_refresh_counter() < get_max_retries() ) {
			inc_refresh_counter();

			d->send_message( get_last_refresh_message()->copy() );

			response_timer.start(d, get_response_timeout());

			return STATE_ANSLP_AUCTIONING; // no change
		}
		// Retry count exceeded, abort.
		else {
			// Uninstall the previous rules.
			if (rule->get_number_mspec_response_objects() > 0){
				session_id = get_id().to_string();
				pthread_mutex_lock(&execute_rules_lock);
				d->remove_auction_rules(session_id, rule);
				pthread_mutex_unlock(&execute_rules_lock);
			}
			
			LogDebug("no response to our REFRESH message");
			d->report_async_event("got no response to our REFRESH message");
			return STATE_ANSLP_CLOSE;
		}
	}
	/*
	 * API teardown event received. The user wants to end the session.
	 */
	else if ( is_api_teardown(evt) ) {
		LogDebug("received API teardown event");

		// Send a CREATE message with a session lifetime of 0.
		set_lifetime(0);
		set_last_refresh_message(NULL);

		// Uninstall the previous rules.
		if (rule->get_number_mspec_response_objects() > 0){ 
			session_id = get_id().to_string();
			pthread_mutex_lock(&execute_rules_lock);
			d->remove_auction_rules(session_id, rule);
			pthread_mutex_unlock(&execute_rules_lock);
		}

		d->send_message( build_refresh_message() );
		LogDebug("Ending state metering - api teardown");
		return STATE_ANSLP_CLOSE;
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
		
		LogDebug("Ending state handle_state_auctioning - api bidding event ");
		
		return STATE_ANSLP_AUCTIONING; // no change
		
	}

	/*
	 * API bidding event received. The user wants to receive an object from the auction server.
	 */
	else if ( is_anslp_bidding(evt) ) {
		LogDebug("received anslp bidding event");

		msg_event *e = dynamic_cast<msg_event *>(evt);
		ntlp_msg *msg = e->get_ntlp_msg();
		anslp_bidding *bidding = e->get_bidding();
						
		// The message is for us, so we send it to the install policy
		// These messages are without any response. 
		// As it is implemented, we delegate the upper layer to retry to send them again.

		session_id = get_id().to_string();
			
		std::vector<msg::anslp_mspec_object *> missing_objects;
			
		auction_rule * to_post = create_auction_rule(bidding);
		
		pthread_mutex_lock(&execute_rules_lock);
		auction_rule * result = d->auction_interaction(false, session_id, to_post);
		pthread_mutex_unlock(&execute_rules_lock);	
		
		saveDelete(to_post);
			
		saveDelete(result);
			
		LogDebug("Ending state handle_state_auctioning - bidding event ");
		
		return STATE_ANSLP_AUCTIONING; // no change
			
	}
	
	
	/*
	 * Outdated timer event, discard and don't log.
	 */
	else if ( is_timer(evt) ) {
		return STATE_ANSLP_AUCTIONING; // no change
	}
	/*
	 * A Anslp_response message arrived in response to our Refresh message.
	 */
	else if ( is_anslp_response(evt, get_last_refresh_message() ) ) {
		msg_event *e = dynamic_cast<msg_event *>(evt);
		anslp_response *resp = e->get_response();

		LogDebug("received RESPONSE: " << *resp);

		/*
		 * Discard if this is no RESPONSE to our last REFRESH.
		 */
		anslp_refresh *c = get_last_refresh_message()->get_anslp_refresh();
		if ( ! resp->is_response_to(c) ) {
			LogWarn("RESPONSE doesn't match REFRESH, discarding");
			return STATE_ANSLP_AUCTIONING; // no change
		}

		if ( resp->is_success() ) {
			d->report_async_event("REFRESH successful");

			response_timer.stop();
			refresh_timer.start(d, get_refresh_interval());

			set_refresh_counter(0);

			return STATE_ANSLP_AUCTIONING; // no change
		}
		else {

			// Uninstall the previous rules.
			if (rule->get_number_mspec_response_objects() > 0){
				session_id = get_id().to_string();
				pthread_mutex_lock(&execute_rules_lock);
				d->remove_auction_rules(session_id, rule);
				pthread_mutex_unlock(&execute_rules_lock);
			}

			d->report_async_event("REFRESH session died");
			return STATE_ANSLP_CLOSE;
		}

	}
	/*
	 * Some other, unexpected event arrived.
	 */
	else {
		LogInfo("discarding unexpected event " << *evt);
		return STATE_ANSLP_AUCTIONING; // no change
	}
}


/**
 * Process an event.
 *
 * This method implements the transition function of the state machine.
 */
void ni_session::process_event(dispatcher *d, event *evt) {
	
	LogDebug("begin process_event(): " << *this);
		
	switch ( get_state() ) {

		case ni_session::STATE_ANSLP_CLOSE:
			state = handle_state_close(d, evt);
			break;

		case ni_session::STATE_ANSLP_PENDING:
			state = handle_state_pending(d, evt);
			break;

		case ni_session::STATE_ANSLP_AUCTIONING:
			state = handle_state_auctioning(d, evt);
			break;

		default:
			assert( false ); // invalid state
	}

	LogDebug("end process_event(): " << *this);
}
