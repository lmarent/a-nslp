/*!
 * The Message Sequence Number Object.
 *
 * $Id: msg_sequence_number.h 2895 2015-08-30 13:21:00Z amarentes $
 * $HeadURL:  $
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
#ifndef ANSLP_MSG_MSG_SEQUENCE_NUMBER_H
#define ANSLP_MSG_MSG_SEQUENCE_NUMBER_H

#include "anslp_object.h"


namespace anslp 
{
 namespace msg {

    using namespace protlib;


/**
 * \class msg_sequence_number
 *
 * \brief Maintains the sequence number of messages.
 *
 * Every A-NSLP message carries a unique sequence number. This number has
 * to be created randomly on first use and is incremented for every message
 * that is sent. Using the MSN, nodes on the path can discover duplicated or
 * missing messages.
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
class msg_sequence_number : public anslp_object {

  public:
	static const uint16 OBJECT_TYPE = 0x00F7;

	explicit msg_sequence_number();
	explicit msg_sequence_number(uint32 msn, 
							     treatment_t t = tr_mandatory,
							     bool _unique = true);

	virtual ~msg_sequence_number();

	virtual msg_sequence_number *new_instance() const;
	virtual msg_sequence_number *copy() const;

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
	uint32 get_value() const;
	void set_value(uint32 msn);

  private:
	// Disallow assignment for now.
	msg_sequence_number &operator=(const msg_sequence_number &other);

	static const char *const ie_name;

	uint32 value;
};


 } // namespace msg
} // namespace anslp

#endif // ANSLP_MSG_MSG_SEQUENCE_NUMBER_H
