/*
 * jitc_obj.c - Coder implementation for JIT object operations.
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

#ifdef IL_JITC_CODE

/*
 * Get a ILJitValue with a pointer to the class' static area.
 */
static ILJitValue _ILJitGetClassStaticArea(ILJitFunction func,
										   ILField *field)
{
	/* Get the static data area for a particular class */
	ILClass *classInfo = ILField_Owner(field);
	void *classStaticData = ((ILClassPrivate *)(classInfo->userData))->staticData;

	if(!classStaticData)
	{
		ILExecThread *thread = ILExecThreadCurrent();

		if(((ILClassPrivate *)(classInfo->userData))->managedStatic)
		{
			classStaticData = _ILEngineAlloc(thread, 0,
			   ((ILClassPrivate *)(classInfo->userData))->staticSize);
		}
		else
		{
			/* There are no managed fields in the static area,
		   	so use atomic allocation */
			classStaticData = _ILEngineAllocAtomic(thread, 0,
			   ((ILClassPrivate *)(classInfo->userData))->staticSize);
		}
		if(!classStaticData)
		{
			jit_exception_builtin(JIT_RESULT_OUT_OF_MEMORY);
		}
		else
		{
			((ILClassPrivate *)(classInfo->userData))->staticData = classStaticData;
		}
	}
	return jit_value_create_nint_constant(func, _IL_JIT_TYPE_VPTR,
												(jit_nint)classStaticData);
}

/*
 * Get the value pointer from a typed reference.
 * A check is made if the type of typed reference matches the given classInfo.
 * If they don't match an InvalidCastEWxception is thrown.
 */
static ILJitValue _ILJitGetValFromRef(ILJITCoder *jitCoder, ILJitValue refValue,
									  ILClass *classInfo)
{
	jit_label_t label1 = jit_label_undefined;
	jit_nuint typeOffset = jit_type_get_offset(_ILJitTypedRef, 0);
	jit_nuint valueOffset = jit_type_get_offset(_ILJitTypedRef, 1);
	ILJitValue info = jit_value_create_nint_constant(jitCoder->jitFunction,
				    								 _IL_JIT_TYPE_VPTR,
								    				 (jit_nint)classInfo);
	ILJitValue ptr = jit_insn_address_of(jitCoder->jitFunction, refValue);;
	ILJitValue type = jit_insn_load_relative(jitCoder->jitFunction,
											 ptr,
											 typeOffset,
											 _IL_JIT_TYPE_VPTR);
	ILJitValue temp;
	ILJitValue valuePtr;

	temp = jit_insn_eq(jitCoder->jitFunction, type, info);
	jit_insn_branch_if(jitCoder->jitFunction, temp, &label1);
	/* TODO: Throw System.InvalidCastException */

	jit_insn_label(jitCoder->jitFunction, &label1);
	valuePtr = jit_insn_load_relative(jitCoder->jitFunction, ptr, valueOffset,
									  _IL_JIT_TYPE_VPTR);
	return valuePtr;
}

/*
 * Process the ldflda / ldsflda opcodes.
 * Returns the ILJitValue with the field address.
 */
static ILJitValue _ILJitLoadFieldAddress(ILJITCoder *coder, ILJitValue base,
										 ILUInt32 offset, int mayBeNull)
{
	if(mayBeNull)
	{
		jit_insn_check_null(coder->jitFunction, base);
	}
	return jit_insn_add_relative(coder->jitFunction, base, offset);
}

/*
 * Process the ldfld / ldsfld opcodes.
 * Returns the ILJitValue with the field contents.
 */
static ILJitValue _ILJitLoadField(ILJITCoder *coder, ILJitValue base,
								  ILType *fieldType, ILUInt32 offset,
								  int mayBeNull)
{
	ILJitType type = _ILJitGetReturnType(fieldType, coder->process);
	ILJitValue value;

	if(mayBeNull)
	{
		jit_insn_check_null(coder->jitFunction, base);
	}
	value =  jit_insn_load_relative(coder->jitFunction, base, offset, type);
	return _ILJitValueConvertToStackType(coder->jitFunction, value);
}

/*
 * Process the stfld / stsfld opcodes.
 */
static void _ILJitStoreField(ILJITCoder *coder, ILJitValue base,
								   ILJitValue value, ILType *fieldType,
								   ILUInt32 offset, int mayBeNull)
								   
