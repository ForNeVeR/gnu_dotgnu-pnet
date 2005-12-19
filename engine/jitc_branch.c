/*
 * jitc_branch.c - Coder implementation for JIT branches.
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
 * Handle a label.
 */
static void JITCoder_Label(ILCoder *_coder, ILUInt32 offset)
{
}


/*
 * Output a branch instruction using a JIT coder.
 */
static void JITCoder_Branch(ILCoder *coder, int opcode, ILUInt32 dest,
				   		    ILEngineType type1, ILEngineType type2)
{
}

/*
 * Output the start of a table-based switch statement.
 */
static void JITCoder_SwitchStart(ILCoder *coder, ILUInt32 numEntries)
{
}

/*
 * Output an entry for a table-based switch statement.
 */
static void JITCoder_SwitchEntry(ILCoder *_coder, ILUInt32 dest)
{
}

/*
 * Output a comparison instruction.
 */
static void JITCoder_Compare(ILCoder *coder, int opcode,
				   		     ILEngineType type1, ILEngineType type2,
							 int invertTest)
{
}

#endif	/* IL_JITC_CODE */
