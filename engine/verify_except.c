/*
 * verify_except.c - Verify instructions related to exceptions.
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

#if defined(IL_VERIFY_GLOBALS)

/*
 * Output a table of exception matching directives.
 * Each table entry specifies a region of code for the
 * directive.  Whenever an exception occurs in this
 * region, the method will jump to the instructions
 * contained in the table entry.  These instructions
 * will typically call "finally" handlers, and then
 * attempt to match the exception against the rules.
 */
static void OutputExceptionTable(ILCoder *coder, ILMethod *method,
								 ILException *exceptions, int hasRethrow)
{
	ILUInt32 offset;
	ILUInt32 end;
	int isStatic;
	ILException *exception;
	ILClass *classInfo;
	
	/* Process all regions in the method */
	offset = 0;
	for(;;)
	{
		/* Find the end of the region that starts at "offset" */
		end = IL_MAX_UINT32;
		exception = exceptions;
		while(exception != 0)
		{
			if(offset < exception->tryOffset)
			{
				/* We are in the code before this exception region */
				if(end > exception->tryOffset)
				{
					end = exception->tryOffset;
				}
			}
			else if(offset >= exception->tryOffset &&
			        offset < (exception->tryOffset + exception->tryLength))
			{
				/* We are in code in the middle of this exception region */
				if(end > (exception->tryOffset + exception->tryLength))
				{
					end = exception->tryOffset + exception->tryLength;
				}
			}
			exception = exception->next;
		}
		if(end == IL_MAX_UINT32)
		{
			break;
		}

		/* Output the region information to the table */
		ILCoderTryHandlerStart(coder, offset, end);

		/* Output exception matching code for this region */
		exception = exceptions;
		while(exception != 0)
		{
			if(offset >= exception->tryOffset &&
			   offset < (exception->tryOffset + exception->tryLength))
			{
				if((exception->flags & (IL_META_EXCEPTION_FINALLY |
										IL_META_EXCEPTION_FAULT)) != 0)
				{
					/* Call a "finally" or "fault" clause */
					ILCoderFinally(coder, exception, exception->handlerOffset);					
				}
				else if((exception->flags & IL_META_EXCEPTION_FILTER) == 0)
				{
					/* Match against a "catch" clause */
					classInfo = ILProgramItemToClass
						((ILProgramItem *)ILImageTokenInfo
							(ILProgramItem_Image(method), exception->extraArg));

					ILCoderCatch(coder, exception, classInfo, hasRethrow);
				}
				else
				{
					/* TODO: handle a "filter" clause */
				}
			}
			exception = exception->next;
		}

		/* If execution falls off the end of the matching code,
		   then throw the exception to the calling method */
		ILCoderThrow(coder, 0);

		/* Mark the end of the handler */
		ILCoderTryHandlerEnd(coder);

		/* Advance to the next region within the code */
		offset = end;
	}

	/* If execution gets here, then there were no applicable catch blocks,
	   so we always throw the exception to the calling method */
	ILCoderTryHandlerStart(coder, 0, IL_MAX_UINT32);
	
	if (ILMethod_IsSynchronized(method))
	{
		/* Exit the sync lock before throwing to the calling method */
		isStatic = ILMethod_IsStatic(method);
		PUSH_SYNC_OBJECT();
		ILCoderCallInlineable(coder, IL_INLINEMETHOD_MONITOR_EXIT, 0);
	}

	ILCoderThrow(coder, 0);
	ILCoderTryHandlerEnd(coder);
}

/*
 * Determine if an offset is inside an exception handler's "try" range.
 */
