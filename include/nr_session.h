/// ----------------------------------------*- mode: C++; -*--
/// @file nr_session.h
/// The nr_session class.
/// ----------------------------------------------------------
/// $Id: nr_session.h 2558 2014-11-08 10:46:00 amarentes $
/// $HeadURL: https://./include/nr_session.h $
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
#ifndef ANSLP_NR_SESSION_H
#define ANSLP_NR_SESSION_H

#include "protlib_types.h"

#include "session.h"
#include "events.h"
#include "auction_rule.h"
#include "msg/ntlp_msg.h"


namespace anslp 
{
    using protlib::uint8;
    using protlib::uint32;
    using protlib::hostaddress;

class event;
class msg_event;

/**
 * A session for a responder.
 */
class nr_session : public session {
	
  public:
  
	nr_session(const session_id &id, anslp_config *conf);
	
	~nr_session();

	bool is_final() const; // inherited from session
	
	uint32 get_msg_bidding_sequence_number() const { return msn_bidding; }		
	
	void set_msg_bidding_sequence_number(uint32 value) { msn_bidding = value; }
	
	uint32 next_msg_bidding_sequence_number();

  protected:
	/**
	 * States of a session.
	 */
	enum state_t {
		STATE_ANSLP_CLOSE				= 0,
		STATE_ANSLP_PENDING				= 1,
		STATE_ANSLP_PENDING_INSTALLING	= 2,
		STATE_ANSLP_AUCTIONING			= 3,
		STATE_ANSLP_PENDING_TEARDOWN    = 4
	};

	void process_event(dispatcher *d, event *evt);
	
	state_t get_state() const;

	nr_session(state_t s=STATE_ANSLP_CLOSE, anslp_config *conf=NULL, uint32 msn=0);
	
	inline timer &get_state_timer() { return state_timer; }
	
	auction_rule *get_auction_rule() const;

	void set_auction_rule(auction_rule *r);	

	void set_last_create_message(msg::ntlp_msg *msg);
	
	msg::ntlp_msg * get_last_create_message() const;

	inline void set_create_counter(uint32 num) { create_counter = num; }
	inline uint32 get_create_counter() const { return create_counter; }

	inline uint32 get_max_retries() const { return max_retries; }
	inline void set_max_retries(uint32 m) { max_retries = m; }

	inline ntlp::mri *get_mri() const { return routing_info; }
	void set_mri(ntlp::mri *m);


  private:
  
	state_t state;

	ntlp::mri *routing_info;

	anslp_config *config;		// shared among instances, don't delete!

	uint32 msn_bidding;
	uint32 lifetime;
	uint32 max_lifetime;
	uint32 response_timeout;
	uint32 create_counter;
	uint32 max_retries;

	timer state_timer;
	timer response_timer;  // timer for servers' response.
	
	auction_rule *act_rule;

	// Original create event.
	msg::ntlp_msg *create_message;


	/*
	 * State machine methods:
	 */
	state_t handle_state_close(dispatcher *d, event *evt);
	state_t handle_state_pending(dispatcher *d, event *evt);
	state_t handle_state_pending_installing(dispatcher *d, event *evt);
	state_t handle_state_auctioning(dispatcher *d, event *e);	
	state_t handle_state_pending_teardown(dispatcher *d, event *evt);
	

	msg::ntlp_msg *build_trace_response(ntlp_msg *msg) const;


	/*
	 * Utility methods:
	 */
	uint32 get_lifetime() const { return lifetime; }
	void set_lifetime(uint32 seconds) { lifetime = seconds; }

	inline uint32 get_max_lifetime() const { return max_lifetime; }
	inline void set_max_lifetime(uint32 t) { max_lifetime = t; }

	inline uint32 get_response_timeout() const { return response_timeout; }
	
	inline void set_response_timeout(uint32 t) { response_timeout = t; }


	friend std::ostream &operator<<(std::ostream &out, const nr_session &s);

	msg::ntlp_msg *build_bidding_message(api_bidding_event *evt);
	
	uint32 create_random_number() const;
	
	void inc_create_counter();
	
	/*
	 * Auctioning methods:
	 */
	bool save_auction_rule(dispatcher *d, 
   							anslp_create *create,
							 std::vector<msg::anslp_mspec_object *> &missing_objects )
		 throw (request_error);
	
	auction_rule *get_auction_rule_copy() const;
	
	auction_rule * create_auction_rule(anslp_bidding *bidding);
};

std::ostream &operator<<(std::ostream &out, const nr_session &s);


inline nr_session::state_t nr_session::get_state() const 
{
	return state;
}

inline bool nr_session::is_final() const 
{
	return get_state() == STATE_ANSLP_CLOSE;
}


inline void nr_session::set_auction_rule(auction_rule *r) 
{
	delete act_rule;
	act_rule = r;
}

inline auction_rule *nr_session::get_auction_rule() const 
{
	return act_rule; // may return NULL!
}

inline void nr_session::set_last_create_message(msg::ntlp_msg *msg) {
	
	if (create_message != NULL){
		delete(create_message);
	}
	
	delete(create_message);
	create_message = msg;

}

inline msg::ntlp_msg *nr_session::get_last_create_message() const {
	assert( create_message != NULL );
	return create_message;
}

inline void nr_session::set_mri(ntlp::mri *m) 
{
	delete routing_info;
	routing_info = m;
}

inline void nr_session::inc_create_counter() 
{
	create_counter++;
}


} // namespace anslp

#endif // ANSLP_NR_SESSION_H
 
