/*
 * Test the ipap_message_test class.
 *
 * $Id: ipap_message_test.cpp 2015-08-28 17:53:00 amarentes $
 * $HeadURL: https://./test/ipap_message_test.cpp $
 */


#include "anslp_ipap_xml_message_test.h"


using namespace anslp;


anslp_ipap_xml_message_test::anslp_ipap_xml_message_test()
{

	log = Logger::getInstance();
	ch = log->createChannel("anslp_ipap_xml_message_test");
	
}

anslp_ipap_xml_message_test::~anslp_ipap_xml_message_test()
{

}

void
anslp_ipap_xml_message_test::buildBidMessage(msg::anslp_ipap_message *message) 
{

#ifdef DEBUG
		log->dlog(ch, "Starting BuildBidMessage" );
#endif

	uint16_t templatedataid = 0;
	uint16_t templateoptiondataid = 0;
	uint32_t starttime = 100;
	uint32_t endtime = 200;

	double maxvaluation = 0.15;
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

		nfields = 5;
		templatedataid = (message->ip_message).new_data_template( nfields, IPAP_SETID_BID_TEMPLATE );
		(message->ip_message).add_field(templatedataid, 0, IPAP_FT_IDBID, 128);
		(message->ip_message).add_field(templatedataid, 0, IPAP_FT_IDRECORD, 128);
		(message->ip_message).add_field(templatedataid, 0, IPAP_FT_MAXUNITVALUATION, 8);
		(message->ip_message).add_field(templatedataid, 0, IPAP_FT_UNITVALUE, 8);	
		(message->ip_message).add_field(templatedataid, 0, IPAP_FT_QUANTITY, 4);
		
		/// insert Values for data record 1.
		ipap_field field1 = (message->ip_message).get_field_definition( 0, IPAP_FT_IDBID );
		ipap_value_field fvalue1 = field1.get_ipap_value_field( buf1, 4);
		
		ipap_field field2 = (message->ip_message).get_field_definition( 0, IPAP_FT_IDRECORD );
		ipap_value_field fvalue2 = field2.get_ipap_value_field( buf2, 4 );

		ipap_field field3 = (message->ip_message).get_field_definition( 0, IPAP_FT_MAXUNITVALUATION );
		ipap_value_field fvalue3 = field3.get_ipap_value_field( maxvaluation );

		ipap_field field4 = (message->ip_message).get_field_definition( 0, IPAP_FT_UNITVALUE );
		ipap_value_field fvalue4 = field4.get_ipap_value_field( unitvalue );

		ipap_field field5 = (message->ip_message).get_field_definition( 0, IPAP_FT_QUANTITY );
		ipap_value_field fvalue5 = field5.get_ipap_value_field( quantity );
	
		ipap_data_record data(templatedataid);
		data.insert_field(0, IPAP_FT_IDBID, fvalue1);
		data.insert_field(0, IPAP_FT_IDRECORD, fvalue2);
		data.insert_field(0, IPAP_FT_MAXUNITVALUATION, fvalue3);
		data.insert_field(0, IPAP_FT_UNITVALUE, fvalue4);
		data.insert_field(0, IPAP_FT_QUANTITY, fvalue5);

#ifdef DEBUG
		log->dlog(ch, "BuildBidMessage - Finish data record 1 %s", data.to_string().c_str() );
#endif
		
		(message->ip_message).include_data(templatedataid, data);


		/// insert Values for data record 2.
		ipap_field field1a = (message->ip_message).get_field_definition( 0, IPAP_FT_IDBID );
		ipap_value_field fvalue1a = field1a.get_ipap_value_field( buf1, 4);

		ipap_field field2a = (message->ip_message).get_field_definition( 0, IPAP_FT_IDRECORD );
		ipap_value_field fvalue2a = field2a.get_ipap_value_field( buf2a, 4 );

		ipap_field field3a = (message->ip_message).get_field_definition( 0, IPAP_FT_MAXUNITVALUATION );
		ipap_value_field fvalue3a = field3a.get_ipap_value_field(  maxvaluation );

		ipap_field field4a = (message->ip_message).get_field_definition( 0, IPAP_FT_UNITVALUE );
		ipap_value_field fvalue4a = field4a.get_ipap_value_field(  unitvalue );

		ipap_field field5a = (message->ip_message).get_field_definition( 0, IPAP_FT_QUANTITY );
		ipap_value_field fvalue5a = field5a.get_ipap_value_field(  quantity );
		
		ipap_data_record data2(templatedataid);
		data2.insert_field(0, IPAP_FT_IDBID, fvalue1a);
		data2.insert_field(0, IPAP_FT_IDRECORD, fvalue2a);
		data2.insert_field(0, IPAP_FT_MAXUNITVALUATION, fvalue3a);
		data2.insert_field(0, IPAP_FT_UNITVALUE, fvalue4a);
		data2.insert_field(0, IPAP_FT_QUANTITY, fvalue5a);
		(message->ip_message).include_data(templatedataid, data2);

#ifdef DEBUG
		log->dlog(ch, "BuildBidMessage - Finish data record 2" );
#endif
		/// insert Values for data record 3.
		ipap_field field1b = (message->ip_message).get_field_definition( 0, IPAP_FT_IDBID );
		ipap_value_field fvalue1b = field1b.get_ipap_value_field( buf1, 4);

		ipap_field field2b = (message->ip_message).get_field_definition( 0, IPAP_FT_IDRECORD );
		ipap_value_field fvalue2b = field2b.get_ipap_value_field( buf2b, 4 );

		ipap_field field3b = (message->ip_message).get_field_definition( 0, IPAP_FT_MAXUNITVALUATION );
		ipap_value_field fvalue3b = field3b.get_ipap_value_field(  maxvaluation );

		ipap_field field4b = (message->ip_message).get_field_definition( 0, IPAP_FT_UNITVALUE );
		ipap_value_field fvalue4b = field4b.get_ipap_value_field(  unitvalue );

		ipap_field field5b = (message->ip_message).get_field_definition( 0, IPAP_FT_QUANTITY );
		ipap_value_field fvalue5b = field5b.get_ipap_value_field(  quantity );
		
		ipap_data_record data3(templatedataid);
		data3.insert_field(0, IPAP_FT_IDBID, fvalue1b);
		data3.insert_field(0, IPAP_FT_IDRECORD, fvalue2b);
		data3.insert_field(0, IPAP_FT_MAXUNITVALUATION, fvalue3b);
		data3.insert_field(0, IPAP_FT_UNITVALUE, fvalue4b);
		data3.insert_field(0, IPAP_FT_QUANTITY, fvalue5b);
		(message->ip_message).include_data(templatedataid, data3);

#ifdef DEBUG
		log->dlog(ch, "BuildBidMessage - Finish data record 3" );
#endif

		nfields = 5;
		templateoptiondataid = (message->ip_message).new_data_template( nfields, IPAP_OPTNS_BID_TEMPLATE );
		(message->ip_message).add_field(templateoptiondataid, 0, IPAP_FT_IDBID, 128);
		(message->ip_message).add_field(templateoptiondataid, 0, IPAP_FT_IDAUCTION, 128);
		(message->ip_message).add_field(templateoptiondataid, 0, IPAP_FT_IDRECORD, 128);		
		(message->ip_message).add_field(templateoptiondataid, 0, IPAP_FT_STARTSECONDS, 4);
		(message->ip_message).add_field(templateoptiondataid, 0, IPAP_FT_ENDSECONDS, 4);


		/// insert Values for option data record 4.
		ipap_field field1c = (message->ip_message).get_field_definition( 0, IPAP_FT_IDBID );
		ipap_value_field fvalue1c = field1c.get_ipap_value_field( buf1, 4);

		ipap_field field3c = (message->ip_message).get_field_definition( 0, IPAP_FT_IDAUCTION );
		ipap_value_field fvalue3c = field3c.get_ipap_value_field( buf3, 4 );

		ipap_field field2c = (message->ip_message).get_field_definition( 0, IPAP_FT_IDRECORD );
		ipap_value_field fvalue2c = field2c.get_ipap_value_field( buf2, 4 );

		ipap_field field4c = (message->ip_message).get_field_definition( 0, IPAP_FT_STARTSECONDS );
		ipap_value_field fvalue4c = field4c.get_ipap_value_field( starttime );

		ipap_field field5c = (message->ip_message).get_field_definition( 0, IPAP_FT_ENDSECONDS );
		ipap_value_field fvalue5c = field5c.get_ipap_value_field( endtime );
		
		ipap_data_record data4(templateoptiondataid);
		data4.insert_field(0, IPAP_FT_IDBID, fvalue1c);
		data4.insert_field(0, IPAP_FT_IDRECORD, fvalue2c);
		data4.insert_field(0, IPAP_FT_IDAUCTION, fvalue3c);
		data4.insert_field(0, IPAP_FT_STARTSECONDS, fvalue4c);
		data4.insert_field(0, IPAP_FT_ENDSECONDS, fvalue5c);
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

		ipap_field field4d = (message->ip_message).get_field_definition( 0, IPAP_FT_STARTSECONDS );
		ipap_value_field fvalue4d = field4d.get_ipap_value_field( starttime );

		ipap_field field5d = (message->ip_message).get_field_definition( 0, IPAP_FT_ENDSECONDS );
		ipap_value_field fvalue5d = field5d.get_ipap_value_field( endtime );
		
		ipap_data_record data5(templateoptiondataid);
		data5.insert_field(0, IPAP_FT_IDBID, fvalue1d);
		data5.insert_field(0, IPAP_FT_IDRECORD, fvalue2d);
		data5.insert_field(0, IPAP_FT_IDAUCTION, fvalue3d);
		data5.insert_field(0, IPAP_FT_STARTSECONDS, fvalue4d);
		data5.insert_field(0, IPAP_FT_ENDSECONDS, fvalue5d);
		(message->ip_message).include_data(templateoptiondataid, data5);

#ifdef DEBUG
		log->dlog(ch, "BuildBidMessage - Finish data record 5" );
#endif


	} catch(ipap_bad_argument &e) {
		cout << "Error: " << e.what() << endl;
	}

}


