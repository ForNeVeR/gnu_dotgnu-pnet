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

#ifdef IL_CONFIG_PINVOKE

/*
 * Get the address of a PInvoke-imported field.
 */
static ILJitValue _ILJitGetPInvokeFieldAddress(ILJitFunction func,
											   ILField *field,
											   ILPInvoke *pinvoke)
{
	char *name;
	void *handle;
	void *symbol;
	const char *symbolName;
	ILJitValue ptr;

	/* Resolve the module */
	name = ILPInvokeResolveModule(pinvoke);
	if(!name)
	{
		ptr = jit_value_create_nint_constant(func, _IL_JIT_TYPE_VPTR, 0);
		jit_insn_check_null(func, ptr);
		return ptr;
	}

	/* Load the module into memory */
	handle = ILDynLibraryOpen(name);
	ILFree(name);
	if(!handle)
	{
		ptr = jit_value_create_nint_constant(func, _IL_JIT_TYPE_VPTR, 0);
		jit_insn_check_null(func, ptr);
		return ptr;
	}

	/* Resolve the symbol and resolve its address */
	symbolName = ILPInvoke_Alias(pinvoke);
	if(!symbolName)
	{
		symbolName = ILField_Name(field);
	}
	symbol = ILDynLibraryGetSymbol(handle, symbolName);

	/* Check if the symbol could be resolved. */
	if(!symbol)
	{
		ptr = jit_value_create_nint_constant(func, _IL_JIT_TYPE_VPTR, 0);
		jit_insn_check_null(func, ptr);
		return ptr;
	}
	ptr = jit_value_create_nint_constant(func,
										 _IL_JIT_TYPE_VPTR,
										 (jit_nint)symbol);
	return ptr;
}

#endif /* IL_CONFIG_PINVOKE */

/*
 * Get a ILJitValue with the address of a RVA static field.
 */
static ILJitValue _ILJitGetRVAAddress(ILJitFunction func,
									  ILField *field)
{
	ILFieldRVA *fieldRVA = ILFieldRVAGetFromOwner(field);
	ILUInt32 rva = ILFieldRVAGetRVA(fieldRVA);
	/* Eventually the address should be computed on runtime. */
	void *ptr = ILImageMapAddress(ILProgramItem_Image(field), rva);

	return jit_value_create_nint_constant(func,
										  _IL_JIT_TYPE_VPTR,
										  (jit_nint)ptr);
}

/*
 * Get an ILJitValue with a pointer to the thread static slot.
 */
