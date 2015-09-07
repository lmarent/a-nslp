/*!
 * The Session Lifetime Object.
 *
 * $Id: session_lifetime.h 2895 2015-08-30 13:32:00Z amarentes $
 * $HeadURL: https://./include/msg/session_lifetime.h $
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
#ifndef ANSLP_MSG__SESSION_LIFETIME_H
#define ANSLP_MSG__SESSION_LIFETIME_H

#include "anslp_object.h"


namespace anslp {
 namespace msg {

    using namespace protlib;


/**
 * \class session_lifetime
 *
 * \brief The Session Lifetime Object.
 * 
 * RFC definition: This object carries the requested lifetime for a A-NSLP session in a
 * 				   CREATE / REFRESH message or the granted session lifetime in a
 * 				   RESPONSE message, in milliseconds. When a A-NSLP session expires,
 *   			   the Metering Manager MUST configure the Monitoring Probe to stop the
 * 				   Metering.
 * 
 * \author Andres Marentes
 *
 * \version 0.1 
 *
 * \date 2015/08/30 13:33:00
 *
 * Contact: la.marentes455@uniandes.edu.co
 *  
 */
class session_lifetime : public anslp_object {

  public:
	static const uint16 OBJECT_TYPE = 0x00F8;

	explicit session_lifetime();
	
	explicit session_lifetime(uint32 slt, treatment_t t = tr_mandatory, bool _unique = true);

	virtual ~session_lifetime();

	virtual session_lifetime *new_instance() const;
	
	virtual session_lifetime *copy() const;

	virtual size_t get_serialized_size(coding_t coding) const;
	
	virtual bool check_body() const;
	
	virtual bool equals_body(const anslp_object &other) const;
	
	virtual const char *get_ie_name() const;
	
	virtual ostream &print_attributes(ostream &os) const;

	virtual bool deserialize_body(NetMsg &msg, uint16 body_length,
			IEErrorList &err, bool skip);

	virtual void serialize_body(NetMsg &msg) const;

	/**
	 * Get the session lifetime value
	 */ 
	uint32 get_value() const;

	/**
	 * Set the session lifetime value
	 */ 
	void set_value(uint32 _slt);

	/**
	 * Assignment operator
	 */
	session_lifetime &operator=(const session_lifetime &other);

  private:

	static const char *const ie_name;

	uint32 slt;
};


 } // namespace msg
} // namespace anslp

#endif // ANSLP_MSG__SESSION_LIFETIME_H
