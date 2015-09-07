/*
 * Test the Session Lifetime Object
 *
 * $Id: session_lifetime.cpp 1707 2015-09-03 21:06:00 $
 * $HeadURL: https://./test/session_lifetime.cpp $
 */
#include "generic_object_test.h"

using namespace anslp::msg;


class SessionLifetimeTest : public GenericObjectTest {

	CPPUNIT_TEST_SUITE( SessionLifetimeTest );

	ANSLP_OBJECT_DEFAULT_TESTS();

	CPPUNIT_TEST_SUITE_END();

  public:
	virtual anslp_object *createInstance1() const {
		return new session_lifetime();
	}

	virtual anslp_object *createInstance2() const {
		return new session_lifetime(30);
	}

	virtual void mutate1(anslp_object *o) const {
		session_lifetime *lt = dynamic_cast<session_lifetime *>(o);
		lt->set_value(50);
	}
};

CPPUNIT_TEST_SUITE_REGISTRATION( SessionLifetimeTest );

// EOF
