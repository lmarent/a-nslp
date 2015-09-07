/// ----------------------------------------*- mode: C++; -*--
/// @file anslp_msg.cpp
/// Implementation of the ANSLP Message.
/// ----------------------------------------------------------
/// $Id: anslp_msg.cpp 2558 2015-08-30 14:44:00Z amarentes $
/// $HeadURL: https://./src/msg/anslp_msg.cpp $
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
#include <iomanip>	// for setw()

#include "logfile.h"

#include <bitset>
#include "msg/anslp_ie.h"
#include "msg/ie_object_key.h"
#include "msg/anslp_msg.h"


using namespace anslp::msg;
using namespace protlib::log;


#define LogError(msg) Log(ERROR_LOG, LOG_NORMAL, "ANSLP_MSG", msg)
#define LogWarn(msg) Log(WARNING_LOG, LOG_NORMAL, "ANSLP_MSG", msg)
#define LogInfo(msg) Log(INFO_LOG, LOG_NORMAL, "ANSLP_MSG", msg)
#define LogDebug(msg) Log(DEBUG_LOG, LOG_NORMAL, "ANSLP_MSG", msg)

/**
 * Length of a ANSLP Message header in bytes.
 */
const uint16 anslp_msg::HEADER_LENGTH = 4;


const char *const anslp_msg::ie_name = "anslp_msg";


/**
 * Standard constructor.
 *
 * Creates an empty ANSLP Message.
 */
anslp_msg::anslp_msg()
		: IE(cat_anslp_msg), msg_type(0) {

	// nothing to do
}


/**
 * Constructor for manual use.
 *
 * Only basic initialization is done. No ANSLP objects exist yet. All other
 * attributes are set to default values. The msg_type parameter is assigned
 * by IANA.
 *
 * @param type the ANSLP Message Type (8 bit)
 */
anslp_msg::anslp_msg(uint8 type)
		: IE(cat_anslp_msg), msg_type(type) 
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
anslp_msg::anslp_msg(const anslp_msg &other)
		: IE(other.category), msg_type(other.get_msg_type()),
		  objects(other.objects) 
{
	
	// nothing else to do
}


/**
 * Destructor.
 *
 * Deletes all objects this message contains.
 */
anslp_msg::~anslp_msg() 
{
	// Nothing to do, objects are deleted by ie_store's destructor.
}


anslp_msg *anslp_msg::new_instance() const 
{
	anslp_msg *qp = NULL;
	catch_bad_alloc(qp = new anslp_msg());
	return qp;
}


anslp_msg *anslp_msg::copy() const 
{
	anslp_msg *qp = NULL;
	catch_bad_alloc(qp = new anslp_msg(*this));
	return qp;
}


/**
 * Extract the ANSLP Message Type.
 *
 * Extract the ANSLP Message Type from a raw header. The header is expected
 * to be in host byte order already.
 *
 * @param header_raw 32 bits from a NetMsg
 * @return the ANSLP Message Type
 */
uint8 anslp_msg::extract_msg_type(uint32 header_raw) throw () 
{
	return (header_raw >> 24);
}


/**
 * Return the message sequence number.
 *
 * This may only be used if has_msg_sequence_number() returned true!
 *
 * @return the value of the message sequence number
 */
uint32 anslp_msg::get_msg_sequence_number() const 
{
	
	ie_object_key key(msg_sequence_number::OBJECT_TYPE, 1);
	
	msg_sequence_number *msn = dynamic_cast<msg_sequence_number *>(
		get_object(key));

	assert( msn != NULL );

	return msn->get_value();
}


/**
 * Test if this message contains a message sequence number object.
 *
 * @return true if it contains an MSN, and false otherwise
 */
bool anslp_msg::has_msg_sequence_number() const 
{
	
	ie_object_key key(msg_sequence_number::OBJECT_TYPE, 1);
	msg_sequence_number *msn = dynamic_cast<msg_sequence_number *>(
		get_object(key));

	return msn != NULL;
}


/**
 * Initialize the object from the given NetMsg.
 *
 * The object returns itself if the deserializing was successful. No new
 * object is created.
 *
 * Note: The ANSLP Header doesn't contain a length field, it is only known
 * to the layer below. Because of this it is very important that NetMsg is in
 * a state where NetMsg::get_bytes_left() returns the length of the ANSLP
 * Message before calling deserialize().
 *
 * @return this object on success, NULL on error
 */
