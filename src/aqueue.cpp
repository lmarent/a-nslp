/// ----------------------------------------*- mode: C++; -*--
/// @file aqueue.cpp
/// wrapper class for fastqueue
/// ----------------------------------------------------------
/// $Id: aqueue.cpp 2549 2015-12-23 7:02:30Z amarentes $
/// $HeadURL:  $
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

#include "stdincpp.h"
#include "aqueue.h"
#include "mspec_rule_key.h"
#include "logfile.h"


namespace anslp {

/** @addtogroup fastqueue Fast Queue
 * @{
 */

using namespace protlib::log;

/**
 * Constructor.
 *
 * Initialize a FastQueue with a queue name and enable/disable expedited
 * data.
 *
 * @param qname the queue's name, or NULL
 * @param exp if true, expedited data support is enabled
 */
FastQueue::FastQueue(const char *qname, bool exp)
    : queue_name((qname == 0) ? "" : (char*)qname), shutdownflag(false)
{
  if ((queue = create_queue(qname)) == NULL)
  {
    Log(ERROR_LOG, LOG_ALERT, "FastQueue", "Could not create queue " << queue_name);
    throw FQError();
  } else queue_enable_expedited(queue,exp);

}


/**
 * Add a event to the queue.
 *
 * If exp is true and the queue allows expedited data, the event will
 * pass all normal events in the queue and thus will be delivered earlier.
 * If there are other expedited events in the queue already, it will be
 * added after the already existing expedited messages.
 *
 * This method may fail (and return false) if the queue is in shutdown mode,
 * there is a problem adquiring locks, or some other threading problem.
 *
 * In case the queue is deleted before this event has been removed, this
 * event is deleted using the delete operator. Because of this, the same
 * message may only appear once in a queue.
 * 
 * @param element a pointer to the event to add
 * @param exp true if this is expedited data
 * 
 * @return true if the element was enqueued successfully
 */
bool FastQueue::enqueue(AnslpEvent *element, bool exp)
{
  if (shutdownflag) return false;
  if (enqueue_element_expedited_signal(queue, (void*)element, exp) < 0)
  {
    Log(ERROR_LOG, LOG_ALERT, "FastQueue", "Could not enqueue element in queue " << queue_name);
    return false;
  }
  return true;
}


/**
 * Wait for a event for a given time.
 *
 * If no event arrives in the given time period, NULL is returned.
 *
 * @param msec the time to wait in milliseconds
 *
 * @return the event, or NULL
 */
AnslpEvent *FastQueue::dequeue_timedwait(const long int msec)
{
  struct timespec tspec = {0,0};
  tspec.tv_sec = msec/1000;
  tspec.tv_nsec = (msec%1000)*1000000;
  void* message = dequeue_element_timedwait(queue, &tspec);
  if (message != NULL){
	return (AnslpEvent*) message;
  } else {
	return NULL;
  }
}


/**
 * Destructor.
 * 
 * Destroys the queue. All events which are still in the queue are deleted
 * using the delete operator.
 */
FastQueue::~FastQueue()
{
  if (queue) 
  {
    cleanup();
    if ((destroy_queue(queue)) < 0)
    {
      Log(ERROR_LOG, LOG_ALERT, "FastQueue", "Could not destroy queue " << queue_name);
    }
  }
  DLog("FastQueue", "~FastQueue() - done for queue " << queue_name);
}

/**
 * Test if the queue is empty.
 *
 * @return true if the queue is empty
 */
bool FastQueue::is_empty() const
{
  if (queue_nr_of_elements(queue)==0)
    return true;
  else
    return false;
}


/**
 * Return the number of events in the queue.
 *
 * @return the number of enqueued events
 */
unsigned long FastQueue::size() const
{
  return queue_nr_of_elements(queue);
}


/**
 * Test if expedited event support is enabled.
 * 
 * @return true if expedited event support is enabled
 */
bool FastQueue::is_expedited_enabled() const
{
  if (queue_is_expedited_enabled(queue))
    return true;
  else
    return false;
}

/**
 * Enable or disable expedited events.
 *
 * This also returns the previous value of this flag.
 *
 * @return true, if expedited events were previously enabled, false otherwise
 */
bool FastQueue::enable_expedited(bool exp)
{
  if (queue_enable_expedited(queue,exp))
    return true;
  else
    return false;
}


/**
 * Disable enqueueing of new events.
 *
 * A queue in shutdown mode does not accept events any more.
 */
void FastQueue::shutdown() { shutdownflag = true; }


/**
 * Put queue into shutdown mode and delete all stored events..
 *
 * @return the number of events that were in the queue
 */
unsigned long FastQueue::cleanup()
{
  unsigned long count = 0;
  AnslpEvent* ae = NULL;
  shutdownflag = true;
  while (!is_empty())
    if ((ae = dequeue(false))) {
      delete ae;
      ae = NULL;
      count++;
    }
  return count;
}

//@}

} // end namespace auction
