/*
 * w32_thread.c - Thread management routines for Win32 systems.
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

#include "w32_include.h"

#ifdef IL_USE_WIN32_THREADS

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * TLS key for associating "ILThread" objects with threads.
 */
DWORD _ILThreadObjectKey;

/*
 * Thread object that is used for the "main" thread.
 */
static ILThread mainThread;

/*
 * Control logic for creating a thread.
 */
static CRITICAL_SECTION threadLockAll;
static HANDLE volatile threadStartupDone;

/*
 * Total number of threads and background threads.
 */
static long volatile numThreads = 0;
static long volatile numBackgroundThreads = 0;

/*
 * A critical section that is used for atomic locking.
 */
static CRITICAL_SECTION atomicLock;

int ILHasThreads(void)
{
	return 1;
}

void ILThreadInit(void)
{
	static int volatile initialized = 0;

	/* Bail out if already initialized */
	if(initialized)
	{
		return;
	}
	initialized = 1;

	/* Allocate a TLS key for storing thread objects */
	_ILThreadObjectKey = TlsAlloc();

	/* Initialize the "main" thread.  We have to duplicate the
	   thread handle because "GetCurrentThread()" returns a
	   pseudo-handle and not a real one.  We need the real one */
	InitializeCriticalSection(&(mainThread.lock));
	DuplicateHandle(GetCurrentProcess(), GetCurrentThread(),
					GetCurrentProcess(), (HANDLE *)(&(mainThread.handle)),
					0, 0, DUPLICATE_SAME_ACCESS);
	mainThread.identifier = GetCurrentThreadId();
	mainThread.state = IL_TS_RUNNING;
	mainThread.startFunc = 0;
	mainThread.objectArg = 0;
	mainThread.resumeSemaphore = CreateSemaphore(NULL, 0, 10, NULL);
	mainThread.suspendSemaphore = CreateSemaphore(NULL, 0, 10, NULL);
	mainThread.numCriticalsHeld = 0;
	TlsSetValue(_ILThreadObjectKey, &mainThread);

	/* We have one foreground thread in the system at present */
	numThreads = 1;
	numBackgroundThreads = 0;

	/* Allocate objects for controlling thread startup */
	InitializeCriticalSection(&threadLockAll);
	threadStartupDone = CreateSemaphore(NULL, 0, 10, NULL);

	/* Allocate the atomic lock */
	InitializeCriticalSection(&atomicLock);
}

/*
 * Main Win32 entry point for a thread.
 */
static DWORD WINAPI ThreadStart(LPVOID arg)
{
	ILThread *thread = (ILThread *)arg;
	int isbg;

	/* Attach the thread object to the thread */
	TlsSetValue(_ILThreadObjectKey, thread);

	/* The thread is ready and waiting to start */
	SignalObjectAndWait(threadStartupDone, thread->resumeSemaphore,
						INFINITE, FALSE);

	/* Run the thread's start function */
	if(thread->startFunc)
	{
		(*(thread->startFunc))(thread->objectArg);
	}

	/* Mark the thread as stopped */
	EnterCriticalSectionSafe(&(thread->lock));
	thread->state |= IL_TS_STOPPED;
	isbg = ((thread->state & IL_TS_BACKGROUND) != 0);
	LeaveCriticalSectionSafe(&(thread->lock));

	/* Update the thread counts */
	EnterCriticalSectionSafe(&threadLockAll);
	--numThreads;
	if(isbg)
	{
		numBackgroundThreads -= 1;
	}
	LeaveCriticalSectionSafe(&threadLockAll);

	/* Exit from the thread */
	return 0;
}

