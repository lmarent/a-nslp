/*
 * Test the ipap_message_test class.
 *
 * $Id: ipap_message_test.cpp 2015-08-28 17:53:00 amarentes $
 * $HeadURL: https://./test/ipap_message_test.cpp $
 */
#include <cppunit/TestCase.h>
#include <cppunit/extensions/HelperMacros.h>

#include "IpAp_def.h"
#include "IpAp_data_record.h"
#include "IpAp_message.h"
#include "anslp_ipap_message.h"
#include "generic_object_test.h"

using namespace anslp::msg;


class Anslp_IpAp_Message_Test : public GenericObjectTest {

	CPPUNIT_TEST_SUITE( Anslp_IpAp_Message_Test );
	CPPUNIT_TEST( testAddTemplate );
	CPPUNIT_TEST( testExceptionAddTemplate );
	CPPUNIT_TEST( testDataRecords );
	CPPUNIT_TEST( testExportImport );
	ANSLP_OBJECT_DEFAULT_TESTS();
	CPPUNIT_TEST( testBasics );
	CPPUNIT_TEST_SUITE_END();

  public:
	void setUp();
	void tearDown();
	void buildMessage(anslp_ipap_message *message) const;
	void testAddTemplate();
	void testExceptionAddTemplate();
	void testDataRecords();
	void testExportImport();
	void testBasics();

	virtual anslp_object *createInstance1() const; 
	virtual anslp_object *createInstance2() const;
	virtual void mutate1(anslp_object *o) const;
  
  private:  
    mutable anslp_ipap_message *mes;
    mutable anslp_ipap_message *mes2;

	Logger *log; //!< link to global logger object
	int ch;      //!< logging channel number used by objects of this class

};

CPPUNIT_TEST_SUITE_REGISTRATION( Anslp_IpAp_Message_Test );


void
Anslp_IpAp_Message_Test::buildMessage(anslp_ipap_message *message) const
{

	uint16_t templatedataid = 0;
	uint64_t starttime = 100;
	uint64_t endtime = 200;
	unsigned char *buf1  = (unsigned char *) "1";
	unsigned char *buf1a = (unsigned char *) "2";
	unsigned char *buf2  = (unsigned char *) "bas";
	unsigned char *buf2a = (unsigned char *) "bas2";

	
	try
	{
				
		int nfields = 4;
		(message->ip_message).delete_all_templates();

		templatedataid = (message->ip_message).new_data_template( nfields, IPAP_SETID_AUCTION_TEMPLATE );
		(message->ip_message).add_field(templatedataid, 0, IPAP_FT_IDAUCTION);
		(message->ip_message).add_field(templatedataid, 0, IPAP_FT_STARTSECONDS);
		(message->ip_message).add_field(templatedataid, 0, IPAP_FT_ENDSECONDS);	
		(message->ip_message).add_field(templatedataid, 0, IPAP_FT_AUCTIONINGALGORITHMNAME);

		ipap_field field1 = (message->ip_message).get_field_definition( 0, IPAP_FT_IDAUCTION );
		ipap_value_field fvalue1 = field1.get_ipap_value_field( buf1, 1 );
		ipap_value_field fvalue1a = field1.get_ipap_value_field( buf1a, 1 );

		ipap_field field2 = (message->ip_message).get_field_definition( 0, IPAP_FT_STARTSECONDS );
		ipap_value_field fvalue2 = field2.get_ipap_value_field( starttime );

		ipap_field field3 = (message->ip_message).get_field_definition( 0, IPAP_FT_ENDSECONDS );
		ipap_value_field fvalue3 = field3.get_ipap_value_field( endtime );

		ipap_field field4 = (message->ip_message).get_field_definition( 0, IPAP_FT_AUCTIONINGALGORITHMNAME );
		ipap_value_field fvalue4 = field4.get_ipap_value_field( buf2, 3 );
		ipap_value_field fvalue4a = field4.get_ipap_value_field( buf2a, 4 );
		
		ipap_data_record data(templatedataid);
		data.insert_field(0, IPAP_FT_IDAUCTION, fvalue1);
		data.insert_field(0, IPAP_FT_STARTSECONDS, fvalue2);
		data.insert_field(0, IPAP_FT_ENDSECONDS, fvalue3);
		data.insert_field(0, IPAP_FT_AUCTIONINGALGORITHMNAME, fvalue4);
		(message->ip_message).include_data(templatedataid, data);

		ipap_data_record data2(templatedataid);
		data2.insert_field(0, IPAP_FT_IDAUCTION, fvalue1a);
		data2.insert_field(0, IPAP_FT_STARTSECONDS, fvalue2);
		data2.insert_field(0, IPAP_FT_ENDSECONDS, fvalue3);
		data2.insert_field(0, IPAP_FT_AUCTIONINGALGORITHMNAME, fvalue4a);
		(message->ip_message).include_data(templatedataid, data2);

	}
	catch(ipap_bad_argument &e)
	{
		cout << "Error: " << e.what() << endl;
	}

}

