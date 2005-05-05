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
	ILMethod* method;
	ILClass *classInfo;
	ILExecValue result, arg;
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

		/* Associate the executing thread with the CLR thread */
		thread->clrThread = (ILObject *)clrThread;
		
		/* Execute the private constructor */

		method = ILExecThreadLookupMethod(thread, "System.Threading.Thread", ".ctor", "(Tj)V");

		if (method == 0)
		{
			ILExecThreadThrowSystem(thread, "System.NotImplementedException", "System.Threading.Thread..ctor()");

			return 0;
		}
		
		/* Pass the OS thread as an argument to the CLR thread's constructor */
		arg.ptrValue = ILThreadSelf();

		_ILCallMethod
		(
			thread,
			method,
			_ILCallUnpackDirectResult,
			&result,
			0,
			clrThread,
			_ILCallPackVParams,
			&arg
		);
	}

	return thread->clrThread;
}


#if defined(IL_USE_INTERRUPT_BASED_X)

static void _ILInterruptHandler(ILInterruptContext *context)
{
	ILExecThread *execThread;

	execThread = ILExecThreadCurrent();
	
	if (execThread == 0)
	{
		return;
	}

	if (execThread->runningManagedCode)
	{
		switch (context->type)
		{
		#if defined(IL_USE_INTERRUPT_BASED_NULL_POINTER_CHECKS)
			case IL_INTERRUPT_TYPE_ILLEGAL_MEMORY_ACCESS:
				execThread->interruptContext = *context;
				IL_LONGJMP(execThread->exceptionJumpBuffer, _IL_INTERRUPT_NULL_POINTER);
		#endif

		#if defined(IL_INTERRUPT_SUPPORTS_INT_DIVIDE_BY_ZERO)
			case IL_INTERRUPT_TYPE_INT_DIVIDE_BY_ZERO:
				execThread->interruptContext = *context;
				IL_LONGJMP(execThread->exceptionJumpBuffer, _IL_INTERRUPT_INT_DIVIDE_BY_ZERO);
		#endif

		#if defined(IL_USE_INTERRUPT_BASED_INT_OVERFLOW_CHECKS)
			case IL_INTERRUPT_TYPE_INT_OVERFLOW:
				execThread->interruptContext = *context;
				IL_LONGJMP(execThread->exceptionJumpBuffer, _IL_INTERRUPT_INT_OVERFLOW);
		#endif
		}
	}
}

#endif

void _ILThreadSetExecContext(ILThread *thread, ILThreadExecContext *context, ILThreadExecContext *saveContext)
{
	if (saveContext)
	{
		_ILThreadSaveExecContext(thread, saveContext);
	}

	ILThreadSetObject(thread, context->execThread);

	#if defined(IL_USE_INTERRUPT_BASED_X)
		if (context->execThread)
		{
			ILThreadRegisterInterruptHandler(thread, _ILInterruptHandler);
		}
	#endif

	context->execThread->supportThread = thread;
}

void _ILThreadSaveExecContext(ILThread *thread, ILThreadExecContext *saveContext)
{
	saveContext->execThread = ILThreadGetObject(thread);
}

void _ILThreadRestoreExecContext(ILThread *thread, ILThreadExecContext *context)
{
	ILThreadSetObject(thread, context->execThread);

	if (context->execThread)
	{
		context->execThread->supportThread = thread;
	
		#if defined(IL_USE_INTERRUPT_BASED_X)
			ILThreadRegisterInterruptHandler(thread, _ILInterruptHandler);
		#endif
	}
	else
	{
		#if defined(IL_USE_INTERRUPT_BASED_X)
			ILThreadUnregisterInterruptHandler(thread, _ILInterruptHandler);
		#endif
	}
}

void _ILThreadClearExecContext(ILThread *thread)
{
	ILExecThread *prev;

	prev = ILThreadGetObject(thread);

	if (prev)
	{
		prev->supportThread = 0;
	}

	#if defined(IL_USE_INTERRUPT_BASED_X)
		ILThreadUnregisterInterruptHandler(thread, _ILInterruptHandler);
	#endif

	ILThreadSetObject(thread, 0);}

/*
 *	Cleanup handler for threads that have been registered for managed execution.
 */
static void ILExecThreadCleanup(ILThread *thread)
{
	ILThreadUnregisterForManagedExecution(thread);
}

/*
 * Registers a thread for managed execution
 */
