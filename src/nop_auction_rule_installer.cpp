/// ----------------------------------------*- mode: C++; -*--
/// @file nop_auction_rule_installer.cpp
/// The nop_auction_rule_installer class.
/// ----------------------------------------------------------
/// $Id: nop_auction_rule_installer.cpp 2558 2015-09-03 08:23:00 amarentes $
/// $HeadURL: https://./src/nop_auction_rule_installer.cpp $
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

#include "address.h"
#include "logfile.h"

#include "nop_auction_rule_installer.h"


using namespace anslp;
using namespace protlib::log;


#define LogError(msg) Log(ERROR_LOG, LOG_NORMAL, \
	"nop_auction_rule_installer", msg)
#define LogWarn(msg) Log(WARNING_LOG, LOG_NORMAL, \
	"nop_auction_rule_installer", msg)
#define LogInfo(msg) Log(INFO_LOG, LOG_NORMAL, \
	"nop_auction_rule_installer", msg)
#define LogDebug(msg) Log(DEBUG_LOG, LOG_NORMAL, \
	"nop_auction_rule_installer", msg)

#define LogUnimp(msg) Log(ERROR_LOG, LOG_UNIMP, "nop_auction_rule_installer", \
	msg << " at " << __FILE__ << ":" << __LINE__)



nop_auction_rule_installer::nop_auction_rule_installer(
		anslp_config *conf) throw () 
		: auction_rule_installer(conf) 
{

	// nothing to do
}


nop_auction_rule_installer::~nop_auction_rule_installer() throw () 
{
	// nothing to do
}


void 
nop_auction_rule_installer::setup() throw (auction_rule_installer_error) 
{

	auction_rule_installer::setup();
	
	LogDebug("NOP: setup()");

}


void 
nop_auction_rule_installer::check(const string sessionId, const msg::anslp_mspec_object *object)
		throw (auction_rule_installer_error) 
{

	LogDebug("NOP: check()");
}


auction_rule * 
nop_auction_rule_installer::create(const string sessionId, const auction_rule *rule) 
{

	LogDebug("NOP: installing policy rule " << *rule);
	auction_rule *rule_return;
	
	if ( rule != NULL ){
		rule_return = rule->copy(); 
		
		objectListConstIter_t i;
		for ( i = rule_return->get_request_objects()->begin(); 
					i != rule_return->get_request_objects()->end(); i++)
		{
			rule_return->set_response_object((i->second)->copy());
		}
	}
	else{
		rule_return = NULL;
	}	
	
	return rule_return;
}


auction_rule * 
nop_auction_rule_installer::put_response(const string sessionId, const auction_rule *rule) 
{

	LogDebug("NOP: put response " << *rule);
	auction_rule *rule_return;
	
	if ( rule != NULL ){
		rule_return = rule->copy(); 
		
		objectListConstIter_t i;
		for ( i = rule_return->get_request_objects()->begin(); 
					i != rule_return->get_request_objects()->end(); i++)
		{
			rule_return->set_response_object((i->second)->copy());
		}
	}
	else{
		rule_return = NULL;
	}	
	
	return rule_return;
}


auction_rule * nop_auction_rule_installer::remove(const string sessionId, const auction_rule *rule) 
{

	LogDebug("NOP: removing auction rule " << *rule);
	auction_rule *rule_return;
	
	if ( rule != NULL ){
		rule_return = rule->copy(); 
	}
	else{
		rule_return = NULL;
	}	

	return rule_return;
}

bool nop_auction_rule_installer::remove_all() 
{

	LogDebug("NOP: removing all metering auction rules ");
	return true;
}

auction_rule * 
nop_auction_rule_installer::auction_interaction(const string sessionId, const auction_rule *rule)
{

	LogDebug("NOP: auction interaction " << *rule);
	auction_rule *rule_return;
	
	if ( rule != NULL ){
		rule_return = rule->copy(); 
		
		objectListConstIter_t i;
		for ( i = rule_return->get_request_objects()->begin(); 
					i != rule_return->get_request_objects()->end(); i++)
		{
			rule_return->set_response_object((i->second)->copy());
		}
	}
	else{
		rule_return = NULL;
	}	
	
	return rule_return;

}


// EOF
