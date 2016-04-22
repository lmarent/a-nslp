/*
 * Test the nr_session_test class.
 *
 * $Id: nr_session.cpp 2133 2015-09-04 08:22:00 amarentes $
 * $HeadURL: ../test/nr_session.cpp $
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


class ResponderTest;


/*
 * We use a subclass for testing and make the test case a friend. This
 * way the test cases have access to protected methods and they don't have
 * to be public in nr_session.
 */
class nr_session_test : public nr_session {
  public:
	nr_session_test(state_t state=STATE_ANSLP_CLOSE, anslp_config *conf=NULL, uint32 msn=0)
		: nr_session(state, conf, msn) { }

	friend class ResponderTest;
};


class ResponderTest : public CppUnit::TestCase {

	CPPUNIT_TEST_SUITE( ResponderTest );

	CPPUNIT_TEST( testClose );
	CPPUNIT_TEST( testPending );
	CPPUNIT_TEST( testPendingInstalling );
	CPPUNIT_TEST( testAuctioning );
	CPPUNIT_TEST( testPendingTeardown );
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
	void testPendingTeardown();
	void testIntegratedStateMachine();

  private:
	static const uint32 START_MSN = 77;
	static const uint32 SESSION_LIFETIME = 30;

	void process(nr_session_test &s, event *evt);

	msg::ntlp_msg *create_anslp_create(uint32 msn=START_MSN,
		uint32 session_lifetime=SESSION_LIFETIME) const;
	
	msg::ntlp_msg *create_anslp_refresh(uint32 msn, uint32 lt) const;

	msg::ntlp_msg *create_anslp_bidding(uint32 msn=START_MSN) const;

	mock_anslp_config *conf;
	nop_auction_rule_installer *auction_installer;
	mock_dispatcher *d;
	hostaddress destination;
	
	msg::anslp_ipap_message *mess1;
	msg::anslp_ipap_message *mess2;
	msg::anslp_ipap_message *mess3;
	
};

CPPUNIT_TEST_SUITE_REGISTRATION( ResponderTest );


void ResponderTest::add_fields(msg::anslp_ipap_message * mess) 
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


/*
 * Call the event handler and delete the event afterwards.
 */
void ResponderTest::process(nr_session_test &s, event *evt) {
	d->clear();
	s.process(d, evt);
	delete evt;
}

void ResponderTest::setUp() 
{
	conf = new mock_anslp_config();
	auction_installer = new nop_auction_rule_installer(conf);
	d = new mock_dispatcher(NULL, auction_installer, conf);
	
	mess1 = new msg::anslp_ipap_message(IPAP_VERSION, true);
	mess2 = new msg::anslp_ipap_message(IPAP_VERSION, true);
	mess3 = new msg::anslp_ipap_message(IPAP_VERSION, true);
	
	add_fields(mess1);
	add_fields(mess2);
	add_fields(mess3);

}

void ResponderTest::tearDown() 
{
	delete d;
	delete auction_installer;	
	delete conf;
	delete mess1;
	delete mess2;
	delete mess3;
}


msg::ntlp_msg *
ResponderTest::create_anslp_create(uint32 msn, uint32 lt) const {

	msg::anslp_create *create = new anslp_create();
	create->set_msg_sequence_number(msn);
	create->set_session_lifetime(lt);
	create->set_selection_auctioning_entities(selection_auctioning_entities::sme_any);
	create->set_mspec_object(mess1->copy());
	create->set_mspec_object(mess2->copy());
	create->set_mspec_object(mess3->copy());
	
	ntlp::mri *ntlp_mri = new ntlp::mri_pathcoupled(
		hostaddress("192.168.0.4"), 32, 0,
		hostaddress("192.168.0.5"), 32, 0,
		"tcp", 0, 0, 0, false
	);

	msg::ntlp_msg *msg_return = new msg::ntlp_msg(session_id(), create, ntlp_mri, 0);
	return msg_return;
}


msg::ntlp_msg *
ResponderTest::create_anslp_refresh(uint32 msn, uint32 lt) const {

	msg::anslp_refresh *refresh = new anslp_refresh();
	refresh->set_session_lifetime(lt);
	refresh->set_msg_sequence_number(msn);

	msg::ntlp_msg *msg = new msg::ntlp_msg(
		session_id(), refresh,
		new ntlp::mri_pathcoupled(), 0
	);

	return msg;
}

