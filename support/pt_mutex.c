/*
 * pt_mutex.c - Mutex management, based on pthreads.
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

#define	_GNU_SOURCE		/* Get read-write lock support from <pthread.h> */

#include "pt_include.h"

#ifdef IL_USE_PTHREADS

#ifdef	__cplusplus
extern	"C" {
#endif

ILMutex *ILMutexCreate(void)
{
	pthread_mutex_t *mutex;
	mutex = (pthread_mutex_t *)ILMalloc(sizeof(pthread_mutex_t));
	if(mutex)
	{
		pthread_mutex_init(mutex, (pthread_mutexattr_t *)0);
	}
	return (ILMutex *)mutex;
}

void ILMutexDestroy(ILMutex *mutex)
{
	pthread_mutex_destroy((pthread_mutex_t *)mutex);
}

void ILMutexLock(ILMutex *mutex)
{
	MutexLockSafe((pthread_mutex_t *)mutex);
}

void ILMutexUnlock(ILMutex *mutex)
{
	MutexUnlockSafe((pthread_mutex_t *)mutex);
}

/*
 * Determine if we have read-write lock support in pthreads.
 */
#ifdef PTHREAD_RWLOCK_INITIALIZER
	#define	IL_HAVE_RWLOCKS
#endif

ILRWLock *ILRWLockCreate(void)
{
#ifdef IL_HAVE_RWLOCKS
	pthread_rwlock_t *rwlock;
	rwlock = (pthread_rwlock_t *)ILMalloc(sizeof(pthread_rwlock_t));
	if(rwlock)
	{
		pthread_rwlock_init(rwlock, (pthread_rwlockattr_t *)0);
	}
	return (ILRWLock *)rwlock;
#else
	return (ILRWLock *)ILMutexCreate();
#endif
}

void ILRWLockDestroy(ILRWLock *rwlock)
{
#ifdef IL_HAVE_RWLOCKS
	pthread_rwlock_destroy((pthread_rwlock_t *)rwlock);
#else
	pthread_mutex_destroy((pthread_mutex_t *)rwlock);
#endif
}

void ILRWLockReadLock(ILRWLock *rwlock)
{
#ifdef IL_HAVE_RWLOCKS
	ReadLockSafe((pthread_rwlock_t *)rwlock);
#else
	MutexLockSafe((pthread_mutex_t *)rwlock);
#endif
}

void ILRWLockWriteLock(ILRWLock *rwlock)
{
#ifdef IL_HAVE_RWLOCKS
	WriteLockSafe((pthread_rwlock_t *)rwlock);
#else
	MutexLockSafe((pthread_mutex_t *)rwlock);
#endif
}

void ILRWLockUnlock(ILRWLock *rwlock)
{
#ifdef IL_HAVE_RWLOCKS
	RWUnlockSafe((pthread_rwlock_t *)rwlock);
#else
	MutexUnlockSafe((pthread_mutex_t *)rwlock);
#endif
}

#ifdef	__cplusplus
};
#endif

#endif /* IL_USE_PTHREADS */
