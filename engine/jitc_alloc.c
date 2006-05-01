/*
 * jitc_alloc.c - Jit coder memory management routines.
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

/*
 * Allocate memory for an object that contains object references.
 */
static ILObject *_ILJitAlloc(ILClass *classInfo, ILUInt32 size)
{
	ILClassPrivate *classPrivate = (ILClassPrivate *)(classInfo->userData);
	void *ptr;
	ILObject *obj;
	
	/* Allocate memory from the heap */
	ptr = ILGCAlloc(size + IL_OBJECT_HEADER_SIZE);

	if(!ptr)
	{
		/* Throw an "OutOfMemoryException" */
		ILRuntimeExceptionThrowOutOfMemory();
	}

	obj = GetObjectFromGcBase(ptr);

	/* Set the class into the block */
	SetObjectClassPrivate(obj, classPrivate);

	/* Attach a finalizer to the object if the class has
	a non-trival finalizer method attached to it */
	if(classPrivate->hasFinalizer)
	{
		ILGCRegisterFinalizer(ptr, _ILFinalizeObject,
							  classPrivate->process->finalizationContext);
	}

	/* Return a pointer to the object */
	return obj;
}

/*
 * Allocate memory for an object that does not contain any object references.
 */
static ILObject *_ILJitAllocAtomic(ILClass *classInfo, ILUInt32 size)
{
	ILClassPrivate *classPrivate = (ILClassPrivate *)(classInfo->userData);
	void *ptr;
	ILObject *obj;

	/* Allocate memory from the heap */
	ptr = ILGCAllocAtomic(size + IL_OBJECT_HEADER_SIZE);

	if(!ptr)
	{
		/* Throw an "OutOfMemoryException" */
		ILRuntimeExceptionThrowOutOfMemory();
	}

	obj = GetObjectFromGcBase(ptr);

	/* Set the class into the block */
	SetObjectClassPrivate(obj, classPrivate);

	/* Attach a finalizer to the object if the class has
	a non-trival finalizer method attached to it */
	if(classPrivate->hasFinalizer)
	{
		ILGCRegisterFinalizer(ptr, _ILFinalizeObject,
							  classPrivate->process->finalizationContext);
	}

	/* Return a pointer to the object */
	return obj;
}

/*
 * Generate the code to allocate the memory for an object with the given size.
 * Returns the ILJitValue with the pointer to the new object.
 */
static ILJitValue _ILJitAllocGen(ILJITCoder *jitCoder, ILClass *classInfo,
								 ILUInt32 size)
{
	ILJitValue newObj;
	ILJitValue args[2];

	/* Make sure the class has been layouted. */
	if(!(classInfo->userData) || (((ILClassPrivate *)(classInfo->userData))->inLayout))
	{
		if(!_LayoutClass(ILExecThreadCurrent(), classInfo))
		{
			return (ILJitValue)0;
		}
	}
	/* We call the alloc functions. */
	/* They thow an out of memory exception so we don't need to care. */
	args[0] = jit_value_create_nint_constant(jitCoder->jitFunction,
											 _IL_JIT_TYPE_VPTR,
											 (jit_nint)classInfo);
	args[1] = jit_value_create_nint_constant(jitCoder->jitFunction,
											 _IL_JIT_TYPE_UINT32, size);

	if(((ILClassPrivate *)(classInfo->userData))->managedInstance)
	{
		newObj = jit_insn_call_native(jitCoder->jitFunction, "_ILJitAlloc",
									  _ILJitAlloc,
									  _ILJitSignature_ILJitAlloc,
				 					  args, 2, 0);
	}
	else
	{
		newObj = jit_insn_call_native(jitCoder->jitFunction,
									  "_ILJitAllocAtomic",
									  _ILJitAllocAtomic,
									  _ILJitSignature_ILJitAlloc,
				 					  args, 2, 0);
	}
	return newObj;
}

/*
 * Generate the code to allocate the memory for an object.
 * Returns the ILJitValue with the pointer to the new object.
 */
static ILJitValue _ILJitAllocObjectGen(ILJITCoder *jitCoder, ILClass *classInfo)
{
	/* Make sure the class has been layouted. */
	if(!(classInfo->userData) || 
	   (((ILClassPrivate *)(classInfo->userData))->inLayout))
	{
		if(!_LayoutClass(ILExecThreadCurrent(), classInfo))
		{
			return (ILJitValue)0;
		}
	}
	return _ILJitAllocGen(jitCoder, classInfo,
						  ((ILClassPrivate *)(classInfo->userData))->size);
}

