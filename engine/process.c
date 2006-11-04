/*
 * process.c - Manage processes within the runtime engine.
 *
 * Copyright (C) 2001  Southern Storm Software, Pty Ltd.
 *
 * Contributions by Thong Nguyen (tum@veridicus.com)
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
/*
 * Add an application domain to the list of application domains.
 * param:	process = application domain to join to the linked list
 *			(must be not null)
 *			engine  = ILExecEngine to join.
 * Returns: void
 */
static IL_INLINE void ILExecProcessJoinEngine(ILExecProcess *process,
												ILExecEngine *engine)
{
	ILMutexLock(engine->processLock);

	process->engine = engine;
	process->nextProcess = engine->firstProcess;
	process->prevProcess = 0;
	if(engine->firstProcess)
	{
		engine->firstProcess->prevProcess = process;
	}
	engine->firstProcess = process;

	if (!engine->defaultProcess)
	{
		engine->defaultProcess = process;
	}
	ILMutexUnlock(engine->processLock);
}

/*
 * Remove an application domain from the list of application domains.
 * param:	process = application domain to remove from the linked list
 *			(must be not null)
 * Returns: void
 */
static IL_INLINE void ILExecProcessDetachFromEngine(ILExecProcess *process)
{
	ILExecEngine *engine = process->engine;
	
	ILMutexLock(engine->processLock);

	/* Detach the application domain from its process */
	if(process->nextProcess)
	{
		process->nextProcess->prevProcess = process->prevProcess;
	}
	if(process->prevProcess)
	{
		process->prevProcess->nextProcess = process->nextProcess;
	}
	else
	{
		engine->firstProcess = process->nextProcess;
	}

	if (engine->defaultProcess == process)
	{
		engine->defaultProcess = 0;
	}
	ILMutexUnlock(engine->processLock);

	/* reset the links */
	process->engine = 0,
	process->prevProcess = 0;
	process->nextProcess = 0;
}
#endif

#ifdef IL_CONFIG_APPDOMAINS
ILExecProcess *ILExecProcessCreate(unsigned long cachePageSize)
#else
ILExecProcess *ILExecProcessCreate(unsigned long stackSize, unsigned long cachePageSize)
#endif
{
	ILExecProcess *process;

	/* Create the process record */
	if((process = (ILExecProcess *)ILGCAllocPersistent
						(sizeof(ILExecProcess))) == 0)
	{
		return 0;
	}
	/* Initialize the fields */
	process->lock = 0;
	process->state = _IL_PROCESS_STATE_CREATED;
	process->firstThread = 0;
	process->mainThread = 0;
	process->finalizerThread = 0;
	process->context = 0;
	process->metadataLock = 0;
	process->exitStatus = 0;
	process->coder = 0;
	process->objectClass = 0;
	process->stringClass = 0;
	process->exceptionClass = 0;
	process->clrTypeClass = 0;
	process->outOfMemoryObject = 0;	
	process->commandLineObject = 0;
	process->threadAbortClass = 0;
	ILGetCurrTime(&(process->startTime));
	process->internHash = 0;
	process->reflectionHash = 0;
	process->loadedModules = 0;
	process->gcHandles = 0;
	process->entryImage = 0;
	process->internalClassTable = 0;
	process->firstClassPrivate = 0;
#ifdef IL_CONFIG_DEBUG_LINES
	process->debugHookFunc = 0;
	process->debugHookData = 0;
	process->debugWatchList = 0;
	process->debugWatchAll = 0;
#endif
	process->randomBytesDelivered = 1024;
	process->randomLastTime = 0;
	process->randomCount = 0;
	process->numThreadStaticSlots = 0;
	process->loadFlags = IL_LOADFLAG_FORCE_32BIT;
#ifdef IL_USE_IMTS
	process->imtBase = 1;
#endif

#ifdef IL_CONFIG_APPDOMAINS
	process->engine = 0;
	ILExecProcessJoinEngine(process, ILExecEngineInstance());
#else
	process->stackSize = ((stackSize < IL_CONFIG_STACK_SIZE)
							? IL_CONFIG_STACK_SIZE : stackSize);
	process->frameStackSize = IL_CONFIG_FRAME_STACK_SIZE;
#endif
	/* Initialize the image loading context */
	if((process->context = ILContextCreate()) == 0)
	{
		ILExecProcessDestroy(process);
		return 0;
	}

	/* Associate the process with the context */
	ILContextSetUserData(process->context, process);

#ifdef IL_USE_JIT
	/* Initialize the JIT coder */
	process->coder = ILCoderCreate(&_ILJITCoderClass, process, 100000, cachePageSize);
#else
	/* Initialize the CVM coder */
	process->coder = ILCoderCreate(&_ILCVMCoderClass, process, 100000, cachePageSize);
#endif
	if(!(process->coder))
	{
		ILExecProcessDestroy(process);
		return 0;
	}

	/* Initialize the object lock */
	process->lock = ILMutexCreate();
	if(!(process->lock))
	{
		ILExecProcessDestroy(process);
		return 0;
	}

	/* Initialize the finalization context */
	process->finalizationContext = (ILFinalizationContext *)ILGCAlloc(sizeof(ILFinalizationContext));
	if (!process->finalizationContext)
	{
		ILExecProcessDestroy(process);
		return 0;
	}

	if (!_ILExecMonitorProcessCreate(process))
	{
		ILExecProcessDestroy(process);
		return 0;
	}

	process->finalizationContext->process = process;

	/* Initialize the metadata lock */
	process->metadataLock = ILRWLockCreate();
	if(!(process->metadataLock))
	{
		ILExecProcessDestroy(process);
		return 0;
	}

	/* Register the main thread for managed execution */
	process->mainThread = ILThreadRegisterForManagedExecution(process, ILThreadSelf());
	
	if(!(process->mainThread))
	{
		ILExecProcessDestroy(process);
		return 0;
	}

	/* If threading isn't supported, then the main thread is the finalizer thread */
	if (!ILHasThreads())
	{
		process->finalizerThread = process->mainThread;
	}
	
	/* Initialize the random seed pool lock */
	process->randomLock = ILMutexCreate();
	if(!(process->randomLock))
	{
		ILExecProcessDestroy(process);
		return 0;
	}

	/* Return the process record to the caller */
	return process;
}

