/*
 * thread.c - Manage threads within the runtime engine.
 *
 * Copyright (C) 2001  Southern Storm Software, Pty Ltd.
 *
 * Contributions from Thong Nguyen <tum@veridicus.com>
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

#include "engine_private.h"
#include "lib_defs.h"
#include "interrupt.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Get an ILExecThread from the given support thread.
 */
static ILExecThread *_ILExecThreadFromThread(ILThread *thread)
{
	return (ILExecThread *)(ILThreadGetObject(thread));
}

/*
 * Gets the currently ILExecThread.
 */
ILExecThread *ILExecThreadCurrent()
{
	return _ILExecThreadFromThread(ILThreadSelf());
}

/*
 * Gets the managed thread object from an engine thread.
 */
ILObject *ILExecThreadCurrentClrThread()
{
	ILClass *classInfo;
	System_Thread *clrThread;
	ILExecThread *thread = ILExecThreadCurrent();

	if (thread == 0)
	{
		return 0;
	}

	if (thread->clrThread == 0)
	{
		/* Main thread or another thread created from inside the engine. */
	
		/* Get the CLR thread class */

		classInfo = ILExecThreadLookupClass(thread, "System.Threading.Thread");		

		/* Allocate a new CLR thread object */

		clrThread = (System_Thread *)_ILEngineAllocObject(thread, classInfo);

		/* Associate the CLR thread object with the OS thread */

		clrThread->privateData = ILThreadSelf();

		/* Associate the executing thread with the CLR thread */
		thread->clrThread = (ILObject *)clrThread;
	}

	return thread->clrThread;
}

/*
 * Associates a support thread with an engine thread and the engine
 * thread with the support thread.
 */
void _ILThreadExecuteOn(ILThread *thread, ILExecThread *execThread)
{
	ILThreadSetObject(thread, execThread);
	execThread->supportThread = thread;
}

/*
 * Disassociates a support thread with an engine thread and the
 * engine thread with the support thread.
 * The support thread will no longer be able to execute managed
 * code and the engine thread will no longer be able to be used
 * to execute anything until it is reassociated using ILThreadExecuteOn.
 */
void _ILThreadUnexecuteOn(ILThread *thread, ILExecThread *execThread)
{
	if (ILThreadGetObject(thread) != execThread)
	{
		return;
	}

	ILThreadSetObject(thread, 0);
	execThread->supportThread = 0;
}

/*
 *	Cleanup handler for threads that have been registered for managed execution.
 */
static void ILExecThreadCleanup(ILThread *thread)
{
	ILThreadUnregisterForManagedExecution(thread);
}

#if defined(IL_USE_INTERRUPT_BASED_NULL_POINTER_CHECKS)

static void _ILIllegalMemoryAccessHandler(ILInterruptContext *context)
{
	ILExecThread *execThread;

	execThread = ILExecThreadCurrent();

	if (execThread == 0)
	{
		return;
	}

	if (execThread->runningManagedCode)
	{
		execThread->interruptContext = *context;
		IL_LONGJMP(execThread->exceptionJumpBuffer, _IL_INTERRUPT_NULL_POINTER);
	}
}

#endif

/*
 * Registers a thread for managed execution
 */
ILExecThread *ILThreadRegisterForManagedExecution(ILExecProcess *process, ILThread *thread)
{	
	ILExecThread *execThread;

	if (process->state & (_IL_PROCESS_STATE_UNLOADED | _IL_PROCESS_STATE_UNLOADING))
	{
		return 0;
	}

	/* If the thread has already been registerered then return the existing engine thread */
	if ((execThread = ILThreadGetObject(thread)) != 0)
	{
		return execThread;
	}

	/* Create a new engine-level thread */	
	if ((execThread = _ILExecThreadCreate(process, 0)) == 0)
	{
		return 0;
	}

#if defined(IL_USE_INTERRUPT_BASED_NULL_POINTER_CHECKS)
	ILThreadRegisterIllegalMemoryAccessHandler(thread, _ILIllegalMemoryAccessHandler);
#endif

	/* TODO: Notify the GC that we possibly have a new thread to be scanned */

	/* Associate the new engine-level thread with the OS-level thread (and vice-versa) */
	_ILThreadExecuteOn(thread, execThread);

	/* Register a cleanup handler for the thread */
	ILThreadRegisterCleanup(thread, ILExecThreadCleanup);

	return execThread;
}

