/*
 * jitc_ptr.c - Coder implementation for JIT pointers and arrays.
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
 * Handle an array access opcode.
 */
static void JITCoder_ArrayAccess(ILCoder *coder, int opcode,
								 ILEngineType indexType, ILType *elemType)
{
}

/*
 * Handle a pointer indirection opcode.
 */
static void JITCoder_PtrAccess(ILCoder *coder, int opcode)
{
}

/*
 * Handle a pointer indirection opcode for a managed value.
 */
static void JITCoder_PtrAccessManaged(ILCoder *coder, int opcode,
									  ILClass *classInfo)
{
}

/*
 * Process a pointer alignment prefix.  An "alignment" value
 * of zero indicates a "volatile" prefix.
 */
static void JITCoder_PtrPrefix(ILCoder *coder, int alignment)
{
}

/*
 * Get the length of an array.
 */
static void JITCoder_ArrayLength(ILCoder *coder)
{
}

/*
 * Construct a new array, given a type and length value.
 */
static void JITCoder_NewArray(ILCoder *coder, ILType *arrayType,
					 		  ILClass *arrayClass, ILEngineType lengthType)
{
}

/*
 * Check the top of stack value for NULL.
 */
static void JITCoder_CheckNull(ILCoder *coder)
{
}

#endif	/* IL_JITC_CODE */
