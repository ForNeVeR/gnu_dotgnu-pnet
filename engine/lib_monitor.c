/*
 * lib_monitor.c - Internalcall methods for "System.Threading.Monitor".
 *
 * Copyright (C) 2003  Southern Storm Software, Pty Ltd.
 *
 * Authors:  Thong Nguyen (tum@veridicus.com) 
 * Lots of tips from Russell Stuart (rstuart@stuart.id.au)
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

#include "engine.h"
#include "lib_defs.h"
#include "wait_mutex.h"
#include "interlocked.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * There are two methods of associating monitors with objects.
 * The standard method stores a pointer to the monitor in the object header.
 * The "thin-lock" method uses a hashtable to associate the monitor with the
 * object.
 * The standard method is faster but requires an extra word for every object
 * even if the object is never used for synchronization.  The thin-lock method
 * is slower but doesn't require an extra word for every object so is better
 * on platforms with limited memory or with programs that create thousands
 * (or millions) of tiny objects or with programs that don't use make much
 * use of synchronization (remember: pnetlib uses synchronization).
 *
 * The standard algorithm is designed to eliminate context switches into kernel
 * mode by avoiding "fat" OS-locks (therefore avoiding system calls).
 * The algorithm stores a pointer to an object's monitor in the object header
 * This pointer is known as the object's lockword.
 * If an object's lockword is 0, the algorithm knows *for sure* that the object is
 * unlocked and will then use a compare-and-exchange to to attach a monitor
 * to the object.  When a thread completely exits a monitor, the monitor is
 * returned to a thread-local free list and the objects' lockword is reset to 0.
 * Because objects are always properly aligned on word boundaries, the first
 * (and second) bits of the pointer/object-lockword are always zero allowing us to 
 * utilize them to store other bits of information.  The first bit of the 
 * lockword is used as a spin lock so that the algorithm can, at necessary 
 * critical sections, prevent other thread from changing the monitor and/or
 * lockword.  There are critical (but extremely unlikely) points where a monitor 
 * may never be returned to the thread-local free list even if the monitor is fully 
 * released.  This isn't actually a problem because monitors are always allocated 
 * on the GC heap.  If the object is locked and unlocked again, the monitor may 
 * get another chance to be returned to the free list.  If the object becomes 
 * garbage and is collected then then monitor will eventually be collected as well.
 *
 * Both algorithms uses 3 main functions: GetObjectLockWord, SetObjectLockWord and 
 * CompareAndExchangeObjectLockWord.  The implementation of these functions is
 * trivial for the standard algorithm and is implemented as macros in
 * engine/lib_def.h.
 * On some platforms, CompareAndExchangeObjectLockWord may use a global lock
 * because it uses ILInterlockedCompareAndExchangePointers which may not be
 * ported to that platform (see support/interlocked.h).  
 * CompareAndExchangeObjectLockWord always uses a global lock if the thin-lock
 * algorithm is used.
 *
 * The algorithm ASSUMES ILInterlockedCompareAndExchangePointers acts as a
 * memory barrier.
 *
 * The thin-lock algorithm is designed to be easily "plugged in" so it uses
 * the concept of lockwords as well as locking of lockwords except that the
 * lockword is stored in a hashtable instead of the object header.  Instead of
 * being defined as macros, the implementation of GetObjectLockWord,
 * SetObjectLockWord and CompareAndExchangeObjectLockWord are defined as functions
 * and their definitions are in engine/monitor.c.
 *
 * This file includes support/wait_mutex.h so that it can have fast access
 * to certain data structures.  These structures should never be accessed
 * directly but should be accessed through MACROs or inline functions
 * defined in support/wait_mutex.h.
 *
 * You can enable thin-locks by configuring pnet with the full-tl profile (1).
 *
 * ./configure --with-profile=full-tl
 *
 * When thin locks are configured, IL_CONFIG_USE_THIN_LOCKS is defined.
 *
 * (1) See the profiles directory for more information about profiles.
 *
 * - Thong Nguyen (tum@veridicus.com) aka Tum
 */

/*
 * TODO: #define out bits of code that isn't needed if IL_NO_THREADS is defined.
 * It may not be possible to remove everything (if anything) because even on
 * single threaded systems, a thread should still be able to interrupt/abort
 * itself so Monitor.Enter and Monitor.Wait still needs to call the underlying
 * methods that check for aborts/interrupts.
 */

