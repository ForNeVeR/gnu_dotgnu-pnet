/*
 * thread.c - Thread management routines.
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
the correct CLI threading semantics based on the primitives in "*_defs.h".
You normally won't need to modify or replace this file when porting.

*/

#include "thr_defs.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Global state that is mutexed between all threads.
 */
static _ILMutex threadLockAll;
static long volatile numThreads;
static long volatile numBackgroundThreads;
static _ILSemaphore startupAck;

/*
 * Global mutex for atomic operations.
 */
static _ILMutex atomicLock;

/*
 * The "main" thread's object.
 */
static ILThread mainThread;

int ILHasThreads(void)
{
	return _ILThreadIsReal;
}

/*
 * Thread library initialization routines that are called once only.
 */
static void ThreadInit(void)
{
	/* Perform system-specific initialization */
	_ILThreadInitSystem(&mainThread);

	/* Initialize synchronization objects that we need */
	_ILMutexCreate(&threadLockAll);
	_ILSemaphoreCreate(&startupAck);
	_ILMutexCreate(&atomicLock);

	/* Set up the "main" thread.  "_ILThreadInitSystem" has already
	   set the "handle" and "identifier" fields for us */
	_ILMutexCreate(&(mainThread.lock));
	mainThread.state            = IL_TS_RUNNING;
	mainThread.resumeRequested  = 0;
	mainThread.suspendRequested = 0;
	mainThread.numLocksHeld     = 0;
	_ILSemaphoreCreate(&(mainThread.resumeAck));
	_ILSemaphoreCreate(&(mainThread.suspendAck));
	mainThread.startFunc        = 0;
	mainThread.objectArg        = 0;
	_ILWakeupCreate(&(mainThread.wakeup));

	/* Set the thread object for the "main" thread */
	_ILThreadSetSelf(&mainThread);

	/* We have 1 foreground thread in the system at present */
	numThreads = 1;
	numBackgroundThreads = 0;
}

void ILThreadInit(void)
{
	_ILCallOnce(ThreadInit);
}

void _ILThreadRun(ILThread *thread)
{
	int isbg;

	/* Signal "ILThreadCreate" that the thread is ready to go */
	_ILSemaphorePost(&startupAck);

	/* Wait until the thread is really started by a call to "ILThreadStart" */
	_ILThreadSuspendSelf(thread);

	/* If we still have a startup function, then execute it.
	   The field may have been replaced with NULL if the thread
	   was aborted before it even got going */
	if(thread->startFunc)
	{
		(*(thread->startFunc))(thread->objectArg);
	}

	/* Mark the thread as stopped */
	_ILMutexLock(&(thread->lock));
	thread->state |= IL_TS_STOPPED;
	isbg = ((thread->state & IL_TS_BACKGROUND) != 0);
	_ILMutexUnlock(&(thread->lock));

	/* Update the thread counts */
	_ILMutexLock(&threadLockAll);
	--numThreads;
	if(isbg)
	{
		numBackgroundThreads -= 1;
	}
	_ILMutexUnlock(&threadLockAll);
}

ILThread *ILThreadCreate(ILThreadStartFunc startFunc, void *objectArg)
{
	ILThread *thread;

	/* We cannot create threads if the system doesn't really support them */
	if(!ILHasThreads())
	{
		return 0;
	}

	/* Create a new thread object and populate it */
	thread = (ILThread *)ILCalloc(1, sizeof(ILThread));
	if(!thread)
	{
		return 0;
	}
	_ILMutexCreate(&(thread->lock));
	thread->state = IL_TS_UNSTARTED;
	thread->resumeRequested = 0;
	thread->suspendRequested = 0;
	thread->numLocksHeld = 0;
	_ILSemaphoreCreate(&(thread->resumeAck));
	_ILSemaphoreCreate(&(thread->suspendAck));
	thread->startFunc = startFunc;
	thread->objectArg = objectArg;
	_ILWakeupCreate(&(thread->wakeup));

	/* Lock out the thread system */
	_ILMutexLock(&threadLockAll);

	/* Create the new thread */
	if(!_ILThreadCreateSystem(thread))
	{
		_ILMutexUnlock(&threadLockAll);
		_ILWakeupDestroy(&(thread->wakeup));
		_ILSemaphoreDestroy(&(thread->suspendAck));
		_ILSemaphoreDestroy(&(thread->resumeAck));
		_ILMutexDestroy(&(thread->lock));
		ILFree(thread);
		return 0;
	}

	/* Wait until the thread startup ack is posted, so that
	   we know that the thread is ready to go */
	_ILSemaphoreWait(&startupAck);

	/* We have one extra thread in the system at present */
	++numThreads;

	/* Unlock the thread system and return */
	_ILMutexUnlock(&threadLockAll);
	return thread;
}

