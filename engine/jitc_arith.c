/*
 * jitc_arith.c - Coder implementation for JIT arithmetic operations.
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
 * Handle a binary opcode.
 */
static void JITCoder_Binary(ILCoder *coder, int opcode,
							ILEngineType type1, ILEngineType type2)
{
}

/*
 * Handle a binary opcode when pointer arithmetic is involved.
 */
static void JITCoder_BinaryPtr(ILCoder *coder, int opcode,
							   ILEngineType type1, ILEngineType type2)
{
}

/*
 * Handle a shift opcode.
 */
static void JITCoder_Shift(ILCoder *coder, int opcode,
						   ILEngineType type1, ILEngineType type2)
{
}

/*
 * Handle a unary opcode.
 */
static void JITCoder_Unary(ILCoder *coder, int opcode, ILEngineType type)
{
}

#endif	/* IL_JITC_CODE */
