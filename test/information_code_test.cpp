/*
 * Test the Information Code Object
 *
 * $Id: information_code.cpp 1743 2015-09-03 21:05:00  $
 * $HeadURL: https://./test/information_code.cpp $
 */
#include "generic_object_test.h"
#include "msg/information_code.h"
#include "msg/anslp_msg.h"

using namespace anslp::msg;


class InformationCodeTest : public GenericObjectTest {

	CPPUNIT_TEST_SUITE( InformationCodeTest );

	ANSLP_OBJECT_DEFAULT_TESTS();
	CPPUNIT_TEST( testBasics );

	CPPUNIT_TEST_SUITE_END();

  public:
	virtual anslp_object *createInstance1() const {
		return new information_code();
	}

	virtual anslp_object *createInstance2() const {
		return new information_code(information_code::sc_success);
	}

	virtual void mutate1(anslp_object *o) const {
		information_code *ic = dynamic_cast<information_code *>(o);
		ic->set_severity_class(information_code::sc_success);
	}

	void testBasics() {
		information_code ic(
			information_code::sc_success,
			information_code::suc_successfully_processed,
			anslp_create::MSG_TYPE);

		CPPUNIT_ASSERT( ic.get_response_object_type()
					== anslp_create::MSG_TYPE );
	}
};

CPPUNIT_TEST_SUITE_REGISTRATION( InformationCodeTest );

// EOF
