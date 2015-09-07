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
	
	objectListIter_t it;
	for ( it = objects.begin(); it != objects.end(); it++)
	{
		delete(it->second);
	}
}

/**
 * Copy constructor.
 */
auction_rule::auction_rule(const auction_rule &rhs)
{
	
	LogDebug("Starting constructor from another instance");
	
	std::map<mspec_rule_key, msg::anslp_mspec_object *>::const_iterator it;
	for ( it = rhs.objects.begin(); it != rhs.objects.end(); it++ )
	{
		const mspec_rule_key id = it->first;
		const msg::anslp_mspec_object *obj = it->second;
		if (obj){
			set_object(id, obj->copy());
		}
	}
	
	// Copy rules keys.
	ruleKeyConstIterList_t it_rules;
	for ( it_rules = rhs.rule_keys.begin(); it_rules != rhs.rule_keys.end(); it_rules++ )
	{
		const mspec_rule_key id = it_rules->first;
		std::vector<std::string> rules = it_rules->second;
		set_commands(id, rules);
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
auction_rule::set_object(mspec_rule_key key, msg::anslp_mspec_object *obj)
{
	
	LogDebug("Starting set_object - Params: key, object");
	
	if ( obj == NULL )
		return;

	msg::anslp_mspec_object *old = objects[key];

	if ( old )
		delete old;

	objects[key] = obj;
}

mspec_rule_key 
auction_rule::set_object(msg::anslp_mspec_object *obj)
{
	LogDebug("Starting set_object - Params: object");
	
	if (obj != NULL){
		mspec_rule_key key;
		objects[key] = obj;
		return key;
	}
	else{
		throw std::invalid_argument( "received a NULL object" );
	}
}

size_t
auction_rule::get_number_mspec_objects()
{
	return objects.size();
}

bool
auction_rule::operator==(const auction_rule &rhs)
{
	if (objects.size() != rhs.objects.size()){
		return false;
	}
	
	// All objects have to be identical.
	objectListConstIter_t i;
	for ( i = objects.begin(); i != objects.end(); i++ ) {
		objectListConstIter_t j = rhs.objects.find(i->first);
		if ( j == rhs.objects.end() ){
			return false;
		}
			
		if ( (i->second)->notEqual(*(j->second))  ){
			return false;
		}
	}	

	// Verify rule keys.
	if (rule_keys.size() != rhs.rule_keys.size())
		return false;
	
	ruleKeyConstIterList_t it;
	for ( it = rule_keys.begin(); it != rule_keys.end(); it++ ) {
		ruleKeyConstIterList_t ij = rhs.rule_keys.find(it->first);
		if ( ij == rhs.rule_keys.end() )
			return false;
		
		if ( are_equal(it->second, ij->second) == false )
			return false;	

	}
	return true;
}

bool 
auction_rule::are_equal(std::vector<std::string> left, std::vector<std::string> right)
{
	std::sort (left.begin(), left.end());
	std::sort (right.begin(), right.end());
	for ( int index = 0; index < rule_keys.size(); index ++ ) {
				
		if ( left[index].compare(right[index]) != 0 ){
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
	for ( it = rhs.objects.begin(); it != rhs.objects.end(); it++ )
	{
		const mspec_rule_key id = it->first;
		const msg::anslp_mspec_object *obj = it->second;
		if (obj){
			set_object(id, obj->copy());
		}
	}
	
	ruleKeyConstIterList_t it_rule_keys;
	for ( it_rule_keys = rhs.rule_keys.begin(); it_rule_keys != rhs.rule_keys.end(); it_rule_keys++)
	{
		mspec_rule_key id = it_rule_keys->first;
		std::vector<std::string> list_keys = it_rule_keys->second;
		set_commands(id, list_keys);
	}
	
	return *this;
}

void auction_rule::set_commands(mspec_rule_key key, std::vector<std::string> commands)
{
	rule_keys.insert(std::pair<mspec_rule_key, std::vector<std::string> > (key, commands));

}

void auction_rule::clear_commands()
{
	rule_keys.clear();
}

size_t auction_rule::get_number_rule_keys()
{
	return rule_keys.size();
}

size_t auction_rule::get_number_rule_keys() const
{
	return rule_keys.size();
}

}

// EOF
