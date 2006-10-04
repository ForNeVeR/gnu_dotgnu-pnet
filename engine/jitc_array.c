/*
 * jitc_array.c - Jit coder array handling routines.
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
 * Prototype for inlining functioncalls.
 *
 * ILJitValue func(ILJITCoder *, ILMethod *, ILCoderMethodInfo *, ILJitValue *, ILInt32)
 */

/*
 * Create a new simple array (1 dimension and zero based) 
 */
static ILJitValue _ILJitSArrayNew(ILJITCoder *jitCoder, ILClass *arrayClass, ILJitValue length)
{
	ILExecThread *thread = ILExecThreadCurrent();
	/* Get the array element type. */
	ILType *elementType = ILType_ElemType(ILClassGetSynType(arrayClass));
	/* Get the size of one array element. */
	ILUInt32 elementSize = ILSizeOfType(thread, elementType);
	ILJitValue headerSize = jit_value_create_nint_constant(jitCoder->jitFunction,
														   _IL_JIT_TYPE_UINT32,
														   sizeof(System_Array));
	ILJitValue arraySize;
	ILJitValue newArray;
	ILClass *elementClass;
	ILClassPrivate *classPrivate;
	ILJitValue args[2];

	if(jit_value_is_constant(length))
	{
		ILUInt64 totalSize = ((ILUInt64)elementSize) * ((ILUInt64)jit_value_get_nint_constant(length));
	
		if(totalSize > (ILUInt64)IL_MAX_INT32)
		{
			/* The array is too large. */
			ILJitValue array = jit_value_create_nint_constant(jitCoder->jitFunction,
															  _IL_JIT_TYPE_VPTR, 0);

			_ILJitThrowSystem(jitCoder, _IL_JIT_OUT_OF_MEMORY);
			return array;
		}
		arraySize = jit_value_create_nint_constant(jitCoder->jitFunction,
												   _IL_JIT_TYPE_UINT32,
												   (ILUInt32)totalSize);
	}
	else
	{
		ILJitValue temp;
		jit_label_t label = jit_label_undefined;
		ILJitValue maxSize = jit_value_create_long_constant(jitCoder->jitFunction,
															_IL_JIT_TYPE_UINT64,
															IL_MAX_INT32);
	
		arraySize = jit_value_create_long_constant(jitCoder->jitFunction,
												   _IL_JIT_TYPE_UINT64,
												   elementSize);
		arraySize = jit_insn_mul(jitCoder->jitFunction, arraySize,
								 _ILJitValueConvertImplicit(jitCoder->jitFunction,
															length,
															_IL_JIT_TYPE_UINT64));

		/* Check if the total arraysize is > IL_MAX_INT32. */
		temp = jit_insn_gt(jitCoder->jitFunction, arraySize, maxSize);
		jit_insn_branch_if_not(jitCoder->jitFunction, temp, &label);
		/* And throw an OutOfMemoryException then. */
		_ILJitThrowSystem(jitCoder, _IL_JIT_OUT_OF_MEMORY);
		jit_insn_label(jitCoder->jitFunction, &label);

		/* Downconvert the array size to an UINT32. */
		arraySize = _ILJitValueConvertImplicit(jitCoder->jitFunction, arraySize, _IL_JIT_TYPE_UINT32);
	}
	/* Make sure the synthetic array class is layouted. */
	if(!(arrayClass->userData))
	{
		/* We have to layout the class first. */
		if(!_LayoutClass(thread, arrayClass))
		{
			return (ILJitValue)0;
		}
	}

	/* Make sure that the length has the correct type. */
	length = _ILJitValueConvertImplicit(jitCoder->jitFunction, length, _IL_JIT_TYPE_UINT32);

	elementClass = ILClassFromType(ILProgramItem_Image(arrayClass),
								   0, elementType, 0);
	elementClass = ILClassResolve(elementClass);
	classPrivate = (ILClassPrivate *)(elementClass->userData);

	if(!classPrivate)
	{
		if(!_LayoutClass(thread, elementClass))
		{
			return (ILJitValue)0;
		}
		classPrivate = (ILClassPrivate *)(elementClass->userData);
	}
#if !defined(IL_CONFIG_REDUCE_CODE) && !defined(IL_WITHOUT_TOOLS) && defined(_IL_JIT_ENABLE_DEBUG)
	if(jitCoder->flags & IL_CODER_FLAG_STATS)
	{
		ILMutexLock(globalTraceMutex);
		fprintf(stdout, "New Array of type: %s\n", ILClass_Name(elementClass));
		ILMutexUnlock(globalTraceMutex);
	}
#endif
	/* We call the alloc functions. */
	/* They thow an out of memory exception so we don't need to care. */
	args[0] = jit_value_create_nint_constant(jitCoder->jitFunction,
											 _IL_JIT_TYPE_VPTR,
											 (jit_nint)arrayClass);
	args[1] = jit_insn_add(jitCoder->jitFunction, arraySize, headerSize);

	if(classPrivate->managedInstance)
	{
		/* The element contains object references. */
		newArray = jit_insn_call_native(jitCoder->jitFunction,
										"_ILJitAlloc",
										_ILJitAlloc,
										_ILJitSignature_ILJitAlloc,
			 							args, 2, 0);
	}
	else
	{
		newArray = jit_insn_call_native(jitCoder->jitFunction,
										"_ILJitAllocAtomic",
										_ILJitAllocAtomic,
										_ILJitSignature_ILJitAlloc,
			 							args, 2, 0);
	}
	/* Set the length in the array. */
	jit_insn_store_relative(jitCoder->jitFunction, newArray, 
							offsetof(System_Array, length), length);
	return newArray;
}


