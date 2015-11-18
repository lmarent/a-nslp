/// ----------------------------------------*- mode: C++; -*--
/// @file nop_auction_rule_installer.h
/// The fake auction_rule_installer classes.
/// ----------------------------------------------------------
/// $Id: nop_auction_rule_installer.h 2558 2015-09-03 08:38:00 amarentes $
/// $HeadURL: https://./include/nop_auction_rule_installer.h $
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
#ifndef NOP_AUCTION_RULE_INSTALLER_H
#define NOP_AUCTION_RULE_INSTALLER_H


#include "auction_rule_installer.h"

namespace anslp 
{

/**
 * An auction rule installer which does nothing.
 *
 * Basically, all its methods are no-ops, but the requested actions are logged.
 */
class nop_auction_rule_installer : public auction_rule_installer 
{
	
  public:

	nop_auction_rule_installer(anslp_config *conf) throw ();

	virtual ~nop_auction_rule_installer() throw ();

	void setup() throw (auction_rule_installer_error);

	virtual void check(const string sessionId, const msg::anslp_mspec_object *object)
		throw (auction_rule_installer_error);

	virtual auction_rule * create(const string sessionId, const auction_rule *mt_object);
	
	virtual auction_rule * put_response(const string sessionId, const auction_rule * mt_object);
	
	virtual auction_rule * auction_interaction(const bool server, const string sessionId, const auction_rule *mt_object);

	virtual auction_rule * remove(const string sessionId, const auction_rule *mt_object);

	virtual bool remove_all();


};

} // namespace anslp


#endif // NOP_AUCTION_RULE_INSTALLER_H
