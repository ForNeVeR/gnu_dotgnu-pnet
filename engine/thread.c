/*
 * thread.c - Manage threads within the runtime engine.
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

#include "engine.h"

/*
 * Warning: "program.h" allows for illegal access to the internal
 * workings of the "image" library.  We use this for speed.
 */
#include "../image/program.h"

#ifdef	__cplusplus
extern	"C" {
#endif

ILExecThread *ILExecThreadCreate(ILExecProcess *process)
{
	ILExecThread *thread;

	/* Create a new thread block */
	if((thread = (ILExecThread *)ILMalloc(sizeof(ILExecThread))) == 0)
	{
		return 0;
	}

	/* Allocate space for the thread-specific value stack */
	if((thread->stackBase = (ILValue *)ILMalloc(sizeof(ILValue) *
												process->stackSize)) == 0)
	{
		ILFree(thread);
		return 0;
	}
	thread->stackLimit = thread->stackBase + process->stackSize;

	/* Initialize the thread state */
	thread->pc = 0;
	thread->stackTop = thread->stackBase;
	thread->args = thread->stackBase;
	thread->locals = thread->stackBase;
	thread->method = 0;
	thread->frame = 0;

	/* Attach the thread to the process */
	thread->process = process;
	thread->nextThread = process->firstThread;
	thread->prevThread = 0;
	if(process->firstThread)
	{
		process->firstThread->prevThread = thread;
	}
	process->firstThread = thread;
	
	/* Return the thread block to the caller */
	return thread;
}

void ILExecThreadDestroy(ILExecThread *thread)
{
	/* If this is the "main" thread, then clear "process->mainThread" */
	if(thread->process->mainThread == thread)
	{
		thread->process->mainThread = 0;
	}

	/* Detach the thread from its process */
	if(thread->nextThread)
	{
		thread->nextThread->prevThread = thread->prevThread;
	}
	if(thread->prevThread)
	{
		thread->prevThread->nextThread = thread->nextThread;
	}
	else
	{
		thread->process->firstThread = thread->nextThread;
	}

	/* Destroy the thread block */
	ILFree(thread);
}

ILExecProcess *ILExecThreadGetProcess(ILExecThread *thread)
{
	return thread->process;
}

/*
 * Size of ILCallFrame, rounded up to a multiple of "sizeof(ILValue)".
 */
#define	CALL_FRAME_SIZE	\
	(sizeof(ILCallFrame) + \
		((sizeof(ILCallFrame) % sizeof(ILValue)) != 0 ? \
			(sizeof(ILValue) - (sizeof(ILCallFrame) % sizeof(ILValue))) : 0))

int _ILExecThreadFramePush(ILExecThread *thread, ILMethod *method)
{
	ILUInt32 numArgs;
	ILType *methodSig;
	ILType *localVarSig;
	ILType *localType;
	ILUInt32 numLocals;
	ILUInt32 localNum;
	ILUInt32 space;
	ILCallFrame *frame;
	ILValue *stackBase;
	ILMethodCode code;

	/* Get the number of arguments for the method */
	methodSig = method->member.signature;	/* Illegal program.h access */
	numArgs = methodSig->num;
	if((methodSig->kind & IL_TYPE_COMPLEX_METHOD_SENTINEL) != 0)
	{
		--numArgs;
	}
	if((methodSig->kind & (IL_META_CALLCONV_HASTHIS << 8)) != 0 &&
	   (methodSig->kind & (IL_META_CALLCONV_EXPLICITTHIS << 8)) == 0)
	{
		++numArgs;
	}

	/* Get information about the method's code */
	if(ILMethodGetCode(method, &code))
	{
		/* Get the raw type signature for the local variables */
		if(code.localVarSig)
		{
			localVarSig = code.localVarSig->type; /* Illegal program.h access */
			numLocals = localVarSig->num;
		}
		else
		{
			localVarSig = 0;
			numLocals = 0;
		}
	}
	else
	{
		localVarSig = 0;
		numLocals = 0;
		code.code = 0;
		code.maxStack = 8;
	}

	/* Determine if we have enough space on the stack */
	space = (CALL_FRAME_SIZE / sizeof(ILValue)) + numLocals + code.maxStack;
	if(space > (ILUInt32)(thread->stackLimit - thread->stackTop))
	{
		/* Stack overflow: we may reallocate the stack here later */
		return 0;
	}

	/* Create the new frame */
	frame = (ILCallFrame *)(thread->stackTop);
	frame->pc = thread->pc;
	stackBase = thread->stackBase;
	frame->stackTop = ((ILUInt32)(thread->stackTop - stackBase)) - numArgs;
	frame->args = (ILUInt32)(thread->args - stackBase);
	frame->locals = (ILUInt32)(thread->locals - stackBase);
	frame->method = thread->method;
	if(thread->frame)
	{
		frame->parent = (ILUInt32)(((ILValue *)(thread->frame)) - stackBase);
	}
	else
	{
		frame->parent = IL_MAX_UINT32;
	}
	thread->frame = frame;

	/* Set up the new frame */
	thread->args = thread->stackTop - numArgs;
	thread->stackTop += (CALL_FRAME_SIZE / sizeof(ILValue));
	thread->locals = thread->stackTop;
	thread->stackTop += numLocals;
	thread->method = method;

	/* Set the "actualType" fields for the argument values */
	stackBase = thread->args;
	if((methodSig->kind & (IL_META_CALLCONV_HASTHIS << 8)) != 0 &&
	   (methodSig->kind & (IL_META_CALLCONV_EXPLICITTHIS << 8)) == 0)
	{
		/* The actual type of "this" is always object reference */
		stackBase->actualType = IL_TYPE_OBJECT_REF;
		++stackBase;
		--numArgs;
	}
	localNum = 0;
	while(localNum < numArgs)
	{
		/* Get the type of the next argument */
		if(localNum < 3)
		{
			localType = methodSig->un.method.param[localNum];
		}
		else
		{
			localType = methodSig->un.params.param[(localNum - 3) & 3];
		}
		++localNum;

		/* Set the argument's actual type */
		if(ILType_IsPrimitive(localType))
		{
			/* Primitive element type */
			switch(ILType_ToElement(localType))
			{
				case IL_META_ELEMTYPE_BOOLEAN:
				{
					stackBase->actualType = IL_TYPE_BOOLEAN;
				}
				break;

				case IL_META_ELEMTYPE_CHAR:
				{
					stackBase->actualType = IL_TYPE_CHAR;
				}
				break;

				case IL_META_ELEMTYPE_I1:
				{
					stackBase->actualType = IL_TYPE_INT8;
				}
				break;

				case IL_META_ELEMTYPE_U1:
				{
					stackBase->actualType = IL_TYPE_UINT8;
				}
				break;

				case IL_META_ELEMTYPE_I2:
				{
					stackBase->actualType = IL_TYPE_INT16;
				}
				break;

				case IL_META_ELEMTYPE_U2:
				{
					stackBase->actualType = IL_TYPE_UINT16;
				}
				break;

				case IL_META_ELEMTYPE_I4:
				{
					stackBase->actualType = IL_TYPE_INT32;
				}
				break;

				case IL_META_ELEMTYPE_U4:
				{
					stackBase->actualType = IL_TYPE_UINT32;
				}
				break;

				case IL_META_ELEMTYPE_I8:
				{
					stackBase->actualType = IL_TYPE_INT64;
				}
				break;

				case IL_META_ELEMTYPE_U8:
				{
					stackBase->actualType = IL_TYPE_UINT64;
				}
				break;

				case IL_META_ELEMTYPE_R4:
				{
					stackBase->actualType = IL_TYPE_FLOAT32;
				}
				break;

				case IL_META_ELEMTYPE_R8:
				{
					stackBase->actualType = IL_TYPE_FLOAT64;
				}
				break;

				case IL_META_ELEMTYPE_I:
				{
					stackBase->actualType = IL_TYPE_NATIVE_INT;
				}
				break;

				case IL_META_ELEMTYPE_U:
				{
					stackBase->actualType = IL_TYPE_NATIVE_UINT;
				}
				break;

				case IL_META_ELEMTYPE_R:
				{
					stackBase->actualType = IL_TYPE_NATIVE_FLOAT;
				}
				break;

				case IL_META_ELEMTYPE_TYPEDBYREF:
				{
					stackBase->actualType = IL_TYPE_OBJECT_REF;
				}
				break;

				default:
				{
					stackBase->actualType = IL_TYPE_INT32;
				}
				break;
			}
		}
		else if(ILType_IsValueType(localType))
		{
			/* Managed value */
			stackBase->actualType = IL_TYPE_MANAGED_VALUE;
		}
		else
		{
			/* Everything else has a default value of "null" */
			stackBase->actualType = IL_TYPE_OBJECT_REF;
		}

		/* Advance to the next argument */
		++stackBase;
	}

	/* Initialize the local variables */
	localNum = 0;
	stackBase = thread->locals;
	while(localNum < numLocals)
	{
		/* Get the type of the next local variable */
		switch(localNum & 3)
		{
			case 0:
			{
				localType = localVarSig->un.locals.local[0];
				++localNum;
			}
			break;

			case 1:
			{
				localType = localVarSig->un.locals.local[1];
				++localNum;
			}
			break;

			case 2:
			{
				localType = localVarSig->un.locals.local[2];
				++localNum;
			}
			break;

			default:
			{
				localType = localVarSig->un.locals.local[3];
				localVarSig = localVarSig->un.locals.next;
				++localNum;
			}
			break;
		}

		/* Initialize the local variable according to its type */
		if(ILType_IsPrimitive(localType))
		{
			/* Primitive element type */
			switch(ILType_ToElement(localType))
			{
				case IL_META_ELEMTYPE_BOOLEAN:
				{
					stackBase->valueType = IL_TYPE_BOOLEAN;
					stackBase->un.i4Value = 0;
				}
				break;

				case IL_META_ELEMTYPE_CHAR:
				{
					stackBase->valueType = IL_TYPE_CHAR;
					stackBase->un.i4Value = 0;
				}
				break;

				case IL_META_ELEMTYPE_I1:
				{
					stackBase->valueType = IL_TYPE_INT8;
					stackBase->un.i4Value = 0;
				}
				break;

				case IL_META_ELEMTYPE_U1:
				{
					stackBase->valueType = IL_TYPE_UINT8;
					stackBase->un.i4Value = 0;
				}
				break;

				case IL_META_ELEMTYPE_I2:
				{
					stackBase->valueType = IL_TYPE_INT16;
					stackBase->un.i4Value = 0;
				}
				break;

				case IL_META_ELEMTYPE_U2:
				{
					stackBase->valueType = IL_TYPE_UINT16;
					stackBase->un.i4Value = 0;
				}
				break;

				case IL_META_ELEMTYPE_I4:
				{
					stackBase->valueType = IL_TYPE_INT32;
					stackBase->un.i4Value = 0;
				}
				break;

				case IL_META_ELEMTYPE_U4:
				{
					stackBase->valueType = IL_TYPE_UINT32;
					stackBase->un.i4Value = 0;
				}
				break;

				case IL_META_ELEMTYPE_I8:
				{
					stackBase->valueType = IL_TYPE_INT64;
					stackBase->un.i8Value = 0;
				}
				break;

				case IL_META_ELEMTYPE_U8:
				{
					stackBase->valueType = IL_TYPE_UINT64;
					stackBase->un.i8Value = 0;
				}
				break;

				case IL_META_ELEMTYPE_R4:
				{
					stackBase->valueType = IL_TYPE_FLOAT32;
					stackBase->un.fValue = 0.0;
				}
				break;

				case IL_META_ELEMTYPE_R8:
				{
					stackBase->valueType = IL_TYPE_FLOAT64;
					stackBase->un.fValue = 0.0;
				}
				break;

				case IL_META_ELEMTYPE_I:
				{
					stackBase->valueType = IL_TYPE_NATIVE_INT;
					stackBase->un.iValue = 0;
				}
				break;

				case IL_META_ELEMTYPE_U:
				{
					stackBase->valueType = IL_TYPE_NATIVE_UINT;
					stackBase->un.iValue = 0;
				}
				break;

				case IL_META_ELEMTYPE_R:
				{
					stackBase->valueType = IL_TYPE_NATIVE_FLOAT;
					stackBase->un.fValue = 0.0;
				}
				break;

				case IL_META_ELEMTYPE_TYPEDBYREF:
				{
					stackBase->valueType = IL_TYPE_OBJECT_REF;
					stackBase->un.oValue = 0;
				}
				break;

				default:
				{
					stackBase->valueType = IL_TYPE_INT32;
					stackBase->un.i4Value = 0;
				}
				break;
			}
		}
		else if(ILType_IsValueType(localType))
		{
			/* Allocate some heap space for a new value of this type */
			stackBase->valueType = IL_TYPE_MANAGED_VALUE;
			stackBase->un.oValue = 0;
		}
		else
		{
			/* Everything else has a default value of "null" */
			stackBase->valueType = IL_TYPE_OBJECT_REF;
			stackBase->un.oValue = 0;
		}
		stackBase->actualType = stackBase->valueType;

		/* Advance to the next local */
		++stackBase;
	}

	/* Set the program counter for the new frame.  This will be
	   set to NULL if the method is native, runtime, or PInvoke */
	thread->pc = (unsigned char *)(code.code);

	/* Done */
	return 1;
}

void _ILExecThreadFramePop(ILExecThread *thread)
{
	ILCallFrame *frame;
	ILValue *stackBase;
	ILValue *stackTop;
	int hasReturn;

	/* Does this method have a return value? */
	hasReturn = (thread->method->member.signature->un.method.retType
						!= ILType_Void);

	/* Find the base of the stack to adjust the saved frame values */
	stackBase = thread->stackBase;

	/* Remember the stack top in case we need to copy a return value */
	stackTop = thread->stackTop;

	/* Copy the contents of the frame to the thread state */
	frame = thread->frame;
	thread->pc = frame->pc;
	thread->stackTop = stackBase + frame->stackTop;
	thread->args = stackBase + frame->args;
	thread->locals = stackBase + frame->locals;
	thread->method = frame->method;

	/* Pop the frame */
	if(frame->parent != IL_MAX_UINT32)
	{
		thread->frame = (ILCallFrame *)(stackBase + frame->parent);
	}
	else
	{
		thread->frame = 0;
	}

	/* Copy the return value */
	if(hasReturn)
	{
		*(thread->stackTop) = stackTop[-1];
		++(thread->stackTop);
	}
}

ILMethod *ILExecThreadStackMethod(ILExecThread *thread, unsigned long num)
{
	ILCallFrame *frame;
	if(!num)
	{
		return thread->method;
	}
	frame = thread->frame;
	while(num > 1)
	{
		if(frame->parent != IL_MAX_UINT32)
		{
			thread->frame = (ILCallFrame *)(thread->stackBase + frame->parent);
		}
		else
		{
			return 0;
		}
		--num;
	}
	return frame->method;
}

#ifdef	__cplusplus
};
#endif