{
	ILJitType type = _ILJitGetReturnType(fieldType, coder->process);

	if(mayBeNull)
	{
		jit_insn_check_null(coder->jitFunction, base);
	}
	if(jit_value_get_type(value) != type)
	{
		value = jit_insn_convert(coder->jitFunction, value, type, 0);
	}
	jit_insn_store_relative(coder->jitFunction, base, offset, value);
}

static void JITCoder_CastClass(ILCoder *coder, ILClass *classInfo,
							   int throwException)
{
	ILJITCoder *jitCoder = _ILCoderToILJITCoder(coder);
	ILJitValue classTo = jit_value_create_nint_constant(jitCoder->jitFunction,
														_IL_JIT_TYPE_VPTR,
														(jit_nint)classInfo);
	ILJitValue object = jitCoder->jitStack[jitCoder->stackTop - 1];
	jit_label_t label = jit_label_undefined;
	ILJitValue args[3];
	ILJitValue returnValue;

	jit_insn_branch_if_not(jitCoder->jitFunction, object, &label);
	if(ILClass_IsInterface(classInfo))
	{
		/* We are casting to an interface */
		args[0] = object;
		args[1] = classTo;
		returnValue = jit_insn_call_native(jitCoder->jitFunction, 0,
										   ILRuntimeClassImplements,
										   _ILJitSignature_ILRuntimeClassImplements,
										   args, 2, 0);
	}
	else
	{
		args[0] = jit_value_get_param(jitCoder->jitFunction, 0);
		args[1] = object;
		args[2] = classTo;
		returnValue = jit_insn_call_native(jitCoder->jitFunction, 0,
										   ILRuntimeCanCastClass,
										   _ILJitSignature_ILRuntimeCanCastClass,
										   args, 3, 0);
	}
	if(throwException)
	{
		jit_insn_branch_if(jitCoder->jitFunction, returnValue, &label);
		/* TODO: Throw an InvalidCastException here. */

	}
	else
	{
		jit_label_t label1 = jit_label_undefined;
		ILJitValue temp = jit_value_create(jitCoder->jitFunction, _IL_JIT_TYPE_VPTR);
		ILJitValue nullPointer = 
				jit_value_create_nint_constant(jitCoder->jitFunction,
											   _IL_JIT_TYPE_VPTR,
											   (jit_nint)0);
	
		jit_insn_branch_if(jitCoder->jitFunction, returnValue, &label1);
		jit_insn_store(jitCoder->jitFunction, temp, nullPointer);
		jit_insn_branch(jitCoder->jitFunction, &label);
		jit_insn_label(jitCoder->jitFunction, &label1);
		jit_insn_store(jitCoder->jitFunction, temp, jitCoder->jitStack[jitCoder->stackTop - 1]);
		jitCoder->jitStack[jitCoder->stackTop - 1] = temp;
	}
	jit_insn_label(jitCoder->jitFunction, &label);
}

static void JITCoder_LoadField(ILCoder *coder, ILEngineType ptrType,
							   ILType *objectType, ILField *field,
							   ILType *fieldType)
{
	ILJITCoder *jitCoder = _ILCoderToILJITCoder(coder);
	ILJitValue ptr = jitCoder->jitStack[jitCoder->stackTop - 1];

#if !defined(IL_CONFIG_REDUCE_CODE) && !defined(IL_WITHOUT_TOOLS)
	if (jitCoder->flags & IL_CODER_FLAG_STATS)
	{
		ILMutexLock(globalTraceMutex);
		fprintf(stdout,
			"LoadField: %s.%s at offset %i\n", 
			ILClass_Name(ILField_Owner(field)),
			ILField_Name(field),
			field->offset);
		ILMutexUnlock(globalTraceMutex);
	}
#endif
	jitCoder->jitStack[jitCoder->stackTop - 1] =
		_ILJitLoadField(jitCoder, ptr, fieldType, field->offset, 1);
}

