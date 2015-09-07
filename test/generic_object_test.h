/*
 * A generic test case for A-NSLP objects.
 *
 * $Id: generic_object_test.h 1732 2015-09-04 8:42:00 amarentes $
 * $HeadURL: https://./test/generic_object_test.h $
 */
#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

#include "msg/anslp_object.h"

using namespace anslp::msg;


/**
 * Helper macro to register test cases in child classes.
 */
#define ANSLP_OBJECT_DEFAULT_TESTS() \
	CPPUNIT_TEST( testMyself ); \
	CPPUNIT_TEST( testComparison ); \
	CPPUNIT_TEST( testCopying ); \
	CPPUNIT_TEST( testSerialize ); \
	CPPUNIT_TEST( testReadWrite );


/**
 * The abstract base class for all object tests.
 *
 * This class uses the Strategy design pattern to run a set of tests which
 * are the same for all anslp_object child classes. Additional to those
 * tests, child classes may define more specific test cases.
 */
class GenericObjectTest : public CppUnit::TestCase {

  protected:
	/**
	 * Create an instance using the default constructor.
	 *
	 * @return an instance created using the default constructor
	 */
	virtual anslp_object *createInstance1() const = 0;

	/**
	 * Create an instance that is not equal to an instance created
	 * by the default constructor or created by createInstance1().
	 *
	 * @return an instance different from that created by createInstance1()
	 */
	virtual anslp_object *createInstance2() const = 0;

	/**
	 * Modify the given parameter.
	 *
	 * An instance created by createInstance1() that is modified by
	 * this method must no longer be equal to that newly created instance.
	 *
	 * @param param the parameter to modify
	 */
	virtual void mutate1(anslp_object *param) const = 0;

  public:
	// Note: Adding a "const" modifier breaks cppunit.
	virtual void testMyself();
	virtual void testComparison();
	virtual void testCopying();
	virtual void testSerialize();
	virtual void testReadWrite();

  protected:
	virtual void tryReadWrite(anslp_object *obj, anslp_object *blank);
};
