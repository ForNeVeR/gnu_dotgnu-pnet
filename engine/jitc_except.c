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

#ifdef	IL_JITC_DECLARATIONS

/*
 * Declaration of the engine internal exceptions.
 */
#define _IL_JIT_OK						0
#define _IL_JIT_OUT_OF_MEMORY			1
#define _IL_JIT_INVALID_CAST			2
#define _IL_JIT_INDEX_OUT_OF_RANGE		3
#define _IL_JIT_MISSING_METHOD			4
#define _IL_JIT_DLL_NOT_FOUND			5
#define _IL_JIT_ENTRYPOINT_NOT_FOUND	6

/*
 * Emit the code to throw a system exception.
 */
static void _ILJitThrowSystem(ILJitFunction jitFunction,
							  ILUInt32 exception);

#endif	/* IL_JITC_DECLARATIONS */

#ifdef	IL_JITC_FUNCTIONS

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
static void _ILJitThrowSystem(ILJitFunction jitFunction,
							  ILUInt32 exception)
{
	static const char * const exceptionClasses[] = {
		"Ok",
		"System.OutOfMemoryException",
		"System.InvalidCastException",
		"System.IndexOutOfRangeException",
		"System.MissingMethodException",
		"System.DllNotFoundException",
		"System.EntryPointNotFoundException"
	};
	#define	numExceptions	(sizeof(exceptionClasses) / sizeof(const char *))
	ILExecThread *_thread = ILExecThreadCurrent();

	if(exception == _IL_JIT_OUT_OF_MEMORY)
	{
		jit_insn_call_native(jitFunction,
							 "ILRuntimeExceptionThrowOutOfMemory",
							 ILRuntimeExceptionThrowOutOfMemory,
							 _ILJitSignature_ILRuntimeExceptionThrowOutOfMemory,
							 0, 0, JIT_CALL_NORETURN);
		return;
	}
	if(exception > 0)
	{
		ILClass *classInfo = _ILLookupClass(_ILExecThreadProcess(_thread),
											exceptionClasses[exception],
											strlen(exceptionClasses[exception]));
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
		info = jit_value_create_nint_constant(jitFunction,
											  _IL_JIT_TYPE_VPTR,
											  (jit_nint)classInfo);
		jit_insn_call_native(jitFunction,
							 "ILRuntimeExceptionThrowClass",
							 ILRuntimeExceptionThrowClass,
							 _ILJitSignature_ILRuntimeExceptionThrowClass,
							 &info, 1, JIT_CALL_NORETURN);
	}
}

/*
 * Check if the current exception thrown is a ThreadAblorException
 * and save it to the threadAbortException in the current ILExecThread.
 * 
 * Because the ThreadAbortException is a sealed class we can simply compare the
 * two ILClass pointers.
 */
static void _ILJitSetThreadAbortException(ILJITCoder *jitCoder,
										  ILJitValue thread,
										  ILJitValue exception)
{
	ILJitValue threadAbortException;
	ILJitValue exceptionClass;
	ILJitValue temp;
	jit_label_t allreadyAbortingLabel = jit_label_undefined;
	ILJitValue threadAbortExceptionClass = 
		jit_value_create_nint_constant(jitCoder->jitFunction,
									   _IL_JIT_TYPE_VPTR,
									   (jit_nint)jitCoder->process->threadAbortClass);

	/* Get the current ThreadAbortException of the thread. */
	threadAbortException = jit_insn_load_relative(jitCoder->jitFunction,
												  thread,
												  offsetof(ILExecThread, threadAbortException),
												  _IL_JIT_TYPE_VPTR);

	/* if there is one then there is nothing else todo. */
	jit_insn_branch_if(jitCoder->jitFunction, 
					   threadAbortException,
					   &allreadyAbortingLabel);

	/* Get the class of the current exception */
	exceptionClass = _ILJitGetObjectClass(jitCoder->jitFunction, exception);

	/* Is the class not the ThreadAbortException class ? */
	temp = jit_insn_ne(jitCoder->jitFunction,
					   exceptionClass,
					   threadAbortExceptionClass);

	/* If it's not a ThreadAbortException then we are ready. */
	jit_insn_branch_if(jitCoder->jitFunction, 
					   temp,
					   &allreadyAbortingLabel);

	/* otherwise save the exception to the threadAbortException in the */
	/* current thread. */
	jit_insn_store_relative(jitCoder->jitFunction,
							thread,
							offsetof(ILExecThread, threadAbortException),
							exception);

	jit_insn_label(jitCoder->jitFunction, &allreadyAbortingLabel);
}

