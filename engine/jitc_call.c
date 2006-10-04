/*
 * jitc_call.c - Coder implementation for JIT method calls.
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
 * Fill the argument array for the methodcall with the args on the stack.
 * This function pops the arguments off the stack too.
 */
static void _ILJitFillArguments(ILJITCoder *coder, ILJitValue *args,
								ILCoderMethodInfo *info)
{
	int argCount = info->numBaseArgs + info->numVarArgs;
	int current = 0;
	
	if(info->hasParamArray)
	{
		++argCount;
	}
	JITC_ADJUST(coder, -argCount);
	for(current = 0; current < argCount; current++)
	{
		args[current] = coder->jitStack[coder->stackTop + current];
	}
}

/*
 * Get the vtable pointer for an interface function from an object.
 */
static ILJitValue _ILJitGetInterfaceFunction(ILJITCoder *jitCoder,
											 ILJitValue object,
											 ILClass *interface,
											 int index)
{
	ILJitValue classPrivate;
	ILJitValue interfaceClass;
	ILJitValue methodIndex;
	ILJitValue args[3];
	ILJitValue jitFunction;
	jit_label_t label = jit_label_undefined;

	_ILJitCheckNull(jitCoder, object);
	classPrivate = _ILJitGetObjectClassPrivate(jitCoder->jitFunction, object);
	interfaceClass = jit_value_create_nint_constant(jitCoder->jitFunction,
													_IL_JIT_TYPE_VPTR,
													(jit_nint)interface);
	methodIndex = jit_value_create_nint_constant(jitCoder->jitFunction,
												 _IL_JIT_TYPE_UINT32,
												 (jit_nint)index);
	args[0] = classPrivate;
	args[1] = interfaceClass;
	args[2] = methodIndex;
	jitFunction = jit_insn_call_native(jitCoder->jitFunction,
									   "_ILRuntimeLookupInterfaceMethod",
									   _ILRuntimeLookupInterfaceMethod,
									   _ILJitSignature_ILRuntimeLookupInterfaceMethod,
									   args, 3, 0);

	jit_insn_branch_if(jitCoder->jitFunction, jitFunction, &label);
	_ILJitThrowSystem(jitCoder, _IL_JIT_MISSING_METHOD);
	jit_insn_label(jitCoder->jitFunction, &label);

	return jitFunction;
}

/*
 * Get the vtable pointer for a virtual function from an object.
 */
static ILJitValue _ILJitGetVirtualFunction(ILJITCoder *jitCoder,
										   ILJitValue object,
										   int index)
{
	ILJitValue classPrivate;
	ILJitValue vtable;
	ILJitValue vtableIndex;
	ILJitValue jitFunction;
	jit_label_t label = jit_label_undefined;

	_ILJitCheckNull(jitCoder, object);
	classPrivate = _ILJitGetObjectClassPrivate(jitCoder->jitFunction, object);
	vtable = jit_insn_load_relative(jitCoder->jitFunction, classPrivate, 
									offsetof(ILClassPrivate, jitVtable), 
									_IL_JIT_TYPE_VPTR);
	vtableIndex = jit_value_create_nint_constant(jitCoder->jitFunction,
												 _IL_JIT_TYPE_INT32,
												 (jit_nint)index);
	jitFunction = jit_insn_load_elem(jitCoder->jitFunction,
							  vtable, vtableIndex, _IL_JIT_TYPE_VPTR);

	jit_insn_branch_if(jitCoder->jitFunction, jitFunction, &label);
	_ILJitThrowSystem(jitCoder, _IL_JIT_MISSING_METHOD);
	jit_insn_label(jitCoder->jitFunction, &label);

	return jitFunction;
}

/*
 * Create a new object and push it on the stack.
 */
static void _ILJitNewObj(ILJITCoder *coder, ILClass *info, ILJitValue *newArg)
{
	*newArg = _ILJitAllocObjectGen(coder->jitFunction, info);
}

/*
 * Pack a set of arguments into a params "Object[]" array and replaces
 * them with the new array on the stack.
 * This is not included in VarArgs as this is needed by non-vararg operations 
 * like BeginInvoke.
 */
