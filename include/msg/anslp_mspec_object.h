/*!
 * The Mspec Object.
 *
 * $Id: anslp_mspec_object.h 2895 2015-08-30 $
 * $HeadURL: https://./include/msg/anslp_mspec_object.h $
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
#ifndef ANSLP_MSG_MSPEC_OBJECT_H
#define ANSLP_MSG_MSPEC_OBJECT_H

#include "anslp_object.h"


namespace anslp {
 namespace msg {

    using namespace protlib;



/**
 * \class asnlp_mspec_object
 *
 * \brief The generic metering spec object.
 * 
 * RFC definition: the MSPEC objects describe the actual exchanged information.
 * 
 * \author Andres Marentes
 *
 * \version 0.1 
 *
 * \date 2015/08/30 12:52:00
 *
 * Contact: la.marentes455@uniandes.edu.co
 *  
 */
class anslp_mspec_object : public anslp_object
{

  public:
	
	/*
	 * Inherited from IE
	 */
	virtual anslp_mspec_object *new_instance() const =0;
	virtual anslp_mspec_object *copy() const = 0;

	virtual const char *get_ie_name() const = 0;
	virtual size_t get_serialized_size(coding_t coding) const = 0;
	
	virtual ~anslp_mspec_object();

	virtual bool isEqual(const anslp_mspec_object &object) const = 0;
	
	virtual bool notEqual(const anslp_mspec_object &object) const = 0;


  protected:

	anslp_mspec_object();
			
	anslp_mspec_object(uint16 object_type, treatment_t tr, bool _unique);

	virtual bool check_body() const = 0;
	virtual bool equals_body(const anslp_object &obj) const = 0;

	virtual bool deserialize_body(NetMsg &msg, uint16 body_length,
			IEErrorList &err, bool skip) = 0;

	virtual void serialize_body(NetMsg &msg) const = 0;
	
};


 } // namespace msg
} // namespace mnslp

#endif // ANSLP_MSG_MSPEC_OBJECT_H
