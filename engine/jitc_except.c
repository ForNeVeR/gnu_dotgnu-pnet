/*
 * jitc_except.c - Coder implementation for JIT exceptions.
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
 * Find the "stackTrace" field within "System.Exception" and then set.
 */
static void _ILJitFindAndSetStackTrace(ILJITCoder *jitCoder, ILJitValue exception)
{
	ILExecThread *_thread = ILExecThreadCurrent();
	ILJitValue thread = _ILJitCoderGetThread(jitCoder);
	ILJitValue trace;
	ILField *field;

	/* Find the "stackTrace" field within the "Exception" class */
	field = ILExecThreadLookupField
			(_thread, "System.Exception", "stackTrace",
			 "[vSystem.Diagnostics.PackedStackFrame;");
	if(field)
	{
		/* Get the stack trace and pop the frame */
		trace = jit_insn_call_native(jitCoder->jitFunction,
									 "_ILJitGetExceptionStackTrace",
									 _ILJitGetExceptionStackTrace,
									 _ILJitSignature_ILJitGetExceptionStackTrace,
									 &thread, 1, JIT_CALL_NOTHROW);

		/* Write the stack trace into the object */
		jit_insn_store_relative(jitCoder->jitFunction, exception,
								field->offset, trace);
	}
}

/*
 * Emit the code to throw a system exception.
 */
static void _ILJitThrowSystem(ILJITCoder *jitCoder, ILUInt32 exception)
{
	static const char * const exceptionClasses[] = {
		"Ok",
		"System.OutOfMemoryException",
		"System.InvalidCastException",
		"System.IndexOutOfRangeException"
	};
	#define	numExceptions	(sizeof(exceptionClasses) / sizeof(const char *))
	ILExecThread *_thread = ILExecThreadCurrent();

	if(exception == _IL_JIT_OUT_OF_MEMORY)
	{
		jit_insn_call_native(jitCoder->jitFunction,
							 "ILRuntimeExceptionThrowOutOfMemory",
							 ILRuntimeExceptionThrowOutOfMemory,
							 _ILJitSignature_ILRuntimeExceptionThrowOutOfMemory,
							 0, 0, JIT_CALL_NORETURN);
	}
	if(exception > 0)
	{
		ILClass *classInfo = ILExecThreadLookupClass(_thread,
													 exceptionClasses[exception]);
		ILJitValue info;
		if(!classInfo)
		{
		#ifndef REDUCED_STDIO
			/* Huh?  The required class doesn't exist.  This shouldn't happen */
			fprintf(stderr, "Fatal error: %s is missing from the system library\n",
					exceptionClasses[exception]);
			return;
		#endif
		}
		classInfo = ILClassResolve(classInfo);
		if(!(classInfo->userData) || 
		   	(((ILClassPrivate *)(classInfo->userData))->inLayout))
		{
			if(!_LayoutClass(_thread, classInfo))
			{
				return;
			}
		}
		info = jit_value_create_nint_constant(jitCoder->jitFunction,
											  _IL_JIT_TYPE_VPTR,
											  (jit_nint)classInfo);
		jit_insn_call_native(jitCoder->jitFunction,
							 "ILRuntimeExceptionThrowClass",
							 ILRuntimeExceptionThrowClass,
							 _ILJitSignature_ILRuntimeExceptionThrowClass,
							 &info, 1, JIT_CALL_NORETURN);
	}
}

/*
 * Set up exception handling for the current method.
 */
