/*! \file   anslp_ipap_xml_message.cpp

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
    message class of nslp ipap message represented by a xml

    $Id: anslp_ipap_xml_message.cpp 748 2015-09-07 18:20:00Z amarentes $
*/

#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>
#include "anslp_ipap_xml_message.h"
#include "anslp_ipap_exception.h"

using namespace anslp::msg;

const string anslp_ipap_xml_message::dataElement = "FIELD";
const string anslp_ipap_xml_message::optionElement = "PREF";

const char *anslp_ipap_xml_message::OBJECT_DTD_XML_TAGS[] = { "auctionfile.dtd", 
															  "bidfile.dtd",
															  "allocation.dtd" };

anslp_ipap_xml_message::anslp_ipap_xml_message(const anslp_ipap_message &mes):
	XMLParser()
{

    log = Logger::getInstance();
    ch = log->createChannel("ANSLP_IPAP_XML_MESSAGE");

#ifdef DEBUG
	log->dlog(ch, "Starting constructor anslp_ipap_xml_message");
#endif	

	uint16_t templid;
	ipap_xml_object_type_t obj_type = IPAP_INVALID;
	
    // insert the data records associated with the template.
    dateRecordListConstIter_t iter;
    for ( iter = (mes.ip_message).begin(); iter != (mes.ip_message).end(); iter++)
	{
		
		ipap_data_record g_data = *iter;
		templid = g_data.get_template_id();
		ipap_template *templ = (mes.ip_message).get_template(templid);	
		if (templ == NULL){
#ifdef DEBUG
			log->dlog(ch, "template with id: %d was not found in message", templid );
#endif
			throw anslp_ipap_bad_argument("template not found in message");			
		}
		else{
		
			ipap_templ_type_t templ_type = templ->get_type();
			
			// Obtains the template Key
			ipap_field_key field_key = mes.get_template_key(templ_type);
			ipap_field fieldTemplate = templ->get_field(field_key.get_eno(), 
								field_key.get_ftype());
								
			ipap_value_field value = g_data.get_field(field_key.get_eno(), 
												field_key.get_ftype());
												
			// Obtains the the dataRecord Key
			ipap_field_key fieldDataRecordKey = mes.get_record_key(templ_type);
			
			ipap_field fieldDataRecord = templ->get_field(fieldDataRecordKey.get_eno(), 
								fieldDataRecordKey.get_ftype());
			
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
			
			// Creates the xml object key as Id of the document and the 
			// type of the document
			xml_object_key xml_key(obj_type, fieldTemplate.writeValue(value));
			
			xmlTemplateIterList_t iter = objectTemplates.find(xml_key);
			if (iter != objectTemplates.end()){
				(objectTemplates[xml_key]).add_template(templ->copy());
				(objectDataRecords[xml_key]).push_back(g_data);
				objectDataRecordKeys[xml_key] = fieldDataRecord;
			}
			else{
				objectTemplates[xml_key] = ipap_template_container();
				(objectTemplates[xml_key]).add_template(templ->copy());
				objectDataRecords[xml_key] = dataRecordList_t();
				(objectDataRecords[xml_key]).push_back(g_data);	
				objectDataRecordKeys[xml_key] = fieldDataRecord;		
			}			
		}
	}

#ifdef DEBUG
	log->dlog(ch, "Ending constructor anslp_ipap_xml_message");
#endif
}
	   
/**
* Create a new class anslp_ipap_xml_message copying 
 *  from another anslp_ipap_xml_message.
 * @param rhs 	 - message to copy from. 
 */
anslp_ipap_xml_message::anslp_ipap_xml_message(const anslp_ipap_xml_message &rhs):
	XMLParser()
{

    log = Logger::getInstance();
    ch = log->createChannel("ANSLP_IPAP_XML_MESSAGE");

#ifdef DEBUG
	log->dlog(ch, "Starting constructor anslp_ipap_xml_message from another instance");
#endif	

	// Copy object templates
	xmlTemplateConstIterList_t iterTempl;
	for (iterTempl = rhs.objectTemplates.begin(); 
			iterTempl != rhs.objectTemplates.end(); ++iterTempl){
		objectTemplates[iterTempl->first] = (iterTempl->second);
	}
	
	// Copy object data records
	xmlDataRecordConstIterList_t iterRecord;
	for (iterRecord = rhs.objectDataRecords.begin(); 
			iterRecord != rhs.objectDataRecords.end(); ++iterRecord){
		objectDataRecords[iterRecord->first] = iterRecord->second;
	}

#ifdef DEBUG
	log->dlog(ch, "Ending constructor anslp_ipap_xml_message from another instance");
#endif

}
	   
