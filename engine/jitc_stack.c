/*
 * jitc_stack.c - Coder implementation for JIT stack operations.
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

#ifdef	IL_JITC_DECLARATIONS

#ifdef _IL_JIT_OPTIMIZE_LOCALS

typedef struct _tagILJitStackItem ILJitStackItem;
struct _tagILJitStackItem
{
	ILInt32			flags;
	ILJitValue		value;
	ILJitLocalSlot *refValue;
};

#define ILJitStackItemNew(name) ILJitStackItem name

#else

typedef ILJitValue ILJitStackItem;

#define ILJitStackItemNew(name) ILJitStackItem name = 0

#endif	/* _IL_JIT_OPTIMIZE_LOCALS */

#endif	/* IL_JITC_DECLARATIONS */

#ifdef	IL_JITC_CODER_INSTANCE

	/* Members to manage the evaluation stack. */
	ILJitValue	   *jitStack;
	int				stackSize;
	int				stackTop;

#endif

#ifdef	IL_JITC_CODER_INIT

	/* Initialize the stack management. */
	coder->jitStack = 0;
	coder->stackTop = -1;
	coder->stackSize = 0;

#endif	/* IL_JITC_CODER_INIT */

#ifdef	IL_JITC_CODER_DESTROY

	if(coder->jitStack)
	{
		ILFree(coder->jitStack);
		coder->jitStack = 0;
	}

#endif	/* IL_JITC_CODER_DESTROY */

#ifdef	IL_JITC_FUNCTIONS

/*
 * Allocate enough space for "n" values on the stack.
 */
#define	ALLOC_STACK(coder, n)	\
	do { \
		if((n) > (coder)->stackSize) \
		{ \
			ILJitValue *newStack = \
				(ILJitValue *)ILRealloc((coder)->jitStack, \
									  (n) * sizeof(ILJitValue)); \
			if(!newStack) \
			{ \
				return 0; \
			} \
			(coder)->jitStack = newStack; \
			(coder)->stackSize = (n); \
		} \
	} while (0)

#define _JITC_ADJUST(coder, num) \
	do { \
		(coder)->stackTop += (num); \
	} while(0)

#define JITC_ADJUST(coder, num) _JITC_ADJUST((coder), (num))

#endif

#ifdef IL_JITC_CODE

/*
 * Refresh the coder's notion of the stack contents.
 */
static void JITCoder_StackRefresh(ILCoder *coder, ILEngineStackItem *stack,
							      ILUInt32 stackSize)
{
	ILJITCoder *jitCoder = _ILCoderToILJITCoder(coder);

#if !defined(IL_CONFIG_REDUCE_CODE) && !defined(IL_WITHOUT_TOOLS)
	if (jitCoder->flags & IL_CODER_FLAG_STATS)
	{
		ILMutexLock(globalTraceMutex);
		fprintf(stdout,
			"StackRefresh: %i\n", 
			stackSize);
		ILMutexUnlock(globalTraceMutex);
	}
#endif
	jitCoder->stackTop = stackSize;
}

/*
 * Duplicate the top element on the stack.
 */
static void JITCoder_Dup(ILCoder *coder, ILEngineType engineType, ILType *type)
{
	ILJITCoder *jitCoder = _ILCoderToILJITCoder(coder);

#if !defined(IL_CONFIG_REDUCE_CODE) && !defined(IL_WITHOUT_TOOLS)
	if (jitCoder->flags & IL_CODER_FLAG_STATS)
	{
		ILMutexLock(globalTraceMutex);
		fprintf(stdout,
			"Dup: \n");
		ILMutexUnlock(globalTraceMutex);
	}
#endif
#ifdef _IL_JIT_OPTIMIZE_LOCALS
	jitCoder->jitStack[jitCoder->stackTop] = 
					 jitCoder->jitStack[jitCoder->stackTop - 1];
#else
	jitCoder->jitStack[jitCoder->stackTop] = 
		jit_insn_dup(jitCoder->jitFunction,
					 jitCoder->jitStack[jitCoder->stackTop - 1]);
#endif
	JITC_ADJUST(jitCoder, 1);
}

/*
 * Pop the top element on the stack.
 */
static void JITCoder_Pop(ILCoder *coder, ILEngineType engineType, ILType *type)
{
	ILJITCoder *jitCoder = _ILCoderToILJITCoder(coder);

	JITC_ADJUST(jitCoder, -1);
}

#endif	/* IL_JITC_CODE */
