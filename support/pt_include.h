/*
 * pt_include.h - Include the necessary headers for pthreads.
 *
 * Copyright (C) 2001  Southern Storm Software, Pty Ltd.
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

#ifndef	_PT_INCLUDE_H
#define	_PT_INCLUDE_H

#include "thr_choose.h"

#ifdef IL_USE_PTHREADS

#include <semaphore.h>
#include <signal.h>

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Extra thread state flags and masks that are used internally.
 */
#define	IL_TS_PUBLIC_FLAGS		0x01FF
#define	IL_TS_INTERRUPTED		0x0200

/*
 * Thread wakeup types.
 */
#define	IL_WAKEUP_NONE			0
#define	IL_WAKEUP_ONCE			1
#define	IL_WAKEUP_ALL			2

/*
 * Determine the minimum and maximum real-time signal numbers.
 */
#if !defined(__SIGRTMIN) && defined(SIGRTMIN)
#define	__SIGRTMIN			SIGRTMIN
#endif
#if !defined(__SIGRTMAX) && defined(SIGRTMAX)
#define	__SIGRTMAX			SIGRTMAX
#endif

/*
 * Signals that are used to bang threads on the head and notify
 * them of various conditions.  Finding a free signal is a bit
 * of a pain as most of the obvious candidates are already in
 * use by pthreads or libgc.  Unix needs more signals!
 */
#if !defined(__SIGRTMIN) || (__SIGRTMAX - __SIGRTMIN < 14)
#define	IL_SIG_SUSPEND		SIGALRM
#define	IL_SIG_RESUME		SIGVTALRM
#define	IL_SIG_INTERRUPT	SIGFPE
#else
#define	IL_SIG_SUSPEND		(__SIGRTMIN+10)
#define	IL_SIG_RESUME		(__SIGRTMIN+11)
#define	IL_SIG_INTERRUPT	(__SIGRTMIN+12)
#endif

/*
 * Signals that are used inside pthreads itself.  This is a bit of
 * a hack, but there isn't any easy way to get this information.
 */
#if !defined(__SIGRTMIN) || (__SIGRTMAX - __SIGRTMIN < 3)
#define PTHREAD_SIG_RESTART	SIGUSR1
#define PTHREAD_PSIG_CANCEL	SIGUSR2
#else
#define PTHREAD_SIG_RESTART	(__SIGRTMIN)
#define PTHREAD_SIG_CANCEL	(__SIGRTMIN+1)
#endif

/*
 * Internal structure of a thread descriptor.
 */
struct _tagILThread
{
	pthread_mutex_t				lock;
	pthread_t					threadId;
	unsigned short    volatile	state;
	unsigned char     volatile	resumeRequested;
	unsigned char     volatile	suspendRequested;
	unsigned int	  volatile  numLocksHeld;
	sem_t						suspendAck;
	ILThreadStartFunc volatile	startFunc;
	void *            volatile	objectArg;
	pthread_mutex_t				wakeupMutex;
	int				  volatile	wakeupType;
	sem_t						wakeup;

};

/*
 * Thread-specific key that is used to store the thread object.
 */
extern pthread_key_t _ILThreadObjectKey;

/*
 * Safe mutex lock, that keeps track of how many locks
 * are held by a thread.  Threads cannot be suspended if
 * they are currently holding a lock.
 */
#define	MutexLockSafe(mutex)	\
			do { \
				ILThread *__self = \
					(ILThread *)(pthread_getspecific(_ILThreadObjectKey)); \
				++(__self->numLocksHeld); \
				pthread_mutex_lock((mutex)); \
			} while (0)

/*
 * Safe mutex unlock that will put the current thread
 * to sleep if a suspend request is pending.
 */
#define	MutexUnlockSafe(mutex)	\
			do { \
				ILThread *__self = \
					(ILThread *)(pthread_getspecific(_ILThreadObjectKey)); \
				pthread_mutex_unlock((mutex)); \
				if(--(__self->numLocksHeld) == 0) \
				{ \
					if(__self->suspendRequested) \
					{ \
						_ILThreadSuspendRequest(__self); \
					} \
				} \
			} while (0)

/*
 * Safe read lock acquisition, similar to "MutexLockSafe".
 */
#define	ReadLockSafe(rwlock)	\
			do { \
				ILThread *__self = \
					(ILThread *)(pthread_getspecific(_ILThreadObjectKey)); \
				++(__self->numLocksHeld); \
				pthread_rwlock_rdlock((rwlock)); \
			} while (0)

/*
 * Safe write lock acquisition, similar to "MutexLockSafe".
 */
#define	WriteLockSafe(rwlock)	\
			do { \
				ILThread *__self = \
					(ILThread *)(pthread_getspecific(_ILThreadObjectKey)); \
				++(__self->numLocksHeld); \
				pthread_rwlock_wrlock((rwlock)); \
			} while (0)

