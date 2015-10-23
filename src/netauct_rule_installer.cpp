/// ----------------------------------------*- mode: C++; -*--
/// @file netauct_rule_installer.cpp
/// The netauct_rule_installer class.
/// ----------------------------------------------------------
/// $Id: netauct_rule_installer.cpp 2558 2015-10-05 07:42:00 amarentes $
/// $HeadURL: https://./src/netauct_rule_installer.cpp $
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
#include <assert.h>
#include <cstdlib>
#include <sys/types.h>
#include <unistd.h>
#include "address.h"
#include "logfile.h"

// curl includes
#include <curl/curl.h>
#include <list>

// xml includes
#include <libxslt/transform.h>
#include <libxslt/xsltutils.h>

#include "stdincpp.h"
#include "anslp_ipap_message.h"
#include "anslp_ipap_xml_message.h"
#include "netauct_rule_installer.h"


using namespace anslp;
using namespace protlib::log;


#define LogError(msg) Log(ERROR_LOG, LOG_NORMAL, \
	"netauct_rule_installer", msg)
#define LogWarn(msg) Log(WARNING_LOG, LOG_NORMAL, \
	"netauct_rule_installer", msg)
#define LogInfo(msg) Log(INFO_LOG, LOG_NORMAL, \
	"netauct_rule_installer", msg)
#define LogDebug(msg) Log(DEBUG_LOG, LOG_NORMAL, \
	"netauct_rule_installer", msg)

#define LogUnimp(msg) Log(ERROR_LOG, LOG_UNIMP, "netauct_rule_installer", \
	msg << " at " << __FILE__ << ":" << __LINE__)



netauct_rule_installer::netauct_rule_installer(
		anslp_config *conf) throw () 
		: auction_rule_installer(conf) 
{

	// nothing to do
}


netauct_rule_installer::~netauct_rule_installer() throw () 
{
	// nothing to do
}


void 
netauct_rule_installer::setup() throw (auction_rule_installer_error) 
{

	auction_rule_installer::setup();
	
	LogDebug("NOP: setup()");

}

bool netauct_rule_installer::responseOk(string response)
{
	 LogDebug("start responseOk - " << response);
	 bool val_return = false;
	 
	 std::size_t found = response.find("OK");
	 if (found!=std::string::npos){
		val_return =  true;
	 }
	 
	 LogDebug("ending responseOk" << val_return);
	 return val_return;
	
}

int netauct_rule_installer::getNumberAuctions(string response)
{
	
	LogDebug("start getNumberAuctions - " << response);
	
	int val_return = 0;
	string tofind("<NbrAuctions>");
	string tofind2("</NbrAuctions>");
	
	std::size_t found = response.find(tofind);
	if (found!=std::string::npos){
		int pos = found + tofind.length() + 1;
		int pos2 = response.find(tofind2);
		if (pos2!=std::string::npos){
			LogDebug("pos:" << pos << " pos2:" << pos2);
			string snbr = response.substr(pos, pos2-pos);
			val_return = atoi(snbr.c_str());
		}
	}
	LogDebug("ending getNumberAuctions" << val_return);
	return val_return;
}

string 
netauct_rule_installer::getMessage(string response)
{
	LogDebug("start getMessage - " << response);
	
	string val_return;
	string tofind("<?xml version=");
	string tofind2("</IPAP_MESSAGE>");
	
	std::size_t pos = response.find(tofind);
	if (pos!=std::string::npos){
		std::size_t pos2 = response.find(tofind2);
		if (pos2!=std::string::npos){
			LogDebug("pos:" << pos << " pos2:" << pos2);
			val_return = response.substr(pos, pos2 + tofind2.length());
		}
	}
	
	LogDebug("ending getMessage" << val_return);
	return val_return;
			
}		

void 
netauct_rule_installer::check(const string sessionId, const msg::anslp_mspec_object *object)
		throw (auction_rule_installer_error) 
{
	LogDebug("start check()");
	
	
	string response;
	string action = "/check_session";
	
	msg::anslp_ipap_xml_message mess;
	string postfield = mess.get_message( *(get_ipap_message(object)) );
	
	LogDebug("check - message:" << postfield);
	postfield = "SessionID=" +  sessionId + "&Message=" + postfield;
	response = execute_command(RULE_INSTALLER_SERVER, action, postfield);
	LogDebug("Reponse" + response);
		
	
	if (!responseOk(response)){
		throw auction_rule_installer_error(response,
			msg::information_code::sc_signaling_session_failures,
			msg::information_code::sigfail_wrong_conf_message);
	} else  {
		
		if (getNumberAuctions(response) <= 0){
			throw auction_rule_installer_error("No auction satisfying filter criteria",
				msg::information_code::sc_signaling_session_failures,
				msg::information_code::sigfail_auction_not_applicable);
		}
	}
	
	LogDebug("end check()");		
}


