/*
 * Test the anslp_ipap_xml_message_test class.
 *
 * $Id: anslp_ipap_xml_message_test.h 2015-09-18 7:22:00 amarentes $
 * $HeadURL: https://./test/anslp_ipap_xml_message_test.h $
 */
// ===========================================================
//                      
// Copyright (C) 2012-2014, all rights reserved by
// - System and Computing Engineering, Universidad de los Andes
//
// More information and contact:
// https://www.uniandes.edu.co/
//                      
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; version 2 of the License
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//
// ===========================================================  
#ifndef ANSLP_IPAP_XML_MESSAGE_TEST_H
#define ANSLP_IPAP_XML_MESSAGE_TEST_H

#include "IpAp_def.h"
#include "IpAp_data_record.h"
#include "IpAp_message.h"
#include "anslp_ipap_message.h"
#include "anslp_ipap_xml_message.h"


namespace anslp {

class anslp_ipap_xml_message_test 
{

  public:
	anslp_ipap_xml_message_test();
	~anslp_ipap_xml_message_test();
	void setUp();
	void tearDown();
	void buildAuctionMessage(msg::anslp_ipap_message *message);
	void buildBidMessage(msg::anslp_ipap_message *message);
	void buildAllocationMessage(msg::anslp_ipap_message *message);
	void testExport();

  private:  
    msg::anslp_ipap_message *auction;
    msg::anslp_ipap_message *bid;
    msg::anslp_ipap_message *allocation;

	Logger *log; //!< link to global logger object
	int ch;      //!< logging channel number used by objects of this class

};

} // namespace anslp

#endif // ANSLP_IPAP_XML_MESSAGE_TEST_H
