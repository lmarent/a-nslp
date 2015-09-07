/// ----------------------------------------*- mode: C++; -*--
/// @file anslp_config.cpp
/// Configuration for a ANSLP instance.
/// ----------------------------------------------------------
/// $Id: anslp_config.cpp 4118 2015-08-31 10:58:00 amarentes $
/// $HeadURL: https://./anslp_config.cpp $
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

#include "anslp_config.h"

using namespace anslp;

void
anslp_config::repository_init() 
{
  DLog("anslp_config", "start - creating configuration parameter singleton");
  configpar_repository::create_instance(anslp::anslp_realm+1);
} // end namespace



/** sets the repository pointer and registers all parameters 
 *  (not done in constructor, because of allowing simulation hosts to initialize later)
 **/ 
void
anslp_config::setRepository(configpar_repository* cfp_rep)
{
	cfgpar_rep= cfp_rep;

	if (cfgpar_rep == 0)
		throw  configParExceptionNoRepository();
	
	// register the realm
	cfgpar_rep->registerRealm(anslp_realm, "a-nslp", anslpconf_maxparno);
	DLog("anslp_config", "registered mnslp realm with realm id " << (int) anslp_realm);
	
	// now register all parameters
	registerAllPars();
}


void 
anslp_config::registerAllPars()
{
  DLog("anslp_config::registerAllPars", "starting registering anslp parameters.");

  // register all mnslp parameters now
  registerPar( new configpar<string>(anslp_realm, anslpconf_conffilename, "config", "configuration file name", true, "nsis-ka.conf") );
  registerPar( new configpar<uint32>(anslp_realm, anslpconf_dispatcher_threads, "dispatcher-threads", "number of dispatcher threads", true, 1) );
  registerPar( new configpar<uint32>(anslp_realm, anslpconf_ni_max_session_lifetime, "ni-max-session-lifetime", "NI session lifetime in seconds", true, 30, "s") );
  registerPar( new configpar<uint32>(anslp_realm, anslpconf_ni_max_retries, "ni-max-retries", "NI max retries", true, 3) );
  registerPar( new configpar<uint32>(anslp_realm, anslpconf_ni_response_timeout, "ni-response-timeout", "NI response timeout", true, 2, "s") );
  registerPar( new configpar<uint32>(anslp_realm, anslpconf_ni_msg_hop_count, "ni-msg-hop-count", "NI Message Hop Count", true, 20, "s") );  
  registerPar( new configpar<uint32>(anslp_realm, anslpconf_nf_max_session_lifetime, "nf-max-session-lifetime", "NF max session lifetime in seconds", true, 60, "s") );
  registerPar( new configpar<uint32>(anslp_realm, anslpconf_nf_response_timeout, "nf-response-timeout", "NF response timeout", true, 2, "s") );
  registerPar( new configpar<bool>(anslp_realm, anslpconf_is_auctioneer, "is-auctioneer", "NE is auctioneer", true, true) );
  registerPar( new configpar<bool>(anslp_realm, anslpconf_install_auction_rules, "install-auction-rules", "ME install auction rules", true, true) );
  registerPar( new configpar<string>(anslp_realm, anslpconf_export_config_file, "export-config-file", "export configuration file name", true, "/home/luis/NSIS/nsis-ka-0.97/etc/export.xml") );
  registerPar( new configpar<string>(anslp_realm, anslpconf_configuration_file, "filter-config-file", "configuration file name", true, "/home/luis/NSIS/nsis-ka-0.97/etc/configuration.xml") );
  registerPar( new configpar<string>(anslp_realm, anslpconf_auctioning_application, "auctioning-application", "auctioning applicartion name", true, "netmate") );
  registerPar( new configpar<string>(anslp_realm, anslpconf_auctioneer_user, "auctioneer-user", "auctioneer application user", true, "admin") );
  registerPar( new configpar<string>(anslp_realm, anslpconf_auctioneer_password, "auctioneer-password", "auctioneer user password", true, "admin") );  
  registerPar( new configpar<string>(anslp_realm, anslpconf_auctioneer_server, "auctioneer-server", "auctioneer http server", true, "localhost") );
  registerPar( new configpar<string>(anslp_realm, anslpconf_auctioneer_def_xsl, "auctioneer-def-xsl", "auctioneer results decoding", true, "/home/luis/NSIS/nsis-ka-0.97/etc/reply2.xsl") );
  registerPar( new configpar<uint32>(anslp_realm, anslpconf_auctioneer_port, "auctioneer-port", "auctioneer port", true, 12244) );
  registerPar( new configpar<string>(anslp_realm, anslpconf_export_directory, "export-directory", "export directory", true, "/tmp/") );

  registerPar( new configpar<uint32>(anslp_realm, anslpconf_nr_max_session_lifetime, "nr-max-session-lifetime", "NR max session lifetime in seconds", true, 60, "s") );
  
  DLog("anslp_config::registerAllPars", "finished registering mnslp parameters.");
}