/**
 * Destructor.
*/
anslp_ipap_xml_message::~anslp_ipap_xml_message()
{

}

void
anslp_ipap_xml_message::createXmlMessage(ipap_xml_object_type_t object_type,
										  xmlTextWriterPtr &writer,
										  xmlBufferPtr &buf )
{
	int rc;

#ifdef DEBUG
	log->dlog(ch, "Starting createXmlMessage ObjectType:%d", object_type);
#endif

    /* Create a new XML buffer, to which the XML document will be
     * written */
    buf = xmlBufferCreate();
    if (buf == NULL) {
#ifdef DEBUG
		log->dlog(ch, "testXmlwriterMemory: Error creating the xml buffer" );
#endif
		throw anslp_ipap_memory_allocation("Error creating the xml buffer");
    }
	
    /* Create a new XmlWriter for memory, with no compression.
     * Remark: there is no compression for this kind of xmlTextWriter */
    writer = xmlNewTextWriterMemory(buf, 0);
    if (writer == NULL) {
#ifdef DEBUG
		log->dlog(ch, "testXmlwriterMemory: Error creating the xml writer" );
#endif
		throw anslp_ipap_memory_allocation("Error creating the xml writer");
    }

    /* Start the document with the xml default for the version,
     * encoding UTF-8 and the default for the standalone
     * declaration. */
    rc = xmlTextWriterStartDocument(writer, NULL, ENCODING, NULL);
    if (rc < 0) {
#ifdef DEBUG
		log->dlog(ch, "testXmlwriterMemory: Error at xmlTextWriterStartDocument" );
#endif
        throw anslp_ipap_bad_argument("Error at xmlTextWriterStartDocument");
    }


    rc = xmlTextWriterStartDTD(writer, 
				BAD_CAST anslp_ipap_message::OBJECT_HEADER_XML_TAGS[object_type], 
				NULL,
				BAD_CAST OBJECT_DTD_XML_TAGS[object_type] );
    if (rc < 0) {
#ifdef DEBUG
		log->dlog(ch, "testXmlwriterMemory: Error at xmlTextWriterStartDocument" );
#endif
        throw anslp_ipap_bad_argument("Error at xmlTextWriterStartDocument");
    }
    
    rc = xmlTextWriterEndDTD( writer );
    
    if (rc < 0) {
#ifdef DEBUG
		log->dlog(ch, "testXmlwriterMemory: Error at xmlTextWriterStartDocument" );
#endif
        throw anslp_ipap_bad_argument("Error at xmlTextWriterStartDocument");
    }

	
#ifdef DEBUG
	log->dlog(ch, "Ending createXmlMessage -Object %s", 
		anslp_ipap_message::OBJECT_HEADER_XML_TAGS[object_type]);
#endif
}


void anslp_ipap_xml_message::writeElement(xmlTextWriterPtr &writer, 
								string elementName)
{

	int rc;

#ifdef DEBUG
	log->dlog(ch, "Starting writeElement elementName:%s", elementName.c_str());
#endif	

    /* Start an element */
    rc = xmlTextWriterStartElement(writer, BAD_CAST elementName.c_str());
    if (rc < 0) {
#ifdef DEBUG
		log->dlog(ch, "Error at xmlTextWriterStartElement" );
#endif
        throw anslp_ipap_bad_argument("Error at xmlTextWriterStartElement");
    }

#ifdef DEBUG
	log->dlog(ch, "Ending writeElement");
#endif	

}

void 
anslp_ipap_xml_message::writeFieldValue(xmlTextWriterPtr &writer, 
										string value)
{										
	int rc = xmlTextWriterWriteFormatString(writer, "%s", value.c_str());
		
	if (rc < 0) {
#ifdef DEBUG
		log->dlog(ch, "testXmlwriterMemory: Error at xmlTextWriterWriteFormatElement" );
#endif
		throw anslp_ipap_bad_argument("Error at xmlTextWriterWriteFormatElement");
	}
}