/*
 * Spins until the object is unmarked and the current thread can mark it.
 */
static IL_INLINE ILLockWord _ILObjectLockWord_WaitAndMark(ILExecThread *thread, ILObject *obj)
{
	ILLockWord lockword;

	for (;;)
	{
		lockword = GetObjectLockWord(thread, obj);

		if ((CompareAndExchangeObjectLockWord(thread, obj, IL_LW_MARK(lockword),
			IL_LW_UNMARK(lockword)) == IL_LW_UNMARK(lockword)))
		{
			return IL_LW_MARK(lockword);
		}

		ILThreadYield();
	}
}

/*
 * Sets an object's lockword to 0.
 */
#define _ILObjectLockWord_Unmark(thread, obj) \
	SetObjectLockWord(thread, obj, IL_LW_UNMARK(GetObjectLockWord(thread, obj)));

/*
 * Adds a monitor to the end of the thread's free monitor list.
 * If the free list grows too large, the monitor is abandoned
 * and left for the garbage collector.
 */
#define _ILExecMonitorAppendToFreeList(thread, monitor) \
	if (thread->freeMonitorCount < 32) \
	{	\
		monitor->waiters = 0; \
		monitor->next = thread->freeMonitor;	\
		thread->freeMonitor = monitor;	\
		thread->freeMonitorCount++;	\
	}

/*
 * public static void Enter(Object obj);
 */
void _IL_Monitor_Enter(ILExecThread *thread, ILObject *obj)
{
	_IL_Monitor_InternalTryEnter(thread, obj, IL_WAIT_INFINITE);
}

/*
 * public static bool InternalTryEnter(Object obj, int timeout);
 *
 * The acquisition algorithm used is designed to be only require
 * a compare-and-exchange when the monitor is uncontested
 * (the most likely case).
 */
ILBool _IL_Monitor_InternalTryEnter(ILExecThread *thread,
									ILObject *obj, ILInt32 timeout)
{
	int result;	
	ILLockWord lockword;
	ILExecMonitor *monitor, *next;
	
	/* Make sure the object isn't null */
	if (obj == 0)
	{
		ILExecThreadThrowArgNull(thread, "obj");

		return 0;
	}

	/* Make sure the timeout is within range */
	if (timeout < -1)
	{
		ILExecThreadThrowArgRange(thread, "timeout", (const char *)0);

		return 0;
	}

retry:

	/* Get the lockword lock word */		
	lockword = GetObjectLockWord(thread, obj);

	if (lockword == 0)
	{
		/* There is no monitor installed for this object */

		monitor = thread->freeMonitor;

		if (monitor == NULL)
		{
			monitor = _ILExecMonitorCreate();
		}

		next = monitor->next;

		/* Try to install the new monitor. */
		if (CompareAndExchangeObjectLockWord
			(
				thread,
				obj,
				IL_LW_MARK(monitor),
				lockword
			) == lockword)
		{
			result = ILWaitMutexFastEnter(thread->supportThread, monitor->waitMutex);

			if (result != 0)
			{
				/* Dissociate the monitor from the object */
				
				SetObjectLockWord(thread, obj, 0);

				/* If the monitor is new, add it to the free list */
				if (monitor != thread->freeMonitor)
				{
					_ILExecMonitorAppendToFreeList(thread, monitor);
				}

				_ILExecThreadHandleWaitResult(thread, result);

				return 0;
			}

			/* If the monitor is from the free list, remove it from the free list */
			if (monitor == thread->freeMonitor)
			{
				thread->freeMonitorCount--;
				thread->freeMonitor = next;
			}

			/* Finally allow other threads to enter the monitor */
			_ILObjectLockWord_Unmark(thread, obj);

			return 1;
		}
		else
		{
			/* Another thread managed to install a monitor first */

			/* If the monitor is new, add it to the free list */
			if (monitor != thread->freeMonitor)
			{
				_ILExecMonitorAppendToFreeList(thread, monitor);
			}

			goto retry;
		}
	}

	/* Lock the object's lockword */
	lockword = _ILObjectLockWord_WaitAndMark(thread, obj);

	monitor = IL_LW_UNMARK(lockword);

	/* We assume that _ILObjectLockWord_WaitAndMark flushed the CPU's cache
	   so that we can see the monitor */

	if (monitor == 0)
	{
		/* Some other thread owned the monitor but has released it
		   since we last called GetObjectLockWord */

		_ILObjectLockWord_Unmark(thread, obj);

		goto retry;
	}

	/* Incrementing waiters prevents other threads from disassociating 
	   the monitor with the object or using FastLeave */

	monitor->waiters++;
	
	_ILObjectLockWord_Unmark(thread, obj);
	
	/* Here we try entering the monitor.  If we know that we own the
	   monitor then we can call ILWaitMutexFastEnter. */
	
	if (ILWaitMutexThreadOwns(thread->supportThread, monitor->waitMutex))
	{
		result = ILWaitMutexFastEnter(thread->supportThread, monitor->waitMutex);
	}
	else
	{		
		result = ILWaitMonitorTryEnter(monitor->waitMutex, timeout);
	}

	_ILObjectLockWord_WaitAndMark(thread, obj);
	--monitor->waiters;
	_ILObjectLockWord_Unmark(thread, obj);

	/* Failed or timed out somehow */
	if (result != 0)
	{
		/* Handle ThreadAbort etc */		
		_ILExecThreadHandleWaitResult(thread, result);
	}

	return result == 0;
}

