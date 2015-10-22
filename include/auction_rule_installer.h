/// ----------------------------------------*- mode: C++; -*--
/// @file auction_rule_installer.h
/// Auction rule installer class.
/// ----------------------------------------------------------
/// $Id: auction_rule_installer.h 2558 2015-09-01 14:57:00 amarentes $
/// $HeadURL: https://./include/auction_rule_installer.h $
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
#ifndef AUCTION_RULE_INSTALLER_H
#define AUCTION_RULE_INSTALLER_H


#include "session.h"
#include "anslp_config.h"

#include "auction_rule.h"
//#include "policy_application_configuration_container.h"


namespace anslp {

/**
 * An exception to be thrown if the auction_rule_installer failed.
 */
class auction_rule_installer_error : public request_error {
	
  public:
	
	auction_rule_installer_error(const std::string &msg,
		uint8 severity=0, uint8 response_code=0) throw ()
		: request_error(msg, severity, response_code) { }
		
	virtual ~auction_rule_installer_error() throw () { }
};

inline std::ostream &operator<<(
		std::ostream &out, const auction_rule_installer_error &e) {

	return out << e.get_msg();
}


/**
 * An abstract interface to the operating system's Metering package.
 *
 * Callers pass Metering policy rules to install() and this class
 * maps it to operating system specific rules. The installed rules can later
 * be removed using remove().
 *
 * Subclasses have to implement locking if necessary.
 */
class auction_rule_installer {

  public:
  
	auction_rule_installer(anslp_config *conf) throw () : 
		config(conf) { }
	
	virtual ~auction_rule_installer() throw ();

	/**
	 * Prepare the external AUCTIONING package for usage.
	 */
	virtual void setup() throw (auction_rule_installer_error);

	/**
	 * Check if the given policy rule is correct and supported.
	 *
	 * If they are not, an exception is thrown.
	 */
	virtual void check(const string sessionId, const msg::anslp_mspec_object *object)
		throw (auction_rule_installer_error) = 0;

	/**
	 * Create the given auction session.
	 * 
	 * Subclasses have to use operating system dependent code to access the
	 * local auctioning application. 
	 * After calling this method, the auction session is created in 
	 * the auctioning application and nodes can start the auction process.
	 */
	virtual auction_rule * create(const string sessionId, const auction_rule *mt_object) = 0;

	/**
	 * Remove the given auction session.
	 *
	 * After calling this method, the previously created sessions are no
	 * longer in effect.
	 *
	 * Note that this is the reverse operation to create(). Only previously
	 * created auction sessions may be deleted!. 
	 */
	virtual auction_rule * remove(const string sessionId, const auction_rule * mt_object) = 0;

	/**
	 * executes an auction interaction between two parties.
	 *	 
	 * Subclasses have to use operating system dependent code to access the
	 * local auctioning application. 
	 *
	 * After calling this method two nodes have exchanged a ipap_message. 
	 */
	virtual auction_rule * auction_interaction(const auction_rule *mt_object) = 0;

	virtual bool remove_all() = 0;
	
	std::string get_auctioning_application(){ return config->get_auctioning_application(); }

	std::string get_auctioning_application() const { return config->get_auctioning_application(); }
		
	std::string get_user() const { return config->get_user(); }
	
	std::string get_password() const { return config->get_password(); } 
	
	std::string get_server() const { return config->get_auctioneer_server(); } 
	
	std::string get_xsl() const { return config->get_auctioneer_xsl(); } 
	
	uint32 get_port() const { return config->get_auctioneer_port(); } 
		
	std::string to_string() const;
  
  private:
  
	anslp_config *config;
	//policy_application_configuration_container * app_container;

	/**
	* parse_export_definition_file:
	* @filename: the file name to parse
	*
	* Parse, validate and load information about the export configuration.
	*/
	void parse_export_definition_file(const char *filename) throw (auction_rule_installer_error);

	/**
	* parse_filter_definition_file:
	* @filename: the file name to parse
	*
	* Parse, validate and load information about the filter configuration.
	*/
	void parse_configuration_definition_file(const char *filename) throw (auction_rule_installer_error);
	
	
	
};

} // namespace anslp

#include "nop_auction_rule_installer.h"
#include "netauct_rule_installer.h"


#endif // AUCTION_RULE_INSTALLER_H