void 
anslp_ipap_xml_message::writeAttribute(xmlTextWriterPtr &writer, 
										string attributeTag,
										string attributeName)
{

#ifdef DEBUG
	log->dlog(ch, "Starting writeAttribute attributeName:%s", attributeName.c_str());
#endif	

	int rc;

	rc = xmlTextWriterWriteAttribute(writer, BAD_CAST attributeTag.c_str(),
										 BAD_CAST attributeName.c_str());
	if (rc < 0) {
#ifdef DEBUG
		log->dlog(ch, "Error at xmlTextWriterWriteAttribute %s", 
						attributeName.c_str() );
#endif		
		throw anslp_ipap_bad_argument("Error at xmlTextWriterWriteAttribute");
	}

#ifdef DEBUG
	log->dlog(ch, "Ending writeAttribute");
#endif	

}

void
anslp_ipap_xml_message::createElement(xmlTextWriterPtr &writer, 
								string elementName, string elementId)
{

	writeElement(writer, elementName);

	writeAttribute(writer, "ID", elementId);

}

void
anslp_ipap_xml_message::createElement(xmlTextWriterPtr &writer, 
								string elementName)
{
	writeElement(writer, elementName);
}


ipap_template * 
anslp_ipap_xml_message::getTemplate(xml_object_key key, uint16_t templid)
{

#ifdef DEBUG
	log->dlog(ch, "Starting getTemplate - key:%s templid:%d", 
					key.to_string().c_str(), templid);
#endif
	ipap_template * templ;
	xmlTemplateIterList_t iter = objectTemplates.find(key);
	if (iter != objectTemplates.end())
	{
		try{
			templ = (iter->second).get_template(templid);
		}catch(ipap_bad_argument &e){
			throw anslp_ipap_bad_argument("Template not found in the container");
		}
	
	}else{
		throw anslp_ipap_bad_argument("Template not found in the container");
	}

#ifdef DEBUG
	log->dlog(ch, "Ending getTemplate - template type:%d", templ->get_type());
#endif

	return templ;
}

void
anslp_ipap_xml_message::writeDataRecord(xmlTextWriterPtr &writer, 
				ipap_template * templ, 
				ipap_data_record &g_data,
				string elementName)
{

#ifdef DEBUG
	log->dlog(ch, "Starting writeDataRecord - Element: %s", elementName.c_str());
#endif

	int rc;
	ipap_templ_type_t type = templ->get_type();
	fieldDataListIter_t iter;
	for (iter = g_data.begin(); iter != g_data.end(); ++iter)
	{
		
		// Bring the definition of the field from the template.
		ipap_field field = templ->get_field( (iter->first).get_eno(), 
											  (iter->first).get_ftype() );
		
		createElement(writer, elementName);

		string attribute = field.get_field_type().xml_name;
		writeAttribute(writer, "NAME", attribute);
		writeFieldValue(writer, field.writeValue(iter->second));
		
		closeElement(writer);	
	}
}

void
anslp_ipap_xml_message::printTemplateDataRecords(xml_object_key key, 
					ipap_field dataField,
					xmlTextWriterPtr &writer)
{

#ifdef DEBUG
	log->dlog(ch, "Starting printTemplateDataRecords %s", key.to_string().c_str());
#endif
	
	xmlDataRecordIterList_t iterRecord;
	for (iterRecord = objectDataRecords.begin(); 
			iterRecord != objectDataRecords.end(); ++iterRecord){
		if ((iterRecord->first) == key)
		{
			dateRecordListIter_t iter_data;
		
			for (iter_data = (iterRecord->second).begin(); 
					iter_data != (iterRecord->second).end(); ++iter_data)
			{
				ipap_data_record g_data = *iter_data;
				ipap_template * templ = getTemplate(key, g_data.get_template_id());
				if ((templ->get_type() == IPAP_SETID_AUCTION_TEMPLATE)
				  || (templ->get_type() == IPAP_SETID_BID_TEMPLATE)
				  || (templ->get_type() == IPAP_SETID_ALLOCATION_TEMPLATE)){
					string elementName = string(ipap_template::TEMPLATE_XML_TAGS[templ->get_type()]);

					ipap_field_key dataKey = ipap_field_key(dataField.get_field_type().eno,  
															dataField.get_field_type().ftype);
					ipap_value_field val = g_data.get_field(dataKey);										
					createElement(writer, elementName, 
											dataField.writeValue(val));
					char *buff = (char *) malloc (sizeof(char)*32);
					sprintf(buff, "%u", g_data.get_template_id());
					writeAttribute(writer, "TEMPLATE_ID", buff);
					free(buff);
					
					writeDataRecord(writer, templ, g_data, dataElement);
					closeElement(writer);
				}
			}
		}
	}
}

