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

#include "thr_choose.h"

#ifdef IL_USE_WIN32_THREADS

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Internal information for a thread.
 */
struct _tagILThread
{
	CRITICAL_SECTION			lock;
	HANDLE			  volatile	handle;
	DWORD			  			identifier;
	unsigned int	  volatile	state;
	ILThreadStartFunc volatile	startFunc;
	void *			  volatile	objectArg;
	HANDLE			  volatile	resumeSemaphore;

};

/*
 * Thread states that are used internally.
 */
#define	IL_TS_PUBLIC_FLAGS		0x01FF
#define	IL_TS_SUSPENDED_SELF	0x0200

/*
 * TLS key for associating "ILThread" objects with threads.
 */
static DWORD threadObjectKey;

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

void ILThreadInit(void)
{
	/* Allocate a TLS key for storing thread objects */
	threadObjectKey = TlsAlloc();

	/* Initialize the "main" thread */
	InitializeCriticalSection(&(mainThread.lock));
	mainThread.handle = GetCurrentThread();
	mainThread.identifier = GetCurrentThreadId();
	mainThread.state = IL_TS_RUNNING;
	mainThread.startFunc = 0;
	mainThread.objectArg = 0;
	mainThread.resumeSemaphore = CreateSemaphore(NULL, 0, 10, NULL);
	TlsSetValue(threadObjectKey, &mainThread);

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
	TlsSetValue(threadObjectKey, thread);

	/* The thread is ready and waiting to start */
	SignalObjectAndWait(threadStartupDone, thread->resumeSemaphore,
						INFINITE, FALSE);

	/* Run the thread's start function */
	if(thread->startFunc)
	{
		(*(thread->startFunc))(thread->objectArg);
	}

	/* Mark the thread as stopped */
	EnterCriticalSection(&(thread->lock));
	thread->state |= IL_TS_STOPPED;
	isbg = ((thread->state & IL_TS_BACKGROUND) != 0);
	LeaveCriticalSection(&(thread->lock));

	/* Update the thread counts */
	EnterCriticalSection(&threadLockAll);
	--numThreads;
	if(isbg)
	{
		numBackgroundThreads -= 1;
	}
	LeaveCriticalSection(&threadLockAll);

	/* Exit from the thread */
	return 0;
}

ILThread *ILThreadCreate(ILThreadStartFunc startFunc, void *objectArg)
{
	ILThread *thread;

	/* Lock down the thread subsystem while we do this */
	EnterCriticalSection(&threadLockAll);

	/* Create a new thread block and populate it */
	thread = (ILThread *)ILMalloc(sizeof(ILThread));
	if(!thread)
	{
		LeaveCriticalSection(&threadLockAll);
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
		LeaveCriticalSection(&threadLockAll);
		return 0;
	}

	/* Attempt to create the thread */
	thread->handle = CreateThread(NULL, 0, ThreadStart,
								  thread, 0, &(thread->identifier));
	if(!(thread->handle))
	{
		CloseHandle(thread->resumeSemaphore);
		DeleteCriticalSection(&(thread->lock));
		ILFree(thread);
		LeaveCriticalSection(&threadLockAll);
		return 0;
	}

	/* We have one extra thread in the system */
	++numThreads;

	/* Wait for "ThreadStart" to wait on the "resumeSemaphore" */
	WaitForSingleObject(threadStartupDone, INFINITE);

	/* The thread is ready to go */
	LeaveCriticalSection(&threadLockAll);
	return thread;
}

int ILThreadStart(ILThread *thread)
{
	int result;

	/* Lock down the thread object */
	EnterCriticalSection(&(thread->lock));

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
	LeaveCriticalSection(&(thread->lock));
	return result;
}

ILThread *ILThreadSelf(void)
{
	return (ILThread *)TlsGetValue(threadObjectKey);
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
	EnterCriticalSection(&(thread->lock));

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
		if(thread->handle == GetCurrentThread())
		{
			/* Put ourselves to sleep until "resumeSemaphore" is posted */
			thread->state |= IL_TS_SUSPENDED_SELF;
			LeaveCriticalSection(&(thread->lock));
			WaitForSingleObject(thread->resumeSemaphore, INFINITE);
			return 1;
		}
		else
		{
			/* Suspend the foreign thread directly */
			SuspendThread(thread->handle);
		}
	}

	/* Unlock the thread object and return */
	LeaveCriticalSection(&(thread->lock));
	return result;
}

void ILThreadResume(ILThread *thread)
{
	/* Lock down the thread object */
	EnterCriticalSection(&(thread->lock));

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
	LeaveCriticalSection(&(thread->lock));
}

void ILThreadInterrupt(ILThread *thread)
{
	/* TODO */
}

int ILThreadGetBackground(ILThread *thread)
{
	int flag;

	/* Lock down the thread object */
	EnterCriticalSection(&(thread->lock));

	/* Determine if this is a background thread */
	flag = ((thread->state & IL_TS_BACKGROUND) != 0);

	/* Unlock the thread object and return */
	LeaveCriticalSection(&(thread->lock));
	return flag;
}

void ILThreadSetBackground(ILThread *thread, int flag)
{
	int change = 0;

	/* Lock down the thread object */
	EnterCriticalSection(&(thread->lock));

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
	LeaveCriticalSection(&(thread->lock));

	/* Adjust "numBackgroundThreads" */
	EnterCriticalSection(&threadLockAll);
	numBackgroundThreads += change;
	LeaveCriticalSection(&threadLockAll);
}

int ILThreadGetState(ILThread *thread)
{
	unsigned int state;

	/* Lock down the thread object */
	EnterCriticalSection(&(thread->lock));

	/* Retrieve the current thread state */
	state = thread->state;

	/* Unlock the thread object */
	LeaveCriticalSection(&(thread->lock));

	/* Return the publicly-interesting flags to the caller */
	return (int)(state & IL_TS_PUBLIC_FLAGS);
}

void ILThreadAtomicStart(void)
{
	EnterCriticalSection(&atomicLock);
}

void ILThreadAtomicEnd(void)
{
	LeaveCriticalSection(&atomicLock);
}

void ILThreadMemoryBarrier(void)
{
	/* TODO */

	/* For now, we just acquire and release a mutex, under the
	   assumption that pthreads will do data synchronisation
	   for us as part of the mutex code */
	EnterCriticalSection(&atomicLock);
	LeaveCriticalSection(&atomicLock);
}

#ifdef	__cplusplus
};
#endif

#endif /* IL_USE_WIN32_THREADS */
