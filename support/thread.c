/*
 * thread.c - Thread management routines.
 *
 * Copyright (C) 2002  Southern Storm Software, Pty Ltd.
 *
 * Contributions from Thong Nguyen (tum) [tum@veridicus.com]
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
#include "interlocked.h"
#include "interrupt.h"

#ifdef HAVE_ALLOCA_H
#include <alloca.h>
#endif
#ifdef HAVE_MALLOC_H
#include <malloc.h>
#endif

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Global state that is mutexed between all threads.
 */
static _ILMutex threadLockAll;
/* Number of threads that have started and not finished */
static long volatile numThreads;
/* Number of threads that have started and not finished and are background threads */
static long volatile numBackgroundThreads;
/* Event that is set when there are no foreground threads left */
static ILWaitHandle *noFgThreadsEvent;

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
static void _ILThreadInit(void)
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
	mainThread.userObject       = 0;
	mainThread.startArg			= 0;
	mainThread.destroyOnExit	= 0;
	mainThread.monitor = ILWaitMonitorCreate();
	#ifdef IL_INTERRUPT_SUPPORTS
		mainThread.interruptHandler = 0;
	#endif

	_ILWakeupCreate(&(mainThread.wakeup));
	_ILWakeupQueueCreate(&(mainThread.joinQueue));

	noFgThreadsEvent = ILWaitEventCreate(1, 1);

	/* Set the thread object for the "main" thread */
	_ILThreadSetSelf(&mainThread);

	/* We have 1 foreground thread in the system at present */
	numThreads = 1;
	numBackgroundThreads = 0;

	_ILInterruptInit();
}

static void _ILThreadDeinit(void)
{
	_ILInterruptDeinit();

	if(noFgThreadsEvent != 0)
	{
		ILWaitHandleClose(noFgThreadsEvent);
	}	
}

/*
 * Changes the counters for the number of threads.
 */
static void _ILThreadAdjustCount(int numThreadsAdjust, int numBackgroundThreadsAdjust)
{
	_ILMutexLock(&threadLockAll);
	{
		numThreads += numThreadsAdjust;		
		numBackgroundThreads += numBackgroundThreadsAdjust;
		
		/* If there is only the main thread left then signal the
		   noFgThreads event */
		if (numThreads - numBackgroundThreads == 1)
		{
			ILWaitEventSet(noFgThreadsEvent);
		}
		else
		{
			ILWaitEventReset(noFgThreadsEvent);
		}
	}
	_ILMutexUnlock(&threadLockAll);
}

void ILThreadInit(void)
{
	_ILCallOnce(_ILThreadInit);
}

void ILThreadDeinit(void)
{
	_ILCallOnce(_ILThreadDeinit);
}

ILWaitHandle *ILThreadGetMonitor(ILThread *thread)
{
	return thread->monitor;
}

static void _ILThreadRunAndFreeCleanups(ILThread *thread)
{
	ILThreadCleanupEntry *entry, *next;
	
	entry = thread->firstCleanupEntry;

	while (entry)
	{
		/* Call the cleanup function */
		entry->cleanup(thread);
		next = entry->next;

		/* Free the entry */
		ILFree(entry);

		entry = next;
	}

	thread->firstCleanupEntry = 0;
}

