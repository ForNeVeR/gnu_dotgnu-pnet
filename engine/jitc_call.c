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
static ILJitValue _ILJitGetInterfaceFunction(ILJitFunction func,
											 ILJitValue object,
											 ILClass *interface,
											 int index)
{
	ILJitValue classPrivate;
	ILJitValue interfaceClass;
	ILJitValue methodIndex;
	ILJitValue args[3];

	jit_insn_check_null(func, object);
	classPrivate = _ILJitGetObjectClassPrivate(func, object);
	interfaceClass = jit_value_create_nint_constant(func,
													_IL_JIT_TYPE_VPTR,
													(jit_nint)interface);
	methodIndex = jit_value_create_nint_constant(func,
												 _IL_JIT_TYPE_UINT32,
												 (jit_nint)index);
	args[0] = classPrivate;
	args[1] = interfaceClass;
	args[2] = methodIndex;
	return jit_insn_call_native(func,
								"_ILRuntimeLookupInterfaceMethod",
								_ILRuntimeLookupInterfaceMethod,
								_ILJitSignature_ILRuntimeLookupInterfaceMethod,
								args, 3, JIT_CALL_NOTHROW);
}

/*
 * Get the vtable pointer for a virtual function from an object.
 */
static ILJitValue _ILJitGetVirtualFunction(ILJitFunction func,
										   ILJitValue object,
										   int index)
{
	ILJitValue classPrivate;
	ILJitValue vtable;
	ILJitValue vtableIndex;

	jit_insn_check_null(func, object);
	classPrivate = _ILJitGetObjectClassPrivate(func, object);
	vtable = jit_insn_load_relative(func, classPrivate, 
									offsetof(ILClassPrivate, jitVtable), 
									_IL_JIT_TYPE_VPTR);
	vtableIndex = jit_value_create_nint_constant(func,
												 _IL_JIT_TYPE_INT32,
												 (jit_nint)index);
	return jit_insn_load_elem(func, vtable, vtableIndex, _IL_JIT_TYPE_VPTR);
}

/*
 * Create a new object and push it on the stack.
 */
