/*
 * convert.c - Convert methods using a coder.
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

#include "engine_private.h"

#ifdef	__cplusplus
extern	"C" {
#endif

unsigned char *_ILConvertMethod(ILExecThread *thread, ILMethod *method)
{
	ILMethodCode code;
	ILPInvoke *pinv;
	ILCoder *coder = thread->process->coder;
	unsigned char *start;
	void *fn;
	void *cif;

	/* Is the method already converted and in the current generation? */
	if(method->userData1 != 0 &&
	   method->userData2 == (void *)(thread->process->coderGeneration))
	{
		return (unsigned char *)(method->userData1);
	}

	/* Get the method code */
	if(!ILMethodGetCode(method, &code))
	{
		return 0;
	}

	/* The conversion is different depending upon whether the
	   method is written in IL or not */
	if(code.code)
	{
		/* Use the bytecode verifier and coder to convert the method */
		if(!_ILVerify(coder, &start, method,
					  &code, 1 /* TODO: safe vs unsafe */))
		{
			return 0;
		}
		thread->process->coderGeneration = ILCoderGeneration(coder);
	}
	else
	{
		/* This is a "PInvoke", "internalcall", or "runtime" method */
		pinv = ILPInvokeFind(method);
		fn = 0;
		switch(method->implementAttrs &
					(IL_META_METHODIMPL_CODE_TYPE_MASK |
					 IL_META_METHODIMPL_INTERNAL_CALL |
					 IL_META_METHODIMPL_JAVA))
		{
			case IL_META_METHODIMPL_IL:
			case IL_META_METHODIMPL_OPTIL:
			{
				/* If we don't have a PInvoke record, then we don't
				   know what to map this method call to */
				if(!pinv)
				{
					return 0;
				}
			}
			break;

			case IL_META_METHODIMPL_RUNTIME:
			case IL_META_METHODIMPL_IL | IL_META_METHODIMPL_INTERNAL_CALL:
			{
				/* "internalcall" and "runtime" methods must not
				   have PInvoke records associated with them */
				if(pinv)
				{
					return 0;
				}
			}
			break;

			default:
			{
				/* No idea how to invoke this method */
				return 0;
			}
			/* Not reached */
		}

		/* Bail out if we did not find the underlying native method */
		if(!fn)
		{
			return 0;
		}

		/* Generate a "cif" structure to handle the native call details */
		cif = _ILMakeCifForMethod(coder, method, (pinv == 0));
		if(!cif)
		{
			/* The coder ran out of memory, so flush it and retry */
			ILCoderFlush(coder);
			thread->process->coderGeneration = ILCoderGeneration(coder);
			cif = _ILMakeCifForMethod(coder, method, (pinv == 0));
			if(!cif)
			{
				return 0;
			}
		}

		/* Generate the coder stub for marshalling the call */
		if(!ILCoderSetupExtern(coder, &start, method, fn, cif, (pinv == 0)))
		{
			return 0;
		}
		if(!ILCoderFinish(coder))
		{
			/* Do we need a coder restart due to cache overflow? */
			if(!ILCoderRestart(coder))
			{
				return 0;
			}
			thread->process->coderGeneration = ILCoderGeneration(coder);
			cif = _ILMakeCifForMethod(coder, method, (pinv == 0));
			if(!ILCoderSetupExtern(coder, &start, method,
								   fn, cif, (pinv == 0)))
			{
				return 0;
			}
			if(!ILCoderFinish(coder))
			{
				return 0;
			}
		}
	}

	/* The method is in the current generation now */
	method->userData1 = (void *)start;
	method->userData2 = (void *)(thread->process->coderGeneration);
	return (unsigned char *)(method->userData1);
}

#ifdef	__cplusplus
};
#endif