static void JITCoder_LoadThisField(ILCoder *coder, ILField *field,
							   	   ILType *fieldType)
{
	ILJITCoder *jitCoder = _ILCoderToILJITCoder(coder);
	/* We need argNum + 1 because the ILExecThread is added as param 0 */
	ILJitValue param = jit_value_get_param(jitCoder->jitFunction, 1);

#if !defined(IL_CONFIG_REDUCE_CODE) && !defined(IL_WITHOUT_TOOLS)
	if (jitCoder->flags & IL_CODER_FLAG_STATS)
	{
		ILMutexLock(globalTraceMutex);
		fprintf(stdout,
			"LoadThisField: %s.%s at offset %i\n", 
			ILClass_Name(ILField_Owner(field)),
			ILField_Name(field),
			field->offset);
		ILMutexUnlock(globalTraceMutex);
	}
#endif
	jitCoder->jitStack[jitCoder->stackTop] =
		_ILJitLoadField(jitCoder, param, fieldType, field->offset, 1);
	JITC_ADJUST(jitCoder, 1);
}

static void JITCoder_LoadStaticField(ILCoder *coder, ILField *field,
							         ILType *fieldType)
{
	ILJITCoder *jitCoder = _ILCoderToILJITCoder(coder);
	ILJitValue ptr = _ILJitGetClassStaticArea(jitCoder->jitFunction,
											  field);
#if !defined(IL_CONFIG_REDUCE_CODE) && !defined(IL_WITHOUT_TOOLS)
	if (jitCoder->flags & IL_CODER_FLAG_STATS)
	{
		ILMutexLock(globalTraceMutex);
		fprintf(stdout,
			"LoadStaticField: %s.%s at offset %i\n", 
			ILClass_Name(ILField_Owner(field)),
			ILField_Name(field),
			field->offset);
		ILMutexUnlock(globalTraceMutex);
	}
#endif
	/* Output a call to the static constructor */
	_ILJitCallStaticConstructor(jitCoder, ILField_Owner(field), 0);

	jitCoder->jitStack[jitCoder->stackTop] =
		_ILJitLoadField(jitCoder, ptr, fieldType, field->offset, 0);
	JITC_ADJUST(jitCoder, 1);
}

static void JITCoder_LoadFieldAddr(ILCoder *coder, ILEngineType ptrType,
							       ILType *objectType, ILField *field,
							       ILType *fieldType)
{
	ILJITCoder *jitCoder = _ILCoderToILJITCoder(coder);
	ILJitValue ptr = jitCoder->jitStack[jitCoder->stackTop - 1];

#if !defined(IL_CONFIG_REDUCE_CODE) && !defined(IL_WITHOUT_TOOLS)
	if (jitCoder->flags & IL_CODER_FLAG_STATS)
	{
		ILMutexLock(globalTraceMutex);
		fprintf(stdout,
			"LoadFieldAddr: %s.%s at offset %i\n", 
			ILClass_Name(ILField_Owner(field)),
			ILField_Name(field),
			field->offset);
		ILMutexUnlock(globalTraceMutex);
	}
#endif
	jitCoder->jitStack[jitCoder->stackTop - 1] =
		_ILJitLoadFieldAddress(jitCoder, ptr, field->offset, 1);
}

static void JITCoder_LoadStaticFieldAddr(ILCoder *coder, ILField *field,
					 ILType *fieldType)
{
	ILJITCoder *jitCoder = _ILCoderToILJITCoder(coder);
	ILJitValue ptr = _ILJitGetClassStaticArea(jitCoder->jitFunction,
											  field);

#if !defined(IL_CONFIG_REDUCE_CODE) && !defined(IL_WITHOUT_TOOLS)
	if (jitCoder->flags & IL_CODER_FLAG_STATS)
	{
		ILMutexLock(globalTraceMutex);
		fprintf(stdout,
			"LoadStaticFieldAddr: %s.%s at offset %i\n", 
			ILClass_Name(ILField_Owner(field)),
			ILField_Name(field),
			field->offset);
		ILMutexUnlock(globalTraceMutex);
	}
#endif
	/* Output a call to the static constructor */
	_ILJitCallStaticConstructor(jitCoder, ILField_Owner(field), 0);

	jitCoder->jitStack[jitCoder->stackTop] =
		_ILJitLoadFieldAddress(jitCoder, ptr, field->offset, 0);
	JITC_ADJUST(jitCoder, 1);
}