/*
 * Calculate the absolute Index of an element in the array.
 * Throws an IndexOutOfRangeException if any of the indexes is out of bounds.
 */
static ILJitValue _ILJitMArrayCalcIndex(ILJITCoder *jitCoder,
										ILJitValue array,
										ILJitValue *index,
										ILInt32 rank)
{
	jit_label_t arrayOutOfBoundsLabel = jit_label_undefined;
	jit_label_t okLabel = jit_label_undefined;
	ILJitValue absoluteIndex = jit_value_create(jitCoder->jitFunction,
												_IL_JIT_TYPE_INT32);
	ILInt32 currentIndex = 0;

	for(currentIndex = 0; currentIndex < rank; currentIndex++)
	{
		ILJitValue temp;
		ILJitValue multiplier;
		ILJitValue pos = index[currentIndex];
		ILJitValue base = jit_insn_load_relative(jitCoder->jitFunction, array,
												 offsetof(System_MArray, bounds) +
												 (sizeof(MArrayBounds) * currentIndex) +
												 offsetof(MArrayBounds, lower),
												 _IL_JIT_TYPE_UINT32);
		ILJitValue length = jit_insn_load_relative(jitCoder->jitFunction, array,
												 offsetof(System_MArray, bounds) +
												 (sizeof(MArrayBounds) * currentIndex) +
												 offsetof(MArrayBounds, size),
												 _IL_JIT_TYPE_UINT32);

		/* calculate the real index for this dimension. */
		pos = jit_insn_sub(jitCoder->jitFunction, pos, base);

		/* Make pos unsigned. We can save one compare this way. */
		temp = pos;
		AdjustSign(jitCoder->jitFunction, &temp, 1, 0);
		temp = jit_insn_lt(jitCoder->jitFunction, temp, length);
		jit_insn_branch_if_not(jitCoder->jitFunction, temp, &arrayOutOfBoundsLabel);

		/* the multilplier is 1 for the last dimension so we don't have to */
		/* use it then.*/
		if(currentIndex < (rank -1))
		{
			multiplier = jit_insn_load_relative(jitCoder->jitFunction, array,
												offsetof(System_MArray, bounds) +
												(sizeof(MArrayBounds) * currentIndex) +
												offsetof(MArrayBounds, multiplier),
												_IL_JIT_TYPE_INT32);
			pos = jit_insn_mul(jitCoder->jitFunction, pos, multiplier);
		}
		if(currentIndex == 0)
		{
			jit_insn_store(jitCoder->jitFunction, absoluteIndex, pos);
		}
		else
		{
			pos = jit_insn_add(jitCoder->jitFunction, absoluteIndex, pos);
			jit_insn_store(jitCoder->jitFunction, absoluteIndex, pos);
		}
	}
	jit_insn_branch(jitCoder->jitFunction, &okLabel);
	jit_insn_label(jitCoder->jitFunction, &arrayOutOfBoundsLabel);
	/* Throw a System.IndexOutOfRange exception. */
	_ILJitThrowSystem(jitCoder, _IL_JIT_INDEX_OUT_OF_RANGE);
	jit_insn_label(jitCoder->jitFunction, &okLabel);

	return absoluteIndex;
}

/*
 * Get the pointer to the start of the data in a MArray.
 */
