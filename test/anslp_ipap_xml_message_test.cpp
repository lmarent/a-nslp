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
#include "anslp_ipap_xml_message.h"
#include "generic_object_test.h"

using namespace anslp::msg;


class anslp_ipap_xml_message_Test : public CppUnit::TestCase {

	CPPUNIT_TEST_SUITE( anslp_ipap_xml_message_Test );
	CPPUNIT_TEST( testExport );
	CPPUNIT_TEST_SUITE_END();

  public:
	void setUp();
	void tearDown();
	void buildRequestMessage(anslp_ipap_message *message);
	void buildAuctionMessage(anslp_ipap_message *message);
	void buildBidMessage(anslp_ipap_message *message);
	void buildAllocationMessage(anslp_ipap_message *message);
	void testExport();

  private:  
    anslp_ipap_message *request;
    anslp_ipap_message *auction;
    anslp_ipap_message *bid;
    anslp_ipap_message *allocation;

	Logger *log; //!< link to global logger object
	int ch;      //!< logging channel number used by objects of this class

};

CPPUNIT_TEST_SUITE_REGISTRATION( anslp_ipap_xml_message_Test );


void
anslp_ipap_xml_message_Test::buildBidMessage(anslp_ipap_message *message) 
{

#ifdef DEBUG
		log->dlog(ch, "Starting BuildBidMessage" );
#endif

	uint16_t templatedataid = 0;
	uint16_t templateoptiondataid = 0;
	uint64_t starttime = 100;
	uint64_t endtime = 200;
	time_t now = time(NULL);
	
	uint32_t seqNo = 250;
	uint32_t ackseqNo = 0;

	double unitbudget = 0.15;
	double unitvalue = 0.13;
	float quantity = 0.7;  // Megabits / second.

		
	unsigned char *buf1  = (unsigned char *) "B001"; // Bid Id
	unsigned char *buf2  = (unsigned char *) "R001"; // Data Record 1
	unsigned char *buf2a  = (unsigned char *) "R002"; // Data Record 2
	unsigned char *buf2b  = (unsigned char *) "R003"; // Data Record 3
	unsigned char *buf3  = (unsigned char *) "A001"; // Nbr Auction 
	unsigned char *buf3a  = (unsigned char *) "A002"; // Nbr Auction 

	
	int nfields = 0;
	try
	{
				
		(message->ip_message).delete_all_templates();
		(message->ip_message).set_seqno(seqNo);
		(message->ip_message).set_ackseqno(ackseqNo);

		nfields = 8;
		templatedataid = (message->ip_message).new_data_template( nfields, IPAP_SETID_BID_TEMPLATE );
		(message->ip_message).add_field(templatedataid, 0, IPAP_FT_IDAUCTION);
		(message->ip_message).add_field(templatedataid, 0, IPAP_FT_IDBID);
		(message->ip_message).add_field(templatedataid, 0, IPAP_FT_IDRECORD);
		(message->ip_message).add_field(templatedataid, 0, IPAP_FT_QUANTITY);
		(message->ip_message).add_field(templatedataid, 0, IPAP_FT_UNITBUDGET);	
		(message->ip_message).add_field(templatedataid, 0, IPAP_FT_UNITVALUE);
		(message->ip_message).add_field(templatedataid, 0, IPAP_FT_STARTSECONDS);
		(message->ip_message).add_field(templatedataid, 0, IPAP_FT_ENDSECONDS); 
		
		/// insert Values for data record 1.
		ipap_field field1 = (message->ip_message).get_field_definition( 0, IPAP_FT_IDBID );
		ipap_value_field fvalue1 = field1.get_ipap_value_field( buf1, 4);
		
		ipap_field field2 = (message->ip_message).get_field_definition( 0, IPAP_FT_IDRECORD );
		ipap_value_field fvalue2 = field2.get_ipap_value_field( buf2, 4 );

		ipap_field field3 = (message->ip_message).get_field_definition( 0, IPAP_FT_IDAUCTION );
		ipap_value_field fvalue3 = field3.get_ipap_value_field( buf2, 4 );

		ipap_field field6 = (message->ip_message).get_field_definition( 0, IPAP_FT_QUANTITY );
		ipap_value_field fvalue6 = field6.get_ipap_value_field( quantity );

		ipap_field field4 = (message->ip_message).get_field_definition( 0, IPAP_FT_UNITBUDGET );
		ipap_value_field fvalue4 = field4.get_ipap_value_field( unitbudget );

		ipap_field field5 = (message->ip_message).get_field_definition( 0, IPAP_FT_UNITVALUE );
		ipap_value_field fvalue5 = field5.get_ipap_value_field( unitvalue );

		ipap_field fieldStart = (message->ip_message).get_field_definition( 0, IPAP_FT_STARTSECONDS );
		ipap_value_field fvalueStart = fieldStart.get_ipap_value_field( starttime );

		ipap_field fieldEnd = (message->ip_message).get_field_definition( 0, IPAP_FT_ENDSECONDS );
		ipap_value_field fvalueEnd = fieldEnd.get_ipap_value_field( endtime );

	
		ipap_data_record data(templatedataid);
		data.insert_field(0, IPAP_FT_IDBID, fvalue1);
		data.insert_field(0, IPAP_FT_IDRECORD, fvalue2);
		data.insert_field(0, IPAP_FT_IDAUCTION, fvalue3);
		data.insert_field(0, IPAP_FT_UNITBUDGET, fvalue4);
		data.insert_field(0, IPAP_FT_UNITVALUE, fvalue5);
		data.insert_field(0, IPAP_FT_QUANTITY, fvalue6);
		data.insert_field(0, IPAP_FT_STARTSECONDS, fvalueStart);		
		data.insert_field(0, IPAP_FT_ENDSECONDS, fvalueEnd);

#ifdef DEBUG
		log->dlog(ch, "BuildBidMessage - Finish data record 1 %s", data.to_string().c_str() );
#endif
		
		(message->ip_message).include_data(templatedataid, data);


		/// insert Values for data record 2.
		ipap_field field1a = (message->ip_message).get_field_definition( 0, IPAP_FT_IDBID );
		ipap_value_field fvalue1a = field1a.get_ipap_value_field( buf1, 4);

		ipap_field field2a = (message->ip_message).get_field_definition( 0, IPAP_FT_IDRECORD );
		ipap_value_field fvalue2a = field2a.get_ipap_value_field( buf2a, 4 );

		ipap_field field3a = (message->ip_message).get_field_definition( 0, IPAP_FT_UNITBUDGET );
		ipap_value_field fvalue3a = field3a.get_ipap_value_field(  unitbudget );

		ipap_field field4a = (message->ip_message).get_field_definition( 0, IPAP_FT_UNITVALUE );
		ipap_value_field fvalue4a = field4a.get_ipap_value_field(  unitvalue );

		ipap_field field5a = (message->ip_message).get_field_definition( 0, IPAP_FT_QUANTITY );
		ipap_value_field fvalue5a = field5a.get_ipap_value_field(  quantity );

		ipap_field field6a = (message->ip_message).get_field_definition( 0, IPAP_FT_IDAUCTION );
		ipap_value_field fvalue6a = field6a.get_ipap_value_field( buf2a, 4 );
		
		ipap_data_record data2(templatedataid);
		data2.insert_field(0, IPAP_FT_IDBID, fvalue1a);
		data2.insert_field(0, IPAP_FT_IDRECORD, fvalue2a);
		data2.insert_field(0, IPAP_FT_UNITBUDGET, fvalue3a);
		data2.insert_field(0, IPAP_FT_UNITVALUE, fvalue4a);
		data2.insert_field(0, IPAP_FT_QUANTITY, fvalue5a);
		data2.insert_field(0, IPAP_FT_IDAUCTION, fvalue6a);
		data2.insert_field(0, IPAP_FT_STARTSECONDS, fvalueStart);		
		data2.insert_field(0, IPAP_FT_ENDSECONDS, fvalueEnd);
		
		(message->ip_message).include_data(templatedataid, data2);

#ifdef DEBUG
		log->dlog(ch, "BuildBidMessage - Finish data record 2" );
#endif
		/// insert Values for data record 3.
		ipap_field field1b = (message->ip_message).get_field_definition( 0, IPAP_FT_IDBID );
		ipap_value_field fvalue1b = field1b.get_ipap_value_field( buf1, 4);

		ipap_field field2b = (message->ip_message).get_field_definition( 0, IPAP_FT_IDRECORD );
		ipap_value_field fvalue2b = field2b.get_ipap_value_field( buf2b, 4 );

		ipap_field field3b = (message->ip_message).get_field_definition( 0, IPAP_FT_UNITBUDGET );
		ipap_value_field fvalue3b = field3b.get_ipap_value_field(  unitbudget );

		ipap_field field4b = (message->ip_message).get_field_definition( 0, IPAP_FT_UNITVALUE );
		ipap_value_field fvalue4b = field4b.get_ipap_value_field(  unitvalue );

		ipap_field field5b = (message->ip_message).get_field_definition( 0, IPAP_FT_QUANTITY );
		ipap_value_field fvalue5b = field5b.get_ipap_value_field(  quantity );

		ipap_field field6b = (message->ip_message).get_field_definition( 0, IPAP_FT_IDAUCTION );
		ipap_value_field fvalue6b = field6b.get_ipap_value_field( buf2b, 4 );
		
		ipap_data_record data3(templatedataid);
		data3.insert_field(0, IPAP_FT_IDBID, fvalue1b);
		data3.insert_field(0, IPAP_FT_IDRECORD, fvalue2b);
		data3.insert_field(0, IPAP_FT_UNITBUDGET, fvalue3b);
		data3.insert_field(0, IPAP_FT_UNITVALUE, fvalue4b);
		data3.insert_field(0, IPAP_FT_QUANTITY, fvalue5b);
		data3.insert_field(0, IPAP_FT_IDAUCTION, fvalue6b);
		data3.insert_field(0, IPAP_FT_STARTSECONDS, fvalueStart);		
		data3.insert_field(0, IPAP_FT_ENDSECONDS, fvalueEnd);


		(message->ip_message).include_data(templatedataid, data3);

#ifdef DEBUG
		log->dlog(ch, "BuildBidMessage - Finish data record 3" );
#endif

		nfields = 3;
		templateoptiondataid = (message->ip_message).new_data_template( nfields, IPAP_OPTNS_BID_TEMPLATE );
		(message->ip_message).add_field(templateoptiondataid, 0, IPAP_FT_IDBID);
		(message->ip_message).add_field(templateoptiondataid, 0, IPAP_FT_IDAUCTION);
		(message->ip_message).add_field(templateoptiondataid, 0, IPAP_FT_IDRECORD); 


		/// insert Values for option data record 4.
		ipap_field field1c = (message->ip_message).get_field_definition( 0, IPAP_FT_IDBID );
		ipap_value_field fvalue1c = field1c.get_ipap_value_field( buf1, 4);

		ipap_field field3c = (message->ip_message).get_field_definition( 0, IPAP_FT_IDAUCTION );
		ipap_value_field fvalue3c = field3c.get_ipap_value_field( buf3, 4 );

		ipap_field field2c = (message->ip_message).get_field_definition( 0, IPAP_FT_IDRECORD );
		ipap_value_field fvalue2c = field2c.get_ipap_value_field( buf2, 4 );
		
		ipap_data_record data4(templateoptiondataid);
		data4.insert_field(0, IPAP_FT_IDBID, fvalue1c);
		data4.insert_field(0, IPAP_FT_IDRECORD, fvalue2c);
		data4.insert_field(0, IPAP_FT_IDAUCTION, fvalue3c);

		(message->ip_message).include_data(templateoptiondataid, data4);

#ifdef DEBUG
		log->dlog(ch, "BuildBidMessage - Finish data record 4" );
#endif
		/// insert Values for option data record 2.
		ipap_field field1d = (message->ip_message).get_field_definition( 0, IPAP_FT_IDBID );
		ipap_value_field fvalue1d = field1d.get_ipap_value_field( buf1, 4);

		ipap_field field3d = (message->ip_message).get_field_definition( 0, IPAP_FT_IDAUCTION );
		ipap_value_field fvalue3d = field3d.get_ipap_value_field( buf3a, 4 );

		ipap_field field2d = (message->ip_message).get_field_definition( 0, IPAP_FT_IDRECORD );
		ipap_value_field fvalue2d = field2d.get_ipap_value_field( buf2b, 4 );
		
		ipap_data_record data5(templateoptiondataid);
		data5.insert_field(0, IPAP_FT_IDBID, fvalue1d);
		data5.insert_field(0, IPAP_FT_IDRECORD, fvalue2d);
		data5.insert_field(0, IPAP_FT_IDAUCTION, fvalue3d);
		(message->ip_message).include_data(templateoptiondataid, data5);

#ifdef DEBUG
		log->dlog(ch, "BuildBidMessage - Finish data record 5" );
#endif


	} catch(ipap_bad_argument &e) {
#ifdef DEBUG
		log->dlog(ch, "Error: %s", e.what() );
		throw e;
#endif		
	}

}

