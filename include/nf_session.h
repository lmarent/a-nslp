/// ----------------------------------------*- mode: C++; -*--
/// @file nf_session.h
/// The nf_session class.
/// ----------------------------------------------------------
/// $Id: nf_session.h 2558 2015-08-31 13:47:00 amarentes $
/// $HeadURL: https://./include/nf_session.h $
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
#ifndef ANSLP_NF_SESSION_H
#define ANSLP_NF_SESSION_H

#include "protlib_types.h"

#include "session.h"
#include "events.h"
#include "auction_rule.h"
#include "msg/anslp_msg.h"


namespace anslp {
    using protlib::uint32;
    using protlib::hostaddress;

class event;
class msg_event;

/**
 * A session for a forwarder.
 *
 * This does *not* include the EXT part.
 */
class nf_session : public session {
	
  public:
  
	nf_session(const session_id &id, const anslp_config *conf);
	
	~nf_session();

	bool is_final() const; // inherited from session

  protected:
	/**
	 * States of a session.
	 */
	enum state_t {
		STATE_ANSLP_CLOSE				= 0,
		STATE_ANSLP_PENDING_CHECK		= 1,
		STATE_ANSLP_PENDING				= 2,
		STATE_ANSLP_PENDING_INSTALLING	= 3,
		STATE_ANSLP_AUCTIONING			= 4,
		STATE_ANSLP_PENDING_TEARDOWN    = 5
	};

	nf_session(state_t s, const anslp_config *conf);

	void process_event(dispatcher *d, event *evt);
	
	state_t get_state() const { return state; }

	uint32 get_lifetime() const { return lifetime; }
	
	void set_lifetime(uint32 seconds) { lifetime = seconds; }

	inline timer &get_state_timer() { return state_timer; }
	
	inline timer &get_response_timer() { return response_timer; }

	inline uint32 get_max_lifetime() const { return max_lifetime; }
	
	inline void set_max_lifetime(uint32 t) { max_lifetime = t; }

	inline uint32 get_response_timeout() const { return response_timeout; }
	
	inline void set_response_timeout(uint32 t) { response_timeout = t; }

	msg::ntlp_msg *get_last_create_message() const;
	
	void set_last_create_message(msg::ntlp_msg *msg);
	
	msg::ntlp_msg *get_last_refresh_message() const;
	
	void set_last_refresh_message(msg::ntlp_msg *msg);

	void set_last_response_message(msg_event *e);
	
	msg_event * get_last_response_message() const;

	inline bool is_proxy_mode() const { return proxy_mode; }
	
	inline void set_proxy_mode(bool value) { proxy_mode = value; }
	
	ntlp::mri_pathcoupled *get_ni_mri() const;
	void set_ni_mri(ntlp::mri_pathcoupled *m);

	ntlp::mri_pathcoupled *get_nr_mri() const;
	void set_nr_mri(ntlp::mri_pathcoupled *m);

	uint32 get_msg_bidding_sequence_number() const { return msn_bidding; }		
	void set_msg_bidding_sequence_number(uint32 value) { msn_bidding = value; }
	
	uint32 next_msg_bidding_sequence_number();

	bool set_auction_rule(dispatcher *d, 
						  anslp_create *create,
						  std::vector<msg::anslp_mspec_object *> &missing_objects);

		
  private:
  
	state_t state;

	const anslp_config *config;	// shared among sessions, don't delete!

	bool proxy_mode;
	
	uint32 msn_bidding;
	
	uint32 lifetime;
	
	uint32 max_lifetime;	
	uint32 response_timeout;
	uint32 create_counter; 
	
	timer state_timer;
	timer response_timer;

	ntlp::mri_pathcoupled *ni_mri;	// the MRI to use for reaching the NI
	ntlp::mri_pathcoupled *nr_mri;	// the MRI to use for reaching the NR
	msg::ntlp_msg *create_message;
	msg::ntlp_msg *refresh_message;
	
	msg_event *response_message;


	state_t process_state_close(dispatcher *d, event *evt);
	state_t handle_state_close(dispatcher *d, event *evt);
	state_t handle_state_pending_check(dispatcher *d, event *evt);
	state_t handle_state_pending(dispatcher *d, event *evt);
	state_t handle_state_pending_installing(dispatcher *d, event *evt);
	state_t handle_state_pending_teardown(dispatcher *d, event *evt);
	
	state_t handle_state_auctioning(dispatcher *d, event *evt);
	
	ntlp::mri_pathcoupled *create_mri_inverted(
		ntlp::mri_pathcoupled *orig_mri) const;
		
	ntlp::mri_pathcoupled *create_mri_with_dest(
		ntlp::mri_pathcoupled *orig_mri) const;
		
	ntlp_msg *create_msg_for_nr(ntlp_msg *msg) const;
	
	ntlp_msg *create_msg_for_ni(ntlp_msg *msg) const;
	
	msg::ntlp_msg *
	build_create_message(anslp_create *c,
						 std::vector<msg::anslp_mspec_object *> & missing_objects);
							
	msg::ntlp_msg * build_teardown_message(); 

	void set_pc_mri(msg_event *evt) throw (request_error);
	
	auction_rule * create_auction_rule(anslp_bidding *bidding);

	uint32 create_random_number() const;
	
	void inc_create_counter();
	
	msg::ntlp_msg *build_bidding_message(api_bidding_event *e );
	
	friend std::ostream &operator<<(std::ostream &out, const nf_session &s);
};

std::ostream &operator<<(std::ostream &out, const nf_session &s);


inline bool nf_session::is_final() const {
	return get_state() == STATE_ANSLP_CLOSE;
}

inline void nf_session::set_last_create_message(msg::ntlp_msg *msg) {
	delete(create_message);
	create_message = msg;
}

inline msg::ntlp_msg *nf_session::get_last_create_message() const {
	assert( create_message != NULL );
	return create_message;
}

inline void nf_session::set_last_refresh_message(msg::ntlp_msg *msg) {
	delete(refresh_message);
	refresh_message = msg;
}

inline msg::ntlp_msg *nf_session::get_last_refresh_message() const {
	assert( refresh_message != NULL );
	return refresh_message;
}

inline void nf_session::set_last_response_message(msg_event *e) 
{
	if (response_message != NULL)
		delete(response_message);
	
	response_message = new msg_event(new anslp::session_id(*e->get_session_id()), 
									 e->get_ntlp_msg()->copy(), 
									 e->is_for_this_node());
}

inline msg_event *nf_session::get_last_response_message() const {
	assert( response_message != NULL );
	return response_message;
}

inline ntlp::mri_pathcoupled *nf_session::get_ni_mri() const {
	assert( ni_mri != NULL );
	return ni_mri;
}

inline void nf_session::set_ni_mri(ntlp::mri_pathcoupled *mri) {
	delete ni_mri;
	ni_mri = mri;
}

inline ntlp::mri_pathcoupled *nf_session::get_nr_mri() const {
	assert( nr_mri != NULL );
	return nr_mri;
}

inline void nf_session::set_nr_mri(ntlp::mri_pathcoupled *mri) {
	delete nr_mri;
	nr_mri = mri;
}

inline void nf_session::inc_create_counter() 
{
	create_counter++;
}


} // namespace anslp

#endif // ANSLP_NF_SESSION_H
