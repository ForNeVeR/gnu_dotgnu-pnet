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

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Gets the currently ILExecThread.
 */
ILExecThread *ILExecThreadCurrent(void)
{
	return ILExecThreadFromThread(ILThreadSelf());
}

/*
 * Get an ILExecThread from the given thread.
 */
ILExecThread *ILExecThreadFromThread(ILThread *thread)
{
	return (ILExecThread *)(ILThreadGetObject(thread));
}

/*
 * Gets the managed thread object from an engine thread.
 */
ILObject *ILExecThreadGetClrThread(ILExecThread *thread)
{
	ILClass *classInfo;
	System_Thread *clrThread;

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

/*
 *	Registers a thread for managed execution
 */
ILExecThread *ILThreadRegisterForManagedExecution(ILExecProcess *process, ILThread *thread)
{	
	ILExecThread *execThread;

	/* If the thread has already been registerered then return the existing engine thread */
	if ((execThread = ILThreadGetObject(thread)) != 0)
	{
		return execThread;
	}

	/* Create a new engine-level thread */	
	if ((execThread = _ILExecThreadCreate(process)) == 0)
	{
		return 0;
	}

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

	/* Disassociate the engine thread with the support thread */
	_ILThreadUnexecuteOn(thread, execThread);

	/* Destroy the engine thread */
	_ILExecThreadDestroy(execThread);
}

ILExecThread *_ILExecThreadCreate(ILExecProcess *process)
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
	thread->abortRequested = 0;	
	thread->runningManagedCode = 0;	
	thread->abortHandlerEndPC = 0;
	thread->threadAbortException = 0;
	thread->abortHandlerFrame = 0;
	
	/* Attach the thread to the process */
	ILMutexLock(process->lock);
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
