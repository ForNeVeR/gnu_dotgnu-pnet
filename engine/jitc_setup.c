/*
 * jitc_setup.c - Coder implementation for JIT method entry setup.
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
 * Set up a JIT coder instance to process a specific method.
 */
static int JITCoder_Setup(ILCoder *_coder, unsigned char **start,
						  ILMethod *method, ILMethodCode *code)
{
	ILJITCoder *coder = ((ILJITCoder *)_coder);
	ILJITLabel *label0;
#ifdef IL_DEBUGGER
	ILDebugger *debugger;
#endif

	/* Record the current jitted function. */
	coder->jitFunction = ILJitFunctionFromILMethod(method);
	/* Record the current method. */
	coder->currentMethod = method;

#if !defined(IL_CONFIG_REDUCE_CODE) && !defined(IL_WITHOUT_TOOLS)
	if (coder->flags & IL_CODER_FLAG_STATS)
	{
		ILMutexLock(globalTraceMutex);
		fprintf(stdout,
			"Setup: %s.%s\n", 
			ILClass_Name(ILMethod_Owner(method)),
			ILMethod_Name(method));
		ILMutexUnlock(globalTraceMutex);
	}
#endif

	if(ILMethod_IsStaticConstructor(method))
	{
		/* We have to take care that the method is executed only once. */
		ILClass *info;
		ILJitValue classInfo;
		ILJitValue cctorOnce;
		ILJitValue attributes;
		ILJitValue temp;
 		jit_label_t startLabel = jit_label_undefined;
		jit_label_t endLabel = jit_label_undefined;
		jit_label_t label1 = jit_label_undefined;

		jit_insn_label(coder->jitFunction, &startLabel);

		info = ILMethod_Owner(method);
		classInfo = jit_value_create_nint_constant(coder->jitFunction,
										   _IL_JIT_TYPE_VPTR,
										   (jit_nint)info);
		cctorOnce = jit_value_create_nint_constant(coder->jitFunction,
										   _IL_JIT_TYPE_UINT32,
										   (jit_nint)IL_META_TYPEDEF_CCTOR_ONCE);
		attributes = jit_insn_load_relative(coder->jitFunction,
													   classInfo,
													   offsetof(ILClass, attributes),
													   _IL_JIT_TYPE_UINT32);
		temp = jit_insn_and(coder->jitFunction, attributes, cctorOnce);
		jit_insn_branch_if_not(coder->jitFunction, temp, &label1);
		jit_insn_return(coder->jitFunction, 0);
		jit_insn_label(coder->jitFunction, &label1);
		temp = jit_insn_or(coder->jitFunction, attributes, cctorOnce);
		jit_insn_store_relative(coder->jitFunction, classInfo,
								offsetof(ILClass, attributes), temp);

		jit_insn_label(coder->jitFunction, &endLabel);
		/* Make sure that this check will remain very first even if any
		   other blocks are later moved to the function start. */
		jit_insn_move_blocks_to_start(coder->jitFunction, startLabel, endLabel);
	}

#ifdef IL_DEBUGGER
	/* Check if this method can be debugged */
	debugger = ILDebuggerFromProcess(coder->process);
	coder->markBreakpoints =
				(debugger && ILDebuggerIsAssemblyWatched(debugger, method));

	/* Insert potential breakpoint with method in data2 */
	if(coder->markBreakpoints)
	{
		jit_insn_mark_breakpoint(coder->jitFunction,
								 JIT_DEBUGGER_DATA1_METHOD_ENTER,
								 (jit_nint) method);
	}
#endif

	/* Initialize the mem stack for the label stackstates. */
	ILMemStackInit(&(coder->stackStates), 0);

	/* Create the parameters. */
	if(!_ILJitParamsCreate(coder))
	{
		return 0;
	}

	/* Create the local variables. */
	if(!_ILJitLocalsCreate(coder, code->localVarSig))
	{
		return 0;
	}
#ifdef _IL_JIT_OPTIMIZE_INIT_LOCALS
	coder->localsInitialized = 0;
#endif

#ifndef IL_JIT_THREAD_IN_SIGNATURE
	/* Reset the cached thread. */
	coder->thread = 0;
#endif

	/* Ensure that the evaluation stack can hold at least the methods maxStack */
	/* items. */
	/* We need two additional slots for the ValueCtorArgs. */
	ALLOC_STACK(coder, code->maxStack + 2);
	
	/* And reset the stack top. */
	coder->stackTop = 0;

	/* Reset the isInCatcher flag. */
	coder->isInCatcher = 0;

	/* Set the label for the start of the function. */
	label0 = _ILJitLabelGet(coder, 0, _IL_JIT_LABEL_NORMAL);
	jit_insn_label(coder->jitFunction, &(label0->label));

	return 1;
}

