/// ----------------------------------------*- mode: C++; -*--
/// @file anslp_ie.cpp
/// The ANSLP IE Manager
/// ----------------------------------------------------------
/// $Id: anslp_ie.cpp 2896 2015-08-30 14:54:00 amarentes $
/// $HeadURL: https://./src/msg/anslp_ie.cpp $
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
#include "logfile.h"

#include "msg/anslp_ie.h"
#include "msg/anslp_msg.h"
#include "msg/anslp_response.h"
#include <bitset>


using namespace anslp::msg;
using namespace protlib::log;

#define LogError(msg) Log(ERROR_LOG, LOG_NORMAL, "ANSLP_IEManager", msg)
#define LogWarn(msg) Log(WARNING_LOG, LOG_NORMAL, "ANSLP_IEManager", msg)
#define LogInfo(msg) Log(INFO_LOG, LOG_NORMAL, "ANSLP_IEManager", msg)
#define LogDebug(msg) Log(DEBUG_LOG, LOG_NORMAL, "ANSLP_IEManager", msg)


/**
 * This is where our single ANSLP_IEManager is stored.
 */
ANSLP_IEManager *ANSLP_IEManager::anslp_inst = NULL;


/**
 * Constructor for child classes.
 *
 * This constructor has been made protected to only allow instantiation
 * via the static instance() method.
 */
ANSLP_IEManager::ANSLP_IEManager() : IEManager() 
{
	// nothing to do
}


/**
 * Singleton static factory method.
 *
 * Returns the same ANSLP_IEManager object, no matter how often it is called.
 *
 * Note: Calling clear() causes the object to be deleted and the next call
 * to instance() create a new ANSLP_IEManager object.
 *
 * @return always the same ANSLP_IEManager object
 */
ANSLP_IEManager *ANSLP_IEManager::instance() 
{

	// Instance already exists, so return it.
	if ( anslp_inst )
		return anslp_inst;

	// We don't have an instance yet. Create it.
	try {
		LogDebug("To create ANSLP_IEManager singleton ");
		anslp_inst = new ANSLP_IEManager();
	}
	catch ( bad_alloc ) {
		LogError("cannot create ANSLP_IEManager singleton");
		throw IEError(IEError::ERROR_NO_IEMANAGER);
	}

	return anslp_inst;
}


/**
 * Delete the singleton instance.
 *
 * After calling this, all pointers to or into the object are invalid.
 * The instance() method has to be called before using the ANSLP_IEManager
 * next time.
 */
void ANSLP_IEManager::clear() {
		
	if ( anslp_inst != NULL ) {
		delete anslp_inst;
		anslp_inst = 0;
		LogDebug("deleted ANSLP_IEManager singleton");
	}
}


/**
 * Register all known IEs.
 *
 * This method clears the registry and then registers all IEs known to this
 * implementation. It solely exists for convenience.
 */
void ANSLP_IEManager::register_known_ies() 
{
	clear();
	LogDebug("Starting register_known_ies");
	
	ANSLP_IEManager *inst = instance();

	inst->register_ie(new anslp_create());
	inst->register_ie(new anslp_refresh());
	inst->register_ie(new anslp_response());
	inst->register_ie(new anslp_notify());
		
	inst->register_ie(new session_lifetime());
	inst->register_ie(new information_code());
	inst->register_ie(new message_hop_count());
	inst->register_ie(new selection_auctioning_entities());
	inst->register_ie(new msg_sequence_number());
	inst->register_ie(new anslp_ipap_message());
	
	// TODO: implement catch-all

	LogDebug("registered known IEs");
}


IE *ANSLP_IEManager::lookup_ie(uint16 category, uint16 type, uint16 subtype) {
	IE *ret = IEManager::lookup_ie(category, type, subtype);

	if ( ret != NULL )
		return ret;

	/*
	 * No IE registered. Return a default IE if possible.
	 */
	switch ( category ) {
	    case cat_anslp_msg:
		return IEManager::lookup_ie(cat_default_anslp_msg, 0, 0);

	    case cat_anslp_object:
		return IEManager::lookup_ie(cat_default_anslp_object, 0, 0);

	    default:
		return NULL;
	}

	// not reached
}


