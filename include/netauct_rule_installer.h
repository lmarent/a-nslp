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
#ifndef NETAUCT_RULE_INSTALLER_H
#define NETAUCT_RULE_INSTALLER_H


#include "auction_rule_installer.h"

namespace anslp 
{

/**
 * An auction rule installer which defines the methods to implement.
 *
 * This implementation is Linux-specific and uses the AuctionManager library to
 * install and remove auction rules.
 */
class netauct_rule_installer : public auction_rule_installer 
{
	
  public:

	netauct_rule_installer(anslp_config *conf) throw ();

	virtual ~netauct_rule_installer() throw ();

	void setup() throw (auction_rule_installer_error);

	//! Verifies that the session request is complete to create the session request.
	virtual void check(const msg::anslp_mspec_object *object)
		throw (auction_rule_installer_error);

	//! Create a new auction session in an auction manager server.
	virtual auction_rule * create(const auction_rule *mt_object);

	//! This method is used every time that any actor wants to post or push
	//! Information for an auction.
	virtual auction_rule * auction_interaction(const auction_rule *mt_object);

	//! Remove an auction session in an auction server.
	virtual auction_rule * remove(const auction_rule *mt_object);

	//! Remove all auction sessions in an auction server.
	virtual bool remove_all();

  private:
	
	//! Cast the object to the ipap_message.
	const msg::anslp_ipap_message * get_ipap_message(const msg::anslp_mspec_object *object);
	
	//! Creates a connection to the auction manager server and execute 
	//! the requested command.
	string execute_command(string action, string post_fields);

	bool responseOk(string response);
	
	int getNumberAuctions(string response);

};

} // namespace anslp


#endif // NETAUCT_RULE_INSTALLER_H