static void JITCoder_StoreField(ILCoder *coder, ILEngineType ptrType,
							    ILType *objectType, ILField *field,
							    ILType *fieldType, ILEngineType valueType)
{
	ILJITCoder *jitCoder = _ILCoderToILJITCoder(coder);
	ILJitValue ptr = jitCoder->jitStack[jitCoder->stackTop - 2];
	ILJitValue value = jitCoder->jitStack[jitCoder->stackTop - 1];

#if !defined(IL_CONFIG_REDUCE_CODE) && !defined(IL_WITHOUT_TOOLS)
	if (jitCoder->flags & IL_CODER_FLAG_STATS)
	{
		ILMutexLock(globalTraceMutex);
		fprintf(stdout,
			"StoreField: %s.%s at offset %i\n", 
			ILClass_Name(ILField_Owner(field)),
			ILField_Name(field),
			field->offset);
		ILMutexUnlock(globalTraceMutex);
	}
#endif
	_ILJitStoreField(jitCoder, ptr, value, fieldType, field->offset, 1);
	JITC_ADJUST(jitCoder, -2);
}

static void JITCoder_StoreStaticField(ILCoder *coder, ILField *field,
				      ILType *fieldType, ILEngineType valueType)
{
	ILJITCoder *jitCoder = _ILCoderToILJITCoder(coder);
	ILJitValue ptr = _ILJitGetClassStaticArea(jitCoder->jitFunction,
											  field);
	ILJitValue value = jitCoder->jitStack[jitCoder->stackTop - 1];

#if !defined(IL_CONFIG_REDUCE_CODE) && !defined(IL_WITHOUT_TOOLS)
	if (jitCoder->flags & IL_CODER_FLAG_STATS)
	{
		ILMutexLock(globalTraceMutex);
		fprintf(stdout,
			"StoreStaticField: %s.%s at offset %i\n", 
			ILClass_Name(ILField_Owner(field)),
			ILField_Name(field),
			field->offset);
		ILMutexUnlock(globalTraceMutex);
	}
#endif
	/* Output a call to the static constructor */
	_ILJitCallStaticConstructor(jitCoder, ILField_Owner(field), 0);

	_ILJitStoreField(jitCoder, ptr, value, fieldType, field->offset, 1);
	JITC_ADJUST(jitCoder, -1);
}

static void JITCoder_CopyObject(ILCoder *coder, ILEngineType destPtrType,
							    ILEngineType srcPtrType, ILClass *classInfo)
{
	ILJITCoder *jitCoder = _ILCoderToILJITCoder(coder);
	ILJitValue  dest = jitCoder->jitStack[jitCoder->stackTop - 2];
	ILJitValue  src = jitCoder->jitStack[jitCoder->stackTop - 1];
	ILType *type = ILClassToType(classInfo);
    ILUInt32 size = _ILSizeOfTypeLocked(jitCoder->process, type);
	ILJitValue  memSize = jit_value_create_nint_constant(jitCoder->jitFunction,
														 _IL_JIT_TYPE_UINT32,
														 (jit_nint)size);

	/*
	 * Do the verification early.
	 */
	jit_insn_check_null(jitCoder->jitFunction, dest);
	jit_insn_check_null(jitCoder->jitFunction, src);

	jit_insn_memcpy(jitCoder->jitFunction, dest, src, memSize);
	JITC_ADJUST(jitCoder, -2);
}

static void JITCoder_CopyBlock(ILCoder *coder, ILEngineType destPtrType,
							   ILEngineType srcPtrType)
{
	ILJITCoder *jitCoder = _ILCoderToILJITCoder(coder);
	ILJitValue  src = jitCoder->jitStack[jitCoder->stackTop - 2];
	ILJitValue  dest = jitCoder->jitStack[jitCoder->stackTop - 3];
	ILJitValue  size = jitCoder->jitStack[jitCoder->stackTop - 1];

	/*
	 * Do the verification early.
	 */
	jit_insn_check_null(jitCoder->jitFunction, dest);
	jit_insn_check_null(jitCoder->jitFunction, src);

	jit_insn_memcpy(jitCoder->jitFunction, dest, src, size);
	JITC_ADJUST(jitCoder, -3);
}