/*
 * Propagate the ThreadAbortException if present.
 */
static void _ILJitPropagateThreadAbort(ILJITCoder *jitCoder,
									   ILException *exception)
{
	/* TODO: Handle the case if the thread is not in the signature.*/
	ILJitValue thread = _ILJitCoderGetThread(jitCoder);
	jit_label_t label = jit_label_undefined;
	ILJitValue currentException = (ILJitValue)(exception->ptrUserData);
	ILJitValue nullException = 
			jit_value_create_nint_constant(jitCoder->jitFunction,
										   _IL_JIT_TYPE_VPTR,
									       (jit_nint)0);
	ILJitValue exceptionObject;
	ILJitValue temp;

#if !defined(IL_CONFIG_REDUCE_CODE) && !defined(IL_WITHOUT_TOOLS)
	if (jitCoder->flags & IL_CODER_FLAG_STATS)
	{
		ILMutexLock(globalTraceMutex);
		fprintf(stdout,
			"PropagateThreadAbortException\n");
		ILMutexUnlock(globalTraceMutex);
	}
#endif

	/* Check if the thread is aborting. */
	temp = jit_insn_load_relative(jitCoder->jitFunction, thread,
								  offsetof(ILExecThread, aborting),
								  jit_type_sys_int);
	jit_insn_branch_if_not(jitCoder->jitFunction,
						   temp,
						   &label);

	/* Get the threadAbortException object. */
	exceptionObject = jit_insn_load_relative(jitCoder->jitFunction, thread,
											 offsetof(ILExecThread, threadAbortException),
											 _IL_JIT_TYPE_VPTR);

	/* Check if they are the same. */
	temp = jit_insn_eq(jitCoder->jitFunction,
					   exceptionObject,
					   currentException);

	jit_insn_branch_if_not(jitCoder->jitFunction,
						   temp,
						   &label);

	/* Clear the ThreadAbortException. */
	jit_insn_store_relative(jitCoder->jitFunction,
							thread,
							offsetof(ILExecThread, threadAbortException),
							nullException);

	jit_insn_call_native(jitCoder->jitFunction,
						 "ILRuntimeExceptionRethrow",
						 ILRuntimeExceptionRethrow,
						 _ILJitSignature_ILRuntimeExceptionRethrow,
						 &exceptionObject, 1, JIT_CALL_NORETURN);

	jit_insn_label(jitCoder->jitFunction, &label);
}

#endif	/* IL_JITC_FUNCTIONS */

#ifdef IL_JITC_CODE

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
			_ILJitStackPushValue(jitCoder, exception);
			_ILJitLabelGet(jitCoder, exceptions->handlerOffset,
									 _IL_JIT_LABEL_STARTCATCH);
			jitCoder->stackTop = 0;

			/* We save the created value in the exception too for rethrow or */
			/* propagating a ThreadAbortException. */
			exceptions->ptrUserData = (void *)exception;
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
		_ILJitStackItemNew(exception);

		_ILJitStackPop(jitCoder, exception);

		jit_insn_call_native(jitCoder->jitFunction,
							 "ILRuntimeExceptionThrow",
							 ILRuntimeExceptionThrow,
							 _ILJitSignature_ILRuntimeExceptionThrow,
							 &(_ILJitStackItemValue(exception)), 1,
							 JIT_CALL_NORETURN);
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
	ILJitValue exceptionObject;

