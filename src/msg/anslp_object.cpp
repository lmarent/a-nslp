/// ----------------------------------------*- mode: C++; -*--
/// @file anslp_object.cpp
/// anslp_object.cpp - Implementation of the abstract anslp_object class
/// ----------------------------------------------------------
/// $Id: anslp_object.cpp 2558 2015-08-30 amarentes $
/// $HeadURL: https://src/msg/anslp_object.cpp $
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
#include <cstring>
#include <limits>

#include "logfile.h"

#include "msg/anslp_ie.h"
#include "msg/anslp_object.h"


using namespace anslp::msg;
using namespace protlib::log;


#define LogError(msg) ERRLog("anslp_object", msg)
#define LogWarn(msg) WLog("anslp_object", msg)
#define LogInfo(msg) ILog("anslp_object", msg)
#define LogDebug(msg) DLog("anslp_object", msg)

/**
 * Standard constructor.
 *
 * This may leave an instance in an uninitialized state. Use deserialize()
 * to initialize it from a NetMsg.
 *
 * The treatment is set to mandatory.
 */
anslp_object::anslp_object()
		: IE(cat_anslp_object), treatment(tr_mandatory), unique(true) {

	// nothing to do									
	
}


/**
 * Constructor for manual use.
 *
 * The obj_type value must not be larger than 12 bit. Note that only a small
 * subset is valid according to the ANSLP standard and registered with IANA.
 *
 * @param object_type the ANSLP Object Type (12 bit)
 */
anslp_object::anslp_object(uint16 obj_type, treatment_t tr, bool _unique)
		: IE(cat_anslp_object), object_type(obj_type), treatment(tr), unique(_unique) 
{

	// nothing to do

}


anslp_object::~anslp_object() 
{
	// nothing to do
}


anslp_object::treatment_t 
anslp_object::get_treatment() const 
{
	return treatment;
}

void anslp_object::set_treatment(treatment_t tr) 
{
	treatment = tr;
}

uint16 anslp_object::get_object_type() const 
{
	return object_type;
}

void anslp_object::set_object_type(uint16 obj_type) 
{
	object_type = obj_type;
}

bool anslp_object::is_unique() const 
{
	return unique;
}

void anslp_object::set_unique(bool _unique)
{
	unique = _unique;
}
/**
 * Extract the object type.
 *
 * Extract the object type from a raw header. The header is expected to be
 * in host byte order already.
 *
 * @param header_raw 32 bits read from a NetMsg
 * @return the object type (12 bit)
 */
uint16 anslp_object::extract_object_type(uint32 header_raw) throw () 
{
	return ( (header_raw >> 16) & 0xFFF );
}


/**
 * Parse a ANSLP object header.
 *
 * If the header is invalid, usually because the NetMsg is truncated, the
 * msg's position is reset.
 *
 * This function throws no exception (that is worth catching).
 *
 * TODO: skip is not yet supported.
 *
 * @param msg the NetMsg to read from
 * @param body_length the length of the body in bytes (according to the header)
 * @param errorlist errors
 * @param skip skip the header in case of parse errors
 * @return true on success, false if there's a parse error
 */
bool anslp_object::deserialize_header(NetMsg &msg, uint16 &body_length,
		IEErrorList &errorlist, bool skip) 
{

	uint32 start_pos = msg.get_pos();

	uint32 header_raw;

	try {
		header_raw = msg.decode32();
	}
	catch ( NetMsgError ) {
		catch_bad_alloc( errorlist.put(
			new IEMsgTooShort(CODING, get_category(), start_pos)) );
		return false;
	}

	set_treatment( treatment_t(header_raw >> 30) );
	set_object_type( extract_object_type(header_raw) );

	// The header's value is in 32bit words, we convert it to bytes.
	body_length = (header_raw & 0xFFF) * 4u;

	// Error: Message is shorter than the length field makes us believe.
	if ( msg.get_bytes_left() < body_length ) {
		catch_bad_alloc( errorlist.put(
			new IEMsgTooShort(CODING, category, start_pos)) );

		msg.set_pos(start_pos);
		return false;
	}

	return true;
}