void _ILJitPackVarArgs(ILJITCoder *jitCoder,
							ILUInt32 firstParam, ILUInt32 numArgs,
							ILType *callSiteSig)
{
	ILExecThread *_thread = ILExecThreadCurrent();
	ILUInt32 param;
	ILType *paramType;
	ILType *enumType;
	ILClass *info;
	ILMethod *ctor;
	ILUInt32 typeSize;
	ILJitFunction jitCtor;
	ILJitValue thread = _ILJitCoderGetThread(jitCoder);
	ILJitValue arrayLength = jit_value_create_nint_constant(jitCoder->jitFunction,
															_IL_JIT_TYPE_UINT32,
															(jit_nint)numArgs);
	ILJitValue array;
	ILJitValue arrayBase;
	ILJitValue boxObject;
	ILJitValue boxValue;
	ILJitValue boxObjectSize;
	ILJitValue ptr;
	jit_label_t labelException = jit_label_undefined;
	jit_label_t label = jit_label_undefined;
	ILJitValue args[3];


	/* Allocate an array to hold all of the arguments */
	/* Find the constructor for the array. */
	ctor = ILExecThreadLookupMethod(_thread, "[oSystem.Object;", ".ctor",
									"(Ti)V");
	jitCtor = ILJitFunctionFromILMethod(ctor);
	if(!jitCtor)
	{
		/* We have to layout the class first. */
		if(!_LayoutClass(_thread, ILMethod_Owner(ctor)))
		{
			return;
		}
		jitCtor = ILJitFunctionFromILMethod(ctor);
	}
	args[0] = thread;
	args[1] = arrayLength;
	array = jit_insn_call(jitCoder->jitFunction, 0, jitCtor,
						  0, args, 2, 0);
	jit_insn_branch_if_not(jitCoder->jitFunction, array, &labelException);
	arrayBase = jit_insn_add_relative(jitCoder->jitFunction,
									  array,
									  sizeof(void *));
	/* Adjust the stack just to the first vararg. */
	JITC_ADJUST(jitCoder, -numArgs);

	/* Convert the arguments into objects in the array */
	for(param = 0; param < numArgs; ++param)
	{
		paramType = ILTypeGetParam(callSiteSig, firstParam + param);
		boxValue = 0;
		ptr = 0;

		if (ILType_IsComplex(paramType)
			&& ILType_Kind(paramType) == IL_TYPE_COMPLEX_BYREF)
		{
			paramType = ILType_Ref(paramType);

			enumType = ILTypeGetEnumType(paramType);
			if(ILType_IsPrimitive(enumType))
			{
				/* Box a primitive value after aligning it properly */
				switch(ILType_ToElement(enumType))
				{
					case IL_META_ELEMTYPE_I1:
					{
						boxValue = jit_insn_load_relative(jitCoder->jitFunction,
								jitCoder->jitStack[jitCoder->stackTop + param],
														  0, _IL_JIT_TYPE_SBYTE);
					}
					break;

					case IL_META_ELEMTYPE_U1:
					case IL_META_ELEMTYPE_BOOLEAN:
					{
						boxValue = jit_insn_load_relative(jitCoder->jitFunction,
								jitCoder->jitStack[jitCoder->stackTop + param],
														  0, _IL_JIT_TYPE_BYTE);
					}
					break;

					case IL_META_ELEMTYPE_I2:
					{
						boxValue = jit_insn_load_relative(jitCoder->jitFunction,
								jitCoder->jitStack[jitCoder->stackTop + param],
														  0, _IL_JIT_TYPE_INT16);
					}
					break;

					case IL_META_ELEMTYPE_U2:
					case IL_META_ELEMTYPE_CHAR:
					{
						boxValue = jit_insn_load_relative(jitCoder->jitFunction,
								jitCoder->jitStack[jitCoder->stackTop + param],
														  0, _IL_JIT_TYPE_UINT16);
					}
					break;

					case IL_META_ELEMTYPE_I4:
#ifdef IL_NATIVE_INT32
					case IL_META_ELEMTYPE_I:
#endif
					{
						boxValue = jit_insn_load_relative(jitCoder->jitFunction,
								jitCoder->jitStack[jitCoder->stackTop + param],
														  0, _IL_JIT_TYPE_INT32);
					}
					break;

					case IL_META_ELEMTYPE_U4:
#ifdef IL_NATIVE_INT32
					case IL_META_ELEMTYPE_U:
#endif
					{
						boxValue = jit_insn_load_relative(jitCoder->jitFunction,
								jitCoder->jitStack[jitCoder->stackTop + param],
														  0, _IL_JIT_TYPE_UINT32);
					}
					break;

					case IL_META_ELEMTYPE_I8:
#ifdef IL_NATIVE_INT64
					case IL_META_ELEMTYPE_I:
#endif
					{
						boxValue = jit_insn_load_relative(jitCoder->jitFunction,
								jitCoder->jitStack[jitCoder->stackTop + param],
														  0, _IL_JIT_TYPE_INT64);
					}
					break;

					case IL_META_ELEMTYPE_U8:
#ifdef IL_NATIVE_INT64
					case IL_META_ELEMTYPE_U:
#endif
					{
						boxValue = jit_insn_load_relative(jitCoder->jitFunction,
								jitCoder->jitStack[jitCoder->stackTop + param],
														  0, _IL_JIT_TYPE_UINT64);
					}
					break;

					case IL_META_ELEMTYPE_R4:
					{
						boxValue = jit_insn_load_relative(jitCoder->jitFunction,
								jitCoder->jitStack[jitCoder->stackTop + param],
														  0, _IL_JIT_TYPE_SINGLE);
					}
					break;

					case IL_META_ELEMTYPE_R8:
					{
						boxValue = jit_insn_load_relative(jitCoder->jitFunction,
								jitCoder->jitStack[jitCoder->stackTop + param],
														  0, _IL_JIT_TYPE_DOUBLE);
					}
					break;

					case IL_META_ELEMTYPE_R:
					{
						boxValue = jit_insn_load_relative(jitCoder->jitFunction,
								jitCoder->jitStack[jitCoder->stackTop + param],
														  0, _IL_JIT_TYPE_NFLOAT);
					}
					break;

					default:
					{
						boxValue = jit_insn_load_relative(jitCoder->jitFunction,
								jitCoder->jitStack[jitCoder->stackTop + param],
														  0, _IL_JIT_TYPE_VPTR);
					}
					break;
				}
			}
			else if(ILType_IsValueType(paramType))
			{
				ptr = jitCoder->jitStack[jitCoder->stackTop + param];
			}
			else if(ILTypeIsReference(paramType))
			{
				/* Ref to an object reference type: pass the object reference */
				boxValue = jit_insn_load_relative(jitCoder->jitFunction,
								jitCoder->jitStack[jitCoder->stackTop + param],
														  0, _IL_JIT_TYPE_VPTR);
				jit_insn_store_relative(jitCoder->jitFunction, arrayBase, 0,
										boxValue);
				boxValue = 0;
			}
			else
			{
				/* Assume that everything else is a pointer, and wrap
				it up within a "System.IntPtr" object */
				boxValue = jit_insn_load_relative(jitCoder->jitFunction,
								jitCoder->jitStack[jitCoder->stackTop + param],
														  0, _IL_JIT_TYPE_VPTR);
				paramType = ILType_Int;
			}
		}
		else
		{
			enumType = ILTypeGetEnumType(paramType);
			if(ILType_IsPrimitive(enumType))
			{
				int x;

				x = ILType_ToElement(enumType);
				/* Box a primitive value after aligning it properly */
				switch(ILType_ToElement(paramType))
				{
					case IL_META_ELEMTYPE_I1:
					{
						boxValue = jit_insn_convert(jitCoder->jitFunction, 
								jitCoder->jitStack[jitCoder->stackTop + param],
								_IL_JIT_TYPE_SBYTE, 0);
					}
					break;

					case IL_META_ELEMTYPE_U1:
					case IL_META_ELEMTYPE_BOOLEAN:
					{
						boxValue = jit_insn_convert(jitCoder->jitFunction, 
								jitCoder->jitStack[jitCoder->stackTop + param],
								_IL_JIT_TYPE_BYTE, 0);
					}
					break;

					case IL_META_ELEMTYPE_I2:
					{
						boxValue = jit_insn_convert(jitCoder->jitFunction, 
								jitCoder->jitStack[jitCoder->stackTop + param],
								_IL_JIT_TYPE_INT16, 0);
					}
					break;

					case IL_META_ELEMTYPE_U2:
					case IL_META_ELEMTYPE_CHAR:
					{
						boxValue = jit_insn_convert(jitCoder->jitFunction, 
								jitCoder->jitStack[jitCoder->stackTop + param],
								_IL_JIT_TYPE_UINT16, 0);
					}
					break;

					case IL_META_ELEMTYPE_I4:
#ifdef IL_NATIVE_INT32
					case IL_META_ELEMTYPE_I:
#endif
					{
						boxValue = jit_insn_convert(jitCoder->jitFunction, 
								jitCoder->jitStack[jitCoder->stackTop + param],
								_IL_JIT_TYPE_INT32, 0);
					}
					break;

					case IL_META_ELEMTYPE_U4:
#ifdef IL_NATIVE_INT32
					case IL_META_ELEMTYPE_U:
#endif
					{
						boxValue = jit_insn_convert(jitCoder->jitFunction, 
								jitCoder->jitStack[jitCoder->stackTop + param],
								_IL_JIT_TYPE_UINT32, 0);
					}
					break;

					case IL_META_ELEMTYPE_I8:
#ifdef IL_NATIVE_INT64
					case IL_META_ELEMTYPE_I:
#endif
					{
						boxValue = jit_insn_convert(jitCoder->jitFunction, 
								jitCoder->jitStack[jitCoder->stackTop + param],
								_IL_JIT_TYPE_INT64, 0);
					}
					break;

					case IL_META_ELEMTYPE_U8:
#ifdef IL_NATIVE_INT64
					case IL_META_ELEMTYPE_U:
#endif
					{
						boxValue = jit_insn_convert(jitCoder->jitFunction, 
								jitCoder->jitStack[jitCoder->stackTop + param],
								_IL_JIT_TYPE_UINT64, 0);
					}
					break;

					case IL_META_ELEMTYPE_R4:
					{
						boxValue = jit_insn_convert(jitCoder->jitFunction, 
								jitCoder->jitStack[jitCoder->stackTop + param],
								_IL_JIT_TYPE_SINGLE, 0);
					}
					break;

					case IL_META_ELEMTYPE_R8:
					{
						boxValue = jit_insn_convert(jitCoder->jitFunction, 
								jitCoder->jitStack[jitCoder->stackTop + param],
								_IL_JIT_TYPE_DOUBLE, 0);
					}
					break;

					case IL_META_ELEMTYPE_R:
					{
						boxValue = jit_insn_convert(jitCoder->jitFunction, 
								jitCoder->jitStack[jitCoder->stackTop + param],
								_IL_JIT_TYPE_NFLOAT, 0);
					}

					default:
					{
						boxValue = jitCoder->jitStack[jitCoder->stackTop + param];
					}
					break;
				}
			}
			else if(ILType_IsValueType(paramType))
			{
				ptr = jit_insn_address_of(jitCoder->jitFunction,
							jitCoder->jitStack[jitCoder->stackTop + param]);
			}
			else if(ILTypeIsReference(paramType))
			{
				/* Object reference type: pass it directly */
				jit_insn_store_relative(jitCoder->jitFunction, arrayBase, 0,
							jitCoder->jitStack[jitCoder->stackTop + param]);
			}
			else
			{
				/* Assume that everything else is a pointer, and wrap
				it up within a "System.IntPtr" object */
				boxValue = jit_insn_convert(jitCoder->jitFunction, 
								jitCoder->jitStack[jitCoder->stackTop + param],
								_IL_JIT_TYPE_VPTR, 0);
				paramType = ILType_Int;
			}
		}
		if(boxValue || ptr)
		{
			/* We have to box the argument. */
			info = ILClassFromType
							(ILContextNextImage(_thread->process->context, 0),
			 				0, paramType, 0);
			info = ILClassResolve(info);
			typeSize = ILSizeOfType(_thread, paramType);
			boxObjectSize = jit_value_create_nint_constant(jitCoder->jitFunction,
												_IL_JIT_TYPE_UINT32, typeSize);

			boxObject = _ILJitAllocGen(jitCoder->jitFunction, info, typeSize);
			if(boxValue)
			{
				jit_insn_store_relative(jitCoder->jitFunction, boxObject, 0,
										boxValue);
			}
			else
			{
				jit_insn_memcpy(jitCoder->jitFunction, boxObject, ptr, boxObjectSize);
			}
			jit_insn_store_relative(jitCoder->jitFunction, arrayBase, 0,
									boxObject);
		}
		arrayBase = jit_insn_add_relative(jitCoder->jitFunction,
										  arrayBase,
										  sizeof(void *));
	}

	jit_insn_branch(jitCoder->jitFunction, &label);
	jit_insn_label(jitCoder->jitFunction, &labelException);
	_ILJitThrowCurrentException(jitCoder);
	jit_insn_label(jitCoder->jitFunction, &label);

	/* push the array on the stack */
	jitCoder->jitStack[jitCoder->stackTop] = array;
	JITC_ADJUST(jitCoder, 1);
}

