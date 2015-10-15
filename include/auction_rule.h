/// ----------------------------------------*- mode: C++; -*--
/// @file auction_rule.h
/// The auction rule classes.
/// ----------------------------------------------------------
/// $Id: auction_rule.h 2558 2015-09-01 09:07:00 amarentes $
/// $HeadURL: https://./include/auction_rule.h $
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
#ifndef AUCTION_RULE_H
#define AUCTION_RULE_H

#include "protlib_types.h"
#include "address.h"
#include "mspec_rule_key.h"
#include "msg/anslp_mspec_object.h"
#include <map>
#include <vector>


namespace anslp 
{
    using protlib::uint8;
    using protlib::uint16;
    using protlib::uint32;
    using protlib::hostaddress;

/**
 * Auction rule.
 *
 * This consists of two parts: 
 *    a set of objects corresponding to requests 
 * 	  a set of objects corresponding to responses to those objects.
 *
 * An auction rule is just an abstraction. It has to be mapped to a concrete
 * auction rule, which is vendor-dependent.
 */

typedef std::map<mspec_rule_key, msg::anslp_mspec_object *> objectList_t;
typedef std::map<mspec_rule_key, msg::anslp_mspec_object *>::iterator objectListIter_t;
typedef std::map<mspec_rule_key, msg::anslp_mspec_object *>::const_iterator objectListConstIter_t;


class auction_rule 
{

  public:

	/**
	* Constructor.
	*/
	auction_rule();

	/**
	* Copy constructor.
	*/
	auction_rule(const auction_rule &rhs);
  
	~auction_rule();

    /**
    * Create a deep copy of this object.
    */
	auction_rule *copy() const;

	void set_request_object(mspec_rule_key key, msg::anslp_mspec_object *obj);

	void set_response_object(mspec_rule_key key, msg::anslp_mspec_object *obj);

    /**
    * Add a mspec objet to the rule. This creates the key to share with the
    * auctioning application. 
    * The key created is returned.
    */    
    mspec_rule_key set_request_object(msg::anslp_mspec_object *obj);
    
    mspec_rule_key set_response_object(msg::anslp_mspec_object *obj);
	
	size_t get_number_mspec_request_objects();

	size_t get_number_mspec_response_objects();

	size_t get_number_mspec_request_objects() const;

	size_t get_number_mspec_response_objects() const;
							
	bool operator==(const auction_rule &rhs);
	
	bool operator!=(const auction_rule &rhs); 
	
	auction_rule & operator=(const auction_rule &rhs);
	
	objectList_t * get_request_objects(void) { return &object_requests; }

	objectList_t * get_response_objects(void) { return &object_responses; }
	

  protected:
	
	/// Map for all mspec objects belonging to the rule to be used in the 
	/// auction application.
	objectList_t object_requests;

	/// Map for all mspec objects responses belonging to the rule to be used in the 
	/// auction application.
	objectList_t object_responses;

		
};

std::ostream &operator<<(std::ostream &out, const auction_rule &mpr);


} // namespace anslp

#endif // AUCTION_RULE_H
