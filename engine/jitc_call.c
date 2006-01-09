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
	
	JITC_ADJUST(coder, -argCount);
	for(current = 0; current < argCount; current++)
	{
		args[current] = coder->jitStack[coder->stackTop + current];
	}
}

/*
 * Create a new object and push it on the stack.
 */
static void _ILJitNewObj(ILJITCoder *coder, ILClass *info, ILJitValue *newArg)
{
	ILJitValue newObj;
	ILJitValue args[3];
	jit_label_t label1 = jit_label_undefined;
	
	args[0] = jit_value_get_param(coder->jitFunction, 0);
	args[1] = jit_value_create_nint_constant(coder->jitFunction,
												jit_type_void_ptr, (jit_nint)info);
	args[2] = jit_value_create_nint_constant(coder->jitFunction,
												jit_type_int, 0);
	newObj = jit_insn_call_native(coder->jitFunction, 0, _ILEngineAlloc,
									_ILJitSignature_ILEngineAlloc, args, 3, 0);
	jit_insn_branch_if(coder->jitFunction, newObj, &label1);
	_ILJitThrowCurrentException(coder);
	jit_insn_label(coder->jitFunction, &label1);
	*newArg = newObj;
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
}

static void JITCoder_ValueCtorArgs(ILCoder *coder, ILClass *classInfo,
								   ILEngineStackItem *args, ILUInt32 numArgs)
{
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

	/* Set the ILExecThread argument. */
	jitParams[0] = jit_value_get_param(jitCoder->jitFunction, 0);

	_ILJitFillArguments(jitCoder, &(jitParams[1]), info);
	/* TODO: create call signature for vararg calls. */	
		
	if(info->tailCall == 1)
	{
		returnValue = jit_insn_call(jitCoder->jitFunction, 0, jitFunction, 0,
									jitParams, argCount + 1, JIT_CALL_TAIL);
	}
	else
	{		
		returnValue = jit_insn_call(jitCoder->jitFunction, 0, jitFunction, 0,
									jitParams, argCount + 1, 0);
		if(returnItem->engineType != ILEngineType_Invalid)
		{
			jitCoder->jitStack[jitCoder->stackTop] = returnValue;
			JITC_ADJUST(jitCoder, 1);
		}
	}
}

static void JITCoder_CallIndirect(ILCoder *coder, ILCoderMethodInfo *info,
								  ILEngineStackItem *returnItem)
{
}

static void JITCoder_CallCtor(ILCoder *coder, ILCoderMethodInfo *info,
					   		  ILMethod *methodInfo)
{
	ILJITCoder *jitCoder = _ILCoderToILJITCoder(coder);
	ILJitFunction jitFunction = ILJitFunctionFromILMethod(methodInfo);
	int argCount = info->numBaseArgs + info->numVarArgs;
	ILJitValue jitParams[argCount + 2];
	ILJitValue returnValue;
	
	/* Output a call to the static constructor */
	_ILJitCallStaticConstructor(jitCoder, ILMethod_Owner(methodInfo), 1);

	/* Output a call to the constructor */
	jitParams[0] = jit_value_get_param(jitCoder->jitFunction, 0); // we add the current function thread as the first param
		
	if(_ILJitIsStringClass(ILMethod_Owner(methodInfo)))
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
		returnValue = jit_insn_call(jitCoder->jitFunction, 0, jitFunction, 0,
									jitParams, argCount + 2, 0);
		jitCoder->jitStack[jitCoder->stackTop] = jitParams[1];
	}	
	JITC_ADJUST(jitCoder, 1);
}

