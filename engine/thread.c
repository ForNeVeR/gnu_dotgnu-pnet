/*
 * thread.c - Manage threads within the runtime engine.
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

#include "engine_private.h"

#ifdef	__cplusplus
extern	"C" {
#endif

ILExecThread *ILExecThreadCurrent(void)
{
	return (ILExecThread *)(ILThreadGetObject(ILThreadSelf()));
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
	thread->osThread = 0;
	thread->pc = 0;
	thread->frame = thread->stackBase;
	thread->stackTop = thread->stackBase;
	thread->method = 0;
	thread->thrownException = 0;
	thread->securityManager = 0;

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

ILExecThread *ILExecThreadCreate(ILExecProcess *process)
{
	ILExecThread *thread = _ILExecThreadCreate(process);
	/* TODO: initialize underlying the OS thread */
	return thread;
}

void ILExecThreadDestroy(ILExecThread *thread)
{
	ILExecProcess *process = thread->process;

	/* Lock down the process */
	ILMutexLock(process->lock);

	/* If this is the "main" thread, then clear "process->mainThread" */
	if(process->mainThread == thread)
	{
		process->mainThread = 0;
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
