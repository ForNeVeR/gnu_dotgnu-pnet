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

#ifdef	__cplusplus 
};
#endif

#endif	/* _IL_THREAD_H */
