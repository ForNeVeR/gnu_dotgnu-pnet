/*
 * il_thread.h - Thread support routines.
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

#ifndef	_IL_THREAD_H
#define	_IL_THREAD_H

#include "il_system.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * States that a thread can be in.  Based on the values
 * in the C# "ThreadState" enumeration.
 */
#define	IL_TS_RUNNING			0x0000
#define	IL_TS_STOP_REQUESTED	0x0001
#define	IL_TS_SUSPEND_REQUESTED	0x0002
#define	IL_TS_BACKGROUND		0x0004
#define	IL_TS_UNSTARTED			0x0008
#define	IL_TS_STOPPED			0x0010
#define	IL_TS_WAIT_SLEEP_JOIN	0x0020
#define	IL_TS_SUSPENDED			0x0040
#define	IL_TS_ABORT_REQUESTED	0x0080
#define	IL_TS_ABORTED			0x0100

/*
 * Type for a thread start function.
 */
typedef void (*ILThreadStartFunc)(void *objectArg);

/*
 * Opaque types for thread-related objects.
 */
typedef struct _tagILThread     ILThread;
typedef struct _tagILMutex      ILMutex;
typedef struct _tagILRWLock     ILRWLock;
typedef struct _tagILWaitHandle ILWaitHandle;

/*
 * Determine if the system has thread support.  This can
 * be called either before or after "ILThreadInit".
 */
int ILHasThreads(void);

/*
 * Initialize the thread support routines.  Only needs to be
 * called once but it is safe to call multiple times.
 */
void ILThreadInit(void);

/*
 * Create a new thread, initially in the "unstarted" state.
 * When the thread is started, it will call "startFunc" with
 * the supplied "objectArg".  Returns NULL if there are
 * insufficient resources to start the thread.
 */
ILThread *ILThreadCreate(ILThreadStartFunc startFunc, void *objectArg);

/*
 * Start a thread running.  Returns zero if not in the
 * correct state to start.
 */
int ILThreadStart(ILThread *thread);

/*
 * Destroy a thread.  This cannot be used to destroy
 * the current thread.
 */
void ILThreadDestroy(ILThread *thread);

/*
 * Get the thread descriptor for the current thread.
 */
ILThread *ILThreadSelf(void);

/*
 * Get the object reference that is associated with a thread.
 */
void *ILThreadGetObject(ILThread *thread);

/*
 * Set the object reference that is associated with a thread.
 * This is typically only required for the "main" thread.
 */
void ILThreadSetObject(ILThread *thread, void *objectArg);

/*
 * Suspend a thread.  Does nothing if the thread is already
 * suspended.  Returns non-zero if OK, or zero if the thread
 * is not in an appropriate state.
 */
int ILThreadSuspend(ILThread *thread);

/*
 * Resume a suspended thread.  Does nothing if the thread
 * is already resumed.
 */
void ILThreadResume(ILThread *thread);

/*
 * Interrupt a thread that is currently in the "wait/sleep/join"
 * thread state.  If the thread is not currently in that state,
 * the request will be queued.
 */
void ILThreadInterrupt(ILThread *thread);

/*
 * Abort a thread.  Returns non-zero if the current thread is
 * being aborted and a previous request hasn't been processed.
 */
int ILThreadAbort(ILThread *thread);

/*
 * Determine if the current thread is in the process of
 * aborting.  If this function returns non-zero, the current
 * thread should call "ILThreadAbort" on itself at some
 * future point to determine if we are processing an abort
 * request, or are in the middle of an abort.  i.e. the
 * correct way to check for aborts is as follows:
 *
 *		if(ILThreadIsAborting())
 *		{
 *			if(ILThreadAbort(ILThreadSelf()))
 *			{
 *				// Abort request received: do initial processing.
 *				...
 *			}
 *			else
 *			{
 *				// We are processing an existing abort.
 *				...
 *			}
 *		}
 */
int ILThreadIsAborting(void);

/*
 * Reset a pending abort on the current thread.  Returns
 * zero if an abort is not pending.
 */
int ILThreadAbortReset(void);

/*
 * Join result codes.
 */
#define	IL_JOIN_TIMEOUT		0	/* Join timed out */
#define	IL_JOIN_OK			1	/* Join was successful */
#define	IL_JOIN_INTERRUPTED	2	/* Join was interrupted */
#define	IL_JOIN_ABORTED		3	/* Thread doing the join was aborted */
#define	IL_JOIN_SELF		4	/* Tried to join with ourselves */
#define	IL_JOIN_MEMORY		5	/* Out of memory */

/*
 * Join with another thread to wait for exit.
 */
int ILThreadJoin(ILThread *thread, ILUInt32 ms);

/*
 * Get the background state of a thread.
 */
int ILThreadGetBackground(ILThread *thread);

/*
 * Set the background state of a thread.
 */
void ILThreadSetBackground(ILThread *thread, int flag);

/*
 * Get the current thread state flags.
 */
int ILThreadGetState(ILThread *thread);

/*
 * Start a critical section for atomic operations.
 * This isn't highly efficient, but it should be
 * safe to use on all platforms.
 */
void ILThreadAtomicStart(void);

/*
 * End a critical section for atomic operations.
 */
void ILThreadAtomicEnd(void);

/*
 * Process a memory barrier within the current thread.
 */
void ILThreadMemoryBarrier(void);

