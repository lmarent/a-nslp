/*!
 * A ANSLP NOTIFY Message.
 *
 * $Id: anslp_notify.h 2895 2014-11-05  $
 * $HeadURL: https://./include/msg/anslp_notify.h $
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
#ifndef ANSLP_NOTIFY_H
#define ANSLP_NOTIFY_H

#include "ie.h"

#include "anslp_msg.h"


namespace anslp 
{
  namespace msg {


/**
 * A ANSLP NOTIFY Message.
 *
 * This class implements the ANSLP NOTIFY Message, which may be sent
 * asynchronously to report errors.
 */
class anslp_notify : public anslp_msg 
{

  public:
	static const uint8 MSG_TYPE = 0x3;

	explicit anslp_notify();
	explicit anslp_notify(const anslp_notify &other);
	virtual ~anslp_notify();

	/*
	 * Inherited methods:
	 */
	anslp_notify *new_instance() const;
	anslp_notify *copy() const;
	void register_ie(IEManager *iem) const;
	virtual void serialize(NetMsg &msg, coding_t coding,
		uint32 &bytes_written) const throw (IEError);
	virtual bool check() const;		
	

	/*
	 * New methods:
	 */
	void set_information_code(uint8 severity, uint8 response_code,
							  uint16 object_type = 0);
	
	/**
	 * Get the severity class type of information code
	 */
	uint8 get_severity_class() const;
	
	/**
	 * Get the response code
	 */
	 uint8 get_response_code() const;
	
	/**
	 * Get the response object type
	 */
	uint16 get_response_object_type() const;
	
	uint16 get_object_type() const;

  protected:
  
	uint32 serialize_object(ie_object_key &key, NetMsg &msg, coding_t coding) const;
	
};


  } // namespace msg
} // namespace anslp

#endif // ANSLP_NOTIFY_H
