/*
 * Test the nf_session class.
 *
 * $Id: nf_session.cpp 1936 2015-09-03 22:01:00 amarentes $
 * $HeadURL: https://./test/nf_session.cpp $
 */
#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

#include "events.h"
#include "session.h"
#include "dispatcher.h"
#include "auction_rule.h"
#include "msg/selection_auctioning_entities.h"

#include "utils.h" // custom assertions

using namespace anslp;
using namespace anslp::msg;

class ForwarderTest;

/*
 * We use a subclass for testing and make the test case a friend. This
 * way the test cases have access to protected methods and they don't have
 * to be public in nf_session.
 */
class nf_session_test : public nf_session {
  public:
	nf_session_test(state_t state, anslp_config *conf)
		: nf_session(state, conf) { }

	friend class ForwarderTest;
};


class ForwarderTest : public CppUnit::TestCase {

	CPPUNIT_TEST_SUITE( ForwarderTest );

	CPPUNIT_TEST( testClose );
	CPPUNIT_TEST( testPending );
	CPPUNIT_TEST( testAuctioning );
	CPPUNIT_TEST( testIntegratedStateMachine );

	CPPUNIT_TEST_SUITE_END();

  public:
	void setUp();
	void tearDown();

	void add_fields(msg::anslp_ipap_message *mess);	

	void testClose();
	void testPending();
	void testAuctioning();
	void testIntegratedStateMachine();

  private:
	void process(nf_session_test &s, event *evt);

	static const uint32 START_MSN = 77;
	static const uint32 SESSION_LIFETIME = 30;

	msg::ntlp_msg *create_anslp_create(uint32 msn=START_MSN,
				uint32 session_lifetime=SESSION_LIFETIME) const;

	msg::ntlp_msg *create_anslp_response(
		uint8 severity, uint8 response_code, uint16 msg_type,
		uint32 msn=START_MSN) const;

	msg::ntlp_msg *create_anslp_refresh(
		uint32 msn, uint32 lt) const;
		
	mock_anslp_config *conf;
	auction_rule_installer *rule_installer;
	mock_dispatcher *d;
	hostaddress destination;
	
	msg::anslp_ipap_message *mess1;
	msg::anslp_ipap_message *mess2;
	msg::anslp_ipap_message *mess3;
	
};

CPPUNIT_TEST_SUITE_REGISTRATION( ForwarderTest );


void ForwarderTest::add_fields(msg::anslp_ipap_message * mess) 
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
void ForwarderTest::process(nf_session_test &s, event *evt) {
	d->clear();
	s.process(d, evt);
	delete evt;
}

void ForwarderTest::setUp() {
	conf = new mock_anslp_config();
	rule_installer = new nop_auction_rule_installer(conf);
	d = new mock_dispatcher(NULL, rule_installer, conf);
	destination = hostaddress("157.253.203.5");
	
	mess1 = new msg::anslp_ipap_message(IPAP_VERSION, true);
	mess2 = new msg::anslp_ipap_message(IPAP_VERSION, true);
	mess3 = new msg::anslp_ipap_message(IPAP_VERSION, true);
	
	add_fields(mess1);
	add_fields(mess2);
	add_fields(mess3);	
}

void ForwarderTest::tearDown() 
{
	delete d;
	delete rule_installer;
	delete conf;
	delete mess1;
	delete mess2;
	delete mess3;
}


msg::ntlp_msg *
ForwarderTest::create_anslp_create(uint32 msn, uint32 lt) const 
{

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
		"tcp", 0, 0, 0, true
	);

	return new msg::ntlp_msg(session_id(), create, ntlp_mri, 0);
}


msg::ntlp_msg *
ForwarderTest::create_anslp_response(uint8 severity,
		uint8 response_code, uint16 msg_type, uint32 msn) const 
{

	anslp_response *resp = new anslp_response();
	resp->set_information_code(severity, response_code, msg_type);
	resp->set_msg_sequence_number(msn);

	ntlp::mri *ntlp_mri = new ntlp::mri_pathcoupled(
		hostaddress("192.168.0.4"), 32, 0,
		hostaddress("192.168.0.5"), 32, 0,
		"tcp", 0, 0, 0, true
	);

	return new msg::ntlp_msg(session_id(), resp, ntlp_mri, 0);
}

msg::ntlp_msg *
ForwarderTest::create_anslp_refresh(uint32 msn, uint32 lt) const 
{

	msg::anslp_refresh *refresh = new anslp_refresh();
	refresh->set_session_lifetime(lt);
	refresh->set_msg_sequence_number(msn);

	msg::ntlp_msg *msg = new msg::ntlp_msg(
		session_id(), refresh,
		new ntlp::mri_pathcoupled(), 0
	);

	return msg;
}