auction_rule * 
netauct_rule_installer::create(const string sessionId, const auction_rule *rule) 
{

	LogDebug("Creating auction session " << *rule);
	
	string response;
	string action = "/add_session";
	
	auction_rule *auc_return = new auction_rule(*rule);
	objectListConstIter_t i;
	objectList_t * requestObjectList = auc_return->get_request_objects();
	
	LogDebug("Nbr objects to install: " << requestObjectList->size());
	
	// Loop through the objects and install them.
	for ( i = requestObjectList->begin(); i != requestObjectList->end(); i++){
		
		LogDebug("Installing object");
		
		msg::anslp_ipap_xml_message mess;
		string postfield = mess.get_message( *(get_ipap_message(i->second)) );
		postfield = "SessionID=" +  sessionId + "&Message=" + postfield;
		response = execute_command(RULE_INSTALLER_SERVER, action, postfield);
				
		if (!responseOk(response)){
			throw auction_rule_installer_error(response,
				msg::information_code::sc_signaling_session_failures,
				msg::information_code::sigfail_wrong_conf_message);

		} else {
			string responseMsg = getMessage(response);
			msg::anslp_ipap_message *ipap_response = mess.from_message(responseMsg);
			(ipap_response->ip_message).output();
			auc_return->set_response_object(ipap_response);
		}
	}	
	
	LogDebug("Finishing, put nbr objects:" << auc_return->get_response_objects());
	
	return auc_return;
}


auction_rule * 
netauct_rule_installer::put_response(const string sessionId, const auction_rule *rule) 
{

	LogDebug("Putting response " << *rule);
	
	string response;
	string action = "/res_add_session";
	
	auction_rule *auc_return = new auction_rule(*rule);
	objectListConstIter_t i;
	objectList_t * requestObjectList = auc_return->get_request_objects();
	
	LogDebug("Nbr objects to install: " << requestObjectList->size());
	
	// Loop through the objects and install them.
	for ( i = requestObjectList->begin(); i != requestObjectList->end(); i++){
		
		LogDebug("Installing object");
		
		msg::anslp_ipap_xml_message mess;
		string postfield = mess.get_message( *(get_ipap_message(i->second)) );
		postfield = "SessionID=" +  sessionId + "&Message=" + postfield;
		response = execute_command(RULE_INSTALLER_CLIENT, action, postfield);
				
		if (!responseOk(response)){
			throw auction_rule_installer_error(response,
				msg::information_code::sc_signaling_session_failures,
				msg::information_code::sigfail_wrong_conf_message);

		} else {
			string responseMsg = getMessage(response);
			msg::anslp_ipap_message *ipap_response = mess.from_message(responseMsg);
			(ipap_response->ip_message).output();
			auc_return->set_response_object(ipap_response);
		}
	}	
	
	LogDebug("Finishing, put nbr objects:" << auc_return->get_response_objects());
	
	return auc_return;
}


auction_rule * 
netauct_rule_installer::remove(const string sessionId, const auction_rule *rule) 
{

	LogDebug("Starting removing auction rule " << *rule);
	string action = "/del_session";

	auction_rule *rule_return = new auction_rule(*rule);
	
	// Iterate over the set of sessions associated with the auction
	
		//create the command with the session 
	//
	
	LogDebug("Ending removing auction rule " << *rule);
	return rule_return;
	
}

bool netauct_rule_installer::remove_all() 
{

	LogDebug("NOP: removing all auction rules ");
	return true;
}


auction_rule * 
netauct_rule_installer::auction_interaction(const auction_rule *mt_object)
{

}

const msg::anslp_ipap_message *
netauct_rule_installer::get_ipap_message(const msg::anslp_mspec_object *object)
{
	const msg::anslp_ipap_message *mess = dynamic_cast<const msg::anslp_ipap_message *>(object);
	assert( mess != NULL );
	return mess;
}

size_t writedata( void *ptr, size_t size, size_t nmemb, void  *stream)
{
    string *s = (string *) stream;

    s->append((char *)ptr, size*nmemb);

    return size*nmemb;
}

std::string getErr(char *e)
{
    string err = e;

    int p = err.find_last_of(":");

    if (p > 0) {
        return err.substr(0,p) + ": " + 
          string(strerror(atoi(err.substr(p+1, err.length()-p).c_str())));
    }

    return "";
}


