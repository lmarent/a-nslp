/*!
 * The ANSLP IE Manager.
 *
 * $Id: anslp_ie.h 1718 2014-11-05  $
 * $HeadURL: https://include/msg/anslp_ie.h $
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
#ifndef ANSLP_MSG_IE_H
#define ANSLP_MSG_IE_H

#include "ie.h"

namespace anslp 
{

 namespace msg {
     using namespace protlib;


/**
 * Categories for ANSLP classes.
 */
enum category_t {
	cat_anslp_msg			= 0,
	cat_anslp_object		= 1,

	cat_default_anslp_msg		= 2,
	cat_default_anslp_object	= 3
};

/**
 * An Interface for reading/writing ANSLP Messages.
 *
 * The ANSLP_IEManager is a Singleton which provides methods to read/write
 * ANSLP Messages from/to NetMsg objects. Those methods are called
 * deserialize() and serialize(), respectively.
 *
 * To deserialize() a ANSLP Message, each IE to be used during the process
 * has to be registered with ANSLP_IEManager using register_ie(). Registered
 * IEs will be freed automatically as soon as either clear() is called or
 * ANSLP_IEManager itself is destroyed.
 *
 * The only way to get an ANSLP_IEManager object is through the static
 * instance() method.
 */
class ANSLP_IEManager : public IEManager {

  public:
	static ANSLP_IEManager *instance();
	static void clear();

	static void register_known_ies();

	virtual IE *deserialize(NetMsg &msg, uint16 category,
			IE::coding_t coding, IEErrorList &errorlist,
			uint32 &bytes_read, bool skip);

  protected:
	// protected constructor to prevent instantiation
	ANSLP_IEManager();
	
	virtual IE *lookup_ie(uint16 category, uint16 type, uint16 subtype);

  private:
  
	static ANSLP_IEManager *anslp_inst;

	IE *deserialize_msg(NetMsg &msg,
		IE::coding_t coding, IEErrorList &errorlist,
		uint32 &bytes_read, bool skip);

	IE *deserialize_object(NetMsg &msg,
		IE::coding_t coding, IEErrorList &errorlist,
		uint32 &bytes_read, bool skip);
};


 } // namespace msg
} // namespace anslp

#endif // ANSLP_MSG_IE_H
