/*
 * thr_defs.h - Common thread definitions.
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

#ifndef	_THR_DEFS_H
#define	_THR_DEFS_H

#define	_GNU_SOURCE		/* Get read-write lock support from <pthread.h> */

/*
 * Choose which thread package we will be using.
 */
#include "thr_choose.h"

/*
 * Include the package-specific definitions.
 */
#ifdef IL_USE_PTHREADS
#include "pt_defs.h"
#endif
#ifdef IL_USE_WIN32_THREADS
#include "w32_defs.h"
#endif
#ifdef IL_NO_THREADS
#include "no_defs.h"
#endif

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Internal structure of a thread descriptor.
 */
struct _tagILThread
{
	_ILMutex						lock;
	_ILThreadHandle		volatile	handle;
	_ILThreadIdentifier	volatile	identifier;
	unsigned short    	volatile	state;
	unsigned char     	volatile	resumeRequested;
	unsigned char     	volatile	suspendRequested;
	unsigned int	  	volatile  	numLocksHeld;
	_ILSemaphore					resumeAck;
	_ILSemaphore					suspendAck;
	ILThreadStartFunc 	volatile	startFunc;
	void *            	volatile	objectArg;
	_ILMutex						wakeupMutex;
	int				  	volatile	wakeupType;
	_ILSemaphore					wakeup;

};

/*
 * Extra thread state flags and masks that are used internally.
 */
#define	IL_TS_PUBLIC_FLAGS		0x01FF
#define	IL_TS_INTERRUPTED		0x0200
#define	IL_TS_SUSPENDED_SELF	0x0400

/*
 * Thread wakeup types.
 */
#define	IL_WAKEUP_NONE			0
#define	IL_WAKEUP_ONCE			1
#define	IL_WAKEUP_ALL			2

/*
 * Safe mutex lock and unlock operations that will prevent the
 * thread from being suspended while it holds a lock.
 */
#define	_ILMutexLock(mutex)	\
			do { \
				ILThread *__self = _ILThreadGetSelf(); \
				++(__self->numLocksHeld); \
				_ILMutexLockUnsafe((mutex)); \
			} while (0)
#define	_ILMutexUnlock(mutex)	\
			do { \
				ILThread *__self = _ILThreadGetSelf(); \
				_ILMutexUnlockUnsafe((mutex)); \
				if(--(__self->numLocksHeld) == 0) \
				{ \
					if(__self->suspendRequested) \
					{ \
						_ILThreadSuspendRequest(__self); \
					} \
				} \
			} while (0)

/*
 * Safe read/write lock and unlock operations that will prevent
 * the thread from being suspended while it holds a lock.
 */
#define	_ILRWLockReadLock(rwlock)	\
			do { \
				ILThread *__self = _ILThreadGetSelf(); \
				++(__self->numLocksHeld); \
				_ILRWLockReadLockUnsafe((rwlock)); \
			} while (0)
#define	_ILRWLockWriteLock(rwlock)	\
			do { \
				ILThread *__self = _ILThreadGetSelf(); \
				++(__self->numLocksHeld); \
				_ILRWLockWriteLockUnsafe((rwlock)); \
			} while (0)
#define	_ILRWLockUnlock(rwlock)	\
			do { \
				ILThread *__self = _ILThreadGetSelf(); \
				_ILRWLockUnlockUnsafe((rwlock)); \
				if(--(__self->numLocksHeld) == 0) \
				{ \
					if(__self->suspendRequested) \
					{ \
						_ILThreadSuspendRequest(__self); \
					} \
				} \
			} while (0)

/*
 * Initialize the system-specific thread routines and find the
 * thread handle and identifier for the main thread.
 */
void _ILThreadInitSystem(ILThread *mainThread);

/*
 * Function that is called to run a thread after system-specific
 * initialization has been performed.  If this function returns,
 * the thread must immediately exit.
 */
void _ILThreadRun(ILThread *thread);

/*
 * Create a new system-specific thread.  Returns zero if
 * the creation process failed.
 */
int _ILThreadCreateSystem(ILThread *thread);

/*
 * Process a request to suspend the current thread.
 */
void _ILThreadSuspendRequest(ILThread *thread);

#ifdef	__cplusplus
};
#endif

#endif	/* _THR_DEFS_H */
