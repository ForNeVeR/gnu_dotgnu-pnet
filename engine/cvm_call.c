/*
 * cvm_call.c - Opcodes for performing calls to other methods.
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

/*
 * Allocate a new call frame.
 */
static ILCallFrame *AllocCallFrame(ILExecThread *thread)
{
	/* TODO: resize the call frame stack */
	return 0;
}
#define	ALLOC_CALL_FRAME()	\
			do { \
				if(thread->numFrames < thread->maxFrames) \
				{ \
					callFrame = &(thread->frameStack[(thread->numFrames)++]); \
				} \
				else \
				{ \
					callFrame = AllocCallFrame(thread); \
					if(!callFrame) \
					{ \
						STACK_OVERFLOW_EXCEPTION(); \
					} \
				} \
			} while (0)

#elif defined(IL_CVM_LOCALS)

ILMethod *methodToCall;
ILCallFrame *callFrame;

#elif defined(IL_CVM_MAIN)

case COP_CALL:
{
	/* Call a method that has already been converted into CVM code */
	methodToCall = (ILMethod *)(ReadPointer(pc + 5));

	/* Allocate a new call frame */
	ALLOC_CALL_FRAME();

	/* Fill in the call frame details */
	callFrame->method = method;
	callFrame->pc = (ILUInt32)(pc - pcstart + sizeof(void *) + 5);
	callFrame->frame = (ILUInt32)(frame - stackbottom);
	callFrame->except = IL_MAX_UINT32;	/* Not an exception frame */

	/* Pass control to the new method */
	pc += IL_READ_INT32(pc + 1);
	pcstart = pc;
	method = methodToCall;
}
break;

case COP_CALL_EXTERN:
{
	/* Call a method that we don't know if it has been converted */
	methodToCall = (ILMethod *)(ReadPointer(pc + 5));

	/* Copy the state back into the thread object */
	COPY_STATE_TO_THREAD();

#if 0
	ALLOC_CALL_FRAME();
	callFrame->method = method;
	callFrame->pc = (ILUInt32)(pc - pcstart + 9);
	callFrame->frame = (ILUInt32)(frame - stackbottom);
	method = ILMethod_FromToken(ILProgramItem_Image(method),
								IL_READ_UINT32(pc + 5));
	if(!(method->callInfo->pcstart))
	{
		/* Convert the method into CVM code */
	}
	pcstart = method->callInfo->pcstart;

	/* Replace this instruction with COP_CALL so that the
	   direct call can be used the next time we get here */
	pc[0] = COP_CALL;
	IL_WRITE_INT32(pc + 1, (ILInt32)(pcstart - pc));
	pc = pcstart;
#endif
	MODIFY_PC_AND_STACK(1, 0);
}
break;

case COP_CALL_NATIVE:
{
	/* Call a native method */
#if 0
	/* Push the call frame */
	ALLOC_CALL_FRAME();
	callFrame->method = method;
	callFrame->pc = (ILUInt32)(pc - pcstart + 5);
	callFrame->frame = (ILUInt32)(frame - stackbottom);

	/* Determine which native method to call */
	method = ILMethod_FromToken(ILProgramItem_Image(method),
								IL_READ_UINT32(pc + 1));

	/* Save the state to the thread object */
	SAVE_STATE();

	/* Call the native method.  We assume that native
	   methods can never throw exceptions */
	CVMCallNative(thread, method);

	/* Restore the state from the thread object */
	RESTORE_STATE();

	/* Pop the call frame */
	goto popFrame;
#endif
	MODIFY_PC_AND_STACK(1, 0);
}
break;

case COP_CALL_NATIVE_VOID:
{
	/* Call a native method that has no return value */
#if 0
	/* Push the call frame */
	ALLOC_CALL_FRAME();
	callFrame->method = method;
	callFrame->pc = (ILUInt32)(pc - pcstart + 5);
	callFrame->frame = (ILUInt32)(frame - stackbottom);

	/* Determine which native method to call */
	method = ILMethod_FromToken(ILProgramItem_Image(method),
								IL_READ_UINT32(pc + 1));

	/* Save the state to the thread object */
	SAVE_STATE();

	/* Call the native method.  We assume that native
	   methods can never throw exceptions */
	CVMCallNative(thread, method);

	/* Restore the state from the thread object */
	RESTORE_STATE();

	/* Pop the call frame */
	goto popFrame;
#endif
	MODIFY_PC_AND_STACK(1, 0);
}
break;

case COP_CALL_VIRTUAL:
{
	MODIFY_PC_AND_STACK(1, 0);
}
break;

case COP_RETURN:
{
	/* Return from a method with no return value */
	stacktop = frame;
popFrame:
	callFrame = &(thread->frameStack[--(thread->numFrames)]);
	methodToCall = callFrame->method;
	if(callFrame->pc != IL_MAX_UINT32)
	{
		/* We are returning to a CVM method that called us */
		if(methodToCall->userData1 == 0 ||
		   method->userData2 != methodToCall->userData2)
		{
			/* We need to re-convert the method because it has been flushed */
			/* TODO */
		}

		/* The CVM code is now valid, so return to the previous level */
		pcstart = (unsigned char *)(methodToCall->userData1);
		pc = pcstart + callFrame->pc;
		frame = stackbottom + callFrame->frame;
		method = methodToCall;
	}
	else
	{
		/* We are returning to an external method that called us.
		   Copy the current state back to the thread object and
		   then return from the interpreter loop */
		thread->pcstart = 0;
		thread->pc = 0;
		thread->frame = callFrame->frame;
		thread->stackTop = stacktop;
		thread->method = methodToCall;
		return 0;
	}
}
break;

case COP_RETURN_1:
{
	/* Return from a method with a single-word return value */
	frame[0] = stacktop[-1];
	stacktop = frame + 1;
	goto popFrame;
}
/* Not reached */

case COP_RETURN_2:
{
	/* Return from a method with a double-word return value */
	frame[0] = stacktop[-2];
	frame[1] = stacktop[-1];
	stacktop = frame + 2;
	goto popFrame;
}
/* Not reached */

case COP_RETURN_N:
{
	/* Return from a method with an N-word return value */
	tempNum = IL_READ_UINT32(pc + 1);
	IL_MEMMOVE(frame, stacktop, sizeof(CVMWord) * tempNum);
	stacktop = frame + tempNum;
	goto popFrame;
}
/* Not reached */

#elif defined(IL_CVM_PREFIX)

case COP_PREFIX_TAIL:
{
	/* Perform a tail call to another method */
	MODIFY_PC_AND_STACK(2, 0);
}
break;

#endif /* IL_CVM_PREFIX */