int ILThreadStart(ILThread *thread)
{
	int result;

	/* Lock down the thread object */
	_ILMutexLock(&(thread->lock));

	/* Are we in the correct state to start? */
	if((thread->state & IL_TS_UNSTARTED) != 0)
	{
		/* Resume the thread, which is suspended in "ThreadStart" */
		thread->state &= ~IL_TS_UNSTARTED;
		_ILThreadResumeSelf(thread);
		result = 1;
	}
	else
	{
		result = 0;
	}

	/* Unlock the thread object and return */
	_ILMutexUnlock(&(thread->lock));
	return result;
}

void ILThreadDestroy(ILThread *thread)
{
	int iscounted;
	int isbg;

	/* Bail out if this is the current thread */
	if(thread == _ILThreadGetSelf())
	{
		return;
	}

	/* Lock down the thread object */
	_ILMutexLock(&(thread->lock));

	/* Nothing to do if the thread is already stopped or aborted */
	iscounted = 0;
	isbg = 0;
	if((thread->state & (IL_TS_STOPPED | IL_TS_ABORTED)) == 0)
	{
		thread->state |= IL_TS_ABORTED;
		_ILThreadTerminate(thread);
		iscounted = 1;
		isbg = ((thread->state & IL_TS_BACKGROUND) != 0);
	}

	/* Unlock the thread object and free it */
	_ILMutexUnlock(&(thread->lock));
	_ILThreadDestroy(thread);
	_ILMutexDestroy(&(thread->lock));
	_ILSemaphoreDestroy(&(thread->suspendAck));
	_ILSemaphoreDestroy(&(thread->resumeAck));
	_ILWakeupDestroy(&(thread->wakeup));
	ILFree(thread);

	/* Adjust the thread counts */
	_ILMutexLock(&threadLockAll);
	if(iscounted)
	{
		--numThreads;
	}
	if(isbg)
	{
		--numBackgroundThreads;
	}
	_ILMutexUnlock(&threadLockAll);
}

ILThread *ILThreadSelf(void)
{
#ifdef IL_NO_THREADS
	return &mainThread;
#else
	return _ILThreadGetSelf();
#endif
}

void *ILThreadGetObject(ILThread *thread)
{
	return thread->objectArg;
}

void ILThreadSetObject(ILThread *thread, void *objectArg)
{
	thread->objectArg = objectArg;
}

