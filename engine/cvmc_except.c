/*
 * cvmc_except.c - Coder implementation for CVM exceptions.
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
#include "cvm_format.h"

#ifdef IL_CVMC_CODE

/*
 * Set up exception handling for the current method.
 */
static void CVMCoder_SetupExceptions(ILCoder *_coder, ILException *exceptions,
									 int hasRethrow)
{
	ILCVMCoder *coder = (ILCVMCoder *)_coder;
	ILUInt32 extraLocals;

/* If the method uses "rethrow", then we need to allocate local
	   variables for each of the "catch" blocks, to hold the exception
	   object temporarily prior to the "rethrow" */
	if(hasRethrow)
	{
		extraLocals = 0;
		while(exceptions != 0)
		{
			if((exceptions->flags & (IL_META_EXCEPTION_FINALLY |
									 IL_META_EXCEPTION_FAULT |
									 IL_META_EXCEPTION_FILTER)) == 0)
			{
				exceptions->userData = extraLocals + coder->minHeight;

				++extraLocals;
			}
			exceptions = exceptions->next;
		}
		if(extraLocals == 1)
		{
			CVM_OUT_NONE(COP_MK_LOCAL_1);
		}
		else if(extraLocals == 2)
		{
			CVM_OUT_NONE(COP_MK_LOCAL_2);
		}
		else if(extraLocals == 3)
		{
			CVM_OUT_NONE(COP_MK_LOCAL_3);
		}
		else if(extraLocals != 0)
		{
			CVM_OUT_WIDE(COP_MK_LOCAL_N, extraLocals);
		}
		coder->height += extraLocals;
		coder->minHeight += extraLocals;
		coder->maxHeight += extraLocals;
	}

	/* Start the method's primary exception region here */
	ILCacheNewRegion(&(coder->codePosn), (void *)0);

	/* Set up the method's frame to perform exception handling */
	coder->needTry = 1;
	CVMP_OUT_NONE(COP_PREFIX_ENTER_TRY);
}

/*
 * Output a throw instruction.
 */
static void CVMCoder_Throw(ILCoder *coder, int inCurrentMethod)
{
	if(inCurrentMethod == 1)
	{
		CVMP_OUT_WORD(COP_PREFIX_THROW, 1);
	}
	else
	{
		CVMP_OUT_NONE(COP_PREFIX_THROW_CALLER);
	}
	CVM_ADJUST(-1);
}

/*
 * Output a stacktrace instruction.
 */
static void CVMCoder_SetStackTrace(ILCoder *coder)
{
	CVMP_OUT_NONE(COP_PREFIX_SET_STACK_TRACE);
}

/*
 * Output a rethrow instruction.
 */
static void CVMCoder_Rethrow(ILCoder *coder, ILException *exception)
{
	/* Push the saved exception object back onto the stack */
	CVM_OUT_WIDE(COP_PLOAD, exception->userData);
	CVM_ADJUST(1);

	/* Throw the object to this method's exception handler table */
	CVMP_OUT_NONE(COP_PREFIX_THROW);
	CVM_ADJUST(-1);
}

/*
 * Output a "jump to subroutine" instruction.
 */
static void CVMCoder_Jsr(ILCoder *coder, ILUInt32 dest)
{
	OutputBranch(coder, COP_JSR, dest);
}

/*
 * Output a "return from subroutine" instruction.
 */
static void CVMCoder_RetFromJsr(ILCoder *coder)
{
	CVM_OUT_NONE(COP_RET_JSR);
}

/*
 * Start a "try" handler block for a region of code.
 */