ILThread *ILThreadCreate(ILThreadStartFunc startFunc, void *objectArg)
{
	ILThread *thread;

	/* Lock down the thread subsystem while we do this */
	EnterCriticalSectionSafe(&threadLockAll);

	/* Create a new thread block and populate it */
	thread = (ILThread *)ILMalloc(sizeof(ILThread));
	if(!thread)
	{
		LeaveCriticalSectionSafe(&threadLockAll);
		return 0;
	}
	InitializeCriticalSection(&(thread->lock));
	thread->handle = NULL;
	thread->identifier = 0;
	thread->state = IL_TS_UNSTARTED;
	thread->startFunc = startFunc;
	thread->objectArg = objectArg;
	thread->resumeSemaphore = CreateSemaphore(NULL, 0, 10, NULL);
	if(!(thread->resumeSemaphore))
	{
		DeleteCriticalSection(&(thread->lock));
		ILFree(thread);
		LeaveCriticalSectionSafe(&threadLockAll);
		return 0;
	}
	thread->suspendSemaphore = CreateSemaphore(NULL, 0, 10, NULL);
	if(!(thread->suspendSemaphore))
	{
		CloseHandle(thread->resumeSemaphore);
		DeleteCriticalSection(&(thread->lock));
		ILFree(thread);
		LeaveCriticalSectionSafe(&threadLockAll);
		return 0;
	}
	thread->numCriticalsHeld = 0;

	/* Attempt to create the thread */
	thread->handle = CreateThread(NULL, 0, ThreadStart,
								  thread, 0, (DWORD *)&(thread->identifier));
	if(!(thread->handle))
	{
		CloseHandle(thread->suspendSemaphore);
		CloseHandle(thread->resumeSemaphore);
		DeleteCriticalSection(&(thread->lock));
		ILFree(thread);
		LeaveCriticalSectionSafe(&threadLockAll);
		return 0;
	}

	/* We have one extra thread in the system */
	++numThreads;

	/* Wait for "ThreadStart" to wait on the "resumeSemaphore" */
	WaitForSingleObject(threadStartupDone, INFINITE);

	/* The thread is ready to go */
	LeaveCriticalSectionSafe(&threadLockAll);
	return thread;
}

int ILThreadStart(ILThread *thread)
{
	int result;

	/* Lock down the thread object */
	EnterCriticalSectionSafe(&(thread->lock));

	/* Are we in the correct state to start? */
	if((thread->state & IL_TS_UNSTARTED) != 0)
	{
		/* Resume the thread, which is suspended in "ThreadStart" */
		thread->state &= ~IL_TS_UNSTARTED;
		ReleaseSemaphore(thread->resumeSemaphore, 1, NULL);
		result = 1;
	}
	else
	{
		result = 0;
	}

	/* Unlock the thread object and return */
	LeaveCriticalSectionSafe(&(thread->lock));
	return result;
}

void ILThreadDestroy(ILThread *thread)
{
	int iscounted;
	int isbg;

	/* Bail out if this is the current thread */
	if(thread == ILThreadSelf())
	{
		return;
	}

	/* Lock down the thread object */
	EnterCriticalSectionSafe(&(thread->lock));

	/* Nothing to do if the thread is already stopped or aborted */
	iscounted = 0;
	isbg = 0;
	if((thread->state & (IL_TS_STOPPED | IL_TS_ABORTED)) == 0)
	{
		thread->state |= IL_TS_ABORTED;
		TerminateThread(thread->handle, 0);
		iscounted = 1;
		isbg = ((thread->state & IL_TS_BACKGROUND) != 0);
	}

	/* Free the system objects that are associated with the thread */
	CloseHandle(thread->handle);
	CloseHandle(thread->resumeSemaphore);
	CloseHandle(thread->suspendSemaphore);

	/* Unlock the thread object, free it, and return */
	LeaveCriticalSectionSafe(&(thread->lock));
	DeleteCriticalSection(&(thread->lock));
	ILFree(thread);

	/* Adjust the thread counts */
	EnterCriticalSectionSafe(&threadLockAll);
	if(iscounted)
	{
		--numThreads;
	}
	if(isbg)
	{
		--numBackgroundThreads;
	}
	LeaveCriticalSectionSafe(&threadLockAll);
}

ILThread *ILThreadSelf(void)
{
	return (ILThread *)TlsGetValue(_ILThreadObjectKey);
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
	EnterCriticalSectionSafe(&(thread->lock));

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
	else
	{
		/* Mark the thread as suspended and waiting for a resume */
		thread->state |= IL_TS_SUSPENDED;

		/* Are we suspending ourselves? */
		if(thread->identifier == GetCurrentThreadId())
		{
			/* Put ourselves to sleep until "resumeSemaphore" is posted */
			thread->state |= IL_TS_SUSPENDED_SELF;
			LeaveCriticalSectionSafe(&(thread->lock));
			WaitForSingleObject(thread->resumeSemaphore, INFINITE);
			return 1;
		}
		else
		{
			/* Suspend the foreign thread directly.  Use the "Safe"
			   version to make sure that it isn't holding any critical
			   sections when we finally shut it down */
			SuspendThreadSafe(thread);
		}
	}

	/* Unlock the thread object and return */
	LeaveCriticalSectionSafe(&(thread->lock));
	return result;
}

