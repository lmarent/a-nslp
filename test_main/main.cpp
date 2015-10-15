
#include "anslp_ipap_xml_message_test.h"
#include "logfile.h"
#include "configfile.h"
#include "gist_conf.h"
#include "anslp_config.h"


using namespace protlib;
using namespace protlib::log;
using namespace anslp;

anslp_config conf;

using namespace ntlp;


// needed for linking all tests
logfile commonlog("test_main.log", false, true); // no colours, quiet start
logfile &protlib::log::DefaultLog(commonlog);


int main(int argc, char *argv[]) 
{

	// Turn off logging
	if ( getenv("TEST_LOG") == NULL ) {
		commonlog.set_filter(ERROR_LOG, LOG_EMERG + 1);
		commonlog.set_filter(WARNING_LOG, LOG_EMERG + 1);
		commonlog.set_filter(EVENT_LOG, LOG_EMERG + 1);
		commonlog.set_filter(INFO_LOG, LOG_EMERG + 1);
		commonlog.set_filter(DEBUG_LOG, LOG_EMERG + 1);
	}

   commonlog.set_filter(ERROR_LOG, LOG_ALL);
   commonlog.set_filter(WARNING_LOG, LOG_ALL);
   commonlog.set_filter(EVENT_LOG, LOG_ALL);
   commonlog.set_filter(INFO_LOG, LOG_ALL);
   commonlog.set_filter(DEBUG_LOG, LOG_ALL);

	anslp::anslp_ipap_xml_message_test test;
	
	test.setUp();
	test.testExport();
	test.tearDown();

}
