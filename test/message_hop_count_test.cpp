/*
 * Test the Message Hop Count Object
 *
 * $Id: message_hop_count.cpp 1743 2015-09-03 21:25:00  $
 * $HeadURL: https://./test/message_hop_count.cpp $
 */
#include "generic_object_test.h"
#include "msg/message_hop_count.h"
#include "msg/anslp_msg.h"

using namespace anslp::msg;

class MessageHopCountTest : public GenericObjectTest {

	CPPUNIT_TEST_SUITE( MessageHopCountTest );

	ANSLP_OBJECT_DEFAULT_TESTS();
	CPPUNIT_TEST( testBasics );

	CPPUNIT_TEST_SUITE_END();

  public:
	virtual anslp_object *createInstance1() const {
		return new message_hop_count();
	}

	virtual anslp_object *createInstance2() const {
		return new message_hop_count(10);
	}

	virtual void mutate1(anslp_object *o) const {
		message_hop_count *mhc = dynamic_cast<message_hop_count *>(o);
		mhc->set_value(10);
	}

	void testBasics() {
		message_hop_count ic(12);
		CPPUNIT_ASSERT( ic.get_value() == 12 );
		ic.set_value(13);
		CPPUNIT_ASSERT( ic.get_value() == 13 );
		message_hop_count ic2 = ic;
		CPPUNIT_ASSERT( ic2.get_value() == 13 );
					
	}
};

CPPUNIT_TEST_SUITE_REGISTRATION( MessageHopCountTest );
