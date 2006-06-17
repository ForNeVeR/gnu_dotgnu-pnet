/*
 * cvm_except.c - Opcodes for handling exceptions.
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

#if defined(IL_CVM_GLOBALS)

/* Moved to system.c */

#elif defined(IL_CVM_LOCALS)

/* No locals required */

#elif defined(IL_CVM_MAIN)

/**
 * <opcode name="jsr" group="Exception handling instructions">
 *   <operation>Jump to local subroutine</operation>
 *
 *   <format>jsr<fsep/>offset<fsep/>0<fsep/>0<fsep/>0<fsep/>0</format>
 *   <format>br_long<fsep/>jsr
 *       <fsep/>offset1<fsep/>offset2<fsep/>offset3<fsep/>offset4</format>
 *   <dformat>{jsr}<fsep/>dest</dformat>
 *
 *   <form name="jsr" code="COP_JSR"/>
 *
 *   <before>...</before>
 *   <after>..., address</after>
 *
 *   <description>The program counter for the next instruction (<i>pc + 6</i>)
 *   is pushed on the stack as type <code>ptr</code>.  Then the program
 *   branches to <i>pc + offset</i>.</description>
 *
 *   <notes>This instruction is used to implement <code>finally</code>
 *   blocks.</notes>
 * </opcode>
 */
VMCASE(COP_JSR):
{
	/* Jump to a subroutine within this method */
	stacktop[0].ptrValue = (void *)CVM_ARG_JSR_RETURN;
	pc = CVM_ARG_BRANCH_SHORT;
	stacktop += 1;
}
VMBREAK(COP_JSR);

/**
 * <opcode name="ret_jsr" group="Exception handling instructions">
 *   <operation>Return from local subroutine</operation>
 *
 *   <format>ret_jsr</format>
 *   <dformat>{ret_jsr}</dformat>
 *
 *   <form name="ret_jsr" code="COP_RET_JSR"/>
 *
 *   <before>..., address</before>
 *   <after>...</after>
 *
 *   <description>The <i>address</i> is popped from the stack as the
 *   type <code>ptr</code> and transferred into <i>pc</i>.</description>
 *
 *   <notes>This instruction is used to implement <code>finally</code>
 *   blocks.</notes>
 * </opcode>
 */
VMCASE(COP_RET_JSR):
{
	/* Return from a subroutine within this method */
	pc = (unsigned char *)(stacktop[-1].ptrValue);
	stacktop -= 1;
}
VMBREAK(COP_RET_JSR);

#elif defined(IL_CVM_PREFIX)

/**
 * <opcode name="enter_try" group="Exception handling instructions">
 *   <operation>Enter <code>try</code> context for the
 *				current method</operation>
 *
 *   <format>prefix<fsep/>enter_try</format>
 *   <dformat>{enter_try}</dformat>
 *
 *   <form name="enter_try" code="COP_PREFIX_ENTER_TRY"/>
 *
 *   <description>The exception frame height for the current method
 *   is set to the current height of the stack.</description>
 *
 *   <notes>This must be in the prolog of any method that includes
 *   <code>try</code> blocks.  It sets the "base height" of the stack
 *   so that <i>throw</i> instructions know where to unwind the stack
 *   to when an exception is thrown.</notes>
 * </opcode>
 */
VMCASE(COP_PREFIX_ENTER_TRY):
{
	/* Enter a try context for this method */
	thread->exceptHeight = stacktop;

	MODIFY_PC_AND_STACK(CVMP_LEN_NONE, 0);
}
VMBREAK(COP_PREFIX_ENTER_TRY);

/* Label that we jump to when the engine throws an internal exception */
throwException:
{
	if(!ILCoderPCToHandler(thread->process->coder, pc, 0))
	{
		goto throwCaller;
	}
}
/* Fall through */

/**
 * <opcode name="throw" group="Exception handling instructions">
 *   <operation>Throw an exception</operation>
 *
 *   <format>prefix<fsep/>throw</format>
 *   <dformat>{throw}</dformat>
 *
 *   <form name="throw" code="COP_PREFIX_THROW"/>
 *
 *   <before>..., working1, ..., workingN, object</before>
 *   <after>..., object</after>
 *
 *   <description>The <i>object</i> is popped from the stack as
 *   type <code>ptr</code>.  The stack is then reset to the same
 *   as the current method's exception frame height.  Then,
 *   <i>object</i> is re-pushed onto the stack and control is
 *   passed to the current method's exception matching code.</description>
 *
 *   <notes>This is used to throw exceptions within methods that
 *   have an <i>enter_try</i> instruction.  Use <i>throw_caller</i>
 *   if the method does not include <code>try</code> blocks.<p/>
 *
 *   Setting the stack height to the exception frame height ensures
 *   that all working values are removed from the stack prior to entering
 *   the exception matching code.</notes>
 * </opcode>
 */
