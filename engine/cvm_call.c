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

/*
 * Restore state information from the thread, except the pc.
 */
#define	RESTORE_STATE_FROM_THREAD()	\
			do { \
				stacktop = thread->stackTop; \
				frame = thread->stackBase + thread->frame; \
				stackbottom = thread->stackBase; \
				stackmax = thread->stackLimit; \
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

	/* Convert the method */
	pcstart = _ILConvertMethod(thread, methodToCall);
	if(!pcstart)
	{
		MISSING_METHOD_EXCEPTION();
	}

	/* Patch the instruction stream to call directly next time */
	if(methodToCall->userData2 == method->userData2)
	{
		pc[0] = COP_CALL;
		tempNum = (ILUInt32)(pcstart - pc);
		pc[1] = (unsigned char)(tempNum);
		pc[2] = (unsigned char)(tempNum >> 8);
		pc[3] = (unsigned char)(tempNum >> 16);
		pc[4] = (unsigned char)(tempNum >> 24);
	}

	/* Allocate a new call frame */
	ALLOC_CALL_FRAME();

	/* Fill in the call frame details */
	callFrame->method = method;
	callFrame->pc = thread->pc + 5 + sizeof(void *);
	callFrame->frame = thread->frame;
	callFrame->except = IL_MAX_UINT32;	/* Not an exception frame */

	/* Restore the state information and jump to the new method */
	RESTORE_STATE_FROM_THREAD();
	pc = pcstart;
	method = methodToCall;
}
break;

case COP_CALL_CTOR:
{
	/* Call a constructor that we don't know if it has been converted */
	methodToCall = (ILMethod *)(ReadPointer(pc + 5));

	/* Copy the state back into the thread object */
	COPY_STATE_TO_THREAD();

	/* Convert the method */
	pcstart = _ILConvertMethod(thread, methodToCall);
	if(!pcstart)
	{
		MISSING_METHOD_EXCEPTION();
	}
	pcstart -= ILCoderCtorOffset(thread->process->coder);

	/* Patch the instruction stream to call directly next time */
	if(methodToCall->userData2 == method->userData2)
	{
		pc[0] = COP_CALL;
		tempNum = (ILUInt32)(pcstart - pc);
		pc[1] = (unsigned char)(tempNum);
		pc[2] = (unsigned char)(tempNum >> 8);
		pc[3] = (unsigned char)(tempNum >> 16);
		pc[4] = (unsigned char)(tempNum >> 24);
	}

	/* Allocate a new call frame */
	ALLOC_CALL_FRAME();

	/* Fill in the call frame details */
	callFrame->method = method;
	callFrame->pc = thread->pc + 5 + sizeof(void *);
	callFrame->frame = thread->frame;
	callFrame->except = IL_MAX_UINT32;	/* Not an exception frame */

	/* Restore the state information and jump to the new method */
	RESTORE_STATE_FROM_THREAD();
	pc = pcstart;
	method = methodToCall;
}
break;

case COP_CALL_NATIVE:
{
	/* Call a native method */
	tempNum = pc[1];
	COPY_STATE_TO_THREAD();
	ffi_call((ffi_cif *)(ReadPointer(pc + 2 + sizeof(void *))),
	         (void (*)())(ReadPointer(pc + 2)), stacktop[-1].ptrValue,
			 (void **)(stacktop - tempNum - 1));
	RESTORE_STATE_FROM_THREAD();
	pcstart = thread->pcstart;
	pc = pcstart + thread->pc;
	MODIFY_PC_AND_STACK(2 + sizeof(void *) * 2, -(ILInt32)(tempNum + 1));
}
break;

