/*
 * wait_mutex.c - Wait mutex objects for the threading sub-system.
 *
 * Copyright (C) 2002  Southern Storm Software, Pty Ltd.
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

Note: the code in this module is generic to all platforms.  It implements
the correct CLI wait mutex semantics based on the primitives in "*_defs.h".
You normally won't need to modify or replace this file when porting.

Wait mutexes differ from ordinary mutexes in that they can be used
from C# code and can be held by a suspended thread.

*/

#include "thr_defs.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * List of mutex names that are in use.
 */
typedef struct _tagILMutexName ILMutexName;
struct _tagILMutexName
{
	ILMutexName	   *next;
	ILWaitHandle   *handle;
	char			name[1];

};
static ILMutexName *nameList;
static _ILMutex nameListLock;

/*
 * Close a regular mutex.
 */
static int MutexClose(ILWaitMutex *mutex)
{
	/* Lock down the mutex and determine if it is currently owned */
	_ILMutexLock(&(mutex->parent.lock));
	if(mutex->owner != 0 || !_ILWakeupQueueIsEmpty(&(mutex->queue)))
	{
		_ILMutexUnlock(&(mutex->parent.lock));
		return IL_WAITCLOSE_OWNED;
	}

	/* Clean up the mutex */
	_ILMutexUnlock(&(mutex->parent.lock));
	_ILWakeupQueueDestroy(&(mutex->queue));
	_ILMutexDestroy(&(mutex->parent.lock));
	return IL_WAITCLOSE_FREE;
}

/*
 * Close a named mutex.
 */
static int MutexCloseNamed(ILWaitMutexNamed *mutex)
{
	ILMutexName *temp, *prev;

	/* We need the name list lock */
	_ILMutexLock(&nameListLock);

	/* Lock down the mutex and determine if it is currently owned */
	_ILMutexLock(&(mutex->parent.parent.lock));
	if(mutex->parent.owner != 0 ||
	   !_ILWakeupQueueIsEmpty(&(mutex->parent.queue)))
	{
		_ILMutexUnlock(&(mutex->parent.parent.lock));
		_ILMutexUnlock(&nameListLock);
		return IL_WAITCLOSE_OWNED;
	}

	/* Decrease the number of users and bail out if still non-zero */
	if(--(mutex->numUsers) != 0)
	{
		_ILMutexUnlock(&(mutex->parent.parent.lock));
		_ILMutexUnlock(&nameListLock);
		return IL_WAITCLOSE_DONT_FREE;
	}

	/* Clean up the mutex */
	_ILMutexUnlock(&(mutex->parent.parent.lock));
	_ILWakeupQueueDestroy(&(mutex->parent.queue));
	_ILMutexDestroy(&(mutex->parent.parent.lock));

	/* Remove the mutex from the name list */
	prev = 0;
	temp = nameList;
	while(temp != 0 && temp->handle != (ILWaitHandle *)mutex)
	{
		prev = temp;
		temp = temp->next;
	}
	if(temp != 0)
	{
		if(prev)
		{
			prev->next = temp->next;
		}
		else
		{
			nameList = temp->next;
		}
		ILFree(temp);
	}
	_ILMutexUnlock(&nameListLock);

	/* The wait handle object is now completely free */
	return IL_WAITCLOSE_FREE;
}

/*
 * Register a wakeup object with a wait mutex.
 */
static int MutexRegister(ILWaitMutex *mutex, _ILWakeup *wakeup)
{
	int result;

	/* Lock down the mutex */
	_ILMutexLock(&(mutex->parent.lock));

	/* Determine what to do based on the mutex's current state */
	if(mutex->owner == 0)
	{
		/* No one owns the mutex, so grab it for ourselves */
		mutex->owner = wakeup;
		mutex->count = 1;
		result = IL_WAITREG_ACQUIRED;
	}
	else if(mutex->owner == wakeup)
	{
		/* The mutex is already owned by this thread, so re-acquire it */
		++(mutex->count);
		result = IL_WAITREG_ACQUIRED;
	}
	else
	{
		/* Someone else owns the mutex, so add ourselves to the wait queue */
		if(_ILWakeupQueueAdd(&(mutex->queue), wakeup, mutex))
		{
			result = IL_WAITREG_OK;
		}
		else
		{
			result = IL_WAITREG_FAILED;
		}
	}

	/* Unlock the mutex and return */
	_ILMutexUnlock(&(mutex->parent.lock));
	return result;
}