/*
 *	Unregisters a thread for managed execution.
 */
void ILThreadUnregisterForManagedExecution(ILThread *thread)
{
	ILWaitHandle *monitor;
	ILExecThread *execThread;

	/* Get the engine thread from the support thread */
	execThread = ILThreadGetObject(thread);

	if (execThread == 0)
	{
		/* Already unregistered */
		return;
	}

#if defined(IL_USE_INTERRUPT_BASED_NULL_POINTER_CHECKS)
	/* Unregister the illegal memory access handler */
	ILThreadUnregisterIllegalMemoryAccessHandler(thread, _ILIllegalMemoryAccessHandler);
#endif

	/* Unregister the cleanup handler */
	ILThreadUnregisterCleanup(thread, ILExecThreadCleanup);

	monitor = ILThreadGetMonitor(thread);

	ILWaitMonitorEnter(monitor);
	{
		/* Disassociate the engine thread with the support thread */
		_ILThreadUnexecuteOn(thread, execThread);

		/* Destroy the engine thread */
		_ILExecThreadDestroy(execThread);

		ILWaitMonitorPulseAll(monitor);
	}
	ILWaitMonitorLeave(monitor);
}

/*
 * Called by the current thread when it was to begin its abort sequence.
 * Returns 0 if the thread has successfully self aborted.
 */
int _ILExecThreadSelfAborting(ILExecThread *thread)
{
	ILObject *exception;

	/* Determine if we currently have an abort in progress,
	   or if we need to throw a new abort exception */
	if(!ILThreadIsAborting())
	{
		if(ILThreadSelfAborting())
		{
			/* Allocate an instance of "ThreadAbortException" and throw */
			
			exception = _ILExecThreadNewThreadAbortException(thread, 0);
			
			ILThreadAtomicStart();
			thread->managedSafePointFlags &= ~_IL_MANAGED_SAFEPOINT_THREAD_ABORT;
			ILThreadAtomicEnd();

			thread->aborting = 1;
			thread->abortHandlerEndPC = 0;
			thread->threadAbortException = 0;
			thread->abortHandlerFrame = 0;

			ILExecThreadSetException(thread, exception);

			return 0;
		}
	}

	return -1;
}

/*
 * Suspend the given thread.
 */
void _ILExecThreadSuspendThread(ILExecThread *thread, ILThread *supportThread)
{
	int result;
	ILWaitHandle *monitor;
	ILExecThread *execThread;
	
	monitor = ILThreadGetMonitor(supportThread);

	/* If the thread being suspended is the current thread then suspend now */
	if (thread->supportThread == supportThread)
	{
		result = ILThreadSuspend(supportThread);

		ILWaitMonitorEnter(monitor);
		ILWaitMonitorPulseAll(monitor);
		ILWaitMonitorLeave(monitor);

		if (result == 0)
		{
			ILExecThreadThrowSystem
				(
					thread,
					"System.Threading.ThreadStateException",
					(const char *)0
				);

			return;
		}
		else if (result < 0)
		{
			if (_ILExecThreadHandleWaitResult(thread, result) != 0)
			{
				return;
			}
		}

		return;
	}

	/* Suspend the thread.  It isn't safe to do this for long but we verify
	   that later on... */
	result = ILThreadSuspend(supportThread);

	if (result == 0)
	{
		ILExecThreadThrowSystem
			(
				thread,
				"System.Threading.ThreadStateException",
				(const char *)0
			);

		return;
	}
	else if (result < 0)
	{
		if (_ILExecThreadHandleWaitResult(thread, result) != 0)
		{
			return;
		}
	}

	/* Entering the monitor keeps the execThread from being destroyed */
	ILWaitMonitorEnter(monitor);

	execThread = _ILExecThreadFromThread(supportThread);

	if (execThread == 0)
	{	
		/* The thread has already finished */
		ILWaitMonitorLeave(monitor);
	}
	else
	{
		if (!execThread->runningManagedCode)
		{
			/* The thread isn't running managed code so it is unsafe to
			   keep the thread suspended like this cause we might hold
			   a CRT or some other important OS lock */

			/* Set the flag so the thread will abort itself at the next safe point */
			ILThreadAtomicStart();			
			execThread->managedSafePointFlags |= _IL_MANAGED_SAFEPOINT_THREAD_SUSPEND;
			ILThreadAtomicEnd();

			/* Resume the thread */
			ILThreadResume(supportThread);

			/* Wait until the thread aborts itself */
			ILWaitMonitorWait(monitor, -1);
		}		

		ILWaitMonitorLeave(monitor);
	}
}

