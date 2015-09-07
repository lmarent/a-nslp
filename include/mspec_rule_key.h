/// ----------------------------------------*- mode: C; -*--
/// @file mspec_rule_key.h
/// A key for object specs used
/// ----------------------------------------------------------
/// $Id: mspec_rule_key.h 2558 2015-09-01 10:27:00 amarentes $
/// $HeadURL: https://./include/mspec_rule_key.h $
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

#ifndef MSPEC_RULE_KEY_H
#define MSPEC_RULE_KEY_H

#include <uuid/uuid.h>

namespace anslp 
{

/**
 * \class mspec_rule_key
 *
 *
 * \brief This class respresents the key of a mnspec object that is 
 * 		  going to be used in a auctioning application (AA). 
 * 		  This value is used whenever we refer to this rule on the AA. 
 * 		  Therefore, we assume that is shared with the AA.
 *
 * \author Andres Marentes
 *
 * \version 0.1 
 *
 * \date 2015/09/01 10:30:00
 *
 * Contact: la.marentes455@uniandes.edu.co
 *  
 */
class mspec_rule_key
{

private:

	// typedef unsigned char uuid_t[16];
	uuid_t uuid;

public:
	
	/// Constructor of the field key
	mspec_rule_key();
	
	/// Copy contructor - Constructs a copy of mspec_rule_key.
	mspec_rule_key(const mspec_rule_key &rul_key);
	
	/// Destructor of the field key
	~mspec_rule_key();
		
	/**
	 *  Equals to operator. It is equal when they have the same uuids.
	 */
	bool operator ==(const mspec_rule_key &rhs) const;

	/** 
	 * less operator. 
	 */ 
	bool operator< (const mspec_rule_key& rhs) const;

	/** 
	 * Assignment operator. 
	*/ 
	inline mspec_rule_key& operator= (const mspec_rule_key& param)
	{
		uuid_copy(uuid, param.uuid); 
		return *this;
	}

	/**
	 * Return the key represented as string. 
	 */
	std::string to_string() const;
	
	/** 
	 * Not equal to operator. 
	*/ 
	inline bool operator != (const mspec_rule_key &rhs) const
	{
		return !(operator ==(rhs));
	}
	
};

} // namespace anslp

#endif // MSPEC_RULE_KEY_H
