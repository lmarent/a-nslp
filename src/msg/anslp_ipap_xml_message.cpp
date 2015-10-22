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

#include <inttypes.h>
#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>
#include "anslp_ipap_xml_message.h"
#include "anslp_ipap_exception.h"

using namespace anslp::msg;

string anslp_ipap_xml_message::err, anslp_ipap_xml_message::warn;

anslp_ipap_xml_message::anslp_ipap_xml_message():
anslp_ipap_message_splitter(), dtdName(""), XMLDoc(NULL), ns(NULL)
{

    log = Logger::getInstance();
    ch = log->createChannel("ANSLP_IPAP_XML_MESSAGE");

#ifdef DEBUG
	log->dlog(ch, "Starting constructor anslp_ipap_xml_message");
#endif	

}

anslp_ipap_xml_message::~anslp_ipap_xml_message()
{

#ifdef DEBUG
	log->dlog(ch, "Starting destructor anslp_ipap_xml_message");
#endif	

    if (ns != NULL) {
        xmlFreeNs(ns);
    }

    if (XMLDoc != NULL) {
		if  (XMLDoc->extSubset != NULL) {
			xmlFreeDtd(XMLDoc->extSubset);
			XMLDoc->extSubset = NULL;
		}
	}

    if (XMLDoc != NULL) {
        xmlFreeDoc(XMLDoc);
    }

    xmlCleanupParser();

}

void
anslp_ipap_xml_message::XMLParserValidate(string _dtdname, const char *buf, int len, string root)
{

#ifdef DEBUG
    log->dlog(ch, "DTD file %s,  root %s", _dtdname.c_str(), root.c_str() );
#endif
 
 
    dtdName = _dtdname;

    try {

        xmlInitParser();

        xmlSetGenericErrorFunc(NULL, XMLErrorCB);

        XMLDoc = xmlParseMemory(buf, len); 	    
	
        if (XMLDoc == NULL) {
            throw Error("XML document parse error");
        }

        validate(root);
	
    } catch (Error &e) {
        if (XMLDoc != NULL) {
            xmlFreeDoc(XMLDoc);
            XMLDoc = NULL;
        }

        if (!warn.empty()) {
            log->wlog(ch, "%s", warn.c_str());
        }

        if (!err.empty()) {
            log->elog(ch, "%s", err.c_str());
        }

        throw(e);
    }
}

void anslp_ipap_xml_message::validate(string root)
{

#ifdef DEBUG
    log->dlog(ch, "validate %s", root.c_str() );
#endif


    xmlNodePtr cur = NULL;
    xmlDtdPtr dtd = NULL;
    xmlValidCtxt cvp;

    try {
        dtd = xmlParseDTD(NULL, (const xmlChar *) dtdName.c_str());
        if (dtd == NULL) 
        {
            throw Error("Could not parse DTD %s", dtdName.c_str());
        } else {
            memset(&cvp, 0, sizeof(cvp));
            cvp.userData = this;
            cvp.error = (xmlValidityErrorFunc) XMLErrorCB;
            cvp.warning = (xmlValidityWarningFunc) XMLWarningCB;
        
            if (!xmlValidateDtd(&cvp, XMLDoc, dtd)) {
                throw Error("xml does not validate against %s", dtdName.c_str());
            }
	    
            cur = xmlDocGetRootElement(XMLDoc);
            if (cur == NULL) {
                throw Error("empty XML document");
            }
            if (xmlStrcmp(cur->name, (const xmlChar *) root.c_str())) {
                throw Error("document of the wrong type, root node = %s", cur->name);
            }

            ns = xmlSearchNsByHref(XMLDoc,cur,NULL);
            // add as external subset
            XMLDoc->extSubset = dtd;
        }
    } catch (Error &e) {
        if (ns != NULL) {
            xmlFreeNs(ns);
            ns = NULL;
        }
        if (dtd != NULL) {
            xmlFreeDtd(dtd);
            dtd = NULL;
            ns = NULL;
        }
        if (XMLDoc != NULL) {
            xmlFreeDoc(XMLDoc);
            XMLDoc = NULL;
        }

        if (!warn.empty()) {
            log->wlog(ch, "%s", warn.c_str());
        }

        if (!err.empty()) {
            log->elog(ch, "%s", err.c_str());
        }
		
        throw(e);
    }
}