// Note: coding is ignored
IE *anslp_object::deserialize(NetMsg &msg, coding_t coding,
		IEErrorList &err, uint32 &bytes_read, bool skip) 
{

	LogDebug("Begin deserialize");

	bytes_read = 0;
	uint32 start_pos = msg.get_pos();

	// check if coding is supported
	uint32 tmp;
	if ( ! check_deser_args(CODING, err, tmp) )
		return NULL;

	// Parse header
	uint16 body_length = 0;
	if ( ! deserialize_header(msg, body_length, err, skip) )
		return NULL;

	// Parse body
	if ( ! deserialize_body(msg, body_length, err, skip) ){
		return NULL;
	}
	
	bytes_read = msg.get_pos() - start_pos;
	
	LogDebug("Ending deserialize " << bytes_read 
					<< " Object type:" << get_object_type());
	return this;	// success
}


/**
 * Write an ANSLP Object header.
 *
 * Note that the body_length parameter is in bytes. It has to be a multiple
 * of 4 because all lengths are measured in 32bit words.
 *
 * @param msg the NetMsg to write to
 * @param body_length the length of the body in bytes(!)
 */
void anslp_object::serialize_header(NetMsg &msg, uint16 body_length) const 
{

	uint32 header = (get_treatment() << 30) | (get_object_type() << 16)
			| (body_length / 4);

	msg.encode32(header);
}


// Note: coding is ignored
void anslp_object::serialize(NetMsg &msg, coding_t coding,
		uint32 &bytes_written) const throw (IEError) 
{

	// TODO: if ( coding != CODING ) -> unsupported!

	uint32 start_pos = msg.get_pos();
	
	// Check if the object is in a valid state. Throw an exception if not.
	uint32 tmp;
	check_ser_args(coding, tmp);
		
	/*
	 * Write header
	 */
	uint16 body_length = get_serialized_size(CODING) - HEADER_LENGTH;

	try {
		serialize_header(msg, body_length);
	}
	catch (NetMsgError) {
		throw IEMsgTooShort(CODING, get_category(), msg.get_pos());
	}

	/*
	 * Write body
	 */
	try {
		serialize_body(msg);
	}
	catch (NetMsgError) {
		throw IEMsgTooShort(CODING, get_category(), msg.get_pos());
	}

	bytes_written = msg.get_pos() - start_pos;
}


/**
 * Check if a given coding is supported.
 *
 * This implementation only supports protocol_v1. Subclasses supporting
 * more codings have to override this method.
 *
 * @param coding the encoding to test
 */
bool anslp_object::supports_coding(coding_t coding) const 
{
	return coding == protocol_v1;
}


/**
 * Register this object with an IEManager.
 *
 * Using new_instance(), each subclass of anslp_object can act as a factory
 * for itself. That way the manager can create new instances of this class
 * on request.
 *
 * @param iem the IEManager to register with
 */
void anslp_object::register_ie(IEManager *iem) const 
{
	iem->register_ie(cat_anslp_object, get_object_type(), 0, this);
}


/**
 * Check the state of this object.
 *
 * This method can only check the header, so it relies on check_body()
 * in child classes.
 *
 * @return true if the object is in a valid state and false otherwise
 */
bool anslp_object::check() const 
{
	uint16_t test = (~0xFFF);
	uint16_t data = get_object_type();

	if ( (data & test) != 0 ){
		return false;
	}

	return check_body();
}


/**
 * Check for equality.
 *
 * @param ie the other operand
 * @return true if this object and the parameter are equal
 */
bool anslp_object::operator==(const IE &ie) const 
{

	const anslp_object *other = dynamic_cast<const anslp_object *>(&ie);

	return other != NULL
		&& get_object_type() == other->get_object_type()
		&& get_treatment() == other->get_treatment()
		&& equals_body(*other);
}


/**
 * Print this object's attributes.
 *
 * This is for debugging only and is used as part of print(). The
 * implementation provided is incomplete and should be overridden by child
 * classes. Only attributes added by the child class have to be printed.
 *
 * @param os the output stream to write to
 */
ostream &anslp_object::print_attributes(ostream &os) const 
{
	return os << "*UNIMPLEMENTED*";
}


ostream &anslp_object::print(ostream &os, uint32 level, const uint32 indent,
		const char *name) const 
{

	os << setw(level*indent) << "";

	if ( name )
		os << name << " = ";

	os << "[" << get_ie_name() << ": ";

	os << "type=" << get_object_type() << ", ";
	os << "treatment=" << get_treatment();

	print_attributes(os);

	os << "]";

	return os;
}
