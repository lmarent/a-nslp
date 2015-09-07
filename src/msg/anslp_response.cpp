/// ----------------------------------------*- mode: C++; -*--
/// @file anslp_response.cpp
/// Implementation of the MNSLP RESPONSE Message.
/// ----------------------------------------------------------
/// $Id: anslp_response.cpp 2896 2015-08-30 $
/// $HeadURL: https://./src/msg/anslp_response.cpp $
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
#include "msg/anslp_response.h"
#include "msg/anslp_object.h"
#include "msg/ie_object_key.h"
#include "msg/anslp_create.h"



using namespace anslp::msg;
using namespace protlib::log;


/**
 * Constructor.
 *
 * Only basic initialization is done. No MNSLP objects exist yet. All other
 * attributes are set to default values.
 *
 */
anslp_response::anslp_response() : anslp_msg(MSG_TYPE) 
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
anslp_response::anslp_response(const anslp_response &other)
		: anslp_msg(other) 
{
	
	// nothing else to do
}


/**
 * Destructor.
 *
 * Deletes all objects this message contains.
 */
anslp_response::~anslp_response() 
{
	// Nothing to do, parent class handles this.
}


anslp_response *anslp_response::new_instance() const 
{
	anslp_response *inst = NULL;
	catch_bad_alloc(inst = new anslp_response());
	return inst;
}


anslp_response *anslp_response::copy() const 
{
	anslp_response *copy = NULL;
	catch_bad_alloc(copy = new anslp_response(*this));
	return copy;
}


void anslp_response::serialize(NetMsg &msg, coding_t coding,
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
	ie_object_key key_inf(information_code::OBJECT_TYPE, 1);
	bytes_written += serialize_object(key_inf, msg, coding);
	ie_object_key key_slf(session_lifetime::OBJECT_TYPE, 1);
	bytes_written += serialize_object(key_slf, msg, coding);
	
	/*
	 * Write the body: Serialize ipfix message.
	 */
	uint32 max_seq_nbr = objects.getMaxSequence(anslp_ipap_message::OBJECT_TYPE);
	
	for ( uint32 i = 1; i <= max_seq_nbr; i++ ) 
	{
		ie_object_key key_ipfix(anslp_ipap_message::OBJECT_TYPE, i);
		bytes_written += serialize_object(key_ipfix, msg, coding);
	}
	
	// this would be an implementation error
	if ( bytes_written != msg.get_pos() - start_pos )
		Log(ERROR_LOG, LOG_CRIT, "anslp_msg",
				"serialize(): byte count mismatch");
	
}

uint32 
anslp_response::serialize_object(ie_object_key &key, 
								 NetMsg &msg, 
								 coding_t coding) const
{
	uint32 obj_bytes_written = 0;
	anslp_object * obj = get_object(key);
	if (obj != NULL){
		obj->serialize(msg, coding, obj_bytes_written);
		return obj_bytes_written;
	}
	else{
		// Throwns an exception.
	}
}


bool anslp_response::check() const 
{
	
	// Check all objects for errors.
	for ( obj_iter i = objects.begin(); i != objects.end(); i++ ) {
		ie_object_key key = i->first;

		if ( (key.get_object_type() != msg_sequence_number::OBJECT_TYPE ) 
		    and (key.get_object_type() != information_code::OBJECT_TYPE )
		    and (key.get_object_type() != session_lifetime::OBJECT_TYPE ) 
		    and (key.get_object_type() != anslp_ipap_message::OBJECT_TYPE ) )
			return false;
	}

	return true; // no error found
}

void anslp_response::register_ie(IEManager *iem) const 
{
	iem->register_ie(cat_anslp_msg, get_msg_type(), 0, this);
}


/**
 * Set the desired session lifetime.
 *
 * @param seconds the session lifetime in milliseconds
 */
void anslp_response::set_session_lifetime(uint32 milliseconds) 
{
	set_object(new session_lifetime(milliseconds, anslp_object::tr_mandatory, true));
}


/**
 * Return the desired session lifetime.
 *
 * @return the session lifetime in milliseconds
 */
