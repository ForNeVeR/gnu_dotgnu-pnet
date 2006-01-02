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

#ifdef IL_JITC_CODE

/*
 * Refresh the coder's notion of the stack contents.
 */
static void JITCoder_StackRefresh(ILCoder *coder, ILEngineStackItem *stack,
							      ILUInt32 stackSize)
{
	ILJITCoder *jitCoder = _ILCoderToILJITCoder(coder);

	jitCoder->stackTop = stackSize;
}

/*
 * Duplicate the top element on the stack.
 */
static void JITCoder_Dup(ILCoder *coder, ILEngineType engineType, ILType *type)
{
	ILJITCoder *jitCoder = _ILCoderToILJITCoder(coder);

	jitCoder->jitStack[jitCoder->stackTop] = 
		jitCoder->jitStack[jitCoder->stackTop - 1];
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