VMCASE(COP_PREFIX_THROW):
{
	/* Move the exception object down the stack to just above the locals */
	thread->exceptHeight->ptrValue = stacktop[-1].ptrValue;
	thread->currentException = stacktop[-1].ptrValue;
	stacktop = thread->exceptHeight + 1;

	/* Search the exception handler table for an applicable handler */
searchForHandler:
#ifdef IL_DUMP_CVM
	fputs("Throw ", IL_DUMP_CVM_STREAM);
	DUMP_STACK();
#endif
	
	tempNum = (ILUInt32)(pc - (unsigned char *)(method->userData));
	pc = ILCoderPCToHandler(thread->process->coder, pc, 0);
	
	while(tempNum < CVM_ARG_TRY_START || tempNum >= CVM_ARG_TRY_END)
	{
		pc += CVM_ARG_TRY_LENGTH;
	}
	pc += CVM_LEN_TRY;
}
VMBREAK(COP_PREFIX_THROW);

/**
 * <opcode name="throw_caller" group="Exception handling instructions">
 *   <operation>Throw an exception to the caller of this method</operation>
 *
 *   <format>prefix<fsep/>throw_caller</format>
 *   <dformat>{throw_caller}</dformat>
 *
 *   <form name="throw_caller" code="COP_PREFIX_THROW_CALLER"/>
 *
 *   <before>..., working1, ..., workingN, object</before>
 *   <after>..., object</after>
 *
 *   <description>The <i>object</i> is popped from the stack as
 *   type <code>ptr</code>.  The call frame stack is then unwound
 *   until a call frame with a non-zero exception frame height is found.
 *   The stack is then reset to the specified exception frame height.
 *   Then, <i>object</i> is re-pushed onto the stack and control is
 *   passed to the call frame method's exception matching code.</description>
 *
 *   <notes>This is used to throw exceptions from within methods that
 *   do not have an <i>enter_try</i> instruction.  Use <i>throw</i>
 *   if the method does include <code>try</code> blocks.</notes>
 * </opcode>
 */
VMCASE(COP_PREFIX_THROW_CALLER):
{
	/* Throw an exception to the caller of this method */
throwCaller:
#ifdef IL_DUMP_CVM
	fputs("Throw Caller ", IL_DUMP_CVM_STREAM);
	DUMP_STACK();
#endif
	tempptr = stacktop[-1].ptrValue;
	thread->currentException = tempptr;
	if(!tempptr)
	{
		--stacktop;
		NULL_POINTER_EXCEPTION();
	}

	/* Locate a call frame that has an exception handler */
	do
	{
		stacktop = frame;
		callFrame = &(thread->frameStack[--(thread->numFrames)]);
		methodToCall = callFrame->method;
		pc = callFrame->pc;
		thread->exceptHeight = callFrame->exceptHeight;
		frame = callFrame->frame;
		method = methodToCall;

#ifdef IL_DUMP_CVM
		if(methodToCall)
		{
			BEGIN_NATIVE_CALL();
			fprintf(IL_DUMP_CVM_STREAM, "Throwing Back To %s::%s\n",
				    methodToCall->member.owner->className->name,
				    methodToCall->member.name);
			END_NATIVE_CALL();
		}
#endif

		/* Should we return to an external method? */
		if(callFrame->pc == IL_INVALID_PC)
		{
			thread->thrownException = tempptr;
			COPY_STATE_TO_THREAD();
			return 1;
		}
	}
	while(!ILCoderPCToHandler(thread->process->coder, pc, 1));

	/* Copy the exception object into place */
	stacktop = thread->exceptHeight;
	stacktop[0].ptrValue = tempptr;
	++stacktop;

	/* Back up one byte to ensure that the pc falls within
	   the exception region for the method */
	--pc;

	/* Search for an exception handler within this method */
	goto searchForHandler;
}
/* Not reached */

/**
 * <opcode name="set_stack_trace" group="Exception handling instructions">
 *   <operation>Set the stack trace in an exception object at
 *              the throw point</operation>
 *
 *   <format>prefix<fsep/>set_stack_trace</format>
 *   <dformat>{set_stack_trace}</dformat>
 *
 *   <form name="set_stack_trace" code="COP_PREFIX_SET_STACK_TRACE"/>
 *
 *   <before>..., object</before>
 *   <after>..., object</after>
 *
 *   <description>The <i>object</i> is popped from the stack as
 *   type <code>ptr</code>; information about the current method's
 *   stack calling context is written into <i>object</i>; and then
 *   <i>object</i> is pushed back onto the stack.</description>
 *
 *   <notes>This opcode will have no effect if <i>object</i> is
 *   <code>null</code>, or if its class does not inherit from
 *   <code>System.Exception</code>.</notes>
 * </opcode>
 */
VMCASE(COP_PREFIX_SET_STACK_TRACE):
{
	/* Set the stack trace within an exception object */
#if defined(IL_CONFIG_REFLECTION) && defined(IL_CONFIG_DEBUG_LINES)
	COPY_STATE_TO_THREAD();
	BEGIN_NATIVE_CALL();
	_ILSetExceptionStackTrace(thread, stacktop[-1].ptrValue);
	END_NATIVE_CALL();
	RESTORE_STATE_FROM_THREAD();
#endif
	MODIFY_PC_AND_STACK(CVMP_LEN_NONE, 0);
}
VMBREAK(COP_PREFIX_SET_STACK_TRACE);

