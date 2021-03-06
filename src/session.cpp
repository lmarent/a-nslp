/// ----------------------------------------*- mode: C++; -*--
/// @file session.cpp
/// The session class.
/// ----------------------------------------------------------
/// $Id: session.cpp 2558 2015-08-31 10:49:00 amarentes $
/// $HeadURL: https://./src/session.cpp $
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

#include "logfile.h"

#include "session.h"
#include "dispatcher.h"
#include "msg/selection_auctioning_entities.h"
#include "thread_mutex_lockable.h"
#include <iostream>


using namespace anslp;
using namespace protlib::log;
using protlib::uint32;

#define LogWarn(msg) Log(WARNING_LOG, LOG_NORMAL, "session", msg)
#define LogInfo(msg) Log(INFO_LOG, LOG_NORMAL, "session", msg)
#define LogDebug(msg) Log(DEBUG_LOG, LOG_NORMAL, "session", msg)


/**
 * Constructor.
 *
 * A random session ID is created and the message sequence number is set to 0.
 * Additionally, the mutex is initialized.
 */
session::session(lock *lockO) : rule(NULL), id(), msn(0), lock_ (lockO)
{
	init();
}


/**
 * Constructor.
 *
 * Like the default constructor, but uses the given session id and msgn instead
 * of generating them.
 *
 * @param sid a hopefully unique session id
 */
session::session(const session_id &sid, lock *lockO) : rule(NULL), id(sid), msn(0), lock_ (lockO)
{
	init();
}


/**
 * Destructor.
 */
session::~session() 
{
	
	LogDebug("Starting destroy session");
	
	
	if (rule != NULL){
		delete rule;
	}
	
	if (lock_ != NULL){
		delete lock_;
	}
	LogDebug("Ending destroy session");
	
}


/**
 * A helper method for the constructors, to avoid code duplication.
 */
void session::init() 
{
		
	rule = new auction_rule();
	
	if (lock_ == NULL){
		lock_ = new lock(new thread_mutex_lockable());
	}
}


/**
 * Process the given event.
 *
 * This method calls the user-defined process_event method and takes care of
 * locking.
 */
void session::process(dispatcher *d, event *evt) 
{


	process_event(d, evt);	// implemented by child classes

}


/**
 * Increment the Message Sequence Number.
 *
 * @return the new (incremented) MSN
 */
uint32 session::next_msg_sequence_number() 
{
	return ++msn;	// UINT_MAX+1 = 0, so wrap arounds as per RFC-1982 work
}


/****************************************************************************
 *
 * Utilities
 *
 ****************************************************************************/


void session::check_lifetime(uint32 lifetime, uint32 max_lifetime)
		const throw (override_lifetime) {

	if ( lifetime > max_lifetime )
		throw override_lifetime("requested session lifetime too long",
			information_code::sc_informational,
			information_code::sigfail_lifetime_too_big);
}


void session::check_authorization(dispatcher *d, const msg_event *e)
		const throw (request_error) {

	if ( ! d->is_authorized(e) )
		throw request_error("authorization/authentication failed",
			information_code::sc_permanent_failure,
			information_code::fail_authentication_failed);
}

bool session::check_participating(const uint32 _sme)
{
	bool val_return = false;
	
	if (type == st_initiator)
	{
	   if (_sme == msg::selection_auctioning_entities::sme_all)
		   val_return = true;
	   else if (_sme == msg::selection_auctioning_entities::sme_any)
	       val_return = true;
	   else if (_sme == msg::selection_auctioning_entities::sme_first)
	       val_return = true;
	   else if (_sme == msg::selection_auctioning_entities::sme_first_last)
	       val_return = true;
	   else
	       val_return = false;
	}
	else if (type == st_forwarder)
	{
	   if (_sme == msg::selection_auctioning_entities::sme_all)
		   val_return = true;
	   else if (_sme == msg::selection_auctioning_entities::sme_any)
	       val_return = true;
	   else
	       val_return = false;
	
	}
	else if (type == st_receiver)
	{
	   if (_sme == msg::selection_auctioning_entities::sme_all)
		   val_return = true;
	   else if (_sme == msg::selection_auctioning_entities::sme_any)
	       val_return = true;
	   else if (_sme == msg::selection_auctioning_entities::sme_first_last)
	       val_return = true;
	   else if (_sme == msg::selection_auctioning_entities::sme_last)
	       val_return = true;
	   else
	       val_return = false;	
	}
	return val_return;
}

void session::set_request_mspec_object(anslp_mspec_object *object) 
{
	if (rule != NULL)
		rule->set_request_object(object);
}

size_t session::get_number_mspec_request_objects(void)
{
	if (rule != NULL)
		return rule->get_number_mspec_request_objects();
	else
		return 0;
}

/**
 * Get the NR/NI+'s address and port.
 */
appladdress session::get_nr_address(msg_event *e) const {
	using namespace ntlp;

	/*
	 * If this is path-coupled MRM, we get the NR's port number from
	 * the MRI.
	 */
	mri_pathcoupled *mri_pc = dynamic_cast<mri_pathcoupled *>(e->get_mri());

	if ( mri_pc != NULL )
		return appladdress(mri_pc->get_destaddress(),
			mri_pc->get_protocol(), mri_pc->get_destport());


	/*
	 * Unknown routing method.
	 */
	throw request_error("unknown MRM: " + int(e->get_mri()->get_mrm()));
}


/**
 * Set the response objects in the auctioning rule from the api install event.
 */
void 
session::set_reponse_objects(api_install_event *install, auction_rule *act_rule) 
{
	
	LogDebug( "Begin set_reponse_objects from api install()");

	assert( install != NULL );
		
	LogDebug( "Nbr objects installed:" << install->getObjects()->size() );
	
	// Check which metering object could be installed in this node.
	objectListConstIter_t it_objects;
	for ( it_objects = install->getObjects()->begin(); 
			it_objects != install->getObjects()->end(); it_objects++)
	{
		act_rule->set_response_object((it_objects->second)->copy());
	}
	
	LogDebug("End set_reponse_objects - objects inserted:" 
					<< act_rule->get_number_mspec_response_objects());
	
}


int 
session::acquire()
{ 
	return lock_->acquire(); 
}
	
int 
session::release() 
{ 
	return lock_->release(); 
} 
