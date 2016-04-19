/*!
 * A ANSLP Message.
 *
 * $Id: asnslp_msg.h 2438 2015-08-30  $
 * $HeadURL: https://./include/msg/anslp_msg.h $
 *
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
#ifndef ANSLP_MSG_ANSLP_MSG_H
#define ANSLP_MSG_ANSLP_MSG_H

#include "ie.h"

#include "ie_store.h"
#include "ie_object_key.h"
#include "anslp_object.h"


namespace anslp 
{
  namespace msg {
     using namespace protlib;


/**
 * A ANSLP Message.
 *
 * This class implements a ANSLP Message. It can read and initialize itself
 * from or write itself into a NetMsg object using deserialize()/serialize(),
 * respectively.
 */
class anslp_msg : public IE {

  public:
	virtual ~anslp_msg();

	/*
	 * Inherited from IE:
	 */
	virtual anslp_msg *new_instance() const;
	virtual anslp_msg *copy() const;

	virtual anslp_msg *deserialize(NetMsg &msg, coding_t coding,
			IEErrorList &errorlist, uint32 &bytes_read, bool skip);

	virtual void serialize(NetMsg &msg, coding_t coding,
			uint32 &bytes_written) const throw (IEError);

	virtual bool check() const;
	virtual bool supports_coding(coding_t c) const;
	virtual size_t get_serialized_size(coding_t coding) const;

	virtual bool operator==(const IE &ie) const;
	virtual const char *get_ie_name() const;
	virtual ostream &print(ostream &os, uint32 level, const uint32 indent,
			const char *name = NULL) const;

	virtual void register_ie(IEManager *iem) const;


	/*
	 * New methods:
	 */
	virtual uint8 get_msg_type() const;
	virtual bool has_msg_sequence_number() const;
	virtual uint32 get_msg_sequence_number() const;

	static uint8 extract_msg_type(uint32 header_raw) throw ();

  protected:
	static const uint16 HEADER_LENGTH;

	// protected constructors to prevent instantiation
	explicit anslp_msg();
	explicit anslp_msg(uint8 msg_type);
	explicit anslp_msg(const anslp_msg &other);

	virtual size_t get_num_objects() const;
	virtual anslp_object *get_object(ie_object_key &object_type) const;
	virtual void set_object(anslp_object *obj);
	virtual anslp_object *remove_object(ie_object_key &object_type);

	virtual void set_msg_type(uint8 mt);

	/**
	 * Map ANSLP Object Type to anslp_object.
	 */
	ie_store objects;
	typedef ie_store::const_iterator obj_iter;


  private:
	static const char *const ie_name;

	/**
	 * ANSLP Message header fields.
	 */
	uint8 msg_type;

};

  } // namespace msg
} // namespace asnlp


/*
 * Include all known messages for convenience.
 */
#include "anslp_create.h"
#include "anslp_bidding.h"
#include "anslp_refresh.h"
#include "anslp_notify.h"

#endif // ANSLP_MSG_ANSLP_MSG_H
 
