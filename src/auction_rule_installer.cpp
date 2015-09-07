/// ----------------------------------------*- mode: C++; -*--
/// @file auction_rule_installer.cpp
/// The auction_rule_installer class.
/// ----------------------------------------------------------
/// $Id: auction_rule_installer.cpp 2558 2015-09-03 15:49:00 amarentes $
/// $HeadURL: https://./src/auction_rule_installer.cpp $
// ===========================================================
//                      
// Copyright (C) 2005-2007, all rights reserved by
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

#include <libxml/xmlreader.h>

#include "auction_rule_installer.h"
#include "msg/information_code.h"


namespace anslp {

auction_rule_installer::~auction_rule_installer() throw()
{


}

void
auction_rule_installer::setup() throw (auction_rule_installer_error)
{
	
	if ( config->is_auctioneer()){
		
		try
		{
			// Parse and load the filter configuration.
			// parse_configuration_definition_file(config->get_configuration_file().c_str());
			
			std::cout << "Finish loading configuration file" << std::endl;
			
			// Parse and load the export configuration.
			// parse_export_definition_file(config->get_export_config_file().c_str());
			
			std::cout << "Finish loading export configuration file" << std::endl;
		}
		catch(auction_rule_installer_error &e)
		{
			std::cout << "Node cannot be configured as metering" << std::endl;
			config->setpar(anslpconf_is_auctioneer, false);
		}
			
	}
}

void auction_rule_installer::parse_export_definition_file(const char *filename) 
		throw (auction_rule_installer_error)
{

    xmlTextReaderPtr reader;
    int ret;

    /*
     * Pass some special parsing options to activate DTD attribute defaulting,
     * entities substitution and DTD validation
     */
    reader = xmlReaderForFile(filename, NULL,
                 XML_PARSE_DTDATTR |  /* default DTD attributes */
				 XML_PARSE_NOENT |    /* substitute entities */
				 XML_PARSE_DTDVALID); /* validate with the DTD */
	
    /*
     * Cleanup function for the XML library.
     */
    xmlCleanupParser();
    /*
     * this is to debug memory for regression tests
     */
    xmlMemoryDump();
	
}

void auction_rule_installer::parse_configuration_definition_file(const char *filename) 
		throw (auction_rule_installer_error)
{

    xmlTextReaderPtr reader;
    int ret;

    /*
     * Pass some special parsing options to activate DTD attribute defaulting,
     * entities substitution and DTD validation
     */
    reader = xmlReaderForFile(filename, NULL,
                 XML_PARSE_DTDATTR |  /* default DTD attributes */
				 XML_PARSE_NOENT |    /* substitute entities */
				 XML_PARSE_DTDVALID); /* validate with the DTD */
	
}

std::string
auction_rule_installer::to_string() const
{
	std::string val_return;
	//if (action_container!= NULL)
	//	val_return.append(action_container->to_string());
	/*
	val_return.append("\n");
	if (app_container != NULL)
		val_return.append(app_container->to_string());
	* */
	return val_return;
}
	
} // anslp
