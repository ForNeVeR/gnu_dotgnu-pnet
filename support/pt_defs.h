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
				sem_wait(&((thread)->suspendAck)); \
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
				sem_wait(&((thread)->resumeAck)); \
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
#define	_ILMutexCreate(mutex)	\
			(pthread_mutex_init((mutex), (pthread_mutexattr_t *)0))
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
#define	_ILCondMutexUnlockUnsafe(mutex)	_ILMutexUnlockUnsafe((mutex))

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
#define	_ILSemaphoreWait(sem)		(sem_wait((sem)))
#define	_ILSemaphorePost(sem)		(sem_post((sem)))

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