static void CVMCoder_TryHandlerStart(ILCoder *_coder,
									 ILUInt32 start, ILUInt32 end)
{
	ILCVMCoder *coder = (ILCVMCoder *)_coder;
	ILCVMLabel *label;
	
	/* End the exception region if this is the first try block */
	if(coder->needTry)
	{
		/* Don't need to do this again */
		coder->needTry = 0;

		/* Set the cookie for the method's exception region
		   to the current method position */
		ILCacheSetCookie(&(coder->codePosn), CVM_POSN());

		/* End the exception region and start a normal region */
		ILCacheNewRegion(&(coder->codePosn), 0);
	}

	/* Output the start and end of the code covered by the handler */
	coder->tryHandler = CVM_POSN();
	if(start == 0 && end == IL_MAX_UINT32)
	{
		/* This handler is the last one in the table */
		CVM_OUT_TRY(start, end);
	}
	else
	{
		/* Convert the IL offsets into CVM offsets.  We assume that
		   the labels were created previously while generating the
		   code for the body of the method */
		label = GetLabel(coder, start);
		if(!label)
		{
			return;
		}
		start = label->offset;
		label = GetLabel(coder, end);
		if(!label)
		{
			return;
		}
		end = label->offset;
		CVM_OUT_TRY(start, end);
	}
}

/*
 * End a "try" handler block for a region of code.
 */
static void CVMCoder_TryHandlerEnd(ILCoder *coder)
{
	/* Back-patch the length value for the handler */	
	CVM_BACKPATCH_TRY(((ILCVMCoder *)coder)->tryHandler);	
}

/*
 * Output instructions to match a "catch" clause.
 */
static void CVMCoder_Catch(ILCoder *_coder, ILException *exception,
						   ILClass *classInfo, int hasRethrow)
{
	ILCVMCoder *coder = (ILCVMCoder *)_coder;
	unsigned char *temp;

	/* Duplicate the exception object */
	CVM_OUT_NONE(COP_DUP);
	CVM_ADJUST(1);

	/* Determine if the object is an instance of the right class */
	CVM_OUT_PTR(COP_ISINST, classInfo);

	/* Branch to the next test if not an instance */
	temp = CVM_POSN();
	CVM_OUT_BRANCH_PLACEHOLDER(COP_BRNULL);

	/* If the method contains "rethrow" instructions, then save
	   the object into a temporary local for this "catch" clause */
	if(hasRethrow)
	{
		CVM_OUT_NONE(COP_DUP);
		CVM_OUT_WIDE(COP_PSTORE, exception->userData);
	}

	CVMP_OUT_PTR(COP_PREFIX_START_CATCH, exception->ptrUserData);
	
	/* Branch to the start of the "catch" clause */
	OutputBranch(_coder, COP_BR, exception->handlerOffset);

	/* Back-patch the "brnull" instruction */
	CVM_BACKPATCH_BRANCH(temp, (ILInt32)(CVM_POSN() - temp));

	/* Adjust the stack back to its original height */
	CVM_ADJUST(-1);
}

static void CVMCoder_EndCatchFinally(ILCoder *coder, ILException *exception)
{
	exception->ptrUserData = CVM_POSN();	
	CVMP_OUT_NONE(COP_PREFIX_PROPAGATE_ABORT);
}

static void CVMCoder_Finally(ILCoder *coder, ILException *exception, int dest)
{
	CVMP_OUT_PTR(COP_PREFIX_START_FINALLY, exception->ptrUserData);
	OutputBranch(coder, COP_JSR, dest);
}

/*
 * Convert a program counter into an exception handler.
 */
static void *CVMCoder_PCToHandler(ILCoder *_coder, void *pc, int beyond)
{
	void *cookie;
	if(beyond)
	{
		pc = ILCacheRetAddrToPC(pc);
	}
	if(ILCacheGetMethod(((ILCVMCoder *)_coder)->cache, pc, &cookie))
	{
		return cookie;
	}
	else
	{
		return 0;
	}
}

/*
 * Convert a program counter into a method descriptor.
 */
static ILMethod *CVMCoder_PCToMethod(ILCoder *_coder, void *pc, int beyond)
{
	if(beyond)
	{
		pc = ILCacheRetAddrToPC(pc);
	}
	return ILCacheGetMethod(((ILCVMCoder *)_coder)->cache, pc, (void **)0);
}

#endif	/* IL_CVMC_CODE */