anslp_object *
Anslp_IpAp_Message_Test::createInstance1() const 
{
#ifdef DEBUG
    log->dlog(ch, "Starting createInstance1" );
#endif 		
	return new anslp_ipap_message();
}

anslp_object *
Anslp_IpAp_Message_Test::createInstance2() const
{
		
	buildMessage(mes);
	(mes->ip_message).output();
	
#ifdef DEBUG
    log->dlog(ch, "Starting createInstance2 - Offset: %d", 
					(mes->ip_message).get_offset()  );
#endif

	return new anslp_ipap_message(*mes);
}

void Anslp_IpAp_Message_Test::mutate1(anslp_object *o) const 
{
#ifdef DEBUG
    log->dlog(ch, "Starting mutate" );
#endif	

	anslp_ipap_message *ic = dynamic_cast<anslp_ipap_message *>(o);
	(ic->ip_message).output();
}


void Anslp_IpAp_Message_Test::setUp() {
	
	try 
	{
		log = Logger::getInstance();
		ch = log->createChannel("ANSLP_IPAP_MESSAGE_TEST");

#ifdef DEBUG
		log->dlog(ch, "Starting setUp" );
#endif 			
		
		mes = new anslp_ipap_message();
		mes2 = new anslp_ipap_message();

#ifdef DEBUG
		log->dlog(ch, "Ending setUp" );
#endif 			


	}
	catch(ipap_bad_argument &e)
	{
		cout << "Error: " << e.what() << endl;
	}
	catch(Error &e)
	{
		cout << "Error setting up the log file: " << endl;
	}
	
}

void Anslp_IpAp_Message_Test::testAddTemplate()
{

#ifdef DEBUG
		log->dlog(ch, "Starting testAddTemplate" );
#endif

	try
	{ 
	
		uint16_t templatedataid = 0;
		
		int nfields;
								
		// Verifies the field add method.
		nfields = 4; // Maximum number of fields.
		templatedataid = (mes->ip_message).new_data_template( nfields, IPAP_SETID_AUCTION_TEMPLATE );
		(mes->ip_message).add_field(templatedataid, 0, IPAP_FT_IDAUCTION);
		(mes->ip_message).add_field(templatedataid, 0, IPAP_FT_STARTSECONDS);
		(mes->ip_message).add_field(templatedataid, 0, IPAP_FT_ENDSECONDS);
		(mes->ip_message).add_field(templatedataid, 0, IPAP_FT_AUCTIONINGALGORITHMNAME);
		(mes->ip_message).delete_all_templates();
						
	}
	catch(ipap_bad_argument &e)
	{
		cout << "Error: " << e.what() << endl;
	}
}


