/// ----------------------------------------*- mode: C++; -*--
/// @file selection_auctioning_entities.cpp
/// The selection_auctioning_entities Object.
/// ----------------------------------------------------------
/// $Id: selection_auctioning_entities.cpp 2558 2015-08-30 amarentes $
/// $HeadURL: https://./src/msg/selection_auctioning_entities.cpp $
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

#include "msg/selection_auctioning_entities.h"


using namespace anslp::msg;


const char *const selection_auctioning_entities::ie_name = "selection_auctioning_entities";


/**
 * Default constructor.
 */
selection_auctioning_entities::selection_auctioning_entities()
		: anslp_object(OBJECT_TYPE, tr_mandatory, true), sme(sme_all) 
{

	// nothing to do
}


/**
 * Constructor for manual use.
 *
 * @param value the nonce value
 */
selection_auctioning_entities::selection_auctioning_entities(uint32 _sme, 
		treatment_t treatment, bool _unique) : 
		anslp_object(OBJECT_TYPE, treatment, _unique), sme(_sme) 
{

	// nothing to do
}


selection_auctioning_entities::~selection_auctioning_entities() 
{
	// nothing to do
}


selection_auctioning_entities *
selection_auctioning_entities::new_instance() const 
{
	selection_auctioning_entities *q = NULL;
	catch_bad_alloc( q = new selection_auctioning_entities() );
	return q;
}


selection_auctioning_entities *
selection_auctioning_entities::copy() const 
{
	selection_auctioning_entities *q = NULL;
	catch_bad_alloc( q = new selection_auctioning_entities(*this) );
	return q;
}


bool 
selection_auctioning_entities::deserialize_body(NetMsg &msg, uint16 body_length,
		IEErrorList &err, bool skip) 
{

	set_value(msg.decode32());

	return true; // success, all values are syntactically valid
}

void 
selection_auctioning_entities::serialize_body(NetMsg &msg) const 
{
	msg.encode32(get_value());
}


size_t 
selection_auctioning_entities::get_serialized_size(coding_t coding) const 
{
	return HEADER_LENGTH + 4;
}


// All values are valid.
bool 
selection_auctioning_entities::check_body() const 
{
	return true;
}


bool 
selection_auctioning_entities::equals_body(const anslp_object &obj) const 
{

	const selection_auctioning_entities *other
		= dynamic_cast<const selection_auctioning_entities *>(&obj);

	return other != NULL && get_value() == other->get_value();
}

const char *
selection_auctioning_entities::get_ie_name() const 
{
	return ie_name;
}


ostream &
selection_auctioning_entities::print_attributes(ostream &os) const 
{
	return os << ", value=" << get_value();
}


/**
 * Returns the selection_auctioning_entities value.
 *
 * @return the selection_auctioning_entities value
 */
uint32 
selection_auctioning_entities::get_value() const 
{
	return sme;
}


/**
 * Set the selection_auctioning_entities value.
 *
 * @param val the selection_auctioning_entities value
 */
void 
selection_auctioning_entities::set_value(uint32 _sme) 
{
	sme = _sme;
}
