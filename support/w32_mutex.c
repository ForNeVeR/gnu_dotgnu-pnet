/*
 * w32_mutex.c - Mutex management routines for Win32 systems.
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

ILMutex *ILMutexCreate(void)
{
	CRITICAL_SECTION *mutex;
	mutex = (CRITICAL_SECTION *)ILMalloc(sizeof(CRITICAL_SECTION));
	if(mutex)
	{
		InitializeCriticalSection(mutex);
	}
	return (ILMutex *)mutex;
}

void ILMutexDestroy(ILMutex *mutex)
{
	DeleteCriticalSection((CRITICAL_SECTION *)mutex);
	ILFree(mutex);
}

void ILMutexLock(ILMutex *mutex)
{
	EnterCriticalSection((CRITICAL_SECTION *)mutex);
}

void ILMutexUnlock(ILMutex *mutex)
{
	LeaveCriticalSection((CRITICAL_SECTION *)mutex);
}

ILRWLock *ILRWLockCreate(void)
{
	CRITICAL_SECTION *rwlock;
	rwlock = (CRITICAL_SECTION *)ILMalloc(sizeof(CRITICAL_SECTION));
	if(rwlock)
	{
		InitializeCriticalSection(rwlock);
	}
	return (ILRWLock *)rwlock;
}

void ILRWLockDestroy(ILRWLock *rwlock)
{
	DeleteCriticalSection((CRITICAL_SECTION *)rwlock);
	ILFree(rwlock);
}

void ILRWLockReadLock(ILRWLock *rwlock)
{
	EnterCriticalSection((CRITICAL_SECTION *)rwlock);
}

void ILRWLockWriteLock(ILRWLock *rwlock)
{
	EnterCriticalSection((CRITICAL_SECTION *)rwlock);
}

void ILRWLockUnlock(ILRWLock *rwlock)
{
	LeaveCriticalSection((CRITICAL_SECTION *)rwlock);
}

#ifdef	__cplusplus
};
#endif

#endif /* IL_USE_WIN32_THREADS */