void 
anslp_ipap_xml_message_Test::buildRequestMessage(anslp_ipap_message *message)
{

	uint16_t optionTemplateId;
	time_t now = time(NULL);
	uint32_t seqNo = 250;
	uint32_t ackseqNo = 251;

	(message->ip_message).set_seqno(seqNo);
	(message->ip_message).set_ackseqno(ackseqNo);
		
	// Add the option bid template
	int nfields = 6;
	optionTemplateId = (message->ip_message).new_data_template( nfields, IPAP_OPTNS_AUCTION_TEMPLATE );

	// put the AUCTIONID.
	(message->ip_message).add_field(optionTemplateId, 0, IPAP_FT_IDAUCTION);
	// put the RECORDID.
	(message->ip_message).add_field(optionTemplateId, 0, IPAP_FT_IDRECORD);
	// put the ResourceID
	(message->ip_message).add_field(optionTemplateId, 0, IPAP_FT_IDRESOURCE);
	// put the starttime
	(message->ip_message).add_field(optionTemplateId, 0, IPAP_FT_STARTSECONDS);
	// put the endtime
	(message->ip_message).add_field(optionTemplateId, 0, IPAP_FT_ENDSECONDS);
	// put the interval
	(message->ip_message).add_field(optionTemplateId, 0, IPAP_FT_INTERVALSECONDS);

	cout << "asdasd 2" << endl;
	
	ipap_data_record dataOption(optionTemplateId);

	// Add the Auction Id
	string auctionId = "";
	ipap_field idauctionIdF = (message->ip_message).get_field_definition( 0, IPAP_FT_IDAUCTION );
	ipap_value_field fvalue1 = idauctionIdF.get_ipap_value_field( 
									strdup(auctionId.c_str()), auctionId.size() );
	dataOption.insert_field(0, IPAP_FT_IDAUCTION, fvalue1);


	// Add the Record Id
	string recordId = "setRecord.record1";
	ipap_field idRecordIdF = (message->ip_message).get_field_definition( 0, IPAP_FT_IDRECORD );
	ipap_value_field fvalue2 = idRecordIdF.get_ipap_value_field( 
									strdup(recordId.c_str()), recordId.size() );
	dataOption.insert_field(0, IPAP_FT_IDRECORD, fvalue2);

	// Add the Resource Id
	string resourceId = "Resource1";
	ipap_field resourceIdF = (message->ip_message).get_field_definition( 0, IPAP_FT_IDRESOURCE );
	ipap_value_field fvalue3 = resourceIdF.get_ipap_value_field( 
									strdup(resourceId.c_str()), resourceId.size() );
	dataOption.insert_field(0, IPAP_FT_IDRESOURCE, fvalue3);
	
	// Add the start datetime
	assert (sizeof(uint64_t) >= sizeof(time_t));
	time_t time = now;
	uint64_t timeUint64 = *reinterpret_cast<uint64_t*>(&time);
	ipap_field idStartF = (message->ip_message).get_field_definition( 0, IPAP_FT_STARTSECONDS );
	ipap_value_field fvalue4 = idStartF.get_ipap_value_field( timeUint64 );
	dataOption.insert_field(0, IPAP_FT_STARTSECONDS, fvalue4);
		
	// Add the endtime
	ipap_field idStopF = (message->ip_message).get_field_definition( 0, IPAP_FT_ENDSECONDS );
	time = now + 100;	
	timeUint64 = *reinterpret_cast<uint64_t*>(&time);
	ipap_value_field fvalue5 = idStopF.get_ipap_value_field( timeUint64 );
	dataOption.insert_field(0, IPAP_FT_ENDSECONDS, fvalue5);
	
	// Add the interval.
	assert (sizeof(uint64_t) >= sizeof(unsigned long));
	uint64_t uinter = 100;
	ipap_field idIntervalF = (message->ip_message).get_field_definition( 0, IPAP_FT_INTERVALSECONDS );
	ipap_value_field fvalue6 = idIntervalF.get_ipap_value_field( uinter );
	dataOption.insert_field(0, IPAP_FT_INTERVALSECONDS, fvalue6);
		
	(message->ip_message).include_data(optionTemplateId, dataOption);
		
}