uint32 anslp_response::get_session_lifetime() const 
{
	
	ie_object_key key(session_lifetime::OBJECT_TYPE, 1);
	session_lifetime *lt = dynamic_cast<session_lifetime *>(get_object(key));

	if ( lt == NULL )
		return 0; 
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
void anslp_response::set_msg_sequence_number(uint32 msn) 
{
	set_object(new msg_sequence_number(msn, anslp_object::tr_mandatory,true));
}


/**
 * Sets the information code.
 *
 * This is used to indicate success or various degrees of failure.
 *
 * @param severity the severity class (4 bit)
 * @param response_code a response code, depending on the severity class (8 bit)
 * @param object_type the object type the response is about (16 bit)
 */
void anslp_response::set_information_code(uint8 severity, uint8 response_code,
                uint16 object_type) 
{

	set_object(new information_code(severity, response_code, object_type,
		anslp_object::tr_mandatory, true));
}


/**
 * Return the object type that triggered this response message.
 *
 * @return a MNSLP object type (16 bit)
 */
uint16 anslp_response::get_object_type() const 
{
	ie_object_key key(information_code::OBJECT_TYPE, 1);
	information_code *ic = dynamic_cast<information_code *>(get_object(key));

	if ( ic == NULL )
		return 0; // TODO: use a default constant here?
	else
		return ic->get_object_type();
}


/**
 * Return the severity class code.
 *
 * @return the severity class as defined in information_code
 */
uint8 anslp_response::get_severity_class() const 
{
	ie_object_key key(information_code::OBJECT_TYPE, 1);
	information_code *ic = dynamic_cast<information_code *>(
		get_object(key));

	if ( ic == NULL )
		return 0; // TODO: use a default constant here?
	else
		return ic->get_severity_class();
}

/**
 * Return the response code.
 *
 * @return the response code as defined in information_code
 */
uint8 anslp_response::get_response_code() const 
{
	ie_object_key key(information_code::OBJECT_TYPE, 1);
	information_code *ic = dynamic_cast<information_code *>(
		get_object(key));

	if ( ic == NULL )
		return 0; // TODO: use a default constant here?
	else
		return ic->get_response_code();
}


/**
 * Return the response object type.
 *
 * @return the response object type as defined in information_code
 */
uint16 anslp_response::get_response_object_type() const 
{
	ie_object_key key(information_code::OBJECT_TYPE, 1);
	information_code *ic = dynamic_cast<information_code *>(
		get_object(key));

	if ( ic == NULL )
		return 0; // TODO: use a default constant here?
	else
		return ic->get_response_object_type();
}

/**
 * Return the Message Sequence Number.
 *
 * @return the message sequence number.
 */
uint32 anslp_response::get_msg_sequence_number() const 
{
	ie_object_key key(msg_sequence_number::OBJECT_TYPE, 1);
	msg_sequence_number *msn = dynamic_cast<msg_sequence_number *>(
		get_object(key));

	if ( msn == NULL )
		return 0; // TODO: use a default constant here?
	else
		return msn->get_value();
}

/**
 * Check if this response indicates success.
 *
 * @return true if the response indicates success
 */
bool anslp_response::is_success() const 
{
	ie_object_key key(information_code::OBJECT_TYPE, 1);
	information_code *io = dynamic_cast<information_code *>(
		get_object(key));

	if ( io != NULL && io->is_success() )
		return true;
	else
		return false;
}


/**
 * Check if this is a response to the given CREATE message.
 *
 * This method can be used to find out if this RESPONSE message is really
 * the response to a previously sent CREATE message.
 *
 * @param msg a previously sent CREATE message
 * @return true if it is
 */
bool anslp_response::is_response_to(const anslp_create *msg) const 
{
	return get_msg_sequence_number() == msg->get_msg_sequence_number();
}


/**
 * Check if this is a response to the given REFRESH message.
 *
 * This method can be used to find out if this RESPONSE message is really
 * the response to a previously sent REFRESH message.
 *
 * @param msg a previously sent REFRESH message
 * @return true if it is
 */
bool anslp_response::is_response_to(const anslp_refresh *msg) const 
{
	return get_msg_sequence_number() == msg->get_msg_sequence_number();
}


/**
 * Set an ipap message for the configuration message. According with the RFC 
 * it is important the order of the message, so it is given by the insertion order.
 * In other words, the first inserted message is the message number one and so on.
 * 
 * @param message object to be inserted.
 */void anslp_response::set_anslp_ipap_message(anslp_ipap_message *message)
{
	set_object(message);
}
