/*
 * w32_include.h - Include the necessary headers for Win32 threads.
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

#ifndef	_W32_INCLUDE_H
#define	_W32_INCLUDE_H

#include "thr_choose.h"

#ifdef IL_USE_WIN32_THREADS

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Internal information for a thread.
 */
struct _tagILThread
{
	CRITICAL_SECTION			lock;
	HANDLE			  volatile	handle;
	DWORD			  volatile	identifier;
	unsigned int	  volatile	state;
	ILThreadStartFunc volatile	startFunc;
	void *			  volatile	objectArg;
	HANDLE			  volatile	resumeSemaphore;
	LONG			  volatile	numCriticalsHeld;
	LONG			  volatile  suspendRequested;
	HANDLE			  volatile	suspendSemaphore;

};

/*
 * Thread states that are used internally.
 */
#define	IL_TS_PUBLIC_FLAGS		0x01FF
#define	IL_TS_SUSPENDED_SELF	0x0200

/*
 * Safe version of "EnterCriticalSection" that keeps track of
 * how many critical sections are held by a thread.  We cannot
 * suspend a thread that is holding critical sections.
 */
#define	EnterCriticalSectionSafe(crit)	\
			do { \
				ILThread *__self = \
					(ILThread *)TlsGetValue(_ILThreadObjectKey); \
				InterlockedIncrement((PLONG)&(__self->numCriticalsHeld)); \
				EnterCriticalSection((crit)); \
			} while (0)

/*
 * Thread-specific key for getting the current thread object.
 */
extern DWORD _ILThreadObjectKey;

/*
 * Safe version of "LeaveCriticalSection" that keeps track of
 * how many critical sections are held by a thread, and which
 * checks for suspension requests.
 */
#define	LeaveCriticalSectionSafe(crit)	\
			do { \
				ILThread *__self = \
					(ILThread *)TlsGetValue(_ILThreadObjectKey); \
				LeaveCriticalSection((crit)); \
				if(!InterlockedDecrement((PLONG)&(__self->numCriticalsHeld))) \
				{ \
					if(InterlockedExchange	\
						((PLONG)&(__self->suspendRequested), 0)) \
					{ \
						_ILThreadSuspendRequest(__self); \
					} \
				} \
			} while (0)

/*
 * Safe version of "SuspendThread" that will only suspend the
 * thread when it gives up all critical sections.  We assume
 * that the current thread holds "thread->lock" and that the
 * current thread is not "thread".
 */
#define	SuspendThreadSafe(thread)	\
			do { \
				SuspendThread((thread)->handle); \
				if((thread)->numCriticalsHeld > 0) \
				{ \
					_ILThreadHardSuspend((thread)); \
				} \
			} while (0)

/*
 * Process a suspension request to the current thread.
 */
void _ILThreadSuspendRequest(ILThread *thread);

/*
 * Perform a "hard" suspend on a thread that we know is currently
 * in possession of critical sections.
 */
void _ILThreadHardSuspend(ILThread *thread);

#ifdef	__cplusplus
};
#endif

#endif /* IL_USE_WIN32_THREADS */

#endif	/* _W32_INCLUDE_H */
