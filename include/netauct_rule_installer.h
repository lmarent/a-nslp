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
#include "aqueue.h"

namespace anslp 
{


typedef enum 
{
    RULE_INSTALLER_SERVER = 0, 
    RULE_INSTALLER_CLIENT
} rule_installer_destination_type_t;

/**
 * An auction rule installer which defines the methods to implement.
 *
 * This implementation is Linux-specific and uses the AuctionManager library to
 * install and remove auction rules.
 */
class netauct_rule_installer : public auction_rule_installer 
{
	
  public:

	netauct_rule_installer(anslp_config *conf, FastQueue *installQueue, bool test=false) throw ();

	virtual ~netauct_rule_installer() throw ();

	void setup() throw (auction_rule_installer_error);

	//! Verifies that the session request is complete to create the session request.
	virtual void check(const string sessionId, 
					   std::vector<msg::anslp_mspec_object *> &missing_objects)
		throw (auction_rule_installer_error);

	//! Create a new auction session in an auction manager server.
	virtual void create(const string sessionId, const auction_rule *mt_object);

	//! put the responses that come to the client.
	virtual void put_response(const string sessionId, const auction_rule * mt_object);

	//! This method is used every time that any actor wants to post or push
	//! Information for an auction.
	virtual auction_rule * auction_interaction(const bool server, const string sessionId, const auction_rule *mt_object);

	//! Remove an auction session in an auction server.
	virtual auction_rule * remove(const string sessionId, const auction_rule *mt_object);

	//! Remove all auction sessions in an auction server.
	virtual bool remove_all();

  protected:

	//! This function puts the objects in rule for processing in an auctioning server or 
	//! auctioner user agent.
	void handle_create_session(const string sessionId, const auction_rule *rule);

	//! This function puts the objects in rule for removing in an auctioning server or 
	//! auctioner user agent.
	auction_rule * handle_remove_session(const string sessionId, const auction_rule *rule);

	//! This function throws an exception when there are not returning objects 
	//! (there is not any auction satisfaying given criteria).
	void handle_response_check(anslp::FastQueue *waitqueue) 
			throw (auction_rule_installer_error);
  
	//! This function throws an exception when there is not an event in queue 
	//! (update auc_return with the number of objects installed).
	void
	handle_response_remove(anslp::FastQueue *waitqueue, auction_rule *auc_return);

  private:
	
	//! Cast the object to the ipap_message.
	const msg::anslp_ipap_message * get_ipap_message(const msg::anslp_mspec_object *object);
	
	//! Creates a connection to the auction manager server and execute 
	//! the requested command.
	string execute_command(rule_installer_destination_type_t destination, 
								string action, string post_fields);

	anslp::FastQueue * getQueue(){ return installQueue; }
		
	bool responseOk(string response);
	
	int getNumberAuctions(string response);
	
	string getMessage(string response);
	
	FastQueue *installQueue;
	
	bool test;
};

} // namespace anslp


#endif // NETAUCT_RULE_INSTALLER_H
