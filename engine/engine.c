/*
 * engine.c - Manage multiple application domains in the runtime engine.
 *
 * Copyright (C) 2001  Southern Storm Software, Pty Ltd.
 *
 * Contributions by Klaus Treichel (ktreichel@web.de)
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
#include "il_utils.h"
#include "il_console.h"


#ifdef	__cplusplus
extern	"C" {
#endif

#ifdef IL_CONFIG_APPDOMAINS
/* global engine object */
ILExecEngine *globalEngine;
#endif

#ifdef IL_CONFIG_APPDOMAINS
int ILExecInit(unsigned long maxSize, unsigned long stackSize)
#else
int ILExecInit(unsigned long maxSize)
#endif
{
#ifdef IL_CONFIG_APPDOMAINS
	globalEngine = 0;
#endif
#if !defined(IL_CONFIG_REDUCE_CODE) && !defined(IL_WITHOUT_TOOLS)
	/* Create the global trace mutex */
	if ((globalTraceMutex = ILMutexCreate()) == 0)
	{
		return IL_EXEC_INIT_OUTOFMEMORY;
	}
#endif

	/* Initialize the thread routines */	
	ILThreadInit();

	/* Initialize the global garbage collector */	
	ILGCInit(maxSize);

#ifdef IL_CONFIG_APPDOMAINS
	globalEngine = ILExecEngineCreate(stackSize);
	if (!globalEngine)
	{
		return IL_EXEC_INIT_OUTOFMEMORY;
	}	
#endif

	return IL_EXEC_INIT_OK;
}

void ILExecDeinit()
{	
#ifdef IL_CONFIG_APPDOMAINS
	if (globalEngine)
	{
		ILExecEngineDestroy(globalEngine);
	}
#endif

	/* Deinitialize the global garbage collector */	
	ILGCDeinit();	

	/* Deinitialize the thread routines */	
	ILThreadDeinit();	

#if !defined(IL_CONFIG_REDUCE_CODE) && !defined(IL_WITHOUT_TOOLS)
	/* Destroy the global trace mutex */
	ILMutexDestroy(globalTraceMutex);
#endif
}


#ifdef IL_CONFIG_APPDOMAINS
/* global accessor function to get the global engine object */
ILExecEngine *ILExecEngineInstance()
{
	return globalEngine;
}

/*
 * Create a new execution engine.
 * Returns the ILExecEngine or 0 if the function fails.
 */
ILExecEngine *ILExecEngineCreate(unsigned long stackSize)
{
	ILExecEngine *engine;

	/* Create the engine record */
	if((engine = (ILExecEngine *)ILGCAllocPersistent
						(sizeof(ILExecEngine))) == 0)
	{
		return 0;
	}
	/* Initialize the fields */
	engine->processLock = 0;
	engine->firstProcess = 0;
	engine->defaultProcess = 0;
	engine->stackSize = ((stackSize < IL_CONFIG_STACK_SIZE)
							? IL_CONFIG_STACK_SIZE : stackSize);
	engine->frameStackSize = IL_CONFIG_FRAME_STACK_SIZE;


	/* Initialize the process lock */
	engine->processLock = ILMutexCreate();
	if(!(engine->processLock))
	{
		ILExecEngineDestroy(engine);
		return 0;
	}
	
	/* Return the engine record to the caller */
	return engine;
}

/*
 * Destroy the engine.
 */
void ILExecEngineDestroy(ILExecEngine *engine)
{
	int count;
	ILExecProcess *process;	
	ILQueueEntry *unloadQueue, *destroyQueue;
	
	unloadQueue = ILQueueCreate();
	destroyQueue = ILQueueCreate();

	/* Lock the engine process list*/
	ILMutexLock(engine->processLock);

	count = 0;

	/* Walk all the application domains and collect them */

	process = engine->firstProcess;

	while (process)
	{
		if (process != engine->defaultProcess)
		{
			if(process->state < _IL_PROCESS_STATE_UNLOADING)
			{
				ILQueueAdd(&unloadQueue, process);

			}
			ILQueueAdd(&destroyQueue, process);
			count++;
		}
		/* Move onto the next process */
		process = process->nextProcess;
	}

	/* Unlock the engine process list */
	ILMutexUnlock(engine->processLock);

	if ((!unloadQueue || !destroyQueue) && count != 0)
	{
		/* Probably ran out of memory trying to build the unload and destroy
			queue */
		return;
	}

	/* unload all processes */
	while (unloadQueue)
	{
		process = (ILExecProcess *)ILQueueRemove(&unloadQueue);
		ILExecProcessUnload(process);
	}

	/* now destroy the processes */
	while (destroyQueue)
	{
		process = (ILExecProcess *)ILQueueRemove(&destroyQueue);
		ILExecProcessDestroy(process);
	}
 
	/* now unload and destroy the default process */
	if(engine->defaultProcess)
	{
		ILExecProcessUnload(engine->defaultProcess);
		ILExecProcessDestroy(engine->defaultProcess);
	}

	if (engine->processLock)
	{
		/* Destroy the process list lock */
		ILMutexDestroy(engine->processLock);
	}

	/* Free the engine block itself */
	ILGCFreePersistent(engine);
}
#endif
#ifdef	__cplusplus
};
#endif

