/*
 * jitc_conv.c - Coder implementation for JIT conversions.
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
 * Handle a conversion opcode.
 */
static void JITCoder_Conv(ILCoder *coder, int opcode, ILEngineType type)
{
}

/*
 * Convert an I or I4 integer into a pointer.
 */
static void JITCoder_ToPointer(ILCoder *coder, ILEngineType type1,
							   ILEngineStackItem *type2)
{
}

/*
 * Output an instruction to convert the top of stack according
 * to a PInvoke marshalling rule.
 */
static void JITCoder_Convert(ILCoder *coder, int opcode)
{
}

/*
 * Output an instruction to convert the top of stack according
 * to a custom marshalling rule.
 */
static void JITCoder_ConvertCustom(ILCoder *coder, int opcode,
						    	   ILUInt32 customNameLen,
								   ILUInt32 customCookieLen,
						    	   void *customName, void *customCookie)
{
}

#endif	/* IL_JITC_CODE */