/*
 * Unregister a thread from a wait mutex's queue.
 */
static void MutexUnregister(ILWaitMutex *mutex, _ILWakeup *wakeup, int release)
{
	/* Lock down the mutex */
	_ILMutexLock(&(mutex->parent.lock));

	/* Remove ourselves from the wait queue if we are currently on it */
	_ILWakeupQueueRemove(&(mutex->queue), wakeup);

	/* Release the mutex if requested */
	if(release && mutex->owner == wakeup)
	{
		if(--(mutex->count) == 0)
		{
			mutex->owner = 0;
		}
	}

	/* Unlock the mutex and return */
	_ILMutexUnlock(&(mutex->parent.lock));
}

ILWaitHandle *ILWaitMutexCreate(int initiallyOwned)
{
	ILWaitMutex *mutex;

	/* Allocate memory for the mutex */
	if((mutex = (ILWaitMutex *)ILMalloc(sizeof(ILWaitMutex))) == 0)
	{
		return 0;
	}

	/* Initialize the mutex */
	_ILMutexCreate(&(mutex->parent.lock));
	mutex->parent.kind = IL_WAIT_MUTEX;
	mutex->parent.closeFunc = (ILWaitCloseFunc)MutexClose;
	mutex->parent.registerFunc = (ILWaitRegisterFunc)MutexRegister;
	mutex->parent.unregisterFunc = (ILWaitUnregisterFunc)MutexUnregister;
	if(initiallyOwned)
	{
		mutex->owner = &((ILThreadSelf())->wakeup);
		mutex->count = 1;
	}
	else
	{
		mutex->owner = 0;
		mutex->count = 0;
	}
	_ILWakeupQueueCreate(&(mutex->queue));

	/* Ready to go */
	return &(mutex->parent);
}

/*
 * Initialize the mutex name list.
 */
static void MutexNameListInit(void)
{
	_ILMutexCreate(&nameListLock);
	nameList = 0;
}

ILWaitHandle *ILWaitMutexNamedCreate(const char *name, int initiallyOwned,
									 int *gotOwnership)
{
	ILWaitHandle *handle;
	ILWaitMutexNamed *mutex;
	ILMutexName *temp;
	int owned;

	/* If we don't have a name, then create a regular mutex */
	if(!name)
	{
		handle = ILWaitMutexCreate(initiallyOwned);
		if(gotOwnership)
		{
			*gotOwnership = initiallyOwned;
		}
		return handle;
	}

	/* Search for a mutex with the same name */
	_ILCallOnce(MutexNameListInit);
	_ILMutexLock(&nameListLock);
	temp = nameList;
	while(temp != 0)
	{
		if(!strcmp(temp->name, name))
		{
			/* Increase the usage count on the mutex */
			mutex = (ILWaitMutexNamed *)(temp->handle);
			_ILMutexLock(&(mutex->parent.parent.lock));
			++(mutex->numUsers);
			_ILMutexUnlock(&(mutex->parent.parent.lock));

			/* Unlock the name list */
			_ILMutexUnlock(&nameListLock);

			/* Attempt to acquire ownership of the mutex if requested */
			if(initiallyOwned)
			{
				owned = (ILWaitOne(&(mutex->parent.parent), 0) == 0);
			}
			else
			{
				owned = 0;
			}
			if(gotOwnership)
			{
				*gotOwnership = owned;
			}

			/* Return the mutex to the caller */
			return &(mutex->parent.parent);
		}
		temp = temp->next;
	}

	/* Allocate memory for the mutex */
	if((mutex = (ILWaitMutexNamed *)ILMalloc(sizeof(ILWaitMutexNamed))) == 0)
	{
		_ILMutexUnlock(&nameListLock);
		return 0;
	}

	/* Allocate space for a name entry */
	temp = (ILMutexName *)ILMalloc(sizeof(ILMutexName) + strlen(name));
	if(!temp)
	{
		_ILMutexUnlock(&nameListLock);
		ILFree(mutex);
		return 0;
	}

	/* Initialize the mutex */
	_ILMutexCreate(&(mutex->parent.parent.lock));
	mutex->parent.parent.kind = IL_WAIT_NAMED_MUTEX;
	mutex->parent.parent.closeFunc = (ILWaitCloseFunc)MutexCloseNamed;
	mutex->parent.parent.registerFunc = (ILWaitRegisterFunc)MutexRegister;
	mutex->parent.parent.unregisterFunc = (ILWaitUnregisterFunc)MutexUnregister;
	if(initiallyOwned)
	{
		mutex->parent.owner = &((ILThreadSelf())->wakeup);
		mutex->parent.count = 1;
	}
	else
	{
		mutex->parent.owner = 0;
		mutex->parent.count = 0;
	}
	_ILWakeupQueueCreate(&(mutex->parent.queue));
	mutex->numUsers = 1;
	if(gotOwnership)
	{
		*gotOwnership = initiallyOwned;
	}

	/* Add the mutex to the name list */
	temp->next = nameList;
	temp->handle = &(mutex->parent.parent);
	strcpy(temp->name, name);
	nameList = temp;
	_ILMutexUnlock(&nameListLock);

	/* Ready to go */
	return &(mutex->parent.parent);
}