void
anslp_ipap_xml_message_Test::buildAuctionMessage(anslp_ipap_message *message) 
{

	uint16_t templatedataid = 0;
	uint64_t starttime = 100;
	uint64_t endtime = 200;
	unsigned char *buf1  = (unsigned char *) "1";
	unsigned char *buf1a = (unsigned char *) "2";
	unsigned char *buf2  = (unsigned char *) "bas";
	unsigned char *buf2a = (unsigned char *) "bas2";
	uint32_t seqNo = 250;
	uint32_t ackseqNo = 251;

	int nfields = 4;
	try
	{
				
		(message->ip_message).delete_all_templates();
		(message->ip_message).set_seqno(seqNo);
		(message->ip_message).set_ackseqno(ackseqNo);
		
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

void
anslp_ipap_xml_message_Test::buildAllocationMessage(anslp_ipap_message *message) 
{

	uint16_t templatedataid = 0;
	uint64_t starttime = 100;
	uint64_t endtime = 200;
	unsigned char *buf1  = (unsigned char *) "1";
	unsigned char *buf1a = (unsigned char *) "2";
	unsigned char *buf2  = (unsigned char *) "bas";
	unsigned char *buf2a = (unsigned char *) "bas2";
	uint32_t seqNo = 250;
	uint32_t ackseqNo = 251;

	int nfields = 4;
	try
	{
				
		(message->ip_message).delete_all_templates();
		(message->ip_message).set_seqno(seqNo);
		(message->ip_message).set_ackseqno(ackseqNo);

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


void 
anslp_ipap_xml_message_Test::setUp() {
	
	try 
	{
		log = Logger::getInstance();
		ch = log->createChannel("ANSLP_IPAP_XML_MESSAGE_TEST");

#ifdef DEBUG
		log->dlog(ch, "Starting setUp" );
#endif 			

		request = new anslp_ipap_message();
		auction = new anslp_ipap_message();
		bid = new anslp_ipap_message();
		allocation = new anslp_ipap_message();
		
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

void 
anslp_ipap_xml_message_Test::testExport()
{
#ifdef DEBUG
	log->dlog(ch, "Starting testExport" );
#endif
	try
	{ 
		buildBidMessage(bid);
		(bid->ip_message).output();

#ifdef DEBUG
		log->dlog(ch, "End Building message" );
#endif

		anslp_ipap_xml_message mes;
	
		string xmlMessage = mes.get_message(*bid);	

#ifdef DEBUG
		log->dlog(ch, "message generated: %s", xmlMessage.c_str() );
#endif		
		
		anslp_ipap_message *other_message = mes.from_message(xmlMessage);
	
#ifdef DEBUG
		log->dlog(ch, "After reading message %d",( (other_message->ip_message).get_template_list()).size() );
#endif
	
		anslp_ipap_xml_message mes2;
		string xmlMessage2= mes2.get_message(*other_message);
				
		CPPUNIT_ASSERT( xmlMessage == xmlMessage2 );
	
		saveDelete(other_message);

#ifdef DEBUG
	log->dlog(ch, "Read and write ok" );
#endif
    
		buildRequestMessage(request);
		anslp_ipap_xml_message mes3;

#ifdef DEBUG
	log->dlog(ch, "it is going to get the message" );
#endif
    
		string xmlMessage3 = mes3.get_message(*request);
		cout << xmlMessage3 << endl;
    
    
#ifdef DEBUG
		log->dlog(ch, "Ending testExport" );
#endif	
	} catch (Error &e) {
		cout << e.getError() << endl;
		throw e;
	}	
	
}

void 
anslp_ipap_xml_message_Test::tearDown() 
{
#ifdef DEBUG
	log->dlog(ch, "Starting teardown" );
#endif	
	saveDelete(auction);
	saveDelete(bid);
	saveDelete(allocation);
	saveDelete(request);

#ifdef DEBUG
	log->dlog(ch, "Ending teardown" );
#endif		
}