void
anslp_ipap_xml_message::printOptionDataRecords(xml_object_key key, 
					ipap_field dataField,
					xmlTextWriterPtr &writer)
{	

#ifdef DEBUG
	log->dlog(ch, "Starting printOptionDataRecords");
#endif

	xmlDataRecordIterList_t iterRecord;
	for (iterRecord = objectDataRecords.begin(); 
			iterRecord != objectDataRecords.end(); ++iterRecord){
		if ((iterRecord->first) == key)
		{
			dateRecordListIter_t iter_data;
		
			for (iter_data = (iterRecord->second).begin(); 
					iter_data != (iterRecord->second).end(); ++iter_data){
				ipap_data_record g_data = *iter_data;
				ipap_template * templ = getTemplate(key, g_data.get_template_id());
				if ((templ->get_type() == IPAP_OPTNS_AUCTION_TEMPLATE)
				  || (templ->get_type() == IPAP_OPTNS_BID_TEMPLATE)
				  || (templ->get_type() == IPAP_OPTNS_ALLOCATION_TEMPLATE)){
					string elementName = string(ipap_template::TEMPLATE_XML_TAGS[templ->get_type()]);
					
					ipap_field_key dataKey = ipap_field_key(dataField.get_field_type().eno,  
															dataField.get_field_type().ftype);
					ipap_value_field val = g_data.get_field(dataKey);
					createElement(writer, elementName, 
									dataField.writeValue(val));
					char *buff = (char *) malloc (sizeof(char)*32);
					sprintf(buff, "%u", g_data.get_template_id());
					writeAttribute(writer, "TEMPLATE_ID", buff);
					free(buff);
					
					writeDataRecord(writer, templ, g_data, optionElement);
					closeElement(writer);
				}
			}
		}
	}
}

void
anslp_ipap_xml_message::closeElement(xmlTextWriterPtr &writer)
{

#ifdef DEBUG
	log->dlog(ch, "Starting closeElement");
#endif
    
    int rc;    
    /* Close the element. */
    rc = xmlTextWriterEndElement(writer);
    if (rc < 0) {
#ifdef DEBUG
		log->dlog(ch, "testXmlwriterMemory: Error at xmlTextWriterEndElement" );
#endif
		throw anslp_ipap_bad_argument("Error at xmlTextWriterEndElement");
    }
}

void 
anslp_ipap_xml_message::writeTemplate(xmlTextWriterPtr &writer, 
											ipap_template *templ)
{
	char *buff = (char *) malloc(sizeof(char) * 32);
	 
	sprintf(buff, "%u", templ->get_template_id());
	createElement(writer, "TEMPLATE", buff);
	
	sprintf(buff, "%u", templ->get_type());
	writeAttribute(writer, "TYPE",	buff);
	
	int numFields = templ->get_numfields();
	for (int i = 0; i < numFields; ++i )
	{
		ipap_field field = (templ->get_field(i)).elem;
		ipap_field_type_t type = field.get_field_type();
		
		createElement(writer, "TFIELD");
	
		sprintf(buff, "%d", type.eno);
		writeAttribute(writer, "ENO", buff);		

		sprintf(buff, "%d", type.ftype);
		writeAttribute(writer, "FTYPE", buff);
		
		sprintf(buff, "%d", type.coding);
		writeAttribute(writer, "CODING", buff);

		sprintf(buff, "%d", (int) type.length);
		writeAttribute(writer, "LENGTH", buff);

		writeAttribute(writer, "XML_NAME",	type.xml_name);

		writeFieldValue(writer, type.name);

		closeElement(writer);

	}
	free(buff);	
	// Close the Template.
	closeElement(writer);
}