static void JITCoder_SetupExceptions(ILCoder *_coder, ILException *exceptions,
									 int hasRethrow)
{
	ILJITCoder *jitCoder = _ILCoderToILJITCoder(_coder);

#if !defined(IL_CONFIG_REDUCE_CODE) && !defined(IL_WITHOUT_TOOLS)
	if (jitCoder->flags & IL_CODER_FLAG_STATS)
	{
		ILMutexLock(globalTraceMutex);
		fprintf(stdout,
			"SetupExceptions: hasRethrow: %i\n", 
			hasRethrow);
		ILMutexUnlock(globalTraceMutex);
	}
#endif

	/* Setup the jit function to handle exceptions. */
	jit_insn_uses_catcher(jitCoder->jitFunction);
	jitCoder->nextBlock = jit_label_undefined;
	jitCoder->rethrowBlock = jit_label_undefined;

	/* We need to setup the filally labels first. */
	while(exceptions != 0)
	{
		if((exceptions->flags & (IL_META_EXCEPTION_FINALLY |
								 IL_META_EXCEPTION_FAULT)) != 0)
		{
		#if !defined(IL_CONFIG_REDUCE_CODE) && !defined(IL_WITHOUT_TOOLS)
			if (jitCoder->flags & IL_CODER_FLAG_STATS)
			{
				ILMutexLock(globalTraceMutex);
				fprintf(stdout,
					"AddFinallyLabel for offset: %i\n", 
					exceptions->handlerOffset);
				ILMutexUnlock(globalTraceMutex);
			}
		#endif
			_ILJitLabelGet(jitCoder, exceptions->handlerOffset,
									 _IL_JIT_LABEL_STARTFINALLY);
		}
		else if ((exceptions->flags & IL_META_EXCEPTION_FILTER) == 0)
		{
			/* Create the label for a catch block */
			/* We need one value on the stack for the exception object. */
			ILJitValue exception = jit_value_create(jitCoder->jitFunction,
													_IL_JIT_TYPE_VPTR);

		#if !defined(IL_CONFIG_REDUCE_CODE) && !defined(IL_WITHOUT_TOOLS)
			if (jitCoder->flags & IL_CODER_FLAG_STATS)
			{
				ILMutexLock(globalTraceMutex);
				fprintf(stdout,
					"AddCatchLabel for offset: %i\n", 
					exceptions->handlerOffset);
				ILMutexUnlock(globalTraceMutex);
			}
		#endif
			jitCoder->jitStack[0] = exception;
			jitCoder->stackTop = 1;
			_ILJitLabelGet(jitCoder, exceptions->handlerOffset,
									 _IL_JIT_LABEL_STARTCATCH);
			jitCoder->stackTop = 0;
		}
		exceptions = exceptions->next;
	}
}

/*
 * Output a throw instruction.
 */
static void JITCoder_Throw(ILCoder *coder, int inCurrentMethod)
{
	ILJITCoder *jitCoder = _ILCoderToILJITCoder(coder);

#if !defined(IL_CONFIG_REDUCE_CODE) && !defined(IL_WITHOUT_TOOLS)
	if (jitCoder->flags & IL_CODER_FLAG_STATS)
	{
		ILMutexLock(globalTraceMutex);
		fprintf(stdout,
			"Throw: inCurrentMethod: %i\n", 
			inCurrentMethod);
		ILMutexUnlock(globalTraceMutex);
	}
#endif

	if(!(jitCoder->isInCatcher))
	{
		ILJitValue exception = jitCoder->jitStack[jitCoder->stackTop - 1];

		jit_insn_call_native(jitCoder->jitFunction,
							 "ILRuntimeExceptionThrow",
							 ILRuntimeExceptionThrow,
							 _ILJitSignature_ILRuntimeExceptionThrow,
							 &exception, 1, JIT_CALL_NORETURN);
		JITC_ADJUST(jitCoder, -1);
	}
}

/*
 * Output a stacktrace instruction.
 */
static void JITCoder_SetStackTrace(ILCoder *coder)
{
}

/*
 * Output a rethrow instruction.
 */
static void JITCoder_Rethrow(ILCoder *coder, ILException *exception)
{
	ILJITCoder *jitCoder = _ILCoderToILJITCoder(coder);

#if !defined(IL_CONFIG_REDUCE_CODE) && !defined(IL_WITHOUT_TOOLS)
	if (jitCoder->flags & IL_CODER_FLAG_STATS)
	{
		ILMutexLock(globalTraceMutex);
		fprintf(stdout,
			"Rethrow: \n");
		ILMutexUnlock(globalTraceMutex);
	}
#endif
}

/*
 * Output a "jump to subroutine" instruction.
 */
static void JITCoder_Jsr(ILCoder *coder, ILUInt32 dest)
{
	ILJITCoder *jitCoder = _ILCoderToILJITCoder(coder);

#if !defined(IL_CONFIG_REDUCE_CODE) && !defined(IL_WITHOUT_TOOLS)
	if (jitCoder->flags & IL_CODER_FLAG_STATS)
	{
		ILMutexLock(globalTraceMutex);
		fprintf(stdout,
			"Jsr: dest: %i\n", 
			dest);
		ILMutexUnlock(globalTraceMutex);
	}
#endif
}

/*
 * Output a "return from subroutine" instruction.
 */