/*
 * Unloads a process and all threads associated with it.
 * The process may not unload immediately (or even ever).
 * If the current thread exists inside the process, a
 * new thread will be created to unload & destroy the
 * process.  When this function exits, the thread that
 * calls this method will still be able to execute managed
 * code even if it resides within the process it tried to
 * destroy.  The process will eventually be destroyed
 * when the thread (and all other process-owned threads)
 * exit.
 */
void ILExecProcessUnload(ILExecProcess *process)
{
	/* TODO: Implement same semantics as AppDomain.Unload
	   for embedders */
}

/*
 * Destroy a process.
 *
 * DO NOT call this function from a thread that expects to be
 * alive after the function returns.  This is *NOT* an implementation
 * of AppDomain.Unload.  A thread that calls this function but exists
 * inside the process will be *unusable* from managed code when this
 * function exits.
 *
 * In the implementation of AppDomain.Unload, this method should
 * not be called by a thread that runs inside the domain it is trying
 * to destroy otherwise the thread will return dead & to a dead domain.
 * A new domain-less thread should be created to destroy the domain.
 * On single threaded systems, this method can be called directly
 * by AppDomain.Unload unless the current thread is living in the
 * domain it is trying to unload in which case AppDomain.Unload *MUST*
 * just return without doing anything.
 *
 * Thong Nguyen (tum@veridicus.com)
 */
