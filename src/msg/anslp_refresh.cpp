/// ----------------------------------------*- mode: C++; -*--
/// @file anslp_refresh.cpp
/// Implementation of the ANSLP REFRESH Message.
/// ----------------------------------------------------------
/// $Id: anslp_refresh.cpp 2896 2015-08-30 $
/// $HeadURL: https://./src/msg/anslp_refresh.cpp $
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

#include "msg/anslp_ie.h"
#include "msg/ie_object_key.h"
#include "msg/anslp_refresh.h"
#include "msg/anslp_object.h"


using namespace anslp::msg;
using namespace protlib::log;


/**
 * Constructor.
 *
 * Only basic initialization is done. No MNSLP objects exist yet. All other
 * attributes are set to default values.
 *
 * @param proxy_mode set to true, if proxy mode is enabled
 */
anslp_refresh::anslp_refresh()
		: anslp_msg(MSG_TYPE) 
{

	// nothing to do
}


/**
 * Copy constructor.
 *
 * Makes a deep copy of the object passed as an argument.
 *
 * @param other the object to copy
 */
anslp_refresh::anslp_refresh(const anslp_refresh &other)
		: anslp_msg(other) 
{
	
	// nothing else to do
}


/**
 * Destructor.
 *
 * Deletes all objects this message contains.
 */
anslp_refresh::~anslp_refresh() 
{
	// Nothing to do, parent class handles this.
}


anslp_refresh *anslp_refresh::new_instance() const 
{
	anslp_refresh *inst = NULL;
	catch_bad_alloc(inst = new anslp_refresh());
	return inst;
}


anslp_refresh *anslp_refresh::copy() const 
{
	anslp_refresh *copy = NULL;
	catch_bad_alloc(copy = new anslp_refresh(*this));
	return copy;
}


void anslp_refresh::serialize(NetMsg &msg, coding_t coding,
		uint32 &bytes_written) const throw (IEError) 
{

	
	const IE *obj;
	uint32 obj_bytes_written;
	uint32 start_pos = msg.get_pos();
	/* 
	 * Write the header.
	 */
	anslp_msg::serialize(msg, coding, bytes_written);
	
	/*
	 * Write the body: Serialize each object.
	 */
	ie_object_key key_msn(msg_sequence_number::OBJECT_TYPE, 1);
	bytes_written += serialize_object(key_msn, msg, coding);
	ie_object_key key_slf(session_lifetime::OBJECT_TYPE, 1);
	bytes_written += serialize_object(key_slf, msg, coding);
	
	// this would be an implementation error
	if ( bytes_written != msg.get_pos() - start_pos )
		Log(ERROR_LOG, LOG_CRIT, "anslp_msg",
				"serialize(): byte count mismatch");
	
}

uint32 
anslp_refresh::serialize_object(ie_object_key &key, NetMsg &msg, coding_t coding) const
{
	uint32 obj_bytes_written = 0;
	anslp_object *obj = get_object(key);
	if (obj != NULL){
		obj->serialize(msg, coding, obj_bytes_written);
		return obj_bytes_written;
	}
	else{
		// Throwns an exception.
	}
}



bool anslp_refresh::check() const 
{
	
	anslp_object *obj;
	// Error: no objects available
	if ( get_num_objects() != 2 )
		return false;

	// Verifies that message sequence number exists.
	ie_object_key key_msn(msg_sequence_number::OBJECT_TYPE, 1);
	obj = get_object(key_msn);
	if (obj == NULL)
		return false;

	// Verifies that session lifetime exists.
	ie_object_key key_slt(session_lifetime::OBJECT_TYPE, 1);
	obj= get_object(key_slt);
	if (obj == NULL)
		return false;

	return true; // no error found
}

void anslp_refresh::register_ie(IEManager *iem) const 
{
	iem->register_ie(cat_anslp_msg, get_msg_type(), 0, this);
}


/**
 * Set the desired session lifetime.
 *
 * @param seconds the session lifetime in milliseconds
 */
void anslp_refresh::set_session_lifetime(uint32 milliseconds) 
{
	set_object(new session_lifetime(milliseconds, anslp_object::tr_mandatory, true));
}


/**
 * Return the desired session lifetime.
 *
 * @return the session lifetime in milliseconds
 */
uint32 anslp_refresh::get_session_lifetime() const 
{
	
	ie_object_key key(session_lifetime::OBJECT_TYPE,1);
	session_lifetime *lt = dynamic_cast<session_lifetime *>(
		get_object(key));

	if ( lt == NULL )
		return 0; // TODO: use a default constant here?
	else
		return lt->get_value();
}




/**
 * Sets the message sequence number.
 *
 * See the msg_sequence_number object for more information.
 *
 * @param msn the value of the message sequence number
 */
void anslp_refresh::set_msg_sequence_number(uint32 msn) 
{
	set_object(new msg_sequence_number(msn, anslp_object::tr_mandatory, true));
}

/**
 * Return the desired message sequence number.
 *
 * @return the message sequence number
 */
uint32 anslp_refresh::get_msg_sequence_number() const 
{
	
	ie_object_key key(msg_sequence_number::OBJECT_TYPE,1);
	msg_sequence_number *msn = dynamic_cast<msg_sequence_number *>(
		get_object(key));

	if ( msn == NULL )
		return 0; // TODO: use a default constant here?
	else
		return msn->get_value();
}