void ILThreadResume(ILThread *thread)
{
	/* Lock down the thread object */
	EnterCriticalSectionSafe(&(thread->lock));

	/* How was the thread actually suspended? */
	if((thread->state & IL_TS_SUSPENDED) != 0)
	{
		if((thread->state & IL_TS_SUSPENDED_SELF) != 0)
		{
			/* The thread put itself to sleep */
			thread->state &= ~(IL_TS_SUSPENDED | IL_TS_SUSPENDED_SELF);
			ReleaseSemaphore(thread->resumeSemaphore, 1, NULL);
		}
		else
		{
			/* The thread was put to sleep by someone else */
			thread->state &= ~IL_TS_SUSPENDED;
			ResumeThread(thread->handle);
		}
	}

	/* Unlock the thread object */
	LeaveCriticalSectionSafe(&(thread->lock));
}

void ILThreadInterrupt(ILThread *thread)
{
	/* TODO */
}

int ILThreadGetBackground(ILThread *thread)
{
	int flag;

	/* Lock down the thread object */
	EnterCriticalSectionSafe(&(thread->lock));

	/* Determine if this is a background thread */
	flag = ((thread->state & IL_TS_BACKGROUND) != 0);

	/* Unlock the thread object and return */
	LeaveCriticalSectionSafe(&(thread->lock));
	return flag;
}

void ILThreadSetBackground(ILThread *thread, int flag)
{
	int change = 0;

	/* Lock down the thread object */
	EnterCriticalSectionSafe(&(thread->lock));

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
	LeaveCriticalSectionSafe(&(thread->lock));

	/* Adjust "numBackgroundThreads" */
	EnterCriticalSectionSafe(&threadLockAll);
	numBackgroundThreads += change;
	LeaveCriticalSectionSafe(&threadLockAll);
}

int ILThreadGetState(ILThread *thread)
{
	unsigned int state;

	/* Lock down the thread object */
	EnterCriticalSectionSafe(&(thread->lock));

	/* Retrieve the current thread state */
	state = thread->state;

	/* Unlock the thread object */
	LeaveCriticalSectionSafe(&(thread->lock));

	/* Return the publicly-interesting flags to the caller */
	return (int)(state & IL_TS_PUBLIC_FLAGS);
}

void ILThreadAtomicStart(void)
{
	EnterCriticalSectionSafe(&atomicLock);
}

void ILThreadAtomicEnd(void)
{
	LeaveCriticalSectionSafe(&atomicLock);
}

void ILThreadMemoryBarrier(void)
{
	/* TODO */

	/* For now, we just acquire and release a mutex, under the
	   assumption that pthreads will do data synchronisation
	   for us as part of the mutex code */
	EnterCriticalSectionSafe(&atomicLock);
	LeaveCriticalSectionSafe(&atomicLock);
}

void ILThreadGetCounts(unsigned long *numForeground,
					   unsigned long *numBackground)
{
	EnterCriticalSectionSafe(&threadLockAll);
	*numForeground = (unsigned long)(numThreads - numBackgroundThreads);
	*numBackground = (unsigned long)(numBackgroundThreads);
	LeaveCriticalSectionSafe(&threadLockAll);
}

void _ILThreadSuspendRequest(ILThread *thread)
{
	/* Clear the "suspendRequested" flag */
	InterlockedExchange((PLONG)&(thread->suspendRequested), 0);

	/* Put ourselves to sleep until "resumeSemaphore" is posted */
	SignalObjectAndWait(thread->suspendSemaphore, thread->resumeSemaphore,
						INFINITE, FALSE);
}

void _ILThreadHardSuspend(ILThread *thread)
{
	/* Note: on entry to this function the thread has been temporarily
	   suspended with "SuspendThread" */

	/* Remember that the thread will be suspending itself */
	thread->state |= IL_TS_SUSPENDED_SELF;

	/* Notify the thread that we want it to suspend itself */
	InterlockedExchange((PLONG)&(thread->suspendRequested), 1);

	/* Resume the thread to allow it to give up all critical sections it has */
	ResumeThread(thread->handle);

	/* Give up the lock on the thread, but don't reduce our
	   "numCriticalsHeld" value just yet */
	LeaveCriticalSection(&(thread->lock));

	/* Wait for the thread to signal us that it has put itself to sleep */
	WaitForSingleObject(thread->suspendSemaphore, INFINITE);

	/* Re-enter the critical section on the thread object */
	EnterCriticalSection(&(thread->lock));
}

#ifdef	__cplusplus
};
#endif

#endif /* IL_USE_WIN32_THREADS */
