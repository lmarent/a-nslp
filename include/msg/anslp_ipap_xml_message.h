/*! \file   anslp_ipap_xml_message.h

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
    message class of nslp ipap message when represented in xml.

    $Id: anslp_ipap_xml_message.h 748 2015-08-31 20:13:00Z amarentes $
*/

#ifndef ANSLP_IPAP_XML_MESSAGE_H
#define ANSLP_IPAP_XML_MESSAGE_H

#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>

#include "IpAp_message.h"
#include "anslp_ipap_message.h"
#include "anslp_mspec_object.h"
#include "xml_object_key.h"




namespace anslp 
{
  namespace msg {

#define ENCODING "UTF-8"
 	
typedef map<xml_object_key, ipap_template_container> 					xmlTemplateList_t;
typedef map<xml_object_key, ipap_template_container>::iterator 			xmlTemplateIterList_t;
typedef map<xml_object_key, ipap_template_container>::const_iterator 	xmlTemplateConstIterList_t;


typedef map<xml_object_key,dataRecordList_t>					xmlDataRecordList_t;
typedef map<xml_object_key,dataRecordList_t>::iterator			xmlDataRecordIterList_t;
typedef map<xml_object_key,dataRecordList_t>::const_iterator	xmlDataRecordConstIterList_t;

// This type maintains the key to use for data records.
typedef map<xml_object_key, ipap_field>						xmlDataFieldKeyList_t;
typedef map<xml_object_key, ipap_field>::iterator			xmlDataFieldKeyIterList_t;
typedef map<xml_object_key, ipap_field>::const_iterator		xmlDataFieldKeyConstIterList_t;


class anslp_ipap_xml_message : public XMLParser
{

private:
	
	Logger *log; //!< link to global logger object
	int ch;      //!< logging channel number used by objects of this class												

	static const string dataElement;
	static const string optionElement;
	
	/// name of DTDs elements by object type.
	static const char *OBJECT_DTD_XML_TAGS[];
	
	xmlTemplateList_t objectTemplates;
	
	xmlDataRecordList_t objectDataRecords;
	
	xmlDataFieldKeyList_t objectDataRecordKeys;

	ipap_template * getTemplate(xml_object_key key, uint16_t templid);

	void createXmlMessage(ipap_xml_object_type_t object_type,
						  xmlTextWriterPtr &writer,
						  xmlBufferPtr &buf );
	
	void writeTemplate(xmlTextWriterPtr &writer, ipap_template *templ);
	
	void writeTemplates(xmlTextWriterPtr &writer,
					ipap_xml_object_type_t object_type,
					map<xml_object_key, xml_object_key> &keys);
					
	void writeRecords(xmlTextWriterPtr &writer, 
					ipap_xml_object_type_t object_type,
					map<xml_object_key, xml_object_key> &keys);
	
	void createElement(xmlTextWriterPtr &writer, 
						string elementName,
						string elementId);
	
	void writeElement(xmlTextWriterPtr &writer, 
						string elementName);

	void writeFieldValue(xmlTextWriterPtr &writer, string value);

	void writeAttribute(xmlTextWriterPtr &writer, 
						string attributeTag,
						string attributeName);
	
	void createElement(xmlTextWriterPtr &writer, 
						string elementName);

	void writeDataRecord(xmlTextWriterPtr &writer, 
						ipap_template * templ, 
						ipap_data_record &g_data, 
						string elementName);
	
	void printTemplateDataRecords(xml_object_key key, 
									ipap_field dataField,
									xmlTextWriterPtr &writer);
									
	void printOptionDataRecords(xml_object_key key, 
								   ipap_field dataField,
									xmlTextWriterPtr &writer);	

	void closeElement(xmlTextWriterPtr &writer);
	
public:
	
		
    /**
    * Create a new class anslp_ipap_xml_message
    * @param By default it sets the version in IPAP and encode in true, 
    * 		    the source id is set to 0.
    */
    anslp_ipap_xml_message(const anslp_ipap_message &mes);
    
    /**
    * Create a new class anslp_ipap_message from the XML string 
    * @param XML String
    */
    anslp_ipap_message * from_message(const string,
						ipap_xml_object_type_t object_type);
  	   
	/**
	 * Create a new class anslp_ipap_xml_message copying 
	 *  from another anslp_ipap_xml_message.
	 * @param rhs 	 - message to copy from. 
	 */
	anslp_ipap_xml_message(const anslp_ipap_xml_message &rhs);
	   
	/**
	 * Destructor.
	*/
	~anslp_ipap_xml_message(void);

	/**
	 * Get the internal buffer that was exported
	 */
	string get_message(ipap_xml_object_type_t);
	
	
	
};

  } // namespace msg
} // namespace asnlp

#endif // ANSLP_IPAP_XML_MESSAGE_H