static ILJitValue _ILJitGetThreadStaticSlot(ILJITCoder *jitCoder,
										    ILField *field)
{
	ILJitValue args[3];
	ILJitValue slot;
	jit_label_t label = jit_label_undefined;

	args[0] = _ILJitCoderGetThread(jitCoder);
	args[1] = jit_value_create_nint_constant(jitCoder->jitFunction,
											 _IL_JIT_TYPE_UINT32,
											 (jit_nint)field->offset);
	args[2] = jit_value_create_nint_constant(jitCoder->jitFunction,
											 _IL_JIT_TYPE_UINT32,
											 (jit_nint)field->nativeOffset);
	slot = jit_insn_call_native(jitCoder->jitFunction,
								"ILRuntimeGetThreadStatic",
								ILRuntimeGetThreadStatic,
								_ILJitSignature_ILRuntimeGetThreadStatic,
								args, 3, JIT_CALL_NOTHROW);
	jit_insn_branch_if(jitCoder->jitFunction, slot, &label);
	_ILJitThrowCurrentException(jitCoder);
	jit_insn_label(jitCoder->jitFunction, &label);
	return slot;
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
	ILJitValue info = jit_value_create_nint_constant(jitCoder->jitFunction,
				    								 _IL_JIT_TYPE_VPTR,
								    				 (jit_nint)classInfo);
	ILJitValue ptr = jit_insn_address_of(jitCoder->jitFunction, refValue);;
	ILJitValue type = jit_insn_load_relative(jitCoder->jitFunction,
											 ptr,
											 offsetof(ILTypedRef, type),
											 _IL_JIT_TYPE_VPTR);
	ILJitValue temp;
	ILJitValue valuePtr;

	temp = jit_insn_eq(jitCoder->jitFunction, type, info);
	jit_insn_branch_if(jitCoder->jitFunction, temp, &label1);
	/* Throw an InvalidCastException */
	_ILJitThrowSystem(jitCoder, _IL_JIT_INVALID_CAST);
	jit_insn_label(jitCoder->jitFunction, &label1);
	valuePtr = jit_insn_load_relative(jitCoder->jitFunction, ptr,
									  offsetof(ILTypedRef, value),
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
		_ILJitCheckNull(coder, base);
	}
	if(offset != 0)
	{
		return jit_insn_add_relative(coder->jitFunction, base, offset);
	}
	else
	{
		return base;
	}
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
		_ILJitCheckNull(coder, base);
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
		_ILJitCheckNull(coder, base);
	}
	if(jit_value_get_type(value) != type)
	{
		value = _ILJitValueConvertImplicit(coder->jitFunction, value, type);
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
	ILJitValue result = jit_value_create(jitCoder->jitFunction,
										 _IL_JIT_TYPE_VPTR);

	if(!throwException)
	{
		jit_insn_store(jitCoder->jitFunction, result, object);
	}
	jit_insn_branch_if_not(jitCoder->jitFunction, object, &label);
	if(ILClass_IsInterface(classInfo))
	{
		/* We are casting to an interface */
		args[0] = object;
		args[1] = classTo;
		returnValue = jit_insn_call_native(jitCoder->jitFunction,
										   "ILRuntimeClassImplements",
										   ILRuntimeClassImplements,
										   _ILJitSignature_ILRuntimeClassImplements,
										   args, 2, JIT_CALL_NOTHROW);
	}
	else
	{
		args[0] = jit_value_create_nint_constant(jitCoder->jitFunction,
												 _IL_JIT_TYPE_VPTR,
												 (jit_nint)jitCoder->currentMethod);
		args[1] = object;
		args[2] = classTo;
		returnValue = jit_insn_call_native(jitCoder->jitFunction,
										   "ILRuntimeCanCastClass",
										   ILRuntimeCanCastClass,
										   _ILJitSignature_ILRuntimeCanCastClass,
										   args, 3, JIT_CALL_NOTHROW);
	}
	if(throwException)
	{
		jit_insn_branch_if(jitCoder->jitFunction, returnValue, &label);
		/* Throw an InvalidCastException. */
		_ILJitThrowSystem(jitCoder, _IL_JIT_INVALID_CAST);

	}
	else
	{
		ILJitValue nullPointer = 
				jit_value_create_nint_constant(jitCoder->jitFunction,
											   _IL_JIT_TYPE_VPTR,
											   (jit_nint)0);
	
		jit_insn_branch_if(jitCoder->jitFunction, returnValue, &label);
		jit_insn_store(jitCoder->jitFunction, result, nullPointer);
		jitCoder->jitStack[jitCoder->stackTop - 1] = result;
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
	ILJitValue param = _ILJitParamValue(jitCoder, 0);

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

#ifdef IL_CONFIG_PINVOKE
	ILPInvoke *pinvoke;
	if((field->member.attributes & IL_META_FIELDDEF_PINVOKE_IMPL) != 0 &&
	   (pinvoke = ILPInvokeFindField(field)) != 0)
	{
		/* Field that is imported via PInvoke */
		ILJitValue ptr = _ILJitGetPInvokeFieldAddress(jitCoder->jitFunction,
													  field,
													  pinvoke);

		#if !defined(IL_CONFIG_REDUCE_CODE) && !defined(IL_WITHOUT_TOOLS)
			if (jitCoder->flags & IL_CODER_FLAG_STATS)
			{
				ILMutexLock(globalTraceMutex);
				fprintf(stdout,
					"LoadStaticPinvokeField: %s.%s\n", 
					ILClass_Name(ILField_Owner(field)),
					ILField_Name(field));
				ILMutexUnlock(globalTraceMutex);
			}
		#endif
		jitCoder->jitStack[jitCoder->stackTop] =
				_ILJitLoadField(jitCoder, ptr, fieldType, 0, 0);
	}
	else
#endif
	if((field->member.attributes & IL_META_FIELDDEF_HAS_FIELD_RVA) == 0)
	{
		/* Output a call to the static constructor */
		_ILJitCallStaticConstructor(jitCoder, ILField_Owner(field), 1);

		/* Regular or thread-static field? */
		if(!ILFieldIsThreadStatic(field))
		{
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
			jitCoder->jitStack[jitCoder->stackTop] =
				_ILJitLoadField(jitCoder, ptr, fieldType, field->offset, 0);
		}
		else
		{
			ILJitValue ptr = _ILJitGetThreadStaticSlot(jitCoder, field);

		#if !defined(IL_CONFIG_REDUCE_CODE) && !defined(IL_WITHOUT_TOOLS)
			if (jitCoder->flags & IL_CODER_FLAG_STATS)
			{
				ILMutexLock(globalTraceMutex);
				fprintf(stdout,
					"LoadThreadStaticField: %s.%s\n", 
					ILClass_Name(ILField_Owner(field)),
					ILField_Name(field));
				ILMutexUnlock(globalTraceMutex);
			}
		#endif
			jitCoder->jitStack[jitCoder->stackTop] =
				_ILJitLoadField(jitCoder, ptr, fieldType, 0, 0);
		}
	}
	else
	{
		ILJitValue ptr = _ILJitGetRVAAddress(jitCoder->jitFunction, field);

		#if !defined(IL_CONFIG_REDUCE_CODE) && !defined(IL_WITHOUT_TOOLS)
			if (jitCoder->flags & IL_CODER_FLAG_STATS)
			{
				ILMutexLock(globalTraceMutex);
				fprintf(stdout,
					"LoadRVAStaticField: %s.%s\n", 
					ILClass_Name(ILField_Owner(field)),
					ILField_Name(field));
				ILMutexUnlock(globalTraceMutex);
			}
		#endif
			jitCoder->jitStack[jitCoder->stackTop] =
				_ILJitLoadField(jitCoder, ptr, fieldType, 0, 0);
	}
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

#ifdef IL_CONFIG_PINVOKE
	ILPInvoke *pinvoke;
	if((field->member.attributes & IL_META_FIELDDEF_PINVOKE_IMPL) != 0 &&
	   (pinvoke = ILPInvokeFindField(field)) != 0)
	{
		/* Field that is imported via PInvoke */
		ILJitValue ptr = _ILJitGetPInvokeFieldAddress(jitCoder->jitFunction,
													  field,
													  pinvoke);

		#if !defined(IL_CONFIG_REDUCE_CODE) && !defined(IL_WITHOUT_TOOLS)
			if (jitCoder->flags & IL_CODER_FLAG_STATS)
			{
				ILMutexLock(globalTraceMutex);
				fprintf(stdout,
					"LoadStaticPivokeFieldAddr: %s.%s\n", 
					ILClass_Name(ILField_Owner(field)),
					ILField_Name(field));
				ILMutexUnlock(globalTraceMutex);
			}
		#endif
			jitCoder->jitStack[jitCoder->stackTop] =
				_ILJitLoadFieldAddress(jitCoder, ptr, 0, 0);

		JITC_ADJUST(jitCoder, 1);
		return;
	}
#endif

	/* Output a call to the static constructor */
	_ILJitCallStaticConstructor(jitCoder, ILField_Owner(field), 1);

	/* Regular or RVA field? */
	if((field->member.attributes & IL_META_FIELDDEF_HAS_FIELD_RVA) == 0)
	{
		/* Regular or thread-static field? */
		if(!ILFieldIsThreadStatic(field))
		{
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
			jitCoder->jitStack[jitCoder->stackTop] =
				_ILJitLoadFieldAddress(jitCoder, ptr, field->offset, 0);
		}
		else
		{
			ILJitValue ptr = _ILJitGetThreadStaticSlot(jitCoder, field);

		#if !defined(IL_CONFIG_REDUCE_CODE) && !defined(IL_WITHOUT_TOOLS)
			if (jitCoder->flags & IL_CODER_FLAG_STATS)
			{
				ILMutexLock(globalTraceMutex);
				fprintf(stdout,
					"LoadThreadStaticFieldAddr: %s.%s\n", 
					ILClass_Name(ILField_Owner(field)),
					ILField_Name(field));
				ILMutexUnlock(globalTraceMutex);
			}
		#endif
			jitCoder->jitStack[jitCoder->stackTop] =
				_ILJitLoadFieldAddress(jitCoder, ptr, 0, 0);
		}
	}
	else
	{
		ILJitValue ptr = _ILJitGetRVAAddress(jitCoder->jitFunction, field);

		#if !defined(IL_CONFIG_REDUCE_CODE) && !defined(IL_WITHOUT_TOOLS)
			if (jitCoder->flags & IL_CODER_FLAG_STATS)
			{
				ILMutexLock(globalTraceMutex);
				fprintf(stdout,
					"LoadRVAStaticFieldAddr: %s.%s\n", 
					ILClass_Name(ILField_Owner(field)),
					ILField_Name(field));
				ILMutexUnlock(globalTraceMutex);
			}
		#endif
			jitCoder->jitStack[jitCoder->stackTop] =
				_ILJitLoadFieldAddress(jitCoder, ptr, 0, 0);
	}
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
	ILJitValue value = jitCoder->jitStack[jitCoder->stackTop - 1];
#ifdef IL_CONFIG_PINVOKE
	ILPInvoke *pinvoke;
#endif

	/* Output a call to the static constructor */
	_ILJitCallStaticConstructor(jitCoder, ILField_Owner(field), 1);

#ifdef IL_CONFIG_PINVOKE
	if((field->member.attributes & IL_META_FIELDDEF_PINVOKE_IMPL) != 0 &&
	   (pinvoke = ILPInvokeFindField(field)) != 0)
	{
		/* Field that is imported via PInvoke */
		ILJitValue ptr = _ILJitGetPInvokeFieldAddress(jitCoder->jitFunction,
													  field,
													  pinvoke);

		#if !defined(IL_CONFIG_REDUCE_CODE) && !defined(IL_WITHOUT_TOOLS)
			if (jitCoder->flags & IL_CODER_FLAG_STATS)
			{
				ILMutexLock(globalTraceMutex);
				fprintf(stdout,
					"StoreStaticPinvokeField: %s.%s\n", 
					ILClass_Name(ILField_Owner(field)),
					ILField_Name(field));
				ILMutexUnlock(globalTraceMutex);
			}
		#endif
			_ILJitStoreField(jitCoder, ptr, value, fieldType, 0, 0);
	}
	else
#endif
	if((field->member.attributes & IL_META_FIELDDEF_HAS_FIELD_RVA) == 0)
	{
		/* Regular or thread-static field? */
		if(!ILFieldIsThreadStatic(field))
		{
			ILJitValue ptr = _ILJitGetClassStaticArea(jitCoder->jitFunction,
													  field);

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
			_ILJitStoreField(jitCoder, ptr, value, fieldType, field->offset, 1);
		}
		else
		{
			ILJitValue ptr = _ILJitGetThreadStaticSlot(jitCoder, field);

		#if !defined(IL_CONFIG_REDUCE_CODE) && !defined(IL_WITHOUT_TOOLS)
			if (jitCoder->flags & IL_CODER_FLAG_STATS)
			{
				ILMutexLock(globalTraceMutex);
				fprintf(stdout,
					"StoreThreadStaticField: %s.%s\n", 
					ILClass_Name(ILField_Owner(field)),
					ILField_Name(field));
				ILMutexUnlock(globalTraceMutex);
			}
		#endif
			_ILJitStoreField(jitCoder, ptr, value, fieldType, 0, 0);
		}
	}
	else
	{
		ILJitValue ptr = _ILJitGetRVAAddress(jitCoder->jitFunction, field);

		#if !defined(IL_CONFIG_REDUCE_CODE) && !defined(IL_WITHOUT_TOOLS)
			if (jitCoder->flags & IL_CODER_FLAG_STATS)
			{
				ILMutexLock(globalTraceMutex);
				fprintf(stdout,
					"StoreRVAStaticField: %s.%s\n", 
					ILClass_Name(ILField_Owner(field)),
					ILField_Name(field));
				ILMutexUnlock(globalTraceMutex);
			}
		#endif
			_ILJitStoreField(jitCoder, ptr, value, fieldType, 0, 0);
	}
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
	_ILJitCheckNull(jitCoder, dest);
	_ILJitCheckNull(jitCoder, src);

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
	_ILJitCheckNull(jitCoder, dest);
	_ILJitCheckNull(jitCoder, src);

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
	_ILJitCheckNull(jitCoder, dest);

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
	_ILJitCheckNull(jitCoder, dest);

	value = _ILJitValueConvertImplicit(jitCoder->jitFunction, value,
									   _IL_JIT_TYPE_BYTE);
	jit_insn_memset(jitCoder->jitFunction, dest, value, size);
	JITC_ADJUST(jitCoder, -3);
}

static void JITCoder_Box(ILCoder *coder, ILClass *boxClass,
					     ILEngineType valueType, ILUInt32 size)
{
	ILJITCoder *jitCoder = _ILCoderToILJITCoder(coder);

	ILJitValue value = jitCoder->jitStack[jitCoder->stackTop - 1];
	ILJitValue newObj;
	ILType *type = ILClassToType(boxClass);
	ILJitType jitType = _ILJitGetReturnType(type, jitCoder->process);

	if(valueType == ILEngineType_TypedRef)
	{
#if !defined(IL_CONFIG_REDUCE_CODE) && !defined(IL_WITHOUT_TOOLS)
		if (jitCoder->flags & IL_CODER_FLAG_STATS)
		{
			ILMutexLock(globalTraceMutex);
			fprintf(stdout,
				"BoxTypedRef: %s Size: %i\n", 
				ILClass_Name(boxClass),
				size);
			ILMutexUnlock(globalTraceMutex);
		}
#endif
		/* We have to unpack the value first. */	
		ILJitValue ptr = _ILJitGetValFromRef(jitCoder, value, boxClass);
		value = jit_insn_load_relative(jitCoder->jitFunction, ptr, 0, jitType);
	}
#if !defined(IL_CONFIG_REDUCE_CODE) && !defined(IL_WITHOUT_TOOLS)
	else
	{
		if (jitCoder->flags & IL_CODER_FLAG_STATS)
		{
			ILMutexLock(globalTraceMutex);
			fprintf(stdout,
				"Box: %s Size: %i\n", 
				ILClass_Name(boxClass),
				size);
			ILMutexUnlock(globalTraceMutex);
		}
	}
#endif

	/* Allocate the object. */
	newObj = _ILJitAllocObjectGen(jitCoder->jitFunction, boxClass);

	if(jit_value_get_type(value) != jitType)
	{
		value = _ILJitValueConvertImplicit(jitCoder->jitFunction, value,
										   jitType);
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

	/* Allocate memory */
	newObj = _ILJitAllocGen(jitCoder->jitFunction,
							boxClass, jit_type_get_size(jitType));
	
	/* If the smallerType is smaller then the initiale type then convert to it. */
	if(jit_value_get_type(value) != jitType)
	{
		value = _ILJitValueConvertImplicit(jitCoder->jitFunction, value,
										   jitType);
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
	/* Create the structure */
	ILJitValue value = jit_value_create(jitCoder->jitFunction, _ILJitTypedRef);
	ILJitValue ptr = jit_insn_address_of(jitCoder->jitFunction, value);
				
	ILJitValue typeConst = jit_value_create_nint_constant(jitCoder->jitFunction,
														  _IL_JIT_TYPE_VPTR,
														  (jit_nint)classInfo);
	
	jit_insn_store_relative(jitCoder->jitFunction, ptr,
							offsetof(ILTypedRef, type), typeConst);
				
	jit_insn_store_relative(jitCoder->jitFunction, ptr,
							offsetof(ILTypedRef, value),
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
	jitCoder->jitStack[jitCoder->stackTop - 1] =
							jit_insn_load_relative(jitCoder->jitFunction,
												   ptr,
												   offsetof(ILTypedRef, type),
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
														  _IL_JIT_TYPE_INT32,
														  (jit_nint)size);
	jitCoder->jitStack[jitCoder->stackTop] = constSize;
	JITC_ADJUST(jitCoder, 1);
}

static void JITCoder_ArgList(ILCoder *coder)
{
}

#endif	/* IL_JITC_CODE */