void 
anslp_ipap_xml_message::writeTemplates(xmlTextWriterPtr &writer, 
					ipap_xml_object_type_t object_type,
					map<xml_object_key, xml_object_key> &keys)
{

	// Build a list of the templates to write, a template 
	// could be used for more than one xml_object.
	templateList_t templates;

	xmlTemplateIterList_t iterTempl;
	for (iterTempl = objectTemplates.begin(); 
				iterTempl != objectTemplates.end(); ++iterTempl)
	{
		std::list<int> list = (iterTempl->second).get_template_list();
		
		std::list<int>::iterator iterContTempl;
		for ( iterContTempl = list.begin(); 
					iterContTempl != list.end(); ++iterContTempl)
		{
			uint16_t templId = *iterContTempl;
			templates[templId] = (iterTempl->second).get_template(*iterContTempl);
		}
	}	
	
	// Loop throught templates and print.
	templateListIter_t iterContTempl;
	for ( iterContTempl = templates.begin(); 
					iterContTempl != templates.end(); ++iterContTempl)
	{	
		writeTemplate(writer, iterContTempl->second);
	}
}


void
anslp_ipap_xml_message::writeRecords(xmlTextWriterPtr &writer, 
					ipap_xml_object_type_t object_type,
					map<xml_object_key, xml_object_key> &keys)
{

	bool elementWrite = false;
	ipap_field dataField;
	// Go over the set of keys sort them and print the xml
	xml_object_key lastKey;
	map<xml_object_key, xml_object_key>::iterator iterKeys;
	for (iterKeys = keys.begin(); iterKeys != keys.end(); ++iterKeys){
		if (lastKey != iterKeys->first){
			// Close the previous element
			if (elementWrite){
				closeElement(writer);
			}
			elementWrite = true;
			lastKey = iterKeys->first;
				
			// Bring element Id for data records.
			xmlDataFieldKeyIterList_t itDataField = objectDataRecordKeys.find(lastKey);
			dataField = itDataField->second;
				
			// Bring element Id for this document.
			cout << lastKey.to_string() << endl;
				
			// Create a new element in the xml
			createElement(writer, 
				string(anslp_ipap_message::OBJECT_LINE_XML_TAGS[object_type]),
				lastKey.get_Id());
		}
		// print all template data records
		printTemplateDataRecords(lastKey, dataField, writer);
			
		// print all option data records
		printOptionDataRecords(lastKey, dataField, writer);
	}
	// Close the final record.
	closeElement(writer);
}

/**
 * Get the internal buffer that was exported
 */
string
anslp_ipap_xml_message::get_message(ipap_xml_object_type_t object_type)
{

#ifdef DEBUG
	log->dlog(ch, "Starting get_message");
#endif

	// Build set of distinct keys
	map<xml_object_key, xml_object_key> keys;
	
	xmlTemplateIterList_t iterTempl;
	for (iterTempl = objectTemplates.begin(); 
			iterTempl != objectTemplates.end(); ++iterTempl){
		if ((iterTempl->first).get_object_type() == object_type){
			keys.insert ( std::pair<xml_object_key, xml_object_key>
				(iterTempl->first,iterTempl->first) );
		}
	}
	
	xmlDataRecordIterList_t iterRecord;
	for (iterRecord = objectDataRecords.begin(); 
			iterRecord != objectDataRecords.end(); ++iterRecord){
		if ((iterRecord->first).get_object_type() == object_type){
			keys.insert ( std::pair<xml_object_key, xml_object_key>
				(iterRecord->first,iterRecord->first) );
		}
	}
	
	if (keys.size() > 0){

		xmlTextWriterPtr writer;
		xmlBufferPtr buf;
		createXmlMessage(object_type, writer, buf);
		
		createElement(writer,
			anslp_ipap_message::OBJECT_HEADER_XML_TAGS[object_type]);
			
		// Write Templates associated with the object type
		writeTemplates(writer, object_type, keys);
		
		// Write Records associated with the object type
		writeRecords(writer, object_type, keys);
			
		// Close the header.
		closeElement(writer);
			
		xmlFreeTextWriter(writer);
		
		string content((const char *) buf->content);
			
		xmlBufferFree(buf);
			
		return content;
		
	}
	else{
		return string();
	}

}


static bool
anslp_ipap_xml_message::str_to_uint16(const char *str, uint16_t *res)
{
	char *end;
	errno = 0;
	intmax_t val = strtoimax(str, &end, 10);
	if (errno == ERANGE || val < 0 || val > UINT16_MAX || end == str || *end != '\0')
		return false;
	*res = (uint16_t) val;
	return true;
}