static void JITCoder_InitObject(ILCoder *coder, ILEngineType ptrType,
				ILClass *classInfo)
{
	ILJITCoder *jitCoder = _ILCoderToILJITCoder(coder);
	ILType *type = ILClassToType(classInfo);
    ILUInt32 size = _ILSizeOfTypeLocked(jitCoder->process, type);
	ILJitValue  dest = jitCoder->jitStack[jitCoder->stackTop - 1];
	ILJitValue  value = jit_value_create_nint_constant(jitCoder->jitFunction,
													   _IL_JIT_TYPE_BYTE,
													   (jit_nint)0);
	ILJitValue  memSize = jit_value_create_nint_constant(jitCoder->jitFunction,
														 _IL_JIT_TYPE_UINT32,
														 (jit_nint)size);
	/*
	 * Do the verification early.
	 */
	jit_insn_check_null(jitCoder->jitFunction, dest);

	jit_insn_memset(jitCoder->jitFunction, dest, value, memSize);

	JITC_ADJUST(jitCoder, -1);
}

static void JITCoder_InitBlock(ILCoder *coder, ILEngineType ptrType)
{
	ILJITCoder *jitCoder = _ILCoderToILJITCoder(coder);
	ILJitValue  dest = jitCoder->jitStack[jitCoder->stackTop - 3];
	ILJitValue  value = jitCoder->jitStack[jitCoder->stackTop - 2];
	ILJitValue  size = jitCoder->jitStack[jitCoder->stackTop - 1];

	/*
	 * Do the verification early.
	 */
	jit_insn_check_null(jitCoder->jitFunction, dest);

	value = jit_insn_convert(jitCoder->jitFunction, value, _IL_JIT_TYPE_BYTE, 0);
	jit_insn_memset(jitCoder->jitFunction, dest, value, size);
	JITC_ADJUST(jitCoder, -3);
}

static void JITCoder_Box(ILCoder *coder, ILClass *boxClass,
					     ILEngineType valueType, ILUInt32 size)
{
	ILJITCoder *jitCoder = _ILCoderToILJITCoder(coder);

	ILJitValue value = jitCoder->jitStack[jitCoder->stackTop - 1];
	ILJitValue newObj;
	ILJitValue args[3];
	ILType *type = ILClassToType(boxClass);
	ILJitType jitType = _ILJitGetReturnType(type, jitCoder->process);
	jit_label_t label1 = jit_label_undefined;

	if(valueType == ILEngineType_TypedRef)
	{
		/* We have to unpack the value first. */	
		ILJitValue ptr = _ILJitGetValFromRef(jitCoder, value, boxClass);
		value = jit_insn_load_relative(jitCoder->jitFunction, ptr, 0, jitType);
	}

	/* Allocate the object. */
	args[0] = jit_value_get_param(jitCoder->jitFunction, 0);
	args[1] = jit_value_create_nint_constant(jitCoder->jitFunction,
							_IL_JIT_TYPE_VPTR, (jit_nint)boxClass);

	args[2] = jit_value_create_nint_constant(jitCoder->jitFunction,
							jit_type_int, size);

	newObj = jit_insn_call_native(jitCoder->jitFunction, 0, _ILEngineAlloc,
					_ILJitSignature_ILEngineAlloc, args, 3, 0);

	jit_insn_branch_if(jitCoder->jitFunction, newObj, &label1);
	_ILJitThrowCurrentException(jitCoder);
	jit_insn_label(jitCoder->jitFunction, &label1);

	if(jit_value_get_type(value)!=jitType)
	{
		value = jit_insn_convert(jitCoder->jitFunction, value, jitType, 1);
	}
	/* Box a managed value */
	jit_insn_store_relative(jitCoder->jitFunction, newObj, 0, value);
	jitCoder->jitStack[jitCoder->stackTop - 1] = newObj;
}

static void JITCoder_BoxSmaller(ILCoder *coder, ILClass *boxClass,
					   		    ILEngineType valueType, ILType *smallerType)
{
	ILJITCoder *jitCoder = _ILCoderToILJITCoder(coder);
	ILJitValue value = jitCoder->jitStack[jitCoder->stackTop - 1];
	ILJitType jitType = _ILJitGetReturnType(smallerType, jitCoder->process);

	ILJitValue newObj;
	ILJitValue args[3];

	jit_label_t label1 = jit_label_undefined;

	/* Allocate memory */
	args[0] = jit_value_get_param(jitCoder->jitFunction, 0);
	args[1] = jit_value_create_nint_constant(jitCoder->jitFunction,
											 _IL_JIT_TYPE_VPTR,
											 (jit_nint)boxClass);
	args[2] = jit_value_create_nint_constant(jitCoder->jitFunction,
											 _IL_JIT_TYPE_UINT32,
											 jit_type_get_size(jitType));
							
	newObj = jit_insn_call_native(jitCoder->jitFunction, 0, _ILEngineAlloc,
								  _ILJitSignature_ILEngineAlloc, args, 3, 0);
					
	jit_insn_branch_if(jitCoder->jitFunction, newObj, &label1);
	_ILJitThrowCurrentException(jitCoder);
	jit_insn_label(jitCoder->jitFunction, &label1);

	
	/* If the smallerType is smaller then the initiale type then convert to it. */
	if(jit_value_get_type(value) != jitType)
	{
		value = jit_insn_convert(jitCoder->jitFunction, value, jitType, 0);
	}
	
	jit_insn_store_relative(jitCoder->jitFunction, newObj, 0, value);
	jitCoder->jitStack[jitCoder->stackTop - 1] = newObj;
}

