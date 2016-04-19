/// ----------------------------------------*- mode: C++; -*--
/// @file anslp_bidding.cpp
/// Implementation of the ANSLP BIDDING Message.
/// ----------------------------------------------------------
/// $Id: anslp_bidding.cpp 2896 2015-08-30 $
/// $HeadURL: https://./src/msg/anslp_bidding.cpp $
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
#include "msg/anslp_bidding.h"
#include "msg/anslp_object.h"


using namespace anslp::msg;
using namespace protlib::log;

#define LogError(msg) ERRLog("anslp_bidding", msg)
#define LogWarn(msg) WLog("anslp_bidding", msg)
#define LogInfo(msg) ILog("anslp_bidding", msg)
#define LogDebug(msg) DLog("anslp_bidding", msg)


/**
 * Constructor.
 *
 * Only basic initialization is done. No anslp objects exist yet. All other
 * attributes are set to default values.
 *
 */
anslp_bidding::anslp_bidding() : anslp_msg(anslp_bidding::MSG_TYPE) 
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
anslp_bidding::anslp_bidding(const anslp_bidding &other)
		: anslp_msg(other) 
{
	
	// nothing else to do
}


/**
 * Destructor.
 *
 * Deletes all objects this message contains.
 */
anslp_bidding::~anslp_bidding() 
{
	// Nothing to do, parent class handles this.
}


anslp_bidding *anslp_bidding::new_instance() const 
{
	anslp_bidding *inst = NULL;
	catch_bad_alloc(inst = new anslp_bidding());
	return inst;
}


anslp_bidding *anslp_bidding::copy() const 
{
	anslp_bidding *copy = NULL;
	catch_bad_alloc(copy = new anslp_bidding(*this));
	return copy;
}

void anslp_bidding::serialize(NetMsg &msg, coding_t coding,
		uint32 &bytes_written) const throw (IEError) 
{
	
	LogDebug("Begin serialize");
	
	if ( check() ) { 
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
	} else {
		
		throw IEError(IEError::ERROR_OBJ_SET_FAILED);
		Log(ERROR_LOG, LOG_CRIT, "anslp_msg",
					"serialize(): Data missing for the message");
	}
	
	LogDebug("Ending serialize");
}

uint32 
anslp_bidding::serialize_object(ie_object_key &key, NetMsg &msg, coding_t coding) const
{
	LogDebug("Starting serialize object");
	
	uint32 obj_bytes_written = 0;
	anslp_object *obj = get_object(key);
	if (obj != NULL) {
		obj->serialize(msg, coding, obj_bytes_written);
		LogDebug("Ending serialize object" << obj_bytes_written);
		return obj_bytes_written;
	}
	else {
		// Throwns an exception.
	}
	
}

bool anslp_bidding::check() const 
{
	
	bool message_included = false;
	bool sequence_included = false;
	
	// Check all objects for errors.
	for ( obj_iter i = objects.begin(); i != objects.end(); i++ ) 
	{
		ie_object_key key = i->first;

		if (key.get_object_type() == msg_sequence_number::OBJECT_TYPE ){ 
			sequence_included = true;
		}
		
		if (key.get_object_type() == anslp_ipap_message::OBJECT_TYPE ){
			message_included = true;
		}
	}
				  
	return sequence_included & message_included; // no error found
}

void anslp_bidding::register_ie(IEManager *iem) const 
{
	iem->register_ie(cat_anslp_msg, get_msg_type(), 0, this);
}


/**
 * Sets the message sequence number.
 *
 * See the msg_sequence_number object for more information.
 *
 * @param msn the value of the message sequence number
 */
void anslp_bidding::set_msg_sequence_number(uint32 msn) 
{
	set_object(new msg_sequence_number(msn, anslp_object::tr_mandatory, true));
}

/**
 * Return the desired message sequence number.
 *
 * @return the message sequence number
 */
uint32 anslp_bidding::get_msg_sequence_number() const 
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
 * Set an ipap message for the bidding message. 
 * it is important the message order. The order is given by the order in which
 * it is inserted. In Other words, the first message inserted is the number one message and so on.
 * *
 * @param message object to be inserted.
 */
void anslp_bidding::set_mspec_object(anslp_mspec_object *message)
{
	set_object(message);
}

void anslp_bidding::get_mspec_objects(vector<anslp_mspec_object *> &list_return)
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
 
