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
	/* Lock down the mutex and determine if it is currently owned */
	_ILMutexLock(&(mutex->parent.parent.lock));
	if(mutex->parent.owner != 0 ||
	   !_ILWakeupQueueIsEmpty(&(mutex->parent.queue)))
	{
		_ILMutexUnlock(&(mutex->parent.parent.lock));
		return IL_WAITCLOSE_OWNED;
	}

	/* Decrease the number of users and bail out if still non-zero */
	if(--(mutex->numUsers) != 0)
	{
		_ILMutexUnlock(&(mutex->parent.parent.lock));
		return IL_WAITCLOSE_DONT_FREE;
	}

	/* Clean up the mutex */
	_ILMutexUnlock(&(mutex->parent.parent.lock));
	_ILWakeupQueueDestroy(&(mutex->parent.queue));
	_ILMutexDestroy(&(mutex->parent.parent.lock));

	/* TODO: remove the mutex from the name list */
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

ILWaitHandle *ILWaitMutexNamedCreate(const char *name, int initiallyOwned,
									 int *gotOwnership)
{
	ILWaitMutexNamed *mutex;

	/* TODO: search for a mutex with the same name */

	/* Allocate memory for the mutex */
	if((mutex = (ILWaitMutexNamed *)ILMalloc(sizeof(ILWaitMutexNamed))) == 0)
	{
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

	/* TODO: add the mutex to the name list */

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

#ifdef	__cplusplus
};
#endif