msg::ntlp_msg *
ResponderTest::create_anslp_bidding(uint32 msn) const 
{

	msg::anslp_bidding *bidding = new anslp_bidding();
	bidding->set_msg_sequence_number(msn);
	bidding->set_mspec_object(mess1->copy());
	bidding->set_mspec_object(mess2->copy());
	bidding->set_mspec_object(mess3->copy());
	
	ntlp::mri *ntlp_mri = new ntlp::mri_pathcoupled(
		hostaddress("192.168.0.4"), 32, 0,
		hostaddress("192.168.0.5"), 32, 0,
		"tcp", 0, 0, 0, true
	);

	return new msg::ntlp_msg(session_id(), bidding, ntlp_mri, 0);
}

void 
ResponderTest::testClose() 
{
		
	/*
	 * CLOSE ---[rx_CREATE && CHECK_AA && CREATE(Lifetime>0) ]---> PENDING
	 */
	nr_session_test s1(nr_session::STATE_ANSLP_CLOSE, conf);

	msg::ntlp_msg *msg = create_anslp_create();
	event *e1 = new msg_event(NULL, msg, true);
	 
	process(s1, e1);
	ASSERT_STATE(s1, nr_session::STATE_ANSLP_PENDING);
	ASSERT_TIMER_STARTED(d, s1.get_state_timer());
		
	/*
	 * CLOSE ---[rx_CREATE && CREATE(Lifetime > MAX) ]---> PENDING
	 */
	nr_session_test s2(nr_session::STATE_ANSLP_CLOSE, conf);
	event *e2 = new msg_event(NULL,
		create_anslp_create(START_MSN+1, 100000), true);
	
	process(s2, e2);
	ASSERT_STATE(s2, nr_session::STATE_ANSLP_PENDING);
	ASSERT_TIMER_STARTED(d, s2.get_state_timer());

	/*
	 * CLOSE ---[rx_BIDDING ]---> CLOSE
	 */
	nr_session_test s3(nr_session::STATE_ANSLP_CLOSE, conf);
	event *e3 = new msg_event(NULL, create_anslp_bidding(START_MSN+1), true);
	
	process(s3, e3);
	ASSERT_STATE(s3, nr_session::STATE_ANSLP_CLOSE);
	ASSERT_NO_MESSAGE(d);
	ASSERT_NO_TIMER(d);
		
}


void 
ResponderTest::testPending() {
	
	
	/*
	 * STATE_ANSLP_PENDING ---[tg_check]---> STATE_ANSLP_PENDING_INSTALL
	 */
	nr_session_test s1(nr_session::STATE_ANSLP_CLOSE, conf);

	msg::ntlp_msg *msg = create_anslp_create();
	event *e1 = new msg_event(NULL, msg, true);
	 
	process(s1, e1);
	ASSERT_STATE(s1, nr_session::STATE_ANSLP_PENDING);
	ASSERT_TIMER_STARTED(d, s1.get_state_timer());
	
	api_check_event *e2 = new api_check_event(new session_id(s1.get_id()), NULL);

	objectListIter_t iter;
	for (iter = (s1.rule)->get_request_objects()->begin(); 
				iter != (s1.rule)->get_request_objects()->end(); ++iter){
		e2->setObject( mspec_rule_key(iter->first), (iter->second)->copy());  	
	}
		
	process(s1, e2);
	ASSERT_STATE(s1, nr_session::STATE_ANSLP_PENDING_INSTALLING);
	ASSERT_TIMER_STARTED(d, s1.get_state_timer());

	/*
	 * STATE_ANSLP_PENDING ---[STATE_TIMEOUT && counter > max_retry]---> STATE_ANSLP_CLOSE
	 */
	nr_session_test s5(nr_session::STATE_ANSLP_PENDING, conf);

	s5.get_state_timer().set_id(47);
	s5.set_max_retries(0); 
	s5.set_last_create_message(create_anslp_create());
	timer_event *e3 = new timer_event(NULL, 47);

	process(s5, e3);
	ASSERT_STATE(s5, nr_session::STATE_ANSLP_CLOSE);
	ASSERT_RESPONSE_MESSAGE_SENT(d, information_code::sc_permanent_failure);
	ASSERT_NO_TIMER(d);

	/*
	 * STATE_ANSLP_PENDING ---[STATE_TIMEOUT && counter <= max_retry]---> STATE_ANSLP_PENDING
	 */
	nr_session_test s6(nr_session::STATE_ANSLP_PENDING, conf);
	
	s6.get_state_timer().set_id(47);
	s6.set_last_create_message(create_anslp_create());
	s6.set_max_retries(3);	
	timer_event *e4 = new timer_event(NULL, 47);

	// We must to wait for a check successful response. 
	process(s6, e4);
	ASSERT_STATE(s6, nr_session::STATE_ANSLP_PENDING);
	ASSERT_TIMER_STARTED(d, s6.get_state_timer());
		
	CPPUNIT_ASSERT(s6.get_create_counter()== 1);


	/*
	 * STATE_ANSLP_PENDING ---[rx_BIDDING ]---> STATE_ANSLP_PENDING
	 */
	nr_session_test s3(nr_session::STATE_ANSLP_PENDING, conf);
	event *e7 = new msg_event(NULL, create_anslp_bidding(START_MSN+1), true);
	
	process(s3, e7);
	ASSERT_STATE(s3, nr_session::STATE_ANSLP_PENDING);
	ASSERT_NO_MESSAGE(d);
	ASSERT_NO_TIMER(d);
	
}

