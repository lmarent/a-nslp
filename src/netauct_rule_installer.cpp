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
#include <pthread.h>
#include <sys/syscall.h>

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
#include "aqueue.h"

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



netauct_rule_installer::netauct_rule_installer(anslp_config *conf, FastQueue *installQueue, bool test) throw () 
		: auction_rule_installer(conf), installQueue(installQueue) , test(test)
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
		LogDebug("found:" << found);
		std::size_t pos = found + tofind.length();
		std::size_t pos2 = response.find(tofind2);
		if (pos2!=std::string::npos){
			LogDebug("data:" << response.substr(0,pos) << "pos:" << pos << " pos2:" << pos2);
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
		} else{
			// the message only contains the header.
			std::size_t pos2 = response.size();
			val_return = response.substr(pos, pos2);
		}	
	}
	
	LogDebug("ending getMessage" << val_return);
	return val_return;
			
}		

void netauct_rule_installer::handle_response_check(anslp::FastQueue *waitqueue) 
								throw (auction_rule_installer_error) 
{
	LogDebug("start handle_response_check()");
	
	AnslpEvent *ret_evt = waitqueue->dequeue_timedwait(10000);

	if (!is_response_checksession_event(ret_evt)){
		throw auction_rule_installer_error("Unexpected anslp event returned, expecting response_checksession",
			msg::information_code::sc_signaling_session_failures,
			msg::information_code::sigfail_wrong_conf_message);			
	}

	ResponseCheckSessionEvent *resAdd = 
		dynamic_cast< ResponseCheckSessionEvent *>(ret_evt);
						
	// Loop through responses to see which of the them work. 			
	if ( resAdd->getObjects()->size() == 0 ){
		throw auction_rule_installer_error("No auction satisfying filter criteria",
			msg::information_code::sc_signaling_session_failures,
			msg::information_code::sigfail_auction_not_applicable);
	}
	
	delete resAdd;
	
	LogDebug("ending handle_response_check()");
}

void 
netauct_rule_installer::check(const string sessionId, 
								objectList_t *missing_objects)
		throw (auction_rule_installer_error) 
{
	LogDebug("start check()");
	
	if (get_install_auction_rules()){
		
		LogDebug("Installing check rule");
		
		anslp::FastQueue retQueue;
		
		CheckEvent *evt = new CheckEvent(&retQueue);
		evt->setSession(sessionId);
		
		objectListConstIter_t it_objects;
		for ( it_objects = missing_objects->begin(); it_objects != missing_objects->end(); it_objects++)
		{
			evt->setObject(mspec_rule_key(it_objects->first), 
									it_objects->second->copy());
		}
				
		bool queued = getQueue()->enqueue(evt);

		if ( !queued ){
						
			throw auction_rule_installer_error("Process could not enqueue the anslp event",
					msg::information_code::sc_signaling_session_failures,
					msg::information_code::sigfail_wrong_conf_message);
		}

		LogDebug("end check()");		
	
	} else {
		LogDebug("NOP: a checker check node");
	}	
}


void
netauct_rule_installer::handle_create_session(const string sessionId, const auction_rule *rule)
{

	LogInfo("starting handle_create_session" );

	int nbrObjects = 0;
				
	auction_rule *auc_return = new auction_rule(*rule);
	objectListConstIter_t i;
	objectList_t * requestObjectList = auc_return->get_request_objects();
		
	LogDebug("Nbr objects to install: " << requestObjectList->size() << "in queue:" << getQueue()->get_name());

	AddSessionEvent *evt = new AddSessionEvent(NULL);
	evt->setSession(sessionId);
	
	for ( i = requestObjectList->begin(); i != requestObjectList->end(); i++){
		nbrObjects++;
		evt->setObject(i->first, i->second->copy());
	}
	
	LogInfo("Queuename:" <<  getQueue()->get_name() << "- pid: " <<  getpid() 
				 << " - getthread_self:" << pthread_self() 
				 << " tid:" << syscall(SYS_gettid) );
		
	bool queued = getQueue()->enqueue(evt);
	if ( !queued ){
						
		throw auction_rule_installer_error("Process could not enqueue the anslp event",
			msg::information_code::sc_signaling_session_failures,
			msg::information_code::sigfail_wrong_conf_message);
	}
		
	LogInfo("Finishing, put nbr objects:" << nbrObjects);
	
}

void
netauct_rule_installer::create(const string sessionId, const auction_rule *rule) 
{

	assert(rule != NULL);
	
	LogDebug("Creating auction session " << *rule);
		
	if (get_install_auction_rules()) {
	
		handle_create_session(sessionId, rule);
	
	} else {

		LogDebug("NOP: installing policy rule " << *rule);
		
	}
	
}


void 
netauct_rule_installer::put_response(const string sessionId, const auction_rule *rule) 
{

	LogDebug("start put response ");

	if ( is_auctioneer() )
	{
		
		return handle_create_session(sessionId, rule);
			
	} else {
		throw auction_rule_installer_error("The Ni-Session agent is not configured as auctioneer",
				msg::information_code::sc_signaling_session_failures,
				msg::information_code::sigfail_wrong_conf_message); 
	}
}