/*
 * public static void Exit(Object obj);
 */
void _IL_Monitor_Exit(ILExecThread *thread, ILObject *obj)
{
	int result;
	ILLockWord lockword;
	ILExecMonitor *monitor;

	/* Make sure obj isn't null */
	if(obj == 0)
	{
		ILExecThreadThrowArgNull(thread, "obj");

		return;
	}

	/* Make sure noone is allowed to change the object's monitor */
	lockword = _ILObjectLockWord_WaitAndMark(thread, obj);

	/* We assume that _ILObjectLockWord_WaitAndMark flushed the CPU's cache
	   so that we can see the monitor */
	monitor = IL_LW_UNMARK(lockword);

	/* Make sure the monitor is valid */
	if (monitor == 0 || (monitor != 0 
		&& !ILWaitMutexThreadOwns(thread->supportThread, monitor->waitMutex)))
	{
		/* Hmm.  Can't call Monitor.Exit before Monitor.Enter */

		_ILObjectLockWord_Unmark(thread, obj);

		ILExecThreadThrowSystem
		(
			thread,
			"System.Threading.SynchronizationLockException",	
			"Exception_ThreadNeedsLock"
		);

		return;
	}

	/*
	 * If there are no waiters then we have exclusive access to the monitor.  This
	 * allows us to do a "FastRelease" and possibly return the monitor to the freelist.
	 */
	if (monitor->waiters == 0)
	{
		result = ILWaitMutexFastRelease(thread->supportThread, monitor->waitMutex);
		
		if (result == IL_WAITMUTEX_RELEASE_STILL_OWNS)
		{
			_ILObjectLockWord_Unmark(thread, obj);
		}
		else
		{
			/* Monitor no longer owned.  Return it to the free list to
			   make the next call to Monitor.Enter on this object fast */
			_ILExecMonitorAppendToFreeList(thread, monitor);

			SetObjectLockWord(thread, obj, 0);
		}
		
		return;		
	}
	
	_ILObjectLockWord_Unmark(thread, obj);

	/*
	 * Another thread is waiting so try leaving the old fashioned way.
	 */
	result = ILWaitMonitorLeave(monitor->waitMutex);

	if (result < 0)
	{
		/* We *should* own the monitor.  This must be a bug in the engine. */
		
		ILExecThreadThrowSystem
		(
			thread,					
			"System.ExecutionEngineException",
			"Exception_UnexpectedEngineState"
		);
	}
}

/*
 * public static bool InternalWait(Object obj, int timeout);
 */