static void JITCoder_CallVirtual(ILCoder *coder, ILCoderMethodInfo *info,
								 ILEngineStackItem *returnItem,
								 ILMethod *methodInfo)
{

	ILJITCoder *jitCoder = _ILCoderToILJITCoder(coder);
	int argCount = info->numBaseArgs + info->numVarArgs;
	ILJitFunction func = methodInfo->userData;
	ILJitType  signature;
	ILJitValue jitParams[argCount + 2];
	ILJitValue returnValue;
	ILJitValue classPrivate;
	ILJitValue vtable;
	ILJitValue vtableIndex;
	ILJitValue jitFunction;

	if(!func)
	{
		signature = _ILJitCreateMethodSignature(jitCoder, methodInfo);
	}
	else
	{
		signature = jit_function_get_signature(func);
	}
	jitParams[0] = jit_value_get_param(jitCoder->jitFunction, 0);
	_ILJitFillArguments(jitCoder, &(jitParams[1]), info);
	/* TODO: handle varargs here and create a call signature. */

	jit_insn_check_null(jitCoder->jitFunction, jitParams[1]);
	classPrivate = _ILJitGetObjectClassPrivate(jitCoder->jitFunction, jitParams[1]);
	vtable = jit_insn_load_relative(jitCoder->jitFunction, classPrivate, 
									offsetof(ILClassPrivate, jitVtable), 
									_IL_JIT_TYPE_VPTR);
	vtableIndex = jit_value_create_nint_constant(jitCoder->jitFunction,
												 _IL_JIT_TYPE_INT32,
												 (jit_nint)methodInfo->index);
	jitFunction = jit_insn_load_elem(jitCoder->jitFunction, vtable,
									 vtableIndex, _IL_JIT_TYPE_VPTR);
	if(info->tailCall == 1)
	{
		returnValue = jit_insn_call_indirect_vtable(jitCoder->jitFunction,
													jitFunction, signature,
													jitParams, argCount,
													JIT_CALL_TAIL);
	}
	else
	{
		returnValue = jit_insn_call_indirect_vtable(jitCoder->jitFunction,
													jitFunction, signature,
													jitParams, argCount,
													0);
	}
	if(returnItem->engineType != ILEngineType_Invalid)
	{
		jitCoder->jitStack[jitCoder->stackTop] = returnValue;
		JITC_ADJUST(jitCoder, 1);
	}
}

static void JITCoder_CallInterface(ILCoder *coder, ILCoderMethodInfo *info,
								   ILEngineStackItem *returnItem,
								   ILMethod *methodInfo)
{
	ILJITCoder *jitCoder = _ILCoderToILJITCoder(coder);
	int argCount = info->numBaseArgs + info->numVarArgs;
	ILJitFunction func = methodInfo->userData;
	ILJitType  signature;
	ILJitValue jitParams[argCount + 2];
	ILJitValue returnValue;
	ILJitValue classPrivate;
	ILJitValue jitFunction;
	ILJitValue interfaceClass;
	ILJitValue methodIndex;
	ILJitValue args[3];
	jit_label_t label = jit_label_undefined;

	if(!func)
	{
		signature = _ILJitCreateMethodSignature(jitCoder, methodInfo);
	}
	else
	{
		signature = jit_function_get_signature(func);
	}
	jitParams[0] = jit_value_get_param(jitCoder->jitFunction, 0);
	_ILJitFillArguments(jitCoder, &(jitParams[1]), info);
	/* TODO: handle varargs here and create a call signature. */

	jit_insn_check_null(jitCoder->jitFunction, jitParams[1]);
	classPrivate = _ILJitGetObjectClassPrivate(jitCoder->jitFunction, jitParams[1]);
	interfaceClass = jit_value_create_nint_constant(jitCoder->jitFunction,
													_IL_JIT_TYPE_VPTR,
													(jit_nint)methodInfo->member.owner);
	methodIndex = jit_value_create_nint_constant(jitCoder->jitFunction,
												 _IL_JIT_TYPE_UINT32,
												 (jit_nint)methodInfo->index);
	args[0] = classPrivate;
	args[1] = interfaceClass;
	args[2] = methodIndex;
	jitFunction = jit_insn_call_native(jitCoder->jitFunction, 0,
									   _ILRuntimeLookupInterfaceMethod,
									   _ILJitSignature_ILRuntimeLookupInterfaceMethod,
									   args, 3, 0);
	jit_insn_branch_if(jitCoder->jitFunction, jitFunction, &label);
	/* TODO: raise a MissingMethodException here. */

	jit_insn_label(jitCoder->jitFunction, &label);
	if(info->tailCall == 1)
	{
		returnValue = jit_insn_call_indirect_vtable(jitCoder->jitFunction,
													jitFunction, signature,
													jitParams, argCount,
													JIT_CALL_TAIL);
	}
	else
	{
		returnValue = jit_insn_call_indirect_vtable(jitCoder->jitFunction,
													jitFunction, signature,
													jitParams, argCount,
													0);
	}
	if(returnItem->engineType != ILEngineType_Invalid)
	{
		jitCoder->jitStack[jitCoder->stackTop] = returnValue;
		JITC_ADJUST(jitCoder, 1);
	}
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
}

static void JITCoder_LoadVirtualAddr(ILCoder *coder, ILMethod *methodInfo)
{
}

static void JITCoder_LoadInterfaceAddr(ILCoder *coder, ILMethod *methodInfo)
{
}

#endif	/* IL_JITC_CODE */
