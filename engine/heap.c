/*
 * heap.c - Heap routines for the runtime engine.
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
#include "lib_defs.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Initialize a class.
 */
static int InitializeClass(ILExecThread *thread, ILClass *classInfo)
{
	/* Quick check to see if the class is already laid out,
	   to avoid acquiring the metadata lock if possible */
	if(_ILLayoutAlreadyDone(classInfo))
	{
		return 1;
	}

	/* Acquire the metadata write lock and disable finalizers */
	IL_METADATA_WRLOCK(thread);
	ILGCDisableFinalizers();

	/* Lay out the class's fields.  This will check for layout
	   again, to avoid race condition situations */
	if(!_ILLayoutClass(classInfo))
	{
		/* Throw a "TypeInitializationException" */
		ILGCEnableFinalizers();
		IL_METADATA_UNLOCK(thread);
		ILGCInvokeFinalizers();
		thread->thrownException = _ILSystemException
			(thread, "System.TypeInitializationException");
		return 0;
	}

	/* Re-enable finalizers and unlock the metadata write lock */
	ILGCEnableFinalizers();
	IL_METADATA_UNLOCK(thread);
	ILGCInvokeFinalizers();
	return 1;
}

/* This method assumes there is only one finalizer thread for the entire os process */
void _ILFinalizeObject(void *block, void *data)
{
	ILObject *object;
	ILClass *classInfo;
	ILMethod *method;
	ILType *signature;
	ILThread *thread;
	ILExecProcess *process;
	ILExecThread *execThread;
	ILFinalizationContext *finalizationContext;
	
	/* Get the finalization context */
	finalizationContext = (ILFinalizationContext *)data;

	/* Skip the object header within the block */
	object = GetObjectFromGcBase(block);

	/* Get the process that created the object */
	process = finalizationContext->process;

	/* Make sure the process is still alive */
	if (process == 0)
	{
		/* Our owner process died.  We're orphaned and can't finalize */
		return;
	}

	/* Get the engine thread to execute the finalizer on */
	execThread = process->finalizerThread;

	/* Get the finalizer thread instance */
	thread = ILThreadSelf();

	if (execThread == 0)
	{				
		/* Create a new engine thread for the finalizers of this process to run on */

		execThread = _ILExecThreadCreate(process);

		if (execThread == 0)
		{
			return;
		}

		execThread->supportThread = thread;
		process->finalizerThread = execThread;
	}

	/* Make the finalizer thread execute in the context of the object's process's
	    finalizer thread */
	_ILThreadExecuteOn(thread, execThread);

	/* Get the object's class and locate the "Finalize" method */
	classInfo = GetObjectClass(object);

	while(classInfo != 0)
	{
		method = 0;
		while((method = (ILMethod *)ILClassNextMemberByKind
			(classInfo, (ILMember *)method, IL_META_MEMBERKIND_METHOD)) != 0)
		{
			if(!strcmp(ILMethod_Name(method), "Finalize"))
			{
				signature = ILMethod_Signature(method);
				if(ILTypeGetReturn(signature) == ILType_Void &&
				   ILTypeNumParams(signature) == 0)
				{
					ILGCRegisterFinalizer(block, 0, 0);
		
					/* Invoke the "Finalize" method on the object */
					if(ILExecThreadCall(ILExecThreadCurrent(),
										method, (void *)0, object))
					{
						ILExecThreadClearException(ILExecThreadCurrent());
					}
				}
			}
		}
		classInfo = ILClassGetParent(classInfo);
	}

	execThread->supportThread = 0;

	/* Make sure the finalizer thread can no longer execute managed code */
	_ILThreadExecuteOn(thread, 0);	
}

ILObject *_ILEngineAlloc(ILExecThread *thread, ILClass *classInfo,
						 ILUInt32 size)
{
	void *ptr;
	ILObject *obj;

	/* Make sure the class has been initialized before we start */
	if(classInfo != 0 && !InitializeClass(thread, classInfo))
	{
		return 0;
	}

	/* Allocate memory from the heap */
	ptr = ILGCAlloc(size + IL_OBJECT_HEADER_SIZE);
	if(!ptr)
	{
		/* Throw an "OutOfMemoryException" */
		thread->thrownException = thread->process->outOfMemoryObject;
		return 0;
	}

	obj = GetObjectFromGcBase(ptr);

	/* Set the class into the block */
	if(classInfo)
	{
		SetObjectClassPrivate(obj, (ILClassPrivate *)(classInfo->userData));
	}
	else
	{
		SetObjectClassPrivate(obj, 0);
	}

	/* Attach a finalizer to the object if the class has
	   a non-trival finalizer method attached to it */
	if(classInfo != 0 &&
	   ((ILClassPrivate *)(classInfo->userData))->hasFinalizer)
	{
		ILGCRegisterFinalizer(ptr, _ILFinalizeObject, thread->process->finalizationContext);
	}

	/* Return a pointer to the object */
	return obj;
}

ILObject *_ILEngineAllocAtomic(ILExecThread *thread, ILClass *classInfo,
							   ILUInt32 size)
{
	void *ptr;
	ILObject *obj;

	/* Make sure the class has been initialized before we start */
	if(classInfo != 0 && !InitializeClass(thread, classInfo))
	{
		return 0;
	}

	/* Allocate memory from the heap */
	ptr = ILGCAllocAtomic(size + IL_OBJECT_HEADER_SIZE);	
	if(!ptr)
	{
		/* Throw an "OutOfMemoryException" */
		thread->thrownException = thread->process->outOfMemoryObject;
		return 0;
	}
	
	obj = GetObjectFromGcBase(ptr);

	/* Set the class into the block */
	if(classInfo)
	{
		SetObjectClassPrivate(obj, (ILClassPrivate *)(classInfo->userData));
	}
	else
	{
		SetObjectClassPrivate(obj, 0);
	}

	/* Attach a finalizer to the object if the class has
	   a non-trival finalizer method attached to it */
	if(classInfo != 0 &&
	   ((ILClassPrivate *)(classInfo->userData))->hasFinalizer)
	{
		ILGCRegisterFinalizer(ptr, _ILFinalizeObject, thread->process->finalizationContext);
	}

	/* Return a pointer to the object */
	return obj;
}

ILObject *_ILEngineAllocObject(ILExecThread *thread, ILClass *classInfo)
{
	if(!InitializeClass(thread, classInfo))
	{
		return 0;
	}
	if(((ILClassPrivate *)(classInfo->userData))->managedInstance)
	{
		return _ILEngineAlloc
				(thread, classInfo,
				 ((ILClassPrivate *)(classInfo->userData))->size);
	}
	else
	{
		/* There are no managed fields, so use atomic allocation */
		return _ILEngineAllocAtomic
				(thread, classInfo,
				 ((ILClassPrivate *)(classInfo->userData))->size);
	}
}

#ifdef	__cplusplus
};
#endif
