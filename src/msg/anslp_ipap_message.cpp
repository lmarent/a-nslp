/*! \file   anslp_ipap_message.cpp

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

    $Id: anslp_ipap_message.cpp 748 2015-08-31 20:13:00Z amarentes $
*/

#include "anslp_ipap_message.h"
#include "anslp_ipap_exception.h"

using namespace anslp::msg;

const char *const anslp_ipap_message::ie_name = "anslp_ipap_mspec";
														

anslp_ipap_message::anslp_ipap_message():
	anslp_mspec_object(OBJECT_TYPE, tr_mandatory, false), 
	ip_message(0, IPAP_VERSION, true )
{

    log = Logger::getInstance();
    ch = log->createChannel("ANSLP_IPAP_MESSAGE");

#ifdef DEBUG
    log->dlog(ch, "in anslp_ipap_message constructor without parameters");
#endif 

}

anslp_ipap_message::anslp_ipap_message(const ipap_message &message):
	anslp_mspec_object(OBJECT_TYPE, tr_mandatory, false), 
	ip_message(message)
{

    log = Logger::getInstance();
    ch = log->createChannel("ANSLP_IPAP_MESSAGE");


#ifdef DEBUG
    log->dlog(ch, "in anslp_ipap_message constructor without parameters");
#endif 

}

anslp_ipap_message::anslp_ipap_message( int domain_id,  int ipap_version, 
										  bool _encode_network):
	anslp_mspec_object(OBJECT_TYPE, tr_mandatory, false), 
	ip_message(domain_id, ipap_version, _encode_network)
	
{
    log = Logger::getInstance();
    ch = log->createChannel("ANSLP_IPAP_MESSAGE");

#ifdef DEBUG
    log->dlog(ch, "in anslp_ipap_message constructor parameters domain_id ip_version, encode");
#endif 


}

anslp_ipap_message::anslp_ipap_message(uchar * param, 
					size_t message_length, bool _encode_network):
	anslp_mspec_object(OBJECT_TYPE, tr_mandatory, false), 
	ip_message(param, message_length, _encode_network)
{
    log = Logger::getInstance();
    ch = log->createChannel("ANSLP_IPAP_MESSAGE");
	
#ifdef DEBUG
    log->dlog(ch, "in anslp_ipap_message constructor parameters message,lenght encode" );
#endif 	
}

anslp_ipap_message::anslp_ipap_message(const anslp_ipap_message &rhs):
	anslp_mspec_object(OBJECT_TYPE, rhs.get_treatment(), false), 
	ip_message(rhs.ip_message)
{
    log = Logger::getInstance();
    ch = log->createChannel("ANSLP_IPAP_MESSAGE");

#ifdef DEBUG
    log->dlog(ch, "in anslp_ipap_message constructor parameters another instance" );
#endif 	
}

anslp_ipap_message::~anslp_ipap_message( void )
{

}

uchar * 
anslp_ipap_message::get_message(void) const
{

#ifdef DEBUG
	log->dlog(ch, "Starting get message" );
#endif

	if (ip_message.get_message() != NULL){
		
		if (ip_message.get_require_output()){
#ifdef DEBUG
			log->dlog(ch, "The message require output" );
#endif
			return NULL;
		}
		else{
			return ip_message.get_message();
		}
	}
	else{

#ifdef DEBUG
		log->dlog(ch, "undefined message" );
#endif
		return NULL;
	}
}


bool 
anslp_ipap_message::isEqual (const anslp_mspec_object &rhs) const
{
#ifdef DEBUG
    log->dlog(ch, "starting isEqual" );
#endif 	
	
	bool val_return = false;
	const anslp_ipap_message* obj = dynamic_cast< const anslp_ipap_message* >( &rhs );
	
	if (obj != NULL)
	{
		val_return = ip_message.operator ==(obj->ip_message);
	}
	return val_return;
}

bool
anslp_ipap_message::notEqual (const anslp_mspec_object &rhs) const
{
	return !(isEqual(rhs));
}

anslp_ipap_message *
anslp_ipap_message::new_instance() const
{
	anslp_ipap_message *q = NULL;
	catch_bad_alloc( q = new anslp_ipap_message() );
	return q;
}

anslp_ipap_message *
anslp_ipap_message::copy() const
{

#ifdef DEBUG
    log->dlog(ch, "starting anslp_ipap_message copy " );
#endif	
	
	anslp_ipap_message *q = NULL;
	catch_bad_alloc( q = new anslp_ipap_message(*this) );


#ifdef DEBUG
    log->dlog(ch, "ending anslp_ipap_message copy " );
#endif	

	return q;


}