static void JITCoder_UpConvertArg(ILCoder *coder, ILEngineStackItem *args,
						          ILUInt32 numArgs, ILUInt32 param,
						          ILType *paramType)
{
}

static void JITCoder_DownConvertArg(ILCoder *coder, ILEngineStackItem *args,
						            ILUInt32 numArgs, ILUInt32 param,
						            ILType *paramType)
{
}

static void JITCoder_PackVarArgs(ILCoder *coder, ILType *callSiteSig,
					             ILUInt32 firstParam, ILEngineStackItem *args,
						         ILUInt32 numArgs)
{
	ILJITCoder *jitCoder = _ILCoderToILJITCoder(coder);

#if !defined(IL_CONFIG_REDUCE_CODE) && !defined(IL_WITHOUT_TOOLS)
	if (jitCoder->flags & IL_CODER_FLAG_STATS)
	{
		ILMutexLock(globalTraceMutex);
		fprintf(stdout,
			"PackVarArgs: firstParam: %i, numArgs %i\n", 
			firstParam,
			numArgs);
		ILMutexUnlock(globalTraceMutex);
	}
#endif
	_ILJitPackVarArgs(jitCoder, firstParam, numArgs, callSiteSig);
}

static void JITCoder_ValueCtorArgs(ILCoder *coder, ILClass *classInfo,
								   ILEngineStackItem *args, ILUInt32 numArgs)
{
	ILJITCoder *jitCoder = _ILCoderToILJITCoder(coder);
	ILInt32 argPos = jitCoder->stackTop - numArgs;
	ILInt32 current = 0;
	ILJitType valueType = _ILJitGetTypeFromClass(classInfo);
	ILJitValue value = jit_value_create(jitCoder->jitFunction, valueType);

#if !defined(IL_CONFIG_REDUCE_CODE) && !defined(IL_WITHOUT_TOOLS)
	if (jitCoder->flags & IL_CODER_FLAG_STATS)
	{
		ILMutexLock(globalTraceMutex);
		fprintf(stdout,
			"ValueCtorArgs: %s %i\n", 
			ILClass_Name(classInfo),
			numArgs);
		ILMutexUnlock(globalTraceMutex);
	}
#endif

	jitCoder->stackTop += 2;
	/* Move the constructor args up two slots on the stack. */
	for(current = 0; current < numArgs; current++)
	{
		jitCoder->jitStack[jitCoder->stackTop - current - 1] = 
			jitCoder->jitStack[jitCoder->stackTop - current - 3];
	}
	jitCoder->jitStack[argPos] = value;
	jitCoder->jitStack[argPos + 1] = jit_insn_address_of(jitCoder->jitFunction,
														 value);
	
}

static void JITCoder_CheckCallNull(ILCoder *coder, ILCoderMethodInfo *info)
{
	/* We don't check the this pointer for non virtual instance method */
	/* calls because it degrades the the performance on these calls. */
	/* The check for null in this case is not in the ECMA specs. */
}

static void JITCoder_CallMethod(ILCoder *coder, ILCoderMethodInfo *info,
								ILEngineStackItem *returnItem,
								ILMethod *methodInfo)
{
	ILJITCoder *jitCoder = _ILCoderToILJITCoder(coder);
	ILJitFunction jitFunction = ILJitFunctionFromILMethod(methodInfo);
	int argCount = info->numBaseArgs + info->numVarArgs;
	ILJitValue jitParams[argCount + 2];
	ILJitValue returnValue;
	char *methodName = 0;
	ILInternalInfo fnInfo;
	ILJitInlineFunc inlineFunc = 0;

#if !defined(IL_CONFIG_REDUCE_CODE) && !defined(IL_WITHOUT_TOOLS)
	if (jitCoder->flags & IL_CODER_FLAG_STATS)
	{
		ILMutexLock(globalTraceMutex);
		fprintf(stdout,
			"CallMethod: %s.%s\n", 
			ILClass_Name(ILMethod_Owner(methodInfo)),
			ILMethod_Name(methodInfo));
		ILMutexUnlock(globalTraceMutex);
	}
#endif
	
	if(!jitFunction)
	{
		/* We need to layout the class first. */
		if(!_LayoutClass(ILExecThreadCurrent(), ILMethod_Owner(methodInfo)))
		{
			return;
		}
		jitFunction = ILJitFunctionFromILMethod(methodInfo);
	}

#if !defined(IL_CONFIG_REDUCE_CODE) && !defined(IL_WITHOUT_TOOLS) && defined(_IL_JIT_ENABLE_DEBUG)
	if (jitCoder->flags & IL_CODER_FLAG_STATS)
	{
		ILJitType  signature  = jit_function_get_signature(jitFunction);

		ILMutexLock(globalTraceMutex);
		fprintf(stdout,
			"CallInfos: StackTop: %i, ArgCount: %i, Signature argCount: %i\n",
			jitCoder->stackTop,
			argCount,
			jit_type_num_params(signature));
		ILMutexUnlock(globalTraceMutex);
	}
#endif
	/* Output a call to the static constructor */
	_ILJitCallStaticConstructor(jitCoder, ILMethod_Owner(methodInfo), 0);

#if !defined(IL_CONFIG_REDUCE_CODE) && !defined(IL_WITHOUT_TOOLS) && defined(_IL_JIT_ENABLE_DEBUG)
	methodName = _ILJitFunctionGetMethodName(jitFunction);
#endif

	if(info->hasParamArray)
	{
		++argCount;
	}

	/* Check if the function can be inlined. */
	if((inlineFunc = ((ILJitMethodInfo*)(methodInfo->userData))->inlineFunc))
	{
		JITC_ADJUST(jitCoder, -argCount);

		returnValue = (*inlineFunc)(jitCoder, methodInfo, info, &(jitCoder->jitStack[jitCoder->stackTop]), argCount);

		if(returnItem && returnItem->engineType != ILEngineType_Invalid)
		{
			jitCoder->jitStack[jitCoder->stackTop] =
					_ILJitValueConvertToStackType(jitCoder->jitFunction,
												  returnValue);
			JITC_ADJUST(jitCoder, 1);
		}
		return;
	}

	/* Check if the function is implemented in the engine. */
	if(_ILJitFunctionIsInternal(jitCoder, methodInfo, &fnInfo, 0))
	{
		ILJitValue thread = _ILJitCoderGetThread(jitCoder);

	#ifndef IL_CONFIG_REDUCE_CODE
		/* Emit the code to increase the call count of the method if profiling is enabled. */
		if(jitCoder->flags & IL_CODER_FLAG_METHOD_PROFILE)
		{
			_ILJitProfileIncreaseMethodCallCount(jitCoder, methodInfo);
		}
	#endif

		/* Call the engine function directly with the supplied args. */
		JITC_ADJUST(jitCoder, -argCount);
		returnValue = _ILJitCallInternal(jitCoder->jitFunction, thread,
										 methodInfo,
										 fnInfo.func, methodName,
										 &(jitCoder->jitStack[jitCoder->stackTop]),
										 argCount);
		if(returnItem && returnItem->engineType != ILEngineType_Invalid)
		{
			jitCoder->jitStack[jitCoder->stackTop] =
					_ILJitValueConvertToStackType(jitCoder->jitFunction,
												  returnValue);
			JITC_ADJUST(jitCoder, 1);
		}
		return;
	}

#ifdef IL_JIT_THREAD_IN_SIGNATURE
	/* Set the ILExecThread argument. */
	jitParams[0] = _ILJitCoderGetThread(jitCoder);

	_ILJitFillArguments(jitCoder, &(jitParams[1]), info);
#else
	_ILJitFillArguments(jitCoder, &(jitParams[0]), info);
#endif

	/* TODO: create call signature for vararg calls. */	
		
	if(info->tailCall == 1)
	{
	#ifdef IL_JIT_THREAD_IN_SIGNATURE
		returnValue = jit_insn_call(jitCoder->jitFunction, methodName,
									jitFunction, 0,
									jitParams, argCount + 1, JIT_CALL_TAIL);
	#else
		returnValue = jit_insn_call(jitCoder->jitFunction, methodName,
									jitFunction, 0,
									jitParams, argCount, JIT_CALL_TAIL);
	#endif
		if(returnItem->engineType != ILEngineType_Invalid)
		{
			jitCoder->jitStack[jitCoder->stackTop] =
					_ILJitValueConvertToStackType(jitCoder->jitFunction,
												  returnValue);
			JITC_ADJUST(jitCoder, 1);
		}
	}
	else
	{
#if !defined(IL_CONFIG_REDUCE_CODE) && !defined(_IL_JIT_ENABLE_DEBUG)
		ILPInvoke *pinv = ILPInvokeFind(methodInfo);
		if(pinv && ((ILJitMethodInfo*)(methodInfo->userData))->fnInfo.func)
		{
		#ifndef IL_CONFIG_REDUCE_CODE
			/* Emit the code to increase the call count of the method if profiling is enabled. */
			if(jitCoder->flags & IL_CODER_FLAG_METHOD_PROFILE)
			{
				_ILJitProfileIncreaseMethodCallCount(jitCoder, methodInfo);
			}
		#endif
			returnValue = _ILJitInlinePinvoke(jitCoder, methodInfo, jitParams);
		}
		else
#endif
		{
		#ifdef IL_JIT_THREAD_IN_SIGNATURE
			returnValue = jit_insn_call(jitCoder->jitFunction, methodName,
										jitFunction, 0,
										jitParams, argCount + 1, 0);
		#else
			returnValue = jit_insn_call(jitCoder->jitFunction, methodName,
										jitFunction, 0,
										jitParams, argCount, 0);
		#endif
		}
		if(returnItem && returnItem->engineType != ILEngineType_Invalid)
		{
				jitCoder->jitStack[jitCoder->stackTop] =
						_ILJitValueConvertToStackType(jitCoder->jitFunction,
													    returnValue);
				JITC_ADJUST(jitCoder, 1);
		}
	}
}

