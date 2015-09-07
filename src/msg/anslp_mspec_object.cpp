/// ----------------------------------------*- mode: C++; -*--
/// @file anslp_mspec_object.cpp
/// anslp_object.cpp - Implementation of the abstract anslp_mspec_object class
/// ----------------------------------------------------------
/// $Id: anslp_mspec_object.cpp 2558 2015-08-30 amarentes $
/// $HeadURL: https://src/msg/anslp_mspec_object.cpp $
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
#include "msg/anslp_mspec_object.h"


using namespace anslp::msg;
using namespace protlib::log;


/**
 * Standard constructor.
 *
 * This may leave an instance in an uninitialized state. Use deserialize()
 * to initialize it from a NetMsg.
 *
 * The treatment is set to mandatory.
 */
anslp_mspec_object::anslp_mspec_object() : anslp_object() 
{

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
anslp_mspec_object::anslp_mspec_object(uint16 obj_type, treatment_t tr, bool _unique)
		: anslp_object(obj_type, tr, _unique) 
{

	// nothing to do
}


anslp_mspec_object::~anslp_mspec_object() 
{
	// nothing to do
}