anslp_msg *
anslp_msg::deserialize(NetMsg &msg, coding_t coding,
		IEErrorList &errorlist, uint32 &bytes_read, bool skip) 
{

	bytes_read = 0;
	uint32 start_pos = msg.get_pos();

	/*
	 * Parse the header.
	 */
	uint32 header_raw;

	try {
		header_raw = msg.decode32();		
	}
	catch ( NetMsgError ) {
		catch_bad_alloc( errorlist.put(
			new IEMsgTooShort(coding, get_category(), start_pos)) );

		return ( skip ? this : NULL ); // We can't go on from here.
	}

	set_msg_type( extract_msg_type(header_raw) );

	bytes_read += 4;


	/* 
	 * Read as many objects from the body as possible.
	 */
	IEManager *mgr = ANSLP_IEManager::instance();

	while ( msg.get_bytes_left() > 0 ) 
	{
		uint32 saved_pos = msg.get_pos();
		uint32 obj_bytes_read;

		IE *ie = mgr->deserialize(msg, cat_anslp_object, coding,
				errorlist, obj_bytes_read, skip);

		// Deserializing failed.
		if ( ie == NULL ){
			return NULL;
		}

		anslp_object *obj = dynamic_cast<anslp_object *>(ie);

		LogDebug("Deserializing object" << obj->get_object_type() << "Unique" << obj->is_unique());

		/*
		 * Error: no anslp_object returned.
		 */
		if ( obj == NULL ) {		
			Log(ERROR_LOG, LOG_CRIT, "anslp_msg",
				"deserialize() returned object of type "
				<< ie->get_ie_name()
				<< " but type asnlp_object expected. "
				<< "Registration invalid?");
			delete ie;
			return NULL;
		}

		// test for duplicate object, other than metering messages.
		if (obj->is_unique())
		{			
			ie_object_key key(obj->get_object_type(),1);
			if ( get_object( key ) != NULL ) {
				catch_bad_alloc( errorlist.put(
					new PDUSyntaxError(coding, get_category(),
						obj->get_object_type(), 1,
						saved_pos, "Duplicate object")) );

				if ( ! skip )
					return NULL;
			}
		}			
		bytes_read += obj_bytes_read;
		set_object(obj);
	}

	// empty messages are not allowed
	if ( get_num_objects() == 0 ) {
		catch_bad_alloc( errorlist.put(
			new PDUSyntaxError(coding, get_category(), 0, 0,
				start_pos, "message may not be empty")) );

		if ( ! skip )
			return NULL;
	}
		
	// this would be an implementation error
	if ( bytes_read != msg.get_pos() - start_pos )
		Log(ERROR_LOG, LOG_CRIT, "anslp_msg",
				"deserialize(): byte count mismatch");

	return this; // no error, return the initialized object
}


/**
 * Write the object to a NetMsg.
 *
 * Serializing into a non-empty NetMsg object is supported. The NetMsg
 * has to be big enough and its position pointer has to be set to the
 * desired position. At the end of the method, the position pointer
 * points to the position after the last written byte.
 *
 * If serialize() throws an exception, both NetMsg and bytes_written are left
 * in an undefined state.
 *
 * @param msg a NetMsg that is large enough
 * @param coding the encoding
 * @param bytes_written returns the number of written bytes
 */
void anslp_msg::serialize(NetMsg &msg, coding_t coding,
		uint32 &bytes_written) const throw (IEError) 
{
	
	LogDebug("Start serialize");
	
	bytes_written = 0;
	uint32 start_pos = msg.get_pos();

	// Check if the object is in a valid state. Throw an exception if not.
	uint32 tmp;
	check_ser_args(coding, tmp);

	/* 
	 * Write the header.
	 */
	 
	uint32 header_raw = (get_msg_type() << 24);
	try {
		msg.encode32(header_raw);
		bytes_written += 4;
	}
	catch (NetMsgError) {
		throw IEMsgTooShort(coding, get_category(), msg.get_pos());
	}
	
	LogDebug("Ending serialize");
	
	// this would be an implementation error
	if ( bytes_written != msg.get_pos() - start_pos )
		Log(ERROR_LOG, LOG_CRIT, "anslp_msg",
				"serialize(): byte count mismatch");
}