static void JITCoder_CallIndirect(ILCoder *coder, ILCoderMethodInfo *info,
								  ILEngineStackItem *returnItem)
{
	ILJITCoder *jitCoder = _ILCoderToILJITCoder(coder);
	ILJitValue ftnPtr = jitCoder->jitStack[jitCoder->stackTop - 1];

#if !defined(IL_CONFIG_REDUCE_CODE) && !defined(IL_WITHOUT_TOOLS)
	if (jitCoder->flags & IL_CODER_FLAG_STATS)
	{
		ILMutexLock(globalTraceMutex);
		fprintf(stdout,
			"CallIndirect\n");
		ILMutexUnlock(globalTraceMutex);
	}
#endif

#ifdef IL_JIT_FNPTR_ILMETHOD
	/* We need to convert the method pointer to the vtable pointer first. */
	ftnPtr = jit_insn_call_native(jitCoder->jitFunction,
								  "ILRuntimeMethodToVtablePointer",
								  ILRuntimeMethodToVtablePointer,
								  _ILJitSignature_ILRuntimeMethodToVtablePointer,
								  &ftnPtr, 1, 0);
#endif

	/* Pop the function pointer from the stack. */
	JITC_ADJUST(jitCoder, -1);
}

static void JITCoder_CallCtor(ILCoder *coder, ILCoderMethodInfo *info,
					   		  ILMethod *methodInfo)
{
	ILJITCoder *jitCoder = _ILCoderToILJITCoder(coder);
	ILClass *classInfo;
	ILType *type;
	ILType *synType;
	ILJitFunction jitFunction = ILJitFunctionFromILMethod(methodInfo);
	int argCount = info->numBaseArgs + info->numVarArgs;
	ILJitValue jitParams[argCount + 2];
	ILJitValue returnValue;
	ILInternalInfo fnInfo;
	int internalType = _IL_JIT_IMPL_DEFAULT;
	char *methodName = 0;
	
#if !defined(IL_CONFIG_REDUCE_CODE) && !defined(IL_WITHOUT_TOOLS)
	if (jitCoder->flags & IL_CODER_FLAG_STATS)
	{
		ILMutexLock(globalTraceMutex);
		fprintf(stdout,
			"CallCtor: %s.%s\n", 
			ILClass_Name(ILMethod_Owner(methodInfo)),
			ILMethod_Name(methodInfo));
		ILMutexUnlock(globalTraceMutex);
	}
#endif

	if(!jitFunction)
	{
		/* We need to layout the class first. */
		if(!_LayoutClass(ILExecThreadCurrent(), ILMethod_Owner(methodInfo)))
		{
			return;
		}
		jitFunction = ILJitFunctionFromILMethod(methodInfo);
	}

#if !defined(IL_CONFIG_REDUCE_CODE) && !defined(IL_WITHOUT_TOOLS)
#if defined(_IL_JIT_ENABLE_DEBUG)
	if (jitCoder->flags & IL_CODER_FLAG_STATS)
	{
		ILJitType  signature  = jit_function_get_signature(jitFunction);

		ILMutexLock(globalTraceMutex);
		fprintf(stdout,
			"CallInfos: StackTop: %i, ArgCount: %i, Signature argCount: %i\n",
			jitCoder->stackTop,
			argCount,
			jit_type_num_params(signature));
		ILMutexUnlock(globalTraceMutex);
	}
	methodName = _ILJitFunctionGetMethodName(jitFunction);
#endif
#endif
	classInfo = ILMethod_Owner(methodInfo);
	type = ILType_FromClass(classInfo);
	synType = ILClassGetSynType(classInfo);

	/* Output a call to the static constructor */
	_ILJitCallStaticConstructor(jitCoder, ILMethod_Owner(methodInfo), 1);

	/* Check if the function is implemented in the engine. */
	if((internalType = _ILJitFunctionIsInternal(jitCoder, methodInfo, &fnInfo, 1)))
	{
		ILJitValue thread = _ILJitCoderGetThread(jitCoder);

		/* Call the engine function directly with the supplied args. */
		if(internalType == _IL_JIT_IMPL_INTERNALALLOC)
		{
			/* This is an allocating constructor. */
			if(info->hasParamArray)
			{
				++argCount;
			}
			JITC_ADJUST(jitCoder, -argCount);
			returnValue = _ILJitCallInternal(jitCoder->jitFunction, thread,
											 methodInfo,
											 fnInfo.func, methodName,
											 &(jitCoder->jitStack[jitCoder->stackTop]),
											 argCount);
			jitCoder->jitStack[jitCoder->stackTop] =
						_ILJitValueConvertToStackType(jitCoder->jitFunction,
													  returnValue);
		}
		else
		{
			/* create a newobj and add it to the jitParams[0]. */
			_ILJitNewObj(jitCoder, ILMethod_Owner(methodInfo), &jitParams[0]); 
			_ILJitFillArguments(jitCoder, &(jitParams[1]), info);

			returnValue = _ILJitCallInternal(jitCoder->jitFunction, thread,
											 methodInfo,
											 fnInfo.func, methodName,
											 jitParams, argCount + 1);
			jitCoder->jitStack[jitCoder->stackTop] =
						_ILJitValueConvertToStackType(jitCoder->jitFunction,
													  jitParams[0]);
		}
		JITC_ADJUST(jitCoder, 1);
		return;
	}

#ifdef IL_JIT_THREAD_IN_SIGNATURE
	/* Output a call to the constructor */
	jitParams[0] = _ILJitCoderGetThread(jitCoder); // we add the current function thread as the first param
#endif
	if((synType && ILType_IsArray(synType)) || ILTypeIsStringClass(type))
	{
	#ifdef IL_JIT_THREAD_IN_SIGNATURE
		_ILJitFillArguments(jitCoder, &(jitParams[1]), info);
		// call the constructor with jitParams as input
		returnValue = jit_insn_call(jitCoder->jitFunction, 0, jitFunction, 0,
									jitParams, argCount + 1, 0);
	#else
		_ILJitFillArguments(jitCoder, &(jitParams[0]), info);
		// call the constructor with jitParams as input
		returnValue = jit_insn_call(jitCoder->jitFunction, 0, jitFunction, 0,
									jitParams, argCount, 0);
	#endif
		jitCoder->jitStack[jitCoder->stackTop] = returnValue;
	}
	else
	{
	#ifdef IL_JIT_THREAD_IN_SIGNATURE
		/* create a newobj and add it to the jitParams[1]. */
		_ILJitNewObj(jitCoder, ILMethod_Owner(methodInfo), &jitParams[1]);
		_ILJitFillArguments(jitCoder, &(jitParams[2]), info);

		// call the constructor with jitParams as input
		returnValue = jit_insn_call(jitCoder->jitFunction, methodName,
									jitFunction, 0,
									jitParams, argCount + 2, 0);

		jitCoder->jitStack[jitCoder->stackTop] = jitParams[1];
	#else
		/* create a newobj and add it to the jitParams[0]. */
		_ILJitNewObj(jitCoder, ILMethod_Owner(methodInfo), &jitParams[0]);
		_ILJitFillArguments(jitCoder, &(jitParams[1]), info);

		// call the constructor with jitParams as input
		returnValue = jit_insn_call(jitCoder->jitFunction, methodName,
									jitFunction, 0,
									jitParams, argCount + 1, 0);

		jitCoder->jitStack[jitCoder->stackTop] = jitParams[0];
	#endif
	}	
	JITC_ADJUST(jitCoder, 1);
}

