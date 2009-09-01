/*
 * monitor.c - Syncronization Monitor routines.
 *
 * Copyright (C) 2009  Southern Storm Software, Pty Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/*
 * This is the generic code for monitor support for all pattforms.
 * The patform dependent code in in the *_defs files.
 */

#include <il_utils.h>
#include "thr_defs.h"
#include "interlocked.h"

#ifdef	__cplusplus
extern	"C" {
#endif

struct _tagILMonitor
{
	ILMonitor		   *next;		/* The next monitor in the list */
	ILThread volatile  *owner;		/* The current owner of the monotor */
	ILInt32			enterCount;		/* The number of enters without corresponding leave by the owner */
	ILUInt32			users;		/* The number of threads using this monitor */
	_ILMonitor			monitor;	/* The platform dependent monitor */
};

static void ILMonitorInit(ILMonitor *monitor)
{
	monitor->next = 0;
	monitor->owner = 0;
	monitor->enterCount = 0;
	monitor->users = 0;
	_ILMonitorCreate(&(monitor->monitor));
}

static void ILMonitorDestroy(ILMonitor *monitor)
{
	monitor->next = 0;
	monitor->owner = 0;
	monitor->enterCount = 0;
	monitor->users = 0;
	_ILMonitorDestroy(&(monitor->monitor));
}

static void DestroyMonitorList(ILMonitor *monitor)
{
	while(monitor)
	{
		ILMonitor *next;

		next = monitor->next;
		ILMonitorDestroy(monitor);
		monitor = next;
	}
}

void ILMonitorPoolInit(ILMonitorPool *pool)
{
	if(!pool)
	{
		return;
	}
	ILMemPoolInitType(&(pool->pool), ILMonitor, 20);
	pool->freeList = 0;
	pool->usedList = 0;
	_ILMutexCreate(&(pool->lock));
}

void ILMonitorPoolDestroy(ILMonitorPool *pool)
{
	if(!pool)
	{
		return;
	}
	DestroyMonitorList(pool->freeList);
	pool->freeList = 0;
	/*
	 * NOTE: If the usedList is not 0 we might be in very big trouble afterwards
	 * but we clean up the resources used by this list anyways.
	 */
	DestroyMonitorList(pool->usedList);
	pool->usedList = 0;
	_ILMutexDestroy(&(pool->lock));
	ILMemPoolDestroy(&(pool->pool));
}

/*
 * Get a new monitor from the monitor pool.
 * This function must be called with the monitorpool lock held.
 */
static ILMonitor *ILMonitorPoolAllocMonitor(ILMonitorPool *pool)
{
	ILMonitor *monitor;

	if(pool->freeList)
	{
		/* We have a monitor on the freelist so reuse this one */
		monitor = pool->freeList;
		pool->freeList = monitor->next;
		/* And add the monitor to the used list */
		monitor->next = pool->usedList;
		pool->usedList = monitor;
	}
	else
	{
		monitor = ILMemPoolAllocItem(&(pool->pool));
		if(monitor)
		{
			ILMonitorInit(monitor);
			/* And add the monitor to the used list */
			monitor->next = pool->usedList;
			pool->usedList = monitor;
		}
	}
	return monitor;
}

/*
 * Move a monitor from the used list to the free list of the monitor pool.
 * This function must be called with the monitorpool lock held.
 */
static void ILMonitorPoolMoveToFreeList(ILMonitorPool *pool,
										ILMonitor *monitor)
{
	if(pool->usedList == monitor)
	{
		pool->usedList = monitor->next;
		monitor->next = pool->freeList;
		pool->freeList = monitor;
	}
	else
	{
		ILMonitor *prevMonitor;

		prevMonitor = pool->usedList;
		while(prevMonitor)
		{
			if(prevMonitor->next == monitor)
			{
				prevMonitor->next = monitor->next;
				monitor->next = pool->freeList;
				pool->freeList = monitor;
				return;
			}
			prevMonitor = prevMonitor->next;
		}
	}
}

int ILMonitorTimedTryEnter(ILMonitorPool *pool, void **monitorLocation,
						   ILUInt32 ms)
{
	ILMonitor * volatile *monLoc;
	ILMonitor *monitor;
	ILThread *thread;

	if(!pool || !monitorLocation)
	{
		return IL_THREAD_ERR_UNKNOWN;
	}
	/* Store the location in a volatile variable */
	monLoc = (ILMonitor **)monitorLocation;

	/* Get my thread */
	thread = _ILThreadGetSelf();

	_ILMutexLock(&(pool->lock));
	if((monitor = *monLoc) == 0)
	{
		/* We have to allocate a new monitor */
		monitor = ILMonitorPoolAllocMonitor(pool);
		if(!monitor)
		{
			_ILMutexUnlock(&(pool->lock));
			return IL_THREAD_ERR_UNKNOWN;
		}
		/* and store the monitor at the location given */
		*monLoc = monitor;	
		/* Add me to the monitor users */
		++(monitor->users);
	}
	else if(monitor->owner != thread)
	{
		/* Add me to the monitor users */
		++(monitor->users);
	}
	_ILMutexUnlock(&(pool->lock));

	if(monitor->owner == thread)
	{
		/*
		 * I'm already the owner of this monitor.
		 * So simply increase the enter count.
		 */
		++(monitor->enterCount);
	}
	else
	{
		/*
		 * We have to enter the monitor.
		 */
		int result;

		result = _ILMonitorTimedTryEnter(&(monitor->monitor), ms);
		if(result == IL_THREAD_OK)
		{
			/*
			 * The owner should be 0 at this point.
			 */
			monitor->owner = thread;
			monitor->enterCount = 1;
		}
		else
		{
			/* We have to leave the monitor again */
			_ILMutexLock(&(pool->lock));

			/* Remove me from the monitor users */
			--(monitor->users);
			if(monitor->users <= 0)
			{
				/* No other waiters on the monitor */
				monitor->users = 0;
				/* So move the monitor to the free list of the  pool */
				ILMonitorPoolMoveToFreeList(pool, monitor);
				/* And clear the monitor location */
				*monitorLocation = 0;
			}
			_ILMutexUnlock(&(pool->lock));
		}
		return result;
	}
	return IL_THREAD_OK;
}

int ILMonitorExit(ILMonitorPool *pool, void **monitorLocation)
{
	ILMonitor * volatile *monLoc;
	ILMonitor *monitor;
	ILThread *thread;

	if(!pool || !monitorLocation)
	{
		return IL_THREAD_ERR_UNKNOWN;
	}
	/* Store the location in a volatile variable */
	monLoc = (ILMonitor **)monitorLocation;
	if((monitor = *monLoc) == 0)
	{
		return IL_THREAD_ERR_SYNCLOCK;
	}
	thread = _ILThreadGetSelf();
	if(monitor->owner != thread)
	{
		return IL_THREAD_ERR_SYNCLOCK;
	}
	/* Decrement the enter count */
	--(monitor->enterCount);
	if(monitor->enterCount <= 0)
	{
		/* We really are leaving the monitor */
		_ILMutexLock(&(pool->lock));

		/* Remove me from the monitor users */
		--(monitor->users);
		/* Clear the owner */
		monitor->owner = 0;
		monitor->enterCount = 0;
		if(monitor->users <= 0)
		{
			/* No other waiters on the monitor */
			monitor->users = 0;
			/* So move the monitor to the free list of the  pool */
			ILMonitorPoolMoveToFreeList(pool, monitor);
			/* And clear the monitor location */
			*monitorLocation = 0;
		}
		_ILMonitorExit(&(monitor->monitor));
		_ILMutexUnlock(&(pool->lock));
	}
	return IL_THREAD_OK;
}

int ILMonitorPulse(void **monitorLocation)
{
	ILMonitor *monitor;
	ILThread *thread;

	if(!monitorLocation)
	{
		return IL_THREAD_ERR_UNKNOWN;
	}
	if((monitor = (ILMonitor *)(*monitorLocation)) == 0)
	{
		return IL_THREAD_ERR_SYNCLOCK;
	}
	thread = _ILThreadGetSelf();
	if(monitor->owner != thread)
	{
		return IL_THREAD_ERR_SYNCLOCK;
	}
	_ILMonitorPulse(&(monitor->monitor));
	return IL_THREAD_OK;
}

int ILMonitorPulseAll(void **monitorLocation)
{
	ILMonitor *monitor;
	ILThread *thread;

	if(!monitorLocation)
	{
		return IL_THREAD_ERR_UNKNOWN;
	}
	if((monitor = (ILMonitor *)(*monitorLocation)) == 0)
	{
		return IL_THREAD_ERR_SYNCLOCK;
	}
	thread = _ILThreadGetSelf();
	if(monitor->owner != thread)
	{
		return IL_THREAD_ERR_SYNCLOCK;
	}
	_ILMonitorPulseAll(&(monitor->monitor));
	return IL_THREAD_OK;
}

/*
 * Predicate function for monitor wait.
 */
static int MonitorPredicate(void *mon)
{
	ILMonitor *monitor;
	ILThread *thread;

	monitor = (ILMonitor *)mon;
	thread = _ILThreadGetSelf();
	ILInterlockedCompareAndExchangePointers((void **)&(monitor->owner),
											thread, 0);
	return (monitor->owner == thread);
}

int ILMonitorTimedWait(void **monitorLocation, ILUInt32 ms)
{
	ILMonitor *monitor;
	ILThread *thread;
	int result;
	ILUInt32 enterCount;

	if(!monitorLocation)
	{
		return IL_THREAD_ERR_UNKNOWN;
	}
	thread = _ILThreadGetSelf();
	monitor = (ILMonitor *)(*monitorLocation);
	if(!monitor || monitor->owner != thread)
	{
		return IL_THREAD_ERR_SYNCLOCK;
	}
	/*
	 * Save the number of times this thread entered the monitor and reset
	 + the value in the monitor
	 */
	enterCount = monitor->enterCount;
	monitor->enterCount = 0;
	result = _ILMonitorTimedWait(&(monitor->monitor), ms, MonitorPredicate, monitor);
	if(result == IL_THREAD_OK || result == IL_THREAD_BUSY)
	{
		monitor->enterCount = enterCount;
		monitor->owner = thread;
	}
	return result;
}

#ifdef	__cplusplus
};
#endif