void anslp_ipap_xml_message::XMLErrorCB(void *ctx, const char *msg, ...)
{
    char buf[8096];
    va_list argp;

    va_start( argp, msg );
    vsprintf(buf, msg, argp);
    vfprintf(stderr, msg, argp);
    va_end( argp );

    err += buf;
}

void anslp_ipap_xml_message::XMLWarningCB(void *ctx, const char *msg, ...)
{
    char buf[8096];
    va_list argp;

    va_start( argp, msg );
    vsprintf(buf, msg, argp); 
    vfprintf(stderr, msg, argp);
    va_end( argp );

    warn += buf;
}

string anslp_ipap_xml_message::xmlCharToString(xmlChar *in)
{
    string out = "";

    if (in != NULL) {
        out = (char *) in;
        xmlFree(in);
    }

    return out;
}
	   
/**
* Create a new class anslp_ipap_xml_message copying 
 *  from another anslp_ipap_xml_message.
 * @param rhs 	 - message to copy from. 
 */
anslp_ipap_xml_message::anslp_ipap_xml_message(const anslp_ipap_xml_message &rhs)
{

    log = Logger::getInstance();
    ch = log->createChannel("ANSLP_IPAP_XML_MESSAGE");

#ifdef DEBUG
	log->dlog(ch, "Starting constructor anslp_ipap_xml_message from another instance");
#endif	


	// Copy templates
	templates = rhs.templates;

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
	   

void
anslp_ipap_xml_message::createXmlMessage(xmlTextWriterPtr &writer,
										 xmlBufferPtr &buf )
{
	int rc;

#ifdef DEBUG
	log->dlog(ch, "Starting createXmlMessage ");
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
				BAD_CAST IPAP_XML_ROOT.c_str(), 
				NULL,
				BAD_CAST DTD_NAME.c_str() );
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
	log->dlog(ch, "Ending createXmlMessage -Object %s", IPAP_XML_ROOT.c_str());
#endif
}


void 
anslp_ipap_xml_message::writeElement(xmlTextWriterPtr &writer, 
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
	char *buff = (char *) malloc(sizeof(char) * 50);
	
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
		
		sprintf(buff, "%d", field.get_field_type().eno);
		writeAttribute(writer, "ENO", buff);

		sprintf(buff, "%d", field.get_field_type().ftype);
		writeAttribute(writer, "FTYPE", buff);
		
		writeFieldValue(writer, field.writeValue(iter->second));
		
		closeElement(writer);	
	}
	free(buff);
}

void
anslp_ipap_xml_message::printTemplateDataRecords(xml_object_key key,
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
					string elementName = IPAP_XML_RECORD;

					createElement(writer, elementName);
					char *buff = (char *) malloc (sizeof(char)*32);
					sprintf(buff, "%u", g_data.get_template_id());
					writeAttribute(writer, "TEMPLATE_ID", buff);
					free(buff);
					
					writeDataRecord(writer, templ, g_data, DATA_XML_ELEMENT);
					closeElement(writer);
				}
			}
		}
	}
}

