/*
 * pt_defs.h - Thread definitions for using pthreads.
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

#ifndef	_PT_DEFS_H
#define	_PT_DEFS_H

#include <errno.h>
#include <semaphore.h>
#include <signal.h>
#ifdef HAVE_LIBGC
#include <private/gc_priv.h>	/* For SIG_SUSPEND */
#if defined(SIG_SUSPEND) && defined(GC_DARWIN_THREADS)
/* SIG_SUSPEND is unused by LIBGC 6 for GC_DARWIN_THREADS and the definition */
/* is wrong (SIGRTMIN + x). SIGRTMIN is not defined on MAC OSX */
#undef SIG_SUSPEND
#endif
#endif

#ifdef	__cplusplus
extern	"C" {
#endif

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
#if defined(__sun)
#define	IL_SIG_SUSPEND		(__SIGRTMIN+0)
#define	IL_SIG_RESUME		(__SIGRTMIN+1)
#define	IL_SIG_ABORT		(__SIGRTMIN+2)
#elif !defined(__SIGRTMIN) || (__SIGRTMAX - __SIGRTMIN < 14)
#define	IL_SIG_SUSPEND		SIGALRM
#define	IL_SIG_RESUME		SIGVTALRM
#define	IL_SIG_ABORT		SIGFPE
#else
#define	IL_SIG_SUSPEND		(__SIGRTMIN+10)
#define	IL_SIG_RESUME		(__SIGRTMIN+11)
#define	IL_SIG_ABORT		(__SIGRTMIN+12)
#endif

/*
 * Signals that are used inside pthreads itself.  This is a bit of
 * a hack, but there isn't any easy way to get this information.
 */
#if defined(SIGCANCEL)
#define PTHREAD_SIG_CANCEL	SIGCANCEL
#elif !defined(__SIGRTMIN) || (__SIGRTMAX - __SIGRTMIN < 3)
#define PTHREAD_SIG_CANCEL	SIGUSR2
#else
#define PTHREAD_SIG_CANCEL	(__SIGRTMIN+1)
#endif

/*
 * Determine if we have read-write lock support in pthreads.
 */
#if defined(PTHREAD_RWLOCK_INITIALIZER) && defined(__USE_UNIX98)
	#define	IL_HAVE_RWLOCKS
#endif

/*
 * Types that are needed elsewhere.
 */
typedef pthread_mutex_t		_ILMutex;
typedef pthread_mutex_t		_ILCondMutex;
typedef pthread_cond_t		_ILCondVar;
typedef pthread_t			_ILThreadHandle;
typedef pthread_t			_ILThreadIdentifier;
typedef sem_t				_ILSemaphore;
#ifdef IL_HAVE_RWLOCKS
typedef pthread_rwlock_t	_ILRWLock;
#else
typedef pthread_mutex_t		_ILRWLock;
#endif

/*
 * Semaphore which allows to release multiple or all waiters.
 */
typedef struct
{
	_ILMutex			_lock;
	_ILSemaphore		_sem;
	ILInt32 volatile	_waiting;
} _ILCountSemaphore;

typedef struct
{
	_ILCondMutex	_mutex;
	_ILCondVar		_cond;
} _ILMonitor;

/*
 * This is a real thread package.
 */
#define	_ILThreadIsReal		1

/*
 * Determine if a thread corresponds to "self".
 */
#define	_ILThreadIsSelf(thread)	\
			(pthread_equal((thread)->handle, pthread_self()))

/*
 * Suspend and resume threads.  Note: these are the primitive
 * versions, which are not "suspend-safe".
 */
#define	_ILThreadSuspendOther(thread)	\
			do { \
				pthread_kill((thread)->handle, IL_SIG_SUSPEND); \
				_ILSemaphoreWait(&((thread)->suspendAck)); \
			} while (0)
#define	_ILThreadSuspendSelf(thread)	\
			do { \
				_ILThreadSuspendUntilResumed((thread)); \
				sem_post(&((thread)->resumeAck)); \
			} while (0)
#define	_ILThreadResumeOther(thread)	\
			do { \
				(thread)->resumeRequested = 1; \
				pthread_kill((thread)->handle, IL_SIG_RESUME); \
				_ILSemaphoreWait(&((thread)->resumeAck)); \
				(thread)->resumeRequested = 0; \
			} while (0)
#define	_ILThreadResumeSelf(thread)		_ILThreadResumeOther((thread))

/*
 * Suspend the current thread until it is resumed.
 */
void _ILThreadSuspendUntilResumed(ILThread *thread);

/*
 * Terminate a running thread.
 */
#define	_ILThreadTerminate(thread)	\
			do { \
				pthread_cancel((thread)->handle); \
			} while (0)

/*
 * Destroy a thread handle that is no longer required.
 */
#define	_ILThreadDestroy(thread)	do { ; } while (0)

/*
 * Primitive mutex operations.  Note: the "Lock" and "Unlock"
 * operations are not "suspend-safe".
 */
extern pthread_mutexattr_t _ILMutexAttr;
#define	_ILMutexCreate(mutex)	\
			(pthread_mutex_init((mutex), &_ILMutexAttr))
#define	_ILMutexDestroy(mutex)	\
			(pthread_mutex_destroy((mutex)))
#define	_ILMutexLockUnsafe(mutex)	\
			(pthread_mutex_lock((mutex)))
#define	_ILMutexUnlockUnsafe(mutex)	\
			(pthread_mutex_unlock((mutex)))

/*
 * Primitive condition mutex operations.  These are similar to
 * normal mutexes, except that they can be used with condition
 * variables to do an atomic "unlock and wait" operation.
 */
#define	_ILCondMutexCreate(mutex)		_ILMutexCreate((mutex))
#define	_ILCondMutexDestroy(mutex)		_ILMutexDestroy((mutex))
#define	_ILCondMutexLockUnsafe(mutex)	_ILMutexLockUnsafe((mutex))
#define	_ILCondMutexTryLockUnsafe(mutex)	\
			(pthread_mutex_trylock((mutex)))
#define	_ILCondMutexUnlockUnsafe(mutex)	_ILMutexUnlockUnsafe((mutex))

/*
 * Try to lock a condition mutex wor a given amount of time.
 * Returns IL_THREAD_OK if the mutex could be acquired, IL_THREAD_BUSY if the
 * operation timed out, IL_THREAD_ERR_INVALID_TIMEOUT if a negative timeout not
 * equal to IL_MAX_UINT32 was supplied or IL_THREAD_ERR_UNKNOWN on every other
 * error.
 */
int _ILCondMutexTimedLockUnsafe(_ILCondMutex *mutex, ILUInt32 ms);

/*
 * Primitive read/write lock operations.  Note: the "Lock" and
 * "Unlock" operations are not "suspend-safe".
 */
#ifdef IL_HAVE_RWLOCKS
#define	_ILRWLockCreate(rwlock)	\
			(pthread_rwlock_init((rwlock), (pthread_rwlockattr_t *)0))
#define	_ILRWLockDestroy(rwlock)	\
			(pthread_rwlock_destroy((rwlock)))
#define	_ILRWLockReadLockUnsafe(rwlock)	\
			(pthread_rwlock_rdlock((rwlock)))
#define	_ILRWLockWriteLockUnsafe(rwlock)	\
			(pthread_rwlock_wrlock((rwlock)))
#define	_ILRWLockUnlockUnsafe(rwlock)	\
			(pthread_rwlock_unlock((rwlock)))
#else
#define	_ILRWLockCreate(rwlock)				(_ILMutexCreate((rwlock)))
#define	_ILRWLockDestroy(rwlock)			(_ILMutexDestroy((rwlock)))
#define	_ILRWLockReadLockUnsafe(rwlock)		(_ILMutexLockUnsafe((rwlock)))
#define	_ILRWLockWriteLockUnsafe(rwlock)	(_ILMutexLockUnsafe((rwlock)))
#define	_ILRWLockUnlockUnsafe(rwlock)		(_ILMutexUnlockUnsafe((rwlock)))
#endif

/*
 * Primitive semaphore operations.
 */
#define	_ILSemaphoreCreate(sem)		(sem_init((sem), 0, 0))
#define	_ILSemaphoreDestroy(sem)	(sem_destroy((sem)))
#define	_ILSemaphoreWait(sem)		\
	do { \
		while(sem_wait((sem)) == -1 && errno == EINTR) \
		{ \
			continue; \
		} \
	} while(0)
#define	_ILSemaphorePost(sem)		(sem_post((sem)))
int _ILSemaphorePostMultiple(_ILSemaphore *sem, ILUInt32 count);

/*
 * Primitive condition variable operations.
 */
#define	_ILCondVarCreate(cond)		\
			(pthread_cond_init((cond), (pthread_condattr_t *)0))
#define	_ILCondVarDestroy(cond)		\
			(pthread_cond_destroy((cond)))
#define	_ILCondVarSignal(cond)		\
			(pthread_cond_signal((cond)))
int _ILCondVarTimedWait(_ILCondVar *cond, _ILCondMutex *mutex, ILUInt32 ms);

/*
 * Release a number of waiting threads from the count semaphore.
 */
int _ILCountSemaphoreSignalCount(_ILCountSemaphore *sem, ILUInt32 count);

/*
 * Release all waiting threads from the count semaphore.
 */
int _ILCountSemaphoreSignalAll(_ILCountSemaphore *sem);

#define _ILCountSemaphoreSignal(sem)	_ILCountSemaphoreSignalCount((sem), 1)

/*
 * Wait on a count semaphore.
 */
int _ILCountSemaphoreTimedWait(_ILCountSemaphore *sem, ILUInt32 ms);

#define _ILCountSemaphoreWait(sem)	_ILCountSemaphoreTimedWait((sem), IL_MAX_UINT32)
#define _ILCountSemaphoreTryWait(sem)	_ILCountSemaphoreTimedWait((sem), 0)

/*
 * Primitive monitor operations.
 */
#define	_ILMonitorCreate(mon)	\
		({ \
			int __result = _ILCondMutexCreate(&((mon)->_mutex)); \
			if(!__result) \
			{ \
				__result = _ILCondVarCreate(&((mon)->_cond)); \
			} \
			__result == 0 ? IL_THREAD_OK : IL_THREAD_ERR_UNKNOWN; \
		})
#define	_ILMonitorDestroy(mon)	\
		({ \
			int __result = _ILCondVarDestroy(&((mon)->_cond)); \
			if(!__result) \
			{ \
				__result = _ILCondMutexDestroy(&((mon)->_mutex)); \
			} \
			__result == 0 ? IL_THREAD_OK : IL_THREAD_ERR_UNKNOWN; \
		})
#define	_ILMonitorPulse(mon)	\
			_ILCondVarSignal(&((mon)->_cond))
#define	_ILMonitorPulseAll(mon)	\
			(pthread_cond_broadcast(&((mon)->_cond)))
int	_ILMonitorTimedTryEnter(_ILMonitor *mon, ILUInt32 ms);
#define _ILMonitorTryEnter(mon) _ILMonitorTimedTryEnter((mon), 0)
#define _ILMonitorEnter(mon) _ILMonitorTimedTryEnter((mon), IL_MAX_UINT32)
int	_ILMonitorExit(_ILMonitor *mon);
int _ILMonitorTimedWait(_ILMonitor *mon, ILUInt32 ms,
						ILMonitorPredicate predicate, void *arg);
#define _ILMonitorWait(mon, pred, arg) \
			_ILMonitorTimedWait((mon), IL_MAX_UINT32, (pred), (arg))

/*
 * Get or set the thread object that is associated with "self".
 */
extern pthread_key_t _ILThreadObjectKey;
#define	_ILThreadGetSelf()	\
			((ILThread *)(pthread_getspecific(_ILThreadObjectKey)))
#define	_ILThreadSetSelf(object)	\
			(pthread_setspecific(_ILThreadObjectKey, (object)))

/*
 * Call a function "once".
 */
#define	_ILCallOnce(func)	\
			do { \
				static pthread_once_t __once = PTHREAD_ONCE_INIT; \
				pthread_once(&__once, (func)); \
			} while (0)


#ifdef _POSIX_PRIORITY_SCHEDULING
	#define _ILThreadYield() sched_yield()
#else
	#define _ILThreadYield()
#endif
	
		
#ifdef	__cplusplus
};
#endif

#endif	/* _PT_DEFS_H */
