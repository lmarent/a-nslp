/*!
 * The Information Code Object.
 *
 * $Id: information_code.h 2895 2008-02-21 00:42:23Z amarentes $
 * $HeadURL: https://./include/msg/information_code.h $
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
#ifndef ANSLP_MSG_INFORMATION_CODE_H
#define ANSLP_MSG_INFORMATION_CODE_H

#include "anslp_object.h"


namespace anslp {
  namespace msg {

     using namespace protlib;


/**
 * \class msg_sequence_number
 *
 * \brief The Information Code Object, which is used in response messages.
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
class information_code : public anslp_object {

  public:
	static const uint16 OBJECT_TYPE = 0x0F4;

	enum severity_class_t 
	{
		sc_informational				= 1,  
		sc_success						= 2,  
		sc_protocol_error				= 3,
		sc_transient_failure			= 4,
		sc_permanent_failure			= 5,
		sc_signaling_session_failures	= 6
	};

	enum informational_code_t 
	{
		ic_route_change					= 1,
		ic_reauthentication_required	= 2,
		ic_node_going_down				= 3
	};

	enum success_code_t 
	{
		suc_successfully_processed		= 1
	};

	enum protocol_error_code_t 
	{
		// TODO: add constants
	    perr_unknown_object_field_value	= 8
	};

	enum transient_failure_code_t 
	{
		tfail_resources_unavailable	= 1
	};

	enum permanent_failure_code_t 
	{
		fail_authentication_failed			= 1,
		fail_authorization_failed			= 2,
		fail_transport_security				= 3,
		fail_internal_error					= 4,
		fail_no_nat_here					= 5,
		fail_no_edge_device_here			= 6,
		fail_nr_not_reached					= 7,
		fail_tracing_not_allowed			= 8,
		fail_configuration_failed			= 9
	};

	enum signaling_session_failures_code_t 
	{
		sigfail_terminated_async			 	= 1,
		sigfail_lifetime_too_big			 	= 2,
		sigfail_no_reservation_found		 	= 3,
		sigfail_auction_denied			 		= 4,
		sigfail_unknown_auction_action	 		= 5,
		sigfail_auction_not_applicable			= 6,		
		sigfail_wrong_conf_message   		 	= 7,
		sigfail_auction_connection_broken    	= 8
	};
	
	enum object_type_t 
	{
		obj_none					= 0x000,
		obj_information_code 		= 0x0F4,   // 244
		obj_selection_met_entities 	= 0x0F5, 
		obj_message_hop_count 		= 0x0F6,
		obj_msg_sequence_number 	= 0x0F7,
		obj_session_lifetime		= 0x0F8,
		obj_ipap_message			= 0x0F9   // 249
	};	

	explicit information_code();
	
	explicit information_code(uint8 severity, uint8 response_code = 0,
							  uint16 object_type = 0, 
							  treatment_t t = tr_mandatory,
							  bool _unique = true);

	virtual ~information_code();

	virtual information_code *new_instance() const;
	
	virtual information_code *copy() const;

	virtual size_t get_serialized_size(coding_t coding) const;
	
	virtual bool check_body() const;
	
	virtual bool equals_body(const anslp_object &other) const;
	
	virtual const char *get_ie_name() const;
	
	virtual ostream &print_attributes(ostream &os) const;

	virtual bool deserialize_body(NetMsg &msg, uint16 body_length,
			IEErrorList &err, bool skip);

	virtual void serialize_body(NetMsg &msg) const;


	/*
	 * New methods:
	 */
	/**
	 * Get the severity class type of information code
	 */
	uint8 get_severity_class() const;
	
	/**
	 * Set the severity class type of information code
	 */
	void set_severity_class(uint8 severity);

	/**
	 * Get the response code
	 */
	uint8 get_response_code() const;

	/**
	 * Set the response code
	 */
	void set_response_code(uint8 code);

	/**
	 * Get the response object type
	 */
	uint16 get_response_object_type() const;

	/**
	 * Set the response object type
	 */
	void set_response_object_type(uint16 object_type);

	/**
	 * return whether the message is success or not. 
	 */
	bool is_success() const;

	/**
	 * Assignment operator. 
	 */
	information_code &operator=(const information_code &other);

  private:

	static const char *const ie_name;

	uint8 severity_class; 			///< 4 bits used
	uint8 response_code;  			///< 8 bits used
	uint16 response_object_type; 	///< 16 bits used
};

  } // namespace msg
} // namespace anslp

#endif // ANSLP_MSG_INFORMATION_CODE_H
