/// ----------------------------------------*- mode: C++; -*--
/// @file gistka_mapper.h
/// Utilities for mapping between GISTka/protlib and ASNLP. 
/// ----------------------------------------------------------
/// $Id: gistka_mapper.h 2558 2015-08-31 12:07:00 amarentes $
/// $HeadURL: https://./include/gistka_mapper.h $
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
#ifndef ANSLP_GISTKA_MAPPER_H
#define ANSLP_GISTKA_MAPPER_H

#include "timer_module.h"

#include "apimessage.h"		// from NTLP

#include "events.h"
#include "anslp_timers.h"
#include "msg/ntlp_msg.h"


namespace anslp {

/**
 * Utilities for mapping between GISTka/protlib and ANSLP. 
 *
 * This class maps GISTka's APIMsg objects to ASNLP events, creates APIMsg
 * objects from a given ntlp_msg etc.
 *
 * It provides functionality used in the dispatcher but has been factored out
 * to keep the dispatcher small and clean.
 */
class gistka_mapper {
	
  public:
	event *map_to_event(const protlib::message *msg) const;

	ntlp::APIMsg *create_api_msg(msg::ntlp_msg *msg) const throw ();

	ntlp::sessionid *create_ntlp_session_id(const session_id &sid) const;
	session_id *create_anslp_session_id(ntlp::sessionid *sid) const;

  private:
	event *map_api_message(const ntlp::APIMsg *msg) const;
	event *map_api_receive_message(const ntlp::APIMsg *msg) const;
	event *map_api_network_notification(const ntlp::APIMsg *msg) const;
	event *map_api_message_status(const ntlp::APIMsg *msg) const;
	event *map_timer_message(const anslp_timer_msg *msg) const;
};


} // namespace anslp

#endif // ANSLP_GISTKA_MAPPER_H