bool anslp_msg::check() const 
{
	
	// Error: no objects available
	if ( get_num_objects() == 0 )
		return false;

	// Check all objects for errors.
	for ( obj_iter i = objects.begin(); i != objects.end(); i++ ) {
		const IE *obj = i->second;

		if ( obj->check() == false )
			return false;
	}

	return true; // no error found
}


bool anslp_msg::supports_coding(coding_t c) const 
{
	return c == protocol_v1;
}


/**
 * Calculate the serialized size of this message, including the header.
 * The size may depend on the selected encoding.
 *
 * @param coding the encoding
 * @return the size in bytes, including the header
 */
size_t anslp_msg::get_serialized_size(coding_t coding) const 
{
	size_t size = HEADER_LENGTH;

	LogDebug("Start get_serialized_size");

	for ( obj_iter i = objects.begin(); i != objects.end(); i++ ) {
		LogDebug("Start get_serialized_size");
		const IE *obj = i->second;

		
		
		size += obj->get_serialized_size(coding);
		
	}
	
	LogDebug("Ending get_serialized_size:" << size);
	return size;
}


bool anslp_msg::operator==(const IE &ie) const 
{

	const anslp_msg *p = dynamic_cast<const anslp_msg *>(&ie);

	if ( (p == NULL) || (get_msg_type() != p->get_msg_type()) ){
		return false;
	}

	// Return true iff all objects are equal, too.
	return ( objects == p->objects );
}


const char *anslp_msg::get_ie_name() const 
{
	return ie_name;
}


ostream &anslp_msg::print(ostream &os, uint32 level, const uint32 indent,
		const char *name) const 
{

	os << setw(level*indent) << "";

	if ( name )
		os << name << " = ";

	os << "[" << get_ie_name() << ": ";

	os << "msg_type=" << int(get_msg_type()) << ", ";

	os << "objects={" << endl;

	for ( obj_iter i = objects.begin(); i != objects.end(); i++ ) {
		i->second->print(os, level+1, indent) << endl;
	}

	os << setw(level*indent) << "" << "}]";

	return os;
}


void anslp_msg::register_ie(IEManager *iem) const 
{
	iem->register_ie(cat_anslp_msg, get_msg_type(), 0, this);
}


/**
 * Returns the Message Type.
 *
 * @return the Message Type.
 */
uint8 anslp_msg::get_msg_type() const 
{
	return msg_type;
}


/**
 * Set the Message Type.
 *
 * @param msg_type the Message Type
 */
void anslp_msg::set_msg_type(uint8 msg_type) 
{
	this->msg_type = msg_type;
}


/**
 * Return the number of contained objects.
 *
 * @return the number of objects this message contains
 */
size_t anslp_msg::get_num_objects() const 
{
	return objects.size();
}


/**
 * Returns the message object registered for a given ANSLP Object Type.
 *
 * The object is not removed from the message.
 *
 * @param the object type (12 bit)
 * @return the ANSLP object or NULL, if none is registered for that type
 */
anslp_object *anslp_msg::get_object(ie_object_key &object_type) const 
{
	return dynamic_cast<anslp_object *>( objects.get(object_type) );
}


/**
 * Add an object to this message.
 *
 * If there is already an object registered with the same object type
 * the old one is deleted. Note that anslp_msg's destructor deletes
 * all registered objects.
 *
 * @param obj the object to add
 */
void anslp_msg::set_object(anslp_object *obj) 
{
	
	if (obj-> is_unique()){
		ie_object_key key(obj->get_object_type() , 1);
		objects.set(key, obj);
	}
	else{
		
		uint32 sequence = objects.getMaxSequence(obj->get_object_type());
		sequence = sequence + 1;
		ie_object_key key(obj->get_object_type() , sequence);
		objects.set(key, obj);
	}	
	
}


/**
 * Remove a ANSLP object.
 *
 * Remove the object with the given ASNLP Object Type from this natfw_msg.
 * If there is no object with that type, NULL is returned.
 *
 * The object is @em not deleted.
 *
 * @param object_type the object type (12 bit)
 * @return the object with that type or NULL if there is none
 */
anslp_object *anslp_msg::remove_object(ie_object_key &object_type) 
{
	return dynamic_cast<anslp_object *>( objects.remove(object_type) );
}
