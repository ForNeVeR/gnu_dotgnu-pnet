/*
 * no_defs.h - Thread definitions for systems without thread support.
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

#ifndef	_NO_DEFS_H
#define	_NO_DEFS_H

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Types that are needed elsewhere.
 */
typedef int	_ILMutex;
typedef int	_ILCondMutex;
typedef int	_ILCondVar;
typedef int	_ILThreadHandle;
typedef int	_ILThreadIdentifier;
typedef int	_ILSemaphore;
typedef int	_ILRWLock;

/*
 * This is not a real thread package.
 */
#define	_ILThreadIsReal		0

/*
 * Determine if a thread corresponds to "self".  Since there is only
 * one thread in a single-threaded system, it must be "self".
 */
#define	_ILThreadIsSelf(thread)		1

/*
 * Suspend and resume threads.
 */
#define	_ILThreadSuspendOther(thread)	do { ; } while (0)
#define	_ILThreadSuspendSelf(thread)	do { ; } while (0)
#define	_ILThreadResumeOther(thread)	do { ; } while (0)
#define	_ILThreadResumeSelf(thread)		do { ; } while (0)

/*
 * Terminate a running thread.
 */
#define	_ILThreadTerminate(thread)		do { ; } while (0)

/*
 * Destroy a thread handle that is no longer required.
 */
#define	_ILThreadDestroy(thread)		do { ; } while (0)

/*
 * Primitive mutex operations.
 */
#define	_ILMutexCreate(mutex)			do { *(mutex) = 0; } while (0)
#define	_ILMutexDestroy(mutex)			do { ; } while (0)
#define	_ILMutexLockUnsafe(mutex)		do { ; } while (0)
#define	_ILMutexUnlockUnsafe(mutex)		do { ; } while (0)

/*
 * Primitive condition mutex operations.
 */
#define	_ILCondMutexCreate(mutex)		do { *(mutex) = 0; } while (0)
#define	_ILCondMutexDestroy(mutex)		do { ; } while (0)
#define	_ILCondMutexLockUnsafe(mutex)	do { ; } while (0)
#define	_ILCondMutexUnlockUnsafe(mutex)	do { ; } while (0)

/*
 * Primitive read/write lock operations.
 */
#define	_ILRWLockCreate(rwlock)				do { *(rwlock) = 0; } while (0)
#define	_ILRWLockDestroy(rwlock)			do { ; } while (0)
#define	_ILRWLockReadLockUnsafe(rwlock)		do { ; } while (0)
#define	_ILRWLockWriteLockUnsafe(rwlock)	do { ; } while (0)
#define	_ILRWLockUnlockUnsafe(rwlock)		do { ; } while (0)

/*
 * Primitive semaphore operations.
 */
#define	_ILSemaphoreCreate(sem)		do { *(sem) = 0; } while (0)
#define	_ILSemaphoreDestroy(sem)	do { ; } while (0)
#define	_ILSemaphoreWait(sem)		do { ; } while (0)
#define	_ILSemaphorePost(sem)		do { ; } while (0)

/*
 * Primitive condition variable operations.
 */
#define	_ILCondVarCreate(cond)		do { *(cond) = 0; } while (0)
#define	_ILCondVarDestroy(cond)		do { ; } while (0)
#define	_ILCondVarSignal(cond)		do { ; } while (0)
int _ILCondVarTimedWait(_ILCondVar *cond, _ILCondMutex *mutex, ILUInt32 ms);

/*
 * Get or set the thread object that is associated with "self".
 */
#define	_ILThreadGetSelf()			(ILThreadSelf())
#define	_ILThreadSetSelf(object)	do { ; } while (0)

/*
 * Call a function "once".
 */
#define	_ILCallOnce(func)	\
			do { \
				static int __once = 0; \
				if(!__once) \
				{ \
					__once = 1; \
					(*(func))(); \
				} \
			} while (0)

#define _ILThreadYield()

#ifdef	__cplusplus
};
#endif

#endif	/* _NO_DEFS_H */