static void JITCoder_CallVirtual(ILCoder *coder, ILCoderMethodInfo *info,
								 ILEngineStackItem *returnItem,
								 ILMethod *methodInfo)
{

	ILJITCoder *jitCoder = _ILCoderToILJITCoder(coder);
	int argCount = info->numBaseArgs + info->numVarArgs;
	ILJitFunction func = ILJitFunctionFromILMethod(methodInfo);
	ILJitType  signature;
	ILJitValue jitParams[argCount + 1];
	ILJitValue returnValue;
	ILJitValue jitFunction;

#if !defined(IL_CONFIG_REDUCE_CODE) && !defined(IL_WITHOUT_TOOLS)
	if (jitCoder->flags & IL_CODER_FLAG_STATS)
	{
		ILMutexLock(globalTraceMutex);
		fprintf(stdout,
			"CallVirtual: %s.%s at slot %i\n", 
			ILClass_Name(ILMethod_Owner(methodInfo)),
			ILMethod_Name(methodInfo),
			methodInfo->index);
		ILMutexUnlock(globalTraceMutex);
	}
#endif

	if(!func)
	{
		/* We might need to layout the class first. */
		if(!_LayoutClass(ILExecThreadCurrent(), ILMethod_Owner(methodInfo)))
		{
			return;
		}
		func = ILJitFunctionFromILMethod(methodInfo);
	}

	if(!func)
	{
		signature = _ILJitCreateMethodSignature(jitCoder, methodInfo, 0);
	}
	else
	{
		signature = jit_function_get_signature(func);
	}
#if !defined(IL_CONFIG_REDUCE_CODE) && !defined(IL_WITHOUT_TOOLS) && defined(_IL_JIT_ENABLE_DEBUG)
	if (jitCoder->flags & IL_CODER_FLAG_STATS)
	{
		ILMutexLock(globalTraceMutex);
		fprintf(stdout,
			"CallInfos: StackTop: %i, ArgCount: %i, Signature argCount: %i\n",
			jitCoder->stackTop,
			argCount,
			jit_type_num_params(signature));
		ILMutexUnlock(globalTraceMutex);
	}
#endif
#ifdef IL_JIT_THREAD_IN_SIGNATURE
	jitParams[0] = _ILJitCoderGetThread(jitCoder);
	_ILJitFillArguments(jitCoder, &(jitParams[1]), info);
	/* TODO: handle varargs here and create a call signature. */

	jitFunction = _ILJitGetVirtualFunction(jitCoder,
										   jitParams[1],
										   methodInfo->index);
#else
	_ILJitFillArguments(jitCoder, &(jitParams[0]), info);
	/* TODO: handle varargs here and create a call signature. */

	jitFunction = _ILJitGetVirtualFunction(jitCoder,
										   jitParams[0],
										   methodInfo->index);
#endif
	if(info->tailCall == 1)
	{
	#ifdef IL_JIT_THREAD_IN_SIGNATURE
		returnValue = jit_insn_call_indirect_vtable(jitCoder->jitFunction,
													jitFunction, signature,
													jitParams, argCount + 1,
													JIT_CALL_TAIL);
	#else
		returnValue = jit_insn_call_indirect_vtable(jitCoder->jitFunction,
													jitFunction, signature,
													jitParams, argCount,
													JIT_CALL_TAIL);
	#endif
	}
	else
	{
	#ifdef IL_JIT_THREAD_IN_SIGNATURE
		returnValue = jit_insn_call_indirect_vtable(jitCoder->jitFunction,
													jitFunction, signature,
													jitParams, argCount + 1,
													0);
	#else
		returnValue = jit_insn_call_indirect_vtable(jitCoder->jitFunction,
													jitFunction, signature,
													jitParams, argCount,
													0);
	#endif
	}
	if(returnItem->engineType != ILEngineType_Invalid)
	{
		jitCoder->jitStack[jitCoder->stackTop] =
			_ILJitValueConvertToStackType(jitCoder->jitFunction, returnValue);
		JITC_ADJUST(jitCoder, 1);
	}
}

static void JITCoder_CallInterface(ILCoder *coder, ILCoderMethodInfo *info,
								   ILEngineStackItem *returnItem,
								   ILMethod *methodInfo)
{
	ILJITCoder *jitCoder = _ILCoderToILJITCoder(coder);
	int argCount = info->numBaseArgs + info->numVarArgs;
	ILJitFunction func = ILJitFunctionFromILMethod(methodInfo);
	ILJitType  signature;
	ILJitValue jitParams[argCount + 1];
	ILJitValue returnValue;
	ILJitValue jitFunction;
	jit_label_t label = jit_label_undefined;

	if(!func)
	{
		signature = _ILJitCreateMethodSignature(jitCoder, methodInfo, 0);
	}
	else
	{
		signature = jit_function_get_signature(func);
	}
#if !defined(IL_CONFIG_REDUCE_CODE) && !defined(IL_WITHOUT_TOOLS)
	if (jitCoder->flags & IL_CODER_FLAG_STATS)
	{
		ILMutexLock(globalTraceMutex);
		fprintf(stdout,
			"CallInterface: %s.%s at slot %i\n", 
			ILClass_Name(ILMethod_Owner(methodInfo)),
			ILMethod_Name(methodInfo),
			methodInfo->index);
		ILMutexUnlock(globalTraceMutex);
	}
#if defined(_IL_JIT_ENABLE_DEBUG)
	if (jitCoder->flags & IL_CODER_FLAG_STATS)
	{
		ILMutexLock(globalTraceMutex);
		fprintf(stdout,
			"CallInfos: StackTop: %i, ArgCount: %i, Signature argCount: %i\n",
			jitCoder->stackTop,
			argCount,
			jit_type_num_params(signature));
		ILMutexUnlock(globalTraceMutex);
	}
#endif
#endif
#ifdef IL_JIT_THREAD_IN_SIGNATURE
	jitParams[0] = _ILJitCoderGetThread(jitCoder);
	_ILJitFillArguments(jitCoder, &(jitParams[1]), info);
	/* TODO: handle varargs here and create a call signature. */

	jitFunction = _ILJitGetInterfaceFunction(jitCoder,
											 jitParams[1],
											 methodInfo->member.owner,
											 methodInfo->index);
#else
	_ILJitFillArguments(jitCoder, &(jitParams[0]), info);
	/* TODO: handle varargs here and create a call signature. */

	jitFunction = _ILJitGetInterfaceFunction(jitCoder,
											 jitParams[0],
											 methodInfo->member.owner,
											 methodInfo->index);
#endif
	jit_insn_branch_if(jitCoder->jitFunction, jitFunction, &label);
	/* TODO: raise a MissingMethodException here. */

	jit_insn_label(jitCoder->jitFunction, &label);
	if(info->tailCall == 1)
	{
	#ifdef IL_JIT_THREAD_IN_SIGNATURE
		returnValue = jit_insn_call_indirect_vtable(jitCoder->jitFunction,
													jitFunction, signature,
													jitParams, argCount + 1,
													JIT_CALL_TAIL);
	#else
		returnValue = jit_insn_call_indirect_vtable(jitCoder->jitFunction,
													jitFunction, signature,
													jitParams, argCount,
													JIT_CALL_TAIL);
	#endif
	}
	else
	{
	#ifdef IL_JIT_THREAD_IN_SIGNATURE
		returnValue = jit_insn_call_indirect_vtable(jitCoder->jitFunction,
													jitFunction, signature,
													jitParams, argCount + 1,
													0);
	#else
		returnValue = jit_insn_call_indirect_vtable(jitCoder->jitFunction,
													jitFunction, signature,
													jitParams, argCount,
													0);
	#endif
	}
	if(returnItem->engineType != ILEngineType_Invalid)
	{
		jitCoder->jitStack[jitCoder->stackTop] = 
			_ILJitValueConvertToStackType(jitCoder->jitFunction, returnValue);
		JITC_ADJUST(jitCoder, 1);
	}
}

