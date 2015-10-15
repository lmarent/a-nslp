/// ----------------------------------------*- mode: C++; -*--
/// @file xml_object_key.cpp
/// Keys used to handled auctioning objects in xml.
/// ----------------------------------------------------------
/// $Id: xml_object_key.cpp 2558 2015-09-07  $
/// $HeadURL: https://./src/msg/xml_object_key.cpp $
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

#include <sstream>
#include "msg/xml_object_key.h"


using namespace anslp::msg;

/// Constructor of the field key
xml_object_key::xml_object_key(): 
	object_type(IPAP_INVALID), id_object("")
{

}

	
/// Constructor of the field key
xml_object_key::xml_object_key(ipap_xml_object_type_t _object_type, 
									string _id_object): 
	object_type(_object_type), id_object(_id_object)
{
	switch (_object_type)
	{
		case IPAP_AUCTION:
			relatedTemplateTypeList.push_back(IPAP_SETID_AUCTION_TEMPLATE);
			relatedTemplateTypeList.push_back(IPAP_OPTNS_AUCTION_TEMPLATE);
			break;
		case IPAP_BID:
			relatedTemplateTypeList.push_back(IPAP_SETID_BID_TEMPLATE);
			relatedTemplateTypeList.push_back(IPAP_OPTNS_BID_TEMPLATE);
			break;
		case IPAP_ALLOCATION:
			relatedTemplateTypeList.push_back(IPAP_SETID_ALLOCATION_TEMPLATE);
			relatedTemplateTypeList.push_back(IPAP_OPTNS_ALLOCATION_TEMPLATE);
			break;
		default:
			break;
	}
}

xml_object_key& 
xml_object_key::operator= (const xml_object_key& param)
{
	object_type = param.object_type;
	id_object = param.id_object;
	relatedTemplateTypeList = param.relatedTemplateTypeList;
	return *this;
}


std::string 
xml_object_key::to_string() const
{
	std::ostringstream o1;
	o1 << "object_type:" << object_type;
	o1 << "Id object:" << id_object;
	return o1.str();

}

bool 
xml_object_key::operator< (const xml_object_key& rhs) const
{
	if (object_type < rhs.object_type){
		return true;
	}
	else{ 
	   return (id_object.compare(rhs.id_object) < 0);
	}
}


