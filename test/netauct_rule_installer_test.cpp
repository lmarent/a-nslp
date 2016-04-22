/*
 * Test the netauct_rule_installer class.
 *
 * $Id: netauct_rule_installer_test.cpp 2133 2015-12-23 17:42:00 amarentes $
 * $HeadURL: ../test/netauct_rule_installer_test.cpp $
 */
#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

#include "mri.h"	// from NTLP

#include "events.h"
#include "session.h"
#include "dispatcher.h"
#include "msg/selection_auctioning_entities.h"

#include "utils.h" // custom assertions

using namespace anslp;
using namespace anslp::msg;


class NetAuctRuleInstallerTest;

/*
 * We use a subclass for testing and make the test case a friend. This
 * way the test cases have access to protected methods and they don't have
 * to be public in nr_session.
 */
class netauct_rule_installer_test : public netauct_rule_installer {
  public:
	netauct_rule_installer_test(anslp_config *conf, anslp::FastQueue *installQueue)
		: netauct_rule_installer(conf, installQueue, true) { }

	friend class NetAuctRuleInstallerTest;
};

class NetAuctRuleInstallerTest : public CppUnit::TestCase {
	
	CPPUNIT_TEST_SUITE( NetAuctRuleInstallerTest );

	// CPPUNIT_TEST( testCheck );
	// CPPUNIT_TEST( testCheckException1 );
	// CPPUNIT_TEST( testCheckException2 );
	
	// CPPUNIT_TEST( testCreate );
	
	// CPPUNIT_TEST( testPutResponse );
	// CPPUNIT_TEST( testRemove );
	// CPPUNIT_TEST( testAuctionInteraction );

	CPPUNIT_TEST_SUITE_END();

  public:
	void setUp();
	void tearDown();
	void add_fields(msg::anslp_ipap_message *mess);
	
	void testCheck();
	void testCheckException1();
	void testCheckException2();
	
	void testCreate();
	
	void testPutResponse();
	void testRemove();
	void testAuctionInteraction();

  private:
	mock_anslp_config *conf;
	mock_dispatcher *d;
	netauct_rule_installer_test *installer;
	anslp::FastQueue *queueRet; 
	
	msg::anslp_ipap_message *mess1;
	msg::anslp_ipap_message *mess2;
	msg::anslp_ipap_message *mess3;
	
};

CPPUNIT_TEST_SUITE_REGISTRATION( NetAuctRuleInstallerTest );

void NetAuctRuleInstallerTest::add_fields(msg::anslp_ipap_message * mess) 
{

    /* 
     * Builds the ipfix message
     */
	uint16_t templatedataid = 0;
	uint64_t starttime = 100;
	uint64_t endtime = 200;
	unsigned char   *buf1  = (unsigned char *) "1";
	unsigned char   *buf1a  = (unsigned char *) "2";
	unsigned char   *buf1b  = (unsigned char *) "3";
	unsigned char   *buf2  = (unsigned char *) "bas";

	(mess->ip_message).delete_all_templates();

	int nfields = 4;
	templatedataid = (mess->ip_message).new_data_template( nfields, IPAP_SETID_AUCTION_TEMPLATE );
	(mess->ip_message).add_field(templatedataid, 0, IPAP_FT_IDAUCTION);
	(mess->ip_message).add_field(templatedataid, 0, IPAP_FT_STARTSECONDS);
	(mess->ip_message).add_field(templatedataid, 0, IPAP_FT_ENDSECONDS);	
	(mess->ip_message).add_field(templatedataid, 0, IPAP_FT_AUCTIONINGALGORITHMNAME);

	ipap_field field1 = (mess->ip_message).get_field_definition( 0, IPAP_FT_STARTSECONDS );
	ipap_value_field fvalue1 = field1.get_ipap_value_field( starttime );

	ipap_field field2 = (mess->ip_message).get_field_definition( 0, IPAP_FT_ENDSECONDS );
	ipap_value_field fvalue2 = field2.get_ipap_value_field( endtime );

	ipap_field field3 = (mess->ip_message).get_field_definition( 0, IPAP_FT_IDAUCTION );
	ipap_value_field fvalue3 = field3.get_ipap_value_field( buf1, 1 );
	ipap_value_field fvalue3a = field3.get_ipap_value_field( buf1a, 1 );
	ipap_value_field fvalue3b = field3.get_ipap_value_field( buf1b, 1 );
		
	ipap_field field4 = (mess->ip_message).get_field_definition( 0, IPAP_FT_AUCTIONINGALGORITHMNAME );
	ipap_value_field fvalue4 = field3.get_ipap_value_field( buf2, 3 );
	
	ipap_data_record data(templatedataid);
	data.insert_field(0, IPAP_FT_STARTSECONDS, fvalue1);
	data.insert_field(0, IPAP_FT_ENDSECONDS, fvalue2);
	data.insert_field(0, IPAP_FT_IDAUCTION, fvalue3);
	data.insert_field(0, IPAP_FT_AUCTIONINGALGORITHMNAME, fvalue4);
	(mess->ip_message).include_data(templatedataid, data);

	ipap_data_record data2(templatedataid);
	data2.insert_field(0, IPAP_FT_STARTSECONDS, fvalue1);
	data2.insert_field(0, IPAP_FT_ENDSECONDS, fvalue2);
	data2.insert_field(0, IPAP_FT_IDAUCTION, fvalue3);
	data2.insert_field(0, IPAP_FT_AUCTIONINGALGORITHMNAME, fvalue4);
	(mess->ip_message).include_data(templatedataid, data2);
	(mess->ip_message).output();	
	
}