void ILExecProcessDestroy(ILExecProcess *process)
{
	int count;
	ILThread *target;
	int mainIsFinalizer;
	ILExecThread *thread;	
	ILQueueEntry *abortQueue1, *abortQueue2;
	
	abortQueue1 = ILQueueCreate();
	abortQueue2 = ILQueueCreate();

	/* Lock down the process */
	ILMutexLock(process->lock);

	process->state = _IL_PROCESS_STATE_UNLOADING;

	/* From this point on, no threads can be created inside or enter the AppDomain */

	/* Clear 4K of stack space in case it contains some stray pointers 
	   that may still be picked up by not so accurate collectors */
	ILThreadClearStack(4096);

	/* Determine if this is a single-threaded system where the main
	   thread is the finalizer thread */
	mainIsFinalizer = process->mainThread == process->finalizerThread;

	if (process->mainThread)
	{
		if (mainIsFinalizer)
		{
			/* If the main thread is the finalizer thread then
			   we have to zero the memory of the CVM stack so that
			   stray pointers are erased */
#ifdef IL_CONFIG_APPDOMAINS
			ILMemZero(process->mainThread->stackBase, process->engine->stackSize);
			ILMemZero(process->mainThread->frameStack, process->engine->frameStackSize);
#else
			ILMemZero(process->mainThread->stackBase, process->stackSize);
			ILMemZero(process->mainThread->frameStack, process->frameStackSize);
#endif
		}
		else
		{
			/* If the main thread isn't the finalizer then it's
			   possible to simply destroy the main thread before
			   calling the finalizers */
		}
	}

	count = 0;

	/* Walk all the threads, collecting CLR thread pointers since they are GC
	managed and stable */

	thread = process->firstThread;

	while (thread)
	{
		if (thread != process->finalizerThread
			/* Make sure its a managed thread */
			&& (thread->clrThread || thread == process->mainThread)
			&& thread->supportThread != ILThreadSelf())
		{
			ILQueueAdd(&abortQueue1, thread->supportThread);
			ILQueueAdd(&abortQueue2, thread->supportThread);

			thread = thread->nextThread;

			count++;
		}
		else
		{
			/* Move onto the next thread */
			thread = thread->nextThread;
		}
	}

	/* Unlock the process */
	ILMutexUnlock(process->lock);

	if (!abortQueue1 || !abortQueue2)
	{
		if (count != 0)
		{
			/* Probably ran out of memory trying to build the abortQueues */
			return;
		}
	}

	/* Abort all threads */
	while (abortQueue1)
	{
		target = (ILThread *)ILQueueRemove(&abortQueue1);

		_ILExecThreadAbortThread(ILExecThreadCurrent(), target);
	}

	/* Wait for all threads */
	while (abortQueue2)
	{
		target = (ILThread *)ILQueueRemove(&abortQueue2);

		ILThreadJoin(target, -1);
	}
	
#ifndef IL_USE_JIT
	/* Unregister (and destroy) the current thread if it isn't needed
	for finalization and if it belongs to this domain. */
	if (!mainIsFinalizer 
		&& ILExecThreadCurrent()
		&& ILExecThreadCurrent() != process->finalizerThread
		&& ILExecThreadCurrent()->process == process)
	{
		ILThreadUnregisterForManagedExecution(ILThreadSelf());
	}
#endif

	/* Invoke the finalizers -- hopefully finalizes all objects left in the
	   process being destroyed.  Objects left lingering are orphans */
	ILGCCollect();

	if (ILGCInvokeFinalizers(30000) != 0)
	{
		/* Finalizers are taking too long.  Abandon unloading of this process */
		return;
	}

	/* We must ensure that objects created and then orphaned by this process
	   won't finalize themselves from this point on (because the process will
	   no longer be valid).  Objects can be orphaned if the GC is conservative
	   (like the boehm GC) */

	/* Disable finalizers to ensure no finalizers are running until we 
	   reenable them */
	if (ILGCDisableFinalizers(10000) != 0)
	{
		/* Finalizers are taking too long.  Abandon unloading of this process */
		return;
	}

	/* Mark the process as dead in the finalization context.  This prevents
	   orphans from finalizing */
	process->finalizationContext->process = 0;

	/* Reenable finalizers */
	ILGCEnableFinalizers();

	/* Unregister (and destroy) the current thread if it 
	   wasn't destroyed above and if it belongs to this domain */
	if (mainIsFinalizer
		&& ILExecThreadCurrent()
		&& ILExecThreadCurrent() != process->finalizerThread
		&& ILExecThreadCurrent()->process == process)
	{
		ILThreadUnregisterForManagedExecution(ILThreadSelf());
	}

	/* Destroy the finalizer thread */
	if (process->finalizerThread)
	{
		/* Only destroy the engine thread.  The support thread is shared by other
		   engine processes and is destroyed when the engine is deinitialized */
		_ILExecThreadDestroy(process->finalizerThread);
	}

	/* Destroy the CVM coder instance */
	if (process->coder)
	{
		ILCoderDestroy(process->coder);
	}

	/* Destroy the metadata lock */
	if(process->metadataLock)
	{
		ILRWLockDestroy(process->metadataLock);
	}

	/* Destroy the image loading context */
	if(process->context)
	{
		ILContextDestroy(process->context);
	}

	if (process->internHash)
	{
		/* Destroy the main part of the intern'ed hash table.
		The rest will be cleaned up by the garbage collector */
		ILGCFreePersistent(process->internHash);
	}

	if (process->reflectionHash)
	{
		/* Destroy the main part of the reflection hash table.
		The rest will be cleaned up by the garbage collector */
		ILGCFreePersistent(process->reflectionHash);
	}

#ifdef IL_CONFIG_PINVOKE
	/* Destroy the loaded module list */
	{
		ILLoadedModule *loaded, *nextLoaded;
		loaded = process->loadedModules;
		while(loaded != 0)
		{
			if(loaded->handle != 0)
			{
				ILDynLibraryClose(loaded->handle);
			}
			nextLoaded = loaded->next;
			ILFree(loaded);
			loaded = nextLoaded;
		}
	}
#endif

#ifdef IL_CONFIG_RUNTIME_INFRA
	/* Destroy the GC handle table */
	if(process->gcHandles)
	{
		extern void _ILGCHandleTableFree(struct _tagILGCHandleTable *table);
		_ILGCHandleTableFree(process->gcHandles);
		process->gcHandles = 0;
	}
#endif

#ifdef IL_CONFIG_DEBUG_LINES
	/* Destroy the breakpoint watch list */
	{
		ILExecDebugWatch *watch, *nextWatch;
		watch = process->debugWatchList;
		while(watch != 0)
		{
			nextWatch = watch->next;
			ILFree(watch);
			watch = nextWatch;
		}
	}
#endif

	if (process->randomLock)
	{
		/* Destroy the random seed pool */
		ILMutexDestroy(process->randomLock);
	}

	if (process->randomPool)
	{
		ILMemZero(process->randomPool, sizeof(process->randomPool));
	}

	_ILExecMonitorProcessDestroy(process);

	if (process->lock)
	{
		/* Destroy the object lock */
		ILMutexDestroy(process->lock);
	}

#ifdef IL_CONFIG_APPDOMAINS

	if (process->engine)
	{
		ILExecProcessDetachFromEngine(process);
	}
#endif

	/* Free the process block itself */
	ILGCFreePersistent(process);

	/* Reset the console to the "normal" mode */
	ILConsoleSetMode(IL_CONSOLE_NORMAL);
}

