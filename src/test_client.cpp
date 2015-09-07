/// ----------------------------------------*- mode: C++; -*--
/// @file test_client.cpp
/// An interactive test client to imitate an NSIS Initiator or Responder.
/// ----------------------------------------------------------
/// $Id: test_client.cpp 4118 2015-02-23 20:23:10 amarentes $
/// $HeadURL: https://./src/test_client.cpp $
// ===========================================================
//                      
// Copyright (C) 2014-2015, all rights reserved by
// - Universidad de los Andes
//
// More information and contact:
// https://www.uniandes.edu.co
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
#include <unistd.h>

#include "logfile.h"
#include <string>
#include <iostream>
#include <list>

#include "anslp_config.h"
#include "anslp_daemon.h"
#include "events.h"

#include "gist_conf.h"
#include "configfile.h"

#include "auction_rule.h"
#include "msg/anslp_ipap_message.h"
#include "msg/anslp_ipap_field.h"
#include <iostream>
#include <limits>


namespace ntlp {
// configuration class
gistconf gconf;
}

using namespace protlib;
using namespace protlib::log;
using namespace ntlp;
using namespace anslp;

logfile commonlog("test_client.log", mnslp_config::USE_COLOURS);
logfile &protlib::log::DefaultLog(commonlog);

static anslp_daemon *mnslpd;
static session_id sid;

static msg::anslp_ipap_message *mess1;

void add_ipfix_vendor_fields(msg::anslp_ipap_message *mess)
{

    
}
 

void create_ipap_message()
{


}

void setup() 
{

     int sourceid = 0x00000000;
     mess1 = new msg::anslp_ipap_message(IPAP_VERSION);

     add_ipfix_vendor_fields(mess1);
     create_ipap_message();

}

static std::string config_filename;
static hostaddress sender_addr;
static hostaddress receiver_addr;
static uint16 sender_port = 0;
static uint16 receiver_port = 0;
static uint8 protocol = 0;
static uint32 lifetime = 30;
static bool proxy_mode = false;


void parse_commandline(int argc, char *argv[]) {
	using namespace std;

	string usage("usage: test_client [-l lifetime] -c config_file "
			"ds_ip dr_ip [[[ds_port] dr_port] protocol]\n");

	/*
	 * Parse command line options.
	 */
	while ( true ) {
		int c = getopt(argc, argv, "l:c:p");

		if ( c == -1 )
			break;

		switch ( c ) {
			case 'c':
				config_filename = optarg;
				break;
			case 'l':
				lifetime = atoi(optarg);
				break;
			default:
				cerr << usage;
				exit(1);
		}
	}

	if ( config_filename == "" ) {
		cerr << usage;
		exit(1);
	}

	argv += optind;
	argc -= optind;

	/*
	 * Only non-option arguments are left. The first is in argv[0].
	 */
	if ( argc < 2 || argc > 5 || sender_addr.set_ip(argv[0]) == false
			|| receiver_addr.set_ip(argv[1]) == false ) {
		cerr << usage;
		exit(1);
	}

	if ( argc > 2 )
		sender_port = atoi(argv[2]);

	if ( argc > 3 )
		receiver_port = atoi(argv[3]);

	if ( argc > 4 )
		protocol = atoi(argv[4]);
}



void tg_configuration( const hostaddress &source_addr, 
					   const hostaddress &destination_addr,
                       uint16 source_port, uint16 dest_port, 
					   uint8 protocol, uint32 session_lifetime )
{
    setup();
    
    FastQueue ret;
	    
    // Build the vector of objects to be configured.
    vector<msg::anslp_mspec_object *> mspec_objects;
    mspec_objects.push_back(mess1->copy());

    // Create a new event for launching the configure event.
    event *e = new api_create_event(source_addr,destination_addr,source_port, 
   				       dest_port, protocol, mspec_objects, 
				       session_lifetime, 
				       selection_metering_entities::sme_any, 
				       &ret);

    anslp_event_msg *msg = new anslp_event_msg(session_id(), e);

    anslpd->get_fqueue()->enqueue(msg);
	
    message *ret_msg = ret.dequeue_timedwait(10000);

    anslp_event_msg *r = dynamic_cast<anslp_event_msg *>(ret_msg);
	
    sid = r->get_session_id();
    delete r;
    cout << "session configured, id = " << sid << "\n";

}

static void tg_teardown(session_id id) {
	cout << "tg_TEARDOWN(session=" << sid << ")" << endl;

	event *e = new api_teardown_event(new session_id(id));

	anslp_event_msg *msg = new anslp_event_msg(session_id(), e);
	anslpd->get_fqueue()->enqueue(msg);
}

void tearDown() 
{
       delete mess1;
}

int main(int argc, char *argv[]) 
{
	using namespace std;

	hostaddress source;

	parse_commandline(argc, argv);
	
	init_framework();

	anslp_config conf;
	
	// create the global configuration parameter repository 
	conf.repository_init();

	// register all A-NSLP configuration parameters at the registry
	conf.setRepository();

	// register all GIST configuration parameters at the registry
	ntlp::gconf.setRepository();

	// read all config values from config file
	configfile cfgfile(configpar_repository::instance());
	conf.getparref<string>(anslpconf_conffilename) = config_filename;

	try {
		cfgfile.load(conf.getparref<string>(anslpconf_conffilename));
	}
	catch(configParException& cfgerr)
	{
		ERRLog("anslpd", "Error occurred while reading the configuration file: " << cfgerr.what());
		cerr << cfgerr.what() << endl << "Exiting." << endl;
		return 1;
	}

	/*
	 * Start the MNSLP daemon thread. It will in turn start the other
	 * threads it requires.
	 */
	anslp_daemon_param param("anslp", conf);
	ThreadStarter<anslp_daemon, anslp_daemon_param> starter(1, param);
	
	// returns after all threads have been started
	starter.start_processing();

	anslpd = starter.get_thread_object();

	// initialize the global session_id, the only one we store
	uint128 raw_id(0, 0, 0, 0);
	sid = session_id(raw_id);

	cout << "config file        : " << config_filename << endl;
	cout << "data sender        : " << sender_addr
   		 << ", port " << sender_port << endl;
	cout << "data receiver      : " << receiver_addr
		 << ", port " << receiver_port << endl;
	cout << "IP protocol        : " << int(protocol) << endl;
	cout << "session lifetime   : " << lifetime << " sec" << endl;

	int input;
        std::cin.clear();
        std::cin.ignore(numeric_limits<streamsize>::max(), '\n');
	do {
		cout << endl;
		cout << "       Current session: " << sid << endl;
		cout << endl;
		cout << "  (1)  tg_CONFIGURE" << endl;
		cout << "  (2)  tg_TEARDOWN for current session " << endl;
		cout << "  (3)  quit" << endl;

		cout << "Input: ";
		std::cin >> input;
		cout << endl;

		if ( input == 1 )
			tg_configuration(sender_addr, receiver_addr,
				sender_port, receiver_port, protocol,
				lifetime);
		else if ( input == 2 )
	      	    tg_teardown(sid);
                std::cin.clear();
		std::cin.ignore(numeric_limits<streamsize>::max(), '\n');
	}
	while ( input != 3 );

	cout << "Stopping A-NSLP thread ..." << endl;

	// shutdown mnslp thread
	starter.stop_processing();
	starter.wait_until_stopped();

	cleanup_framework();
}