ILExecThread *ILThreadRegisterForManagedExecution(ILExecProcess *process, ILThread *thread)
{	
	ILExecThread *execThread;
	ILThreadExecContext context;

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

	/* TODO: Notify the GC that we possibly have a new thread to be scanned */

	context.execThread = execThread;

	_ILThreadSetExecContext(thread, &context, 0);

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

	/* Unregister the cleanup handler */
	ILThreadUnregisterCleanup(thread, ILExecThreadCleanup);

	monitor = ILThreadGetMonitor(thread);

	ILWaitMonitorEnter(monitor);
	{
		/* Disassociate the engine thread with the support thread */
		_ILThreadClearExecContext(thread);

		/* Destroy the engine thread */
		_ILExecThreadDestroy(execThread);
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

ILInt32 _ILExecThreadGetState(ILExecThread *thread, ILThread* supportThread)
{
	return ILThreadGetState(supportThread);
}

void _ILExecThreadResumeThread(ILExecThread *thread, ILThread *supportThread)
{
	ILWaitHandle *monitor;
	ILExecThread *execThread;

	monitor = ILThreadGetMonitor(supportThread);

	/* Locking the monitor prevents the associated engine thread from being destroyed */
	ILWaitMonitorEnter(monitor);

	execThread = _ILExecThreadFromThread(supportThread);

	if (execThread == 0)
	{
		/* The thread has finished running */

		ILWaitMonitorLeave(monitor);

		ILExecThreadThrowSystem
			(
				thread,
				"System.Threading.ThreadStateException",
				(const char *)0
			);

		return;
	}

	/* Remove the _IL_MANAGED_SAFEPOINT_THREAD_SUSPEND flag */
	ILThreadAtomicStart();			
	execThread->managedSafePointFlags &= ~_IL_MANAGED_SAFEPOINT_THREAD_SUSPEND;
	ILThreadAtomicEnd();

	/* Call the threading subsystem's resume */
	ILThreadResume(supportThread);

	ILWaitMonitorLeave(monitor);
}

/*
 * Suspend the given thread.
 * If the thread is a in wait/sleep/join state or the thread is executing non-managed code
 * then a suspend request will be made and the method will exit immediately.  The suspend
 * request will be processed either by the thread subsystem (when the thread next enters
 * and exits a wait/sleep/join state) or by the engine (when the thread next enters a 
 * managed safepoint).
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
		/* Suspend the current thread */
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

		return;
	}

	/* Entering the monitor keeps the execThread from being destroyed */
	ILWaitMonitorEnter(monitor);

	execThread = _ILExecThreadFromThread(supportThread);

	if (execThread == 0)
	{
		/* The thread is dead */
		result = IL_SUSPEND_FAILED;
	}
	else
	{
		result = ILThreadSuspendRequest(supportThread, !execThread->runningManagedCode);
	}

	if (result == IL_SUSPEND_FAILED)
	{
		ILExecThreadThrowSystem
			(
				thread,
				"System.Threading.ThreadStateException",
				(const char *)0
			);

		ILWaitMonitorLeave(monitor);

		return;
	}
	else if (result == IL_SUSPEND_REQUESTED)
	{		
		/* In order to prevent a suspend_request from being processed twice (once by
		    the threading subsystem and twice by the engine when it detects the safepoint
		    flags, the engine needs to check the ThreadState after checking the safepoint
		    flags (see cvm_call.c) */

		ILThreadAtomicStart();			
		execThread->managedSafePointFlags |= _IL_MANAGED_SAFEPOINT_THREAD_SUSPEND;
		ILThreadAtomicEnd();

		ILWaitMonitorLeave(monitor);

		return;
	}
	else if (result < 0)
	{
		if (_ILExecThreadHandleWaitResult(thread, result) != 0)
		{
			ILWaitMonitorLeave(monitor);

			return;
		}
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
#ifdef IL_CONFIG_APPDOMAINS
					(sizeof(CVMWord) * process->engine->stackSize)) == 0)
#else
					(sizeof(CVMWord) * process->stackSize)) == 0)
#endif
	{
		ILGCFreePersistent(thread);
		return 0;
	}
#ifdef IL_CONFIG_APPDOMAINS
	thread->stackLimit = thread->stackBase + process->engine->stackSize;
#else
	thread->stackLimit = thread->stackBase + process->stackSize;
#endif

	/* Allocate space for the initial frame stack */
	if((thread->frameStack = (ILCallFrame *)ILGCAllocPersistent
#ifdef IL_CONFIG_APPDOMAINS
					(sizeof(ILCallFrame) * process->engine->frameStackSize))
#else
					(sizeof(ILCallFrame) * process->frameStackSize))
#endif
			== 0)
	{
		ILGCFreePersistent(thread->stackBase);
		ILGCFreePersistent(thread);
		return 0;
	}

	thread->numFrames = 0;
#ifdef IL_CONFIG_APPDOMAINS
	thread->maxFrames = process->engine->frameStackSize;
#else
	thread->maxFrames = process->frameStackSize;
#endif

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

		ILMutexUnlock(process->lock);

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
		_ILThreadClearExecContext(thread->supportThread);
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
