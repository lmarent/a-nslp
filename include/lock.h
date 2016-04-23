/// ----------------------------------------*- mode: C++; -*--
/// @file lock.h
/// 	The lock class.
/// ----------------------------------------------------------
/// $Id: session.h 2558 2016-04-23 08:44:36Z  $
/// $HeadURL: https://./include/lock.h $
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

#ifndef ANSLP_LOCK_H
#define ANSLP_LOCK_H

#include "lockable.h"

namespace anslp 
{

class lock
{
	public:
		// Constructor stores a reference to the
		// base class.
		lock (lockable *l): lock_ (l) {};

		// Destructor for the class.
		~lock();
		
		// Acquire the lock by forwarding to the
		// polymorphic acquire() method.
		int acquire (void) { return lock_->acquire (); }
		
		// Release the lock by forwarding to the
		// polymorphic release() method.
		int release (void) { return lock_->release (); }

	private:

		// Maintain a pointer to the polymorphic lock.
		lockable *lock_;
};

inline lock::~lock()
{
	if (lock_ != NULL)
		delete lock_;
}

} // namespace anslp

#endif // ANSLP_LOCK_H