void 
ResponderTest::testPendingInstalling() {
	
	
	/*
	 * STATE_ANSLP_PENDING_INSTALLING ---[tg_install(== Nbr objects)]---> AUCTIONING
	 */
	nr_session_test s1(nr_session::STATE_ANSLP_PENDING_INSTALLING, conf);
	
	s1.set_last_create_message(create_anslp_create());
	s1.rule->set_request_object(mess1->copy());
	s1.rule->set_request_object(mess2->copy());
	s1.rule->set_request_object(mess3->copy());
	
	api_install_event *e2 = new api_install_event(new session_id(s1.get_id()), NULL);

	// objects installed.
	mspec_rule_key key1, key2, key3;
	e2->setObject( key1, mess1->copy());
	e2->setObject( key2, mess2->copy());
	e2->setObject( key3, mess3->copy());
	
	process(s1, e2);
	ASSERT_STATE(s1, nr_session::STATE_ANSLP_AUCTIONING);
	ASSERT_RESPONSE_MESSAGE_SENT(d, information_code::sc_success);
	ASSERT_TIMER_STARTED(d, s1.get_state_timer());
	
	
	/*
	 * STATE_ANSLP_PENDING_INSTALLING ---[tg_install(!= Nbr objects)]---> CLOSE
	 */
	nr_session_test s2(nr_session::STATE_ANSLP_PENDING_INSTALLING, conf);
	s2.set_last_create_message(create_anslp_create());
	s2.rule->set_request_object(mess1->copy());
	s2.rule->set_request_object(mess2->copy());
	s2.rule->set_request_object(mess3->copy());

	api_install_event *e4 = new api_install_event(new session_id(s2.get_id()), NULL);

	// objects installed.
	mspec_rule_key key4, key5;
	e4->setObject( key4, mess1->copy());
	e4->setObject( key5, mess2->copy());

	process(s2, e4);
	ASSERT_STATE(s2, nr_session::STATE_ANSLP_CLOSE);
	ASSERT_RESPONSE_MESSAGE_SENT(d, information_code::sc_permanent_failure);
	ASSERT_NO_TIMER(d);

	/*
	 * STATE_ANSLP_PENDING_INSTALLING ---[STATE_TIMEOUT && counter > max_retry]---> STATE_ANSLP_CLOSE
	 */
	nr_session_test s5(nr_session::STATE_ANSLP_PENDING_INSTALLING, conf);

	s5.get_state_timer().set_id(47);
	s5.set_max_retries(0); 
	s5.set_last_create_message(create_anslp_create());
	timer_event *e5 = new timer_event(NULL, 47);

	process(s5, e5);
	ASSERT_STATE(s5, nr_session::STATE_ANSLP_CLOSE);
	ASSERT_RESPONSE_MESSAGE_SENT(d, information_code::sc_permanent_failure);
	ASSERT_NO_TIMER(d);

	/*
	 * STATE_ANSLP_PENDING_INSTALLING ---[STATE_TIMEOUT && counter <= max_retry]---> STATE_ANSLP_PENDING
	 */
	nr_session_test s6(nr_session::STATE_ANSLP_PENDING_INSTALLING, conf);
	
	s6.get_state_timer().set_id(47);
	s6.set_last_create_message(create_anslp_create());
	s6.set_max_retries(3);	
	timer_event *e6 = new timer_event(NULL, 47);

	// We must to wait for a check successful response. 
	process(s6, e6);
	ASSERT_STATE(s6, nr_session::STATE_ANSLP_PENDING_INSTALLING);
	ASSERT_TIMER_STARTED(d, s6.get_state_timer());
		
	CPPUNIT_ASSERT(s6.get_create_counter()== 1);


	/*
	 * STATE_ANSLP_PENDING_INSTALLING ---[rx_BIDDING ]---> STATE_ANSLP_PENDING_INSTALLING
	 */
	nr_session_test s3(nr_session::STATE_ANSLP_PENDING_INSTALLING, conf);
	event *e7 = new msg_event(NULL, create_anslp_bidding(START_MSN+1), true);
	
	process(s3, e7);
	ASSERT_STATE(s3, nr_session::STATE_ANSLP_PENDING_INSTALLING);
	ASSERT_NO_MESSAGE(d);
	ASSERT_NO_TIMER(d);
	
}


