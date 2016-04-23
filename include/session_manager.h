/// ----------------------------------------*- mode: C++; -*--
/// @file session_manager.h
/// The session manager.
/// ----------------------------------------------------------
/// $Id: session_manager.h 2558 2015-08-31 14:05:00 amarentes $
/// $HeadURL: https://./include/session_manager.h $
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
#ifndef ANSLP_SESSION_MANAGER_H
#define ANSLP_SESSION_MANAGER_H

#include <ext/hash_map>

#include "protlib_types.h"

#include "session.h"
#include "ni_session.h"
#include "nf_session.h"
#include "nr_session.h"


namespace anslp 
{

	//! list of done auctions
	typedef list<session*>            sessionDone_t;
	typedef list<session*>::iterator  sessionDoneIter_t;


/**
 * The session manager.
 *
 * The session manager is the interface to ANSLP's session table. It can be
 * used to get a session using its session ID. This class also serves as a
 * session factory, because it can verify that a created session_id is really
 * unique on this node.
 *
 * Instances of this class are thread-safe.
 */
class session_manager 
{

  public:
  
	session_manager(anslp_config *conf);
	
	~session_manager();

	ni_session *create_ni_session();
		
	nf_session *create_nf_session(const session_id &sid);
	
	nr_session *create_nr_session(const session_id &sid);
	
	session *get_session(const session_id &sid);
	
	session *remove_session(const session_id &sid);

  private:
  
	pthread_mutex_t mutex;
	
	anslp_config *config; // shared by many objects, don't delete
	
	hash_map<session_id, session *> session_table;
	
	sessionDone_t sessionDone;
	
	typedef hash_map<session_id, session *>::const_iterator c_iter;

	session_id create_unique_id() const;

	// Large initial size to avoid resizing of the session table.
	static const int SESSION_TABLE_SIZE = 500000;
	
	static const int DONE_SESSION_LIST_SIZE = 300; 
	
	/*! \short add the session to the list of finished sessions

       \arg \c session - session to be set as done
    */
    void store_session_asdone(session *s);
};


} // namespace anslp

#endif // ANSLP_SESSION_MANAGER_H
