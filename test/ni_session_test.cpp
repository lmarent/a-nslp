/*
 * Test the ni_session_test class.
 *
 * $Id: ni_session.cpp 1936 2015-09-04 8:02:00 amarentes $
 * $HeadURL: https://./test/ni_session.cpp $
 */
#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

#include "events.h"
#include "session.h"
#include "dispatcher.h"
#include <vector>
#include "msg/information_code.h"
#include "msg/selection_auctioning_entities.h"
#include "nop_auction_rule_installer.h"
#include <iostream>

#include "utils.h" // custom assertions

using namespace anslp;
using namespace anslp::msg;


class InitiatorTest;


/*
 * We use a subclass for testing and make the test case a friend. This
 * way the test cases have access to protected methods and they don't have
 * to be public in ni_session.
 */
class ni_session_test : public ni_session {
  public:
	ni_session_test(state_t state=STATE_ANSLP_CLOSE) : ni_session(state) { }

	friend class InitiatorTest;
};


class InitiatorTest : public CppUnit::TestCase {

	CPPUNIT_TEST_SUITE( InitiatorTest );

	CPPUNIT_TEST( testClose );
	CPPUNIT_TEST( testPending );
	CPPUNIT_TEST( testPendingInstalling );
	CPPUNIT_TEST( testAuctioning );
	CPPUNIT_TEST( testIntegratedStateMachine );

	CPPUNIT_TEST_SUITE_END();

  public:
	void setUp();
	void tearDown();
	void add_fields(msg::anslp_ipap_message *mess);


	void testClose();
	void testPending();
	void testPendingInstalling();
	void testAuctioning();
	void testIntegratedStateMachine();

  private:
	static const uint32 START_MSN = 77;

	void process(ni_session_test &s, event *evt);

	msg::ntlp_msg *create_anslp_create() const;

	msg::ntlp_msg *create_anslp_response(
		uint8 severity, uint8 response_code, uint16 msg_type,
		uint32 msn=START_MSN) const;

	anslp_response *create_anslp_response_with_objects(
		uint8 severity, uint8 response_code, uint16 msg_type,
		uint32 msn=START_MSN) const;

	msg::ntlp_msg *create_anslp_refresh() const;

	mock_anslp_config *conf;
	nop_auction_rule_installer *auction_installer;
	mock_dispatcher *d;
	hostaddress source;
	hostaddress destination;
	
	msg::anslp_ipap_message *mess1;
	msg::anslp_ipap_message *mess2;
	msg::anslp_ipap_message *mess3;
	
};

CPPUNIT_TEST_SUITE_REGISTRATION( InitiatorTest );


