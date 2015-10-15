
/* \file ConstantsAnslp.cpp

    Copyright 2014-2015 Universidad de los Andes, Bogotá, Colombia

    This file is part of Network Auction Manager System (NETAUM).

    NETAUM is free software; you can redistribute it and/or modify 
    it under the terms of the GNU General Public License as published by 
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    NETAUM is distributed in the hope that it will be useful, 
    but WITHOUT ANY WARRANTY; without even the implied warranty of 
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this software; if not, write to the Free Software 
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

	Description:
	here all string and numeric constants for the NETAUM toolset are stored

	$Id: ConstantsAnslp.cpp 748 2015-10-03 18:04:00 amarentes $

*/

#include "stdincpp.h"
#include "anslp_constants.h"

namespace anslp
{
	
namespace msg
{


// anslp_ipap_xml_message.h
const string DTD_NAME = "ipap_xml_message.dtd";
const string DTD_FILENAME = DEF_SYSCONFDIR "/ipap_xml_message.dtd";
const string IPAP_XML_ROOT = "IPAP_MESSAGE";
extern const string IPAP_XML_RECORD = "DATARECORD";

const string DATA_XML_ELEMENT = "FIELD";
const string OPTION_XML_ELEMENT = "FIELD";

};

};
