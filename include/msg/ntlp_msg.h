/*!
 * An NTLP Message.
 *
 * $Id: ntlp_msg.h 2438 2014-11-05 14:10:00  $
 * $HeadURL: https://./include/msg/ntlp_msg.h $
 */
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
#ifndef NTLP_MSG_H
#define NTLP_MSG_H

#include "mri.h"	// from NTLP

#include "anslp_msg.h"
#include "session_id.h"
#include "session.h"
#include "anslp_response.h"



namespace anslp {
  namespace msg {
     using namespace protlib;


/**
 * An NTLP Message.
 *
 * This class encapsulates all the data required for sending a message via
 * the NTLP. It consists of data for the NTLP header, such as addresses,
 * ports, and protocol IDs, and of a ANSLP message. The ANSLP message
 * represents the NTLP body.
 *
 * Note: We don't inherit from IE because this message will not be serialized
 * or deserialized (only the ANSLP message will).
 */
class ntlp_msg {

  public:
	ntlp_msg(const session_id &id, anslp_msg *body,
			ntlp::mri *m, uint32 sii_handle);
	virtual ~ntlp_msg();

	inline session_id get_session_id() const { return sid; }
	inline anslp_msg *get_anslp_msg() const { return msg; }
	inline ntlp::mri *get_mri() const { return routing_info; }
	inline uint32 get_sii_handle() const { return sii_handle; }


	ntlp_msg *copy() const;
	ntlp_msg *copy_for_forwarding(ntlp::mri *new_mri=NULL) const;

	ntlp_msg *create_response(uint8 severity, uint8 response_code) const;
	ntlp_msg *create_error_response(const request_error &e) const;
	ntlp_msg *create_success_response(uint32 session_lifetime) const;

	inline anslp_create *get_anslp_create() const;
	inline anslp_refresh *get_anslp_refresh() const;
	inline anslp_notify *get_anslp_notify() const;
	inline anslp_response *get_anslp_response() const;

  private:
	ntlp_msg(const anslp_msg &other);

	session_id sid;

	ntlp::mri *routing_info;
	uint32 sii_handle;

	// This will be deserialized and used as the NTLP body.
	anslp_msg *msg;
};

std::ostream &operator<<(std::ostream &out, const ntlp_msg &s);

bool is_greater_than(uint32 s1, uint32 s2);


inline anslp_create *ntlp_msg::get_anslp_create() const {
	return dynamic_cast<anslp_create *>(get_anslp_msg());
}

inline anslp_notify *ntlp_msg::get_anslp_notify() const {
	return dynamic_cast<anslp_notify *>(get_anslp_msg());
}

inline anslp_refresh *ntlp_msg::get_anslp_refresh() const {
	return dynamic_cast<anslp_refresh *>(get_anslp_msg());
}


inline anslp_response *ntlp_msg::get_anslp_response() const {
	return dynamic_cast<anslp_response *>(get_anslp_msg());
}

  } // namespace msg
} // namespace anslp

#endif // NTLP_MSG_H
