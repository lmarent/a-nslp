
/* \file ConstantsAnslp.h

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
	here all string and numeric constants for the netaum toolset are declared

	$Id: AuctionManagerComponent.h 748 2015-07-26 9:33:00 amarentes $

*/


#ifndef _CONSTANTS_ANLSP_H_
#define _CONSTANTS_ANLSP_H_

#include "config.h"

namespace anslp 
{
  namespace msg {

// anslp_ipap_message.h
extern const string DTD_NAME;
extern const string DTD_FILENAME;
extern const string IPAP_XML_ROOT;
extern const string IPAP_XML_RECORD;

extern const string DATA_XML_ELEMENT;
extern const string OPTION_XML_ELEMENT;

} // namespace msg

} // namespace anslp

#endif // _CONSTANTS_ANLSP_H_