static void JITCoder_RetFromJsr(ILCoder *coder)
{
	ILJITCoder *jitCoder = _ILCoderToILJITCoder(coder);

#if !defined(IL_CONFIG_REDUCE_CODE) && !defined(IL_WITHOUT_TOOLS)
	if (jitCoder->flags & IL_CODER_FLAG_STATS)
	{
		ILMutexLock(globalTraceMutex);
		fprintf(stdout,
			"RetFromJsr: \n");
		ILMutexUnlock(globalTraceMutex);
	}
#endif
}

/*
 * Start a "try" handler block for a region of code.
 */
static void JITCoder_TryHandlerStart(ILCoder *_coder,
									 ILUInt32 start, ILUInt32 end)
{
	ILJITCoder *jitCoder = _ILCoderToILJITCoder(_coder);

#if !defined(IL_CONFIG_REDUCE_CODE) && !defined(IL_WITHOUT_TOOLS)
	if (jitCoder->flags & IL_CODER_FLAG_STATS)
	{
		ILMutexLock(globalTraceMutex);
		fprintf(stdout,
			"TryHandlerStart: start: %i end: %i\n", 
			start,
			end);
		ILMutexUnlock(globalTraceMutex);
	}
#endif

	if(!(jitCoder->isInCatcher))
	{
		/* Tell libjit that we are in the catcher. */
		jit_insn_start_catcher(jitCoder->jitFunction);
		
		jitCoder->isInCatcher = 1;
	}
	else
	{
		/* Insert the jump target for the previous block. */
		jit_insn_label(jitCoder->jitFunction, &(jitCoder->nextBlock));
	}
	/* and reset the label so that it can be used with the next block. */
	jitCoder->nextBlock = jit_label_undefined;

	if(start == 0 && end == IL_MAX_UINT32)
	{
		/* This handler was the last one in the table */
		jit_insn_label(jitCoder->jitFunction, &(jitCoder->rethrowBlock));
		jit_insn_rethrow_unhandled(jitCoder->jitFunction);
	}
	else
	{
		ILJITLabel *startLabel = _ILJitLabelFind(jitCoder, start);;
		ILJITLabel *endLabel = _ILJitLabelFind(jitCoder, end);
		if(startLabel && endLabel)
		{
			jit_insn_branch_if_pc_not_in_range(jitCoder->jitFunction,
											   startLabel->label,
											   endLabel->label,
											   &(jitCoder->nextBlock));
		}
	}
}

/*
 * End a "try" handler block for a region of code.
 */
static void JITCoder_TryHandlerEnd(ILCoder *coder)
{
	ILJITCoder *jitCoder = _ILCoderToILJITCoder(coder);

#if !defined(IL_CONFIG_REDUCE_CODE) && !defined(IL_WITHOUT_TOOLS)
	if (jitCoder->flags & IL_CODER_FLAG_STATS)
	{
		ILMutexLock(globalTraceMutex);
		fprintf(stdout,
			"TryHandlerEnd: \n");
		ILMutexUnlock(globalTraceMutex);
	}
#endif
	jit_insn_branch(jitCoder->jitFunction, &(jitCoder->rethrowBlock));
}

/*
 * Output instructions to match a "catch" clause.
 */