static void _ILPrivateThreadDestroy(ILThread *thread, int allowSelf)
{
	/* Bail out if this is the current thread or main thread */
	if((thread == _ILThreadGetSelf() && !allowSelf) || thread == &mainThread)
	{
		return;
	}

	/* Lock down the thread object */
	_ILMutexLock(&(thread->lock));

	/* Don't terminate the thread or adjust counts if it has already been stopped */
	if((thread->state & IL_TS_STOPPED) == 0)
	{
		thread->state |= IL_TS_STOPPED;

		/* Only terminate the system thread if one was created */
		if((thread->state & IL_TS_UNSTARTED) == 0)
		{
			/* Terminating the thread is unsafe so just don't
			   destroy now and tell the thread to destroy itself
			   on exit */

			thread->destroyOnExit = 1;

			_ILMutexUnlock(&thread->lock);
			
			ILThreadAbort(thread);

			return;
		}

		_ILMutexUnlock(&thread->lock);

		/* Run and free the cleanup handlers */
		_ILThreadRunAndFreeCleanups(thread);
	}
	else
	{
		/* Unlock the thread object and free it */
		_ILMutexUnlock(&(thread->lock));
	}

	/* Only destroy the system thread if one was created */
	if((thread->state & IL_TS_UNSTARTED) == 0)
	{
		_ILThreadDestroy(thread);
	}

	ILWaitHandleClose(thread->monitor);
	_ILMutexDestroy(&(thread->lock));
	_ILSemaphoreDestroy(&(thread->suspendAck));
	_ILSemaphoreDestroy(&(thread->resumeAck));	
	_ILWakeupQueueDestroy(&(thread->joinQueue));
	ILFree(thread);
}

void ILThreadDestroy(ILThread *thread)
{
	_ILPrivateThreadDestroy(thread, 0);
}

void _ILThreadRun(ILThread *thread)
{	
	/* When a thread starts, it blocks until the ILThreadStart function
	   has finished setup */
	_ILThreadSuspendSelf(thread);

	/* If we still have a startup function, then execute it.
	   The field may have been replaced with NULL if the thread
	   was aborted before it even got going */
	if(thread->startFunc)
	{
		(*(thread->startFunc))(thread->startArg);
	}

	thread->startArg = 0;
	
	_ILMutexLock(&(thread->lock));
	{
		/* Mark the thread as stopped */
		thread->state |= IL_TS_STOPPED;	
		/* Change the thread count */		
		_ILThreadAdjustCount(-1, ((thread->state & IL_TS_BACKGROUND) != 0) ? -1 : 0);
	}
	_ILMutexUnlock(&(thread->lock));

	/* Run and free the cleanup handlers */
	_ILThreadRunAndFreeCleanups(thread);
	
	_ILMutexLock(&(thread->lock));
	{		
		/* Wakeup everyone waiting to join */
		_ILWakeupQueueWakeAll(&(thread->joinQueue));
	}
	_ILMutexUnlock(&(thread->lock));

	/* The wakeup isn't needed anymore so destroy it now to allow
	   held mutexes to be released */
	_ILWakeupDestroy(&(thread->wakeup));

	if (thread->destroyOnExit)
	{
		_ILPrivateThreadDestroy(thread, 1);
	}
}

ILThread *ILThreadCreate(ILThreadStartFunc startFunc, void *startArg)
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
	thread->firstCleanupEntry = 0;
	thread->lastCleanupEntry = 0;
	thread->monitor = ILWaitMonitorCreate();
	_ILSemaphoreCreate(&(thread->resumeAck));
	_ILSemaphoreCreate(&(thread->suspendAck));
	thread->startFunc = startFunc;
	thread->userObject = 0;
	thread->startArg = startArg;
	_ILWakeupCreate(&(thread->wakeup));
	_ILWakeupQueueCreate(&(thread->joinQueue));
	thread->handle = 0;
	thread->destroyOnExit = 0;
	#ifdef IL_INTERRUPT_SUPPORTS
		thread->interruptHandler = 0;
	#endif

	/* Lock out the thread system */
	_ILMutexLock(&threadLockAll);
	
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
			thread->state |= IL_TS_RUNNING;

			_ILThreadAdjustCount(1, (thread->state & IL_TS_BACKGROUND) ? 1 : 0);

		#if defined(GC_OPENBSD_THREADS)
			pthread_yield();
		#endif

			/* Let the thread start running */
			_ILThreadResumeSelf(thread);

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
	return thread->userObject;
}

void ILThreadSetObject(ILThread *thread, void *userObject)
{
	thread->userObject = userObject;
}

int ILThreadSuspend(ILThread *thread)
{
	return ILThreadSuspendRequest(thread, 0);
}