static int JITCoder_CallInlineable(ILCoder *coder, int inlineType,
								   ILMethod *methodInfo)
{
	ILJITCoder *jitCoder = _ILCoderToILJITCoder(coder);

	/* Determine what to do for the inlineable method type */
	switch(inlineType)
	{
		case IL_INLINEMETHOD_MONITOR_ENTER:
		{
			/* Enter the monitor on the top-most object */
			ILJitValue args[2];

			args[0] = _ILJitCoderGetThread(jitCoder);
			args[1] = jitCoder->jitStack[jitCoder->stackTop - 1]; 

			/* Pop the object from the stack. */
			JITC_ADJUST(jitCoder, -1);

			_ILJitBeginNativeCall(jitCoder->jitFunction, args[0]);
			jit_insn_call_native(jitCoder->jitFunction,
								 "_IL_Monitor_Enter",
								 _IL_Monitor_Enter,
								 _ILJitSignature_ILMonitorEnter,
								 args, 2, 0);
			_ILJitEndNativeCall(jitCoder->jitFunction, args[0]);

			return 1;
		}
		/* Not reached */

		case IL_INLINEMETHOD_MONITOR_EXIT:
		{
			/* Exit the monitor on the top-most object */
			ILJitValue args[2];

			args[0] = _ILJitCoderGetThread(jitCoder);
			args[1] = jitCoder->jitStack[jitCoder->stackTop - 1]; 

			/* Pop the object from the stack. */
			JITC_ADJUST(jitCoder, -1);

			_ILJitBeginNativeCall(jitCoder->jitFunction, args[0]);
			jit_insn_call_native(jitCoder->jitFunction,
								 "_IL_Monitor_Exit",
								 _IL_Monitor_Exit,
								 _ILJitSignature_ILMonitorExit,
								 args, 2, 0);
			_ILJitEndNativeCall(jitCoder->jitFunction, args[0]);

			return 1;
		}
		/* Not reached */
		case IL_INLINEMETHOD_TYPE_FROM_HANDLE:
		{
			/* Convert a RuntimeTypeHandle into a Type object */
			ILJitValue returnValue = jit_value_create(jitCoder->jitFunction,
													  _IL_JIT_TYPE_VPTR);
			ILJitValue temp;
			ILJitValue args[2];
			jit_label_t label = jit_label_undefined;
;
			jit_insn_store(jitCoder->jitFunction,
						   returnValue, 
						   jitCoder->jitStack[jitCoder->stackTop - 1]);

			jit_insn_branch_if_not(jitCoder->jitFunction, returnValue, &label);

			args[0] = _ILJitCoderGetThread(jitCoder);
			args[1] = returnValue;
			_ILJitBeginNativeCall(jitCoder->jitFunction, args[0]);
			temp = jit_insn_call_native(jitCoder->jitFunction,
										"_ILGetClrType",
										_ILGetClrType,
										_ILJitSignature_ILGetClrType,
										args, 2, 0);
			_ILJitEndNativeCall(jitCoder->jitFunction, args[0]);
			jit_insn_store(jitCoder->jitFunction,
						   returnValue, 
						   temp);
			jit_insn_label(jitCoder->jitFunction, &label);
			jitCoder->jitStack[jitCoder->stackTop - 1] = returnValue;
			return 1;
		}
		/* Not reached */

		/*
		 * Cases for Math class inlines.
		 */
		case IL_INLINEMETHOD_ABS_I4:
		{
			ILJitValue value = jitCoder->jitStack[jitCoder->stackTop - 1];

			value = _ILJitValueConvertImplicit(jitCoder->jitFunction,
											   value,
											   _IL_JIT_TYPE_INT32);
			value = jit_insn_abs(jitCoder->jitFunction, value);
			value = _ILJitValueConvertToStackType(jitCoder->jitFunction,
												  value);
			
			jitCoder->jitStack[jitCoder->stackTop - 1] = value;
			return 1;
		}
		/* Not reached */

		case IL_INLINEMETHOD_ABS_R4:
		{
			ILJitValue value = jitCoder->jitStack[jitCoder->stackTop - 1];

			value = _ILJitValueConvertImplicit(jitCoder->jitFunction,
											   value,
											   _IL_JIT_TYPE_SINGLE);
			value = jit_insn_abs(jitCoder->jitFunction, value);
			value = _ILJitValueConvertToStackType(jitCoder->jitFunction,
												  value);
			
			jitCoder->jitStack[jitCoder->stackTop - 1] = value;
			return 1;
		}
		/* Not reached */

		case IL_INLINEMETHOD_ABS_R8:
		{
			ILJitValue value = jitCoder->jitStack[jitCoder->stackTop - 1];

			value = _ILJitValueConvertImplicit(jitCoder->jitFunction,
											   value,
											   _IL_JIT_TYPE_DOUBLE);
			value = jit_insn_abs(jitCoder->jitFunction, value);
			value = _ILJitValueConvertToStackType(jitCoder->jitFunction,
												  value);
			
			jitCoder->jitStack[jitCoder->stackTop - 1] = value;
			return 1;
		}
		/* Not reached */

		case IL_INLINEMETHOD_ASIN:
		{
			ILJitValue value = jitCoder->jitStack[jitCoder->stackTop - 1];

			value = _ILJitValueConvertImplicit(jitCoder->jitFunction,
											   value,
											   _IL_JIT_TYPE_DOUBLE);
			value = jit_insn_asin(jitCoder->jitFunction, value);
			value = _ILJitValueConvertToStackType(jitCoder->jitFunction,
												  value);
			
			jitCoder->jitStack[jitCoder->stackTop - 1] = value;
			return 1;
		}
		/* Not reached */

		case IL_INLINEMETHOD_ATAN:
		{
			ILJitValue value = jitCoder->jitStack[jitCoder->stackTop - 1];

			value = _ILJitValueConvertImplicit(jitCoder->jitFunction,
											   value,
											   _IL_JIT_TYPE_DOUBLE);
			value = jit_insn_atan(jitCoder->jitFunction, value);
			value = _ILJitValueConvertToStackType(jitCoder->jitFunction,
												  value);
			
			jitCoder->jitStack[jitCoder->stackTop - 1] = value;
			return 1;
		}
		/* Not reached */

		case IL_INLINEMETHOD_ATAN2:
		{
			return 0;
		}
		/* Not reached */

		case IL_INLINEMETHOD_CEILING:
		{
			ILJitValue value = jitCoder->jitStack[jitCoder->stackTop - 1];

			value = _ILJitValueConvertImplicit(jitCoder->jitFunction,
											   value,
											   _IL_JIT_TYPE_DOUBLE);
			value = jit_insn_ceil(jitCoder->jitFunction, value);
			value = _ILJitValueConvertToStackType(jitCoder->jitFunction,
												  value);
			
			jitCoder->jitStack[jitCoder->stackTop - 1] = value;
			return 1;
		}
		/* Not reached */

		case IL_INLINEMETHOD_COS:
		{
			ILJitValue value = jitCoder->jitStack[jitCoder->stackTop - 1];

			value = _ILJitValueConvertImplicit(jitCoder->jitFunction,
											   value,
											   _IL_JIT_TYPE_DOUBLE);
			value = jit_insn_cos(jitCoder->jitFunction, value);
			value = _ILJitValueConvertToStackType(jitCoder->jitFunction,
												  value);
			
			jitCoder->jitStack[jitCoder->stackTop - 1] = value;
			return 1;
		}
		/* Not reached */

		case IL_INLINEMETHOD_COSH:
		{
			ILJitValue value = jitCoder->jitStack[jitCoder->stackTop - 1];

			value = _ILJitValueConvertImplicit(jitCoder->jitFunction,
											   value,
											   _IL_JIT_TYPE_DOUBLE);
			value = jit_insn_cosh(jitCoder->jitFunction, value);
			value = _ILJitValueConvertToStackType(jitCoder->jitFunction,
												  value);
			
			jitCoder->jitStack[jitCoder->stackTop - 1] = value;
			return 1;
		}
		/* Not reached */

		case IL_INLINEMETHOD_EXP:
		{
			ILJitValue value = jitCoder->jitStack[jitCoder->stackTop - 1];

			value = _ILJitValueConvertImplicit(jitCoder->jitFunction,
											   value,
											   _IL_JIT_TYPE_DOUBLE);
			value = jit_insn_exp(jitCoder->jitFunction, value);
			value = _ILJitValueConvertToStackType(jitCoder->jitFunction,
												  value);
			
			jitCoder->jitStack[jitCoder->stackTop - 1] = value;
			return 1;
		}
		/* Not reached */

		case IL_INLINEMETHOD_FLOOR:
		{
			ILJitValue value = jitCoder->jitStack[jitCoder->stackTop - 1];

			value = _ILJitValueConvertImplicit(jitCoder->jitFunction,
											   value,
											   _IL_JIT_TYPE_DOUBLE);
			value = jit_insn_floor(jitCoder->jitFunction, value);
			value = _ILJitValueConvertToStackType(jitCoder->jitFunction,
												  value);
			
			jitCoder->jitStack[jitCoder->stackTop - 1] = value;
			return 1;
		}
		/* Not reached */

		case IL_INLINEMETHOD_IEEEREMAINDER:
		{
			return 0;
		}
		/* Not reached */

		case IL_INLINEMETHOD_LOG:
		{
			ILJitValue value = jitCoder->jitStack[jitCoder->stackTop - 1];

			value = _ILJitValueConvertImplicit(jitCoder->jitFunction,
											   value,
											   _IL_JIT_TYPE_DOUBLE);
			value = jit_insn_log(jitCoder->jitFunction, value);
			value = _ILJitValueConvertToStackType(jitCoder->jitFunction,
												  value);
			
			jitCoder->jitStack[jitCoder->stackTop - 1] = value;
			return 1;
		}
		/* Not reached */

		case IL_INLINEMETHOD_LOG10:
		{
			ILJitValue value = jitCoder->jitStack[jitCoder->stackTop - 1];

			value = _ILJitValueConvertImplicit(jitCoder->jitFunction,
											   value,
											   _IL_JIT_TYPE_DOUBLE);
			value = jit_insn_log10(jitCoder->jitFunction, value);
			value = _ILJitValueConvertToStackType(jitCoder->jitFunction,
												  value);
			
			jitCoder->jitStack[jitCoder->stackTop - 1] = value;
			return 1;
		}
		/* Not reached */

		case IL_INLINEMETHOD_MAX_I4:
		{
			ILJitValue value1 = jitCoder->jitStack[jitCoder->stackTop - 2];
			ILJitValue value2 = jitCoder->jitStack[jitCoder->stackTop - 1];
			ILJitValue result;

			value1 = _ILJitValueConvertImplicit(jitCoder->jitFunction,
											   value1,
											   _IL_JIT_TYPE_INT32);
			value2 = _ILJitValueConvertImplicit(jitCoder->jitFunction,
											   value2,
											   _IL_JIT_TYPE_INT32);
			result = jit_insn_max(jitCoder->jitFunction, value1, value2);
			result = _ILJitValueConvertToStackType(jitCoder->jitFunction,
												   result);
			
			/* Pop one value from the stack. */
			JITC_ADJUST(jitCoder, -1);

			/* and replace the second one with the result. */
			jitCoder->jitStack[jitCoder->stackTop - 1] = result;
			return 1;
		}
		/* Not reached */

		case IL_INLINEMETHOD_MIN_I4:
		{
			ILJitValue value1 = jitCoder->jitStack[jitCoder->stackTop - 2];
			ILJitValue value2 = jitCoder->jitStack[jitCoder->stackTop - 1];
			ILJitValue result;

			value1 = _ILJitValueConvertImplicit(jitCoder->jitFunction,
											   value1,
											   _IL_JIT_TYPE_INT32);
			value2 = _ILJitValueConvertImplicit(jitCoder->jitFunction,
											   value2,
											   _IL_JIT_TYPE_INT32);
			result = jit_insn_min(jitCoder->jitFunction, value1, value2);
			result = _ILJitValueConvertToStackType(jitCoder->jitFunction,
												   result);
			
			/* Pop one value from the stack. */
			JITC_ADJUST(jitCoder, -1);

			/* and replace the second one with the result. */
			jitCoder->jitStack[jitCoder->stackTop - 1] = result;
			return 1;
		}
		/* Not reached */

		case IL_INLINEMETHOD_MAX_R4:
		{
			ILJitValue value1 = jitCoder->jitStack[jitCoder->stackTop - 2];
			ILJitValue value2 = jitCoder->jitStack[jitCoder->stackTop - 1];
			ILJitValue result;

			value1 = _ILJitValueConvertImplicit(jitCoder->jitFunction,
											   value1,
											   _IL_JIT_TYPE_SINGLE);
			value2 = _ILJitValueConvertImplicit(jitCoder->jitFunction,
											   value2,
											   _IL_JIT_TYPE_SINGLE);
			result = jit_insn_max(jitCoder->jitFunction, value1, value2);
			result = _ILJitValueConvertToStackType(jitCoder->jitFunction,
												   result);
			
			/* Pop one value from the stack. */
			JITC_ADJUST(jitCoder, -1);

			/* and replace the second one with the result. */
			jitCoder->jitStack[jitCoder->stackTop - 1] = result;
			return 1;
		}
		/* Not reached */

		case IL_INLINEMETHOD_MIN_R4:
		{
			ILJitValue value1 = jitCoder->jitStack[jitCoder->stackTop - 2];
			ILJitValue value2 = jitCoder->jitStack[jitCoder->stackTop - 1];
			ILJitValue result;

			value1 = _ILJitValueConvertImplicit(jitCoder->jitFunction,
											   value1,
											   _IL_JIT_TYPE_SINGLE);
			value2 = _ILJitValueConvertImplicit(jitCoder->jitFunction,
											   value2,
											   _IL_JIT_TYPE_SINGLE);
			result = jit_insn_min(jitCoder->jitFunction, value1, value2);
			result = _ILJitValueConvertToStackType(jitCoder->jitFunction,
												   result);
			
			/* Pop one value from the stack. */
			JITC_ADJUST(jitCoder, -1);

			/* and replace the second one with the result. */
			jitCoder->jitStack[jitCoder->stackTop - 1] = result;
			return 1;
		}
		/* Not reached */

		case IL_INLINEMETHOD_MAX_R8:
		{
			ILJitValue value1 = jitCoder->jitStack[jitCoder->stackTop - 2];
			ILJitValue value2 = jitCoder->jitStack[jitCoder->stackTop - 1];
			ILJitValue result;

			value1 = _ILJitValueConvertImplicit(jitCoder->jitFunction,
											   value1,
											   _IL_JIT_TYPE_DOUBLE);
			value2 = _ILJitValueConvertImplicit(jitCoder->jitFunction,
											   value2,
											   _IL_JIT_TYPE_DOUBLE);
			result = jit_insn_max(jitCoder->jitFunction, value1, value2);
			result = _ILJitValueConvertToStackType(jitCoder->jitFunction,
												   result);
			
			/* Pop one value from the stack. */
			JITC_ADJUST(jitCoder, -1);

			/* and replace the second one with the result. */
			jitCoder->jitStack[jitCoder->stackTop - 1] = result;
			return 1;
		}
		/* Not reached */

		case IL_INLINEMETHOD_MIN_R8:
		{
			ILJitValue value1 = jitCoder->jitStack[jitCoder->stackTop - 2];
			ILJitValue value2 = jitCoder->jitStack[jitCoder->stackTop - 1];
			ILJitValue result;

			value1 = _ILJitValueConvertImplicit(jitCoder->jitFunction,
											   value1,
											   _IL_JIT_TYPE_DOUBLE);
			value2 = _ILJitValueConvertImplicit(jitCoder->jitFunction,
											   value2,
											   _IL_JIT_TYPE_DOUBLE);
			result = jit_insn_min(jitCoder->jitFunction, value1, value2);
			result = _ILJitValueConvertToStackType(jitCoder->jitFunction,
												   result);
			
			/* Pop one value from the stack. */
			JITC_ADJUST(jitCoder, -1);

			/* and replace the second one with the result. */
			jitCoder->jitStack[jitCoder->stackTop - 1] = result;
			return 1;
		}
		/* Not reached */

		case IL_INLINEMETHOD_POW:
		case IL_INLINEMETHOD_ROUND:
		{
			return 0;
		}
		/* Not reached */

		case IL_INLINEMETHOD_SIN:
		{
			ILJitValue value = jitCoder->jitStack[jitCoder->stackTop - 1];

			value = _ILJitValueConvertImplicit(jitCoder->jitFunction,
											   value,
											   _IL_JIT_TYPE_DOUBLE);
			value = jit_insn_sin(jitCoder->jitFunction, value);
			value = _ILJitValueConvertToStackType(jitCoder->jitFunction,
												  value);
			
			jitCoder->jitStack[jitCoder->stackTop - 1] = value;
			return 1;
		}
		/* Not reached */

		case IL_INLINEMETHOD_SIGN_I4:
		case IL_INLINEMETHOD_SIGN_R4:
		case IL_INLINEMETHOD_SIGN_R8:
		{
			return 0;
		}
		/* Not reached */

		case IL_INLINEMETHOD_SINH:
		{
			ILJitValue value = jitCoder->jitStack[jitCoder->stackTop - 1];

			value = _ILJitValueConvertImplicit(jitCoder->jitFunction,
											   value,
											   _IL_JIT_TYPE_DOUBLE);
			value = jit_insn_sinh(jitCoder->jitFunction, value);
			value = _ILJitValueConvertToStackType(jitCoder->jitFunction,
												  value);
			
			jitCoder->jitStack[jitCoder->stackTop - 1] = value;
			return 1;
		}
		/* Not reached */

		case IL_INLINEMETHOD_SQRT:
		{
			ILJitValue value = jitCoder->jitStack[jitCoder->stackTop - 1];

			value = _ILJitValueConvertImplicit(jitCoder->jitFunction,
											   value,
											   _IL_JIT_TYPE_DOUBLE);
			value = jit_insn_sqrt(jitCoder->jitFunction, value);
			value = _ILJitValueConvertToStackType(jitCoder->jitFunction,
												  value);
			
			jitCoder->jitStack[jitCoder->stackTop - 1] = value;
			return 1;
		}
		/* Not reached */

		case IL_INLINEMETHOD_TAN:
		{
			ILJitValue value = jitCoder->jitStack[jitCoder->stackTop - 1];

			value = _ILJitValueConvertImplicit(jitCoder->jitFunction,
											   value,
											   _IL_JIT_TYPE_DOUBLE);
			value = jit_insn_tan(jitCoder->jitFunction, value);
			value = _ILJitValueConvertToStackType(jitCoder->jitFunction,
												  value);
			
			jitCoder->jitStack[jitCoder->stackTop - 1] = value;
			return 1;
		}
		/* Not reached */

		case IL_INLINEMETHOD_TANH:
		{
			ILJitValue value = jitCoder->jitStack[jitCoder->stackTop - 1];

			value = _ILJitValueConvertImplicit(jitCoder->jitFunction,
											   value,
											   _IL_JIT_TYPE_DOUBLE);
			value = jit_insn_tanh(jitCoder->jitFunction, value);
			value = _ILJitValueConvertToStackType(jitCoder->jitFunction,
												  value);
			
			jitCoder->jitStack[jitCoder->stackTop - 1] = value;
			return 1;
		}
		/* Not reached */
	}
	/* If we get here, then we don't know how to inline the method */
	return 0;
}

