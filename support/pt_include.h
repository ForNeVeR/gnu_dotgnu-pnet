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

#include "il_config.h"
#include "il_thread.h"

/*
 * An easy way to turn off threads completely for testing.
 */
/*#define IL_NO_THREADS*/

/*
 * Determine if we can use pthreads.  Right now, we only do this
 * for Linux, but we will extend it to other systems later.
 */
#if !defined(IL_NO_THREADS)
#if defined(linux) || defined(__linux) || defined(__linux__)
#define	IL_USE_PTHREADS
#endif
#if !defined(IL_USE_PTHREADS)
#define	IL_NO_THREADS
#endif
#endif

#ifdef IL_USE_PTHREADS

/* We need to include <pthread.h> via the garbage collector,
   because it redirects some of the routines elsewhere to
   ensure that the GC is notified of thread creation */
#include <gc.h>
#include <semaphore.h>

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
 * Internal structure of a thread descriptor.
 */
struct _tagILThread
{
	pthread_mutex_t				lock;
	pthread_t					threadId;
	unsigned short    volatile	state;
	unsigned short    volatile	resumeRequested;
	ILThreadStartFunc volatile	startFunc;
	void *            volatile	objectArg;
	pthread_mutex_t				wakeupMutex;
	int				  volatile	wakeupType;
	sem_t						wakeup;

};

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
