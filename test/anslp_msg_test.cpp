/*
 * anslp_msg.cpp - Test the anslp_msg class.
 *
 * $Id: anslp_msg.cpp 2438 2015-09-03 21:07:00Z amarentes $
 * $HeadURL: https://./test/anslp_msg.cpp $
 *
 */
#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

#include "network_message.h"

#include "msg/anslp_ie.h"
#include "msg/anslp_msg.h"
#include "msg/anslp_object.h"

using namespace anslp::msg;


class AnslpMsgTest : public CppUnit::TestCase {

	CPPUNIT_TEST_SUITE( AnslpMsgTest );

	CPPUNIT_TEST( testComparison );
	CPPUNIT_TEST( testCopying );


	CPPUNIT_TEST_SUITE_END();

  public:
	void testComparison();
	void testCopying();
};

CPPUNIT_TEST_SUITE_REGISTRATION( AnslpMsgTest );


void AnslpMsgTest::testComparison() 
{
		
	anslp_refresh *m1 = new anslp_refresh();
	anslp_refresh *m2 = new anslp_refresh();

	CPPUNIT_ASSERT( *m1 == *m1 );
	CPPUNIT_ASSERT( *m2 == *m2 );
	CPPUNIT_ASSERT( *m1 == *m2 );
	CPPUNIT_ASSERT( *m2 == *m1 );
   
	m1->set_session_lifetime(10);	
	CPPUNIT_ASSERT( *m1 != *m2 );

	m2->set_session_lifetime(10);
	CPPUNIT_ASSERT( *m1 == *m2 );

	delete m2;
	delete m1;
}


void AnslpMsgTest::testCopying() 
{
	anslp_create *m1 = new anslp_create();

	anslp_create *m2 = m1->copy();
	CPPUNIT_ASSERT( m1 != m2 );
	CPPUNIT_ASSERT( *m1 == *m2 );

	delete m2;
	delete m1;
}
