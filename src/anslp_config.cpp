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
anslp_config::setRepository(configpar_repository* newcfp_rep)
{

	if (newcfp_rep == 0)
		throw  configParExceptionNoRepository();
	
	// register the realm
	cfgpar_rep= newcfp_rep;
	cfgpar_rep->registerRealm(anslp_realm, "anslp-nslp", anslpconf_maxparno);
	DLog("anslp_config", "registered anslp realm with realm id " << (int) anslp_realm);
	
	// now register all parameters
	registerAllPars();
}


void 
anslp_config::registerAllPars()
{
  DLog("anslp_config::registerAllPars", "starting registering anslp parameters.");

  // register all mnslp parameters now - They must be in the same order because authors use a vector.
  registerPar( new configpar<string>(anslp_realm, anslpconf_conffilename, "config", "configuration file name", true, "nsis-ka.conf") );
  registerPar( new configpar<uint32>(anslp_realm, anslpconf_dispatcher_threads, "dispatcher-threads", "number of dispatcher threads", true, 1) );
  registerPar( new configpar<bool>(anslp_realm, anslpconf_is_auctioneer, "as-is-auctioneer", "NE is auctioneer", false, false) );
  registerPar( new configpar<bool>(anslp_realm, anslpconf_install_auction_rules, "as-install-auction-rules", "ME install auction rules", false, false) );

  registerPar( new configpar<string>(anslp_realm, anslpconf_auctioneer_application, "as-auctioneer-application", "auctioneer application name", true, "auctionManager") );
  registerPar( new configpar<string>(anslp_realm, anslpconf_auctioneer_user, "as-auctioneer-user", "auctioneer application user", true, "admin") );
  registerPar( new configpar<string>(anslp_realm, anslpconf_auctioneer_password, "as-auctioneer-password", "auctioneer user password", true, "admin") );  
  registerPar( new configpar<string>(anslp_realm, anslpconf_auctioneer_server, "as-auctioneer-server", "auctioneer http server", true, "localhost") );
  registerPar( new configpar<string>(anslp_realm, anslpconf_auctioneer_def_xsl, "as-auctioneer-def-xsl", "auctioneer results decoding", true, DEF_SYSCONFDIR "/reply2.xsl") );
  registerPar( new configpar<uint32>(anslp_realm, anslpconf_auctioneer_port, "as-auctioneer-port", "auctioneer port", true, 12244) );

  registerPar( new configpar<uint32>(anslp_realm, anslpconf_ni_session_lifetime, "ni-session-lifetime", "NI session lifetime in seconds", true, 30, "s") );
  registerPar( new configpar<uint32>(anslp_realm, anslpconf_ni_response_timeout, "ni-response-timeout", "NI response timeout", true, 2, "s") );
  registerPar( new configpar<uint32>(anslp_realm, anslpconf_ni_max_session_lifetime, "ni-max-session-lifetime", "NI session lifetime in seconds", true, 30, "s") );
  registerPar( new configpar<uint32>(anslp_realm, anslpconf_ni_max_retries, "ni-max-retries", "NI max retries", true, 3) );
  registerPar( new configpar<uint32>(anslp_realm, anslpconf_ni_msg_hop_count, "ni-msg-hop-count", "NI Message Hop Count", true, 20, "s") );  

  registerPar( new configpar<string>(anslp_realm, anslpconf_ni_user, "ni-user", "bid user", true, "admin") );
  registerPar( new configpar<string>(anslp_realm, anslpconf_ni_password, "ni-password", "bid user password", true, "admin") );  
  registerPar( new configpar<string>(anslp_realm, anslpconf_ni_server, "ni-server", "bid http server", true, "localhost") );
  registerPar( new configpar<uint32>(anslp_realm, anslpconf_ni_port, "ni-port", "bid port", true, 12248) );

  registerPar( new configpar<uint32>(anslp_realm, anslpconf_nf_max_session_lifetime, "nf-max-session-lifetime", "NF max session lifetime in seconds", true, 60, "s") );
  registerPar( new configpar<uint32>(anslp_realm, anslpconf_nf_response_timeout, "nf-response-timeout", "NF response timeout", true, 2, "s") );
  registerPar( new configpar<bool>(anslp_realm, anslpconf_nf_is_edge, "nf-is-edge", "NF is an ege", false, false) );

  registerPar( new configpar<uint32>(anslp_realm, anslpconf_nr_max_session_lifetime, "nr-max-session-lifetime", "NR max session lifetime in seconds", true, 60, "s") );
  
  DLog("anslp_config::registerAllPars", "finished registering anslp parameters.");
}
