/*
 * w32_defs.h - Thread definitions for using Win32 threads.
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

#ifndef	_W32_DEFS_H
#define	_W32_DEFS_H

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Types that are needed elsewhere.
 */
typedef CRITICAL_SECTION	_ILMutex;
typedef HANDLE				_ILCondMutex;
typedef HANDLE				_ILCondVar;
typedef HANDLE				_ILThreadHandle;
typedef DWORD				_ILThreadIdentifier;
typedef HANDLE				_ILSemaphore;
typedef CRITICAL_SECTION	_ILRWLock;

/*
 * This is a real thread package.
 */
#define	_ILThreadIsReal		1

/*
 * Determine if a thread corresponds to "self".
 */
#define	_ILThreadIsSelf(thread)	\
			((thread)->identifier == GetCurrentThreadId())

/*
 * Suspend and resume threads.  Note: these are the primitive
 * versions, which are not "suspend-safe".
 */
#define	_ILThreadSuspendOther(thread)	\
			do { \
				SuspendThread((thread)->handle); \
			} while (0)
#define	_ILThreadSuspendSelf(thread)	\
			do { \
				WaitForSingleObject(thread->resumeAck, INFINITE); \
			} while (0)
#define	_ILThreadResumeOther(thread)	\
			do { \
				ResumeThread((thread)->handle); \
			} while (0)
#define	_ILThreadResumeSelf(thread)		\
			do { \
				ReleaseSemaphore(thread->resumeAck, 1, NULL); \
			} while (0)

/*
 * Terminate a running thread.
 */
#define	_ILThreadTerminate(thread)	\
			do { \
				TerminateThread((thread)->handle, 0); \
			} while (0)

/*
 * Destroy a thread handle that is no longer required.
 */
#define	_ILThreadDestroy(thread)	\
			do { \
				CloseHandle((thread)->handle); \
			} while (0)

/*
 * Primitive mutex operations.  Note: the "Lock" and "Unlock"
 * operations are not "suspend-safe".
 */
#define	_ILMutexCreate(mutex)	\
			do { \
				InitializeCriticalSection((mutex)); \
			} while (0)
#define	_ILMutexDestroy(mutex)	\
			do { \
				DeleteCriticalSection((mutex)); \
			} while (0)
#define	_ILMutexLockUnsafe(mutex)	\
			do { \
				EnterCriticalSection((mutex)); \
			} while (0)
#define	_ILMutexUnlockUnsafe(mutex)	\
			do { \
				LeaveCriticalSection((mutex)); \
			} while (0)

/*
 * Primitive condition mutex operations.  These are similar to
 * normal mutexes, except that they can be used with condition
 * variables to do an atomic "unlock and wait" operation.
 */
#define	_ILCondMutexCreate(mutex)	\
			do { \
				*(mutex) = CreateMutex(NULL, FALSE, NULL); \
			} while (0)
#define	_ILCondMutexDestroy(mutex)	\
			do { \
				CloseHandle(*(mutex)); \
			} while (0)
#define	_ILCondMutexLockUnsafe(mutex)	\
			do { \
				WaitForSingleObject(*(mutex), INFINITE); \
			} while (0)
#define	_ILCondMutexUnlockUnsafe(mutex)	\
			do { \
				ReleaseMutex(*(mutex)); \
			} while (0)

/*
 * Primitive read/write lock operations.  Note: the "Lock" and
 * "Unlock" operations are not "suspend-safe".
 */
#define	_ILRWLockCreate(rwlock)				_ILMutexCreate((rwlock))
#define	_ILRWLockDestroy(rwlock)			_ILMutexDestroy((rwlock))
#define	_ILRWLockReadLockUnsafe(rwlock)		_ILMutexLockUnsafe((rwlock))
#define	_ILRWLockWriteLockUnsafe(rwlock)	_ILMutexLockUnsafe((rwlock))
#define	_ILRWLockUnlockUnsafe(rwlock)		_ILMutexUnlockUnsafe((rwlock))

/*
 * Primitive semaphore operations.
 */
#define	_ILSemaphoreCreate(sem)	\
			do { \
				*(sem) = CreateSemaphore(NULL, 0, 0x7FFFFFFF, NULL); \
			} while (0)
#define	_ILSemaphoreDestroy(sem)	\
			do { \
				CloseHandle(*(sem)); \
			} while (0)
#define	_ILSemaphoreWait(sem)	\
			do { \
				WaitForSingleObject(*(sem), INFINITE); \
			} while (0)
#define	_ILSemaphorePost(sem)	\
			do { \
				ReleaseSemaphore(*(sem), 1, NULL); \
			} while (0)

/*
 * Primitive condition variable operations.
 */
#define	_ILCondVarCreate(cond)		_ILSemaphoreCreate((cond))
#define	_ILCondVarDestroy(cond)		_ILSemaphoreDestroy((cond))
#define	_ILCondVarSignal(cond)		_ILSemaphorePost((cond))
int _ILCondVarTimedWait(_ILCondVar *cond, _ILCondMutex *mutex, ILUInt32 ms);

/*
 * Get or set the thread object that is associated with "self".
 */
extern DWORD _ILThreadObjectKey;
#define	_ILThreadGetSelf()	\
			((ILThread *)(TlsGetValue(_ILThreadObjectKey)))
#define	_ILThreadSetSelf(object)	\
			(TlsSetValue(_ILThreadObjectKey, (object)))

/*
 * Call a function "once".
 */
#define	_ILCallOnce(func)	\
			do { \
				static LONG volatile __once = 0; \
				if(!InterlockedExchange((PLONG)&__once, 1)) \
				{ \
					(*(func))(); \
				} \
			} while (0)

#define _ILThreadYield() Sleep(0)

#ifdef	__cplusplus
};
#endif

#endif	/* _W32_DEFS_H */