void Anslp_IpAp_Message_Test::testExceptionAddTemplate()
{
#ifdef DEBUG
		log->dlog(ch, "Starting testExceptionAddTemplate" );
#endif	
	
	int nfields = 0;
	CPPUNIT_ASSERT_THROW( (mes->ip_message).new_data_template( nfields, IPAP_SETID_AUCTION_TEMPLATE ),ipap_bad_argument);
	CPPUNIT_ASSERT_THROW( (mes->ip_message).new_data_template( nfields, IPAP_SETID_BID_TEMPLATE ),ipap_bad_argument);
	CPPUNIT_ASSERT_THROW( (mes->ip_message).new_data_template( nfields, IPAP_SETID_ALLOCATION_TEMPLATE ),ipap_bad_argument);

	uint16_t templateAuctionid = 0;
	uint16_t templateBidid = 0;
	uint16_t templateAllocationid = 0;
	

	// Verifies that a data field cannot be added to a empty template
	CPPUNIT_ASSERT_THROW( (mes->ip_message).add_field(templateAuctionid, 0, IPAP_FT_STARTSECONDS), 
						  ipap_bad_argument);

	CPPUNIT_ASSERT_THROW( (mes->ip_message).add_field(templateBidid, 0, IPAP_FT_STARTSECONDS), 
						  ipap_bad_argument);

	CPPUNIT_ASSERT_THROW( (mes->ip_message).add_field(templateAllocationid, 0, IPAP_FT_STARTSECONDS), 
						  ipap_bad_argument);
			
	// Verifies that only add a valid field in the collection
	CPPUNIT_ASSERT_THROW( (mes->ip_message).add_field(templateAuctionid, 0, 3000),
						  ipap_bad_argument);
						  
	nfields = 3;
	templateAuctionid = (mes->ip_message).new_data_template( nfields, IPAP_SETID_AUCTION_TEMPLATE );
	// Verifies that only the maximum number of data fields can be inserted.
	(mes->ip_message).add_field(templateAuctionid, 0, IPAP_FT_STARTSECONDS);
	(mes->ip_message).add_field(templateAuctionid, 0, IPAP_FT_ENDSECONDS);
	(mes->ip_message).add_field(templateAuctionid, 0, IPAP_FT_STARTMILLISECONDS);
	CPPUNIT_ASSERT_THROW( (mes->ip_message).add_field(templateAuctionid, 0, IPAP_FT_ENDMILLISECONDS),
						  ipap_bad_argument);
	

	(mes->ip_message).delete_all_templates();

	ipap_fields_t a[3]; 
	a[0].eno = 0;
	a[0].ienum = IPAP_FT_STARTSECONDS;
	a[0].length = 8;

	a[1].eno = 0;
	a[1].ienum = IPAP_FT_ENDSECONDS;
	a[1].length = 8;

	a[2].eno = 0;
	a[2].ienum = IPAP_FT_UNITBUDGET;
	a[2].length = 8;

	uint16_t templid = 256;
	CPPUNIT_ASSERT_THROW( (mes->ip_message).make_template(a, 4, IPAP_SETID_ALLOCATION_TEMPLATE, templid), 
							ipap_bad_argument);

#ifdef DEBUG
		log->dlog(ch, "Ending testExceptionAddTemplate" );
#endif	

}