void ILExecProcessSetLibraryDirs(ILExecProcess *process,
								 char **libraryDirs,
								 int numLibraryDirs)
{
	ILContextSetLibraryDirs(process->context, libraryDirs, numLibraryDirs);
}

ILContext *ILExecProcessGetContext(ILExecProcess *process)
{
	return process->context;
}

ILExecThread *ILExecProcessGetMain(ILExecProcess *process)
{
	return process->mainThread;
}

/*
 * Load standard classes and objects.
 */
static void LoadStandard(ILExecProcess *process, ILImage *image)
{
	ILClass *classInfo;

	if(!(process->outOfMemoryObject))
	{
		/* If this image caused "OutOfMemoryException" to be
		loaded, then create an object based upon it.  We must
		allocate this object ahead of time because we won't be
		able to when the system actually runs out of memory */
		classInfo = ILClassLookupGlobal(ILImageToContext(image),
			"OutOfMemoryException", "System");
		if(classInfo)
		{
			/* Set the system image, for standard type resolutions */
			ILContextSetSystem(ILImageToContext(image),
						   	   ILProgramItem_Image(classInfo));

			/* We don't call the "OutOfMemoryException" constructor,
			to avoid various circularity problems at this stage
			of the loading process */
			process->outOfMemoryObject =
				_ILEngineAllocObject(process->mainThread, classInfo);
		}
	}
	
	/* Look for "System.Object" */
	if(!(process->objectClass))
	{
		process->objectClass = ILClassLookupGlobal(ILImageToContext(image),
							        			   "Object", "System");
	}

	/* Look for "System.String" */
	if(!(process->stringClass))
	{
		process->stringClass = ILClassLookupGlobal(ILImageToContext(image),
							        			   "String", "System");
	}

	/* Look for "System.Exception" */
	if(!(process->exceptionClass))
	{
		process->exceptionClass = ILClassLookupGlobal(ILImageToContext(image),
							        			      "Exception", "System");
	}

	/* Look for "System.Reflection.ClrType" */
	if(!(process->clrTypeClass))
	{
		process->clrTypeClass = ILClassLookupGlobal(ILImageToContext(image),
								        "ClrType", "System.Reflection");
	}

	/* Look for "System.Threading.ThreadAbortException" */
	if(!(process->threadAbortClass))
	{
		process->threadAbortClass = ILClassLookupGlobal(ILImageToContext(image),
			"ThreadAbortException", "System.Threading");
	}
}