void
anslp_ipap_xml_message::printOptionDataRecords(xml_object_key key,
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
					
					string elementName = IPAP_XML_RECORD;
					createElement(writer, elementName);
					char *buff = (char *) malloc (sizeof(char)*32);
					sprintf(buff, "%u", g_data.get_template_id());
					writeAttribute(writer, "TEMPLATE_ID", buff);
					free(buff);
					
					writeDataRecord(writer, templ, g_data, OPTION_XML_ELEMENT);
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
	char *buff = (char *) malloc(sizeof(char) * 50);
	 
	sprintf(buff, "%u", templ->get_template_id());
	createElement(writer, "TEMPLATE", buff);
	
	sprintf(buff, "%u", templ->get_type());
	writeAttribute(writer, "TYPE",	buff);
	
	int numFields = templ->get_numfields();
	
	sprintf(buff, "%u", templ->get_numfields());
	writeAttribute(writer, "NUM_FIELDS", buff);
	
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

#ifdef DEBUG
	log->dlog(ch, "Starting writeRecords");
#endif

	bool elementWrite = false;
	ipap_field dataField;
	// Go over the set of keys sort them and print the xml
	xml_object_key lastKey;
	map<xml_object_key, xml_object_key>::iterator iterKeys;
	for (iterKeys = keys.begin(); iterKeys != keys.end(); ++iterKeys){

#ifdef DEBUG
	log->dlog(ch, "key: %s", ((iterKeys->first).to_string()).c_str());
#endif

		// print all template data records
		printTemplateDataRecords(iterKeys->first,  writer);
			
		// print all option data records
		printOptionDataRecords(iterKeys->first,  writer);
	}
	
}


void 
anslp_ipap_xml_message::writeObjectTypeData(xmlTextWriterPtr &writer, 
										ipap_xml_object_type_t object_type)
{

#ifdef DEBUG
	log->dlog(ch, "Starting writeObjectTypeData");
#endif

	// Build set of distinct keys filtering those corresponding to
	// the object type.
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
		
		// Write Templates associated with the object type
		writeTemplates(writer, object_type, keys);
		
		// Write Records associated with the object type
		writeRecords(writer, object_type, keys);
	}

#ifdef DEBUG
	log->dlog(ch, "Ending writeObjectTypeData");
#endif

}


void 
anslp_ipap_xml_message::writeNotRelatedTemplates(xmlTextWriterPtr &writer)
{
	
#ifdef DEBUG
	log->dlog(ch, "Starting writeNotRelatedTemplates");
#endif

	
	std::list<int> intList = templates.get_template_list();
	std::list<int>::iterator tmplIter;
	for (tmplIter = intList.begin(); tmplIter != intList.end(); ++tmplIter){
		ipap_template *tmpl = templates.get_template(*tmplIter);
		writeTemplate(writer,tmpl);
	}

#ifdef DEBUG
	log->dlog(ch, "Ending writeNotRelatedTemplates");
#endif

}

string
anslp_ipap_xml_message::get_message(const anslp_ipap_message &mes)
{

#ifdef DEBUG
	log->dlog(ch, "Starting get_message");
#endif
	char *buff = (char *) malloc(sizeof(char) * 50);
	split(mes);

	try{
		xmlTextWriterPtr writer;
		xmlBufferPtr buf;
		createXmlMessage(writer, buf);
		createElement(writer, IPAP_XML_ROOT);

		// Write the last template Id.
		sprintf(buff, "%u", (mes.ip_message).get_last_template_id());
		writeAttribute(writer, "LAST_TEMPLATE_ID", buff);
		
		// Write the domain Id
		sprintf(buff, "%d", (mes.ip_message).get_domain());
		writeAttribute(writer, "DOMAIN_ID", buff);
		
		// Write the message version
		sprintf(buff, "%d", (mes.ip_message).get_version());
		writeAttribute(writer, "VERSION", buff);
		
		// Write the exporttime
		sprintf(buff,"%lu", (unsigned long) (mes.ip_message).get_exporttime());
		writeAttribute(writer, "EXPORT_TIME", buff);

		// Write the Seq_no
		sprintf(buff,"%lu", (unsigned long) (mes.ip_message).get_seqno());
		writeAttribute(writer, "SEQ_NO", buff);
		
		// Copy templates no related with data record.
		writeNotRelatedTemplates(writer);

		// Copy data related with different object types
		for (int i=0; i < IPAP_MAX_XML_OBJECT_TYPE; i++) {
			writeObjectTypeData(writer, (ipap_xml_object_type_t) i);
		}
				
		// Close the header.
		closeElement(writer);
				
		xmlFreeTextWriter(writer);
			
		string content((const char *) buf->content);
				
		xmlBufferFree(buf);
				
		return content;
	}
	catch (anslp_ipap_bad_argument &e){
#ifdef DEBUG
		log->elog(ch, "Error: %s", e.what());
#endif	
		throw e;
	}
}