static void JITCoder_JumpMethod(ILCoder *coder, ILMethod *methodInfo)
{
}

static void JITCoder_ReturnInsn(ILCoder *coder, ILEngineType engineType,
							    ILType *returnType)
{
	ILJITCoder *jitCoder = _ILCoderToILJITCoder(coder);

#if !defined(IL_CONFIG_REDUCE_CODE) && !defined(IL_WITHOUT_TOOLS)
	if (jitCoder->flags & IL_CODER_FLAG_STATS)
	{
		ILMutexLock(globalTraceMutex);
		fprintf(stdout,
			"Return\n");
		ILMutexUnlock(globalTraceMutex);
	}
#endif

#ifdef IL_DEBUGGER
	/* Insert potential breakpoint with method in data2 */
	if(jitCoder->markBreakpoints)
	{
		jit_insn_mark_breakpoint(jitCoder->jitFunction,
								 JIT_DEBUGGER_DATA1_METHOD_LEAVE,
								 (jit_nint) jitCoder->currentMethod);
	}
#endif

	if(engineType == ILEngineType_Invalid)
	{
	       jit_insn_return(jitCoder->jitFunction, 0);
	}
	else
	{
		jit_insn_return(jitCoder->jitFunction,
					jitCoder->jitStack[jitCoder->stackTop - 1]);
		JITC_ADJUST(jitCoder, -1);
	}
}

