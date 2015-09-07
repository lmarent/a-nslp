/*!
 * The Selection Auctioning Entities Object.
 *
 * $Id: selection_auctioning_entities.h 2895 2015-08-30 $
 * $HeadURL: https://./include/msg/selection_auctioning_entities.h $
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
#ifndef SELECTION_AUCTIONING_ENTITIES_H
#define SELECTION_AUCTIONING_ENTITIES_H

#include "anslp_object.h"


namespace anslp {
 namespace msg {

    using namespace protlib;



/**
 * \class msg_sequence_number
 *
 * \brief The Selection Auctioning Entities Object.
 * 
 * RFC definition: This object is required to determine which MNEs will actually take
 *                 part in the metering.
 * 
 * \author Andres Marentes
 *
 * \version 0.1 
 *
 * \date 2015/08/30 13:30:00
 *
 * Contact: la.marentes455@uniandes.edu.co
 *  
 */
class selection_auctioning_entities : public anslp_object {

  public:
	static const uint16 OBJECT_TYPE = 0x00F5;
	
	enum selection_auctioning_entities_t {
		sme_all							= 1,
		sme_any							= 2,
		sme_first						= 3,  
		sme_last						= 4,  
		sme_first_last					= 5,
		sme_enterprise_specific			= 1024
	};

	explicit selection_auctioning_entities();
	explicit selection_auctioning_entities(uint32 _sme, 
										   treatment_t t = tr_mandatory,
										   bool _unique = true);

	virtual ~selection_auctioning_entities();

	virtual selection_auctioning_entities *new_instance() const;
	virtual selection_auctioning_entities *copy() const;

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
	 * Get the value specified for the selection metering entities
	 */ 
	uint32 get_value() const;
	
	/**
	 * Set the value specified for the selection metering entities
	 */ 
	void set_value(uint32 _sme);

	/**
	 * Assigment operator
	 */
	selection_auctioning_entities &operator=(const selection_auctioning_entities &other);

  private:

	static const char *const ie_name;

	uint32 sme;   			///< 4 bytes used.
};


 } // namespace msg
} // namespace anslp

#endif // SELECTION_AUCTIONING_ENTITIES_H
