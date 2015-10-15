/*!  \file   anslp_ipap_message_splitter.h 

    Copyright 2014-2015 Universidad de los Andes, Bogota, Colombia
    This file is part of Network Auction Manager System (NETAUM).
    NETAUM is free software; you can redistribute it and/or modify 
    it under the terms of the GNU General Public License as published by 
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.
    
    NETAUM is distributed in the hope that it will be useful, 
    but WITHOUT ANY WARRANTY; without even the implied warranty of 
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with this software; if not, write to the Free Software 
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
    
    Description:
    
    Splits message data by object key ( Auctions, Bids, Allocations).
    $Id: anslp_ipap_message_splitter.h 748 2015-10-07 11:56:00Z amarentes $
*/

#ifndef _ANSLP_IPAP_MESSAGE_SPLITTER_H_
#define _ANSLP_IPAP_MESSAGE_SPLITTER_H_

#include "Logger.h"
#include "IpAp_template_container.h"
#include "IpAp_message.h"
#include "anslp_ipap_message.h"

#include "xml_object_key.h"

namespace anslp {

namespace msg {

typedef map<xml_object_key, ipap_template_container> 					xmlTemplateList_t;
typedef map<xml_object_key, ipap_template_container>::iterator 			xmlTemplateIterList_t;
typedef map<xml_object_key, ipap_template_container>::const_iterator 	xmlTemplateConstIterList_t;


typedef map<xml_object_key,dataRecordList_t>					xmlDataRecordList_t;
typedef map<xml_object_key,dataRecordList_t>::iterator			xmlDataRecordIterList_t;
typedef map<xml_object_key,dataRecordList_t>::const_iterator	xmlDataRecordConstIterList_t;


class anslp_ipap_message_splitter 
{

  private:
	Logger *log; //!< link to global logger object
	int ch;      //!< logging channel number used by objects of this class
  
  protected: 
	
	//! Templates that come within the ipap_message and not related with a record data 
	ipap_template_container templates;
	
	//! Relates the templates with each object key 
	//! (i.e., auction_id, bid_id, allocation_id)
	xmlTemplateList_t objectTemplates;
	
	//! Relates records with each object key 
	//! (i.e., auction_id, bid_id, allocation_id)	
	xmlDataRecordList_t objectDataRecords;
	
	void split(const anslp_ipap_message &mes);
	
  public:
    
    anslp_ipap_message_splitter();
    
    ~anslp_ipap_message_splitter();

};

}	// namespace msg

}	// namespace anslp 


#endif // _IPAP_MESSAGE_SPLITTER_H_