static void JITCoder_LoadFuncAddr(ILCoder *coder, ILMethod *methodInfo)
{
	ILJITCoder *jitCoder = _ILCoderToILJITCoder(coder);
	ILJitFunction jitFunction = ILJitFunctionFromILMethod(methodInfo);
#ifndef IL_JIT_FNPTR_ILMETHOD
	void *function; /* vtable pointer for the function. */
	ILJitValue functionPtr; /* jit value containing the vtable pointer. */
#else
	ILJitValue functionPtr = jit_value_create_nint_constant(jitCoder->jitFunction,
															_IL_JIT_TYPE_VPTR,
															(jit_nint)methodInfo);
#endif

#if !defined(IL_CONFIG_REDUCE_CODE) && !defined(IL_WITHOUT_TOOLS)
	if (jitCoder->flags & IL_CODER_FLAG_STATS)
	{
		ILMutexLock(globalTraceMutex);
		fprintf(stdout,
			"LoadFuncAddr: %s.%s\n",
			ILClass_Name(ILMethod_Owner(methodInfo)),
			ILMethod_Name(methodInfo));
		ILMutexUnlock(globalTraceMutex);
	}
#endif
	if(!jitFunction)
	{
		/* We need to layout the class first. */
		if(!_LayoutClass(ILExecThreadCurrent(), ILMethod_Owner(methodInfo)))
		{
			return;
		}
		jitFunction = ILJitFunctionFromILMethod(methodInfo);
	}
#ifndef IL_JIT_FNPTR_ILMETHOD
	/* Get the vtable pointer for the function. */
	function = jit_function_to_vtable_pointer(jitFunction);
	functionPtr = jit_value_create_nint_constant(jitCoder->jitFunction,
												 _IL_JIT_TYPE_VPTR,
												(jit_nint)function);
#endif
	/* Push the function pointer on the stack. */
	jitCoder->jitStack[jitCoder->stackTop] = functionPtr;
	JITC_ADJUST(jitCoder, 1);
}

static void JITCoder_LoadVirtualAddr(ILCoder *coder, ILMethod *methodInfo)
{
	ILJITCoder *jitCoder = _ILCoderToILJITCoder(coder);
	ILJitValue object = jitCoder->jitStack[jitCoder->stackTop - 1];
	ILJitValue jitFunction;
#ifdef IL_JIT_FNPTR_ILMETHOD
	ILJitValue classPrivate;
	ILJitValue vtable;
	ILJitValue vtableIndex;
#endif

#if !defined(IL_CONFIG_REDUCE_CODE) && !defined(IL_WITHOUT_TOOLS)
	if (jitCoder->flags & IL_CODER_FLAG_STATS)
	{
		ILMutexLock(globalTraceMutex);
		fprintf(stdout,
			"LoadVirtualAddr: %s.%s at slot %i\n", 
			ILClass_Name(ILMethod_Owner(methodInfo)),
			ILMethod_Name(methodInfo),
			methodInfo->index);
		ILMutexUnlock(globalTraceMutex);
	}
#endif

#ifdef IL_JIT_FNPTR_ILMETHOD
	_ILJitCheckNull(jitCoder, object);
	classPrivate = _ILJitGetObjectClassPrivate(jitCoder->jitFunction, object);
	vtable = jit_insn_load_relative(jitCoder->jitFunction, classPrivate, 
									offsetof(ILClassPrivate, vtable), 
									_IL_JIT_TYPE_VPTR);
	vtableIndex = jit_value_create_nint_constant(jitCoder->jitFunction,
												 _IL_JIT_TYPE_INT32,
												 (jit_nint)methodInfo->index);
	jitFunction = jit_insn_load_elem(jitCoder->jitFunction,
									 vtable, vtableIndex, _IL_JIT_TYPE_VPTR);
#else
	jitFunction = _ILJitGetVirtualFunction(jitCoder,
										   object,
										   methodInfo->index);
#endif
	/* Push the function pointer on the stack. */
	jitCoder->jitStack[jitCoder->stackTop - 1] = jitFunction;
}

static void JITCoder_LoadInterfaceAddr(ILCoder *coder, ILMethod *methodInfo)
{
	ILJITCoder *jitCoder = _ILCoderToILJITCoder(coder);
	ILJitValue object = jitCoder->jitStack[jitCoder->stackTop - 1];
	ILJitValue jitFunction;
#ifdef IL_JIT_FNPTR_ILMETHOD
	ILJitValue args[3];
#endif

#if !defined(IL_CONFIG_REDUCE_CODE) && !defined(IL_WITHOUT_TOOLS)
	if (jitCoder->flags & IL_CODER_FLAG_STATS)
	{
		ILMutexLock(globalTraceMutex);
		fprintf(stdout,
			"LoadInterfaceAddr: %s.%s at slot %i\n", 
			ILClass_Name(ILMethod_Owner(methodInfo)),
			ILMethod_Name(methodInfo),
			methodInfo->index);
		ILMutexUnlock(globalTraceMutex);
	}
#endif

#ifdef IL_JIT_FNPTR_ILMETHOD
	_ILJitCheckNull(jitCoder, object);
	args[0] = _ILJitGetObjectClassPrivate(jitCoder->jitFunction, object);
	args[1] = jit_value_create_nint_constant(jitCoder->jitFunction,
											 _IL_JIT_TYPE_VPTR,
											 (jit_nint)methodInfo->member.owner);
	args[2] = jit_value_create_nint_constant(jitCoder->jitFunction,
											 _IL_JIT_TYPE_UINT32,
											 (jit_nint)methodInfo->index);

	jitFunction = jit_insn_call_native(jitCoder->jitFunction,
									   "_ILRuntimeLookupInterfaceILMethod",
									   _ILRuntimeLookupInterfaceILMethod,
									   _ILJitSignature_ILRuntimeLookupInterfaceMethod,
									   args, 3, 0);

#else
	jitFunction = _ILJitGetInterfaceFunction(jitCoder,
											 object,
											 methodInfo->member.owner,
											 methodInfo->index);
#endif
	/* Push the function pointer on the stack. */
	jitCoder->jitStack[jitCoder->stackTop - 1] = jitFunction;
}

#endif	/* IL_JITC_CODE */
