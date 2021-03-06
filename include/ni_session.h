/// ----------------------------------------*- mode: C++; -*--
/// @file ni_session.h
/// ni_session class.
/// ----------------------------------------------------------
/// $Id: ni_session.h 2558 2015-08-31 13:51:00 amarentes $
/// $HeadURL: https://./include/ni_session.h $
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
#ifndef ANSLP_NI_SESSION_H
#define ANSLP_NI_SESSION_H

#include "protlib_types.h"

#include "anslp_config.h"
#include "session.h"
#include "events.h"
#include "anslp_timers.h"
#include "msg/ntlp_msg.h"
#include <stdexcept>



namespace anslp 
{
    using protlib::uint32;
    using protlib::hostaddress;


class event;
class api_configure_event;

/**
 * A session for an initiator.
 *
 * An object of this class holds all the data for a ANSLP signaling session
 * and also implements the NI state machine.
 *
 * There are two different attributes for proxy operation: proxy_mode and
 * proxy_session. If proxy_mode is set, then all outgoing CREATE messages
 * will contain a nonce and the P-flag is set. If proxy_session is set, then
 * this session has been created by another session and acts as a proxy for
 * the initiator which is non ANSLP-NSLP aware.
 *
 * Note that the getters and setters are declared protected to make them
 * accessible from a subclass. This is required for the test suite.
 */
class ni_session : public session {
	
  public:
  
	ni_session(const session_id &id, const anslp_config *conf);
	
	~ni_session();

	bool is_final() const; // inherited from session

  protected:
	/**
	 * States of a session.
	 */
	enum state_t {
		STATE_ANSLP_CLOSE	= 0,
		STATE_ANSLP_PENDING	= 1,
		STATE_ANSLP_PENDING_INSTALLING = 2,
		STATE_ANSLP_AUCTIONING	= 3,
		STATE_ANSLP_PENDING_TEARDOWN = 4
	};

	ni_session(state_t s=STATE_ANSLP_CLOSE);

	void process_event(dispatcher *d, event *evt);
	
	inline state_t get_state() const { return state; }

	void set_last_create_message(msg::ntlp_msg *msg);
	msg::ntlp_msg *get_last_create_message() const;
	
	void set_last_refresh_message(msg::ntlp_msg *msg);
	msg::ntlp_msg *get_last_refresh_message() const;
	
	void set_last_auction_install_rule(auction_rule *act); 
	auction_rule * get_last_auction_install_rule() const;
	
	inline void set_create_counter(uint32 num) { create_counter = num; }
	inline uint32 get_create_counter() const { return create_counter; }

	inline void set_refresh_counter(uint32 num) { refresh_counter = num; }
	inline uint32 get_refresh_counter() const { return refresh_counter; }

	inline void set_teardown_counter(uint32 num) { teardown_counter = num; }
	inline uint32 get_teardown_counter() const { return teardown_counter; }

	inline uint32 get_refresh_interval() const { return refresh_interval; }
	inline void set_refresh_interval(uint32 sec) { refresh_interval = sec; }
	inline void cal_refresh_interval() { refresh_interval = (uint32) lifetime * 2/3; }

	inline uint32 get_lifetime() const { return lifetime; }
	inline void set_lifetime(uint32 seconds) { lifetime = seconds; }

	inline ntlp::mri *get_mri() const { return routing_info; }
	void set_mri(ntlp::mri *m);

	inline bool is_proxy_mode() const { return proxy_mode; }
	
	inline void set_proxy_mode(bool value) { proxy_mode = value; }

	inline bool is_proxy_session() const { return proxy_session; }

	inline uint32 get_response_timeout() const { return response_timeout; }
	inline void set_response_timeout(uint32 t) { response_timeout = t; }

	inline uint32 get_max_retries() const { return max_retries; }
	inline void set_max_retries(uint32 m) { max_retries = m; }

