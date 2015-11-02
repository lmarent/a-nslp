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
#include "anslp_ipap_message_splitter.h"
#include "anslp_constants.h"


namespace anslp 
{
  namespace msg {

#define ENCODING "UTF-8"
 	



class anslp_ipap_xml_message : public anslp_ipap_message_splitter
{

private:
	
	Logger *log; //!< link to global logger object
	int ch;      //!< logging channel number used by objects of this class

    //! ---------------- attributes related with the xml parsing

    //! corresponding dtd file name
    string dtdName;

    static string err, warn;

    //! callback for parser errors
    static void XMLErrorCB(void *ctx, const char *msg, ...);

    //! callback for parser warnings
    static void XMLWarningCB(void *ctx, const char *msg, ...);

    //! validates doc vs. dtd
    void validate(string root);

    //! pointer to the root of the doc
    xmlDocPtr XMLDoc;

    //! name space
    xmlNsPtr ns;
	
	
    //! Methods related with the XML validation

    inline string getDtdName() { return dtdName; }

	void createXmlMessage(xmlTextWriterPtr &writer, xmlBufferPtr &buf );

	void createElement(xmlTextWriterPtr &writer, 
						string elementName,
						string elementId);

	void createElement(xmlTextWriterPtr &writer, 
						string elementName);
	
	void writeElement(xmlTextWriterPtr &writer, string elementName);
    
    void XMLParserValidate(string dtdname, const char *buf, int len, string root);
	
    string xmlCharToString(xmlChar *in);

	void closeElement(xmlTextWriterPtr &writer);

	bool str_to_uint16(const char *str, uint16_t *res);
            
	ipap_template * getTemplate(xml_object_key key, uint16_t templid);

	void printTemplateDataRecords(xml_object_key key, 
								  xmlTextWriterPtr &writer);
									
	void printOptionDataRecords(xml_object_key key, 
								xmlTextWriterPtr &writer);	
	
	void writeTemplate(xmlTextWriterPtr &writer, ipap_template *templ);
	
	void writeTemplates(xmlTextWriterPtr &writer,
						ipap_object_type_t object_type,
						map<xml_object_key, xml_object_key> &keys);
					
	void writeRecords(xmlTextWriterPtr &writer, 
					  ipap_object_type_t object_type,
					  map<xml_object_key, xml_object_key> &keys);
	
	void writeFieldValue(xmlTextWriterPtr &writer, string value);

	void writeAttribute(xmlTextWriterPtr &writer, 
						string attributeTag,
						string attributeName);
	
	void writeDataRecord(xmlTextWriterPtr &writer, 
						ipap_template * templ, 
						ipap_data_record &g_data, 
						string elementName);

	void writeObjectTypeData(xmlTextWriterPtr &writer, 
							 ipap_object_type_t object_type);

	void writeNotRelatedTemplates(xmlTextWriterPtr &writer);
		
	
	
	/*---- Methods for reading the ipap_message from xml ---*/
	
	void ReadTemplateFields(xmlNodePtr cur, 
							uint16_t idTemplate,  
							ipap_fields_t *fields);
	
	void ReadRecords( xmlNodePtr cur, 
					  uint16_t templId,
					  anslp_ipap_message *message);
	
	
	
public:
	
    /**
    * Create a new class anslp_ipap_xml_message
    * @param By default it sets the version in IPAP and encode in true, 
    * 		    the source id is set to 0.
    */
	anslp_ipap_xml_message();
    
    /**
    * Create a new class anslp_ipap_message from the XML string 
    * @param XML String
    */
    anslp_ipap_message * from_message(const string);
  	   
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
	string get_message(const anslp_ipap_message &mes);
	
	
	
};

  } // namespace msg
} // namespace asnlp

#endif // ANSLP_IPAP_XML_MESSAGE_H
