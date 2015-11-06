/// ----------------------------------------*- mode: C++; -*--
/// @file session_id.cpp
/// The session_id class.
/// ----------------------------------------------------------
/// $Id: session_id.cpp 2558 2015-08-31 10:53:00 $
/// $HeadURL: https://./src/session_id.cpp $
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
#include <openssl/rand.h>
#include <iostream>
#include "logfile.h"

#include "session.h"
// #include "dispatcher.h"


using namespace anslp;
using namespace protlib::log;


/**
 * Constructor.
 *
 * Initializes this object with a random 128 Bit session ID. The random
 * numbers used are cryptographically strong (according to OpenSSL's docs).
 */
session_id::session_id() 
{
	// Use OpenSSL's cryptographically strong random numbers.
	int ret = RAND_bytes((unsigned char *) &id, sizeof(id));

	assert( ret == 1 );
}

session_id::session_id(string sessionId)
{
	unsigned long parts[5];
	char *errp = NULL;
	unsigned long n;
	int i = 0;

	std::size_t found = sessionId.find_first_of('.');
	std::size_t init = 0;
	
	do 	{
		string part = sessionId.substr(init, found-init);
		n = strtoul(part.c_str(), &errp, 0);
		parts[i] = n;
		if (part.empty() || (*errp != '\0')) {
			throw request_error("Invalid session Id:" + sessionId);
		}

		init = found+1;
		found = sessionId.find_first_of('.',init);
		i++;
	} while ((found != std::string::npos));

	//takes the last part.
	string part = sessionId.substr(init, found-init);
	n = strtoul(part.c_str(), &errp, 0);
	parts[i] = n;
	if (part.empty() || (*errp != '\0')) {
		throw request_error("Invalid session Id:" + sessionId);
	}
	
	
	// Everything went well.
	if (i == 3 ){
		id.w1 = parts[0];
		id.w2 = parts[1];
		id.w3 = parts[2];
		id.w4 = parts[3];
	} else {
		ostringstream o;
		o << "Invalid session Id:" + sessionId + " number of parts";
		o << i << endl; 
		
		throw request_error(o.str());
	}
	

  
}


std::ostream &anslp::operator<<(std::ostream &out, const session_id &sid) 
{
	using namespace std;

	uint128 val = sid.get_id();

	ios_base::fmtflags old_fmt = out.flags(); // save stream format

	out << uppercase << hex
		<< setw(8) << setfill('0') << val.w1 << '_'
		<< setw(8) << setfill('0') << val.w2 << '_'
		<< setw(8) << setfill('0') << val.w3 << '_'
		<< setw(8) << setfill('0') << val.w4;

	out.flags(old_fmt); // revert to old stream format

	return out;
}

string session_id::to_string() 
{
	char str1[11];
	char str2[11];
	char str3[11];
	char str4[11];
	string separator = ".";
	
	snprintf(str1, sizeof str1, "%lu", (unsigned long)id.w1);
	snprintf(str2, sizeof str2, "%lu", (unsigned long)id.w2);
	snprintf(str3, sizeof str3, "%lu", (unsigned long)id.w3);
	snprintf(str4, sizeof str4, "%lu", (unsigned long)id.w4);
	
	string val_result = string(str1) + separator + string(str2) 
						+ separator + string(str3) + separator + string(str4);
	
	return val_result;

}