static void JITCoder_Catch(ILCoder *_coder, ILException *exception,
						   ILClass *classInfo, int hasRethrow)
{
	ILJITCoder *jitCoder = _ILCoderToILJITCoder(_coder);
	ILJitValue thread = _ILJitCoderGetThread(jitCoder);
	ILJitValue classTo = jit_value_create_nint_constant(jitCoder->jitFunction,
														_IL_JIT_TYPE_VPTR,
														(jit_nint)classInfo);
	ILJitValue method = jit_value_create_nint_constant(jitCoder->jitFunction,
													   _IL_JIT_TYPE_VPTR,
													   (jit_nint)jitCoder->currentMethod);
	ILJitValue nullException = jit_value_create_nint_constant(jitCoder->jitFunction,
															  _IL_JIT_TYPE_VPTR,
														      (jit_nint)0);
	ILJitValue exceptionObject;
	ILJitValue args[3];
	ILJitValue returnValue;
	ILJITLabel *catchBlock = 0;
	jit_label_t label = jit_label_undefined;

#if !defined(IL_CONFIG_REDUCE_CODE) && !defined(IL_WITHOUT_TOOLS)
	if (jitCoder->flags & IL_CODER_FLAG_STATS)
	{
		ILMutexLock(globalTraceMutex);
		fprintf(stdout,
			"Catch: %s hasRethrow: %i at %i\n",
			ILClass_Name(classInfo),
			hasRethrow,
			exception->handlerOffset);
		ILMutexUnlock(globalTraceMutex);
	}
#endif

	/* Get the current exception bject. */
	exceptionObject = jit_insn_load_relative(jitCoder->jitFunction, thread,
											 offsetof(ILExecThread, thrownException),
											 _IL_JIT_TYPE_VPTR);

	/* Push the exception object on the stack. */
	jitCoder->jitStack[0] = exceptionObject;
	jitCoder->stackTop = 1;
	catchBlock = _ILJitLabelGet(jitCoder, exception->handlerOffset,
										  _IL_JIT_LABEL_STARTCATCH);

	/* Look if the object can be casted to the cought exception type. */
	args[0] = method;
	args[1] = exceptionObject;
	args[2] = classTo;
	returnValue = jit_insn_call_native(jitCoder->jitFunction,
									   "ILRuntimeCanCastClass",
									   ILRuntimeCanCastClass,
									   _ILJitSignature_ILRuntimeCanCastClass,
									   args, 3, JIT_CALL_NOTHROW);
	jit_insn_branch_if_not(jitCoder->jitFunction, returnValue, &label);
	jit_insn_store_relative(jitCoder->jitFunction, thread, 
							offsetof(ILExecThread, thrownException),
							nullException);
	jit_insn_call_native(jitCoder->jitFunction, "jit_exception_clear_last",
												jit_exception_clear_last,
												_ILJitSignature_JitExceptionClearLast,
									   			0, 0, JIT_CALL_NOTHROW);
	jit_insn_branch(jitCoder->jitFunction, &(catchBlock->label));
	jit_insn_label(jitCoder->jitFunction, &label);
}

static void JITCoder_EndCatchFinally(ILCoder *coder, ILException *exception)
{
	ILJITCoder *jitCoder = _ILCoderToILJITCoder(coder);
	
	if((exception->flags & (IL_META_EXCEPTION_FINALLY |
							 IL_META_EXCEPTION_FAULT)) != 0)
	{
	#if !defined(IL_CONFIG_REDUCE_CODE) && !defined(IL_WITHOUT_TOOLS)
		if (jitCoder->flags & IL_CODER_FLAG_STATS)
		{
			ILMutexLock(globalTraceMutex);
			fprintf(stdout,
				"EndFinally: \n");
			ILMutexUnlock(globalTraceMutex);
		}
	#endif
		jit_insn_return_from_finally(jitCoder->jitFunction);
	}
}

/*
 * Handle the leave opcodes.
 */
static void JITCoder_Finally(ILCoder *coder, ILException *exception, int dest)
{
	ILJITCoder *jitCoder = _ILCoderToILJITCoder(coder);
	ILJITLabel *label = _ILJitLabelGet(jitCoder, dest,
									   _IL_JIT_LABEL_STARTFINALLY);

#if !defined(IL_CONFIG_REDUCE_CODE) && !defined(IL_WITHOUT_TOOLS)
	if (jitCoder->flags & IL_CODER_FLAG_STATS)
	{
		ILMutexLock(globalTraceMutex);
		fprintf(stdout,
			"CallFinally: dest: %i\n",
			dest);
		ILMutexUnlock(globalTraceMutex);
	}
#endif
	jit_insn_call_finally(jitCoder->jitFunction, &(label->label));
}

/*
 * Convert a program counter into an exception handler.
 */
static void *JITCoder_PCToHandler(ILCoder *_coder, void *pc, int beyond)
{
	ILJITCoder *jitCoder = _ILCoderToILJITCoder(_coder);
	void *handler;

	ILJitFunction jitFunction = jit_function_from_pc(jitCoder->context, pc,
													 &handler);
	if(jitFunction)
	{
		return handler;
	}
	return 0;
}

/*
 * Convert a program counter into a method descriptor.
 */
static ILMethod *JITCoder_PCToMethod(ILCoder *_coder, void *pc, int beyond)
{
	ILJITCoder *jitCoder = _ILCoderToILJITCoder(_coder);
	ILJitFunction jitFunction = jit_function_from_pc(jitCoder->context, pc,
													 (void **)0);

	if(jitFunction)
	{
		return (ILMethod *)jit_function_get_meta(jitFunction,
												 IL_JIT_META_METHOD);
	}
	return 0;
}

#endif	/* IL_JITC_CODE */
