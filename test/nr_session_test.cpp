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
	nr_session_test(state_t state=STATE_CLOSE, uint32 msn=0)
		: nr_session(state, msn) { }

	friend class ResponderTest;
};


class ResponderTest : public CppUnit::TestCase {

	CPPUNIT_TEST_SUITE( ResponderTest );

	CPPUNIT_TEST( testClose );
	CPPUNIT_TEST( testAuctioning );
	CPPUNIT_TEST( testIntegratedStateMachine );

	CPPUNIT_TEST_SUITE_END();

  public:
	void setUp();
	void tearDown();
	void add_fields(msg::anslp_ipap_message *mess);
	void testClose();
	void testAuctioning();
	void testIntegratedStateMachine();

  private:
	static const uint32 START_MSN = 77;
	static const uint32 SESSION_LIFETIME = 30;

	void process(nr_session_test &s, event *evt);

	msg::ntlp_msg *create_anslp_create(uint32 msn=START_MSN,
		uint32 session_lifetime=SESSION_LIFETIME) const;
	
	msg::ntlp_msg *create_anslp_refresh(uint32 msn, uint32 lt) const;

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
	uint32_t starttime = 100;
	uint32_t endtime = 200;
	unsigned char   *buf1  = (unsigned char *) "1";
	unsigned char   *buf1a  = (unsigned char *) "2";
	unsigned char   *buf1b  = (unsigned char *) "3";
	unsigned char   *buf2  = (unsigned char *) "bas";

	(mess->ip_message).delete_all_templates();

	int nfields = 4;
	templatedataid = (mess->ip_message).new_data_template( nfields, IPAP_SETID_AUCTION_TEMPLATE );
	(mess->ip_message).add_field(templatedataid, 0, IPAP_FT_IDAUCTION, 65535);
	(mess->ip_message).add_field(templatedataid, 0, IPAP_FT_STARTSECONDS, 4);
	(mess->ip_message).add_field(templatedataid, 0, IPAP_FT_ENDSECONDS, 4);	
	(mess->ip_message).add_field(templatedataid, 0, IPAP_FT_AUCTIONINGALGORITHMNAME, 65535);

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


void 
ResponderTest::testClose() {
	
	
	/*
	 * CLOSE ---[rx_CREATE && CHECK_AA && CREATE(Lifetime>0) ]---> AUCTIONING
	 */
	nr_session_test s1(nr_session::STATE_CLOSE);

	msg::ntlp_msg *msg = create_anslp_create();
	event *e1 = new msg_event(NULL, msg, true);
	 
	process(s1, e1);
	ASSERT_STATE(s1, nr_session::STATE_AUCTIONING);
	ASSERT_RESPONSE_MESSAGE_SENT(d, information_code::sc_success);
	ASSERT_TIMER_STARTED(d, s1.get_state_timer());
	
	/*
	 * CLOSE ---[rx_CREATE && CREATE(Lifetime > MAX) ]---> AUCTIONING
	 */
	nr_session_test s2(nr_session::STATE_CLOSE);
	event *e2 = new msg_event(NULL,
		create_anslp_create(START_MSN+1, 100000), true);
	
	process(s2, e2);
	ASSERT_STATE(s2, nr_session::STATE_AUCTIONING);
	ASSERT_RESPONSE_MESSAGE_SENT(d,
		information_code::sc_success);
	ASSERT_TIMER_STARTED(d, s2.get_state_timer());
	
}


void 
ResponderTest::testAuctioning() {
	
	
	/*
	 * STATE_AUCTIONING ---[rx_REFRESH && CHECK_AA && REFRESH(Lifetime>0) ]---> STATE_AUCTIONING
	 */
	nr_session_test s1(nr_session::STATE_AUCTIONING, START_MSN);
	event *e1 = new msg_event(NULL, create_anslp_refresh(START_MSN+1, 10), true);

	process(s1, e1);
	ASSERT_STATE(s1, nr_session::STATE_AUCTIONING);
	CPPUNIT_ASSERT( s1.get_msg_sequence_number() == START_MSN+1 );
	ASSERT_RESPONSE_MESSAGE_SENT(d, information_code::sc_success);
	ASSERT_TIMER_STARTED(d, s1.get_state_timer());

	/*
	 * STATE_AUCTIONING ---[rx_REFRESH && REFRESH(Lifetime > MAX) ]---> STATE_AUCTIONING
	 */
	nr_session_test s2(nr_session::STATE_AUCTIONING, START_MSN);
	event *e2 = new msg_event(NULL,
		create_anslp_refresh(START_MSN+1, 10), true);

	process(s2, e2);
	ASSERT_STATE(s2, nr_session::STATE_AUCTIONING);
	CPPUNIT_ASSERT( s1.get_msg_sequence_number() == START_MSN+1 );
	ASSERT_RESPONSE_MESSAGE_SENT(d, information_code::sc_success);
	ASSERT_TIMER_STARTED(d, s2.get_state_timer());

	/*
	 * STATE_AUCTIONING ---[rx_REFRESH && REFRESH(Lifetime == 0) ]--->STATE_CLOSE
	 */
	nr_session_test s3(nr_session::STATE_AUCTIONING, START_MSN);
	event *e3 = new msg_event(NULL,
			create_anslp_refresh(START_MSN+1, 0), true);

	process(s3, e3);
	ASSERT_STATE(s3, nr_session::STATE_CLOSE);
	ASSERT_RESPONSE_MESSAGE_SENT(d, information_code::sc_success);
	ASSERT_NO_TIMER(d);

	/*
	 * STATE_AUCTIONING_PART ---[rx_REFRESH && MSN too low ]---> STATE_AUCTIONING_PART
	 */
	nr_session_test s4(nr_session::STATE_AUCTIONING, START_MSN);
	event *e4 = new msg_event(NULL, create_anslp_refresh(START_MSN, 10), true);

	process(s4, e4);
	ASSERT_STATE(s4, nr_session::STATE_AUCTIONING);
	ASSERT_NO_MESSAGE(d);
	ASSERT_NO_TIMER(d);
}

void 
ResponderTest::testIntegratedStateMachine()
{

	/*
	 * CLOSE ---[rx_CREATE && CHECK_AA && CREATE(Lifetime>0) ]---> AUCTIONING
	 */
	nr_session_test s1(nr_session::STATE_CLOSE);
	event *e1 = new msg_event(NULL, create_anslp_create(), true);

	process(s1, e1);
	ASSERT_STATE(s1, nr_session::STATE_AUCTIONING);
	ASSERT_RESPONSE_MESSAGE_SENT(d, information_code::sc_success);
	ASSERT_TIMER_STARTED(d, s1.get_state_timer());


	/*
	 * STATE_AUCTIONING ---[rx_REFRESH && REFRESH(Lifetime == 0) ]--->STATE_CLOSE
	 */
	event *e2 = new msg_event(NULL,
			create_anslp_refresh(START_MSN+1, 0), true);

	process(s1, e2);
	ASSERT_STATE(s1, nr_session::STATE_CLOSE);
	ASSERT_RESPONSE_MESSAGE_SENT(d, information_code::sc_success);
	ASSERT_NO_TIMER(d);


}