#ifndef REDUCED_STDIO

int ILExecProcessLoadImage(ILExecProcess *process, FILE *file)
{
	ILImage *image;
	int loadError;
	ILRWLockWriteLock(process->metadataLock);
	loadError = ILImageLoad(file, 0, process->context, &image,
					   	    process->loadFlags);
	ILRWLockUnlock(process->metadataLock);
	if(loadError == 0)
	{
		LoadStandard(process, image);
	}
	return loadError;
}

#endif

int ILExecProcessLoadFile(ILExecProcess *process, const char *filename)
{
	int error;
	ILImage *image;
	ILRWLockWriteLock(process->metadataLock);
	error = ILImageLoadFromFile(filename, process->context, &image,
								process->loadFlags, 0);
	ILRWLockUnlock(process->metadataLock);
	if(error == 0)
	{
		LoadStandard(process, image);
	}
	return error;
}

void ILExecProcessSetLoadFlags(ILExecProcess *process, int mask, int flags)
{
	process->loadFlags &= ~mask;
	process->loadFlags |= flags;
}

int ILExecProcessGetStatus(ILExecProcess *process)
{
	return process->exitStatus;
}

ILMethod *ILExecProcessGetEntry(ILExecProcess *process)
{
	ILImage *image = 0;
	ILToken token;
	ILMethod *method = 0;
	ILRWLockReadLock(process->metadataLock);
	while((image = ILContextNextImage(process->context, image)) != 0)
	{
		if(ILImageType(image) != IL_IMAGETYPE_EXE)
		{
			continue;
		}
		token = ILImageGetEntryPoint(image);
		if(token && (token & IL_META_TOKEN_MASK) == IL_META_TOKEN_METHOD_DEF)
		{
			process->entryImage = image;
			method = ILMethod_FromToken(image, token);
			break;
		}
	}
	ILRWLockUnlock(process->metadataLock);
	return method;
}

int ILExecProcessEntryType(ILMethod *method)
{
	ILType *signature;
	ILType *paramType;
	int entryType;

	/* The method must be static */
	if(!ILMethod_IsStatic(method))
	{
		return IL_ENTRY_INVALID;
	}

	/* The method must have either "void" or "int" as the return type */
	signature = ILMethod_Signature(method);
	if(ILType_HasThis(signature))
	{
		return IL_ENTRY_INVALID;
	}
	paramType = ILTypeGetReturn(signature);
	if(paramType == ILType_Void)
	{
		entryType = IL_ENTRY_NOARGS_VOID;
	}
	else if(paramType == ILType_Int32)
	{
		entryType = IL_ENTRY_NOARGS_INT;
	}
	else
	{
		return IL_ENTRY_INVALID;
	}

	/* The method must either have no args or a single "String[]" arg */
	if(ILTypeNumParams(signature) != 0)
	{
		if(ILTypeNumParams(signature) != 1)
		{
			return IL_ENTRY_INVALID;
		}
		paramType = ILTypeGetParam(signature, 1);
		if(!ILType_IsSimpleArray(paramType))
		{
			return IL_ENTRY_INVALID;
		}
		if(!ILTypeIsStringClass(ILTypeGetElemType(paramType)))
		{
			return IL_ENTRY_INVALID;
		}
		entryType += (IL_ENTRY_ARGS_VOID - IL_ENTRY_NOARGS_VOID);
	}

	/* Return the entry point type to the caller */
	return entryType;
}