void NetAuctRuleInstallerTest::setUp() 
{
	
	queueRet = new anslp::FastQueue();
	conf = new mock_anslp_config();
	installer = new netauct_rule_installer_test(conf, queueRet);
	d = new mock_dispatcher(NULL, installer, conf);
	
	mess1 = new msg::anslp_ipap_message(IPAP_VERSION, true);
	mess2 = new msg::anslp_ipap_message(IPAP_VERSION, true);
	mess3 = new msg::anslp_ipap_message(IPAP_VERSION, true);
	
	add_fields(mess1);
	add_fields(mess2);
	add_fields(mess3);

}

void NetAuctRuleInstallerTest::tearDown() 
{
	delete queueRet;
	delete installer;	
	delete conf;
	delete mess1;
	delete mess2;
	delete mess3;
}

void NetAuctRuleInstallerTest::testCheck()
{
	string sessionId = "123456";
	
	AnslpEvent *ret_evt = NULL;
	
	try{
		
		objectList_t mspec_objects;
		anslp::mspec_rule_key key;
		mspec_objects[key] = mess1->copy();
		
		installer->check( sessionId, &mspec_objects);
	
		ret_evt = queueRet->dequeue_timedwait(1000);

		CPPUNIT_ASSERT( ret_evt != NULL);
		CPPUNIT_ASSERT( is_check_event(ret_evt) == true);
				
		anslp::FastQueue reponse;
		
		ResponseCheckSessionEvent *resCheck = new ResponseCheckSessionEvent();
		resCheck->setObject(key, mess3->copy());
		
		reponse.enqueue(resCheck);
		
		installer->handle_response_check(&reponse);
				
	} catch (auction_rule_installer_error &e){
		throw e;
	}
	
}

void NetAuctRuleInstallerTest::testCheckException1()
{

	string sessionId = "123456";
		
	objectList_t mspec_objects;
	anslp::mspec_rule_key key;
	mspec_objects[key] = mess1->copy();

	installer->check( sessionId, &mspec_objects );
	
	anslp::FastQueue reponse;
		
	ResponseCheckSessionEvent *resCheck = new ResponseCheckSessionEvent();
		
	reponse.enqueue(resCheck);
		
	CPPUNIT_ASSERT_THROW( installer->handle_response_check(&reponse), auction_rule_installer_error);

}

void NetAuctRuleInstallerTest::testCheckException2()
{

	string sessionId = "123456";
		
	objectList_t mspec_objects;
	anslp::mspec_rule_key key;
	mspec_objects[key] = mess1->copy();

	installer->check( sessionId, &mspec_objects );
	
	anslp::FastQueue reponse;
		
	ResponseAddSessionEvent *resCheck = new ResponseAddSessionEvent();
		
	reponse.enqueue(resCheck);
		
	CPPUNIT_ASSERT_THROW( installer->handle_response_check(&reponse), auction_rule_installer_error);

}



void NetAuctRuleInstallerTest::testCreate()
{
	string sessionId = "123456";
	
	auction_rule *rule = NULL;
	
	
	try{
		
		rule = new auction_rule();
		
		mspec_rule_key key1;
		rule->set_request_object(key1, mess1->copy());
		
		mspec_rule_key key2;
		rule->set_request_object(key2, mess2->copy());
		
		installer->handle_create_session( sessionId, rule );
			
		saveDelete(rule);
	
	} catch (auction_rule_installer_error &e){
		if (rule != NULL)
			saveDelete(rule);
		
		throw e;
	}
		 
}

void NetAuctRuleInstallerTest::testPutResponse()
{
	string sessionId = "123456";
	/*
	try{
	
		installer->put_response( sessionId, mess1 );
	
		AnslpEvent *ret_evt = queueRet->dequeue_timedwait(10000);
	
	} catch (auction_rule_installer_error &e){
		
	}
	
	CPPUNIT_ASSERT( ret_evt != NULL);
	CPPUNIT_ASSERT( is_addsession_event(ret_evt) == true);
	*/ 
}


void NetAuctRuleInstallerTest::testRemove()
{
	string sessionId = "123456";
	
	auction_rule *rule = NULL;	
	
	try{
		
		rule = new auction_rule();
		
		mspec_rule_key key1;
		rule->set_request_object(key1, mess1->copy());
		
		mspec_rule_key key2;
		rule->set_request_object(key2, mess2->copy());

		AnslpEvent *ret_evt = NULL;
		
		installer->handle_remove_session( sessionId, rule );
		
		// Verify that it creates a new remove session event, 
		// which will be given to the auction server or auction client.
		CPPUNIT_ASSERT( ret_evt != NULL);
		CPPUNIT_ASSERT( is_removesession_event(ret_evt) == true);
		
		anslp::FastQueue reponse;
		
		mspec_rule_key key;
	
		// test an exception when it is given a not waited message
		ResponseCheckSessionEvent *resCreate = new ResponseCheckSessionEvent();
		
		reponse.enqueue(resCreate);
		

	} catch (auction_rule_installer_error &e){
		if (rule != NULL)
			saveDelete(rule);
		
		throw e;
	}
	
}

void NetAuctRuleInstallerTest::testAuctionInteraction()
{
	string sessionId = "123456";
	/*
	try{
	
		installer->auction_interaction( sessionId, mess1 );
	
		AnslpEvent *ret_evt = queueRet->dequeue_timedwait(10000);
	
	} catch (auction_rule_installer_error &e){
		
	}
	
	CPPUNIT_ASSERT( ret_evt != NULL);
	CPPUNIT_ASSERT( is_auction_interaction_event(ret_evt) == true);
	*/ 
}