static void JITCoder_Unbox(ILCoder *coder, ILClass *boxClass)
{
	/* We don't have to do anything here: the object reference
	   points at the start of the object's fields, which is
	   exactly the pointer that we need for the unboxed value */
}

static void JITCoder_MakeTypedRef(ILCoder *coder, ILClass *classInfo)
{
	ILJITCoder *jitCoder = _ILCoderToILJITCoder(coder);
	jit_nuint typeOffset = jit_type_get_offset(_ILJitTypedRef, 0);
	jit_nuint valueOffset = jit_type_get_offset(_ILJitTypedRef, 1);
	/* Create the structure */
	ILJitValue value = jit_value_create(jitCoder->jitFunction, _ILJitTypedRef);
	ILJitValue ptr = jit_insn_address_of(jitCoder->jitFunction, value);
				
	ILJitValue typeConst = jit_value_create_nint_constant(jitCoder->jitFunction,
														  _IL_JIT_TYPE_VPTR,
														  (jit_nint)classInfo);
	
	jit_insn_store_relative(jitCoder->jitFunction, ptr, typeOffset, typeConst);
				
	jit_insn_store_relative(jitCoder->jitFunction, ptr, valueOffset,
							jitCoder->jitStack[jitCoder->stackTop - 1]);

	jitCoder->jitStack[jitCoder->stackTop - 1] = value;
}

static void JITCoder_RefAnyVal(ILCoder *coder, ILClass *classInfo)
{
	ILJITCoder *jitCoder = _ILCoderToILJITCoder(coder);
	ILJitValue value = jitCoder->jitStack[jitCoder->stackTop - 1];

	jitCoder->jitStack[jitCoder->stackTop - 1] =
					_ILJitGetValFromRef(jitCoder, value, classInfo);
}

static void JITCoder_RefAnyType(ILCoder *coder)
{
	ILJITCoder *jitCoder = _ILCoderToILJITCoder(coder);
	ILJitValue ptr =
				jit_insn_address_of(jitCoder->jitFunction,
									jitCoder->jitStack[jitCoder->stackTop - 1]);
	jit_nuint typeOffset = jit_type_get_offset(_ILJitTypedRef, 0);
	jitCoder->jitStack[jitCoder->stackTop - 1] =
								jit_insn_load_relative(jitCoder->jitFunction,
													   ptr,
													   typeOffset,
													   _IL_JIT_TYPE_VPTR);
}

static void JITCoder_PushToken(ILCoder *coder, ILProgramItem *item)
{
	ILJITCoder *jitCoder = _ILCoderToILJITCoder(coder);

	jitCoder->jitStack[jitCoder->stackTop] = 
		jit_value_create_nint_constant(jitCoder->jitFunction,
									   _IL_JIT_TYPE_VPTR, 
							    	   (jit_nint)item);

	JITC_ADJUST(jitCoder, 1);
}

static void JITCoder_SizeOf(ILCoder *coder, ILType *type)
{
	ILJITCoder *jitCoder = _ILCoderToILJITCoder(coder);
    ILUInt32 size = _ILSizeOfTypeLocked(jitCoder->process, type);
	ILJitValue constSize = jit_value_create_nint_constant(jitCoder->jitFunction,
														  _IL_JIT_TYPE_UINT32,
														  (jit_nint)size);
	jitCoder->jitStack[jitCoder->stackTop] = constSize;
	JITC_ADJUST(jitCoder, 1);
}

static void JITCoder_ArgList(ILCoder *coder)
{
}

#endif	/* IL_JITC_CODE */