void
anslp_ipap_xml_message::ReadTemplateFields(xmlNodePtr cur, 
					unit16_t idTemplate,  anslp_ipap_message *message)
{
	while (cur != NULL) 
	{
        // get TFIELD
        if ((!xmlStrcmp(cur->name, (const xmlChar *)"TFIELD")) && (cur->ns == ns)) {
			uint32_t sEno = strtoul(xmlCharToString(xmlGetProp(cur, (const xmlChar *)"ENO")));
			uint16_t sFType = strtoul(xmlCharToString(xmlGetProp(cur, (const xmlChar *)"FTYPE")));
			uint16_t sCoding = strtoul(xmlCharToString(xmlGetProp(cur, (const xmlChar *)"CODING")));
			uint16_t sLength = strtoul(xmlCharToString(xmlGetProp(cur, (const xmlChar *)"LENGTH")));
			string sXmlName = xmlCharToString(xmlGetProp(cur, (const xmlChar *)"XML_NAME"));
			(message->ip_message).add_field(idTemplate, sEno, sFType, sLength);
		}
		cur = cur->next; 
	}
}

void 
anslp_ipap_xml_message::ReadDataRecords(xmlNodePtr cur, 
			map< unit16_t, unit16_t > &templateAssociation,
			ipap_xml_object_type_t object_type
			anslp_ipap_message *message)
{

    xmlNodePtr cur2;
    ipap_templ_type_t tempType = object_type;

	while (cur != NULL) 
	{
        if ((!xmlStrcmp(cur->name, 
			(const xmlChar *) ipap_template::TEMPLATE_XML_TAGS[tempType])) && (cur->ns == ns)) {
			
			uint16_t uid;
			struing sId = xmlCharToString(xmlGetProp(cur, (const xmlChar *)"TEMPLATE_ID"));
			str_to_uint16(sId.c_str(), &uid);
			uint16_t newUid = templateAssociatio[uid];
			ipap_template *templ = (message->ip_message).get_template(newUid);
			
			cur2 = cur->xmlChildrenNode;
			while (cur2 != NULL) 
			{
				
				
				cur2 = cur2->next; 
			}
		}
		cur = cur->next; 
	}
}

anslp_ipap_message *
anslp_ipap_xml_message::from_message(const string str, 
										ipap_xml_object_type_t object_type)
{
	
	string DTD_FILENAME = DIRECTORYFILE_DTD + string(OBJECT_DTD_XML_TAGS[object_type]);
	
	XMLParserValidate(DTD_FILENAME, str.c_str(), str.length(),
						string(anslp_ipap_message::OBJECT_HEADER_XML_TAGS[object_type]));
	
	anslp_ipap_message *message = new anslp_ipap_message();

	xmlNodePtr cur, cur2, cur3;
    cur = xmlDocGetRootElement(XMLDoc);
	cur = cur->xmlChildrenNode;
	
	map< unit16_t, unit16_t > templateAssociation;
	
	// First, it is required to read templates.
    while (cur != NULL) {
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"TEMPLATE")) && (cur->ns == ns)) {
			
			// Read the Id of the template
			uint16_t uid;
			string id = xmlCharToString(xmlGetProp(cur, (const xmlChar *)"ID"));
			str_to_uint16(is.c_str(), &uid);
			
			// read the template type.
			string type = xmlCharToString(xmlGetProp(cur, (const xmlChar *)"TYPE"));
			int itype = atoi(type.c_str());

			// read the number of fields in the template.
			string numFields = xmlCharToString(xmlGetProp(cur, (const xmlChar *)"NUM_FIELDS"));
			int inumFields = atoi(numFields.c_str());
			
			uint16_t newUid = (message->ip_message).new_data_template( inumFields, itype );
			
			cur2 = cur->xmlChildrenNode;
			ReadTemplateFields(cur2, newUid, message);
			templateAssociation[uid] = newUid;
		}
        cur = cur->next;
    }
    
    cur = xmlDocGetRootElement(XMLDoc);
    cur = cur->xmlChildrenNode;
    // Second, it is required to read data records.
    while (cur != NULL) {
		if ((!xmlStrcmp(cur->name, (const xmlChar *) 
				anslp_ipap_message::OBJECT_LINE_XML_TAGS[object_type] )) && (cur->ns == ns)) {
			cur2 = cur->xmlChildrenNode;
			ReadDataRecords(cur2, templateAssociation, message);
		}
	}
}
