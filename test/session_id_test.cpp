/*
 * Test the session_id_test class.
 *
 * $Id: session_id_test.cpp 2015-10-21 10:03:00 amarentes $
 * $HeadURL: https://./test/session_id_test.cpp $
 */
#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

#include "session_id.h"

using namespace anslp;


class Session_Id_Test : public CppUnit::TestFixture {

	CPPUNIT_TEST_SUITE( Session_Id_Test );

	CPPUNIT_TEST( testAssign );

	CPPUNIT_TEST_SUITE_END();

  public:
	void setUp();
	void tearDown();
	void testAssign();

  private:
    
    
};

CPPUNIT_TEST_SUITE_REGISTRATION( Session_Id_Test );


void Session_Id_Test::setUp() 
{
			
}

void Session_Id_Test::tearDown() 
{

}

void Session_Id_Test::testAssign()
{
	anslp::session_id *session1 = new anslp::session_id();
	anslp::session_id *session2 = new anslp::session_id();
	
	anslp::session_id *session3 = new anslp::session_id(); 
	
	*session3 = *session1;
	
	CPPUNIT_ASSERT( *session3 == *session1 );
	cout << "session Id:" <<  session1->to_string() << endl;
	
}
// EOF