int ILWaitMutexRelease(ILWaitHandle *handle)
{
	ILWaitMutex *mutex = (ILWaitMutex *)handle;
	int result;

	/* Lock down the mutex */
	_ILMutexLock(&(mutex->parent.lock));

	/* Determine what to do based on the mutex's state */
	if((mutex->parent.kind & IL_WAIT_MUTEX) == 0)
	{
		/* This isn't actually a mutex */
		result = 0;
	}
	else if(mutex->owner != &((ILThreadSelf())->wakeup))
	{
		/* This thread doesn't currently own the mutex */
		result = 0;
	}
	else if(--(mutex->count) == 0)
	{
		/* The count has returned to zero, so find something
		   else to give the ownership of the mutex to */
		mutex->owner = _ILWakeupQueueWake(&(mutex->queue));
		if(mutex->owner != 0)
		{
			mutex->count = 1;
		}
		result = 1;
	}
	else
	{
		/* The current thread still owns the mutex */
		result = 1;
	}

	/* Unlock the mutex and return */
	_ILMutexUnlock(&(mutex->parent.lock));
	return result;
}

/*
 * Close a monitor.
 */
static int MonitorClose(ILMonitor *monitor)
{
	/* Lock down the monitor and determine if it is currently owned */
	_ILMutexLock(&(monitor->parent.parent.lock));
	if(monitor->parent.owner != 0 ||
	   !_ILWakeupQueueIsEmpty(&(monitor->parent.queue)))
	{
		_ILMutexUnlock(&(monitor->parent.parent.lock));
		return IL_WAITCLOSE_OWNED;
	}

	/* Clean up the monitor */
	_ILMutexUnlock(&(monitor->parent.parent.lock));
	_ILWakeupQueueDestroy(&(monitor->parent.queue));
	_ILWakeupQueueDestroy(&(monitor->signalQueue));
	_ILMutexDestroy(&(monitor->parent.parent.lock));
	return IL_WAITCLOSE_FREE;
}

ILWaitHandle *ILWaitMonitorCreate(void)
{
	ILMonitor *monitor;

	/* Allocate memory for the monitor */
	if((monitor = (ILMonitor *)ILMalloc(sizeof(ILMonitor))) == 0)
	{
		return 0;
	}

	/* Initialize the monitor fields */
	_ILMutexCreate(&(monitor->parent.parent.lock));
	monitor->parent.parent.kind = IL_WAIT_MONITOR;
	monitor->parent.parent.closeFunc = (ILWaitCloseFunc)MonitorClose;
	monitor->parent.parent.registerFunc = (ILWaitRegisterFunc)MutexRegister;
	monitor->parent.parent.unregisterFunc =
			(ILWaitUnregisterFunc)MutexUnregister;
	monitor->parent.owner = 0;
	monitor->parent.count = 0;
	_ILWakeupQueueCreate(&(monitor->parent.queue));
	_ILWakeupQueueCreate(&(monitor->signalQueue));

	/* Ready to go */
	return &(monitor->parent.parent);
}

