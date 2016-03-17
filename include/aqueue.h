/// ----------------------------------------*- mode: C++; -*--
/// @file aqueue.h
/// a wrapper class for fastqueue C implementation
/// ----------------------------------------------------------
/// $Id: aqueue.h 2549 2015-12-23 7:13:00Z amarentes $
/// $HeadURL: 
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
// This class is a copy NSIS fqueue.cpp class, it changes the 
// type of objects going in the queue.
// ===========================================================

/** @ingroup fastqueue
 *
 */

#ifndef __AQUEUE_H__
#define __AQUEUE_H__


extern "C" {
#include"fastqueue.h"
}

#include "auction_rule.h"

namespace anslp {

typedef enum 
{
   ANSLP_CREATE_SESSION,
   ANLSP_CREATE_CHECK_SESSION,
   ANLSP_RESPONSE_CREATE_SESSION,
   ANLSP_RESPONSE_CREATE_CHECK_SESSION,
   ANSLP_AUCTION_INTERACTION,
   ANSLP_REMOVE_SESSION,
   ANSLP_RESPONSE_REMOVE_SESSION
} anslp_event_t;


class FastQueue;

class AnslpEvent{
      
  private:

    //! type of the event   
    anslp_event_t type;    

	///! message for the auction interaction that is going to be added.
	///! first element corresponds to the object key, 
	///! second element corresponds to a message representing the object.
	objectList_t objects;
	
	///! SessionId to be related.
	string sessionId;
	
  public:
	
	/*! \short  create an AnslpEvent */
	
	AnslpEvent(anslp_event_t type): type(type){}

    virtual ~AnslpEvent() 
    {
		objectListIter_t it;
		for ( it = objects.begin(); it != objects.end(); it++)
		{
			if (it->second != NULL)
				delete(it->second);
		}
	}
    
    //! get event type
    anslp_event_t getType() 
    { 
        return type;
    }

    int isType( anslp_event_t atype )
    { 
        return (type == atype);
    }
	
	void setObject(mspec_rule_key key, msg::anslp_mspec_object *obj)
	{
		if ( obj == NULL )
		return;

		msg::anslp_mspec_object *old = objects[key];

		if ( old )
			delete old;

		objects[key] = obj;

	}

	objectList_t * getObjects()
	{
		return &objects;
	}

	void setSession(string _sessionId)
	{
		sessionId = _sessionId;
	}

	string getSession()
	{
		return sessionId;
	}
};

class CheckEvent: public AnslpEvent
{
	private:
		
		
		///! Fast queue where we have to put the return. 
		///! It can be NULL, in which case this event is processed without response
		anslp::FastQueue *ret; 
	
	public:
		CheckEvent(anslp::FastQueue * _ret): 
			AnslpEvent(ANLSP_CREATE_CHECK_SESSION), ret(_ret){}
		
		virtual ~CheckEvent() {}
		
		
		anslp::FastQueue * getQueue()
		{
			return ret;
		}
};

class ResponseCheckSessionEvent: public AnslpEvent
{
					
	public:
		//! ctrlcomm events always expire now
		ResponseCheckSessionEvent(  ): 
			AnslpEvent(ANLSP_RESPONSE_CREATE_CHECK_SESSION){}
		
		virtual ~ResponseCheckSessionEvent() {}
		
		
};

class AddSessionEvent: public AnslpEvent
{
	private:
				
		///! Fast queue where we have to put the return. 
		///! It can be NULL, in which case this event is processed without response
		anslp::FastQueue *ret; 
	
	public:
		//! ctrlcomm events always expire now
		AddSessionEvent( anslp::FastQueue * _ret): 
			AnslpEvent(ANSLP_CREATE_SESSION), ret(_ret){}
		
		virtual ~AddSessionEvent() {}
				
		anslp::FastQueue * getQueue()
		{
			return ret;
		}
};

class ResponseAddSessionEvent: public AnslpEvent
{
					
	public:

		ResponseAddSessionEvent( ): 
			AnslpEvent( ANLSP_RESPONSE_CREATE_SESSION ){}
		
		virtual ~ResponseAddSessionEvent() {}
				
};

class AuctionInteractionEvent: public AnslpEvent
{
					
	public:

		AuctionInteractionEvent(  ): 
			AnslpEvent( ANSLP_AUCTION_INTERACTION ){}
		
		virtual ~AuctionInteractionEvent() {}
		
		
};

class RemoveSessionEvent: public AnslpEvent
{
	private:
				
		///! Fast queue where we have to put the return. 
		///! It can be NULL, in which case this event is processed without response
		anslp::FastQueue *ret; 
	
