/*!
 * Interface for M-NSLP objects.
 *
 * $Id: anslp_object.h 2896 2008-02-21 00:54:43Z bless $
 * $HeadURL: /include/msg/anslp_object.h $
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
#ifndef ANSLP_MSG_MNSLP_OBJECT_H
#define ANSLP_MSG_MNSLP_OBJECT_H

#include "ie.h"

namespace anslp 
{
  namespace msg {
     using namespace protlib;


/**
 * The representation of a anslp object.
 *
 * This class provides methods to serialize/deserialize anslp objects
 * as well as access to various header fields. mnlsp_object is abstract;
 * all concrete parameters have to be subclasses.
 */
class anslp_object : public IE {

  public:
	enum treatment_t {
		tr_mandatory	= 0,
		tr_optional	= 1,
		tr_forward	= 2,
		tr_reserved	= 3
	};

	virtual ~anslp_object();

	/*
	 * Inherited from IE
	 */
	virtual anslp_object *new_instance() const = 0;
	virtual anslp_object *copy() const = 0;

	virtual const char *get_ie_name() const = 0;
	virtual size_t get_serialized_size(coding_t coding) const = 0;

	virtual IE *deserialize(NetMsg &msg, coding_t coding, IEErrorList &err,
			uint32 &bytes_read, bool skip);

	virtual void serialize(NetMsg &msg, coding_t coding,
			uint32 &bytes_written) const throw (IEError);

	virtual bool operator==(const IE &ie) const;
	virtual bool check() const;
	virtual bool supports_coding(coding_t c) const;
	virtual void register_ie(IEManager *iem) const;


	/*
	 * New methods
	 */
	treatment_t get_treatment() const;
	void set_treatment(treatment_t t);

	uint16 get_object_type() const;
	void set_object_type(uint16 obj_type);

    bool is_unique() const;
    void set_unique(bool _unique);

	static uint16 extract_object_type(uint32 header_raw) throw ();

  protected:
	/**
	 * Length of a anslp Object header in bytes.
	 */
	static const uint16 HEADER_LENGTH = 4;

	/**
	 * The encoding used (necessary for the IE interfaces).
	 */
	static const coding_t CODING = protocol_v1;


	anslp_object();
	anslp_object(uint16 object_type, treatment_t tr, bool _unique);

	virtual bool deserialize_header(NetMsg &msg, uint16 &body_length,
			IEErrorList &err, bool skip);

	virtual void serialize_header(NetMsg &msg, uint16 body_len) const;

	virtual ostream &print_attributes(ostream &os) const;
	virtual ostream &print(ostream &os, uint32 level, const uint32 indent,
			const char *name = NULL) const;

	virtual bool check_body() const = 0;
	virtual bool equals_body(const anslp_object &obj) const = 0;

	virtual bool deserialize_body(NetMsg &msg, uint16 body_length,
			IEErrorList &err, bool skip) = 0;

	virtual void serialize_body(NetMsg &msg) const = 0;

  private:
	uint16 object_type;
	bool unique;				///< This value identifies if the object must be unique within the message.
	treatment_t treatment;
												
	
};


  } // namespace msg
} // namespace anslp


/*
 * Include all known objects for convenience.
 */
#include "session_lifetime.h"
#include "information_code.h"
#include "selection_auctioning_entities.h"
#include "msg_sequence_number.h"
#include "message_hop_count.h"



#endif // ANSLP_MSG_MNSLP_OBJECT_H