/*
 * Abort the given thread.
 */
void _ILExecThreadAbortThread(ILExecThread *thread, ILThread *supportThread)
{
	ILWaitHandle *monitor;
	ILExecThread *execThread;

	monitor = ILThreadGetMonitor(supportThread);

	/* Prevent the ILExecThread from being destroyed while
	   we are accessing it */
	ILWaitMonitorEnter(monitor);

	execThread = _ILExecThreadFromThread(supportThread);

	if (execThread == 0)
	{	
		/* The thread has already finished */
		ILWaitMonitorLeave(monitor);

		return;
	}
	else
	{
		/* Mark the flag so the thread self aborts when it next returns
		   to managed code */
		ILThreadAtomicStart();
		execThread->managedSafePointFlags |= _IL_MANAGED_SAFEPOINT_THREAD_ABORT;
		ILThreadAtomicEnd();
	}

	ILWaitMonitorLeave(monitor);

	/* Abort the thread if its in or when it next enters a wait/sleep/join
	   state */
	ILThreadAbort(supportThread);

	/* If the current thread is aborting itself then abort immediately */
	if (supportThread == thread->supportThread)
	{
		_ILExecThreadSelfAborting(execThread);		
	}
}

/*
 * Handle the result from an "ILWait*" function call.
 */
int _ILExecThreadHandleWaitResult(ILExecThread *thread, int result)
{
	switch (result)
	{
	case IL_WAIT_INTERRUPTED:
		{
			ILExecThreadThrowSystem
				(
				thread,
				"System.Threading.ThreadInterruptedException",
				(const char *)0
				);
		}
		break;
	case IL_WAIT_ABORTED:
		{
			_ILExecThreadSelfAborting(thread);			
		}
		break;
	case IL_WAIT_FAILED:
		{
			ILExecThreadThrowSystem
				(
					thread,
					"System.Threading.SystemException",
					(const char *)0
				);
		}
	}

	return result;
}