static void _ILJitNewObj(ILJITCoder *coder, ILClass *info, ILJitValue *newArg)
{
	ILUInt32 objSize = _ILJitGetSizeOfClass(info);
	ILJitValue newObj;
	ILJitValue args[3];
	jit_label_t label1 = jit_label_undefined;
	
	args[0] = jit_value_get_param(coder->jitFunction, 0);
	args[1] = jit_value_create_nint_constant(coder->jitFunction,
												jit_type_void_ptr, (jit_nint)info);
	args[2] = jit_value_create_nint_constant(coder->jitFunction,
												jit_type_int, objSize);
	newObj = jit_insn_call_native(coder->jitFunction, "_ILEngineAlloc",
								  _ILEngineAlloc,
								  _ILJitSignature_ILEngineAlloc, args, 3,
								  JIT_CALL_NOTHROW);
	jit_insn_branch_if(coder->jitFunction, newObj, &label1);
	_ILJitThrowCurrentException(coder);
	jit_insn_label(coder->jitFunction, &label1);
	*newArg = newObj;
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
	ILJitValue thread = jit_value_get_param(jitCoder->jitFunction, 0);
	ILJitValue arrayLength = jit_value_create_nint_constant(jitCoder->jitFunction,
															_IL_JIT_TYPE_UINT32,
															(jit_nint)numArgs);
	ILJitValue array;
	ILJitValue arrayBase;
	ILJitValue boxObject;
	ILJitValue boxValue;
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

			args[0] = thread;
			args[1] = jit_value_create_nint_constant(jitCoder->jitFunction,
									_IL_JIT_TYPE_VPTR, (jit_nint)info);
			args[2] = jit_value_create_nint_constant(jitCoder->jitFunction,
												_IL_JIT_TYPE_UINT32, typeSize);

			boxObject = jit_insn_call_native(jitCoder->jitFunction,
											 "_ILEngineAlloc",
								  			 _ILEngineAlloc,
											 _ILJitSignature_ILEngineAlloc,
					 			  			 args, 3, JIT_CALL_NOTHROW);
			jit_insn_branch_if_not(jitCoder->jitFunction, boxObject,
								   &labelException);
			if(boxValue)
			{
				jit_insn_store_relative(jitCoder->jitFunction, boxObject, 0,
										boxValue);
			}
			else
			{
				jit_insn_memcpy(jitCoder->jitFunction, boxObject, ptr, args[2]);
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

static int _ILJitIsStringClass(ILClass *info)
{
	ILImage *systemImage;
	if(!strcmp(info->className->name, "String") &&
	    info->className->namespace &&
	    !strcmp(info->className->namespace, "System"))
	{
		/* Check that it is within the system image, to prevent
		   applications from fooling us into believeing that their
		   own class is the system's string class */
		
		info = ILClassResolve(info);
		systemImage = info->programItem.image->context->systemImage;
		if(!systemImage || systemImage == info->programItem.image)
		{
			return 1;
		}
	}	
	return 0;
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
}

static void JITCoder_CallMethod(ILCoder *coder, ILCoderMethodInfo *info,
								ILEngineStackItem *returnItem,
								ILMethod *methodInfo)
{
	ILJITCoder *jitCoder = _ILCoderToILJITCoder(coder);
	ILJitFunction jitFunction = ILJitFunctionFromILMethod(methodInfo);
	int argCount = info->numBaseArgs + info->numVarArgs;
	ILJitValue jitParams[argCount + 1];
	ILJitValue returnValue;
	char *methodName = 0;

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

	/* Set the ILExecThread argument. */
	jitParams[0] = jit_value_get_param(jitCoder->jitFunction, 0);

	_ILJitFillArguments(jitCoder, &(jitParams[1]), info);
	/* TODO: create call signature for vararg calls. */	
		
#if !defined(IL_CONFIG_REDUCE_CODE) && !defined(IL_WITHOUT_TOOLS) && defined(_IL_JIT_ENABLE_DEBUG)
	methodName = _ILJitFunctionGetMethodName(jitFunction);
#endif
	if(info->tailCall == 1)
	{
		returnValue = jit_insn_call(jitCoder->jitFunction, methodName,
									jitFunction, 0,
									jitParams, argCount + 1, JIT_CALL_TAIL);
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
		returnValue = jit_insn_call(jitCoder->jitFunction, methodName,
									jitFunction, 0,
									jitParams, argCount + 1, 0);
		if(returnItem && returnItem->engineType != ILEngineType_Invalid)
		{
			jitCoder->jitStack[jitCoder->stackTop] =
					_ILJitValueConvertToStackType(jitCoder->jitFunction,
												  returnValue);
			JITC_ADJUST(jitCoder, 1);
		}
	}
	/* Restore the current method in the thread on return. */
	_ILJitSetMethodInThread(jitCoder->jitFunction, 
							jit_value_get_param(jitCoder->jitFunction, 0),
							jitCoder->currentMethod);
}

static void JITCoder_CallIndirect(ILCoder *coder, ILCoderMethodInfo *info,
								  ILEngineStackItem *returnItem)
{
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

	/* Output a call to the constructor */
	jitParams[0] = jit_value_get_param(jitCoder->jitFunction, 0); // we add the current function thread as the first param
		
	if((synType && ILType_IsArray(synType)) || ILTypeIsStringClass(type))
	{
		_ILJitFillArguments(jitCoder, &(jitParams[1]), info);
		// call the constructor with jitParams as input
		returnValue = jit_insn_call(jitCoder->jitFunction, 0, jitFunction, 0,
									jitParams, argCount + 1, 0);
		jitCoder->jitStack[jitCoder->stackTop] = returnValue;
	}
	else
	{
		_ILJitNewObj(jitCoder, ILMethod_Owner(methodInfo), &jitParams[1]); // create a newobj and add it to the jitParams[1]
		_ILJitFillArguments(jitCoder, &(jitParams[2]), info);

		// call the constructor with jitParams as input
		returnValue = jit_insn_call(jitCoder->jitFunction, methodName,
									jitFunction, 0,
									jitParams, argCount + 2, 0);
		jitCoder->jitStack[jitCoder->stackTop] = jitParams[1];
	}	
	JITC_ADJUST(jitCoder, 1);

	/* Restore the current method in the thread on return. */
	_ILJitSetMethodInThread(jitCoder->jitFunction, 
							jit_value_get_param(jitCoder->jitFunction, 0),
							jitCoder->currentMethod);
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
		signature = _ILJitCreateMethodSignature(jitCoder, methodInfo);
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
	jitParams[0] = jit_value_get_param(jitCoder->jitFunction, 0);
	_ILJitFillArguments(jitCoder, &(jitParams[1]), info);
	/* TODO: handle varargs here and create a call signature. */

	jitFunction = _ILJitGetVirtualFunction(jitCoder->jitFunction,
										   jitParams[1],
										   methodInfo->index);
	if(info->tailCall == 1)
	{
		returnValue = jit_insn_call_indirect_vtable(jitCoder->jitFunction,
													jitFunction, signature,
													jitParams, argCount + 1,
													JIT_CALL_TAIL);
	}
	else
	{
		returnValue = jit_insn_call_indirect_vtable(jitCoder->jitFunction,
													jitFunction, signature,
													jitParams, argCount + 1,
													0);
	}
	if(returnItem->engineType != ILEngineType_Invalid)
	{
		jitCoder->jitStack[jitCoder->stackTop] =
			_ILJitValueConvertToStackType(jitCoder->jitFunction, returnValue);
		JITC_ADJUST(jitCoder, 1);
	}
	/* Restore the current method in the thread on return. */
	_ILJitSetMethodInThread(jitCoder->jitFunction, 
							jit_value_get_param(jitCoder->jitFunction, 0),
							jitCoder->currentMethod);
}

static void JITCoder_CallInterface(ILCoder *coder, ILCoderMethodInfo *info,
								   ILEngineStackItem *returnItem,
								   ILMethod *methodInfo)
{
	ILJITCoder *jitCoder = _ILCoderToILJITCoder(coder);
	int argCount = info->numBaseArgs + info->numVarArgs;
	ILJitFunction func = methodInfo->userData;
	ILJitType  signature;
	ILJitValue jitParams[argCount + 1];
	ILJitValue returnValue;
	ILJitValue jitFunction;
	jit_label_t label = jit_label_undefined;

	if(!func)
	{
		signature = _ILJitCreateMethodSignature(jitCoder, methodInfo);
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
	jitParams[0] = jit_value_get_param(jitCoder->jitFunction, 0);
	_ILJitFillArguments(jitCoder, &(jitParams[1]), info);
	/* TODO: handle varargs here and create a call signature. */

	jitFunction = _ILJitGetInterfaceFunction(jitCoder->jitFunction,
											 jitParams[1],
											 methodInfo->member.owner,
											 methodInfo->index);
	jit_insn_branch_if(jitCoder->jitFunction, jitFunction, &label);
	/* TODO: raise a MissingMethodException here. */

	jit_insn_label(jitCoder->jitFunction, &label);
	if(info->tailCall == 1)
	{
		returnValue = jit_insn_call_indirect_vtable(jitCoder->jitFunction,
													jitFunction, signature,
													jitParams, argCount + 1,
													JIT_CALL_TAIL);
	}
	else
	{
		returnValue = jit_insn_call_indirect_vtable(jitCoder->jitFunction,
													jitFunction, signature,
													jitParams, argCount + 1,
													0);
	}
	if(returnItem->engineType != ILEngineType_Invalid)
	{
		jitCoder->jitStack[jitCoder->stackTop] = 
			_ILJitValueConvertToStackType(jitCoder->jitFunction, returnValue);
		JITC_ADJUST(jitCoder, 1);
	}
	/* Restore the current method in the thread on return. */
	_ILJitSetMethodInThread(jitCoder->jitFunction, 
							jit_value_get_param(jitCoder->jitFunction, 0),
							jitCoder->currentMethod);
}

static int JITCoder_CallInlineable(ILCoder *coder, int inlineType,
								   ILMethod *methodInfo)
{
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
	void *function; /* vtable pointer for the function. */
	ILJitValue functionPtr; /* jt value containing the vtable pointer. */

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
	/* Get the vtable pointer for the function. */
	function = jit_function_to_vtable_pointer(jitFunction);
	functionPtr = jit_value_create_nint_constant(jitCoder->jitFunction,
												 _IL_JIT_TYPE_VPTR,
												(jit_nint)function);

	/* Push the function pointer on the stack. */
	jitCoder->jitStack[jitCoder->stackTop] = functionPtr;
	JITC_ADJUST(jitCoder, 1);
}

static void JITCoder_LoadVirtualAddr(ILCoder *coder, ILMethod *methodInfo)
{
	ILJITCoder *jitCoder = _ILCoderToILJITCoder(coder);
	ILJitValue object = jitCoder->jitStack[jitCoder->stackTop - 1];
	ILJitValue jitFunction;

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

	jitFunction = _ILJitGetVirtualFunction(jitCoder->jitFunction,
										   object,
										   methodInfo->index);
	/* Push the function pointer on the stack. */
	jitCoder->jitStack[jitCoder->stackTop - 1] = jitFunction;
}

static void JITCoder_LoadInterfaceAddr(ILCoder *coder, ILMethod *methodInfo)
{
	ILJITCoder *jitCoder = _ILCoderToILJITCoder(coder);
	ILJitValue object = jitCoder->jitStack[jitCoder->stackTop - 1];
	ILJitValue jitFunction;

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

	jitFunction = _ILJitGetInterfaceFunction(jitCoder->jitFunction,
											 object,
											 methodInfo->member.owner,
											 methodInfo->index);
	/* Push the function pointer on the stack. */
	jitCoder->jitStack[jitCoder->stackTop - 1] = jitFunction;
}

#endif	/* IL_JITC_CODE */
