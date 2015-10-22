/// ----------------------------------------*- mode: C++; -*--
/// @file auction_rule.cpp
/// The auction rule classes.
/// ----------------------------------------------------------
/// $Id: auction_rule.cpp 3166 2015-09-03 15:13:00 amarentes $
/// $HeadURL: https://./src/auction_rule.cpp $
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
#include <assert.h>

#include "logfile.h"
#include "auction_rule.h"
#include "session.h"
#include "ie.h"

using namespace protlib::log;

#define LogError(msg) ERRLog("Auction_Rule", msg)
#define LogWarn(msg) WLog("Auction_Rule", msg)
#define LogInfo(msg) ILog("Auction_Rule", msg)
#define LogDebug(msg) DLog("Auction_Rule", msg)


namespace anslp{

/*****************************************************************************
 *
 * The auction_rule class.
 *
 *****************************************************************************/

/**
 * Constructor.
 */
auction_rule::auction_rule()
{
	LogDebug("Starting constructor auction_rule");

	// nothing to do
}

auction_rule::~auction_rule()
{
	LogDebug("Starting destructor auction_rule");
	
	// Delete requests
	objectListIter_t it;
	
	LogDebug("Starting destructor object_request:" << object_requests.size());
	
	for ( it = object_requests.begin(); it != object_requests.end(); it++)
	{
		LogDebug("Entro 2" << (it->first).to_string());
		if (it->second != NULL)
			delete(it->second);
	}

	LogDebug("Starting destructor object_responses:" << object_responses.size());
	
	// Delete responses.
	for ( it = object_responses.begin(); it != object_responses.end(); it++)
	{
		LogDebug("Entro 2" << (it->first).to_string());
		if (it->second != NULL){
			delete(it->second);
		}
	}
	
	LogDebug("Ending destructor auction_rule");
}

/**
 * Copy constructor.
 */
auction_rule::auction_rule(const auction_rule &rhs)
{
	
	LogDebug("Starting constructor from another instance");
	
	// Copy object request
	std::map<mspec_rule_key, msg::anslp_mspec_object *>::const_iterator it;
	for ( it = rhs.object_requests.begin(); it != rhs.object_requests.end(); it++ )
	{
		const mspec_rule_key id = it->first;
		const msg::anslp_mspec_object *obj = it->second;
		if (obj){
			set_request_object(id, obj->copy());
		}
	}
	
	// Copy rules responses.
	for ( it = rhs.object_responses.begin(); it != rhs.object_responses.end(); it++ )
	{
		const mspec_rule_key id = it->first;
		const msg::anslp_mspec_object *obj = it->second;
		if (obj){
			set_response_object(id, obj->copy());
		}
	}	
}

/**
 * Create a deep copy of this object.
 */
auction_rule *
auction_rule::copy() const 
{
	LogDebug("Starting copy");
	
	auction_rule *q = NULL;
	q = new auction_rule(*this);
	
	return q;
}

void 
auction_rule::set_request_object(mspec_rule_key key, msg::anslp_mspec_object *obj)
{
	
	LogDebug("Starting set_request_object - Params: key, object");
	
	if ( obj == NULL )
		return;

	msg::anslp_mspec_object *old = object_requests[key];

	if ( old )
		delete old;

	object_requests[key] = obj;
}

void 
auction_rule::set_response_object(mspec_rule_key key, msg::anslp_mspec_object *obj)
{
	
	LogDebug("Starting set_response_object - Params: key, object");
	
	if ( obj == NULL )
		return;

	msg::anslp_mspec_object *old = object_responses[key];

	if ( old )
		delete old;

	object_responses[key] = obj;
	
	LogDebug("Ending set_response_object");
}

mspec_rule_key 
auction_rule::set_request_object(msg::anslp_mspec_object *obj)
{
	LogDebug("Starting set_request_object - Params: object");
	
	if (obj != NULL){
		mspec_rule_key key;
		object_requests[key] = obj;
		return key;
	}
	else{
		throw std::invalid_argument( "received a NULL object" );
	}
}

mspec_rule_key 
auction_rule::set_response_object(msg::anslp_mspec_object *obj)
{
	LogDebug("Starting set_response_object - Params: object");
	
	if (obj != NULL){
		mspec_rule_key key;
		object_requests[key] = obj;
		return key;
	}
	else{
		throw std::invalid_argument( "received a NULL object" );
	}
}

size_t
auction_rule::get_number_mspec_request_objects()
{
	return object_requests.size();
}

size_t
auction_rule::get_number_mspec_request_objects() const
{
	return object_requests.size();
}

size_t
auction_rule::get_number_mspec_response_objects()
{
	return object_responses.size();
}

size_t
auction_rule::get_number_mspec_response_objects() const
{
	return object_responses.size();
}


bool
auction_rule::operator==(const auction_rule &rhs)
{

	// All object requests have to be identical.
	if (object_requests.size() != rhs.object_requests.size()){
		return false;
	}
	
	objectListConstIter_t i;
	for ( i = object_requests.begin(); i != object_requests.end(); i++ ) {
		objectListConstIter_t j = rhs.object_requests.find(i->first);
		if ( j == rhs.object_requests.end() ){
			return false;
		}
			
		if ( (i->second)->notEqual(*(j->second))  ){
			return false;
		}
	}	


	// All object responses have to be identical.
	if (object_responses.size() != rhs.object_responses.size()){
		return false;
	}
	
	for ( i = object_responses.begin(); i != object_responses.end(); i++ ) {
		objectListConstIter_t j = rhs.object_responses.find(i->first);
		if ( j == rhs.object_responses.end() ){
			return false;
		}
			
		if ( (i->second)->notEqual(*(j->second))  ){
			return false;
		}
	}	

	return true;
}


bool
auction_rule::operator!=(const auction_rule &rhs) 
{
	return !(operator==(rhs));
}

std::ostream &
operator<<(std::ostream &out, const auction_rule &r) 
{
	// TODO AM: implement this function.
}

auction_rule & 
auction_rule::operator=(const auction_rule &rhs)
{
	std::map<mspec_rule_key, msg::anslp_mspec_object *>::const_iterator it;
	for ( it = rhs.object_requests.begin(); it != rhs.object_requests.end(); it++ )
	{
		const mspec_rule_key id = it->first;
		const msg::anslp_mspec_object *obj = it->second;
		if (obj){
			set_request_object(id, obj->copy());
		}
	}
	

	for ( it = rhs.object_responses.begin(); it != rhs.object_responses.end(); it++ )
	{
		const mspec_rule_key id = it->first;
		const msg::anslp_mspec_object *obj = it->second;
		if (obj){
			set_response_object(id, obj->copy());
		}
	}
	
	return *this;
}


}

// EOF