ILExecThread *_ILExecThreadCreate(ILExecProcess *process, int ignoreProcessState)
{
	ILExecThread *thread;

	/* Create a new thread block */
	if((thread = (ILExecThread *)ILGCAllocPersistent
									(sizeof(ILExecThread))) == 0)
	{
		return 0;
	}

	/* Allocate space for the thread-specific value stack */
	if((thread->stackBase = (CVMWord *)ILGCAllocPersistent
					(sizeof(CVMWord) * process->stackSize)) == 0)
	{
		ILGCFreePersistent(thread);
		return 0;
	}
	thread->stackLimit = thread->stackBase + process->stackSize;

	/* Allocate space for the initial frame stack */
	if((thread->frameStack = (ILCallFrame *)ILGCAllocPersistent
					(sizeof(ILCallFrame) * process->frameStackSize))
			== 0)
	{
		ILGCFreePersistent(thread->stackBase);
		ILGCFreePersistent(thread);
		return 0;
	}

	thread->numFrames = 0;
	thread->maxFrames = process->frameStackSize;

	/* Initialize the thread state */
	thread->supportThread = 0;
	thread->clrThread = 0;
	thread->aborting = 0;
	thread->freeMonitor = 0;
	thread->freeMonitorCount = 0;
	thread->pc = 0;
	thread->isFinalizerThread = 0;
	thread->frame = thread->stackBase;
	thread->stackTop = thread->stackBase;
	thread->method = 0;
	thread->thrownException = 0;	
	thread->threadStaticSlots = 0;
	thread->threadStaticSlotsUsed = 0;
	thread->currentException = 0;
	thread->managedSafePointFlags = 0;
	thread->runningManagedCode = 0;	
	thread->abortHandlerEndPC = 0;
	thread->threadAbortException = 0;
	thread->abortHandlerFrame = 0;
	/* Attach the thread to the process */
	ILMutexLock(process->lock);
	if (!ignoreProcessState
		&& process->state & (_IL_PROCESS_STATE_UNLOADED | _IL_PROCESS_STATE_UNLOADING))
	{
		ILGCFreePersistent(thread->stackBase);
		ILGCFreePersistent(thread->frameStack);
		ILGCFreePersistent(thread);

		return 0;
	}
	thread->process = process;
	thread->nextThread = process->firstThread;
	thread->prevThread = 0;
	if(process->firstThread)
	{
		process->firstThread->prevThread = thread;
	}
	process->firstThread = thread;

	ILMutexUnlock(process->lock);
	
	/* Return the thread block to the caller */
	return thread;
}

void _ILExecThreadDestroy(ILExecThread *thread)
{	
	ILExecProcess *process = thread->process;

	/* Lock down the process */
	ILMutexLock(process->lock);

	/* If this is the "main" thread, then clear "process->mainThread" */
	if(process->mainThread == thread)
	{
		process->mainThread = 0;
	}

	if (process->finalizerThread == thread)
	{
		process->finalizerThread = 0;
	}

	/* Detach the thread from its process */
	if(thread->nextThread)
	{
		thread->nextThread->prevThread = thread->prevThread;
	}
	if(thread->prevThread)
	{
		thread->prevThread->nextThread = thread->nextThread;
	}
	else
	{
		process->firstThread = thread->nextThread;
	}

	/* Remove associations between ILExecThread and ILThread if they
	   haven't already been removed */
	if (thread->supportThread)
	{		
		_ILThreadUnexecuteOn(thread->supportThread, thread);
	}

	/* Destroy the operand stack */
	ILGCFreePersistent(thread->stackBase);

	/* Destroy the call frame stack */
	ILGCFreePersistent(thread->frameStack);

	/* Destroy the thread block */
	ILGCFreePersistent(thread);

	/* Unlock the process */
	ILMutexUnlock(process->lock);
	
}

ILExecProcess *ILExecThreadGetProcess(ILExecThread *thread)
{
	return thread->process;
}

ILCallFrame *_ILGetCallFrame(ILExecThread *thread, ILInt32 n)
{
	ILCallFrame *frame;
	ILUInt32 posn;
	if(n < 0)
	{
		return 0;
	}
	posn = thread->numFrames;
	while(posn > 0)
	{
		--posn;
		frame = &(thread->frameStack[posn]);
		if(!n)
		{
			return frame;
		}
		--n;
	}
	return 0;
}

ILCallFrame *_ILGetNextCallFrame(ILExecThread *thread, ILCallFrame *frame)
{
	ILUInt32 posn;
	posn = frame - thread->frameStack;
	if(posn > 0)
	{
		--posn;
		return &(thread->frameStack[posn]);
	}
	else
	{
		return 0;
	}
}

ILMethod *ILExecThreadStackMethod(ILExecThread *thread, unsigned long num)
{
	ILCallFrame *frame;
	if(!num)
	{
		return thread->method;
	}
	frame = _ILGetCallFrame(thread, (ILInt32)(num - 1));
	if(frame)
	{
		return frame->method;
	}
	else
	{
		return 0;
	}
}

#ifdef	__cplusplus
};
#endif
