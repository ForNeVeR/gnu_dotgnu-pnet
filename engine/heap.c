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

	/* Acquire the metadata write lock */
	IL_METADATA_WRLOCK(thread);

	/* Lay out the class's fields.  This will check for layout
	   again, to avoid race condition situations */
	if(!_ILLayoutClass(classInfo))
	{
		/* TODO: Throw a "TypeInitializationException" */
		IL_METADATA_UNLOCK(thread);
		return 0;
	}

	/* The class has been initialized */
	IL_METADATA_UNLOCK(thread);
	return 1;
}

/*
 * Finalization callback that is invoked by the garbage collector.
 */
static void FinalizeObject(void *block, void *data)
{
	/* TODO */
}

ILObject *_ILEngineAlloc(ILExecThread *thread, ILClass *classInfo,
						 ILUInt32 size)
{
	void *ptr;

	/* Make sure the class has been initialized before we start */
	if(classInfo != 0 && !InitializeClass(thread, classInfo))
	{
		return 0;
	}

	/* Allocate memory from the heap */
	ptr = ILGCAlloc(size + IL_BEST_ALIGNMENT);
	if(!ptr)
	{
		/* Throw an "OutOfMemoryException" */
		thread->thrownException = thread->process->outOfMemoryObject;
		return 0;
	}

	/* Set the class into the block */
	*((ILClass **)ptr) = classInfo;

	/* Attach a finalizer to the object if the class has
	   a non-trival finalizer method attached to it */
	if(classInfo != 0 &&
	   ((ILClassPrivate *)(classInfo->userData))->hasFinalizer)
	{
		ILGCRegisterFinalizer(ptr, FinalizeObject, 0);
	}

	/* Return a pointer to the data just after the class information */
	return (void *)(((unsigned char *)ptr) + IL_BEST_ALIGNMENT);
}

ILObject *_ILEngineAllocAtomic(ILExecThread *thread, ILClass *classInfo,
							   ILUInt32 size)
{
	void *ptr;

	/* Make sure the class has been initialized before we start */
	if(classInfo != 0 && !InitializeClass(thread, classInfo))
	{
		return 0;
	}

	/* Allocate memory from the heap */
	ptr = ILGCAllocAtomic(size + IL_BEST_ALIGNMENT);
	if(!ptr)
	{
		/* Throw an "OutOfMemoryException" */
		thread->thrownException = thread->process->outOfMemoryObject;
		return 0;
	}

	/* Set the class into the block */
	*((ILClass **)ptr) = classInfo;

	/* Return a pointer to the data just after the class information */
	return (void *)(((unsigned char *)ptr) + IL_BEST_ALIGNMENT);
}

ILObject *_ILEngineAllocObject(ILExecThread *thread, ILClass *classInfo)
{
	if(!InitializeClass(thread, classInfo))
	{
		return 0;
	}
	return _ILEngineAlloc(thread, classInfo,
						  ((ILClassPrivate *)(classInfo->userData))->size);
}

#ifdef	__cplusplus
};
#endif