bool
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
					uint16_t idTemplate,  ipap_fields_t *fields)
{
	int index = 0;
	while (cur != NULL) 
	{
        // get TFIELD
        if ((!xmlStrcmp(cur->name, (const xmlChar *)"TFIELD")) && (cur->ns == ns)) {
			uint32_t sEno = strtoul((
						xmlCharToString(xmlGetProp(cur, (const xmlChar *)"ENO"))).c_str(), NULL,0);
						
			uint16_t sFType = strtoul(
						(xmlCharToString(xmlGetProp(cur, (const xmlChar *)"FTYPE"))).c_str(), NULL,0);
						
			uint16_t sCoding = strtoul(
						(xmlCharToString(xmlGetProp(cur, (const xmlChar *)"CODING"))).c_str(),NULL,0);
						
			uint16_t sLength = strtoul(
						(xmlCharToString(xmlGetProp(cur, (const xmlChar *)"LENGTH"))).c_str(),NULL,0);
						
			string sXmlName = xmlCharToString(xmlGetProp(cur, (const xmlChar *)"XML_NAME"));
			fields[index].eno = sEno;
			fields[index].ienum =  sFType;
			fields[index].length = sLength;
			
			index++;
		}
		cur = cur->next; 
	}
}


void 
anslp_ipap_xml_message::ReadRecords(xmlNodePtr cur, 
									uint16_t templId,
									anslp_ipap_message *message)
{

#ifdef DEBUG
	log->dlog(ch, "Starting ReadRecords %d", templId);
#endif	
	
    xmlNodePtr cur2, cur3;
    
    try{
		
		ipap_template *templ = message->ip_message.get_template_object(templId);

		// Creates the new data record.
		ipap_data_record data(templId);
				
		// This code reads all fields within a data record
		cur2 = cur->xmlChildrenNode;
		

		
		while (cur2 != NULL) 
		{

			if ((!xmlStrcmp(cur2->name, (const xmlChar *)"FIELD")) && (cur2->ns == ns)) {
		
				string sName = xmlCharToString(xmlGetProp(cur2, (const xmlChar *)"NAME"));
				string sEno = xmlCharToString(xmlGetProp(cur2, (const xmlChar *)"ENO"));
				string sType = xmlCharToString(xmlGetProp(cur2, (const xmlChar *)"FTYPE"));
					
#ifdef DEBUG
				log->dlog(ch, "Field values Name:%s Eno:%s FType:%s", 
							sName.c_str(), sEno.c_str(), sType.c_str());
#endif					
					
				int eno = atoi( sEno.c_str() );
				int type = atoi( sType.c_str() );
				templ->get_field(eno, type);
				ipap_field field = templ->get_field( eno, type );
					
				string value = xmlCharToString(xmlNodeListGetString(XMLDoc, cur2->xmlChildrenNode, 1));
				if (value.empty()) {
					throw anslp_ipap_bad_argument("Missing value");
				}
				ipap_value_field val = field.parse(value);
				data.insert_field(eno, type, val);
			}
			cur2 = cur2->next; 
		}

		(message->ip_message).include_data(templId, data);

	} catch(Error &e){
#ifdef DEBUG
		log->elog(ch, e.getError().c_str());
#endif		
		throw e;
	}
}