void 
ResponderTest::testAuctioning() {
	
	
	/*
	 * STATE_ANSLP_AUCTIONING ---[rx_REFRESH && CHECK_AA && REFRESH(Lifetime>0) ]---> STATE_ANSLP_AUCTIONING
	 */
	nr_session_test s1(nr_session::STATE_ANSLP_AUCTIONING, conf, START_MSN);
	event *e1 = new msg_event(NULL, create_anslp_refresh(START_MSN+1, 10), true);

	process(s1, e1);
	ASSERT_STATE(s1, nr_session::STATE_ANSLP_AUCTIONING);
	CPPUNIT_ASSERT( s1.get_msg_sequence_number() == START_MSN+1 );
	ASSERT_RESPONSE_MESSAGE_SENT(d, information_code::sc_success);
	ASSERT_TIMER_STARTED(d, s1.get_state_timer());

	/*
	 * STATE_ANSLP_AUCTIONING ---[rx_REFRESH && REFRESH(Lifetime > MAX) ]---> STATE_ANSLP_AUCTIONING
	 */
	nr_session_test s2(nr_session::STATE_ANSLP_AUCTIONING, conf, START_MSN);
	event *e2 = new msg_event(NULL,
		create_anslp_refresh(START_MSN+1, 10), true);

	process(s2, e2);
	ASSERT_STATE(s2, nr_session::STATE_ANSLP_AUCTIONING);
	CPPUNIT_ASSERT( s1.get_msg_sequence_number() == START_MSN+1 );
	ASSERT_RESPONSE_MESSAGE_SENT(d, information_code::sc_success);
	ASSERT_TIMER_STARTED(d, s2.get_state_timer());

	/*
	 * STATE_ANSLP_AUCTIONING ---[rx_REFRESH && REFRESH(Lifetime == 0), NBR_OBJECT = 0 ]---> STATE_ANSLP_PENDING_TEARDOWN
	 */
	nr_session_test s3(nr_session::STATE_ANSLP_AUCTIONING, conf, START_MSN);
	
	event *e3 = new msg_event(NULL,
			create_anslp_refresh(START_MSN+1, 0), true);

	process(s3, e3);
	ASSERT_STATE(s3, nr_session::STATE_ANSLP_CLOSE);
	ASSERT_RESPONSE_MESSAGE_SENT(d, information_code::sc_success);
	ASSERT_NO_TIMER(d);


	/*
	 * STATE_ANSLP_AUCTIONING ---[rx_REFRESH && REFRESH(Lifetime == 0), NBR_OBJECT > 0 ]---> STATE_ANSLP_PENDING_TEARDOWN
	 */
	nr_session_test s31(nr_session::STATE_ANSLP_AUCTIONING, conf, START_MSN);
	
	anslp::mspec_rule_key key10;
	(s31.rule)->set_response_object(key10, mess1->copy());

	event *e31 = new msg_event(NULL,
			create_anslp_refresh(START_MSN+1, 0), true);

	process(s31, e31);
	
	ASSERT_STATE(s31, nr_session::STATE_ANSLP_PENDING_TEARDOWN);
	ASSERT_RESPONSE_MESSAGE_SENT(d, information_code::sc_success);
	ASSERT_TIMER_STARTED(d, s31.get_state_timer());


	/*
	 * STATE_AUCTIONING_PART ---[rx_REFRESH && MSN too low ]---> STATE_AUCTIONING_PART
	 */
	nr_session_test s4(nr_session::STATE_ANSLP_AUCTIONING, conf, START_MSN);
	event *e4 = new msg_event(NULL, create_anslp_refresh(10, 20), true);
	
	process(s4, e4);
	ASSERT_STATE(s4, nr_session::STATE_ANSLP_AUCTIONING);
	ASSERT_NO_MESSAGE(d);
	ASSERT_NO_TIMER(d);
	
	/*
	 * STATE_AUCTIONING_PART ---[rx_BIDDING ]---> STATE_AUCTIONING_PART
	 */
	nr_session_test s5(nr_session::STATE_ANSLP_AUCTIONING, conf, START_MSN);
	event *e5 = new msg_event(NULL, create_anslp_bidding(START_MSN + 1), true);

	process(s5, e5);
	ASSERT_STATE(s5, nr_session::STATE_ANSLP_AUCTIONING);
	ASSERT_NO_MESSAGE(d);
	ASSERT_NO_TIMER(d);
	
}


