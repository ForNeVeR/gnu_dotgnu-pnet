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

void *_ILSystemException(ILExecThread *thread, const char *className)
{
	ILObject *object;
	ILObject *trace;
	ILCallFrame *callFrame;
	ILClass *classInfo = ILExecThreadLookupClass(thread, className);
	if(!classInfo)
	{
		/* Huh?  The required class doesn't exist.  This shouldn't happen */
		fprintf(stderr, "Fatal error: %s is missing from the system library\n",
				className);
		exit(1);
	}
	object = _ILEngineAllocObject(thread, classInfo);
	if(object)
	{
		/* Find the "stackTrace" field within the "Exception" class */
		ILField *field = ILExecThreadLookupField
				(thread, "System.Exception", "stackTrace",
				 "[vSystem.Diagnostics.PackedStackFrame;");
		if(field)
		{
			/* Push the current frame data onto the stack temporarily
			   so that "GetExceptionStackTrace" can find it */
			if(thread->numFrames < thread->maxFrames)
			{
				callFrame = &(thread->frameStack[(thread->numFrames)++]);
			}
			else if((callFrame = _ILAllocCallFrame(thread)) == 0)
			{
				/* We ran out of memory trying to push the frame */
				object = thread->thrownException;
				thread->thrownException = 0;
				return object;
			}
			callFrame->method = thread->method;
			callFrame->pc = thread->pc;
			callFrame->frame = thread->frame;
			callFrame->exceptHeight = thread->exceptHeight;

			/* Get the stack trace and pop the frame */
			trace = (ILObject *)_IL_StackFrame_GetExceptionStackTrace(thread);
			--(thread->numFrames);
			if(!trace)
			{
				/* We ran out of memory obtaining the stack trace */
				object = thread->thrownException;
				thread->thrownException = 0;
				return object;
			}

			/* Write the stack trace into the object */
			*((ILObject **)(((unsigned char *)object) + field->offset))
					= trace;
		}
	}
	else
	{
		/* The system ran out of memory, so copy the "OutOfMemoryException" */
		object = thread->thrownException;
		thread->thrownException = 0;
	}
	return object;
}

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
	stacktop[0].ptrValue = (void *)(pc + 6);
	pc += (ILInt32)(ILInt8)(pc[1]);
	stacktop += 1;
}
VMBREAK;

/**
 * <opcode name="ret_jsr" group="Exception handling instructions">
 *   <operation>Return from local subroutine</operation>
 *
 *   <format>ret_jsr</format>
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
VMBREAK;

#elif defined(IL_CVM_PREFIX)

/**
 * <opcode name="enter_try" group="Exception handling instructions">
 *   <operation>Enter <code>try</code> context for the
 *				current method</operation>
 *
 *   <format>prefix<fsep/>enter_try</format>
 *
 *   <form name="enter_try" code="COP_PREFIX_ENTER_TRY"/>
 *
 *   <description>The exception frame height for the current method
 *   is set to the current height of the stack.</description>
 *
 *   <notes>This must be in the prolog of any method that includes
 *   <code>try</code> blocks.  It makes the "base height" of the stack
 *   so that <i>throw</i> instructions know where to unwind the stack
 *   to when an exception is thrown.</notes>
 * </opcode>
 */
VMCASE(COP_PREFIX_ENTER_TRY):
{
	/* Enter a try context for this method */
	thread->exceptHeight = (ILUInt32)(stacktop - frame);
	MODIFY_PC_AND_STACK(2, 0);
}
VMBREAK;

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
	frame[thread->exceptHeight].ptrValue = stacktop[-1].ptrValue;
	stacktop = frame + thread->exceptHeight + 1;

	/* Search the exception handler table for an applicable handler */
searchForHandler:
#ifdef IL_DUMP_CVM
	fputs("Throw ", IL_DUMP_CVM_STREAM);
	DUMP_STACK();
#endif
	tempNum = (ILUInt32)(pc - (unsigned char *)(method->userData));
	pc = ILCoderPCToHandler(thread->process->coder, pc, 0);
	while(tempNum < IL_READ_UINT32(pc) || tempNum >= IL_READ_UINT32(pc + 4))
	{
		pc += IL_READ_UINT32(pc + 8);
	}
	pc += 12;
}
VMBREAK;

/**
 * <opcode name="throw_caller" group="Exception handling instructions">
 *   <operation>Throw an exception to the caller of this method</operation>
 *
 *   <format>prefix<fsep/>throw_caller</format>
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
			fprintf(IL_DUMP_CVM_STREAM, "Throwing Back To %s::%s\n",
				    methodToCall->member.owner->name,
				    methodToCall->member.name);
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
	stacktop = frame + thread->exceptHeight;
	stacktop[0].ptrValue = tempptr;
	++stacktop;

	/* Back up one byte to ensure that the pc falls within
	   the exception region for the method */
	--pc;

	/* Search for an exception handler within this method */
	goto searchForHandler;
}
/* Not reached */

#endif /* IL_CVM_PREFIX */