anslp_ipap_message *
anslp_ipap_xml_message::from_message(const string str)
{

#ifdef DEBUG
	log->dlog(ch, "Starting from_message");
#endif	

	anslp_ipap_message *message = NULL;
	uint16_t lastTemplId;
	int domainId, version; 
	uint32_t exportTime, seqNo;
	
	try{

#ifdef DEBUG
		log->dlog(ch, "DTD name: %s root:% \n message:%s", DTD_FILENAME.c_str(), IPAP_XML_ROOT.c_str(), str.c_str() );
#endif	
		
		XMLParserValidate(DTD_FILENAME, str.c_str(), str.length(), IPAP_XML_ROOT);

		xmlNodePtr cur, cur2, cur3;
		cur = xmlDocGetRootElement(XMLDoc);

		// As a first step, we read the message header.
		while (cur != NULL) {
			if ((!xmlStrcmp(cur->name, (const xmlChar *) IPAP_XML_ROOT.c_str())) && (cur->ns == ns)) {
								
				// Read the header fields.
				string slastTemplId = xmlCharToString(xmlGetProp(cur, (const xmlChar *)"LAST_TEMPLATE_ID"));
				str_to_uint16(slastTemplId.c_str(), &lastTemplId);				
				
				string sdomainId = xmlCharToString(xmlGetProp(cur, (const xmlChar *)"DOMAIN_ID"));
				domainId = atoi(sdomainId.c_str());
				
				string sversion = xmlCharToString(xmlGetProp(cur, (const xmlChar *)"VERSION"));
				version = atoi(sversion.c_str());
				
				string sexportTime = xmlCharToString(xmlGetProp(cur, (const xmlChar *)"EXPORT_TIME"));
				exportTime = atoi(sexportTime.c_str());
				
				string sseqNo = xmlCharToString(xmlGetProp(cur, (const xmlChar *)"SEQ_NO"));
				seqNo = atoi(sseqNo.c_str());
				
			}
			cur = cur->next;
		}

		message = new anslp_ipap_message(domainId, version); 		
		(message->ip_message).set_seqno(seqNo);
		(message->ip_message).set_exporttime(exportTime);
		
		
		// Second, it is required to read templates.
		cur = xmlDocGetRootElement(XMLDoc);
		cur = cur->xmlChildrenNode;
		
		while (cur != NULL) {
			if ((!xmlStrcmp(cur->name, (const xmlChar *)"TEMPLATE")) && (cur->ns == ns)) {
				
				// Read the Id of the template
				uint16_t uid;
				string id = xmlCharToString(xmlGetProp(cur, (const xmlChar *)"ID"));
				str_to_uint16(id.c_str(), &uid);
				
				// read the template type.
				string type = xmlCharToString(xmlGetProp(cur, (const xmlChar *)"TYPE"));
				int itype = atoi(type.c_str());

				// read the number of fields in the template.
				string numFields = xmlCharToString(xmlGetProp(cur, (const xmlChar *)"NUM_FIELDS"));
				int inumFields = atoi(numFields.c_str());
							
				if (inumFields <= 0){
					throw anslp_ipap_bad_argument("Invalid template, non positive field number");
				} else{
					ipap_fields_t fields[inumFields]; 
										
					cur2 = cur->xmlChildrenNode;
					ReadTemplateFields(cur2, uid, fields);
					
					cout << "We are going to read a template with type:" << itype << endl;
					(message->ip_message).make_template(fields, inumFields, (ipap_templ_type_t) itype , uid);
					
					
				}
			}
			cur = cur->next;
		}

#ifdef DEBUG
		log->dlog(ch, "Finish reading templates from_message");
#endif	

		// Third, it is required to read data records.
		cur = xmlDocGetRootElement(XMLDoc);
		cur = cur->xmlChildrenNode;
		while (cur != NULL) {
			if ((!xmlStrcmp(cur->name, (const xmlChar *) "DATARECORD" )) && (cur->ns == ns)) {

				uint16_t uid;
				string sId = xmlCharToString(xmlGetProp(cur, (const xmlChar *)"TEMPLATE_ID"));
				str_to_uint16(sId.c_str(), &uid);

				ReadRecords(cur, uid, message);
			}
			cur = cur->next;
		}

	#ifdef DEBUG
		log->dlog(ch, "Ending from_message");
	#endif	
		
		return message;

	} catch(Error &e){
		if (message){
			saveDelete(message);
		}
		log->elog(ch, e.getError().c_str());
		throw e;
	}
}

