/*!
  ie_store.h - Store IEs by ID
 
  $Id: ie_store.h 2276 2014-11-05  $
  $HeadURL: https://./include/msg/ie_store.h $
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

#ifndef PROTLIB__IE_STORE_H
#define PROTLIB__IE_STORE_H

#include <map>

#include "ie.h"
#include "ie_object_key.h"


namespace protlib 
{


/**
 * Stores (ID, IE) mappings.
 *
 * This is a helper class intented for internal use. It mainly specializes
 * std::map and takes care of memory management issues.
 */
class ie_store 
{
	
  public:
	ie_store();
	ie_store(const ie_store &other);
	~ie_store();

	size_t size() const throw();
	IE *get(ie_object_key id) const throw();
	void set(ie_object_key id, IE *ie) throw();
	IE *remove(ie_object_key id) throw();

	bool operator==(const ie_store &other) const throw();
	uint32 getMaxSequence(uint32 id) const;

	typedef std::map<ie_object_key, IE *>::const_iterator const_iterator;

	const_iterator begin() const throw() { return entries.begin(); }
	const_iterator end() const throw() { return entries.end(); }

  private:
	/**
	 * Maps IDs to IEs.
	 *
	 * Note: Don't use __gnu_cxx::hash_map here. It is vector-based and
	 *       *extremely* expensive to initialize. The constructor eats
	 *       up much more processing time than we can ever gain by the
	 *       cheaper lookup method.
	 */
	std::map<ie_object_key, IE *> entries;

	/**
	 * Shortcut.
	 */
	typedef const_iterator c_iter;
};


} // namespace protlib

#endif // PROTLIB__IE_STORE_H
