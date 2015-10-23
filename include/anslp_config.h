/// ----------------------------------------*- mode: C++; -*--
/// @file anslp_config.h
/// This file defines constants used throughout this implementation.
/// ----------------------------------------------------------
/// $Id: anslp_config.h 4118 2015-08-31 12:04:00Z amarentes $
/// $HeadURL: https://./include/anslp_config.h $
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
#ifndef ANSLP_ANSLP_CONFIG_H
#define ANSLP_ANSLP_CONFIG_H

#include "config.h"
#include "messages.h"
#include "address.h"
#include "configpar.h"
#include "configpar_repository.h"

// since we re-use some GIST parameter, we need to define them here
#include "gist_conf.h"

namespace anslp {
  using namespace protlib;

  // 0 = global realm, 1 = protlib_realm, 2 = gist_realm 3 = qos_nslp_realm 
  // 4 = natfw_realm 5= mnslp_realm 6= anslp_realm

  const realm_id_t anslp_realm = 6;


  enum anslp_configpar_id_t {
    anslpconf_invalid,
    anslpconf_conffilename,
    anslpconf_dispatcher_threads,
    anslpconf_is_auctioneer,
    anslpconf_install_auction_rules,    
    
    /* Auctioneer application parameters */
    anslpconf_auctioneer_application,
	anslpconf_auctioneer_user,
	anslpconf_auctioneer_password,
	anslpconf_auctioneer_server,
	anslpconf_auctioneer_def_xsl,
	anslpconf_auctioneer_port,
    
    /* NI  */
    anslpconf_ni_session_lifetime,
    anslpconf_ni_response_timeout,
    anslpconf_ni_max_session_lifetime,
    anslpconf_ni_max_retries,
    anslpconf_ni_msg_hop_count,
    anslpconf_ni_user,
	anslpconf_ni_password,
	anslpconf_ni_server,			
	anslpconf_ni_port,					

    /* NF  */
    anslpconf_nf_max_session_lifetime,
    anslpconf_nf_response_timeout,
    anslpconf_nf_is_edge,
    /* NR  */
    anslpconf_nr_max_session_lifetime,
    anslpconf_maxparno
  };


/**
 * The central configuration point for a ANSLP instance.
 */
class anslp_config {

  public:
	anslp_config(configpar_repository *cfpgar_rep= NULL) : cfgpar_rep(cfpgar_rep) {};
	
	void repository_init();

	void setRepository(configpar_repository* cfp_rep= configpar_repository::instance());

	/// register copy of configuration parameter instance
	void registerPar(const configparBase& configparid) { cfgpar_rep->registerPar(configparid); }
	
	/// register instance configuration parameter directly
	void registerPar(configparBase* configparid) { cfgpar_rep->registerPar(configparid);  }

	// these are just convenience functions
	template <class T> void setpar(anslp_configpar_id_t configparid, const T& value);
	
	template <class T> T getpar(anslp_configpar_id_t configparid) const;
	
	template <class T> T& getparref(anslp_configpar_id_t configparid) const;
	
	string getparname(anslp_configpar_id_t configparid);

    bool has_ipv4_address() const { 
		return ntlp::gconf.getparref<hostaddresslist_t>(ntlp::gistconf_localaddrv4).size() > 0; }
    
	const hostaddresslist_t &get_ipv4_addresses() const {
		return ntlp::gconf.getparref<hostaddresslist_t>(ntlp::gistconf_localaddrv4); }

    bool has_ipv6_address() const { 
		return ntlp::gconf.getparref<hostaddresslist_t>(ntlp::gistconf_localaddrv6).size() > 0; }
    
	const hostaddresslist_t &get_ipv6_addresses() const {
		return ntlp::gconf.getparref<hostaddresslist_t>(ntlp::gistconf_localaddrv6); }

    uint16 get_gist_port_udp() const { 
		return ntlp::gconf.getpar<uint16>(ntlp::gistconf_udpport); }
    
	uint16 get_gist_port_tcp() const { 
		return ntlp::gconf.getpar<uint16>(ntlp::gistconf_tcpport); }
	
	uint16 get_gist_port_tls() const { 
		return ntlp::gconf.getpar<uint16>(ntlp::gistconf_tlsport); }