/*
 * Set up a JIT coder instance to process a specific external method.
 */
static int JITCoder_SetupExtern(ILCoder *_coder, unsigned char **start,
								ILMethod *method, void *fn, void *cif,
								int isInternal)
{
	return 1;
}

/*
 * Set up a JIT coder instance to process a specific external constructor.
 */
static int JITCoder_SetupExternCtor(ILCoder *_coder, unsigned char **start,
								    ILMethod *method, void *fn, void *cif,
								    void *ctorfn, void *ctorcif,
									int isInternal)
{
	return 1;
}

/*
 * Get the offset of an allocation constructor entry point
 * relative to the main method entry point.
 */
static int JITCoder_CtorOffset(ILCoder *coder)
{
	return 0;
}

/*
 * Finish processing a method using a JIT coder instance.
 */
static int JITCoder_Finish(ILCoder *_coder)
{
	ILJITCoder *jitCoder = ((ILJITCoder *)_coder);
#if !defined(IL_CONFIG_REDUCE_CODE) && !defined(IL_WITHOUT_TOOLS) && defined(_IL_JIT_ENABLE_DEBUG)
	char *methodName = _ILJitFunctionGetMethodName(jitCoder->jitFunction);
#endif

	/* Destroy the mem stack for the label stackstates. */
	ILMemStackDestroy(&(jitCoder->stackStates));

	/* Clear the label pool */
	ILMemPoolClear(&(jitCoder->labelPool));
	jitCoder->labelList = 0;
	if(jitCoder->labelOutOfMemory)
	{
		/* We ran out of memory trying to allocate labels */
		jitCoder->labelOutOfMemory = 0;

		return IL_CODER_END_TOO_BIG;
	}
	jitCoder->labelOutOfMemory = 0;

#if !defined(IL_CONFIG_REDUCE_CODE) && !defined(IL_WITHOUT_TOOLS) && defined(_IL_JIT_ENABLE_DEBUG)
#ifdef _IL_JIT_DUMP_FUNCTION
	if(jitCoder->flags & IL_CODER_FLAG_STATS)
	{
		ILMutexLock(globalTraceMutex);
		jit_dump_function(stdout, jitCoder->jitFunction, methodName);
		ILMutexUnlock(globalTraceMutex);
	}
#endif
#ifdef _IL_JIT_DISASSEMBLE_FUNCTION
	if(jitCoder->flags & IL_CODER_FLAG_STATS)
	{
		if(!jit_function_compile(jitCoder->jitFunction))
		{
			return IL_CODER_END_TOO_BIG;
		}
		ILMutexLock(globalTraceMutex);
		jit_dump_function(stdout, jitCoder->jitFunction, methodName);
		ILMutexUnlock(globalTraceMutex);
	}
#endif
#endif
	return IL_CODER_END_OK;
}

/*
 * Set the flags for profiling debugging etc.
 */
static void JITCoder_SetFlags(ILCoder *_coder,int flags)
{
	(_ILCoderToILJITCoder(_coder))->flags = flags;
}

static int JITCoder_GetFlags(ILCoder *_coder)
{
	return (_ILCoderToILJITCoder(_coder))->flags;
}

/*
 * Mark the end of a method's bytecode, just prior to the exception tables.
 */
static void JITCoder_MarkEnd(ILCoder *coder)
{
}

/*
 * Allocate an extra local variable in the current method frame.
 * Returns the local variable index.
 */
static ILUInt32 JITCoder_AllocExtraLocal(ILCoder *coder, ILType *type)
{
	/* TODO */
	return 0;
}

/*
 * Push a thread value onto the stack for an internalcall.
 */
static void JITCoder_PushThread(ILCoder *_coder, int useRawCalls)
{
}

/*
 * Load the address of an argument onto the native argument stack.
 */
static void JITCoder_LoadNativeArgAddr(ILCoder *_coder, ILUInt32 num)
{
}

/*
 * Load the address of a local onto the native argument stack.
 */
static void JITCoder_LoadNativeLocalAddr(ILCoder *_coder, ILUInt32 num)
{
}

/*
 * Start pushing arguments for a "libffi" call onto the stack.
 */
static void JITCoder_StartFfiArgs(ILCoder *_coder)
{
}

/*
 * Push the address of the raw argument block onto the stack.
 */
static void JITCoder_PushRawArgPointer(ILCoder *_coder)
{
}

/*
 * Perform a function call using "libffi".
 */
static void JITCoder_CallFfi(ILCoder *coder, void *fn, void *cif,
					  		 int useRawCalls, int hasReturn)
{
}

#endif	/* IL_JITC_CODE */