void ForwarderTest::testClose() {
	/*
	 * STATE_ANSLP_CLOSE ---[rx_CREATE && CREATE(Lifetime>0) ]---> STATE_ANSLP_PENDING
	 */
	nf_session_test s1(nf_session::STATE_ANSLP_CLOSE, conf);
	event *e1 = new msg_event(new session_id(s1.get_id()),
		create_anslp_create());
		
	process(s1, e1);
	ASSERT_STATE(s1, nf_session::STATE_ANSLP_PENDING);
	ASSERT_CREATE_MESSAGE_SENT(d);
	ASSERT_TIMER_STARTED(d, s1.get_state_timer());

	
	/*
	 * STATE_ANSLP_CLOSE ---[rx_CREATE && CREATE(Lifetime > MAX) ]---> STATE_ANSLP_PENDING
	 */
	nf_session_test s2(nf_session::STATE_ANSLP_CLOSE, conf);
	event *e2 = new msg_event(new session_id(s2.get_id()),
		create_anslp_create(START_MSN, 1000000)); // more than allowed
	
	process(s2, e2);
	ASSERT_STATE(s2, nf_session::STATE_ANSLP_PENDING);
	ASSERT_CREATE_MESSAGE_SENT(d);
	ASSERT_TIMER_STARTED(d, s2.get_state_timer());
	
}


void ForwarderTest::testPending() {
	
	/*
	 * STATE_ANSLP_PENDING ---[rx_RESPONSE(SUCCESS,CREATE)]---> STATE_ANSLP_AUCTIONING
	 */
	nf_session_test s1(nf_session::STATE_ANSLP_PENDING, conf);
	s1.set_last_create_message(create_anslp_create());

	ntlp_msg *resp1 = create_anslp_response(information_code::sc_success,
		information_code::suc_successfully_processed,
		information_code::obj_none, START_MSN);

	event *e1 = new msg_event(new session_id(s1.get_id()), resp1);

	process(s1, e1);
	ASSERT_STATE(s1, nf_session::STATE_ANSLP_AUCTIONING);
	ASSERT_RESPONSE_MESSAGE_SENT(d, information_code::sc_success);
	ASSERT_TIMER_STARTED(d, s1.get_state_timer());

	/*
	 * STATE_ANSLP_PENDING ---[rx_RESPONSE(ERROR,CREATE)]---> STATE_ANSLP_CLOSE
	 */
	nf_session_test s2(nf_session::STATE_ANSLP_PENDING, conf);
	s2.set_last_create_message(create_anslp_create());

	ntlp_msg *resp2 = create_anslp_response(
		information_code::sc_permanent_failure, 0,
		information_code::obj_none, START_MSN);

	event *e2 = new msg_event(new session_id(s2.get_id()), resp2);

	process(s2, e2);
	ASSERT_STATE(s2, nf_session::STATE_ANSLP_CLOSE);
	ASSERT_RESPONSE_MESSAGE_SENT(d, information_code::sc_permanent_failure);
	ASSERT_NO_TIMER(d);

	/*
	 * STATE_ANSLP_PENDING ---[rx_CREATE && CREATE(Lifetime == 0) ]---> STATE_ANSLP_CLOSE
	 */
	nf_session_test s3(nf_session::STATE_ANSLP_PENDING, conf);
	s3.set_last_create_message(create_anslp_create());

	event *e3 = new msg_event(new session_id(s3.get_id()),
			create_anslp_create(START_MSN+1, 0));

	// We must to wait for a response successful response message. 
	process(s3, e3);
	ASSERT_STATE(s3, nf_session::STATE_ANSLP_CLOSE);
	ASSERT_CREATE_MESSAGE_SENT(d);
	ASSERT_NO_TIMER(d);

	/*
	 * STATE_ANSLP_PENDING ---[rx_CREATE && CREATE(Lifetime > 0) ]---> STATE_PENDING
	 */
	nf_session_test s4(nf_session::STATE_ANSLP_PENDING, conf);
	s4.set_last_create_message(create_anslp_create());

	event *e4 = new msg_event(new session_id(s4.get_id()),
			create_anslp_create(START_MSN, 10));

	// We must to wait for a response successful response message. 
	process(s4, e4);
	ASSERT_STATE(s4, nf_session::STATE_ANSLP_PENDING);
	ASSERT_CREATE_MESSAGE_SENT(d);
	ASSERT_TIMER_STARTED(d, s4.get_state_timer());

	/*
	 * STATE_ANSLP_PENDING ---[rx_CREATE && CREATE(Lifetime > 0) ]---> STATE_ANSLP_PENDING
	 * State changes from participating to forward because it is assumed any 
	 * selection metering entities.
	 */
	nf_session_test s5(nf_session::STATE_ANSLP_PENDING, conf);
	s5.set_last_create_message(create_anslp_create());

	event *e5 = new msg_event(new session_id(s5.get_id()),
			create_anslp_create(START_MSN+1, 10));

	// We must to wait for a response successful response message. 
	process(s5, e5);
	ASSERT_STATE(s5, nf_session::STATE_ANSLP_PENDING);
	ASSERT_CREATE_MESSAGE_SENT(d);
	ASSERT_TIMER_STARTED(d, s5.get_state_timer());
	
	/*
	 * STATE_ANSLP_PENDING ---[STATE_TIMEOUT]---> STATE_ANSLP_CLOSE
	 */
	nf_session_test s6(nf_session::STATE_ANSLP_PENDING, conf);

	s6.get_state_timer().set_id(47);
	s6.set_last_create_message(create_anslp_create());
	timer_event *e6 = new timer_event(NULL, 47);

	process(s6, e6);
	ASSERT_STATE(s6, nf_session::STATE_ANSLP_CLOSE);
	ASSERT_RESPONSE_MESSAGE_SENT(d, information_code::sc_permanent_failure);
	ASSERT_NO_TIMER(d);
	
}

