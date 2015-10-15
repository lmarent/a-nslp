/*!  \file   anslp_ipap_message_splitter.cpp 

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
    $Id: ipap_message_splitter.cpp 748 2015-10-07 11:56:00Z amarentes $
*/

#include "anslp_ipap_message_splitter.h"

using namespace anslp::msg;

anslp_ipap_message_splitter::anslp_ipap_message_splitter()
{
    log = Logger::getInstance();
    ch = log->createChannel("ANLP_IPAP_MESSAGE_SPLITTER");
    	
#ifdef DEBUG
	log->dlog(ch, "Starting constructor anslp_ipap_message_splitter");
#endif	    	
}

anslp_ipap_message_splitter::~anslp_ipap_message_splitter()
{

}

void anslp_ipap_message_splitter::split( const anslp_ipap_message &message )
{

#ifdef DEBUG
	log->dlog(ch, "Starting split");
#endif

	uint16_t templid;

	ipap_xml_object_type_t obj_type = IPAP_INVALID;

	// Templates that are included because they have datarecords associated in the message.
	map<uint16_t, uint16_t> templatesIncluded;

    // insert the data records associated with the template.
    dateRecordListConstIter_t iter;


    for ( iter = (message.ip_message).begin(); iter != (message.ip_message).end(); iter++)
	{
		
		ipap_data_record g_data = *iter;
		templid = g_data.get_template_id();
		ipap_template *templ = (message.ip_message).get_template_object(templid);

		
		if (templ == NULL){
#ifdef DEBUG
			log->dlog(ch, "template with id: %d was not found in message", templid );
#endif
			throw Error("anslp_ipap_xml_message: required template not included in message");

		}
		else{
		
			// Include them in the templated included.
			templatesIncluded[templid] = templid;
			
			ipap_templ_type_t templ_type = templ->get_type();

#ifdef DEBUG
			log->dlog(ch, " template sucesfully read uid%d type:%d", templid, templ_type);
#endif			
			// Obtains the template Key
			try{
				set<ipap_field_key> keys = ipap_template::getTemplateTypeKeys(templ_type);
				
				string dataKey;
				
				for (set<ipap_field_key>::iterator kIter = keys.begin(); kIter != keys.end(); ++kIter ){

					ipap_field field = templ->get_field(kIter->get_eno(), 
															kIter->get_ftype());

					ipap_value_field value = g_data.get_field(kIter->get_eno(), 
													kIter->get_ftype());
					
					dataKey = dataKey + "%" + field.writeValue(value);
				}
																
#ifdef DEBUG
				log->dlog(ch, " template sucesfully read 1b");
#endif											
				switch (templ_type)
				{
					case IPAP_SETID_AUCTION_TEMPLATE:
					case IPAP_OPTNS_AUCTION_TEMPLATE:
						obj_type = IPAP_AUCTION;
						break;

					case IPAP_SETID_BID_TEMPLATE:
					case IPAP_OPTNS_BID_TEMPLATE:
						obj_type = IPAP_BID;
						break;
					
					case IPAP_SETID_ALLOCATION_TEMPLATE:
					case IPAP_OPTNS_ALLOCATION_TEMPLATE:
						obj_type = IPAP_ALLOCATION;
						break;
				}

#ifdef DEBUG
				log->dlog(ch, " read key :%s ", dataKey.c_str());
#endif

				// Creates the xml object key as Id of the document and the 
				// type of the document
				xml_object_key xml_key(obj_type, dataKey);
				
				xmlTemplateIterList_t iter = objectTemplates.find(xml_key);
				if (iter != objectTemplates.end()){
					(objectTemplates[xml_key]).add_template(templ->copy());
					(objectDataRecords[xml_key]).push_back(g_data);
				}
				else{
					objectTemplates[xml_key] = ipap_template_container();
					(objectTemplates[xml_key]).add_template(templ->copy());
					objectDataRecords[xml_key] = dataRecordList_t();
					(objectDataRecords[xml_key]).push_back(g_data);	
				}			
			} catch (ipap_bad_argument &){
				throw Error("anslp_ipap_xml_message: error while reading data record");
			}	
		}
	}

	// Copy templates from message thar are not related with a record data
	std::list<int>::iterator iterTemp;
	std::list<int> tmplList = (message.ip_message).get_template_list();
	
	for ( iterTemp = tmplList.begin(); iterTemp != tmplList.end(); ++iterTemp )	{
		map<uint16_t, uint16_t>::iterator found = templatesIncluded.find(*iterTemp);
		
		// Only include this templates if there have been not included before.
		if ( found == templatesIncluded.end()){
			templates.add_template((message.ip_message).get_template_object(*iterTemp));
		}	
	}

#ifdef DEBUG
	log->dlog(ch, "Ending split");
#endif

}