	public:
		//! ctrlcomm events always expire now
		RemoveSessionEvent( anslp::FastQueue * _ret): 
			AnslpEvent(ANSLP_REMOVE_SESSION), ret(_ret){}
		
		virtual ~RemoveSessionEvent() {}
				
		anslp::FastQueue * getQueue()
		{
			return ret;
		}
};


class ResponseRemoveSessionEvent: public AnslpEvent
{
					
	public:
		//! ctrlcomm events always expire now
		ResponseRemoveSessionEvent( ): 
			AnslpEvent(ANSLP_RESPONSE_REMOVE_SESSION) {}
		
		virtual ~ResponseRemoveSessionEvent() {}
				
};


inline bool is_check_event(const AnslpEvent *evt) 
{
	return dynamic_cast<const CheckEvent *>(evt) != NULL;
}

inline bool is_addsession_event(const AnslpEvent *evt) 
{
	return dynamic_cast<const AddSessionEvent *>(evt) != NULL;
}

inline bool is_response_addsession_event(const AnslpEvent *evt) 
{
	return dynamic_cast<const ResponseAddSessionEvent *>(evt) != NULL;
}

inline bool is_response_checksession_event(const AnslpEvent *evt) 
{
	return dynamic_cast<const ResponseCheckSessionEvent *>(evt) != NULL;
}

inline bool is_auction_interaction_event(const AnslpEvent *evt) 
{
	return dynamic_cast<const AuctionInteractionEvent *>(evt) != NULL;
}

inline bool is_removesession_event(const AnslpEvent *evt)
{
	return dynamic_cast<const RemoveSessionEvent *>(evt) != NULL;
}

inline bool is_response_removesession_event(const AnslpEvent *evt) 
{
	return dynamic_cast<const ResponseRemoveSessionEvent *>(evt) != NULL;
}

/** @addtogroup fastqueue Fast Queue
 * @{
 */

/** 
 * A fast event queue.
 *
 * This is a fast and thread-safe event queue with expedited data
 * support. It is an object oriented wrapper around fastqueue.c.
 * The queue grows dynamically and has no built-in entry limit.
 */
class FastQueue {
public:
	/// FastQueue error
	class FQError{};
	/// constructor
	FastQueue(const char *qname = 0, bool exp = false);
	/// destructor
	~FastQueue();
	/// enqueue event
	bool enqueue(AnslpEvent *element, bool exp = false);
	/// dequeue event
	AnslpEvent *dequeue(bool blocking = true);
	/// dequeue event, timed wait
	AnslpEvent *dequeue_timedwait(const struct timespec &tspec);
	/// dequeue event, timed wait
	AnslpEvent *dequeue_timedwait(const long int msec);
	/// is queue empty
	bool is_empty() const;
	/// get number of enqueued events
	unsigned long size() const;
	/// is expedited data support enabled
	bool is_expedited_enabled() const;
	/// enable/disable expedited data
	bool enable_expedited(bool exp);
	/// shutdown queue, do not accept events
	void shutdown();
	/// delete stored events
	unsigned long cleanup();
	/// Return the name of the queue.
	const char* get_name() const { return queue_name.c_str(); }
private:
	/// C fastqueue
	queue_t *queue;
	/// name of the queue, also stored in the queue_t
	string queue_name;
	/// accept or reject events
	bool shutdownflag;
};


/**
 * Remove the first event from the queue.
 *
 * Events are removed in the same order they were added to the queue (FIFO).
 * If the expedited events feature is enabled, there's an exception to
 * this rule: Expedited events are always removed before all other events.
 * The FIFO condition still holds among expedited events, however.
 *
 * If blocking is set, wait infinitely for a event. If set to false,
 * return immediately if the queue is empty. In this case, NULL is returned.
 *
 * @param blocking if true, block until a event arrives
 *
 * @return the event, or NULL
 */
inline
AnslpEvent *
FastQueue::dequeue(bool blocking)
{
  return static_cast<AnslpEvent*>(blocking ? 
			       dequeue_element_wait(queue) :
			       dequeue_element_nonblocking(queue));
}


/**
 * Wait for a event for a given time.
 *
 * If no event arrives in the given time period, NULL is returned.
 *
 * @param tspec the time to wait
 *
 * @return the event, or NULL
 */
inline
AnslpEvent *
FastQueue::dequeue_timedwait(const struct timespec& tspec)
{
  return (AnslpEvent*)dequeue_element_timedwait(queue, &tspec);
}

//@}

} // end namespace anslp

#endif 