void
netauct_rule_installer::handle_remove_session(const string sessionId, const auction_rule *rule)
{

	LogDebug("starting handle_remove_session" );

	anslp::FastQueue retQueue; 
	
	LogDebug("Nbr objects to remove: " << rule->get_request_objects()->size() << "in queue:" << getQueue()->get_name());
	RemoveSessionEvent *evt = new RemoveSessionEvent( &retQueue);
	evt->setSession(sessionId);
	
	objectListConstIter_t i;			
	for ( i = rule->get_request_objects()->begin(); i != rule->get_request_objects()->end(); i++){
		evt->setObject(mspec_rule_key(i->first), i->second->copy());
	}
		
	bool queued = getQueue()->enqueue(evt);
	if ( !queued ){
						
		throw auction_rule_installer_error("Process could not enqueue the anslp event",
			msg::information_code::sc_signaling_session_failures,
			msg::information_code::sigfail_wrong_conf_message);
	}
	
}


void 
netauct_rule_installer::remove(const string sessionId, const auction_rule *rule) 
{


	assert(rule != NULL);
	
	LogDebug("Remove auction session " << *rule);
	
	auction_rule *rule_return;
	
	if (get_install_auction_rules() || is_auctioneer() )
	{
	
		handle_remove_session(sessionId, rule);
	
	} else {

		LogDebug("NOP: removing policy rule " << *rule);
		
	}
	
}

bool netauct_rule_installer::remove_all() 
{
	if (get_install_auction_rules()){
		LogDebug("Removing all auction rules ");
		return true;
	} else {
	
	}
}


auction_rule * 
netauct_rule_installer::auction_interaction(const bool server, const string sessionId, const auction_rule *rule)
{

	LogDebug("Creating auction interaction " << *rule);
				
	auction_rule *auc_return = new auction_rule(*rule);
	objectListConstIter_t i;
	objectList_t * requestObjectList = auc_return->get_request_objects();

	LogDebug("Nbr objects to post: " << requestObjectList->size());

	AuctionInteractionEvent *evt = new AuctionInteractionEvent( );
	evt->setSession(sessionId);
	
	for ( i = requestObjectList->begin(); i != requestObjectList->end(); i++){
		evt->setObject(i->first, i->second->copy());
	}
		
	bool queued = getQueue()->enqueue(evt);

	if ( queued ){
						
		// it just believe that auction interactions were posted. 
		
		for ( i = requestObjectList->begin(); i != requestObjectList->end(); i++){
			auc_return->set_response_object(i->second->copy());
		}
						
	} else { // Error Queuing the event.
			
		throw auction_rule_installer_error("Process could not enqueue the anslp event",
				msg::information_code::sc_signaling_session_failures,
				msg::information_code::sigfail_wrong_conf_message);
	}
				
	LogDebug("Finishing, nbr objects posted:" << auc_return->get_response_objects());
	
	return auc_return;

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

	LogDebug("Server " << server << "Port:" << port << "userpswd:" << userpwd );


    
	// initialize libcurl
	curl = curl_easy_init();
	if (curl == NULL) {
		throw auction_rule_installer_error("Error during policy installation",
			msg::information_code::sc_signaling_session_failures,
			msg::information_code::sigfail_auction_connection_broken);
	}
	
	LogDebug("after easily init" );
	
	memset(cebuf, 0, sizeof(cebuf));
    xmlSubstituteEntitiesDefault(1);
    xmlLoadExtDtdDefaultValue = 1;
    cur = xsltParseStylesheetFile((const xmlChar *)stylesheet.c_str());

	if (cur == NULL){
		throw auction_rule_installer_error("Error opening the xls",
			msg::information_code::sc_signaling_session_failures,
			msg::information_code::sigfail_auction_connection_broken);
	}

	LogDebug("Here -1 " << res);	
	
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

	LogDebug("Here -2 " << res);	

    // debug
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 0);
    curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, cebuf);

    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *) &response);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writedata);
	
	LogDebug("Here -3 " << res);	
	
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
    
    LogDebug("Here before doing perform " << res);	    	
    res = curl_easy_perform(curl);
   	LogDebug("Here # response " << res);	

    
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
	
	LogDebug("Here 0 ");	
	
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
	
	LogDebug("Here 1 ");	
	
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

	LogDebug("Here 2 ");	
    
    if (!strcmp(ctype, "text/xml")) {
       // translate
      
	   xmlChar *output = 0; 
	   int len = 0; 
      
      
       doc = xmlParseMemory(response.c_str(), response.length());
       out = xsltApplyStylesheet(cur, doc, NULL);
       
       if (out == NULL){
			throw auction_rule_installer_error("RESULT output could not be transformed",
			msg::information_code::sc_signaling_session_failures,
			msg::information_code::sigfail_auction_connection_broken);
	   }
	   
       xsltSaveResultToString(&output, &len, out, cur);         
       string strReturn (reinterpret_cast<char*>(output));
       response = strReturn;
       xmlFreeDoc(out);
       xmlFreeDoc(doc);
    } 
	
	LogDebug("Here 3 ");	
	
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
	
	LogDebug("Response: " << response.c_str() );
	
	return response;
}




// EOF