void ForwarderTest::testAuctioning() {
	

	/*
	 * STATE_ANSLP_AUCTIONING ---[RESPONSE_TIMEOUT]---> STATE_ANSLP_CLOSE
	 */
	nf_session_test s2(nf_session::STATE_ANSLP_AUCTIONING, conf);

	s2.set_last_refresh_message(create_anslp_refresh(START_MSN+1, 10));
	s2.get_response_timer().set_id(47);

	timer_event *e2 = new timer_event(NULL, 47);

	process(s2, e2);
	ASSERT_STATE(s2, nf_session::STATE_ANSLP_CLOSE);
	ASSERT_RESPONSE_MESSAGE_SENT(d, information_code::sc_permanent_failure);
	ASSERT_NO_TIMER(d);
		
    /*
     * STATE_ANSLP_AUCTIONING ---[ && REFRESH(Lifetime == 0) ]---> STATE_ANSLP_AUCTIONING
	 */
	nf_session_test s3(nf_session::STATE_ANSLP_AUCTIONING, conf);
	s3.set_last_create_message(create_anslp_create());

	event *e3 = new msg_event(new session_id(s3.get_id()),
		create_anslp_refresh(START_MSN+1, 0));

	process(s3, e3);
	ASSERT_STATE(s3, nf_session::STATE_ANSLP_AUCTIONING);
	ASSERT_REFRESH_MESSAGE_SENT(d);
	ASSERT_TIMER_STARTED(d, s3.get_response_timer());
	
	/*
	 * STATE_ANSLP_AUCTIONING ---[rx_REFRESH && REFRESH(Lifetime == 0) ]---> STATE_ANSLP_AUCTIONING
	 */
	nf_session_test s4(nf_session::STATE_ANSLP_AUCTIONING, conf);
	s4.set_last_refresh_message(create_anslp_refresh(START_MSN+1, 10));

	event *e4 = new msg_event(new session_id(s4.get_id()),
		create_anslp_refresh(START_MSN+1, 0));

	process(s4, e4);
	ASSERT_STATE(s4, nf_session::STATE_ANSLP_AUCTIONING);
	ASSERT_REFRESH_MESSAGE_SENT(d);
	ASSERT_TIMER_STARTED(d, s4.get_response_timer());
	
	/*
	 * STATE_ANSLP_AUCTIONING ---[rx_REFRESH, STATE_TIMEOUT]---> STATE_ANSLP_CLOSE
	 */
	nf_session_test s5(nf_session::STATE_ANSLP_AUCTIONING, conf);

	s5.set_last_refresh_message(create_anslp_refresh(START_MSN+1, 10));
	s5.get_state_timer().set_id(47);

	timer_event *e5 = new timer_event(NULL, 47);
	
	process(s5, e5);
		
	ASSERT_STATE(s5, nf_session::STATE_ANSLP_CLOSE);
	ASSERT_NO_MESSAGE(d);
	ASSERT_NO_TIMER(d);
	
	/*
	 * STATE_ANSLP_AUCTIONING ---[rx_create, STATE_TIMEOUT]---> STATE_ANSLP_CLOSE
	 */
	nf_session_test s6(nf_session::STATE_ANSLP_AUCTIONING, conf);

	s6.set_last_create_message(create_anslp_create());
	s6.get_state_timer().set_id(47);

	timer_event *e6 = new timer_event(NULL, 47);

	process(s6, e6);
	ASSERT_STATE(s6, nf_session::STATE_ANSLP_CLOSE);
	ASSERT_NO_MESSAGE(d);
	ASSERT_NO_TIMER(d);
	
	/*
	 * STATE_ANSLP_AUCTIONING ---[rx_REFRESH , RESPONSE=success, lifetime >0 ]---> STATE_ANSLP_AUCTIONING
	 */
	nf_session_test s7(nf_session::STATE_ANSLP_AUCTIONING, conf);

	s7.set_last_refresh_message(create_anslp_refresh(START_MSN+1, 10));
	s7.set_lifetime(10);

	ntlp_msg *resp1 = create_anslp_response(information_code::sc_success,
		information_code::suc_successfully_processed,
		information_code::obj_none, START_MSN+1);

	event *e7 = new msg_event(new session_id(s7.get_id()), resp1);

	process(s7, e7);
	ASSERT_STATE(s7, nf_session::STATE_ANSLP_AUCTIONING);
	ASSERT_RESPONSE_MESSAGE_SENT(d, information_code::sc_success);
	ASSERT_TIMER_STARTED(d, s7.get_state_timer());

	/*
	 * STATE_ANSLP_AUCTIONING ---[rx_REFRESH , RESPONSE=success, lifetime =0 ]---> STATE_ANSLP_CLOSE
	 */
	nf_session_test s8(nf_session::STATE_ANSLP_AUCTIONING, conf);

	s8.set_last_refresh_message(create_anslp_refresh(START_MSN+1, 0));
	s8.set_lifetime(0);

	ntlp_msg *resp2 = create_anslp_response(information_code::sc_success,
		information_code::suc_successfully_processed,
		information_code::obj_none, START_MSN+1);

	event *e8 = new msg_event(new session_id(s8.get_id()), resp2);

	process(s8, e8);
	ASSERT_STATE(s8, nf_session::STATE_ANSLP_CLOSE);
	ASSERT_RESPONSE_MESSAGE_SENT(d, information_code::sc_success);
	ASSERT_TIMER_STARTED(d, s8.get_state_timer());
	
}

