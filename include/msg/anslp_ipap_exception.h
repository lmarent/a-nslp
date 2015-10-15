/*! \file   anslp_ipap_exception.h

    Copyright 2014-2015 Universidad de los Andes, Bogotá, Colombia

    This file is part of IP Auction Processing protocol (IPAP).

    IPAP is free software; you can redistribute it and/or modify 
    it under the terms of the GNU General Public License as published by 
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    IPAP is distributed in the hope that it will be useful, 
    but WITHOUT ANY WARRANTY; without even the implied warranty of 
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this software; if not, write to the Free Software 
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    Description:
    Exceptions for the anslp ipap message.

    $Id: IpAp_exception.h 748 2015-09-08 14:35:00Z amarentes $
*/

#ifndef ANSLP_IPAP_EXCEPTION_H
#define ANSLP_IPAP_EXCEPTION_H

#include "stdincpp.h"
#include "Error.h"

namespace anslp 
{
  namespace msg {

class anslp_ipap_bad_argument: public Error
{

public:
	/** Constructor (C strings). Creates bad argument Exception
     *  @param message C-style string error message.
     *                 The string contents are copied upon construction.
     *                 Hence, responsibility for deleting the \c char* lies
     *                 with the caller. 
     */
	anslp_ipap_bad_argument(const char* message);
	
	/** Constructor (C++ STL strings).
     *  @param message The error message.
     */
    anslp_ipap_bad_argument(const std::string& message);
      
    /** Destructor.
     * Virtual to allow for subclassing.
     */
    ~anslp_ipap_bad_argument();
    
    /** Returns a pointer to the (constant) error description.
     *  @return A pointer to a \c const \c char*. The underlying memory
     *          is in posession of the \c Exception object. Callers \a must
     *          not attempt to free the memory.
     */
    const char* what();
    
}; 

inline anslp_ipap_bad_argument::anslp_ipap_bad_argument(const char* message):
Error(0,message)
{

}

inline anslp_ipap_bad_argument::anslp_ipap_bad_argument(const std::string& message):
Error(0,message)
{

}

inline anslp_ipap_bad_argument::~anslp_ipap_bad_argument()
{

}

inline const char* anslp_ipap_bad_argument::what()
{
	 return (Error::getError()).c_str();
}


class anslp_ipap_memory_allocation: public Error
{

public:
	/** Constructor (C strings). Creates bad argument Exception
     *  @param message C-style string error message.
     *                 The string contents are copied upon construction.
     *                 Hence, responsibility for deleting the \c char* lies
     *                 with the caller. 
     */
	anslp_ipap_memory_allocation(const char* message);
	
	/** Constructor (C++ STL strings).
     *  @param message The error message.
     */
    anslp_ipap_memory_allocation(const std::string& message);
      
    /** Destructor.
     * Virtual to allow for subclassing.
     */
    ~anslp_ipap_memory_allocation();
    
    /** Returns a pointer to the (constant) error description.
     *  @return A pointer to a \c const \c char*. The underlying memory
     *          is in posession of the \c Exception object. Callers \a must
     *          not attempt to free the memory.
     */
    const char* what();
    
}; 

inline anslp_ipap_memory_allocation::anslp_ipap_memory_allocation(const char* message):
Error(0,message)
{

}

inline anslp_ipap_memory_allocation::anslp_ipap_memory_allocation(const std::string& message):
Error(0,message)
{

}

inline anslp_ipap_memory_allocation::~anslp_ipap_memory_allocation()
{

}

inline const char* anslp_ipap_memory_allocation::what()
{
	 return (Error::getError()).c_str();
}

}

}

#endif // ANSLP_IPAP_EXCEPTION_H
