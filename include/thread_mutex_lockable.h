/// ----------------------------------------*- mode: C++; -*--
/// @file thread_mutex_lockable.h
/// The thread mutex lockable classes.
/// ----------------------------------------------------------
/// $Id: session.h 2558 2016-04-23 08:44:36Z  $
/// $HeadURL: https://./include/thread_mutex_lockable.h $
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


#ifndef ANSLP_THREAD_MUTEX_LOCKABLE_H
#define ANSLP_THREAD_MUTEX_LOCKABLE_H

#include "lockable.h"
#include <pthread.h>
#include <assert.h>

namespace anslp 
{

class thread_mutex_lockable: public lockable
{
	public:
		// Constructor stores a reference to the
		// base class.
		thread_mutex_lockable ();

		// Destructor for the class.
		~thread_mutex_lockable();
		
		// Acquire the lock.
		virtual int acquire (void);

		// Release the lock.
		virtual int release (void);
	
	private:
		
		// Concrete lock type.
		pthread_mutex_t	mutex;
};

} // namespace anslp

#endif // ANSLP_THREAD_MUTEX_LOCKABLE_H
