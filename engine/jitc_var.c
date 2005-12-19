/*
 * jitc_var.c - Coder implementation for JIT variables.
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
 * Handle a load from an argument.
 */
static void JITCoder_LoadArg(ILCoder *coder, ILUInt32 argNum, ILType *type)
{
}

/*
 * Handle a load from a local variable.
 */
static void JITCoder_LoadLocal(ILCoder *coder, ILUInt32 localNum, ILType *type)
{
}

/*
 * Handle a store to an argument.
 */
static void JITCoder_StoreArg(ILCoder *coder, ILUInt32 argNum,
							  ILEngineType engineType, ILType *type)
{
}

/*
 * Handle a store to a local variable.
 */
static void JITCoder_StoreLocal(ILCoder *coder, ILUInt32 localNum,
								ILEngineType engineType, ILType *type)
{
}

/*
 * Load the address of an argument onto the stack.
 */
static void JITCoder_AddrOfArg(ILCoder *coder, ILUInt32 argNum)
{
}

/*
 * Load the address of a local onto the stack.
 */
static void JITCoder_AddrOfLocal(ILCoder *coder, ILUInt32 localNum)
{
}

/*
 * Allocate local stack space.
 */
static void JITCoder_LocalAlloc(ILCoder *coder, ILEngineType sizeType)
{
}

#endif	/* IL_JITC_CODE */