void 
ForwarderTest::testIntegratedStateMachine()
{

	/*
	 * STATE_ANSLP_CLOSE ---[rx_CREATE && CREATE(Lifetime>0) ]---> STATE_ANSLP_PENDING
	 */
	nf_session_test s1(nf_session::STATE_ANSLP_CLOSE, conf);
	event *e1 = new msg_event(new session_id(s1.get_id()),
		create_anslp_create());

	process(s1, e1);
	ASSERT_STATE(s1, nf_session::STATE_ANSLP_PENDING);
	ASSERT_CREATE_MESSAGE_SENT(d);
	ASSERT_TIMER_STARTED(d, s1.get_state_timer());


	/*
	 * STATE_ANSLP_PENDING ---[rx_RESPONSE(SUCCESS,CREATE)]---> STATE_ANSLP_AUCTIONING
	 */
	ntlp_msg *resp = create_anslp_response(information_code::sc_success,
		information_code::suc_successfully_processed,
		information_code::obj_none, START_MSN);

	event *e2 = new msg_event(new session_id(s1.get_id()), resp);

	process(s1, e2);
	ASSERT_STATE(s1, nf_session::STATE_ANSLP_AUCTIONING);
	ASSERT_RESPONSE_MESSAGE_SENT(d, information_code::sc_success);
	ASSERT_TIMER_STARTED(d, s1.get_state_timer());

    /*
     * STATE_ANSLP_AUCTIONING ---[ && REFRESH(Lifetime == 0) ]---> STATE_AUCTIONING
	 */
	event *e3 = new msg_event(new session_id(s1.get_id()),
							create_anslp_refresh(START_MSN+1, 0));

	process(s1, e3);
	ASSERT_STATE(s1, nf_session::STATE_ANSLP_AUCTIONING);
	ASSERT_REFRESH_MESSAGE_SENT(d);
	ASSERT_TIMER_STARTED(d, s1.get_response_timer());

	/*
	 * STATE_ANSLP_AUCTIONING ---[rx_REFRESH, lifetime =0, RESPONSE=success ]---> STATE_ANSLP_CLOSE
	 */

	ntlp_msg *resp2 = create_anslp_response(information_code::sc_success,
		information_code::suc_successfully_processed,
		information_code::obj_none, START_MSN+1);

	event *e4 = new msg_event(new session_id(s1.get_id()), resp2);

	process(s1, e4);
	ASSERT_STATE(s1, nf_session::STATE_ANSLP_CLOSE);
	ASSERT_RESPONSE_MESSAGE_SENT(d, information_code::sc_success);
	ASSERT_TIMER_STARTED(d, s1.get_state_timer());

	
}
