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

ILExecProcess *ILExecProcessCreate(void)
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
	process->stackSize = IL_ENGINE_STACK_SIZE;
	process->frameStackSize = IL_ENGINE_FRAME_STACK_SIZE;
	process->context = 0;
	process->exitStatus = 0;
	process->coder = 0;
	process->coderGeneration = 0;
	process->stringClass = 0;
	process->runtimeTypeClass = 0;
	process->outOfMemoryObject = 0;
	ILGetCurrTime(&(process->startTime));

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

	/* Free the process block itself */
	ILGCFreePersistent(process);
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
			process->outOfMemoryObject =
				_ILEngineAllocObject(process->mainThread, classInfo);
		}
	}

	/* Look for "System.String" */
	if(!(process->stringClass))
	{
		process->stringClass = ILClassLookupGlobal(ILImageToContext(image),
							        			   "String", "System");
	}

	/* Look for "System.RuntimeType" */
	if(!(process->runtimeTypeClass))
	{
		process->runtimeTypeClass = ILClassLookupGlobal(ILImageToContext(image),
								        "RuntimeType", "System");
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
	FILE *file;
	int error;
	ILImage *image;
	if((file = fopen(filename, "rb")) == NULL)
	{
		/* Try again using "r" in case libc doesn't support "rb" */
		if((file = fopen(filename, "r")) == NULL)
		{
			return -1;
		}
	}
	error = ILImageLoad(file, filename, process->context, &image,
						IL_LOADFLAG_FORCE_32BIT);
	fclose(file);
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
			return ILMethod_FromToken(image, token);
		}
	}
	return 0;
}

#ifdef	__cplusplus
};
#endif
