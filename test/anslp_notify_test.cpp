/*
 * Test the anslp_notify class.
 *
 * $Id: anslp_notify.cpp 1730 2015-09-03 21:42:00  $
 * $HeadURL: https://./test/anslp_notify.cpp $
 */
#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

#include "network_message.h"

#include "msg/anslp_notify.h"
#include "msg/anslp_object.h"

#include "msg/anslp_ie.h"
#include "msg/anslp_msg.h"
#include "msg/anslp_ipap_message.h"
#include "IpAp_field.h"
#include "IpAp_data_record.h"
#include "msg/information_code.h"


using namespace anslp::msg;


class AnslpNotifyTest : public CppUnit::TestCase {

	CPPUNIT_TEST_SUITE( AnslpNotifyTest );

	CPPUNIT_TEST( testBasics );
	CPPUNIT_TEST( testComparison );
	CPPUNIT_TEST( testCopying );
	CPPUNIT_TEST( testManager );

	CPPUNIT_TEST_SUITE_END();

  public:
	void testBasics();
	void testComparison();
	void testCopying();
	void testManager();
};

CPPUNIT_TEST_SUITE_REGISTRATION( AnslpNotifyTest );


void AnslpNotifyTest::testBasics() {
	anslp_notify c1;

	c1.set_information_code(information_code::sc_success, 
					        information_code::suc_successfully_processed, 
					        information_code::obj_none);
	CPPUNIT_ASSERT( c1.get_severity_class() == information_code::sc_success );
	CPPUNIT_ASSERT( c1.get_response_code() == information_code::suc_successfully_processed );
	CPPUNIT_ASSERT( c1.get_response_object_type() == information_code::obj_none );


}


void AnslpNotifyTest::testComparison() {
	anslp_notify *c1 = new anslp_notify();
	anslp_notify *c2 = new anslp_notify();
	c2->set_information_code(information_code::sc_success, 
					        information_code::suc_successfully_processed, 
					        information_code::obj_none);

	CPPUNIT_ASSERT( *c1 == *c1 );
	CPPUNIT_ASSERT( *c2 == *c2 );
	CPPUNIT_ASSERT( *c1 != *c2 );
	CPPUNIT_ASSERT( *c2 != *c1 );

	c1->set_information_code(information_code::sc_success, 
					        information_code::suc_successfully_processed, 
					        information_code::obj_none);
	CPPUNIT_ASSERT( *c1 == *c2 );
	CPPUNIT_ASSERT( *c2 == *c1 );

	delete c2;
	delete c1;
}

void AnslpNotifyTest::testCopying() {
	anslp_notify *c1 = new anslp_notify();

	anslp_notify *c2 = c1->copy();
	CPPUNIT_ASSERT( c1 != c2 );
	CPPUNIT_ASSERT( *c1 == *c2 );

	delete c2;
	delete c1;
}


void AnslpNotifyTest::testManager() {

	ANSLP_IEManager::clear();

	ANSLP_IEManager::register_known_ies();

	ANSLP_IEManager *mgr = ANSLP_IEManager::instance();

    /* 
     * Builds the notify message
     */
	anslp_notify *m1 = new anslp_notify();
	m1->set_information_code(information_code::sc_success, 
					        information_code::suc_successfully_processed, 
					        information_code::obj_none);
       
	NetMsg msg( m1->get_serialized_size(IE::protocol_v1) );
	uint32 bytes_written;
	m1->serialize(msg, IE::protocol_v1, bytes_written);

	/*
	 * Now read the serialized object.
	 */
		
	msg.set_pos(0);
	IEErrorList errlist;
	uint32 num_read;

	IE *ie = mgr->deserialize(msg, cat_anslp_msg, IE::protocol_v1, errlist,
			num_read, false);
			
	CPPUNIT_ASSERT( ie != NULL );
	CPPUNIT_ASSERT( errlist.is_empty() );
	CPPUNIT_ASSERT( num_read == ie->get_serialized_size(IE::protocol_v1) );
	CPPUNIT_ASSERT( ie != NULL );
    CPPUNIT_ASSERT( *m1 == *ie );
    
    mgr->clear();
        
}
