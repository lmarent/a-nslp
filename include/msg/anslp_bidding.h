/*!
 * A ANSLP BIDDING Message.
 *
 * $Id: anslp_bidding.h 2014-11-05  $
 * $HeadURL: https://./include/msg/anslp_bidding.h $
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
#ifndef ANSLP_BIDDING_H
#define ANSLP_BIDDING_H

#include "ie.h"

#include "anslp_msg.h"
#include "anslp_mspec_object.h"
#include "anslp_ipap_message.h"


namespace anslp {
  namespace msg {


/**
 * A ANSLP BIDDING Message.
 *
 * This class implements a ANSLP BIDDING Message.
 */
class anslp_bidding : public anslp_msg {

  public:
	static const uint8 MSG_TYPE = 0x6;

	explicit anslp_bidding();
	explicit anslp_bidding(const anslp_bidding &other);
	virtual ~anslp_bidding();

	/*
	 * Inherited methods:
	 */
	anslp_bidding *new_instance() const;
	anslp_bidding *copy() const;
	void register_ie(IEManager *iem) const;
	virtual void serialize(NetMsg &msg, coding_t coding, uint32 &bytes_written) const throw (IEError);		
	virtual bool check() const;

	/*
	 * New methods:
	 */
	void set_msg_sequence_number(uint32 msn);
	
	uint32 get_msg_sequence_number() const;
		
	void set_mspec_object(anslp_mspec_object *obj);
	
	void get_mspec_objects(vector<anslp_mspec_object *> &list_return);
	
  protected:
	
	uint32 serialize_object(ie_object_key &key, NetMsg &msg, coding_t coding) const;

};


  } // namespace msg
} // namespace anslp

#endif // ANSLP_BIDDING_H