void
anslp_ipap_xml_message_test::buildAuctionMessage(msg::anslp_ipap_message *message) 
{

	uint16_t templatedataid = 0;
	uint32_t starttime = 100;
	uint32_t endtime = 200;
	unsigned char *buf1  = (unsigned char *) "1";
	unsigned char *buf1a = (unsigned char *) "2";
	unsigned char *buf2  = (unsigned char *) "bas";
	unsigned char *buf2a = (unsigned char *) "bas2";

	int nfields = 4;
	try
	{
				
		(message->ip_message).delete_all_templates();

		templatedataid = (message->ip_message).new_data_template( nfields, IPAP_SETID_AUCTION_TEMPLATE );
		(message->ip_message).add_field(templatedataid, 0, IPAP_FT_IDAUCTION, 65535);
		(message->ip_message).add_field(templatedataid, 0, IPAP_FT_STARTSECONDS, 4);
		(message->ip_message).add_field(templatedataid, 0, IPAP_FT_ENDSECONDS, 4);	
		(message->ip_message).add_field(templatedataid, 0, IPAP_FT_AUCTIONINGALGORITHMNAME, 65535);

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
anslp_ipap_xml_message_test::buildAllocationMessage(msg::anslp_ipap_message *message) 
{

	uint16_t templatedataid = 0;
	uint32_t starttime = 100;
	uint32_t endtime = 200;
	unsigned char *buf1  = (unsigned char *) "1";
	unsigned char *buf1a = (unsigned char *) "2";
	unsigned char *buf2  = (unsigned char *) "bas";
	unsigned char *buf2a = (unsigned char *) "bas2";

	int nfields = 4;
	try
	{
				
		(message->ip_message).delete_all_templates();

		templatedataid = (message->ip_message).new_data_template( nfields, IPAP_SETID_AUCTION_TEMPLATE );
		(message->ip_message).add_field(templatedataid, 0, IPAP_FT_IDAUCTION, 65535);
		(message->ip_message).add_field(templatedataid, 0, IPAP_FT_STARTSECONDS, 4);
		(message->ip_message).add_field(templatedataid, 0, IPAP_FT_ENDSECONDS, 4);	
		(message->ip_message).add_field(templatedataid, 0, IPAP_FT_AUCTIONINGALGORITHMNAME, 65535);

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
anslp_ipap_xml_message_test::setUp() {
	
	try 
	{
#ifdef DEBUG
		log->dlog(ch, "Starting setUp" );
#endif 			

		auction = new msg::anslp_ipap_message();
		bid = new msg::anslp_ipap_message();
		allocation = new msg::anslp_ipap_message();
		
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
anslp_ipap_xml_message_test::testExport()
{
#ifdef DEBUG
	log->dlog(ch, "Starting testExport" );
#endif
	
	buildBidMessage(bid);
	(bid->ip_message).output();

#ifdef DEBUG
	log->dlog(ch, "End Building message" );
#endif

	msg::anslp_ipap_xml_message mes(*bid);
	
	string xmlMessage = mes.get_message(msg::IPAP_BID);
	cout << xmlMessage << endl;
	
	msg::anslp_ipap_message *other_message = mes.from_message(xmlMessage, msg::IPAP_BID);
	
	msg::anslp_ipap_xml_message mes2(*other_message);
	string xmlMessage2= mes2.get_message(msg::IPAP_BID);
	cout << xmlMessage2 << endl;

	delete(other_message);

#ifdef DEBUG
	log->dlog(ch, "Ending testExport" );
#endif	
	
}

void 
anslp_ipap_xml_message_test::tearDown() 
{
#ifdef DEBUG
	log->dlog(ch, "Starting teardown" );
#endif	
	saveDelete(auction);
	saveDelete(bid);
	saveDelete(allocation);

#ifdef DEBUG
	log->dlog(ch, "Ending teardown" );
#endif		
}


