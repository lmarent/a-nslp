/*! \file   anslp_ipap_message.h

    Copyright 2014-2015 Universidad de los Andes, Bogotá, Colombia

    This file is part of IP Auction Processing protocol (IPAP).

    IPAP is free software; you can redistribute it and/or modify 
    it under the terms of the GNU General Public License as published by 
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    IPAP is distributed in the hope that it will be useful, 
    but WITHOUT ANY WARRANTY; without even the implied warranty of 
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this software; if not, write to the Free Software 
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    Description:
    message class of nslp ipap message protocol

    $Id: anslp_ipap_message.h 748 2015-08-31 20:13:00Z amarentes $
*/

#ifndef ANSLP_IPAP_MESSAGE_H
#define ANSLP_IPAP_MESSAGE_H

#include "IpAp_message.h"
#include "anslp_mspec_object.h"

namespace anslp 
{
  namespace msg {

typedef map<ipap_templ_type_t, ipap_field_key> 			 		templateListKeys_t;
typedef map<ipap_templ_type_t, ipap_field_key>::iterator 		templateListIterKeys_t;
typedef map<ipap_templ_type_t, ipap_field_key>::const_iterator 	templateListConstIterKeys_t;


typedef map<ipap_templ_type_t, ipap_field_key>					recordListKeys_t;
typedef map<ipap_templ_type_t, ipap_field_key>::iterator 		recordListIterKeys_t;
typedef map<ipap_templ_type_t, ipap_field_key>::const_iterator 	recordListConstIterKeys_t;


class anslp_ipap_message : public anslp_mspec_object
{

private:
	/// Name of the object.
	static const char *const 			ie_name;
	
	/// list of keys by template
	templateListKeys_t    				templateKeys;
	
	/// list of keys by record
	recordListKeys_t					recordKeys;
		
	Logger *log; //!< link to global logger object
	int ch;      //!< logging channel number used by objects of this class												
	
	/// Load the fields used as key for templates 
	void load_template_keys(void);
	
	/// Load the fields used as key for records. 
	void load_record_keys(void);
	
public:

	/// name of Header elements by object type.
	static char *OBJECT_HEADER_XML_TAGS[];

	/// name of Line elements by object type.
	static char *OBJECT_LINE_XML_TAGS[];
	
	ipap_message ip_message;
	
	static const uint16 OBJECT_TYPE = 0x00F9;
	

   /**
    * Create a new class anslp_ipap_message
    * @param By default it sets the version in IPAP and encode in true, 
    * 		    the source id is set to 0.
    */
   explicit anslp_ipap_message();

	/**
	 * Create a new class anslp_ipap_message
	 * @param ipap_version 	 - message version. 
	 *  	  _encode_network - establish whether the message is going to be network encoded or not. 
	 */
	explicit anslp_ipap_message( int ipap_version, bool _encode_network = true);
	   
	/**
	 * Create a new class anslp_ipap_message
	 * @param 	 param  		 - buffer containing the message.
	 * 		  	 message_length	 - length of the message.
	 *  		 _encode_network - establish whether the message is going to be network decoded or not. 
	 */
	explicit anslp_ipap_message(uchar * param, size_t message_length, bool _encode_network);
	   
	/**
	 * Create a new class anslp_ipap_message copying from another anslp_ipap_message.
	 * @param rhs 	 - message to copy from. 
	 */
	explicit anslp_ipap_message(const anslp_ipap_message &rhs);
	   
	/**
	 * Destructor.
	*/
	virtual ~anslp_ipap_message(void);

	/**
	 * Get the internal buffer that was exported
	 */
	uchar * get_message(void) const;

	ipap_field_key get_template_key(ipap_templ_type_t temp_type) const;

	ipap_field_key get_record_key(ipap_templ_type_t temp_type) const;
	
	/**
	 *  Equals operator. 
	 *  It is equal when it has the same amount of templates and 
	 *  data records, and the information inside is equal. The order in
	 * the data records must be same too.
	 */
	virtual bool isEqual(const anslp_mspec_object &rhs) const;


	/**
	 *  Not equals operator. 
	 *  It is not equal when it has a different amount of templates and 
	 *  data records, or the information inside is not equal.
	 */
	virtual bool notEqual(const anslp_mspec_object &rhs) const;
	   
	/**
	 * Get a new instance from the ipap message
	 */
	virtual anslp_ipap_message *new_instance() const;
	
	/**
	 * Copy this IPAP message into a new message
	 */
	virtual anslp_ipap_message *copy() const;

	/**
	 * Length in bytes of the IPAP message
	 */
	virtual size_t get_serialized_size(coding_t coding) const;
	
	/**
	 * We say that a IPAP message is ok whenever executing the output 
	 * method, the character representing the message has a length greater 
	 * than zero.
	*/ 
	virtual bool check_body() const;
	
	/**
	 * Check if the IPAP message given as parameter has the same definition.
	 */
	virtual bool equals_body(const anslp_object &other) const;
	
	/** 
	 * Get The name of the oject
	 */
	virtual const char *get_ie_name() const;
	
	/** 
	 * print in an output stream the message definition
	 */
	virtual ostream &print_attributes(ostream &os) const;

	/**
	 * Deserialize the message from the body
	 */
	virtual bool deserialize_body(NetMsg &msg, uint16 body_length,
									 IEErrorList &err, bool skip);

	/** 
	 * serialize the message
	 */
	virtual void serialize_body(NetMsg &msg) const;
	
	
};

  } // namespace msg
} // namespace asnlp

#endif // ANSLP_IPAP_MESSAGE_H
