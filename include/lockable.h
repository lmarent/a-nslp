/// ----------------------------------------*- mode: C++; -*--
/// @file Lockable.h
/// The Lockable classes.
/// ----------------------------------------------------------
/// $Id: session.h 2558 2016-04-23 08:44:36Z  $
/// $HeadURL: https://./include/Lockable.h $
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

#ifndef ANSLP_LOCKABLE_H
#define ANSLP_LOCKABLE_H

namespace anslp 
{

class lockable
{
	public:
		
		lockable() { }
		
		virtual ~lockable() { } 
	
		// Acquire the lock.
		virtual int acquire (void) = 0;
		
		// Release the lock.
		virtual int release (void) = 0;

};

} // namespace anslp

#endif // ANSLP_SESSION_H
