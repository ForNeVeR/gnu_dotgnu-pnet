/*
 * process.c - Manage processes within the runtime engine.
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

#include "engine.h"

#ifdef	__cplusplus
extern	"C" {
#endif

void ILExecInit(unsigned long maxSize)
{
	/* Initialize the global garbage collector */
	ILGCInit(maxSize);
}

ILExecProcess *ILExecProcessCreate(unsigned long stackSize)
{
	ILExecProcess *process;

	/* Create the process record */
	if((process = (ILExecProcess *)ILGCAllocPersistent
						(sizeof(ILExecProcess))) == 0)
	{
		return 0;
	}

	/* Initialize the fields */
	process->firstThread = 0;
	process->mainThread = 0;
	process->stackSize = ((stackSize < IL_ENGINE_STACK_SIZE)
							? IL_ENGINE_STACK_SIZE : stackSize);
	process->frameStackSize = IL_ENGINE_FRAME_STACK_SIZE;
	process->context = 0;
	process->exitStatus = 0;
	process->coder = 0;
	process->objectClass = 0;
	process->stringClass = 0;
	process->exceptionClass = 0;
	process->clrTypeClass = 0;
	process->outOfMemoryObject = 0;
	ILGetCurrTime(&(process->startTime));
	process->internHash = 0;
	process->reflectionHash = 0;
	process->loadedModules = 0;
	process->entryImage = 0;

	/* Initialize the image loading context */
	if((process->context = ILContextCreate()) == 0)
	{
		ILExecProcessDestroy(process);
		return 0;
	}

	/* Create the "main" thread */
	process->mainThread = ILExecThreadCreate(process);
	if(!(process->mainThread))
	{
		ILExecProcessDestroy(process);
		return 0;
	}

	/* Initialize the CVM coder */
	process->coder = ILCoderCreate(&_ILCVMCoderClass, 100000);
	if(!(process->coder))
	{
		ILExecProcessDestroy(process);
		return 0;
	}

	/* Return the process record to the caller */
	return process;
}

void ILExecProcessDestroy(ILExecProcess *process)
{
	ILLoadedModule *loaded, *nextLoaded;

	/* Destroy the threads associated with the process */
	while(process->firstThread != 0)
	{
		ILExecThreadDestroy(process->firstThread);
	}

	/* Destroy the CVM coder instance */
	ILCoderDestroy(process->coder);

	/* Destroy the image loading context */
	if(process->context)
	{
		ILContextDestroy(process->context);
	}

	/* Destroy the main part of the intern'ed hash table.
	   The rest will be cleaned up by the garbage collector */
	ILGCFreePersistent(process->internHash);

	/* Destroy the main part of the reflection hash table.
	   The rest will be cleaned up by the garbage collector */
	ILGCFreePersistent(process->reflectionHash);

	/* Destroy the loaded module list */
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
			/* We call the "OutOfMemoryException(int dummy)" constructor
			   in "pnetlib", which sets up the out of memory exception
			   object with the necessary localised string information,
			   but with no stack trace associated with it */
#if 0
			process->outOfMemoryObject =
				ILExecThreadNew(process->mainThread,
								"System.OutOfMemoryException",
								"(Ti)V", (ILVaInt)0);
#endif
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
}

int ILExecProcessLoadImage(ILExecProcess *process, FILE *file)
{
	ILImage *image;
	int loadError;
	loadError = ILImageLoad(file, 0, process->context, &image,
					   	    IL_LOADFLAG_FORCE_32BIT);
	if(loadError == 0)
	{
		LoadStandard(process, image);
	}
	return loadError;
}

int ILExecProcessLoadFile(ILExecProcess *process, const char *filename)
{
	int error;
	ILImage *image;
	error = ILImageLoadFromFile(filename, process->context, &image,
								IL_LOADFLAG_FORCE_32BIT, 0);
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
	while((image = ILContextNextImage(process->context, image)) != 0)
	{
		token = ILImageGetEntryPoint(image);
		if(token && (token & IL_META_TOKEN_MASK) == IL_META_TOKEN_METHOD_DEF)
		{
			process->entryImage = image;
			return ILMethod_FromToken(image, token);
		}
	}
	return 0;
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
	}
	return -1;
}

#ifdef	__cplusplus
};
#endif
