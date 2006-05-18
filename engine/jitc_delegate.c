/*
 * jitc_delegate.c - Jit coder delegates support.
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
 * Emit the code to call a delegate.
 * args contains only the arguments passed to the method (excluding the this pointer).
 */
static ILJitValue _ILJitDelegateInvokeCodeGen(ILJitFunction func,
											  ILJitFunction invokeFunc,
											  ILJitValue delegate,
											  ILJitValue *args,
											  ILUInt32 argCount)
{
#ifdef IL_JIT_THREAD_IN_SIGNATURE
	ILJitValue thread = _ILJitFunctionGetThread(func);
#endif
	ILJitType signature = jit_function_get_signature(invokeFunc);
	ILUInt32 numArgs = jit_type_num_params(signature);
	ILJitType returnType = jit_type_get_return(signature);
	jit_label_t noTarget = jit_label_undefined;
	jit_label_t endLabel = jit_label_undefined;
	ILJitValue returnValue = 0;
	ILJitValue temp;
	ILJitValue target = jit_insn_load_relative(func, delegate,
											   offsetof(System_Delegate, target),
											   _IL_JIT_TYPE_VPTR);
	ILJitValue function = jit_insn_load_relative(func, delegate,
												 offsetof(System_Delegate, methodInfo),
												 _IL_JIT_TYPE_VPTR);
	ILJitType types[numArgs];
	ILJitValue invokeArgs[numArgs];
	ILUInt32 current;

#ifdef IL_JIT_THREAD_IN_SIGNATURE
	if(argCount != numArgs - 2)
#else
	if(argCount != numArgs - 1)
#endif
	{
		/* Something is wrong here. */
	}

	if(returnType != _IL_JIT_TYPE_VOID)
	{
		returnValue = jit_value_create(func, returnType);
	}
#ifdef IL_JIT_FNPTR_ILMETHOD
	/* We need to convert the methodInfo pointer to the vtable pointer first. */
	function = jit_insn_call_native(func,
									"ILRuntimeMethodToVtablePointer",
									ILRuntimeMethodToVtablePointer,
									_ILJitSignature_ILRuntimeMethodToVtablePointer,
									&function, 1, 0);
#endif

	jit_insn_branch_if_not(func, target, &noTarget);
#ifdef IL_JIT_THREAD_IN_SIGNATURE
	invokeArgs[0] = thread;
	invokeArgs[1] = target;
#else
	invokeArgs[0] = target;
#endif
	for(current = 0; current < argCount; ++current)
	{
	#ifdef IL_JIT_THREAD_IN_SIGNATURE
		invokeArgs[current + 2] = args[current];
	#else
		invokeArgs[current + 1] = args[current];
	#endif
	}
	temp = jit_insn_call_indirect_vtable(func,
										 function, signature,
										 invokeArgs, numArgs, 0);
	if(returnType != _IL_JIT_TYPE_VOID)
	{
		jit_insn_store(func, returnValue, temp);
	}
	jit_insn_branch(func, &endLabel);
	jit_insn_label(func, &noTarget);
#ifdef IL_JIT_THREAD_IN_SIGNATURE
	types[0] = jit_type_get_param(signature, 0);
	args[0] = thread;
#endif
	for(current = 0; current < argCount; ++current)
	{
	#ifdef IL_JIT_THREAD_IN_SIGNATURE
		types[current + 1] = jit_type_get_param(signature, current + 2);
		invokeArgs[current + 1] = args[current];
	#else
		types[current] = jit_type_get_param(signature, current + 1);
		invokeArgs[current] = args[current];
	#endif
	}
	if(!(signature = jit_type_create_signature(IL_JIT_CALLCONV_DEFAULT,
											   returnType,
											   types, numArgs - 1, 1)))
	{
		return 0;
	}
	temp = jit_insn_call_indirect_vtable(func,
										 function, signature,
										 invokeArgs, numArgs - 1, 0);
	jit_type_free(signature);
	if(returnType != _IL_JIT_TYPE_VOID)
	{
		jit_insn_store(func, returnValue, temp);
	}
	jit_insn_label(func, &endLabel);
	return returnValue;
}

/*
 * Build the Invoke method for the delegate.
 * The arguments to this function are:
 * arg 0 = thread
 * arg 1 = this pointer to the delegate
 * arg 2 ... n = arguments for the delegate function
 */
static int _ILJitCompileDelegateInvoke(ILJitFunction func)
{

}

/*
 * Build the Invoke method for the multicast delegate.
 * The arguments to this function are:
 * arg 0 = thread
 * arg 1 = this pointer to the multicast delegate
 * arg 2 ... n = arguments for the delegate function
 */
static int _ILJitCompileMultiCastDelegateInvoke(ILJitFunction func)
{
#ifdef IL_JIT_THREAD_IN_SIGNATURE
	ILJitValue thread = _ILJitFunctionGetThread(func);
#endif
	ILJitType signature = jit_function_get_signature(func);
	ILUInt32 numArgs = jit_type_num_params(signature);
	jit_label_t invokeThis = jit_label_undefined;
	ILJitValue delegate = jit_value_get_param(func, 1);
	ILJitValue returnValue;
	ILJitValue prevDelegate = jit_insn_load_relative(func, delegate,
													 offsetof(System_Delegate, prev),
													 _IL_JIT_TYPE_VPTR);
	ILJitValue args[numArgs]; /* The array for the invokation args. */
	ILUInt32 current;

	if(numArgs < 2)
	{
		/* There is something wrong with this delegate. */
		return JIT_RESULT_COMPILE_ERROR;
	}
	/* If there is no previous delegate then invoke the delegate method. */
	jit_insn_branch_if_not(func, prevDelegate, &invokeThis);
	/* We need to invoke the previous delegate first. */
#ifdef IL_JIT_THREAD_IN_SIGNATURE
	args[0] = thread;
	args[1] = prevDelegate;
	for(current = 2; current < numArgs; ++current)
#else
	args[0] = prevDelegate;
	for(current = 1; current < numArgs; ++current)
#endif
	{
		args[current] = jit_value_get_param(func, current);
	}
	returnValue = jit_insn_call(func, 0, func, 0, args, numArgs, 0);
	jit_insn_label(func, &invokeThis);
#ifdef IL_JIT_THREAD_IN_SIGNATURE
	returnValue = _ILJitDelegateInvokeCodeGen(func, func, delegate,
											  &(args[2]), numArgs - 2);
#else
	returnValue = _ILJitDelegateInvokeCodeGen(func, func, delegate,
											  &(args[1]), numArgs - 1);
#endif
	jit_insn_return(func, returnValue);

	return JIT_RESULT_OK;
}