void 
ResponderTest::testPendingTeardown() 
{
	
	
	/*
	 * STATE_ANSLP_PENDING_TEARDOWN ---[ timer, count < max ]---> STATE_ANSLP_PENDING_TEARDOWN
	 */
	nr_session_test s1(nr_session::STATE_ANSLP_PENDING_TEARDOWN, conf, START_MSN);
	s1.set_max_retries(2); 

	s1.get_state_timer().set_id(47);
	timer_event *e1 = new timer_event(NULL, 47);

	process(s1, e1);
	ASSERT_STATE(s1, nr_session::STATE_ANSLP_PENDING_TEARDOWN);
	ASSERT_NO_MESSAGE(d);
	ASSERT_TIMER_STARTED(d, s1.get_state_timer());

	/*
	 * STATE_ANSLP_PENDING_TEARDOWN ---[ timer, count > max ]---> STATE_ANSLP_CLOSE
	 */
	nr_session_test s2(nr_session::STATE_ANSLP_PENDING_TEARDOWN, conf, START_MSN);
	s2.set_max_retries(0); 

	s2.get_state_timer().set_id(47);
	timer_event *e2 = new timer_event(NULL, 47);

	process(s2, e2);
	ASSERT_STATE(s2, nr_session::STATE_ANSLP_CLOSE);
	ASSERT_NO_MESSAGE(d);
	ASSERT_NO_TIMER(d);


	/*
	 * STATE_ANSLP_PENDING_TEARDOWN ---[ api_check ]---> STATE_ANSLP_PENDING_TEARDOWN
	 */
	nr_session_test s3(nr_session::STATE_ANSLP_PENDING_TEARDOWN, conf, START_MSN);

	api_check_event *e3 = new api_check_event(new session_id(s3.get_id()), NULL);

	process(s3, e3);
	ASSERT_STATE(s3, nr_session::STATE_ANSLP_PENDING_TEARDOWN);
	ASSERT_NO_MESSAGE(d);
	ASSERT_NO_TIMER(d);
	
	/*
	 * STATE_ANSLP_PENDING_TEARDOWN ---[ api_install ]---> STATE_ANSLP_PENDING_TEARDOWN
	 */
	nr_session_test s4(nr_session::STATE_ANSLP_PENDING_TEARDOWN, conf, START_MSN);

	api_install_event *e4 = new api_install_event(new session_id(s4.get_id()), NULL);

	process(s4, e4);
	ASSERT_STATE(s4, nr_session::STATE_ANSLP_PENDING_TEARDOWN);
	ASSERT_NO_MESSAGE(d);
	ASSERT_NO_TIMER(d);

	/*
	 * STATE_ANSLP_PENDING_TEARDOWN ---[ anslp_bidding ]---> STATE_ANSLP_PENDING_TEARDOWN
	 */
	nr_session_test s5(nr_session::STATE_ANSLP_PENDING_TEARDOWN, conf, START_MSN);

	event *e5 = new msg_event(NULL, create_anslp_bidding(START_MSN + 1), true);
	
	process(s5, e5);
	ASSERT_STATE(s5, nr_session::STATE_ANSLP_PENDING_TEARDOWN);
	ASSERT_NO_MESSAGE(d);
	ASSERT_NO_TIMER(d);
	
	/*
	 * STATE_ANSLP_PENDING_TEARDOWN ---[ api_bidding ]---> STATE_ANSLP_PENDING_TEARDOWN
	 */
	nr_session_test s6(nr_session::STATE_ANSLP_PENDING_TEARDOWN, conf, START_MSN);
	event *e6 = new msg_event(NULL, create_anslp_bidding(START_MSN + 1), true);

	process(s6, e6);
	ASSERT_STATE(s6, nr_session::STATE_ANSLP_PENDING_TEARDOWN);
	ASSERT_NO_MESSAGE(d);
	ASSERT_NO_TIMER(d);


	/*
	 * STATE_ANSLP_PENDING_TEARDOWN ---[ api_remove ]---> STATE_ANSLP_PENDING_TEARDOWN
	 */
	nr_session_test s7(nr_session::STATE_ANSLP_PENDING_TEARDOWN, conf, START_MSN);

	api_remove_event *e7 = new api_remove_event(new session_id(s6.get_id()), NULL);

	process(s7, e7);
	ASSERT_STATE(s7, nr_session::STATE_ANSLP_CLOSE);
	ASSERT_NO_MESSAGE(d);
	ASSERT_NO_TIMER(d);
		
}


