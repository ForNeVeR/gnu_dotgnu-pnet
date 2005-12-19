/*
 * jitc_except.c - Coder implementation for JIT exceptions.
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
 * Set up exception handling for the current method.
 */
static void JITCoder_SetupExceptions(ILCoder *_coder, ILException *exceptions,
									 int hasRethrow)
{
}

/*
 * Output a throw instruction.
 */
static void JITCoder_Throw(ILCoder *coder, int inCurrentMethod)
{
}

/*
 * Output a stacktrace instruction.
 */
static void JITCoder_SetStackTrace(ILCoder *coder)
{
}

/*
 * Output a rethrow instruction.
 */
static void JITCoder_Rethrow(ILCoder *coder, ILException *exception)
{
}

/*
 * Output a "jump to subroutine" instruction.
 */
static void JITCoder_Jsr(ILCoder *coder, ILUInt32 dest)
{
}

/*
 * Output a "return from subroutine" instruction.
 */
static void JITCoder_RetFromJsr(ILCoder *coder)
{
}

/*
 * Start a "try" handler block for a region of code.
 */
static void JITCoder_TryHandlerStart(ILCoder *_coder,
									 ILUInt32 start, ILUInt32 end)
{
}

/*
 * End a "try" handler block for a region of code.
 */
static void JITCoder_TryHandlerEnd(ILCoder *coder)
{
}

/*
 * Output instructions to match a "catch" clause.
 */
static void JITCoder_Catch(ILCoder *_coder, ILException *exception,
						   ILClass *classInfo, int hasRethrow)
{
}

static void JITCoder_EndCatchFinally(ILCoder *coder, ILException *exception)
{
}

static void JITCoder_Finally(ILCoder *coder, ILException *exception, int dest)
{
}

/*
 * Convert a program counter into an exception handler.
 */
static void *JITCoder_PCToHandler(ILCoder *_coder, void *pc, int beyond)
{
	return 0;
}

/*
 * Convert a program counter into a method descriptor.
 */
static ILMethod *JITCoder_PCToMethod(ILCoder *_coder, void *pc, int beyond)
{
	return 0;
}

#endif	/* IL_JITC_CODE */