int ILWaitMonitorWait(ILWaitHandle *handle, ILUInt32 timeout)
{
	ILMonitor *monitor = (ILMonitor *)handle;
	_ILWakeup *wakeup = &((ILThreadSelf())->wakeup);
	int result, result2;
	unsigned long saveCount;

	/* Lock down the monitor */
	_ILMutexLock(&(monitor->parent.parent.lock));

	/* Determine what to do based on the monitor's state */
	if(monitor->parent.parent.kind != IL_WAIT_MONITOR)
	{
		/* This isn't actually a monitor */
		result = 0;
	}
	else if(monitor->parent.owner != wakeup)
	{
		/* This thread doesn't currently own the monitor */
		result = 0;
	}
	else
	{
		/* Save the count and reset the monitor to unowned */
		saveCount = monitor->parent.count;
		monitor->parent.owner = 0;
		monitor->parent.count = 0;

		/* Add ourselves to the signal wakeup queue */
		_ILWakeupQueueAdd(&(monitor->signalQueue), wakeup, monitor);

		/* Unlock the monitor */
		_ILMutexUnlock(&(monitor->parent.parent.lock));

		/* Wait until we are signalled */
		if(_ILWakeupSetLimit(wakeup, 1))
		{
			result = _ILWakeupWait(wakeup, timeout, 0);
			if(result < 0)
			{
				result = IL_WAIT_INTERRUPTED;
			}
			else if(result == 0)
			{
				result = IL_WAIT_TIMEOUT;
			}
		}
		else
		{
			result = IL_WAIT_INTERRUPTED;
		}

		/* Wait to re-acquire the monitor */
		result2 = ILWaitOne(handle, IL_WAIT_INFINITE);
		if(result > 0 && result2 < 0)
		{
			result = result2;
		}

		/* Lock down the monitor and set the count back to the right value */
		_ILMutexLock(&(monitor->parent.parent.lock));
		if(monitor->parent.owner == 0)
		{
			monitor->parent.owner = wakeup;
			monitor->parent.count = saveCount;
		}
		else if(monitor->parent.owner == wakeup)
		{
			monitor->parent.count = saveCount;
		}
	}

	/* Unlock the monitor and return */
	_ILMutexUnlock(&(monitor->parent.parent.lock));
	return result;
}

int ILWaitMonitorPulse(ILWaitHandle *handle)
{
	ILMonitor *monitor = (ILMonitor *)handle;
	_ILWakeup *wakeup = &((ILThreadSelf())->wakeup);
	int result;

	/* Lock down the monitor */
	_ILMutexLock(&(monitor->parent.parent.lock));

	/* Determine what to do based on the monitor's state */
	if(monitor->parent.parent.kind != IL_WAIT_MONITOR)
	{
		/* This isn't actually a monitor */
		result = 0;
	}
	else if(monitor->parent.owner != wakeup)
	{
		/* This thread doesn't currently own the monitor */
		result = 0;
	}
	else
	{
		/* Wake up something on the signal queue */
		_ILWakeupQueueWake(&(monitor->signalQueue));
		result = 1;
	}

	/* Unlock the monitor and return */
	_ILMutexUnlock(&(monitor->parent.parent.lock));
	return result;
}

int ILWaitMonitorPulseAll(ILWaitHandle *handle)
{
	ILMonitor *monitor = (ILMonitor *)handle;
	_ILWakeup *wakeup = &((ILThreadSelf())->wakeup);
	int result;

	/* Lock down the monitor */
	_ILMutexLock(&(monitor->parent.parent.lock));

	/* Determine what to do based on the monitor's state */
	if(monitor->parent.parent.kind != IL_WAIT_MONITOR)
	{
		/* This isn't actually a monitor */
		result = 0;
	}
	else if(monitor->parent.owner != wakeup)
	{
		/* This thread doesn't currently own the monitor */
		result = 0;
	}
	else
	{
		/* Wake up everything on the signal queue */
		_ILWakeupQueueWakeAll(&(monitor->signalQueue));
		result = 1;
	}

	/* Unlock the monitor and return */
	_ILMutexUnlock(&(monitor->parent.parent.lock));
	return result;
}

#ifdef	__cplusplus
};
#endif