void 
ResponderTest::testIntegratedStateMachine()
{

	/*
	 * CLOSE ---[rx_CREATE && CHECK_AA && CREATE(Lifetime>0) ]---> STATE_ANSLP_PENDING_INSTALLING.
	 */
	nr_session_test s1(nr_session::STATE_ANSLP_CLOSE, conf);
	event *e1 = new msg_event(NULL, create_anslp_create(), true);

	process(s1, e1);
	ASSERT_STATE(s1, nr_session::STATE_ANSLP_PENDING);
	ASSERT_TIMER_STARTED(d, s1.get_state_timer());

	api_check_event *e2 = new api_check_event(new session_id(s1.get_id()), NULL);

	objectListIter_t iter;
	for (iter = (s1.rule)->get_request_objects()->begin(); 
				iter != (s1.rule)->get_request_objects()->end(); ++iter){
		e2->setObject( mspec_rule_key(iter->first), (iter->second)->copy());  	
	}
	
	process(s1, e2);
	ASSERT_STATE(s1, nr_session::STATE_ANSLP_PENDING_INSTALLING);
	ASSERT_TIMER_STARTED(d, s1.get_state_timer());


	/* 
	 * STATE_ANSLP_PENDING_INSTALLING ---  [tg_install]  ---> AUCTIONING
	 */
	

	api_install_event *e3 = new api_install_event(new session_id(s1.get_id()), NULL);

	// objects installed.
	mspec_rule_key key1, key2, key3;
	e3->setObject( key1, mess1->copy());
	e3->setObject( key2, mess2->copy());
	e3->setObject( key3, mess3->copy());

	process(s1, e3);
	ASSERT_STATE(s1, nr_session::STATE_ANSLP_AUCTIONING);
	ASSERT_RESPONSE_MESSAGE_SENT(d, information_code::sc_success);
	ASSERT_TIMER_STARTED(d, s1.get_state_timer());


	/*
	 * STATE_ANSLP_AUCTIONING ---[rx_REFRESH && REFRESH(Lifetime == 0) ]--->STATE_ANSLP_PENDING_TEARDOWN
	 */
	event *e4 = new msg_event(NULL,
			create_anslp_refresh(START_MSN+1, 0), true);

	process(s1, e4);
	ASSERT_STATE(s1, nr_session::STATE_ANSLP_PENDING_TEARDOWN);
	ASSERT_RESPONSE_MESSAGE_SENT(d, information_code::sc_success);
	ASSERT_TIMER_STARTED(d, s1.get_state_timer());

	/*
	 * STATE_ANSLP_PENDING_TEARDOWN ---[ api_remove ]---> STATE_ANSLP_CLOSE
	 */
	api_remove_event *e5 = new api_remove_event(new session_id(s1.get_id()), NULL);

	process(s1, e5);
	ASSERT_STATE(s1, nr_session::STATE_ANSLP_CLOSE);
	ASSERT_NO_MESSAGE(d);
	ASSERT_NO_TIMER(d);
	
}