/*
 * Get the number of foreground and background threads
 * that currently exist in the system.
 */
void ILThreadGetCounts(unsigned long *numForeground,
					   unsigned long *numBackground);

/*
 * Put a thread to sleep for a given number of milliseconds.
 * Specifying "ms == 0" is the same as yielding the thread.
 * Specifying "ms == IL_MAX_UINT32" will sleep forever.
 * Returns zero if the sleep was interrupted, -1 if the
 * thread was aborted, and 1 if the sleep completed.
 */
int ILThreadSleep(ILUInt32 ms);

/*
 * Create a mutex.  Note: this type of mutex will not
 * necessarily update the thread's "wait/sleep/join"
 * state, so it isn't directly suitable for emulating
 * Windows-like wait handle mutexes.  It is useful for
 * simple non-recursive mutual exclusion operations
 * that won't otherwise affect the thread's state.
 */
ILMutex *ILMutexCreate(void);

/*
 * Destroy a mutex.
 */
void ILMutexDestroy(ILMutex *mutex);

/*
 * Lock a mutex.
 */
void ILMutexLock(ILMutex *mutex);

/*
 * Unlock a mutex.
 */
void ILMutexUnlock(ILMutex *mutex);

/*
 * Create a read-write lock.  If the system does not
 * support read-write locks, this will act like a mutex.
 * Note: read-write locks, like mutexes, do not necessarily
 * update the thread's "wait/sleep/join" state.
 */
ILRWLock *ILRWLockCreate(void);

/*
 * Destroy a read-write lock.
 */
void ILRWLockDestroy(ILRWLock *rwlock);

/*
 * Lock a read-write lock for reading.
 */
void ILRWLockReadLock(ILRWLock *rwlock);

/*
 * Lock a read-write lock for writing.
 */
void ILRWLockWriteLock(ILRWLock *rwlock);

/*
 * Unlock a read-write lock.
 */
void ILRWLockUnlock(ILRWLock *rwlock);

/*
 * Close a wait handle.  Returns zero if the handle is
 * currently owned by a thread.
 */
int ILWaitHandleClose(ILWaitHandle *handle);

/*
 * Special timeout values.
 */
#define	IL_WAIT_INFINITE		((ILUInt32)IL_MAX_UINT32)

/*
 * Return values for "ILWaitOne", "ILWaitAny", and "ILWaitAll".
 */
#define	IL_WAIT_TIMEOUT			-1
#define	IL_WAIT_FAILED			-2
#define	IL_WAIT_INTERRUPTED		-3
#define	IL_WAIT_ABORTED			-4

/*
 * Wait for one wait handle to become available.  Returns
 * zero if the wait handle was acquired.
 */
int ILWaitOne(ILWaitHandle *handle, ILUInt32 timeout);

/*
 * Wait for any wait handle in a set to become available.
 * Returns the index of the handle that was acquired.
 */
int ILWaitAny(ILWaitHandle **handles, ILUInt32 numHandles, ILUInt32 timeout);

/*
 * Wait for all handles in a set to become available.
 * Returns zero if all wait handles were acquired.
 */
int ILWaitAll(ILWaitHandle **handles, ILUInt32 numHandles, ILUInt32 timeout);

/*
 * Create a wait mutex, which differs from a regular
 * mutex in that it can be used from C# code, and can
 * be held for long periods of time.
 */
ILWaitHandle *ILWaitMutexCreate(int initiallyOwned);

/*
 * Create a named wait mutex, or return an existing named
 * mutex with the same name.
 */
ILWaitHandle *ILWaitMutexNamedCreate(const char *name, int initiallyOwned,
									 int *gotOwnership);

/*
 * Release a wait mutex that is currently held by the
 * current thread.  Returns zero if not held.
 */
int ILWaitMutexRelease(ILWaitHandle *handle);

/*
 * Create a wait handle that corresponds to a monitor.
 */
ILWaitHandle *ILWaitMonitorCreate(void);

/*
 * Wait on a monitor for a "pulse" operation.  Returns zero if we
 * don't own the monitor, 1 if waiting, or an "IL_WAIT_*" error
 * code otherwise.
 */
int ILWaitMonitorWait(ILWaitHandle *handle, ILUInt32 timeout);

/*
 * Pulse a single waiting thread on a monitor.
 */
int ILWaitMonitorPulse(ILWaitHandle *handle);

/*
 * Pulse all waiting threads on a monitor.
 */
int ILWaitMonitorPulseAll(ILWaitHandle *handle);

/*
 * Syntactic sugar.
 */
#define	ILWaitMonitorEnter(monitor)		\
			(ILWaitOne((monitor), IL_WAIT_INFINITE))
#define	ILWaitMonitorTryEnter(monitor,timeout)		\
			(ILWaitOne((monitor), (timeout)))
#define	ILWaitMonitorLeave(monitor)		\
			(ILWaitMutexRelease((monitor)))

/*
 * Wait Event definitions
 *
 * 12-DEC-2002  Thong Nguyen (tum@veridicus.com)
 */

/*
 * Create a wait event.
 */
ILWaitHandle *ILWaitEventCreate(int manualReset, int initialState);

/*
 * Set the event.
 */
int ILWaitEventSet(ILWaitHandle *event);

/*
 * Reset the event.
 */
int ILWaitEventReset(ILWaitHandle *event);

#ifdef	__cplusplus 
};
#endif

#endif	/* _IL_THREAD_H */