/**
 * Parse a PDU in NetMsg and return it.
 *
 * This method parses a PDU from the given NetMsg and returns a new
 * IE object representing the PDU. Based on the category parameter,
 * the IE can be a A-NSLP Message or A-NSLP Object (that is part of a
 * message).
 *
 * All errors encountered during parsing are added to the errorlist object.
 * If no errors occurred, the errorlist will be in the same state as before
 * the call.
 *
 * @param msg a buffer containing the serialized PDU
 * @param category the category the IE belongs to
 * @param coding the protocol version used in msg
 * @param errorlist returns the exceptions caught while parsing the message
 * @param bytes_read returns the number of bytes read from msg
 * @param skip if true, try to ignore errors and continue reading
 * @return the newly created IE, or NULL on error
 */
IE *ANSLP_IEManager::deserialize(NetMsg &msg, uint16 category,
		IE::coding_t coding, IEErrorList &errorlist,
		uint32 &bytes_read, bool skip) {

	// Note: The registered objects decide if they support a given coding.
	
	switch ( category ) {
		case cat_anslp_msg:
			return deserialize_msg(msg, coding,
					errorlist, bytes_read, skip);

		case cat_anslp_object:
			return deserialize_object(msg, coding,
					errorlist, bytes_read, skip);

		default:
			LogError("category " << category << " not supported");

			catch_bad_alloc( errorlist.put(
				new IEError(IEError::ERROR_CATEGORY)) );
			return NULL;
	}

	return NULL;	// not reached
}


/**
 * Use a registered natfw_msg instance to deserialize a NetMsg.
 *
 * Helper method for deserialize(). Parameters work like in deserialize().
 *
 * @param msg a buffer containing the serialized PDU
 * @param coding the protocol version used in msg
 * @param errorlist returns the exceptions caught while parsing the message
 * @param bytes_read returns the number of bytes read from msg
 * @param skip if true, try to ignore errors and continue reading
 * @return the newly created IE, or NULL on error
 */
IE *ANSLP_IEManager::deserialize_msg(NetMsg &msg,
		IE::coding_t coding, IEErrorList &errorlist,
		uint32 &bytes_read, bool skip) {

	/*
	 * Peek ahead to find out the message type.
	 */
	uint32 msg_type;
	try {
		uint32 header_raw = msg.decode32(false); // don't move position
		msg_type = anslp_msg::extract_msg_type(header_raw);
	}
	catch ( NetMsgError ) {
		errorlist.put(new IEMsgTooShort(
				coding, cat_anslp_msg, msg.get_pos()) );
		return NULL; // fatal error
	}


	IE *ie = new_instance(cat_anslp_msg, msg_type, 0);

	if ( ie == NULL ) {
		LogError("no anslp_msg registered for ID " << msg_type);
		errorlist.put(new IEError(IEError::ERROR_WRONG_TYPE));
		return NULL;
	}

	// ie returns iteself on success
	IE *ret = ie->deserialize(msg, coding, errorlist, bytes_read, skip);

	if ( ret == NULL ){
		delete ie;
	}
	
	return ret;	// the deserialized object on success, NULL on error
}


/**
 * Use a registered msnlp_object instance to deserialize a NetMsg.
 *
 * Helper method for deserialize(). Parameters work like in deserialize().
 *
 * @param msg a buffer containing the serialized PDU
 * @param coding the protocol version used in msg
 * @param errorlist returns the exceptions caught while parsing the message
 * @param bytes_read returns the number of bytes read from msg
 * @param skip if true, try to ignore errors and continue reading
 * @return the newly created IE, or NULL on error
 */
IE *ANSLP_IEManager::deserialize_object(NetMsg &msg,
		IE::coding_t coding, IEErrorList &errorlist,
		uint32 &bytes_read, bool skip) {

	/*
	 * Peek ahead to find out the MNSLP Object Type.
	 */
	uint32 object_type;
	try {
		uint32 header_raw = msg.decode32(false); // don't move position
		object_type = anslp_object::extract_object_type(header_raw);
	}
	catch ( NetMsgError ) {
		errorlist.put(new IEMsgTooShort(
				coding, cat_anslp_object, msg.get_pos()) );
		return NULL; // fatal error
	}

	IE *ie = new_instance(cat_anslp_object, object_type, 0);

	if ( ie == NULL ) {
		LogError("no anslp_object registered for ID " << object_type);
		errorlist.put(new IEError(IEError::ERROR_WRONG_SUBTYPE));
		return NULL;
	}

	// ie returns iteself on success
	IE *ret = ie->deserialize(msg, coding, errorlist, bytes_read, skip);

	if ( ret == NULL )
		delete ie;

	return ret;	// the deserialized object on success, NULL on error
}