ILBool _IL_Monitor_InternalWait(ILExecThread *thread,
								ILObject *obj, ILInt32 timeout)
{
	int result;
	ILLockWord lockword;
	ILExecMonitor *monitor;

	/* Make sure obj isn't null */
	if (obj == 0)
	{		
		ILExecThreadThrowArgNull(thread, "obj");

		return 0;
	}

	lockword = _ILObjectLockWord_WaitAndMark(thread, obj);
	
	monitor = IL_LW_UNMARK(lockword);

	if (monitor == 0 || (monitor != 0 
		&& !ILWaitMutexThreadOwns(thread->supportThread, monitor->waitMutex)))
	{
		_ILObjectLockWord_Unmark(thread, obj);

		/* Object needs to own monitor */

		ILExecThreadThrowSystem
		(
			thread,
			"System.Threading.SynchronizationLockException",
			"Exception_ThreadNeedsLock"
		);

		return 0;
	}

	++monitor->waiters;
	
	_ILObjectLockWord_Unmark(thread, obj);

	/* If we get here then we know that no other thread can dissociate
	   the monitor from the object because we own the monitor! */

	result = ILWaitMonitorWait(monitor->waitMutex, timeout);

	_ILObjectLockWord_WaitAndMark(thread, obj);	
	--monitor->waiters;	
	_ILObjectLockWord_Unmark(thread, obj);

	switch (result)
	{
	case 0:

		/* We *should* own the monitor.  This must be a bug in the engine. */

		ILExecThreadThrowSystem
		(
			thread,
			"System.ExecutionEngineException",
			"Exception_UnexpectedEngineState"
		);

		return 0;

	case 1:
	case IL_WAIT_TIMEOUT:
	case IL_WAIT_FAILED:

		/* Success or timed out */

		/* Returning 1 because the lock is always regained */
		return 1;

	default:

		/* Handle ThreadAbort etc */

		_ILExecThreadHandleWaitResult(thread, result);

		return 1;
	}
}

/*
 * public static void Pulse(Object obj);
 */
void _IL_Monitor_Pulse(ILExecThread *thread, ILObject *obj)
{
	int result;
	ILExecMonitor * monitor;

	if(obj == 0)
	{
		ILExecThreadThrowArgNull(thread, "obj");

		return;
	}

	_ILObjectLockWord_WaitAndMark(thread, obj);

	monitor = GetObjectMonitor(thread, obj);

	if (monitor == 0 || (monitor != 0 
		&& !ILWaitMutexThreadOwns(thread->supportThread, monitor->waitMutex)))
	{
		/* Object needs to own monitor */

		_ILObjectLockWord_Unmark(thread, obj);

		ILExecThreadThrowSystem
		(
			thread,
			"System.Threading.SynchronizationLockException",
			"Exception_ThreadNeedsLock"
		);

		return;
	}

	_ILObjectLockWord_Unmark(thread, obj);

	result = ILWaitMonitorPulse(monitor->waitMutex);

	switch (result)
	{
	case 0:

		/* We *should* own the monitor.  This must be a bug in the engine. */

		ILExecThreadThrowSystem
		(
			thread,
			"System.ExecutionEngineException",
			"Exception_UnexpectedEngineState"
		);

		return;

	case 1:

		/* Successfully pulsed */

		return;

	default:

		/* Handle ThreadAbort etc */

		_ILExecThreadHandleWaitResult(thread, result);

		return;
	}
}

/*
 * public static void PulseAll(Object obj);
 */
void _IL_Monitor_PulseAll(ILExecThread *thread, ILObject *obj)
{
	int result;	
	ILExecMonitor * monitor;

	if(obj == 0)
	{
		ILExecThreadThrowArgNull(thread, "obj");

		return;
	}

	_ILObjectLockWord_WaitAndMark(thread, obj);

	monitor = GetObjectMonitor(thread, obj);

	if (monitor == 0 || (monitor != 0 
		&& !ILWaitMutexThreadOwns(thread->supportThread, monitor->waitMutex)))
	{
		/* Object needs to own monitor */

		_ILObjectLockWord_Unmark(thread, obj);

		ILExecThreadThrowSystem
		(
			thread,
			"System.Threading.SynchronizationLockException",
			"Exception_ThreadNeedsLock"
		);

		return;
	}

	_ILObjectLockWord_Unmark(thread, obj);

	result = ILWaitMonitorPulseAll(monitor->waitMutex);

	switch (result)
	{ 
	case 0:

		/* We *should* own the monitor.  This must be a bug in the engine. */

		ILExecThreadThrowSystem
		(
			thread,
			"System.ExecutionEngineException",
			"Exception_UnexpectedEngineState"
		);

		return;

	case 1:

		/* Successfully pulsed */

		return;

	default:

		/* Handle ThreadAbort etc */

		_ILExecThreadHandleWaitResult(thread, result);

		return;
	}
}

#ifdef	__cplusplus
};
#endif
