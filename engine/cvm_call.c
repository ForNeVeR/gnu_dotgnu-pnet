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

/* No globals required */

#elif defined(IL_CVM_LOCALS)

/* No locals required */

#elif defined(IL_CVM_MAIN)

case COP_CALL:
{
	/* Call a method that has been converted into CVM code */
#if 0
	ALLOC_CALL_FRAME();
	callFrame->method = method;
	callFrame->pc = (ILUInt32)(pc - pcstart + 9);
	callFrame->frame = (ILUInt32)(frame - stackbottom);
	method = ILMethod_FromToken(ILProgramItem_Image(method),
								IL_READ_UINT32(pc + 5));
	pc += IL_READ_INT32(pc + 1);
	pcstart = pc;
#endif
	MODIFY_PC_AND_STACK(1, 0);
}
break;

case COP_CALL_EXTERN:
{
	/* Call a method that we don't know if it has been converted */
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

case COP_CALL_VIRTUAL:
{
	MODIFY_PC_AND_STACK(1, 0);
}
break;

case COP_RETURN:
{
	/* Return from a method with no return value */
#if 0
	stacktop = frame;
popFrame:
	--callFrame;
	method = callFrame->method;
	if(!(method->callInfo->pcstart))
	{
		/* The method was flushed since we were last
		   here, so we need to convert it again */
	}
	frame = stackbottom + callFrame->frame;
	pcstart = method->callInfo->pcstart;
	pc = pcstart + callFrame->pc;
#endif
	MODIFY_PC_AND_STACK(1, 0);
}
break;

case COP_RETURN_1:
{
	/* Return from a method with a single-word return value */
#if 0
	frame[0] = stacktop[-1];
	stacktop = frame + 1;
	goto popFrame;
#endif
	MODIFY_PC_AND_STACK(1, 0);
}
break;

case COP_RETURN_2:
{
	/* Return from a method with a double-word return value */
#if 0
	frame[0] = stacktop[-2];
	frame[1] = stacktop[-1];
	stacktop = frame + 2;
	goto popFrame;
#endif
	MODIFY_PC_AND_STACK(1, 0);
}
break;

case COP_RETURN_N:
{
	/* Return from a method with an N-word return value */
#if 0
	temp = IL_READ_UINT32(pc + 1);
	IL_MEMMOVE(frame, stacktop, sizeof(CVMWord) * temp);
	stacktop = frame + temp;
	goto popFrame;
#endif
	MODIFY_PC_AND_STACK(1, 0);
}
break;

#elif defined(IL_CVM_PREFIX)

case COP_PREFIX_TAIL:
{
	/* Perform a tail call to another method */
	MODIFY_PC_AND_STACK(2, 0);
}
break;

case COP_PREFIX_CALL_NON_IL:
{
	/* Perform a call to a method that is not IL */
	MODIFY_PC_AND_STACK(2, 0);
}
break;

#endif /* IL_CVM_PREFIX */
