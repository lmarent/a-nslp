/// ----------------------------------------*- mode: C++; -*--
/// @file anslp_daemon.h
/// The ANSLP daemon thread.
/// ----------------------------------------------------------
/// $Id: anslp_daemon.h 2558 2015-08-31 12:14:00 amarentes $
/// $HeadURL: https://./include/anslp_daemon.h $
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
#ifndef ANSLP_DAEMON_H
#define ANSLP_DAEMON_H

#include "protlib_types.h"

#include "ntlp_starter.h" // from NTLP

#include "anslp_config.h"
#include "session_manager.h"
#include "auction_rule_installer.h"


namespace anslp 
{
  using protlib::uint32;
  using ntlp::NTLPStarterParam;
  using ntlp::NTLPStarter;


/**
 * Encapsulated parameters for a anslp_daemon thread.
 */
class anslp_daemon_param : public ThreadParam 
{
  public:
	anslp_daemon_param(const char *name, const anslp_config &conf)
		: ThreadParam((uint32)-1, name), config(conf) { }

	anslp_config config;
};


/**
 * The ANSLP daemon thread.
 *
 * This thread is the ANSLP daemon implemenation. It starts a NTLP thread,
 * registers with it and handles all messages it gets from the NTLP.
 */
class anslp_daemon : public Thread {
  public:
  
	anslp_daemon(const anslp_daemon_param &param);
	
	virtual ~anslp_daemon();

	virtual void startup();

	virtual void main_loop(uint32 thread_num);

	virtual void shutdown();

  private:
  
	anslp_config config;

	session_manager session_mgr;
		
	auction_rule_installer *rule_installer;

	ThreadStarter<NTLPStarter, NTLPStarterParam> *ntlp_starter;
};


void init_framework();
void cleanup_framework();


}; // namespace anslp

#endif // ANSLP_DAEMON_H