int ILThreadSuspend(ILThread *thread)
{
	unsigned int state;
	int result = 1;

	/* Lock down the thread object */
	_ILMutexLock(&(thread->lock));

	/* Determine what to do based on the thread's state */
	state = thread->state;
	if((state & IL_TS_SUSPENDED) != 0)
	{
		/* Nothing to do here - it is already suspended */
	}
	else if((state & IL_TS_WAIT_SLEEP_JOIN) != 0)
	{
		/* Request a suspend, but otherwise ignore the request */
		thread->state |= IL_TS_SUSPEND_REQUESTED;
	}
	else if((state & (IL_TS_ABORT_REQUESTED | IL_TS_ABORTED |
					  IL_TS_UNSTARTED | IL_TS_STOPPED)) != 0)
	{
		/* We cannot suspend a thread that is being aborted,
		   was never started in the first place, or is done */
		result = 0;
	}
	else if(_ILThreadIsSelf(thread))
	{
		/* Mark the thread as suspended */
		thread->state |= IL_TS_SUSPENDED | IL_TS_SUSPENDED_SELF;
		thread->resumeRequested = 0;
		
		/* Unlock the thread object prior to suspending */
		_ILMutexUnlock(&(thread->lock));

		/* Suspend until we receive notification from another thread */
		_ILThreadSuspendSelf(thread);

		/* We are resumed, and the thread object is already unlocked */
		return 1;
	}
	else
	{
		/* Mark the thread as suspended and waiting for a resume */
		thread->state |= IL_TS_SUSPENDED;
		thread->resumeRequested = 0;

		/* Put the thread to sleep temporarily */
		_ILThreadSuspendOther(thread);

		/* If the thread does not hold any locks, then everything is OK */
		if(!(thread->numLocksHeld))
		{
			_ILMutexUnlock(&(thread->lock));
			return 1;
		}

		/* Notify the thread that we want it to suspend itself */
		thread->suspendRequested = 1;

		/* Resume the thread to allow it to give up all locks that it has */
		_ILThreadResumeOther(thread);

		/* Give up the lock on the thread, but don't reduce
		   "numLocksHeld" on the current thread just yet */
		_ILMutexUnlockUnsafe(&(thread->lock));

		/* Wait for the thread to signal us that it has put itself to sleep */
		_ILSemaphoreWait(&(thread->suspendAck));

		/* Re-acquire the lock on the thread object */
		_ILMutexLockUnsafe(&(thread->lock));
	}

	/* Unlock the thread object and return */
	_ILMutexUnlock(&(thread->lock));
	return result;
}

void ILThreadResume(ILThread *thread)
{
	unsigned int state;

	/* Lock down the thread object */
	_ILMutexLock(&(thread->lock));

	/* Determine what to do based on the thread's state */
	state = thread->state;
	if((state & IL_TS_SUSPENDED) != 0)
	{
		if((state & IL_TS_SUSPENDED_SELF) != 0)
		{
			/* The thread put itself to sleep */
			thread->state &= ~(IL_TS_SUSPENDED | IL_TS_SUSPENDED_SELF);
			_ILThreadResumeSelf(thread);
		}
		else
		{
			/* Someone else suspended the thread */
			thread->state &= ~IL_TS_SUSPENDED;
			_ILThreadResumeOther(thread);
		}
	}
	else if((state & IL_TS_SUSPEND_REQUESTED) != 0)
	{
		/* A suspend was requested, but it hadn't started yet */
		thread->state &= ~IL_TS_SUSPEND_REQUESTED;
	}

	/* Unlock the thread object */
	_ILMutexUnlock(&(thread->lock));
}

void ILThreadInterrupt(ILThread *thread)
{
	/* Lock down the thread object */
	_ILMutexLock(&(thread->lock));

	/* Determine what to do based on the thread's state */
	if((thread->state & (IL_TS_STOPPED | IL_TS_ABORTED)) == 0)
	{
		/* Unlock the thread object: we never hold the thread
		   lock when updating the thread's wakeup object */
		_ILMutexUnlock(&(thread->lock));

		/* Mark the thread as needing to be interrupted the next
		   time a "wait/sleep/join" occurs */
		_ILWakeupInterrupt(&(thread->wakeup));
	}
	else
	{
		/* Unlock the thread object */
		_ILMutexUnlock(&(thread->lock));
	}
}

int ILThreadGetBackground(ILThread *thread)
{
	int flag;

	/* Lock down the thread object */
	_ILMutexLock(&(thread->lock));

	/* Determine if this is a background thread */
	flag = ((thread->state & IL_TS_BACKGROUND) != 0);

	/* Unlock the thread object and return */
	_ILMutexUnlock(&(thread->lock));
	return flag;
}