void Anslp_IpAp_Message_Test::testDataRecords()
{

#ifdef DEBUG
		log->dlog(ch, "Starting testDataRecords" );
#endif	
	
	uint16_t templatedataid = 0;
	uint64_t starttime = 100;
	uint64_t endtime = 200;
	unsigned char   *buf1  = (unsigned char *) "1";
	unsigned char   *buf1a  = (unsigned char *) "2";
	unsigned char   *buf1b  = (unsigned char *) "3";
	unsigned char   *buf2  = (unsigned char *) "bas";

	try
	{
		(mes->ip_message).delete_all_templates();
		
		int nfields = 4;
		templatedataid = (mes->ip_message).new_data_template( nfields, IPAP_SETID_AUCTION_TEMPLATE );
		(mes->ip_message).add_field(templatedataid, 0, IPAP_FT_IDAUCTION);
		(mes->ip_message).add_field(templatedataid, 0, IPAP_FT_STARTSECONDS);
		(mes->ip_message).add_field(templatedataid, 0, IPAP_FT_ENDSECONDS);	
		(mes->ip_message).add_field(templatedataid, 0, IPAP_FT_AUCTIONINGALGORITHMNAME);

		ipap_field field1 = (mes->ip_message).get_field_definition( 0, IPAP_FT_STARTSECONDS );
		ipap_value_field fvalue1 = field1.get_ipap_value_field( starttime );

		ipap_field field2 = (mes->ip_message).get_field_definition( 0, IPAP_FT_ENDSECONDS );
		ipap_value_field fvalue2 = field2.get_ipap_value_field( endtime );

		ipap_field field3 = (mes->ip_message).get_field_definition( 0, IPAP_FT_IDAUCTION );
		ipap_value_field fvalue3 = field3.get_ipap_value_field( buf1, 1 );
		ipap_value_field fvalue3a = field3.get_ipap_value_field( buf1a, 1 );
		ipap_value_field fvalue3b = field3.get_ipap_value_field( buf1b, 1 );
		
		ipap_field field4 = (mes->ip_message).get_field_definition( 0, IPAP_FT_AUCTIONINGALGORITHMNAME );
		ipap_value_field fvalue4 = field3.get_ipap_value_field( buf2, 3 );
		
		ipap_data_record data(templatedataid);
		data.insert_field(0, IPAP_FT_STARTSECONDS, fvalue1);
		data.insert_field(0, IPAP_FT_ENDSECONDS, fvalue2);
		data.insert_field(0, IPAP_FT_IDAUCTION, fvalue3);
		data.insert_field(0, IPAP_FT_AUCTIONINGALGORITHMNAME, fvalue4);
		(mes->ip_message).include_data(templatedataid, data);
		
		ipap_data_record data2(templatedataid);
		data2.insert_field(0, IPAP_FT_STARTSECONDS, fvalue1);
		data2.insert_field(0, IPAP_FT_ENDSECONDS, fvalue2);
		data2.insert_field(0, IPAP_FT_IDAUCTION, fvalue3);
		data2.insert_field(0, IPAP_FT_AUCTIONINGALGORITHMNAME, fvalue4);
		(mes->ip_message).include_data(templatedataid, data2);
		
		ipap_data_record data3(templatedataid);
		data3.insert_field(0, IPAP_FT_STARTSECONDS, fvalue1);
		data3.insert_field(0, IPAP_FT_ENDSECONDS, fvalue2);
		data3.insert_field(0, IPAP_FT_IDAUCTION, fvalue3b);
		data3.insert_field(0, IPAP_FT_AUCTIONINGALGORITHMNAME, fvalue4);
		(mes->ip_message).include_data(templatedataid, data3);
		
		
		CPPUNIT_ASSERT( data2 == data );
		CPPUNIT_ASSERT( data2 != data3);
		
		ipap_data_record data4 = data3;
		CPPUNIT_ASSERT( data3 == data4);
		
		int num_fields_record = data3.get_num_fields();
		CPPUNIT_ASSERT( num_fields_record == 4);

		ipap_value_field fvalue1b = data3.get_field(0, IPAP_FT_STARTSECONDS);
		CPPUNIT_ASSERT( fvalue1 == fvalue1b );
		
		(mes->ip_message).delete_all_templates();
		
	}
	catch(ipap_bad_argument &e)
	{
		cout << "Error: " << e.what() << endl;
	}


#ifdef DEBUG
		log->dlog(ch, "Ending testDataRecords" );
#endif	


}


void Anslp_IpAp_Message_Test::testExportImport()
{
#ifdef DEBUG
	log->dlog(ch, "Starting testExportImport" );
#endif
	
	unsigned char * message;
	int offset;

	buildMessage(mes);
	(mes->ip_message).output();
	message = (mes->ip_message).get_message();
	offset = (mes->ip_message).get_offset();

#ifdef DEBUG
	log->dlog(ch, "After it was built and output the message" );
#endif
				
	anslp_ipap_message msgb (message, offset, true);		

#ifdef DEBUG
	log->dlog(ch, "After create a new message from a uint8_t*" );
#endif

	CPPUNIT_ASSERT( (msgb.ip_message).operator==(mes->ip_message) );

#ifdef DEBUG
	log->dlog(ch, "messages are equal" );
#endif

	saveDelete(mes2);
	mes2 = new anslp_ipap_message(*mes);
	CPPUNIT_ASSERT( (mes->ip_message).operator==(mes->ip_message) );

#ifdef DEBUG
	log->dlog(ch, "Ending testExportImport - Offset: %d", 
					(mes->ip_message).get_offset()  );
#endif
				 
}

void Anslp_IpAp_Message_Test::tearDown() 
{
#ifdef DEBUG
	log->dlog(ch, "Starting teardown" );
#endif	
	saveDelete(mes);
	saveDelete(mes2);

#ifdef DEBUG
	log->dlog(ch, "Ending teardown" );
#endif		
}

void Anslp_IpAp_Message_Test::testBasics() 
{

}