	inline timer &get_response_timer() { return response_timer; }
	inline timer &get_refresh_timer() { return refresh_timer; }

	auction_rule *build_auction_install_rule(anslp_response *resp);


  private:
	state_t state;

	ntlp::mri *routing_info;

	/*
	 * The latest CREATE message we sent. We keep it because we need it for
	 * retransmission and to check if a received RESPONSE matches the
	 * CREATE sent earlier.
	 */
	msg::ntlp_msg *last_create_msg;

	/*
	 * The latest REFRESH message we sent. We keep it because we need it for
	 * retransmission and to check if a received RESPONSE matches the
	 * REFRESH sent earlier.
	 */
	msg::ntlp_msg *last_refresh_msg;


	/*
	 * The AUCTION RULE to install. We keep it because we need it for
	 * retransmission and to check if a received RESPONSE matches the
	 * RULE sent earlier.
	 */
	auction_rule *last_auction_install_rule;
	
	
	bool proxy_mode;
	uint32 lifetime;
	uint32 refresh_interval;
	uint32 response_timeout;
	uint32 create_counter; 
	uint32 refresh_counter; 
	uint32 max_retries;
	uint32 teardown_counter; 

	bool proxy_session;

	/*
	 * For an NI session, we have two types of timers: RESPONSE and REFRESH.
	 * At any time, there may be only one timer active per type.
	 */
	timer response_timer;
	timer refresh_timer;

	/*
	 * State machine methods:
	 */
	state_t handle_state_close(dispatcher *d, event *evt);
	state_t handle_state_pending(dispatcher *d, event *evt);
	state_t handle_state_pending_installing(dispatcher *d, event *evt);
	state_t handle_state_auctioning(dispatcher *d, event *evt);
	state_t handle_state_pending_teardown(dispatcher *d, event *evt);

	/*
	 * Utility methods:
	 */
	void setup_session(dispatcher *d, 
					   api_create_event *evt,
					   std::vector<msg::anslp_mspec_object *> &missing_objects);
												
	msg::ntlp_msg *build_create_message(api_create_event *evt, 
								  	    std::vector<msg::anslp_mspec_object *> & missing_objects);

	msg::ntlp_msg *build_bidding_message(api_bidding_event *evt);
										   
	msg::ntlp_msg *build_refresh_message(); 
		
	uint32 create_random_number() const;
	void inc_create_counter();
	void inc_refresh_counter();
	void inc_teardown_counter();

	auction_rule * create_auction_rule(anslp_bidding *bidding);

	friend std::ostream &operator<<(std::ostream &out, const ni_session &s);
	friend class session_manager;
};

std::ostream &operator<<(std::ostream &out, const ni_session &s);

inline bool ni_session::is_final() const 
{
	return get_state() == STATE_ANSLP_CLOSE;
}

inline void ni_session::inc_create_counter() 
{
	create_counter++;
}

inline void ni_session::inc_refresh_counter() 
{
	refresh_counter++;
}

inline void ni_session::inc_teardown_counter() 
{
	teardown_counter++;
}

inline msg::ntlp_msg *ni_session::get_last_create_message() const 
{
	assert( last_create_msg != NULL );
	return last_create_msg;
}

inline void ni_session::set_last_create_message(msg::ntlp_msg *msg) 
{
	delete last_create_msg;
	last_create_msg = msg;
}

inline void ni_session::set_last_refresh_message(msg::ntlp_msg *msg) 
{
	delete last_refresh_msg;
	last_refresh_msg = msg;
}

inline void ni_session::set_last_auction_install_rule(auction_rule *act) 
{ 
	last_auction_install_rule = act; 
}

inline auction_rule * 
ni_session::get_last_auction_install_rule() const 
{ 

	return last_auction_install_rule; 

} 

inline void ni_session::set_mri(ntlp::mri *m) 
{
	delete routing_info;
	routing_info = m;
}


} // namespace anlsp

#endif // ANSLP_NI_SESSION_H