#if !defined(IL_CONFIG_REDUCE_CODE) && !defined(IL_WITHOUT_TOOLS)
	if (jitCoder->flags & IL_CODER_FLAG_STATS)
	{
		ILMutexLock(globalTraceMutex);
		fprintf(stdout,
			"Rethrow: \n");
		ILMutexUnlock(globalTraceMutex);
	}
#endif

	/* The rethrow instruction is allowed in a catch block. */
	if((exception->flags & (IL_META_EXCEPTION_FILTER |
							IL_META_EXCEPTION_FINALLY |
							IL_META_EXCEPTION_FAULT)) == 0)
	{
		/* Get the current exception object. */
		/* We saved the exception in the userData of the ILException. */
		exceptionObject = (ILJitValue)(exception->ptrUserData);

		jit_insn_call_native(jitCoder->jitFunction,
							 "ILRuntimeExceptionRethrow",
							 ILRuntimeExceptionRethrow,
							 _ILJitSignature_ILRuntimeExceptionRethrow,
							 &exceptionObject, 1, JIT_CALL_NORETURN);
	}
	/* If the instruction is outside a catch handler an exception should */
	/* be thrown. */
	/* TODO */
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
	ILJitValue currentException = (ILJitValue)(exception->ptrUserData);
	ILJitValue classTo = jit_value_create_nint_constant(jitCoder->jitFunction,
														_IL_JIT_TYPE_VPTR,
														(jit_nint)classInfo);
	ILJitValue method = jit_value_create_nint_constant(jitCoder->jitFunction,
													   _IL_JIT_TYPE_VPTR,
													   (jit_nint)ILCCtorMgr_GetCurrentMethod(&(jitCoder->cctorMgr)));
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

	/* Get the thrown exception object. */
	exceptionObject = jit_insn_load_relative(jitCoder->jitFunction,
											 thread,
											 offsetof(ILExecThread, currentException),
											 _IL_JIT_TYPE_VPTR);

	/* Save the thrown exception to the current exception block. */
	jit_insn_store(jitCoder->jitFunction, currentException, exceptionObject);

	/* Look if the object can be casted to the cought exception type. */
	args[0] = method;
	args[1] = currentException;
	args[2] = classTo;
	returnValue = jit_insn_call_native(jitCoder->jitFunction,
									   "ILRuntimeCanCastClass",
									   ILRuntimeCanCastClass,
									   _ILJitSignature_ILRuntimeCanCastClass,
									   args, 3, JIT_CALL_NOTHROW);
	jit_insn_branch_if_not(jitCoder->jitFunction, returnValue, &label);

	/* Save the exception in the threadAbortException if it is one. */
	_ILJitSetThreadAbortException(jitCoder, thread, currentException);

	/* Push the exception object on the stack. */
	jitCoder->stackTop = 0;
	_ILJitStackPushValue(jitCoder, currentException);
	catchBlock = _ILJitLabelGet(jitCoder, exception->handlerOffset,
										  _IL_JIT_LABEL_STARTCATCH);

	jit_insn_call_native(jitCoder->jitFunction, "jit_exception_clear_last",
												jit_exception_clear_last,
												_ILJitSignature_JitExceptionClearLast,
									   			0, 0, JIT_CALL_NOTHROW);
	jit_insn_store_relative(jitCoder->jitFunction, thread, 
							offsetof(ILExecThread, currentException),
							nullException);
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
	else if((exception->flags & (IL_META_EXCEPTION_FILTER |
								 IL_META_EXCEPTION_FINALLY |
								 IL_META_EXCEPTION_FAULT)) == 0)
	{
		_ILJitPropagateThreadAbort(jitCoder, exception);
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
