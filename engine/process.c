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

#ifdef	__cplusplus
extern	"C" {
#endif

void ILExecInit(unsigned long maxSize)
{
	/* Initialize the thread routines */	
	ILThreadInit();

	/* Initialize the global garbage collector */	
	ILGCInit(maxSize);	
}

void ILExecDeinit()
{
	/* Deinitialize the global garbage collector */	
	ILGCDeinit();	

	/* Deinitialize the thread routines */	
	ILThreadDeinit();	
}

ILExecProcess *ILExecProcessCreate(unsigned long stackSize, unsigned long cachePageSize)
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
	process->firstThread = 0;
	process->mainThread = 0;
	process->finalizerThread = 0;
#ifdef USE_HASHING_MONITORS
	process->monitorHash = 0;
#endif
	process->stackSize = ((stackSize < IL_CONFIG_STACK_SIZE)
							? IL_CONFIG_STACK_SIZE : stackSize);
	process->frameStackSize = IL_CONFIG_FRAME_STACK_SIZE;
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
#ifdef IL_USE_IMTS
	process->imtBase = 1;
#endif

	/* Initialize the image loading context */
	if((process->context = ILContextCreate()) == 0)
	{
		ILExecProcessDestroy(process);
		return 0;
	}

	/* Associate the process with the context */
	ILContextSetUserData(process->context, process);

	/* Initialize the CVM coder */
	process->coder = ILCoderCreate(&_ILCVMCoderClass, 100000, cachePageSize);
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

#ifdef USE_HASHING_MONITORS
	/* Initialize the monitor system lock */
	process->monitorSystemLock = ILMutexCreate();
	if(!(process->monitorSystemLock))
	{
		ILExecProcessDestroy(process);
		return 0;
	}
#endif

	/* Initialize the finalization context */
	process->finalizationContext = (ILFinalizationContext *)ILGCAlloc(sizeof(ILFinalizationContext));
	if (!process->finalizationContext)
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

typedef struct _tagThreadListEntry ThreadListEntry;
struct _tagThreadListEntry
{
	ILThread *thread;
	ThreadListEntry *next;
};

void ILExecProcessDestroy(ILExecProcess *process)
{
	int result;
	int mainIsFinalizer = 0;
	ILThread *mainSupportThread;
	ILExecThread *thread, *nextThread;	
	ThreadListEntry *firstEntry, *entry, *next;

	if (process->mainThread)
	{
		/* TODO: Stack still contains last frame */
		ILMemZero(process->mainThread->stackBase, sizeof(int));
	}

	/* Delete all managed threads so the objects they used can be finalizerd */
	/* Keeps deleting managed threads until they're all gone just in case
	    threads spawn new threads when they exit */
	for (;;)
	{				
		firstEntry = 0;

		/* Lock down the process */
		ILMutexLock(process->lock);

		/* Walk all the threads */
		thread = process->firstThread;

		while (thread)
		{
			if (thread != process->finalizerThread && 
				/* Make sure its a managed thread */
				(thread->clrThread || thread == process->mainThread)
				&& thread->supportThread != ILThreadSelf())
			{
				/*
				 * Instead of aborting and then waiting on each thread, we abort all
				 * threads and then wait for them all at the bottom.  This prevents us from
				 *	deadlocking on a thread that's waiting on another (not yet aborted) thread.
				 */

				nextThread = thread->nextThread;

				entry = ILMalloc(sizeof(ThreadListEntry));

				if (!entry)
				{
					ILExecThreadThrowOutOfMemory(ILExecThreadCurrent());

					return;
				}

				entry->thread = thread->supportThread;
								
				if (!firstEntry)
				{
					entry->next = 0;
					firstEntry = entry;
				}
				else
				{
					entry->next = firstEntry;
					firstEntry = entry;
				}

				/* Abort the thread */			
				ILThreadAbort(thread->supportThread);

				thread = nextThread;

				/* Note: The CLR thread (and thus the support thread) might still be used
					by another thread so the support thread is left to be destroyed by the 
					CLR thread's finalizer */
			}
			else
			{
				/* Move onto the next thread */
				thread = thread->nextThread;
			}
		}

		/* Unlock the process */
		ILMutexUnlock(process->lock);

		entry = firstEntry;

		/*
		 *	Wait for the threads that have been aborted.
		 */

		if (entry)
		{
			while (entry)
			{
				next = entry->next;

				result = ILThreadJoin(entry->thread, -1);

				ILFree(entry);

				if (result != 0)
				{
					/* The thread might be aborted while trying to exit */

					entry = next;

					while (entry)
					{
						next = entry->next;
						ILFree(entry);
						entry = next;
					}

					_ILHandleWaitResult(ILExecThreadCurrent(), result);

					return;
				}

				entry = next;
			}
		}
		else
		{
			break;
		}
	}
	
	mainIsFinalizer = process->mainThread == process->finalizerThread;
	mainSupportThread = process->mainThread->supportThread;

	/* Invoke the finalizers -- hopefully finalizes all objects left in the process being destroyed */
	/* Any objects left lingering (because of a stray or fake pointer) are orphans */
	ILGCCollect();
	ILGCInvokeFinalizers();

	/* We must ensure that objects created and then orphaned by this process won't 
	    finalize themselves from this point on (because the process will no longer be valid).
		Objects can be orphaned if we're using a conservative GC like the Boehm GC */

	/* Disable finalizers to ensure no finalizers are running until we reenable them */
	ILGCDisableFinalizers();

	/* Mark the process as dead in the finalization context.  This prevents orphans from
	    finalizing */
	process->finalizationContext->process = 0;

	/* Reenable finalizers */
	ILGCEnableFinalizers();	

	/* Destroy the main thread if we aren't the main thread or the finalizer thread*/
	if (!mainIsFinalizer && ILThreadSelf() != mainSupportThread)
	{
		/* Abort the thread */
		ILThreadAbort(mainSupportThread);

		/* Wait for the thread to be aborted */
		ILThreadJoin(mainSupportThread, -1);

		/* Destroy the thread object */
		ILThreadDestroy(mainSupportThread);
	}
	
	/* Destroy the finalizer thread */
	if (process->finalizerThread)
	{
		/* Only destroy the engine thread.  The support thread is shared by other
		    engine processes and is destroyed when the engine is deinitialized */
		_ILExecThreadDestroy(process->finalizerThread);
	}

	if (process->coder)
	{
		/* Destroy the CVM coder instance */
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

#ifdef USE_HASHING_MONITORS
	/* Destroy the monitor system lock */
	ILMutexDestroy(process->monitorSystemLock);
#endif

	if (process->lock)
	{
		/* Destroy the object lock */
		ILMutexDestroy(process->lock);
	}

	/* Free the process block itself */
	ILGCFreePersistent(process);
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
					   	    IL_LOADFLAG_FORCE_32BIT);
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
								IL_LOADFLAG_FORCE_32BIT, 0);
	ILRWLockUnlock(process->metadataLock);
	if(error == 0)
	{
		LoadStandard(process, image);
	}
	return error;
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

void ILExecProcessSetCoderFlags(ILExecProcess *process,int flags)
{
	ILCoderSetFlags(process->coder,flags);
}

#ifdef	__cplusplus
};
#endif
