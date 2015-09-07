/*!
 * The Message Hop Count Object.
 *
 * $Id: message_hop_count.h 2895 2015-08-30 12:36:00 amarentes $
 * $HeadURL: https://./include/msg/message_hop_count.h $
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
#ifndef ANSLP_MSG_MESSAGE_HOP_COUNT_H
#define ANSLP_MSG_MESSAGE_HOP_COUNT_H

#include "anslp_object.h"


namespace anslp 
{
  namespace msg {


    using namespace protlib;


/**
 * \class msg_sequence_number
 *
 * \brief The Message hop count Object
 * 
 * RFC definition: this object carries the number of previous MNEs on the signaling path
 * for this session. This object is a integer which starts by zero at
 * the MNI and MUST be incremented by one at each MNSLP hop on the
 * signaling path.
 * 
 * \author Andres Marentes
 *
 * \version 0.1 
 *
 * \date 2014/12/22 10:31:00
 *
 * Contact: la.marentes455@uniandes.edu.co
 *  
 */
class message_hop_count : public anslp_object 
{

  public:
	static const uint16 OBJECT_TYPE = 0x00F6;

	explicit message_hop_count();
	
	explicit message_hop_count(uint32 mhc, 
							   treatment_t t = tr_mandatory,
							   bool _unique = true);

	virtual ~message_hop_count();

	virtual message_hop_count *new_instance() const;
	
	virtual message_hop_count *copy() const;

	virtual size_t get_serialized_size(coding_t coding) const;
	
	virtual bool check_body() const;
	
	virtual bool equals_body(const anslp_object &other) const;
	
	virtual const char *get_ie_name() const;
	
	virtual ostream &print_attributes(ostream &os) const;

	virtual bool deserialize_body(NetMsg &msg, uint16 body_length,
			IEErrorList &err, bool skip);

	virtual void serialize_body(NetMsg &msg) const;


	/*
	 * New methods
	 */
	/**
	 * Get the message hop count for the message 
	 */
	uint32 get_value() const;

	/**
	 * Set the message hop count for the message 
	 */
	void set_value(uint32 mhc);

	/**
	 * Assignment operator
	 */
	message_hop_count &operator=(const message_hop_count &other);

  private:

	static const char *const ie_name;

	uint32 value;   ///< Number of hops that the message has tranverse. For now 32 bits.
};


  } // namespace msg
} // namespace anslp

#endif // ANSLP_MSG_MESSAGE_HOP_COUNT_H