static IL_INLINE int InsideExceptionBlock(ILException *exception,
										  ILUInt32 offset)
{
	if(offset >= exception->tryOffset &&
	   offset < (exception->tryOffset + exception->tryLength))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

/*
 * Determine if an offset is inside an exception handler's "handle it" range.
 */
static IL_INLINE int InsideExceptionHandler(ILException *exception,
										    ILUInt32 offset)
{
	if(offset >= exception->handlerOffset &&
	   offset < (exception->handlerOffset + exception->handlerLength))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

/*
 * Emit code to throw a system-level exception.
 */
static void _ThrowSystem(ILCoder *coder, ILMethod *method, int hasExceptions,
						 const char *name, const char *namespace)
{
	ILClass *classInfo;
	ILMethod *ctor;
	ILCoderMethodInfo callInfo;

	/* Find the no-argument constructor for the class */
	classInfo = ILClassResolveSystem(ILProgramItem_Image(method), 0,
								     name, namespace);
	if(!classInfo)
	{
		return;
	}
	ctor = 0;
	while((ctor = (ILMethod *)ILClassNextMemberByKind
			(classInfo, (ILMember *)ctor, IL_META_MEMBERKIND_METHOD)) != 0)
	{
		if(ILMethod_IsConstructor(ctor) &&
		   ILTypeNumParams(ILMethod_Signature(ctor)) == 0)
		{
			break;
		}
	}
	if(!ctor)
	{
		return;
	}

	/* Invoke the constructor */
	callInfo.args = 0;
	callInfo.numBaseArgs = 0;
	callInfo.numVarArgs = 0;
	callInfo.hasParamArray = 0;
	ILCoderCallCtor(coder, &callInfo, ctor);

	/* Set the stack trace & throw the object */
	ILCoderSetStackTrace(coder);
	ILCoderThrow(coder, hasExceptions);
}
#define	ThrowSystem(namespace,name)	\
			_ThrowSystem(coder, method, (exceptions != 0), (name), (namespace))

#elif defined(IL_VERIFY_LOCALS)

/* No locals required */

#else /* IL_VERIFY_CODE */

case IL_OP_THROW:
{
	/* Throw an exception */
	if(stackSize >= 1 && stack[stackSize - 1].engineType == ILEngineType_O)
	{
		/* If the current method has exception handlers, then throw
		   the object to those handlers.  Otherwise throw directly
		   to the calling method */
		ILCoderSetStackTrace(coder);
		if (exceptions)
		{
			/* Throw to the exception table */
			ILCoderThrow(coder, 1);
		}
		else
		{
			/* If throwing to the calling method then make sure we exit the sync lock */

			if (isSynchronized)
			{
				PUSH_SYNC_OBJECT();
				ILCoderCallInlineable(coder, IL_INLINEMETHOD_MONITOR_EXIT, 0);
			}

			ILCoderThrow(coder, 0);
		}
		stackSize = 0;
		lastWasJump = 1;
	}
	else
	{
		VERIFY_TYPE_ERROR();
	}
}
break;

case IL_OP_PREFIX + IL_PREFIX_OP_RETHROW:
{
	/* Re-throw the current exception */
	exception = exceptions;
	while(exception != 0)
	{
		if((exception->flags & (IL_META_EXCEPTION_FINALLY |
								IL_META_EXCEPTION_FAULT |
								IL_META_EXCEPTION_FILTER)) == 0 &&
		   InsideExceptionHandler(exception, offset))
		{
			break;
		}
		exception = exception->next;
	}
	if(exception != 0)
	{
		ILCoderRethrow(coder, exception);
		lastWasJump = 1;
	}
	else
	{
		VERIFY_INSN_ERROR();
	}
}
break;

case IL_OP_ENDFINALLY:
{
	/* End the current "finally" or "fault" clause */
	if(stackSize == 0)
	{
		currentException = 0;
		
		exception = exceptions;

		while(exception != 0)
		{
			if (offset >= exception->handlerOffset 
				&& offset <= (exception->handlerOffset + exception->handlerLength))
			{
				if (exception->flags & IL_META_EXCEPTION_FINALLY)
				{
					/* This is a current exception clause that's leaving. */
					currentException = exception;

					break;
				}
			}

			exception = exception->next;
		}

		if (currentException == 0)
		{
			VERIFY_BRANCH_ERROR();
		}
		
		ILCoderEndCatchFinally(coder, currentException);
		ILCoderRetFromJsr(coder);
		lastWasJump = 1;
	}
	else
	{
		VERIFY_STACK_ERROR();
	}
}
break;

case IL_OP_PREFIX + IL_PREFIX_OP_ENDFILTER:
{
	/* End the current "filter" clause */
	/* TODO */
	lastWasJump = 1;
}
break;

case IL_OP_LEAVE_S:
{
	/* Unconditional short branch out of an exception block */
	dest = GET_SHORT_DEST();
processLeave:
	
	currentException = 0;
	
	/* The stack must be empty when we leave the block */
	while(stackSize)
	{
		/* Pop the current top of stack */
		ILCoderPop(coder, stack[stackSize -1].engineType,
			   stack[stackSize -1].typeInfo);
		stackSize--;
	}

	/* Find the handler for this "leave" instruction*/
	exception = exceptions;
	while(exception != 0)
	{
		if (offset >= exception->handlerOffset 
			&& offset < (exception->handlerOffset + exception->handlerLength))
		{
			if (exception->flags == IL_META_EXCEPTION_FINALLY 
				|| exceptions->flags == IL_META_EXCEPTION_CATCH)
			{
				currentException = exception;				
			}			
		}
		
		exception = exception->next;
	}
	
	/* Call any applicable "finally" handlers, but not "fault" handlers */
	exception = exceptions;
	while(exception != 0)
	{		
		if((exception->flags & IL_META_EXCEPTION_FINALLY) != 0 &&
		   InsideExceptionBlock(exception, offset) &&
		   !InsideExceptionBlock(exception, dest))
		{
			/* Call the "finally" clause for exiting this level */
			ILCoderFinally(coder, exception, exception->handlerOffset);
		}

		exception = exception->next;
	}

	if (currentException)
	{
		ILCoderEndCatchFinally(coder, currentException);
	}
	
	/* Output the branch instruction */
	ILCoderBranch(coder, opcode, dest, ILEngineType_I4, ILEngineType_I4);
	VALIDATE_BRANCH_STACK(dest);
	lastWasJump = 1;
}
break;

case IL_OP_LEAVE:
{
	/* Unconditional long branch out of an exception block */
	dest = GET_LONG_DEST();
	goto processLeave;
}
/* Not reached */

#endif /* IL_VERIFY_CODE */
