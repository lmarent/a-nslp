/// ----------------------------------------*- mode: C++; -*--
/// @file dispatcher.h
/// The dispatcher receives events and calls handlers.
/// ----------------------------------------------------------
/// $Id: dispatcher.h 2558 2015-08-31 10:16:00 amarentes $
/// $HeadURL: https://./include/dispatcher.h $
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
#ifndef ANSLP_DISPATCHER_H
#define ANSLP_DISPATCHER_H

#include "timer_module.h"

#include "apimessage.h"		// from NTLP

#include "anslp_config.h"
#include "session_manager.h"
#include "auction_rule_installer.h"
#include "events.h"
#include "msg/ntlp_msg.h"
#include "gistka_mapper.h"


namespace anslp {

/**
 * Receive events and call handlers.
 *
 * The dispatcher is an instance that receives events, examines them, and
 * then executes a handler. In other words, it is the top-level state machine.
 *
 * Even though not all methods are declared as 'const', the dispatcher itself
 * is constant and stateless. All state is kept in the session manager, A-NSLP
 * manager classes etc., which are shared among dispatcher instances.
 * Because of this, the dispatcher doesn't have to be thread-safe, it is enough
 * that the used components (session_manager etc.) are thread-safe.
 *
 * This class contains a rich interface which offers all functionality the
 * lower-level state machines (implemented in the session classes) require.
 * The services include sending messages, starting times, installing policy
 * rules etc.
 */
class dispatcher {
	
  public:
	dispatcher(session_manager *m, 
			   auction_rule_installer *p, 
			   anslp_config *conf);
			
	virtual ~dispatcher();

	virtual void process(event *evt) throw ();

	/*
	 * Services which are used by the event handlers.
	 */
	virtual void send_message(msg::ntlp_msg *msg) throw ();
	
	virtual id_t start_timer(const session *s, int secs) throw ();
	
	virtual void report_async_event(std::string msg) throw ();
	
	virtual bool check(const string session_id, const msg::anslp_mspec_object *object);
	
	virtual auction_rule * install_auction_rules(const string session_id, const auction_rule *act_rule) 
		throw (auction_rule_installer_error);
	
	virtual auction_rule * auction_interaction(const string session_id, const auction_rule *act_rule)
		throw (auction_rule_installer_error);
		
	virtual auction_rule * send_response(const string session_id, const auction_rule *act_rule) 
		throw (auction_rule_installer_error);
		
	virtual void remove_auction_rules(const string session_id, const auction_rule *act_rule)
		throw (auction_rule_installer_error);

	virtual bool is_authorized(const msg_event *evt) const throw ();


  private:
	/*
	 * The targets of these four pointers are shared among dispatchers.
	 * They may not be deleted by the destructor!
	 */
	session_manager *session_mgr;
	auction_rule_installer *rule_installer;
	anslp_config *config;

	gistka_mapper mapper;

	session *create_session(event *evt) const throw ();
	
	void send_receive_answer(const routing_state_check_event *evt) const;
};


} // namespace anslp

#endif // ANSLP_DISPATCHER_H
