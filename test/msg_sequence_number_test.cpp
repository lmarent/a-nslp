/*
 * Test the Information Code Object
 *
 * $Id: msg_sequence_number.cpp 1743 2015-09-03 21:57:00  $
 * $HeadURL: https://./test/msg_sequence_number.cpp $
 */
#include "generic_object_test.h"
#include "msg/msg_sequence_number.h"
#include "msg/anslp_msg.h"

using namespace anslp::msg;


class MsgSequenceNumberTest : public GenericObjectTest {

	CPPUNIT_TEST_SUITE( MsgSequenceNumberTest );

	ANSLP_OBJECT_DEFAULT_TESTS();
	CPPUNIT_TEST( testBasics );

	CPPUNIT_TEST_SUITE_END();

  public:
	virtual anslp_object *createInstance1() const {
		return new msg_sequence_number();
	}

	virtual anslp_object *createInstance2() const {
		return new msg_sequence_number(15);
	}

	virtual void mutate1(anslp_object *o) const {
		msg_sequence_number *ic = dynamic_cast<msg_sequence_number *>(o);
		ic->set_value(14);
	}

	void testBasics() {
		msg_sequence_number msn(17);

		CPPUNIT_ASSERT( msn.get_value() == 17 );
		msg_sequence_number msn2 = msn;
		CPPUNIT_ASSERT( msn2.get_value() == 17 );
		msn2.set_value(18);
		CPPUNIT_ASSERT( msn2.get_value() == 18 );
		
	}
};

CPPUNIT_TEST_SUITE_REGISTRATION( MsgSequenceNumberTest );