/*
 * Safe read/write unlock, similar to "MutexUnlockSafe".
 */
#define	RWUnlockSafe(rwlock)	\
			do { \
				ILThread *__self = \
					(ILThread *)(pthread_getspecific(_ILThreadObjectKey)); \
				pthread_rwlock_unlock((rwlock)); \
				if(--(__self->numLocksHeld) == 0) \
				{ \
					if(__self->suspendRequested) \
					{ \
						_ILThreadSuspendRequest(__self); \
					} \
				} \
			} while (0)

/*
 * Safe way to suspend a thread, which will wait for the
 * thread to give up ownership of all locks.  It is assumed
 * that "thread" is not the current thread, and that the
 * current thread holds "thread->lock".
 *
 * This function first does a "trial suspend" to stop the thread
 * running.  Then it determines if the thread is in possession
 * of one or more locks.  If it is, then it will mark the thread
 * as needing to be suspended, resume it, and then wait for
 * "MutexUnlockSafe" or "RWUnlockSafe" to do the real suspend.
 */
void _ILThreadSafeSuspend(ILThread *thread);

/*
 * Process a request to suspend the current thread.
 */
void _ILThreadSuspendRequest(ILThread *thread);

/*
 * Internal structure of a FIFO wait queue.
 */
typedef struct _tagILWaitItem  ILWaitItem;
typedef struct _tagILWaitQueue ILWaitQueue;
struct _tagILWaitItem
{
	ILThread   * volatile thread;
	ILWaitItem * volatile next;
};
struct _tagILWaitQueue
{
	ILWaitItem * volatile first;
	ILWaitItem * volatile last;
	ILWaitItem			  space;
	int          volatile spaceUsed;
};

/*
 * Initialize a wait queue.
 */
void _ILWaitQueueInit(ILWaitQueue *queue);

/*
 * Add a thread to a wait queue.  Returns zero if out of memory.
 */
int _ILWaitQueueAdd(ILWaitQueue *queue, ILThread *thread);

/*
 * Remove a thread from a wait queue.
 */
void _ILWaitQueueRemove(ILWaitQueue *queue, ILThread *thread);

/*
 * Get the first thread on a wait queue.  Returns NULL if empty.
 */
ILThread *_ILWaitQueueFirst(ILWaitQueue *queue);

/*
 * Function prototype for destroying a wait handle.  Returns
 * zero if there are threads waiting on the handle.
 */
typedef int (*ILWaitHandleDestroyFunc)(ILWaitHandle *handle);

/*
 * Return values for "ILWaitHandleRegister" functions.
 */
#define	IL_WAITREG_FAILED		0
#define	IL_WAITREG_OK			1
#define	IL_WAITREG_ACQUIRED		2

/*
 * Function prototype for registering a thread with a wait handle.
 */
typedef int (*ILWaitHandleRegisterFunc)
		(ILWaitHandle *handle, ILThread *thread);

/*
 * Function prototype for unregistering a thread from a wait handle.
 */
typedef void (*ILWaitHandleUnregisterFunc)
		(ILWaitHandle *handle, ILThread *thread);

/*
 * Wait handle kinds.
 */
#define	IL_WAIT_MUTEX		1
#define	IL_WAIT_MONITOR		2

/*
 * Internal structure of a wait handle.
 *
 * Threads register their wakeup semaphores with the wait handle
 * when they wish to wait for the handle to become available.
 * They then do a "sem_wait" or "sem_timedwait" to block until woken.
 *
 * A thread may register itself with multiple handles, and then wait
 * for either 1 or N "sem_wait" calls to implement "WaitForAny" or
 * "WaitForAll" semantics.
 */
struct _tagILWaitHandle
{
	pthread_mutex_t						lock;
	int						   volatile	kind;
	ILWaitHandleDestroyFunc    volatile destroyFunc;
	ILWaitHandleRegisterFunc   volatile	registerFunc;
	ILWaitHandleUnregisterFunc volatile	unregisterFunc;

};

/*
 * Wait handle based mutex.
 */
typedef struct _tagILWaitMutex ILWaitMutex;
struct _tagILWaitMutex
{
	ILWaitHandle			parent;
	ILThread    * volatile  owner;
	unsigned long volatile	count;
	ILWaitQueue				queue;

};

/*
 * Wait handle based monitor, which augments the mutex semantics
 * with the ability to do "Wait", "Pulse", and "PulseAll".
 */
typedef struct _tagILWaitMonitor ILWaitMonitor;
struct _tagILWaitMonitor
{
	ILWaitMutex				mutex;
	ILWaitQueue				signalQueue;

};

/*
 * Wake up a thread.  Returns zero if already woken.
 */
int _ILWaitWakeupThread(ILThread *thread);

#ifdef	__cplusplus
};
#endif

#endif /* IL_USE_PTHREADS */

#endif	/* _PT_INCLUDE_H */
