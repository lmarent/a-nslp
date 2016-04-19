/// ----------------------------------------*- mode: C++; -*--
/// @file anslp_create.cpp
/// Implementation of the ANSLP CREATE Message.
/// ----------------------------------------------------------
/// $Id: anslp_create.cpp 2896 2015-08-30 $
/// $HeadURL: https://./src/msg/anslp_create.cpp $
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
#include "msg/anslp_create.h"
#include "msg/anslp_object.h"


using namespace anslp::msg;
using namespace protlib::log;

#define LogError(msg) ERRLog("anslp_create", msg)
#define LogWarn(msg) WLog("anslp_create", msg)
#define LogInfo(msg) ILog("anslp_create", msg)
#define LogDebug(msg) DLog("anslp_create", msg)


/**
 * Constructor.
 *
 * Only basic initialization is done. No anslp objects exist yet. All other
 * attributes are set to default values.
 *
 * @param proxy_mode set to true, if proxy mode is enabled
 */
anslp_create::anslp_create() : anslp_msg(anslp_create::MSG_TYPE) 
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
anslp_create::anslp_create(const anslp_create &other)
		: anslp_msg(other) 
{
	
	// nothing else to do
}


/**
 * Destructor.
 *
 * Deletes all objects this message contains.
 */
anslp_create::~anslp_create() 
{
	// Nothing to do, parent class handles this.
}


anslp_create *anslp_create::new_instance() const 
{
	anslp_create *inst = NULL;
	catch_bad_alloc(inst = new anslp_create());
	return inst;
}


anslp_create *anslp_create::copy() const 
{
	anslp_create *copy = NULL;
	catch_bad_alloc(copy = new anslp_create(*this));
	return copy;
}

void anslp_create::serialize(NetMsg &msg, coding_t coding,
		uint32 &bytes_written) const throw (IEError) 
{
	
	LogDebug("Begin serialize");
	
	if ( check() ){ 
		const IE *obj;
		uint32 obj_bytes_written;
		uint32 start_pos = msg.get_pos();
		/* 
		 * Write the header.
		 */
		anslp_msg::serialize(msg, coding, bytes_written);
		
		uint32 start_pos2 = msg.get_pos();

		/*
		 * Write the body: Serialize each object. According with the RFC
		 * Order of object is important. So we give the correct order here.
		 */
		ie_object_key key_msn(msg_sequence_number::OBJECT_TYPE, 1);
		bytes_written += serialize_object(key_msn, msg, coding);
		ie_object_key key_slf(session_lifetime::OBJECT_TYPE, 1);
		bytes_written += serialize_object(key_slf, msg, coding);
		ie_object_key key_sme(selection_auctioning_entities::OBJECT_TYPE, 1);
		bytes_written += serialize_object(key_sme, msg, coding);
		ie_object_key key_mhc(message_hop_count::OBJECT_TYPE, 1);
		bytes_written += serialize_object(key_mhc, msg, coding);
		
		/*
		 * Write the body: Serialize anslp ipap message.
		 */
		uint32 max_seq_nbr = objects.getMaxSequence(anslp_ipap_message::OBJECT_TYPE);

		LogDebug("Serialize anslp messages:" << max_seq_nbr );

		for ( uint32 i = 1; i <= max_seq_nbr; i++ ) {
			ie_object_key key_ipap(anslp_ipap_message::OBJECT_TYPE, i);
			LogDebug("Serialize object:" << i );
			bytes_written += serialize_object(key_ipap, msg, coding);
		}
		        
		// this would be an implementation error
		if ( bytes_written != msg.get_pos() - start_pos )
			Log(ERROR_LOG, LOG_CRIT, "anslp_msg",
					"serialize(): byte count mismatch");
	}
	else
	{
		
		throw IEError(IEError::ERROR_OBJ_SET_FAILED);
		Log(ERROR_LOG, LOG_CRIT, "anslp_msg",
					"serialize(): Data missing for the message");
	}
	
	LogDebug("Ending serialize");
}

uint32 
anslp_create::serialize_object(ie_object_key &key, NetMsg &msg, coding_t coding) const
{
	LogDebug("Starting serialize object");
	
	uint32 obj_bytes_written = 0;
	anslp_object *obj = get_object(key);
	if (obj != NULL){
		obj->serialize(msg, coding, obj_bytes_written);
		LogDebug("Ending serialize object" << obj_bytes_written);
		return obj_bytes_written;
	}
	else{
		// Throwns an exception.
	}
	
}