case COP_CALL_NATIVE_VOID:
{
	/* Call a native method that has no return value */
	COPY_STATE_TO_THREAD();
	tempNum = pc[1];
	ffi_call((ffi_cif *)(ReadPointer(pc + 2 + sizeof(void *))),
	         (void (*)())(ReadPointer(pc + 2)), 0,
			 (void **)(stacktop - tempNum));
	RESTORE_STATE_FROM_THREAD();
	pcstart = thread->pcstart;
	pc = pcstart + thread->pc;
	MODIFY_PC_AND_STACK(2 + sizeof(void *) * 2, -(ILInt32)tempNum);
}
break;

case COP_CALL_VIRTUAL:
{
	MODIFY_PC_AND_STACK(1, 0);
}
break;

case COP_CALL_INTERFACE:
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
	if(methodToCall)
	{
		/* Returning to within the context of a CVM method */
		if(methodToCall->userData1 == 0 ||
		   method->userData2 != methodToCall->userData2)
		{
			/* We need to re-convert the method because it has been flushed */
			/* TODO */
		}
		pcstart = (unsigned char *)(methodToCall->userData1);
	}
	else
	{
		/* Returning to the top-most level of the thread */
		pcstart = 0;
	}
	pc = pcstart + callFrame->pc;
	frame = stackbottom + callFrame->frame;
	method = methodToCall;

	/* Should we return to an external method? */
	if(callFrame->pc == IL_MAX_UINT32)
	{
		COPY_STATE_TO_THREAD();
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

case COP_PUSH_THREAD:
{
	/* Push a pointer to the thread value onto the stack */
	stacktop[0].ptrValue = (void *)&thread;
	MODIFY_PC_AND_STACK(1, 1);
}
break;

case COP_PUSHDOWN:
{
	/* Push a value on the stack top down and duplicate it twice */
	if(((ILUInt32)(stackmax - stacktop)) >= 1)
	{
		tempptr = stacktop[-1].ptrValue;
		tempNum = IL_READ_UINT32(pc + 1);
		if(tempNum != 0)
		{
			IL_MEMMOVE(stacktop + 1 - tempNum, stacktop - 1 - tempNum,
					   sizeof(CVMWord) * tempNum);
		}
		(stacktop - tempNum - 1)->ptrValue = tempptr;
		(stacktop - tempNum)->ptrValue = tempptr;
		MODIFY_PC_AND_STACK(5, 1);
	}
	else
	{
		STACK_OVERFLOW_EXCEPTION();
	}
}
break;

#elif defined(IL_CVM_WIDE)

case COP_CALL_NATIVE:
{
	/* Wide version of "call_native" */
	tempNum = IL_READ_UINT32(pc + 2);
	ffi_call((ffi_cif *)(ReadPointer(pc + 6 + sizeof(void *))),
	         (void (*)())(ReadPointer(pc + 6)), stacktop[-1].ptrValue,
			 (void **)(stacktop - tempNum - 1));
	MODIFY_PC_AND_STACK(6 + sizeof(void *) * 2, -(ILInt32)(tempNum + 1));
}
break;

case COP_CALL_NATIVE_VOID:
{
	/* Wide version of "call_native_void" */
	tempNum = IL_READ_UINT32(pc + 2);
	ffi_call((ffi_cif *)(ReadPointer(pc + 6 + sizeof(void *))),
	         (void (*)())(ReadPointer(pc + 6)), 0,
			 (void **)(stacktop - tempNum));
	MODIFY_PC_AND_STACK(6 + sizeof(void *) * 2, -(ILInt32)tempNum);
}
break;

case COP_CALL_VIRTUAL:
{
	/* Wide version of "call_virtual" */
	MODIFY_PC_AND_STACK(2, 0);
}
break;

case COP_CALL_INTERFACE:
{
	/* Wide version of "call_interface" */
	MODIFY_PC_AND_STACK(2, 0);
}
break;

#elif defined(IL_CVM_PREFIX)

case COP_PREFIX_TAIL:
{
	/* Perform a tail call to another method */
	MODIFY_PC_AND_STACK(2, 0);
}
break;

#endif /* IL_CVM_PREFIX */