static ILJitValue _ILJitMarrayGetBase(ILJITCoder *jitCoder,
									  ILJitValue array)
{
	ILJitValue data = jit_insn_load_relative(jitCoder->jitFunction, array,
											 offsetof(System_MArray, data),
											 _IL_JIT_TYPE_VPTR);
	
	return data;
}

/*
 * Get an array element at the specified indexes from a complex array.
 */
static ILJitValue _ILJitMArrayGet(ILJITCoder *jitCoder,
								  ILMethod *method,
								  ILCoderMethodInfo *methodInfo,
								  ILJitValue *args,
								  ILInt32 numArgs)
{
	ILJitFunction jitFunction = ILJitFunctionFromILMethod(method);
	ILJitValue array = args[0];
	ILJitValue arrayBase;
	ILJitType signature;
	ILJitType arrayType;
	ILJitValue index;

	if(!jitFunction)
	{
		/* We need to layout the class first. */
		if(!_LayoutClass(ILExecThreadCurrent(), ILMethod_Owner(methodInfo)))
		{
			return 0;
		}
		jitFunction = ILJitFunctionFromILMethod(method);
	}

	signature = jit_function_get_signature(jitFunction);
	arrayType = jit_type_get_return(signature);
	index = _ILJitMArrayCalcIndex(jitCoder, array, &(args[1]), numArgs - 1);
	arrayBase = _ILJitMarrayGetBase(jitCoder, array);
	return jit_insn_load_elem(jitCoder->jitFunction, arrayBase, index, arrayType);
}

/*
 * Get the address of an array element at the specified indexes in a complex
 * array.
 */
static ILJitValue _ILJitMArrayAddress(ILJITCoder *jitCoder,
									  ILMethod *method,
									  ILCoderMethodInfo *methodInfo,
									  ILJitValue *args,
									  ILInt32 numArgs)
{
	ILJitFunction jitFunction = ILJitFunctionFromILMethod(method);
	ILJitValue array = args[0];
	ILClass *arrayClass = ILMethod_Owner(method);
	ILType *elementType = ILTypeGetElemType(ILClassToType(arrayClass));
	ILJitValue arrayBase;
	ILJitType signature;
	ILJitType arrayType;
	ILJitValue index;

	if(!jitFunction)
	{
		/* We need to layout the class first. */
		if(!_LayoutClass(ILExecThreadCurrent(), ILMethod_Owner(methodInfo)))
		{
			return 0;
		}
		jitFunction = ILJitFunctionFromILMethod(method);
	}

	signature = jit_function_get_signature(jitFunction);
	arrayType = _ILJitGetReturnType(elementType, 
									((ILClassPrivate *)arrayClass->userData)->process);
	index = _ILJitMArrayCalcIndex(jitCoder, array, &(args[1]), numArgs - 1);
	arrayBase = _ILJitMarrayGetBase(jitCoder, array);
	return jit_insn_load_elem_address(jitCoder->jitFunction,
									  arrayBase,
									  index,
									  arrayType);
}

/*
 * Set an array element at the specified indexes in a complex array.
 */
static ILJitValue _ILJitMArraySet(ILJITCoder *jitCoder,
								  ILMethod *method,
								  ILCoderMethodInfo *methodInfo,
								  ILJitValue *args,
								  ILInt32 numArgs)
{
	ILJitFunction jitFunction = ILJitFunctionFromILMethod(method);
	ILJitValue array = args[0];
	ILJitValue value = args[numArgs - 1];
	ILJitValue arrayBase;
	ILJitType signature;
	ILJitType arrayType;
	ILJitValue index;

	if(!jitFunction)
	{
		/* We need to layout the class first. */
		if(!_LayoutClass(ILExecThreadCurrent(), ILMethod_Owner(methodInfo)))
		{
			return 0;
		}
		jitFunction = ILJitFunctionFromILMethod(method);
	}

	signature = jit_function_get_signature(jitFunction);
#ifdef IL_JIT_THREAD_IN_SIGNATURE
	arrayType = jit_type_get_param(signature, numArgs);
#else
	arrayType = jit_type_get_param(signature, numArgs - 1);
#endif
	index = _ILJitMArrayCalcIndex(jitCoder, array, &(args[1]), numArgs - 2);
	arrayBase = _ILJitMarrayGetBase(jitCoder, array);
	value = _ILJitValueConvertImplicit(jitCoder->jitFunction, value, arrayType);
	jit_insn_store_elem(jitCoder->jitFunction, arrayBase, index, value);
	/* We have no return value in this case. */
	return 0;
}