void ILThreadSetBackground(ILThread *thread, int flag)
{
	int change = 0;

	/* Lock down the thread object */
	_ILMutexLock(&(thread->lock));

	/* Change the background state of the thread */
	if(flag)
	{
		if((thread->state & IL_TS_BACKGROUND) == 0)
		{
			thread->state |= IL_TS_BACKGROUND;
			change = 1;
		}
	}
	else
	{
		if((thread->state & IL_TS_BACKGROUND) != 0)
		{
			thread->state &= ~IL_TS_BACKGROUND;
			change = -1;
		}
	}

	/* Unlock the thread object */
	_ILMutexUnlock(&(thread->lock));

	/* Adjust "numBackgroundThreads" */
	_ILMutexLock(&threadLockAll);
	numBackgroundThreads += change;
	_ILMutexUnlock(&threadLockAll);
}

int ILThreadGetState(ILThread *thread)
{
	unsigned int state;

	/* Lock down the thread object */
	_ILMutexLock(&(thread->lock));

	/* Retrieve the current thread state */
	state = thread->state;

	/* Unlock the thread object */
	_ILMutexUnlock(&(thread->lock));

	/* Return the publicly-interesting flags to the caller */
	return (int)(state & IL_TS_PUBLIC_FLAGS);
}

void ILThreadAtomicStart(void)
{
	_ILMutexLock(&atomicLock);
}

void ILThreadAtomicEnd(void)
{
	_ILMutexUnlock(&atomicLock);
}

void ILThreadMemoryBarrier(void)
{
	/* TODO */

	/* For now, we just acquire and release a mutex, under the
	   assumption that pthreads will do data synchronisation
	   for us as part of the mutex code */
	_ILMutexLock(&atomicLock);
	_ILMutexUnlock(&atomicLock);
}

void ILThreadGetCounts(unsigned long *numForeground,
					   unsigned long *numBackground)
{
	_ILMutexLock(&threadLockAll);
	*numForeground = (unsigned long)(numThreads - numBackgroundThreads);
	*numBackground = (unsigned long)(numBackgroundThreads);
	_ILMutexUnlock(&threadLockAll);
}

int ILThreadSleep(ILUInt32 ms)
{
	ILThread *thread = _ILThreadGetSelf();
	int result;

	/* Lock down the thread */
	_ILMutexLock(&(thread->lock));

	/* Put the thread into the "wait/sleep/join" state */
	thread->state |= IL_TS_WAIT_SLEEP_JOIN;

	/* Unlock the thread to allow others to access it */
	_ILMutexUnlock(&(thread->lock));

	/* Wait on the thread's wakeup object, which will never be signalled,
	   but which may be interrupted by some other thread */
	result = (_ILWakeupWait(&(thread->wakeup), ms, 1, (void **)0) >= 0);

	/* Lock down the thread again */
	_ILMutexLock(&(thread->lock));

	/* Exit from the "wait/sleep/join" state */
	thread->state &= ~IL_TS_WAIT_SLEEP_JOIN;

	/* Did someone else ask us to suspend? */
	if((thread->state & IL_TS_SUSPEND_REQUESTED) != 0)
	{
		/* Suspend the current thread */
		thread->state &= ~IL_TS_SUSPEND_REQUESTED;
		thread->state |= IL_TS_SUSPENDED | IL_TS_SUSPENDED_SELF;
		thread->resumeRequested = 0;
		
		/* Unlock the thread object prior to suspending */
		_ILMutexUnlock(&(thread->lock));

		/* Suspend until we receive notification from another thread */
		_ILThreadSuspendSelf(thread);

		/* We are resumed, and the thread object is already unlocked */
		return result;
	}

	/* Unlock the thread and exit */
	_ILMutexUnlock(&(thread->lock));
	return result;
}

void _ILThreadSuspendRequest(ILThread *thread)
{
	/* Clear the "suspendRequested" and "resumeRequested" flags */
	thread->suspendRequested = 0;
	thread->resumeRequested = 0;

	/* Signal the thread that wanted to make us suspend that we are */
	_ILSemaphorePost(&(thread->suspendAck));

	/* Suspend the current thread until we receive a resume signal */
	_ILThreadSuspendSelf(thread);
}

#ifdef	__cplusplus
};
#endif
