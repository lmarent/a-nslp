/*
 * Test the anslp_refresh class.
 *
 * $Id: anslp_refresh.cpp 1730 2015-09-03 21:46:00  $
 * $HeadURL: https://./test/anslp_refresh.cpp $
 */
#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

#include "network_message.h"

#include "msg/anslp_refresh.h"
#include "msg/anslp_object.h"

#include "msg/anslp_ie.h"
#include "msg/anslp_msg.h"
#include "IpAp_field.h"
#include "IpAp_data_record.h"
#include "msg/information_code.h"
#include "msg/msg_sequence_number.h"



using namespace anslp::msg;


class AnslpRefreshTest : public CppUnit::TestCase {

	CPPUNIT_TEST_SUITE( AnslpRefreshTest );

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

CPPUNIT_TEST_SUITE_REGISTRATION( AnslpRefreshTest );


void AnslpRefreshTest::testBasics() {
	anslp_refresh c1;
	uint32 numTmp = 4;
	uint32 slt = 100;

	CPPUNIT_ASSERT( c1.get_msg_sequence_number() == 0 );
	c1.set_msg_sequence_number(numTmp);
	CPPUNIT_ASSERT( c1.get_msg_sequence_number() == 4 );
	CPPUNIT_ASSERT( c1.get_session_lifetime() == 0 );
	c1.set_session_lifetime(slt);
	CPPUNIT_ASSERT( c1.get_session_lifetime() == 100 );


}


void AnslpRefreshTest::testComparison() {
	
	uint32 numTmp = 4;
	uint32 slt = 100;
	
	anslp_refresh *c1 = new anslp_refresh();
	anslp_refresh *c2 = new anslp_refresh();
	c2->set_msg_sequence_number(numTmp);
	c2->set_session_lifetime(slt);

	CPPUNIT_ASSERT( *c1 == *c1 );
	CPPUNIT_ASSERT( *c2 == *c2 );
	CPPUNIT_ASSERT( *c1 != *c2 );
	CPPUNIT_ASSERT( *c2 != *c1 );

	c1->set_msg_sequence_number(numTmp);
	c1->set_session_lifetime(slt);
	
	CPPUNIT_ASSERT( *c1 == *c2 );
	CPPUNIT_ASSERT( *c2 == *c1 );

	delete c2;
	delete c1;
}

void AnslpRefreshTest::testCopying() {
	uint32 numTmp = 4;
	uint32 slt = 100;
	
	anslp_refresh *c1 = new anslp_refresh();
	c1->set_msg_sequence_number(numTmp);
	c1->set_session_lifetime(slt);

	anslp_refresh *c2 = c1->copy();
	CPPUNIT_ASSERT( c1 != c2 );
	CPPUNIT_ASSERT( *c1 == *c2 );

	delete c2;
	delete c1;
}


void AnslpRefreshTest::testManager() {

	uint32 numTmp = 4;
	uint32 slt = 100;
	ANSLP_IEManager::clear();
	ANSLP_IEManager::register_known_ies();
	ANSLP_IEManager *mgr = ANSLP_IEManager::instance();

    /* 
     * Builds the notify message
     */
	anslp_refresh *m1 = new anslp_refresh();
	m1->set_msg_sequence_number(numTmp);
	m1->set_session_lifetime(slt);
       
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