int ILThreadSuspendRequest(ILThread *thread, int requestOnly)
{
	unsigned int state;
	int result = IL_SUSPEND_OK;

	/* Lock down the thread object */
	_ILMutexLock(&(thread->lock));

	/* Determine what to do based on the thread's state */
	state = thread->state;
	if ((state & (IL_TS_ABORT_REQUESTED)) != 0)
	{
		return IL_WAIT_ABORTED;
	}
	else if((state & IL_TS_SUSPENDED) != 0)
	{
		/* Nothing to do here - it is already suspended */
	}
	else if((state & (IL_TS_UNSTARTED | IL_TS_STOPPED)) != 0)
	{
		/* We cannot suspend a thread that was never started
		   in the first place, or is stopped */
		result = IL_SUSPEND_FAILED;
	}
	else if(((state & IL_TS_WAIT_SLEEP_JOIN) != 0) || requestOnly)
	{
		/* Request a suspend, but otherwise ignore the request */
		thread->state |= IL_TS_SUSPEND_REQUESTED;

		result = IL_SUSPEND_REQUESTED;
	}
	else if(_ILThreadIsSelf(thread))
	{
		/* Mark the thread as suspended */
		thread->state &= ~ IL_TS_SUSPEND_REQUESTED;
		thread->state |= IL_TS_SUSPENDED | IL_TS_SUSPENDED_SELF;
		thread->resumeRequested = 0;
		
		/* Unlock the thread object prior to suspending */
		_ILMutexUnlock(&(thread->lock));

		/* Suspend until we receive notification from another thread */
		_ILThreadSuspendSelf(thread);

		/* We are resumed, and the thread object is already unlocked */
		return IL_SUSPEND_OK;
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
			return IL_SUSPEND_OK;
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
			thread->state |= IL_TS_RUNNING;
			_ILThreadResumeSelf(thread);
		}
		else
		{
			/* Someone else suspended the thread */
			thread->state &= ~IL_TS_SUSPENDED;
			thread->state |= IL_TS_RUNNING;			
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

int ILThreadSelfAborting()
{
	int result;
	ILThread *thread = _ILThreadGetSelf();
	
	_ILMutexLock(&(thread->lock));
	
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
	
	_ILMutexUnlock(&(thread->lock));
	
	return result;
}

int ILThreadAbort(ILThread *thread)
{
	int result;

	/* Lock down the thread object */
	_ILMutexLock(&(thread->lock));

	if((thread->state & (IL_TS_ABORTED | IL_TS_ABORT_REQUESTED)) != 0)
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
			_ILWakeupInterrupt(&(thread->wakeup));

			_ILMutexUnlock(&(thread->lock));
						
			return 0;
		}
		else if (((thread->state & (IL_TS_SUSPENDED_SELF | IL_TS_SUSPENDED))) != 0)
		{
			_ILMutexUnlock(&(thread->lock));

			ILThreadResume(thread);

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

	/* Determine if an abort is in progress on this thread */
	aborting = ((thread->state & (IL_TS_ABORTED)) != 0);

	/* Unlock the thread object and return */
	_ILMutexUnlock(&(thread->lock));
	return aborting;
}

int ILThreadIsAbortRequested(void)
{
	ILThread *thread = _ILThreadGetSelf();
	int abortRequested;

	/* Lock down the thread object */
	_ILMutexLock(&(thread->lock));

	/* Determine if an abort is in progress on this thread */
	abortRequested = ((thread->state & (IL_TS_ABORT_REQUESTED)) != 0);

	/* Unlock the thread object and return */
	_ILMutexUnlock(&(thread->lock));
	return abortRequested;
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
		thread->state &= ~(IL_TS_INTERRUPTED);

		_ILWakeupCancelInterrupt(&thread->wakeup);

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
	else if ((thread->state & IL_TS_UNSTARTED) != 0)
	{
		/* Can't join a thread that hasn't started */
		result = IL_JOIN_UNSTARTED;
	}
	else
	{
		/* Note: We must set our wait limit before adding ourselves to any wait queues.
		    Failure to do so may mean we may miss some signals because they will be 
			unset by the signal invoker (which reads us as having a 0 wait limit).
			In this specific case, the order doesn't actually matter because we have locked 
			the queue owner (the thread) but both operations must be performed before we
			unlock the thread - Tum */

		/* Set our wait limit to 1 */
		if(!_ILWakeupSetLimit(&(self->wakeup), 1))
		{
			result = -1;
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
				if((self->state & (IL_TS_ABORT_REQUESTED)) != 0)
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

				result = _ILWakeupWait(&(self->wakeup), ms, (void **)0);
				
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
				
				/* Check and process any pending suspend request */
				if ((thread->state & IL_TS_SUSPEND_REQUESTED) != 0)
				{
					thread->state &= ~IL_TS_SUSPEND_REQUESTED;
					thread->state |= IL_TS_SUSPENDED | IL_TS_SUSPENDED_SELF;
					thread->resumeRequested = 0;

					/* Unlock the thread object prior to suspending */
					_ILMutexUnlock(&(self->lock));

					/* Lock down the foreign thread again */
					_ILMutexLock(&(thread->lock));

					/* Remove ourselves from the foreign thread's join queue */
					_ILWakeupQueueRemove(&(thread->joinQueue), &(self->wakeup));

					/* Suspend until we receive notification from another thread */
					_ILThreadSuspendSelf(thread);
				}
				else
				{
					_ILMutexUnlock(&(self->lock));

					/* Lock down the foreign thread again */
					_ILMutexLock(&(thread->lock));

					/* Remove ourselves from the foreign thread's join queue */
					_ILWakeupQueueRemove(&(thread->joinQueue), &(self->wakeup));
				}
			}
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
		if(!(thread->state & IL_TS_BACKGROUND))
		{
			thread->state |= IL_TS_BACKGROUND;
			
			if(!(thread->state & (IL_TS_UNSTARTED | IL_TS_STOPPED)))
			{
				_ILThreadAdjustCount(0, 1);
			}
		}
	}
	else
	{
		if((thread->state & IL_TS_BACKGROUND))
		{
			thread->state &= ~IL_TS_BACKGROUND;
			change = -1;

			if(!(thread->state & (IL_TS_UNSTARTED | IL_TS_STOPPED)))
			{
				_ILThreadAdjustCount(0, -1);
			}
		}
	}

	/* Unlock the thread object */
	_ILMutexUnlock(&(thread->lock));
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
	ILInterlockedMemoryBarrier();
}

void ILThreadGetCounts(unsigned long *numForeground,
					   unsigned long *numBackground)
{
	_ILMutexLock(&threadLockAll);
	*numForeground = (unsigned long)(numThreads - numBackgroundThreads);
	*numBackground = (unsigned long)(numBackgroundThreads);
	_ILMutexUnlock(&threadLockAll);
}

void ILThreadYield()
{
	_ILThreadYield();
}

int ILThreadSleep(ILUInt32 ms)
{
	ILThread *thread = _ILThreadGetSelf();
	int result;

	/* Lock down the thread */
	_ILMutexLock(&(thread->lock));

	/* Bail out if the current thread is aborted or interrupted */

	if (thread->state & (IL_TS_ABORT_REQUESTED))
	{
		_ILMutexUnlock(&(thread->lock));
		
		return IL_WAIT_ABORTED;
	}
	else if (thread->state & IL_TS_INTERRUPTED)
	{
		thread->state &= ~(IL_TS_INTERRUPTED);
		
		_ILMutexUnlock(&(thread->lock));

		return IL_WAIT_INTERRUPTED;
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
		
		if (result == 0)
		{
			result = IL_WAIT_TIMEOUT;
		}
	}
	else
	{
		
		result = IL_WAIT_TIMEOUT;
	}

	/* Lock down the thread again */
	_ILMutexLock(&(thread->lock));

	if (thread->state & (IL_TS_ABORT_REQUESTED))
	{
		result = IL_WAIT_ABORTED;
	}
	else if (thread->state & IL_TS_INTERRUPTED)
	{
		result = IL_WAIT_INTERRUPTED;
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
	_ILWaitOneBackupInterruptsAndAborts(noFgThreadsEvent, timeout);
#endif
}

int ILThreadRegisterCleanup(ILThread *thread, ILThreadCleanupFunc func)
{
	ILThreadCleanupEntry *entry;

	_ILMutexLock(&(thread->lock));

	if((thread->state & IL_TS_STOPPED))
	{
		/* Thread has stopped */

		_ILMutexUnlock(&(thread->lock));

		return -1;
	}

	entry = thread->firstCleanupEntry;

	while(entry)
	{
		if(entry->cleanup == func)
		{
			/* Function already registered */

			_ILMutexUnlock(&(thread->lock));

			return -1;
		}
	}

	if((entry = (ILThreadCleanupEntry *)ILMalloc(sizeof(ILThreadCleanupEntry))) == 0)
	{
		/* Out of memory */

		_ILMutexUnlock(&(thread->lock));

		return -1;
	}

	entry->cleanup = func;
	entry->next = 0;

	/* Add the entry to the end up the cleanup list */

	if(thread->lastCleanupEntry)
	{
		thread->lastCleanupEntry->next = entry;
		thread->lastCleanupEntry = entry;
	}
	else
	{
		thread->firstCleanupEntry = thread->lastCleanupEntry = entry;
	}

	_ILMutexUnlock(&(thread->lock));
	
	return 0;
}

int ILThreadUnregisterCleanup(ILThread *thread, ILThreadCleanupFunc func)
{
	ILThreadCleanupEntry *entry, *prev;

	_ILMutexLock(&(thread->lock));

	if((thread->state & IL_TS_STOPPED))
	{
		/* Thread has stopped */

		_ILMutexUnlock(&(thread->lock));

		return -1;
	}

	/* Walk the list and remove the cleanup function */

	prev = 0;
	entry = thread->firstCleanupEntry;

	while(entry)
	{
		if(entry->cleanup == func)
		{
			/* Remove the entry from the list */

			if(prev)
			{
				/* Entry is in the tail of the list */

				prev->next = entry->next;

				if(prev->next == 0)
				{
					thread->lastCleanupEntry = prev;
				}

				ILFree(entry);
			}
			else
			{
				/* Entry is in the head of the list */

				thread->firstCleanupEntry = entry->next;
				
				if(thread->firstCleanupEntry == 0)
				{
					thread->lastCleanupEntry = 0;
				}
			}

			_ILMutexUnlock(&(thread->lock));

			/* Found and removed */

			return 0;
		}
	}

	_ILMutexUnlock(&(thread->lock));

	/* Not found */

	return -1;
}

void ILThreadSetPriority(ILThread *thread, int priority)
{
	_ILThreadSetPriority(thread, priority);
}

int ILThreadGetPriority(ILThread *thread)
{
	return _ILThreadGetPriority(thread);
}

void _ILThreadSuspendRequest(ILThread *thread)
{
	_ILMutexLock(&(thread->lock));

	/* Clear the "suspendRequested" and "resumeRequested" flags */
	thread->state |= IL_TS_SUSPENDED | IL_TS_SUSPENDED_SELF;
	thread->suspendRequested = 0;
	thread->resumeRequested = 0;

	/* Signal the thread that wanted to make us suspend that we are */
	_ILSemaphorePost(&(thread->suspendAck));

	_ILMutexUnlock(&(thread->lock));

	/* Suspend the current thread until we receive a resume signal */
	_ILThreadSuspendSelf(thread);
}

/*
 * Clear stack space.
 */
void ILThreadClearStack(int length)
{
	char *ptr;

#ifdef HAVE_ALLOCA
#ifdef _MSC_VER
	ptr = _alloca(length);
#else
	ptr = alloca(length);
#endif
#else
	char stackBuffer[4096];
	length = 4096;
	ptr = &stackBuffer[0];
#endif

	if (ptr)
	{
		ILMemZero(ptr, length);
	}
}

#ifdef	__cplusplus
};
#endif
