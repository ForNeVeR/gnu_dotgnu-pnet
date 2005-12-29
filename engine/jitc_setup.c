/*
 * jitc_setup.c - Coder implementation for JIT method entry setup.
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

#ifdef IL_JITC_CODE

/*
 * Allocate enough space for "n" locals.
 */
#define	ALLOC_LOCALS(n)	\
			do { \
				ILUInt32 temp = (ILUInt32)(((n) + 7) & ~7); \
				if(temp > coder->maxLocals) \
				{ \
					jit_value_t *newLocals = \
						(jit_value_t *)ILRealloc(coder->jitLocals, \
											  temp * sizeof(jit_value_t)); \
					if(!newLocals) \
					{ \
						return 0; \
					} \
					coder->jitLocals = newLocals; \
					coder->maxLocals = temp; \
				} \
			} while (0)


/*
 * Create the jit vars for the declared local variables.
 * Returns zero if out of memory.
 */
static int _JITCreateLocals(ILJITCoder *coder, ILStandAloneSig *localVarSig)
{
	ILType *signature;
	ILType *type;
	ILJitType jitType;
	ILUInt32 num;
	ILUInt32 current;

	if(localVarSig)
	{
		/* Determine the number of locals to allocate */
		signature = ILStandAloneSigGetType(localVarSig);
		num = ILTypeNumLocals(signature);

		/* Allocate the "jitLocals" array for the variables */
		ALLOC_LOCALS(num);

		/* Set the offsets for each of the local variables */
		for(current = 0; current < num; ++current)
		{
			type = ILTypeGetLocal(signature, current);
			if(!(jitType = _ILJitGetLocalsType(type, coder->process)))
			{
				return 0;
			}
			if(!(coder->jitLocals[current] = jit_value_create(coder->jitFunction, jitType)))
			{
				return 0;
			}
		}
		/* Record the number of used locals in the coder. */
		coder->numLocals = num;
	}
	else
	{
		/* Set the number of used locals to 0. */
		coder->numLocals = 0;
	}

	return 1;
}

/*
 * Set up a JIT coder instance to process a specific method.
 */
static int JITCoder_Setup(ILCoder *_coder, unsigned char **start,
						  ILMethod *method, ILMethodCode *code)
{
	ILJITCoder *coder = ((ILJITCoder *)_coder);

	/* Record the current jitted function. */
	coder->jitFunction = (ILJitFunction)(method->userData);

	/* Create the local variables. */
	if(!_JITCreateLocals(coder, code->localVarSig))
	{
		return 0;
	}

	/* Ensure that the evaluation stack can hold at least the methods maxStack */
	/* items. */
	if(code->maxStack > 0)
	{
		ALLOC_STACK(coder, code->maxStack);
	}
	
	/* And reset the stack top. */
	coder->stackTop = 0;

	return 1;
}

/*
 * Set up a JIT coder instance to process a specific external method.
 */
static int JITCoder_SetupExtern(ILCoder *_coder, unsigned char **start,
								ILMethod *method, void *fn, void *cif,
								int isInternal)
{
	return 1;
}

/*
 * Set up a JIT coder instance to process a specific external constructor.
 */
static int JITCoder_SetupExternCtor(ILCoder *_coder, unsigned char **start,
								    ILMethod *method, void *fn, void *cif,
								    void *ctorfn, void *ctorcif,
									int isInternal)
{
	return 1;
}

/*
 * Get the offset of an allocation constructor entry point
 * relative to the main method entry point.
 */
static int JITCoder_CtorOffset(ILCoder *coder)
{
	return 0;
}

/*
 * Finish processing a method using a JIT coder instance.
 */
static int JITCoder_Finish(ILCoder *_coder)
{
	return IL_CODER_END_OK;
}

/*
 * Set the flags for profiling debugging etc.
 */
static void JITCoder_SetFlags(ILCoder *_coder,int flags)
{
	(_ILCoderToILJITCoder(_coder))->flags = flags;
}

static int JITCoder_GetFlags(ILCoder *_coder)
{
	return (_ILCoderToILJITCoder(_coder))->flags;
}

/*
 * Mark the end of a method's bytecode, just prior to the exception tables.
 */
static void JITCoder_MarkEnd(ILCoder *coder)
{
}

/*
 * Allocate an extra local variable in the current method frame.
 * Returns the local variable index.
 */
static ILUInt32 JITCoder_AllocExtraLocal(ILCoder *coder, ILType *type)
{
	/* TODO */
	return 0;
}

/*
 * Push a thread value onto the stack for an internalcall.
 */
static void JITCoder_PushThread(ILCoder *_coder, int useRawCalls)
{
}

/*
 * Load the address of an argument onto the native argument stack.
 */
static void JITCoder_LoadNativeArgAddr(ILCoder *_coder, ILUInt32 num)
{
}

/*
 * Load the address of a local onto the native argument stack.
 */
static void JITCoder_LoadNativeLocalAddr(ILCoder *_coder, ILUInt32 num)
{
}

/*
 * Start pushing arguments for a "libffi" call onto the stack.
 */
static void JITCoder_StartFfiArgs(ILCoder *_coder)
{
}

/*
 * Push the address of the raw argument block onto the stack.
 */
static void JITCoder_PushRawArgPointer(ILCoder *_coder)
{
}

/*
 * Perform a function call using "libffi".
 */
static void JITCoder_CallFfi(ILCoder *coder, void *fn, void *cif,
					  		 int useRawCalls, int hasReturn)
{
}

#endif	/* IL_JITC_CODE */
