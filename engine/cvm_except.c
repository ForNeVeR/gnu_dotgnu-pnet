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

/*
 * Create a system exception object of a particular class.
 * We do this very carefully, to avoid re-entering the engine.
 * We cannot call the exception's constructor, so we do the
 * equivalent work here instead.
 */
static void *SystemException(ILExecThread *thread, const char *className)
{
	ILObject *object;
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
		/* TODO: initialize the stack trace within the exception object */
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

case COP_JSR:
{
	/* Jump to a subroutine within this method.  The address
	   is saved as an offset because the subroutine may call
	   other methods, which may cause the CVM cache to be
	   flushed.  When the subroutine finally returns, this
	   method may no longer be located where it used to be */
	stacktop[0].intValue = (ILInt32)((pc + 6) - pcstart);
	pc += (ILInt32)(ILInt8)(pc[1]);
	stacktop += 1;
}
break;

case COP_RET_JSR:
{
	/* Return from a subroutine within this method */
	pc = pcstart + stacktop[-1].intValue;
	stacktop -= 1;
}
break;

#elif defined(IL_CVM_PREFIX)

case COP_PREFIX_ENTER_TRY:
{
	/* Enter a try context for this method */
	thread->except = (ILUInt32)((pc + IL_READ_INT32(pc + 2)) - pcstart);
	thread->exceptHeight = (ILUInt32)(stacktop - frame);
	MODIFY_PC_AND_STACK(6, 0);
}
break;

/* Label that we jump to when the engine throws an internal exception */
throwException:
{
	if(thread->except == IL_MAX_UINT32)
	{
		goto throwCaller;
	}
}
/* Fall through */

case COP_PREFIX_THROW:
{
	/* Move the exception object down the stack to just above the locals */
	frame[thread->exceptHeight].ptrValue = stacktop[-1].ptrValue;
	stacktop = frame + thread->exceptHeight + 1;

	/* Search the exception handler table for an applicable handler */
searchForHandler:
	tempNum = (ILUInt32)(pc - pcstart);
	pc = pcstart + thread->except;
	while(tempNum < IL_READ_UINT32(pc) || tempNum >= IL_READ_UINT32(pc + 4))
	{
		pc += IL_READ_UINT32(pc + 8);
	}
	pc += 12;
}
break;

case COP_PREFIX_THROW_CALLER:
{
	/* Throw an exception to the caller of this method */
throwCaller:
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
		if(methodToCall)
		{
			/* Returning to within the context of a CVM method */
			if(methodToCall->userData1 == 0 ||
			   method->userData2 != methodToCall->userData2)
			{
				/* We need to re-convert the method because it was flushed */
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
		thread->except = callFrame->except;
		thread->exceptHeight = callFrame->exceptHeight;
		frame = stackbottom + callFrame->frame;
		method = methodToCall;

		/* Should we return to an external method? */
		if(callFrame->pc == IL_MAX_UINT32)
		{
			thread->thrownException = tempptr;
			COPY_STATE_TO_THREAD();
			return 1;
		}
	}
	while(thread->except == IL_MAX_UINT32);

	/* Copy the exception object into place */
	stacktop = frame + thread->exceptHeight;
	stacktop[0].ptrValue = tempptr;
	++stacktop;

	/* Search for an exception handler within this method */
	goto searchForHandler;
}
/* Not reached */

#endif /* IL_CVM_PREFIX */
