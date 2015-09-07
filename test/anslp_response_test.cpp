/*
 * Test the anslp_response class.
 *
 * $Id: anslp_response.cpp 1730 2015-09-03 21:51:00  $
 * $HeadURL: https://./test/anslp_response.cpp $
 */
#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

#include "network_message.h"

#include "msg/anslp_response.h"
#include "msg/anslp_object.h"

#include "msg/anslp_ie.h"
#include "msg/anslp_msg.h"
#include "msg/anslp_ipap_message.h"
#include "IpAp_field.h"
#include "IpAp_data_record.h"




using namespace anslp::msg;


class AnslpResponseTest : public CppUnit::TestCase {

	CPPUNIT_TEST_SUITE( AnslpResponseTest );

	CPPUNIT_TEST( testBasics );
	CPPUNIT_TEST( testComparison );
	CPPUNIT_TEST( testCopying );
	CPPUNIT_TEST( testManager01 );  // Message without ipap object
	CPPUNIT_TEST( testManager02 );  // Message with ipap object

	CPPUNIT_TEST_SUITE_END();

  public:
	void testBasics();
	void testComparison();
	void testCopying();
	void testManager01();
	void testManager02();
	
};

CPPUNIT_TEST_SUITE_REGISTRATION( AnslpResponseTest );


void AnslpResponseTest::testBasics() {
	anslp_response c1;

	c1.set_session_lifetime(27);
	CPPUNIT_ASSERT( c1.get_session_lifetime() == 27 );

	c1.set_msg_sequence_number(42000);
	CPPUNIT_ASSERT( c1.get_msg_sequence_number() == 42000 );

	c1.set_information_code(information_code::sc_success, 
					        information_code::suc_successfully_processed, 
					        information_code::obj_none);

	CPPUNIT_ASSERT( c1.get_severity_class() == information_code::sc_success );
	CPPUNIT_ASSERT( c1.get_response_code() == information_code::suc_successfully_processed );
	CPPUNIT_ASSERT( c1.get_response_object_type() == information_code::obj_none );

}


void AnslpResponseTest::testComparison() {
	anslp_response *c1 = new anslp_response();
	anslp_response *c2 = new anslp_response();
	c2->set_msg_sequence_number(7777);

	CPPUNIT_ASSERT( *c1 == *c1 );
	CPPUNIT_ASSERT( *c2 == *c2 );
	CPPUNIT_ASSERT( *c1 != *c2 );
	CPPUNIT_ASSERT( *c2 != *c1 );

	c1->set_msg_sequence_number(7777);
	CPPUNIT_ASSERT( *c1 == *c2 );
	CPPUNIT_ASSERT( *c2 == *c1 );

	delete c2;
	delete c1;
}

void AnslpResponseTest::testCopying() {
	anslp_response *c1 = new anslp_response();

	c1->set_session_lifetime(27);
	CPPUNIT_ASSERT( c1->get_session_lifetime() == 27 );

	c1->set_msg_sequence_number(42000);
	CPPUNIT_ASSERT( c1->get_msg_sequence_number() == 42000 );

	c1->set_information_code(information_code::sc_success, 
					        information_code::suc_successfully_processed, 
					        information_code::obj_none);


	anslp_response *c2 = c1->copy();
	CPPUNIT_ASSERT( c1 != c2 );
	CPPUNIT_ASSERT( *c1 == *c2 );

	delete c2;
	delete c1;
}


void AnslpResponseTest::testManager01() {
	ANSLP_IEManager::clear();
	ANSLP_IEManager::register_known_ies();
	ANSLP_IEManager *mgr = ANSLP_IEManager::instance();
	
    /* 
     * Builds the response message without any ipfix message.
     */
	anslp_response *m1 = new anslp_response();
	m1->set_session_lifetime(30);
	m1->set_msg_sequence_number(47);
    
    
	NetMsg msg( m1->get_serialized_size(IE::protocol_v1) );
	uint32 bytes_written;
	m1->serialize(msg, IE::protocol_v1, bytes_written);

	/*
	 * Now read the serialized object.
	 */
	
	std::cout << "----------------------------------------bytes written:" << m1->get_serialized_size(IE::protocol_v1) << std::endl;
	
	msg.set_pos(0);
	IEErrorList errlist;
	uint32 num_read;

	IE *ie = mgr->deserialize(msg, cat_anslp_msg, IE::protocol_v1, 
								errlist, num_read, false);
			
	std::cout << "--------------------------------------- bytes read:" << num_read << std::endl;	
	CPPUNIT_ASSERT( ie != NULL );
	CPPUNIT_ASSERT( errlist.is_empty() );
	CPPUNIT_ASSERT( num_read == ie->get_serialized_size(IE::protocol_v1) );
	CPPUNIT_ASSERT( ie != NULL );
    CPPUNIT_ASSERT( *m1 == *ie );
    
	delete m1;
	delete ie;
	mgr->clear();
    
}


void AnslpResponseTest::testManager02() {
	ANSLP_IEManager::clear();
	ANSLP_IEManager::register_known_ies();
	ANSLP_IEManager *mgr = ANSLP_IEManager::instance();


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

	anslp_ipap_message *mess;
    mess = new anslp_ipap_message(IPAP_VERSION);	
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

    /* 
     * Builds the refresh message without any ipfix message.
     */
	anslp_response *m1 = new anslp_response();
	m1->set_session_lifetime(30);
	m1->set_msg_sequence_number(47);
	m1->set_anslp_ipap_message(mess);
    
    
	NetMsg msg( m1->get_serialized_size(IE::protocol_v1) );
	uint32 bytes_written;
	m1->serialize(msg, IE::protocol_v1, bytes_written);

	/*
	 * Now read the serialized object.
	 */
	
	std::cout << "----------------------------------------bytes written:" << m1->get_serialized_size(IE::protocol_v1) << std::endl;
	
	msg.set_pos(0);
	IEErrorList errlist;
	uint32 num_read;

	IE *ie = mgr->deserialize(msg, cat_anslp_msg, IE::protocol_v1, errlist,
			num_read, false);
			
	std::cout << "--------------------------------------- bytes read:" << num_read << std::endl;	
	CPPUNIT_ASSERT( ie != NULL );
	CPPUNIT_ASSERT( errlist.is_empty() );
	CPPUNIT_ASSERT( num_read == ie->get_serialized_size(IE::protocol_v1) );
	CPPUNIT_ASSERT( ie != NULL );
    CPPUNIT_ASSERT( *m1 == *ie );
    
	delete m1;
	delete ie;
	mgr->clear();
    
}
