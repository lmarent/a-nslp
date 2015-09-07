/*!
 * A ANSLP CREATE Message.
 *
 * $Id: anslp_create.h 2014-11-05  $
 * $HeadURL: https://./include/msg/anslp_create.h $
 */
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
#ifndef ANSLP_CREATE_H
#define ANSLP_CREATE_H

#include "ie.h"

#include "anslp_msg.h"
#include "anslp_mspec_object.h"
#include "anslp_ipap_message.h"


namespace anslp {
  namespace msg {


/**
 * A ANSLP CREATE Message.
 *
 * This class implements a ANSLP CREATE Message.
 */
class anslp_create : public anslp_msg {

  public:
	static const uint8 MSG_TYPE = 0x1;

	explicit anslp_create();
	explicit anslp_create(const anslp_create &other);
	virtual ~anslp_create();

	/*
	 * Inherited methods:
	 */
	anslp_create *new_instance() const;
	anslp_create *copy() const;
	void register_ie(IEManager *iem) const;
	virtual void serialize(NetMsg &msg, coding_t coding, uint32 &bytes_written) const throw (IEError);		
	virtual bool check() const;

	/*
	 * New methods:
	 */
	void set_session_lifetime(uint32 seconds);

	uint32 get_session_lifetime() const;

	void set_msg_sequence_number(uint32 msn);
	
	uint32 get_msg_sequence_number() const;
	
	void set_selection_auctioning_entities(uint32 value);

	uint32 get_selection_auctioning_entities() const;
	
	void set_message_hop_count(uint32 value);
	
	uint32 get_message_hop_count() const;
	
	void set_mspec_object(anslp_mspec_object *obj);
	
	void get_mspec_objects(vector<anslp_mspec_object *> &list_return);
	
  protected:
	uint32 serialize_object(ie_object_key &key, NetMsg &msg, coding_t coding) const;


};


  } // namespace msg
} // namespace anslp

#endif // ANSLP_CREATE_H
