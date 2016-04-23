/// ----------------------------------------*- mode: C++; -*--
/// @file thread_mutex_lockable.cpp
/// The thread mutex lockable classes.
/// ----------------------------------------------------------
/// $Id: session.h 2558 2016-04-23 08:44:36Z  $
/// $HeadURL: https://./include/thread_mutex_lockable.cpp $
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

#include "thread_mutex_lockable.h"

using namespace anslp;

// Constructor stores a reference to the
// base class.
thread_mutex_lockable::thread_mutex_lockable ()
{

	pthread_mutexattr_t mutex_attr;

	pthread_mutexattr_init(&mutex_attr);

#ifdef _DEBUG
	pthread_mutexattr_settype(&mutex_attr, PTHREAD_MUTEX_ERRORCHECK);
#else
	pthread_mutexattr_settype(&mutex_attr, PTHREAD_MUTEX_NORMAL);
#endif

	pthread_mutex_init(&mutex, &mutex_attr);

	pthread_mutexattr_destroy(&mutex_attr); // valid, doesn't affect mutex

}

// Destructor for the class.
thread_mutex_lockable::~thread_mutex_lockable()
{
	pthread_mutex_destroy(&mutex);
}


// Acquire the lock.
int thread_mutex_lockable::acquire(void)
{

	//typedef void (*cleanup_t)(void *);
	int ret;

	//pthread_cleanup_push((cleanup_t) pthread_mutex_unlock, (void *) &mutex);
	ret = pthread_mutex_lock(&mutex);
	assert( ret == 0 );
	
	return ret;
}


// Release the lock.
int thread_mutex_lockable::release(void)
{
	int ret;

	ret = pthread_mutex_unlock(&mutex);
	assert( ret == 0 );
	//pthread_cleanup_pop(0);
	
	return ret;
}
	