long ILExecProcessGetParam(ILExecProcess *process, int type)
{
	switch(type)
	{
		case IL_EXEC_PARAM_GC_SIZE:
		{
			return ILGCGetHeapSize();
		}
		/* Not reached */

		case IL_EXEC_PARAM_MC_SIZE:
		{
			return (long)(ILCoderGetCacheSize(process->coder));
		}
		/* Not reached */

		case IL_EXEC_PARAM_MALLOC_MAX:
		{
			extern long _ILMallocMaxUsage(void);
			return _ILMallocMaxUsage();
		}
		/* Not reached */
	}
	return -1;
}

ILObject *ILExecProcessSetCommandLine(ILExecProcess *process,
									  const char *progName, char *args[])
{
	ILExecThread *thread;
	ILObject *mainArgs;
	ILObject *allArgs;
	ILString *argString;
	int opt;
	int argc;
	char *expanded;

	/* Cound the number of arguments in the "args" array */
	argc = 0;
	while(args != 0 && args[argc] != 0)
	{
		++argc;
	}

	/* Create two arrays: one for "Main" and the other for
	   "TaskMethods.GetCommandLineArgs".  The former does
	   not include "argv[0]", but the latter does */
	thread = ILExecProcessGetMain(process);
	mainArgs = ILExecThreadNew(thread, "[oSystem.String;",
						       "(Ti)V", (ILVaInt)argc);
	if(!mainArgs || ILExecThreadHasException(thread))
	{
		return 0;
	}
	allArgs = ILExecThreadNew(thread, "[oSystem.String;",
						      "(Ti)V", (ILVaInt)(argc + 1));
	if(!allArgs || ILExecThreadHasException(thread))
	{
		return 0;
	}

	/* Populate the argument arrays */
	expanded = ILExpandFilename(progName, (char *)0);
	if(!expanded)
	{
		ILExecThreadThrowOutOfMemory(thread);
		return 0;
	}
	argString = ILStringCreate(thread, expanded);
	ILFree(expanded);
	if(!argString || ILExecThreadHasException(thread))
	{
		return 0;
	}
	ILExecThreadSetElem(thread, allArgs, (ILInt32)0, argString);
	for(opt = 0; opt < argc; ++opt)
	{
		argString = ILStringCreate(thread, args[opt]);
		if(!argString || ILExecThreadHasException(thread))
		{
			return 0;
		}
		ILExecThreadSetElem(thread, mainArgs, (ILInt32)opt, argString);
		ILExecThreadSetElem(thread, allArgs, (ILInt32)(opt + 1), argString);
	}

	/* Set the value for "TaskMethods.GetCommandLineArgs" */
	process->commandLineObject = allArgs;

	/* Return the "Main" arguments to the caller */
	return mainArgs;
}

#ifndef IL_CONFIG_APPDOMAINS
int ILExecProcessAddInternalCallTable(ILExecProcess* process, 
					const ILEngineInternalClassInfo* internalClassTable,
					int tableSize)
{
	ILEngineInternalClassList* tmp;
	if((!internalClassTable) || (tableSize<=0))return 0;

	if(!(process->internalClassTable))
	{
		process->internalClassTable=(ILEngineInternalClassList*)ILMalloc(
									sizeof(ILEngineInternalClassList));
		process->internalClassTable->size=tableSize;
		process->internalClassTable->list=internalClassTable;
		process->internalClassTable->next=NULL;
		return 1;
	}
	for(tmp=process->internalClassTable;tmp->next!=NULL;tmp=tmp->next);
	tmp->next=(ILEngineInternalClassList*)ILMalloc(
								sizeof(ILEngineInternalClassList));
	tmp=tmp->next; /* advance */
	tmp->size=tableSize;
	tmp->list=internalClassTable;
	tmp->next=NULL;
	return 1;
}
#endif

void ILExecProcessSetCoderFlags(ILExecProcess *process,int flags)
{
	ILCoderSetFlags(process->coder,flags);
}

#ifdef	__cplusplus
};
#endif
