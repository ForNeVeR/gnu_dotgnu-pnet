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
static ILWaitHandle *foregroundThreadsFinished;

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
	_ILWakeupQueueCreate(&(mainThread.joinQueue));

	foregroundThreadsFinished = ILWaitEventCreate(1, 1);

	/* Set the thread object for the "main" thread */
	_ILThreadSetSelf(&mainThread);

	/* We have 1 foreground thread in the system at present */
	numThreads = 1;
	numBackgroundThreads = 0;
}

void ThreadDeinit(void)
{
	if (foregroundThreadsFinished != 0)
	{
		ILWaitHandleClose(foregroundThreadsFinished);
	}
}

void ILThreadInit(void)
{
	_ILCallOnce(ThreadInit);
}

void ILThreadDeinit(void)
{
	_ILCallOnce(ThreadDeinit);
}

void _ILThreadRun(ILThread *thread)
{
	int isbg;

	/* If we still have a startup function, then execute it.
	   The field may have been replaced with NULL if the thread
	   was aborted before it even got going */
	if(thread->startFunc)
	{
		(*(thread->startFunc))(thread->objectArg);
	}

	/* Mark the thread as stopped, and wakeup everyone waiting to join */
	_ILMutexLock(&(thread->lock));
	thread->state |= IL_TS_STOPPED;
	isbg = ((thread->state & IL_TS_BACKGROUND) != 0);
	_ILWakeupQueueWakeAll(&(thread->joinQueue));
	_ILMutexUnlock(&(thread->lock));

	/* Update the thread counts */
	_ILMutexLock(&threadLockAll);
	--numThreads;
	if(isbg)
	{
		numBackgroundThreads -= 1;
	}

	/* If there are no more foreground threads (except the main one) then set
		the event that signals that state */
	if(numThreads - numBackgroundThreads == 1)
	{
		ILWaitEventSet(foregroundThreadsFinished);
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
	_ILWakeupQueueCreate(&(thread->joinQueue));
	thread->handle = 0;

	/* Lock out the thread system */
	_ILMutexLock(&threadLockAll);
	
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
		/* Create the new thread */
		if(!_ILThreadCreateSystem(thread))
		{
			result = 0;
		}
		else
		{
			/* Set the thread state to running (0) */
			thread->state &= ~IL_TS_UNSTARTED;
			
			/* If this thread isn't a background thread then unset the
				foregroundThreadsFinished event.
				This occurs here rather than in ILThreadCreate so that unstarted
				threads won't unset the foregroundThreadsFinished event without ever 
				setting it again (which normally happens at the end of _ILThreadRun) */

			if (!ILThreadGetBackground(thread))
			{
				ILWaitEventReset(foregroundThreadsFinished);
			}

			result = 1;
		}
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

	/* Nothing to do if the thread is already stopped */
	iscounted = 0;
	isbg = 0;
	if((thread->state & IL_TS_STOPPED) == 0)
	{
		thread->state |= IL_TS_STOPPED;
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
	_ILWakeupQueueDestroy(&(thread->joinQueue));
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

	/* If there are no more foreground threads (except the main one) then set
		the event that signals that state */
	if (numThreads - numBackgroundThreads == 1)
	{
		ILWaitEventSet(foregroundThreadsFinished);
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
	else if((state & (IL_TS_UNSTARTED | IL_TS_STOPPED)) != 0)
	{
		/* We cannot suspend a thread that was never started
		   in the first place, or is stopped */
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
	if((thread->state & IL_TS_STOPPED) == 0)
	{
		/* Mark the thread as interrupted */
		thread->state |= IL_TS_INTERRUPTED;

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

int ILThreadAbort(ILThread *thread)
{
	ILThread *self = _ILThreadGetSelf();
	int result;

	/* Lock down the thread object */
	_ILMutexLock(&(thread->lock));

	/* Is the given thread the current thread? */
	if(thread == self)
	{
		/* Determine if we've already seen the abort request or not */
		if((thread->state & IL_TS_ABORTED) != 0)
		{
			/* Already aborted */
			result = 0;
		}
			else if((thread->state & IL_TS_ABORT_REQUESTED) != 0)
		{
			/* Abort was requested */
			thread->state &= ~IL_TS_ABORT_REQUESTED;
			thread->state |= IL_TS_ABORTED;
			result = 1;
		}
		else
		{
			/* The thread is not aborting: we were called in error */
			result = 0;
		}
	}
	else if((thread->state & (IL_TS_ABORTED | IL_TS_ABORT_REQUESTED)) != 0)
	{
		/* The thread is already processing an abort or an abort request */
		result = 0;
	}
	else
	{
		/* Mark the thread as needing to be aborted */
		thread->state |= IL_TS_ABORT_REQUESTED;

		/* If the thread is in the "wait/sleep/join" state, then interrupt it */
		if((thread->state & IL_TS_WAIT_SLEEP_JOIN) != 0)
		{
			_ILMutexUnlock(&(thread->lock));
			_ILWakeupInterrupt(&(thread->wakeup));

			return 0;
		}

		/* No need to abort the current thread */
		result = 0;
	}

	/* Unlock the thread object and return */
	_ILMutexUnlock(&(thread->lock));
	return result;
}

int ILThreadIsAborting(void)
{
	ILThread *thread = _ILThreadGetSelf();
	int aborting;

	/* Lock down the thread object */
	_ILMutexLock(&(thread->lock));

	/* Determine if an abort or an abort request is pending on this thread */
	aborting = ((thread->state & (IL_TS_ABORTED | IL_TS_ABORT_REQUESTED)) != 0);

	/* Unlock the thread object and return */
	_ILMutexUnlock(&(thread->lock));
	return aborting;
}

int ILThreadAbortReset(void)
{
	ILThread *thread = _ILThreadGetSelf();
	int result;

	/* Lock down the thread object */
	_ILMutexLock(&(thread->lock));

	/* Reset the "abort" and "abort requested" flags */
	if((thread->state & (IL_TS_ABORTED | IL_TS_ABORT_REQUESTED)) != 0)
	{
		thread->state &= ~(IL_TS_ABORTED | IL_TS_ABORT_REQUESTED);
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

int ILThreadJoin(ILThread *thread, ILUInt32 ms)
{
	ILThread *self = _ILThreadGetSelf();
	int result;

	/* Bail out if we are trying to join with ourselves */
	if(self == thread)
	{
		return IL_JOIN_SELF;
	}

	/* Lock down the thread object */
	_ILMutexLock(&(thread->lock));

	/* Determine what to do based on the thread's state */
	if((thread->state & IL_TS_STOPPED) != 0)
	{
		/* The thread is already stopped, so return immediately */
		result = IL_JOIN_OK;
	}
	else
	{
		/* Add ourselves to the foreign thread's join queue */
		if(!_ILWakeupQueueAdd(&(thread->joinQueue), &(self->wakeup), self))
		{
			result = IL_JOIN_MEMORY;
		}
		else
		{
			/* Unlock the foreign thread */
			_ILMutexUnlock(&(thread->lock));

			/* Put ourselves into the "wait/sleep/join" state */
			_ILMutexLock(&(self->lock));
			if((self->state & (IL_TS_ABORTED | IL_TS_ABORT_REQUESTED)) != 0)
			{
				/* The current thread is aborted */
				_ILMutexUnlock(&(self->lock));
				_ILMutexLock(&(thread->lock));
				_ILWakeupQueueRemove(&(thread->joinQueue), &(self->wakeup));
				_ILMutexUnlock(&(thread->lock));
				return IL_JOIN_ABORTED;
			}
			self->state |= IL_TS_WAIT_SLEEP_JOIN;
			_ILMutexUnlock(&(self->lock));

			/* Wait until we are woken or a timeout occurs */
			if(_ILWakeupSetLimit(&(self->wakeup), 1))
			{
				result = _ILWakeupWait(&(self->wakeup), ms, (void **)0);
			}
			else
			{
				result = -1;
			}
			if(result < 0)
			{
				/* The wakeup was interrupted.  It may be either an
				   "interrupt" or an "abort request".  We assume abort
				   for now until we can inspect "self->state" below */
				result = IL_JOIN_ABORTED;
			}
			else if(result > 0)
			{
				result = IL_JOIN_OK;
			}
			else
			{
				result = IL_JOIN_TIMEOUT;
			}

			/* Remove ourselves from the "wait/sleep/join" state,
			   and check for a pending interrupt */
			_ILMutexLock(&(self->lock));
			if((self->state & IL_TS_INTERRUPTED) != 0)
			{
				result = IL_JOIN_INTERRUPTED;
			}
			self->state &= ~(IL_TS_WAIT_SLEEP_JOIN | IL_TS_INTERRUPTED);
			_ILMutexUnlock(&(self->lock));

			/* Lock down the foreign thread again */
			_ILMutexLock(&(thread->lock));

			/* Remove ourselves from the foreign thread's join queue */
			_ILWakeupQueueRemove(&(thread->joinQueue), &(self->wakeup));
		}
	}

	/* Unlock the thread object and return */
	_ILMutexUnlock(&(thread->lock));
	return result;
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

	/* If there are no more foreground threads (except the main one) then set
		the event that signals that state */
	if (numThreads - numBackgroundThreads == 1)
	{
		ILWaitEventSet(foregroundThreadsFinished);
	}

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
	/* For now, we just acquire and release a mutex, under the
	   assumption that the underlying thread package will do
	   data synchronisation for us as part of the mutex code */
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

	/* Bail out if the current thread is aborted */
	if((thread->state & (IL_TS_ABORTED | IL_TS_ABORT_REQUESTED)) != 0)
	{
		_ILMutexUnlock(&(thread->lock));
		return -1;
	}

	/* Put the thread into the "wait/sleep/join" state */
	thread->state |= IL_TS_WAIT_SLEEP_JOIN;

	/* Unlock the thread to allow others to access it */
	_ILMutexUnlock(&(thread->lock));

	/* Wait on the thread's wakeup object, which will never be signalled,
	   but which may be interrupted or aborted by some other thread */
	if(_ILWakeupSetLimit(&(thread->wakeup), 1))
	{
		result = (_ILWakeupWait(&(thread->wakeup), ms, (void **)0) >= 0);
	}
	else
	{
		result = -1;
	}

	/* Lock down the thread again */
	_ILMutexLock(&(thread->lock));

	/* Determine if the interrupt on the wakeup object was a user-level
	   interrupt or an abort request */
	if(!result && (thread->state & IL_TS_INTERRUPTED) == 0)
	{
		/* It was an abort request */
		result = -1;
	}

	/* Exit from the "wait/sleep/join" and "interrupted" states */
	thread->state &= ~(IL_TS_WAIT_SLEEP_JOIN | IL_TS_INTERRUPTED);

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

void ILThreadWaitForForegroundThreads(int timeout)
{
#ifdef IL_NO_THREADS
	/* Nothing to do */
#else	
	ILWaitOne(foregroundThreadsFinished, timeout);
#endif
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
