/// ----------------------------------------*- mode: C; -*--
/// @file ie_object_key.h
/// Tools for processing NSIS metering.
/// ----------------------------------------------------------
/// $Id: IE_object_key.h 2558 2015/08/30 12:29:00 amarentes $
/// $HeadURL: https://./include/IE_object_key.h $
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

#ifndef XML_OBJECT_KEY_H
#define XML_OBJECT_KEY_H


#include "stdincpp.h"

namespace anslp 
{
  namespace msg {


typedef enum 
{
    IPAP_INVALID = -1,
    IPAP_AUCTION = 0, 
    IPAP_BID,
    IPAP_ALLOCATION,
    IPAP_MAX_XML_OBJECT_TYPE
} ipap_xml_object_type_t;

/**
 * \class xml_object_key
 *
 *
 * \brief This respresents the key of a xml object to be exchanged.
 *
 * This class is used to represent the key of a xml object that is going to be exchange.
 * The class is the key used on the map of xml objects inside the anslp ipap xml message.
 *
 * \author Andres Marentes
 *
 * \version 0.1 
 *
 * \date 2015/09/07 18:29:00
 *
 * Contact: la.marentes455@uniandes.edu.co
 *  
 */
class xml_object_key
{

private:

	ipap_xml_object_type_t object_type;  		///< Object type assigned
	string id_object;           ///< Object id within the object type.

public:

	/// Constructor of the field key
	inline xml_object_key(): 
			object_type(IPAP_INVALID), id_object(""){}

	
	/// Constructor of the field key
	inline xml_object_key(ipap_xml_object_type_t _object_type, string _id_object): 
			object_type(_object_type), id_object(_id_object){}
	
	/// Destructor of the field key
	inline ~xml_object_key(){}
		
	/**
	 *  Equals to operator. It is equal when it has the same object type and sequence.
	 */
	inline bool operator ==(const xml_object_key &rhs) const
	{ 
		return ((object_type == rhs.object_type) && (id_object.compare(rhs.id_object) == 0 )); 
	}

	/** less operator. a key field is less than other when the object type is less 
	 *    or the string is shorter.
	*/ 
	bool operator< (const xml_object_key& rhs) const;

	/** 
	 * Assignment operator. 
	*/ 
	inline xml_object_key& operator= (const xml_object_key& param)
	{
		object_type = param.object_type;
		id_object = param.id_object;
		return *this;
	}
	
	inline ipap_xml_object_type_t get_object_type() const
	{
		return object_type;
	}
	
	inline string get_Id() const
	{
		return id_object;
	}
	
	/** 
	 * Not equal to operator. 
	*/ 
	inline bool operator != (const xml_object_key &rhs) const
	{
		return !(operator ==(rhs)); 
	}
	
	/** Convert the key field in a string.
	*/ 
	std::string to_string() const;

};

} // namespace msg

} // namespace anslp

#endif // XML_OBJECT_KEY_H