/**
 * <opcode name="start_catch" group="Exception handling instructions">
 *   <operation>Save state information for Thread.Abort</operation>
 *
 *   <format>prefix<fsep/>start_catch</format>
 *   <dformat>{set_stack_trace}</dformat>
 *
 *   <form name="set_stack_trace" code="COP_PREFIX_START_CATCH"/>
 *
 *   <before>...</before>
 *   <after>...</after>
 *
 *   <description>
 *   If the thread is aborting and a <code>ThreadAbortException</code>
 *   has been thrown then save the current point where the
 *   <code>ThreadAbortException</code> was thrown.
 *   If the thread is aborting and the current exception isn't a 
 *   ThreadAbortException then reset the current exception to
 *   ThreadAbortException.  This happens if a thread throws an
 *   exception while it is being aborted (usually occurs in a finally clause).
 *   </description>
 * </opcode>
 */
VMCASE(COP_PREFIX_START_CATCH):
{
	if (thread->aborting)
	{
		if (thread->currentException
			&& ILExecThreadIsThreadAbortException(thread, thread->currentException)
			&& !thread->threadAbortException)
		{
			/* Save info about the handler that noticed the abort */
			thread->threadAbortException = thread->currentException;
			thread->abortHandlerEndPC = CVMP_ARG_PTR(unsigned char *);
			thread->abortHandlerFrame = thread->numFrames;
		}
		else
		{
			/* A non-thread abort exception has been caught so restore
			   the "current exception" to be the ThreadAbortException */
			thread->currentException = thread->threadAbortException;
		}
	}

	MODIFY_PC_AND_STACK(CVMP_LEN_PTR, 0);
}
VMBREAK(COP_PREFIX_START_CATCH);

/**
 * <opcode name="start_finally" group="Exception handling instructions">
 *   <operation>Save state information for Thread.Abort</operation>
 *
 *   <format>prefix<fsep/>start_finally</format>
 *   <dformat>{set_stack_trace}</dformat>
 *
 *   <form name="set_stack_trace" code="COP_PREFIX_START_FINALLY"/>
 *
 *   <before>...</before>
 *   <after>...</after>
 *
 *   <description>
 *   If the thread is aborting and a <code>ThreadAbortException</code>
 *   has been thrown then save the current point where the
 *   <code>ThreadAbortException</code> was thrown.
 *   </description>
 * </opcode>
 */
VMCASE(COP_PREFIX_START_FINALLY):
{
	if (thread->aborting)
	{
		if (thread->currentException
			&& ILExecThreadIsThreadAbortException(thread, thread->currentException)
			&& !thread->threadAbortException)
		{
			/* Save info about the handler that noticed the abort */
			thread->threadAbortException = thread->currentException;
			thread->abortHandlerEndPC = CVMP_ARG_PTR(unsigned char *);
			thread->abortHandlerFrame = thread->numFrames;
		}
	}

	MODIFY_PC_AND_STACK(CVMP_LEN_PTR, 0);
}
VMBREAK(COP_PREFIX_START_FINALLY);

/**
 * <opcode name="propagate_abort" group="Exception handling instructions">
 *   <operation>Propagate ThreadAbortExceptions</operation>
 *
 *   <format>prefix<fsep/>propagate_abort</format>
  *
 *   <form name="propagate_abort" code="COP_PREFIX_PROPAGATE_ABORT"/>
 *
 *   <before>...</before>
 *   <after>...</after>
 *
 *   <description>
 *   Check if the thread is aborting and propagate the ThreadAbortException
 *   if the thread is at the end (or past) the catch or finally clause that
 *   first detected the exception.
 *   </description>
 * </opcode>
 */
VMCASE(COP_PREFIX_PROPAGATE_ABORT):
{	
	if (thread->aborting)
	{
		/* Check to see if we are at a point where the ThreadAbortException
		   should be propagated */

		if (
			/* Verify that currentException isn't null (it shouldn't be) */
			thread->currentException
			/* Make sure exception is a ThreadAbortException */
			&& ILExecThreadIsThreadAbortException(thread, thread->currentException)
			&& 
			/* Make sure we've reached or gone below (call stack wise) the catch/finally
			   clause that first noticed the ThreadAbortException */
			((pc >= thread->abortHandlerEndPC && thread->numFrames == thread->abortHandlerFrame)
			|| 
			(thread->numFrames < thread->abortHandlerFrame)))
		{
			/* Push the ThreadAbortException onto the stack */
			stacktop[0].ptrValue = thread->currentException;						
			
			/* Since we've reached the end of the abort handler, reset these for 
			   the next time a catch/finally is entered */
			thread->threadAbortException = 0;
			thread->abortHandlerEndPC = 0;
			thread->abortHandlerFrame = 0;

			/* Move PC on and increment stack by 1 */
			MODIFY_PC_AND_STACK(CVMP_LEN_NONE, 1);

			goto throwException;
		}
	}

	MODIFY_PC_AND_STACK(CVMP_LEN_NONE, 0);
}
VMBREAK(COP_PREFIX_PROPAGATE_ABORT);

#endif /* IL_CVM_PREFIX */