void InitiatorTest::add_fields(msg::anslp_ipap_message * mess) 
{

    /* 
     * Builds the anslp ipap message
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



/*
 * Call the event handler and delete the event afterwards.
 */
void InitiatorTest::process(ni_session_test &s, event *evt) {
	d->clear();	
	s.process(d, evt);
	delete evt;
}

void InitiatorTest::setUp() 
{
		
	conf = new mock_anslp_config();
	auction_installer = new nop_auction_rule_installer(conf);
	d = new mock_dispatcher(NULL, auction_installer, conf);
	
	source = hostaddress("10.0.2.15");
	destination = hostaddress("173.194.37.80");
	
	mess1 = new msg::anslp_ipap_message( IPAP_VERSION, true);
	mess2 = new msg::anslp_ipap_message( IPAP_VERSION, true);
	mess3 = new msg::anslp_ipap_message( IPAP_VERSION, true);
	
	add_fields(mess1);
	add_fields(mess2);
	add_fields(mess3);

}

void InitiatorTest::tearDown() 
{
	
	if (d != NULL){
		delete d;
		d = NULL;
	}
		
	if (auction_installer != NULL){
		delete auction_installer;	
		auction_installer = NULL;
	}
		
	if (conf != NULL){
		delete conf;
		conf = NULL;
	}
	
	delete mess1;
	delete mess2;
	delete mess3;
	
}


msg::ntlp_msg *InitiatorTest::create_anslp_create() const {

	msg::anslp_create *create = new anslp_create();
	create->set_msg_sequence_number(START_MSN);

	msg::ntlp_msg *msg = new msg::ntlp_msg(
		session_id(), create,
		new ntlp::mri_pathcoupled(), 0
	);

	return msg;
}


msg::ntlp_msg *InitiatorTest::create_anslp_response(uint8 severity,
		uint8 response_code, uint16 msg_type, uint32 msn) const {

	anslp_response *resp = new anslp_response();
	resp->set_information_code(severity, response_code, 
							   information_code::obj_none);
	resp->set_msg_sequence_number(msn);

	ntlp::mri *ntlp_mri = new ntlp::mri_pathcoupled(
		hostaddress("192.168.1.4"), 32, 0,
		hostaddress("192.168.1.5"), 32, 0,
		"tcp", 0, 0, 0, true
	);

	return new msg::ntlp_msg(session_id(), resp, ntlp_mri, 0);
}

anslp_response *InitiatorTest::create_anslp_response_with_objects(uint8 severity,
		uint8 response_code, uint16 msg_type, uint32 msn) const {

	anslp_response *resp = new anslp_response();
	resp->set_information_code(severity, response_code, 
							   information_code::obj_none);
	resp->set_msg_sequence_number(msn);

	resp->set_mspec_object(mess1->copy());
	resp->set_mspec_object(mess2->copy());
	resp->set_mspec_object(mess3->copy());

	return resp;
}


msg::ntlp_msg *InitiatorTest::create_anslp_refresh() const {

	msg::anslp_refresh *refresh = new anslp_refresh();
	refresh->set_session_lifetime(10);
	refresh->set_msg_sequence_number(START_MSN);

	msg::ntlp_msg *msg = new msg::ntlp_msg(
		session_id(), refresh,
		new ntlp::mri_pathcoupled(), 0
	);

	return msg;
}


void InitiatorTest::testClose() 
{
	anslp::FastQueue *output=new anslp::FastQueue();
	string sessionIdIn = "12345678901223";
	AnslpEvent * anslpEvt = NULL;
	
	/*
	 * STATE_ANSLP_CLOSE ---[tg_CONF]---> STATE_ANSLP_CLOSE
	 * This part tests when no queue was given, so response could not be delivered.
	 */
	ni_session_test s1(ni_session::STATE_ANSLP_CLOSE);
	vector<msg::anslp_mspec_object *> mspec_objects;
	mspec_objects.push_back(mess1->copy());
	mspec_objects.push_back(mess2->copy());
	mspec_objects.push_back(mess3->copy());
		
	event *e1 = new api_create_event(sessionIdIn, source, destination);

	process(s1, e1);
	ASSERT_STATE(s1, ni_session::STATE_ANSLP_CLOSE);
	ASSERT_CREATE_MESSAGE_SENT(d);
	ASSERT_TIMER_STARTED(d, s1.get_response_timer());
		
	event *e2 = new api_create_event(sessionIdIn, source,destination,(protlib::uint16) 0, 
									 (protlib::uint16) 0, (protlib::uint8) 0,
									  mspec_objects, conf->get_ni_session_lifetime(),
									  selection_auctioning_entities::sme_any, output);
	
	ni_session_test s2(ni_session::STATE_ANSLP_CLOSE);
	process(s2, e2);
	ASSERT_STATE(s2, ni_session::STATE_ANSLP_PENDING);
	ASSERT_CREATE_MESSAGE_SENT(d);
	ASSERT_TIMER_STARTED(d, s2.get_response_timer());
	
	CPPUNIT_ASSERT( output->size() == 1 );
	anslpEvt = output->dequeue();
	CPPUNIT_ASSERT(anslpEvt != NULL && \
					dynamic_cast<anslp::AddAnslpSessionEvent *>(anslpEvt) != NULL );
	saveDelete(anslpEvt);
	

	ni_session_test s3(ni_session::STATE_ANSLP_CLOSE);

	vector<msg::anslp_mspec_object *> mspec_objects2;
	mspec_objects2.push_back(mess1->copy());
	mspec_objects2.push_back(mess2->copy());
	mspec_objects2.push_back(mess3->copy());
	
	anslp::session_id *sessionId = new anslp::session_id(s3.get_id());
		
	event *e3 = new api_bidding_event(sessionId, source,destination,(protlib::uint16) 0, 
									 (protlib::uint16) 0, (protlib::uint8) 0,
									  mspec_objects2, NULL);
	
	process(s3, e3);
	ASSERT_STATE(s3, ni_session::STATE_ANSLP_CLOSE);
	ASSERT_NO_MESSAGE(d);
	ASSERT_NO_TIMER(d);

	
}


void InitiatorTest::testPending() 
{
		
	/*
	 * STATE_ANSLP_PENDING ---[rx_RESPONSE(SUCCESS,CREATE)]---> STATE_ANSLP_PENDING_INSTALLING
	 */
	ni_session_test s1(ni_session::STATE_ANSLP_PENDING);
	s1.set_last_create_message(create_anslp_create());

	ntlp_msg *resp1 = create_anslp_response(information_code::sc_success,
		information_code::suc_successfully_processed,
		information_code::obj_none);

	event *e1 = new msg_event(NULL, resp1);

	process(s1, e1);
	ASSERT_STATE(s1, ni_session::STATE_ANSLP_PENDING_INSTALLING);
	ASSERT_NO_MESSAGE(d);
	ASSERT_TIMER_STARTED(d, s1.get_response_timer());


	/*
	 * STATE_ANSLP_PENDING ---[rx_RESPONSE(ERROR)]---> CLOSE
	 */
	ni_session_test s2(ni_session::STATE_ANSLP_PENDING);
	s2.set_last_create_message(create_anslp_create());

	ntlp_msg *resp2 = create_anslp_response(
		information_code::sc_permanent_failure, 0,
		information_code::obj_ipap_message);

	event *e2 = new msg_event(NULL, resp2);

	process(s2, e2);
	ASSERT_STATE(s2, ni_session::STATE_ANSLP_CLOSE);
	ASSERT_NO_MESSAGE(d);
	ASSERT_NO_TIMER(d);


	/*
	 * STATE_ANSLP_PENDING ---[RESPONSE_TIMEOUT, retry]---> STATE_PENDING
	 */
	ni_session_test s3(ni_session::STATE_ANSLP_PENDING);
	// fake a previously sent Create message
	s3.set_last_create_message(create_anslp_create());
	s3.get_response_timer().set_id(47);

	timer_event *e3 = new timer_event(NULL, 47);

	process(s3, e3);
	ASSERT_STATE(s3, ni_session::STATE_ANSLP_PENDING);
	ASSERT_CREATE_MESSAGE_SENT(d);
	ASSERT_TIMER_STARTED(d, s3.get_response_timer());


	/*
	 * STATE_ANSLP_PENDING ---[RESPONSE_TIMEOUT, no retry]---> CLOSE
	 */
	ni_session_test s4(ni_session::STATE_ANSLP_PENDING);
	// fake a previously sent Configure message
	s4.set_last_create_message(create_anslp_create());	
	s4.set_create_counter(1000);
	s4.get_response_timer().set_id(47);
	timer_event *e4 = new timer_event(NULL, 47);

	process(s4, e4);
	ASSERT_STATE(s4, ni_session::STATE_ANSLP_CLOSE);
	ASSERT_NO_MESSAGE(d);
	ASSERT_NO_TIMER(d);


	/*
	 * STATE_ANSLP_PENDING ---[tg_TEARDOWN]---> STATE_ANSLP_CLOSE
	 */
	ni_session_test s5(ni_session::STATE_ANSLP_PENDING);
	s5.set_last_create_message(create_anslp_create());
	event *e5 = new api_teardown_event(NULL);

	process(s5, e5);
	ASSERT_STATE(s5, ni_session::STATE_ANSLP_CLOSE);
	ASSERT_REFRESH_MESSAGE_SENT(d);
	ASSERT_NO_TIMER(d);


	/*
	 * STATE_ANSLP_PENDING ---[tg_TEARDOWN]---> STATE_ANSLP_CLOSE
	 */
	ni_session_test s6(ni_session::STATE_ANSLP_PENDING);
	s6.set_last_create_message(create_anslp_create());
	event *e6 = new no_next_node_found_event(NULL);

	process(s6, e6);
	ASSERT_STATE(s6, ni_session::STATE_ANSLP_CLOSE);
	ASSERT_NO_MESSAGE(d);
	ASSERT_NO_TIMER(d);


	/*
	 * STATE_ANSLP_PENDING ---[tg_BIDDING]---> STATE_ANSLP_PENDING
	 */
	ni_session_test s7(ni_session::STATE_ANSLP_PENDING);
	s7.set_last_create_message(create_anslp_create());

	vector<msg::anslp_mspec_object *> mspec_objects2;
	mspec_objects2.push_back(mess1->copy());
	mspec_objects2.push_back(mess2->copy());
	mspec_objects2.push_back(mess3->copy());
		
	anslp::session_id *sessionId2 = new anslp::session_id(s7.get_id());
	event *e7 = new api_bidding_event(sessionId2, source,destination,(protlib::uint16) 0, 
									 (protlib::uint16) 0, (protlib::uint8) 0,
									  mspec_objects2, NULL);


	process(s7, e7);
		
	ASSERT_STATE(s7, ni_session::STATE_ANSLP_PENDING);
	ASSERT_NO_MESSAGE(d);
	ASSERT_NO_TIMER(d);

}


void InitiatorTest::testPendingInstalling() 
{
	
	string sessionIdIn = "12345678901223";
		
	/*
	 * STATE_ANSLP_PENDING_INSTALLING ---[tg_install(NBR_OBJECTS = NBR_OBJECTS)]---> STATE_ANSLP_AUCTIONING
	 */
	anslp_response *resp1 = create_anslp_response_with_objects(information_code::sc_success,
		information_code::suc_successfully_processed,
		information_code::obj_none);
			
	ni_session_test s1(ni_session::STATE_ANSLP_PENDING_INSTALLING);
	s1.set_last_create_message(create_anslp_create());
	s1.set_last_auction_install_rule(s1.build_auction_install_rule(resp1));

	vector<msg::anslp_mspec_object *> mspec_objects;
	mspec_objects.push_back(mess1->copy());
	mspec_objects.push_back(mess2->copy());
	mspec_objects.push_back(mess3->copy());
		
	event *e1 = new api_install_event(sessionIdIn, mspec_objects, NULL);

	process(s1, e1);
	ASSERT_STATE(s1, ni_session::STATE_ANSLP_AUCTIONING);
	ASSERT_NO_MESSAGE(d);
	ASSERT_TIMER_STARTED(d, s1.get_refresh_timer());


	/*
	 * STATE_ANSLP_PENDING_INSTALLING ---[tg_install(NBR_OBJECTS != NBR_OBJECTS)]---> CLOSE
	 */
	anslp_response *resp2 = create_anslp_response_with_objects(information_code::sc_success,
		information_code::suc_successfully_processed,
		information_code::obj_none);
    
	ni_session_test s2(ni_session::STATE_ANSLP_PENDING_INSTALLING);
	s2.set_last_create_message(create_anslp_create());
	s2.set_last_auction_install_rule(s2.build_auction_install_rule(resp2));
	
	vector<msg::anslp_mspec_object *> mspec_objects2;
	mspec_objects2.push_back(mess1->copy());
	mspec_objects2.push_back(mess2->copy());

	event *e2 = new api_install_event(sessionIdIn, mspec_objects2, NULL);
    
	process(s2, e2);
	ASSERT_STATE(s2, ni_session::STATE_ANSLP_CLOSE);
	ASSERT_REFRESH_MESSAGE_SENT(d);
	ASSERT_NO_TIMER(d);

	
	/*
	 * STATE_ANSLP_PENDING_INSTALLING ---[RESPONSE_TIMEOUT, retry]---> STATE_ANSLP_PENDING_INSTALLING
	 */
	anslp_response *resp3 = create_anslp_response_with_objects(information_code::sc_success,
		information_code::suc_successfully_processed,
		information_code::obj_none);

	ni_session_test s3(ni_session::STATE_ANSLP_PENDING_INSTALLING);
	// fake a previously sent Create message
	s3.set_last_create_message(create_anslp_create());
	s3.set_last_auction_install_rule(s3.build_auction_install_rule(resp3));
	s3.get_response_timer().set_id(47);

	timer_event *e3 = new timer_event(NULL, 47);

	process(s3, e3);
	ASSERT_STATE(s3, ni_session::STATE_ANSLP_PENDING_INSTALLING);
	ASSERT_NO_MESSAGE(d);
	ASSERT_TIMER_STARTED(d, s3.get_response_timer());

	/*
	 * STATE_ANSLP_PENDING_INSTALLING ---[RESPONSE_TIMEOUT, no retry]---> CLOSE
	 */
	anslp_response *resp4 = create_anslp_response_with_objects(information_code::sc_success,
		information_code::suc_successfully_processed,
		information_code::obj_none);

	ni_session_test s4(ni_session::STATE_ANSLP_PENDING_INSTALLING);
	// fake a previously sent Configure message
	s4.set_last_create_message(create_anslp_create());	
	s4.set_last_auction_install_rule(s4.build_auction_install_rule(resp4));
	s4.set_create_counter(1000);
	s4.get_response_timer().set_id(47);
	timer_event *e4 = new timer_event(NULL, 47);

	process(s4, e4);
	ASSERT_STATE(s4, ni_session::STATE_ANSLP_CLOSE);
	ASSERT_REFRESH_MESSAGE_SENT(d);
	ASSERT_NO_TIMER(d);


	/*
	 * STATE_ANSLP_PENDING_INSTALLING ---[tg_TEARDOWN]---> STATE_ANSLP_CLOSE
	 */
	anslp_response *resp5 = create_anslp_response_with_objects(information_code::sc_success,
		information_code::suc_successfully_processed,
		information_code::obj_none);

	ni_session_test s5(ni_session::STATE_ANSLP_PENDING_INSTALLING);
	s5.set_last_create_message(create_anslp_create());
	s5.set_last_auction_install_rule(s5.build_auction_install_rule(resp5));
	event *e5 = new api_teardown_event(NULL);

	process(s5, e5);
	ASSERT_STATE(s5, ni_session::STATE_ANSLP_CLOSE);
	ASSERT_REFRESH_MESSAGE_SENT(d);
	ASSERT_NO_TIMER(d);


	/*
	 * STATE_ANSLP_PENDING_INSTALLING ---[no next node found]---> STATE_ANSLP_PENDING_INSTALLING
	 */
	anslp_response *resp6 = create_anslp_response_with_objects(information_code::sc_success,
		information_code::suc_successfully_processed,
		information_code::obj_none);

	ni_session_test s6(ni_session::STATE_ANSLP_PENDING_INSTALLING);
	s6.set_last_create_message(create_anslp_create());
	s6.set_last_auction_install_rule(s6.build_auction_install_rule(resp6));
	event *e6 = new no_next_node_found_event(NULL);

	process(s6, e6);
	ASSERT_STATE(s6, ni_session::STATE_ANSLP_PENDING_INSTALLING);
	ASSERT_NO_MESSAGE(d);
	ASSERT_NO_TIMER(d);


	/*
	 * STATE_ANSLP_PENDING_INSTALLING ---[tg_BIDDING]---> STATE_ANSLP_PENDING_INSTALLING
	 */
	anslp_response *resp7 = create_anslp_response_with_objects(information_code::sc_success,
		information_code::suc_successfully_processed,
		information_code::obj_none);

	ni_session_test s7(ni_session::STATE_ANSLP_PENDING_INSTALLING);
	s7.set_last_create_message(create_anslp_create());
	s7.set_last_auction_install_rule(s7.build_auction_install_rule(resp7));

	vector<msg::anslp_mspec_object *> mspec_objects3;
	mspec_objects3.push_back(mess1->copy());
	mspec_objects3.push_back(mess2->copy());

	anslp::session_id *sessionId2 = new anslp::session_id(s7.get_id());
	event *e7 = new api_bidding_event(sessionId2, source,destination,(protlib::uint16) 0, 
									 (protlib::uint16) 0, (protlib::uint8) 0,
									  mspec_objects3, NULL);


	process(s7, e7);
		
	ASSERT_STATE(s7, ni_session::STATE_ANSLP_PENDING_INSTALLING);
	ASSERT_NO_MESSAGE(d);
	ASSERT_NO_TIMER(d);
	
}


void InitiatorTest::testAuctioning() 
{
		
	/*
	 * STATE_ANSLP_AUCTIONING ---[rx_RESPONSE(SUCCESS,REFRESH)]---> STATE_ANSLP_AUCTIONING
	 */
	ni_session_test s1(ni_session::STATE_ANSLP_AUCTIONING);
	s1.set_last_refresh_message(create_anslp_refresh());

	ntlp_msg *resp1 = create_anslp_response(information_code::sc_success,
		information_code::suc_successfully_processed,
		information_code::obj_none);

	event *e1 = new msg_event(NULL, resp1);

	process(s1, e1);
	ASSERT_STATE(s1, ni_session::STATE_ANSLP_AUCTIONING);
	ASSERT_NO_MESSAGE(d);
	ASSERT_TIMER_STARTED(d, s1.get_refresh_timer());
	
	/*
	 * STATE_ANSLP_AUCTIONING ---[rx_RESPONSE(ERROR,REFRESH)]---> CLOSE
	 */
	ni_session_test s2(ni_session::STATE_ANSLP_AUCTIONING);
	s2.set_last_refresh_message(create_anslp_refresh());

	ntlp_msg *resp2 = create_anslp_response(
		information_code::sc_permanent_failure, 0,
		information_code::obj_ipap_message);

	event *e2 = new msg_event(NULL, resp2);

	process(s2, e2);
	ASSERT_STATE(s2, ni_session::STATE_ANSLP_CLOSE);
	ASSERT_NO_MESSAGE(d);
	ASSERT_NO_TIMER(d);


	/*
	 * STATE_ANSLP_AUCTIONING ---[RESPONSE_TIMEOUT, retry]---> STATE_ANSLP_AUCTIONING
	 */
	ni_session_test s3(ni_session::STATE_ANSLP_AUCTIONING);
	s3.set_last_refresh_message(create_anslp_refresh());
	s3.get_response_timer().set_id(47);

	timer_event *e3 = new timer_event(NULL, 47);

	process(s3, e3);
	ASSERT_STATE(s3, ni_session::STATE_ANSLP_AUCTIONING);
	ASSERT_REFRESH_MESSAGE_SENT(d);
	ASSERT_TIMER_STARTED(d, s3.get_response_timer());


	/*
	 * STATE_ANSLP_AUCTIONING ---[RESPONSE_TIMEOUT, no retry]---> STATE_ANSLP_AUCTIONING
	 */
	ni_session_test s4(ni_session::STATE_ANSLP_AUCTIONING);
	s4.set_last_refresh_message(create_anslp_refresh());	
	s4.set_refresh_counter(1000);
	s4.get_response_timer().set_id(47);
	timer_event *e4 = new timer_event(NULL, 47);

	process(s4, e4);
	ASSERT_STATE(s4, ni_session::STATE_ANSLP_CLOSE);
	ASSERT_NO_MESSAGE(d);
	ASSERT_NO_TIMER(d);


	/*
	 * STATE_ANSLP_AUCTIONING ---[tg_TEARDOWN]---> STATE_ANSLP_CLOSE
	 */
	ni_session_test s5(ni_session::STATE_ANSLP_AUCTIONING);
	s5.set_last_refresh_message(create_anslp_refresh());
	event *e5 = new api_teardown_event(NULL);

	process(s5, e5);
	ASSERT_STATE(s5, ni_session::STATE_ANSLP_CLOSE);
	ASSERT_REFRESH_MESSAGE_SENT(d);
	ASSERT_NO_TIMER(d);


	/*
	 * STATE_ANSLP_AUCTIONING ---[REFRESH_TIMEOUT]---> STATE_ANSLP_AUCTIONING
	 */
	ni_session_test s6(ni_session::STATE_ANSLP_AUCTIONING);
	s6.set_last_refresh_message(create_anslp_refresh());
	s6.get_refresh_timer().set_id(0xABCD);
	event *e6 = new timer_event(NULL, 0xABCD);

	process(s6, e6);
	ASSERT_STATE(s6, ni_session::STATE_ANSLP_AUCTIONING);
	ASSERT_REFRESH_MESSAGE_SENT(d);
	ASSERT_TIMER_STARTED(d, s6.get_response_timer());


	/*
	 * STATE_ANSLP_AUCTIONING ---[tg_BIDDING]---> STATE_ANSLP_AUCTIONING
	 */
	ni_session_test s7(ni_session::STATE_ANSLP_AUCTIONING);
	s7.set_last_refresh_message(create_anslp_refresh());

	vector<msg::anslp_mspec_object *> mspec_objects2;
	mspec_objects2.push_back(mess1->copy());
	mspec_objects2.push_back(mess2->copy());
	mspec_objects2.push_back(mess3->copy());

	anslp::session_id *sessionId3 = new anslp::session_id(s7.get_id());
	event *e7 = new api_bidding_event(sessionId3, source,destination,(protlib::uint16) 0, 
									 (protlib::uint16) 0, (protlib::uint8) 0,
									  mspec_objects2, NULL);


	process(s7, e7);
	ASSERT_STATE(s7, ni_session::STATE_ANSLP_AUCTIONING);
	ASSERT_BIDDING_MESSAGE_SENT(d);
	ASSERT_NO_TIMER(d);	
}

void InitiatorTest::testIntegratedStateMachine()
{
	
	string sessionIdIn = "1232342343";
		
	vector<msg::anslp_mspec_object *> mspec_objects;
	mspec_objects.push_back(mess1->copy());
	mspec_objects.push_back(mess2->copy());
	mspec_objects.push_back(mess3->copy());
	
	anslp::FastQueue *installQueue = new anslp::FastQueue();
	
	event *e1 = new api_create_event(sessionIdIn, source,destination,
   					    (protlib::uint16) 0, //Srcport 
					    (protlib::uint16) 0, //Dstport
					    (protlib::uint8) 0, // Protocol
					    mspec_objects, // Mspec Objects
					    conf->get_ni_session_lifetime(), 
					    selection_auctioning_entities::sme_any,
					    installQueue);

	ni_session_test s1(ni_session::STATE_ANSLP_CLOSE);
	process(s1, e1);
	ASSERT_STATE(s1, ni_session::STATE_ANSLP_PENDING);
	ASSERT_CREATE_MESSAGE_SENT(d);
	ASSERT_TIMER_STARTED(d, s1.get_response_timer());

	
	/*
	 * STATE_ANSLP_PENDING ---[rx_RESPONSE(SUCCESS,CREATE)]---> STATE_ANSLP_PENDING_INSTALLING
	 */
	ntlp_msg *resp1 = create_anslp_response(information_code::sc_success,
		information_code::suc_successfully_processed,
		information_code::obj_none, s1.get_msg_sequence_number());

	event *e2 = new msg_event(NULL, resp1);

	process(s1, e2);
	ASSERT_STATE(s1, ni_session::STATE_ANSLP_PENDING_INSTALLING);
	ASSERT_NO_MESSAGE(d);
	ASSERT_TIMER_STARTED(d, s1.get_response_timer());


	/*
	 * STATE_ANSLP_PENDING_INSTALLING ---[tg_install]---> STATE_ANSLP_AUCTIONING
	 */
	anslp_response *resp2 = create_anslp_response_with_objects(information_code::sc_success,
		information_code::suc_successfully_processed,
		information_code::obj_none);

	s1.set_last_auction_install_rule(s1.build_auction_install_rule(resp2));

	vector<msg::anslp_mspec_object *> mspec_objects2;
	mspec_objects2.push_back(mess1->copy());
	mspec_objects2.push_back(mess2->copy());
	mspec_objects2.push_back(mess3->copy());
		
	event *e3 = new api_install_event(sessionIdIn, mspec_objects2, NULL);

	process(s1, e3);
	ASSERT_STATE(s1, ni_session::STATE_ANSLP_AUCTIONING);
	ASSERT_NO_MESSAGE(d);
	ASSERT_TIMER_STARTED(d, s1.get_refresh_timer());
	
	/*
	 * STATE_ANSLP_AUCTIONING ---[tg_TEARDOWN]---> STATE_ANSLP_CLOSE
	 */
	event *e4 = new api_teardown_event(NULL);

	process(s1, e4);
	ASSERT_STATE(s1, ni_session::STATE_ANSLP_CLOSE);
	ASSERT_REFRESH_MESSAGE_SENT(d);
	ASSERT_NO_TIMER(d);


}