bool anslp_create::check() const 
{
	
	bool message_included = false;
	bool sequence_included = false;
	bool sel_meter_ent = false;
	bool session_lt = false;
	bool msg_hop_count = false;
	
	// Check all objects for errors.
	for ( obj_iter i = objects.begin(); i != objects.end(); i++ ) 
	{
		ie_object_key key = i->first;

		if (key.get_object_type() == msg_sequence_number::OBJECT_TYPE ){ 
			sequence_included = true;
		}
		if (key.get_object_type() == selection_auctioning_entities::OBJECT_TYPE ){
			sel_meter_ent = true;
		}
		if (key.get_object_type() == session_lifetime::OBJECT_TYPE ){
			session_lt = true;
		}
		if (key.get_object_type() == message_hop_count::OBJECT_TYPE ){
			msg_hop_count = true;
		}
		
		// A configure message can be received in a node without any mspec object
		// because they already have been deployed.
	}
				  
	return sequence_included & sel_meter_ent & session_lt & msg_hop_count; // no error found
}

void anslp_create::register_ie(IEManager *iem) const 
{
	iem->register_ie(cat_anslp_msg, get_msg_type(), 0, this);
}


/**
 * Set the desired session lifetime.
 *
 * @param seconds the session lifetime in milliseconds
 */
void anslp_create::set_session_lifetime(uint32 milliseconds) 
{
	set_object(new session_lifetime(milliseconds, anslp_object::tr_mandatory, true));
}


/**
 * Return the desired session lifetime.
 *
 * @return the session lifetime in milliseconds
 */
uint32 anslp_create::get_session_lifetime() const 
{
	
	ie_object_key key(session_lifetime::OBJECT_TYPE, 1);
	
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
void anslp_create::set_msg_sequence_number(uint32 msn) 
{
	set_object(new msg_sequence_number(msn, anslp_object::tr_mandatory, true));
}

/**
 * Return the desired message sequence number.
 *
 * @return the message sequence number
 */
uint32 anslp_create::get_msg_sequence_number() const 
{
	
	ie_object_key key(msg_sequence_number::OBJECT_TYPE, 1);
	
	msg_sequence_number *lt = dynamic_cast<msg_sequence_number *>(
		get_object(key));

	if ( lt == NULL )
		return 0; // TODO: use a default constant here?
	else
		return lt->get_value();
}


/**
 * Set the selection metering entities.
 * *
 * @param value the selection metering entities value
 */
void anslp_create::set_selection_auctioning_entities(uint32 value) 
{
	set_object(new selection_auctioning_entities(value, anslp_object::tr_mandatory, true));
}

/**
 * Return the selection metering entities.
 *
 *
 * @return the selection metering entities's value
 */
uint32 anslp_create::get_selection_auctioning_entities() const 
{
	
	ie_object_key key(selection_auctioning_entities::OBJECT_TYPE, 1);
	
	selection_auctioning_entities *sme = dynamic_cast<selection_auctioning_entities *>(
		get_object(key));

	if ( sme == NULL )
		return 0; // TODO: use a default constant here?
	else
		return sme->get_value();
}


/**
 * Set the message hop count.
 * *
 * @param value the message hop count value
 */
void anslp_create::set_message_hop_count(uint32 value) 
{
	set_object(new message_hop_count(value, anslp_object::tr_mandatory, true));
}

/**
 * Return the message hop count.
 *
 *
 * @return the message hop count's value
 */
uint32 anslp_create::get_message_hop_count() const {
	
	ie_object_key key(message_hop_count::OBJECT_TYPE, 1);
	
	message_hop_count *mhc = dynamic_cast<message_hop_count *>(
		get_object(key));

	if ( mhc == NULL )
		return 0; // TODO: use a default constant here?
	else
		return mhc->get_value();
}

/**
 * Set an ipfix message for the configuration message. According with the RFC 
 * it is important the order of the message, so the order is given by the order in which
 * it is inserted. In Other words, the first message inserted is the number one message and so on.
 * *
 * @param message object to be inserted.
 */
void anslp_create::set_mspec_object(anslp_mspec_object *message)
{
	set_object(message);
}

void anslp_create::get_mspec_objects(vector<anslp_mspec_object *> &list_return)
{
	
	LogDebug("Starting get_mspec_objects");	
	
	for ( obj_iter i = objects.begin(); i != objects.end(); i++ ) {
		const ie_object_key key = i->first;
		const anslp_mspec_object *obj = dynamic_cast<const anslp_mspec_object *>( i->second);
		
		if ((key.get_object_type() != message_hop_count::OBJECT_TYPE) 
		    and (key.get_object_type() != message_hop_count::OBJECT_TYPE)
		    and (key.get_object_type() != selection_auctioning_entities::OBJECT_TYPE) 
		    and (key.get_object_type() != msg_sequence_number::OBJECT_TYPE)
		    and (key.get_object_type() != session_lifetime::OBJECT_TYPE)){
			LogDebug("Starting get_mspec_objects");	
			list_return.push_back(obj->copy());
		}
	}
}
 