size_t 
anslp_ipap_message::get_serialized_size(coding_t coding) const
{
	
	size_t val_return;
	
#ifdef DEBUG
    log->dlog(ch, "starting get_serialized_size" );
#endif 		
	
	val_return = HEADER_LENGTH;
	val_return = val_return + ip_message.get_offset();
	
	// For GIST it is required to add padding if the message is not multiple of 4
	int num_padding = val_return %4;
	if (num_padding != 0){
		num_padding = 4 - num_padding;
	}
	
		
	val_return = val_return + num_padding;

#ifdef DEBUG
    log->dlog(ch, "Ending get_serialized_size - size:%d", val_return);
#endif 		


	return val_return;
}

bool 
anslp_ipap_message::check_body() const
{

#ifdef DEBUG
    log->dlog(ch, "starting check_body" );
#endif


	if (ip_message.get_offset() > 0 ){
		return true;
    }
    else{
		return false;
    }
}

bool 
anslp_ipap_message::equals_body(const anslp_object &other) const
{

#ifdef DEBUG
    log->dlog(ch, "starting equals_body" );
#endif

	bool val_return = false;
	const anslp_ipap_message *obj
		= dynamic_cast<const anslp_ipap_message *>(&other);
		
	
	 if (obj != NULL){
		val_return = this->isEqual(*obj);
	 }
	
#ifdef DEBUG
    log->dlog(ch, "Ending equals_body :%s", val_return ? "true" : "false" );
#endif
	return val_return;
}

const char *
anslp_ipap_message::get_ie_name() const
{
	return ie_name;
}

ostream &
anslp_ipap_message::print_attributes(ostream &os) const
{
	// TODO AM: Missing function definition
	return os;
}

bool 
anslp_ipap_message::deserialize_body(NetMsg &msg, uint16 body_length,
									 IEErrorList &err, bool skip)
{
#ifdef DEBUG
    log->dlog(ch, "starting deserialize_body: %d", body_length );
#endif

	uchar *messdef;
	int num_read=0;
	uchar *padding = new uchar[4];
	int num_padding = 0;

	messdef = (uchar *) malloc(sizeof(uchar) * body_length);
	uint32 start_pos = msg.get_pos();
	
	msg.copy_to(messdef, start_pos, body_length);
	ip_message.close();
	
	try{ 
		num_read = ip_message.ipap_import(messdef, body_length );
	
	} catch (ipap_bad_argument &e){
		log->elog(ch, "Error importing the message: %s", e.what() );
		throw IEMsgTooShort(CODING, get_category(), msg.get_pos());
	}

	// Manage the possible padding added in the origin.
	num_padding = body_length - num_read;	
	for (int i = 0 ; i < num_padding; i++ )
	{
		if (messdef[num_read + i] != 0){
			return false;
		}
	}	
	msg.set_pos(start_pos + body_length);
	
	free(messdef);
	
#ifdef DEBUG
    log->dlog(ch, "ending deserialize_body" );
#endif	
	return true;
}

void 
anslp_ipap_message::serialize_body(NetMsg &msg) const
{

#ifdef DEBUG
    log->dlog(ch, "starting serialize_body" );
#endif

	uchar *padding;
	int num_padding = 0;
	int offset = 0;
	uint32 start_pos = msg.get_pos();

#ifdef DEBUG
    log->dlog(ch, "before inserting the message" );
#endif
	
	if (get_message() == NULL){
#ifdef DEBUG
		log->dlog(ch, "message is null" );
#endif
	
	}
	
	msg.copy_from(get_message(),start_pos, ip_message.get_offset());	

#ifdef DEBUG
    log->dlog(ch, "after inserting the message" );
#endif

	msg.set_pos(start_pos + ip_message.get_offset());
	
	// For GIST it is required to add padding if the message is not multiple
	// of 4.
	offset = ip_message.get_offset();
	num_padding = offset % 4;

	if ( num_padding != 0 ){
		num_padding = 4 - num_padding; // How many additional bytes are required.
		padding = new uchar[4];
		for (int i = 0 ; i < num_padding; i++ ){
			padding[i] = 0;
		}
		uint32 start_pos2 = msg.get_pos();
		msg.copy_from(padding,start_pos2, num_padding);
		msg.set_pos(start_pos2 + num_padding);
	}

#ifdef DEBUG
    log->dlog(ch, "ending serialize_body offset" );
#endif
}