	uint32 get_num_dispatcher_threads() const {
		return getpar<uint32>(anslpconf_dispatcher_threads); }

	bool is_auctioneer() const { return getpar<bool>(anslpconf_is_auctioneer); }
	
	string get_auctioning_application() const { 
		return getpar<string>(anslpconf_auctioneer_application); }
			
	string get_user() const {
		return getpar<string>(anslpconf_auctioneer_user); }
	
	string get_password() const {
		return getpar<string>(anslpconf_auctioneer_password); }

	string get_auctioneer_server() const {
		return getpar<string>(anslpconf_auctioneer_server); }

	string get_auctioneer_xsl() const {
		return getpar<string>(anslpconf_auctioneer_def_xsl); }

	uint32 get_auctioneer_port() const {
		return getpar<uint32>(anslpconf_auctioneer_port); }

	string get_bid_user() const {
		return getpar<string>(anslpconf_ni_user); }
	
	string get_bid_password() const {
		return getpar<string>(anslpconf_ni_password); }

	string get_bid_server() const {
		return getpar<string>(anslpconf_ni_server); }

	uint32 get_bid_port() const {
		return getpar<uint32>(anslpconf_ni_port); }

    uint32 get_ni_session_lifetime() const { 
		return getpar<uint32>(anslpconf_ni_max_session_lifetime); }
        
	uint32 get_ni_max_retries() const { 
		return getpar<uint32>(anslpconf_ni_max_retries); }
    
    uint32 get_ni_msg_hop_count() const {
		return getpar<uint32>(anslpconf_ni_msg_hop_count); }
    
    uint32 get_ni_response_timeout() const { 
		return getpar<uint32>(anslpconf_ni_response_timeout); }

	uint32 get_nf_max_session_lifetime() const { 
	  return getpar<uint32>(anslpconf_nf_max_session_lifetime); }
	  
	uint32 get_nf_response_timeout() const { 
	  return getpar<uint32>(anslpconf_nf_response_timeout); }

	bool is_nf_edge() const {
	  return getpar<bool>(anslpconf_nf_is_edge); }
	
	bool get_install_auction_rules() const {
		return getpar<bool>(anslpconf_install_auction_rules); }

	uint32 get_nr_max_session_lifetime() const {
		return getpar<uint32>(anslpconf_nr_max_session_lifetime); }
		
	/// The ID of the queue that receives messages from the NTLP.
	static const message::qaddr_t INPUT_QUEUE_ADDRESS
		= message::qaddr_api_3;

	/// The ID of the queue inside the NTLP that we send messages to.
	static const message::qaddr_t OUTPUT_QUEUE_ADDRESS
		= message::qaddr_coordination;

	/// The timer module's queue address.
	static const message::qaddr_t TIMER_MODULE_QUEUE_ADDRESS
		= message::qaddr_timer;

	/// A non reserved protocol id identified was used.
	static const uint16 NSLP_ID				    = 34;

	static const uint32 SEND_MSG_TIMEOUT		= 5;
	static const uint16 SEND_MSG_IP_TTL			= 100;
	static const uint32 SEND_MSG_GIST_HOP_COUNT	= 10;

	/// Whether to use colours in the logging messages.
	static const bool USE_COLOURS				= true;

  protected:
  
	configpar_repository* cfgpar_rep;

	hostaddress get_hostaddress(const std::string &key);

	void registerAllPars();
};


// this is just a convenience function
template <class T>
void 
anslp_config::setpar(anslp_configpar_id_t configparid, const T& value)
{
	cfgpar_rep->setPar(anslp_realm, configparid, value);
}


// this is just a convenience function
template <class T> T
anslp_config::getpar(anslp_configpar_id_t configparid) const
{
	return cfgpar_rep->getPar<T>(anslp_realm, configparid);
}


// this is just a convenience function
template <class T> T&
anslp_config::getparref(anslp_configpar_id_t configparid) const
{
	return cfgpar_rep->getParRef<T>(anslp_realm, configparid);
}


// this is just a convenience function
inline
string
anslp_config::getparname(anslp_configpar_id_t configparid)
{
	// reference to the config repository singleton
	return cfgpar_rep->getConfigPar(anslp_realm, configparid)->getName();
}


} // namespace anslp

#endif // ANSLP_ANSLP_CONFIG_H