string 
netauct_rule_installer::execute_command(rule_installer_destination_type_t destination, 
											std::string action, std::string post_fields)
{

    
    char cebuf[CURL_ERROR_SIZE], *ctype;
	char *post_body = NULL;
	string userpwd, stylesheet, server;
	int port;
	
	string response;
	string input, input2;
	
	
	unsigned long rcode;
	bool val_return = true;
	
#ifdef USE_SSL
    int use_ssl = 0;
#endif
	CURL *curl;
	CURLcode res;
	xsltStylesheetPtr cur = NULL;
	xmlDocPtr doc, out;

	LogDebug("Starting execute command - action: " << action);


    stylesheet = get_xsl();
    switch (destination)
    {
		case RULE_INSTALLER_SERVER:
			server = get_server();
			port = get_port();
			userpwd = get_user() + ":" + get_password();
			
          break;
        case RULE_INSTALLER_CLIENT:
			server = get_bid_server();
			port = get_bid_port();
			userpwd = get_bid_user() + ":" + get_bid_password();
          break;
	}


    
	// initialize libcurl
	curl = curl_easy_init();
	if (curl == NULL) {
		throw auction_rule_installer_error("Error during policy installation",
			msg::information_code::sc_signaling_session_failures,
			msg::information_code::sigfail_auction_connection_broken);
	}

	memset(cebuf, 0, sizeof(cebuf));
    xmlSubstituteEntitiesDefault(1);
    xmlLoadExtDtdDefaultValue = 1;
    cur = xsltParseStylesheetFile((const xmlChar *)stylesheet.c_str());

#ifdef USE_SSL
    use_ssl = 1;
    curl_easy_setopt(curl, CURLOPT_SSLCERTTYPE, "PEM");
    curl_easy_setopt(curl, CURLOPT_SSLCERT, CERT_FILE.c_str());
    curl_easy_setopt(curl, CURLOPT_SSLKEYPASSWD, SSL_PASSWD);
    curl_easy_setopt(curl, CURLOPT_SSLKEYTYPE, "PEM");
    curl_easy_setopt(curl, CURLOPT_SSLKEY, CERT_FILE.c_str());
    /* do not validate server's cert because its self signed */
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
    /* do not verify host */
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0);
#endif

    // debug
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 0);
    curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, cebuf);

    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *) &response);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writedata);
   
    curl_easy_setopt(curl, CURLOPT_USERPWD, userpwd.c_str());
  
    ostringstream url;
          
    // build URL
#ifdef USE_SSL
    if (use_ssl) {
       url << "https://";
    } else {
#endif
       url << "http://";
#ifdef USE_SSL
    }
#endif
    url << server << ":" << port;
    url << action;
                       
    char *_url = strdup(url.str().c_str());
    curl_easy_setopt(curl, CURLOPT_URL, _url);
    post_body =  curl_escape(post_fields.c_str(), post_fields.length());	
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_body);

    res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
       response = "";
       free(_url);
#ifdef HAVE_CURL_FREE
       curl_free(post_body);
#else
       free(post_body);
#endif

	   curl_easy_cleanup(curl);
	   xsltFreeStylesheet(cur);
	   xsltCleanupGlobals();
	   xmlCleanupParser();
	   throw auction_rule_installer_error(getErr(cebuf),
			msg::information_code::sc_signaling_session_failures,
			msg::information_code::sigfail_auction_connection_broken);       
    }

    res = curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &ctype);
    if (res != CURLE_OK) {
       response = "";
       free(_url);
#ifdef HAVE_CURL_FREE
       curl_free(post_body);
#else
       free(post_body);
#endif
      
	   curl_easy_cleanup(curl);
	   xsltFreeStylesheet(cur);
	   xsltCleanupGlobals();
	   xmlCleanupParser();
	   throw auction_rule_installer_error(getErr(cebuf),
			msg::information_code::sc_signaling_session_failures,
			msg::information_code::sigfail_auction_connection_broken);
    }

    res = curl_easy_getinfo(curl, CURLINFO_HTTP_CODE, &rcode);
    if (res != CURLE_OK) {

       response = "";
       free(_url);
#ifdef HAVE_CURL_FREE
       curl_free(post_body);
#else
       free(post_body);
#endif
      
	   curl_easy_cleanup(curl);
	   xsltFreeStylesheet(cur);
	   xsltCleanupGlobals();
	   xmlCleanupParser();
	   throw auction_rule_installer_error(getErr(cebuf),
			msg::information_code::sc_signaling_session_failures,
			msg::information_code::sigfail_auction_connection_broken);
    }

    if (!strcmp(ctype, "text/xml")) {
       // translate
      
	   xmlChar *output = 0; 
	   int len = 0; 
      
       doc = xmlParseMemory(response.c_str(), response.length());
       out = xsltApplyStylesheet(cur, doc, NULL);
       xsltSaveResultToString(&output, &len, out, cur);         
       string strReturn (reinterpret_cast<char*>(output));
       response = strReturn;
       xmlFreeDoc(out);
       xmlFreeDoc(doc);
    } 

    free(_url);
#ifdef HAVE_CURL_FREE
    curl_free(post_body);
#else
    free(post_body);
#endif
     
    curl_easy_cleanup(curl);
	xsltFreeStylesheet(cur);
	xsltCleanupGlobals();
	xmlCleanupParser();

	return response;
}




// EOF
