/*
 * cvm_except.c - Opcodes for handling exceptions.
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

#if defined(IL_CVM_GLOBALS)

/* No globals required */

#elif defined(IL_CVM_LOCALS)

/* No locals required */

#elif defined(IL_CVM_MAIN)

case COP_JSR:
{
	/* Jump to a subroutine within this method.  The address
	   is saved as an offset because the subroutine may call
	   other methods, which may cause the CVM cache to be
	   flushed.  When the subroutine finally returns, this
	   method may no longer be located where it used to be */
	stacktop[0].intValue = (ILInt32)((pc + 5) - pcstart);
	pc += IL_READ_INT32(pc + 1);
	stacktop += 1;
}
break;

case COP_RET_JSR:
{
	/* Return from a subroutine within this method */
	pc = pcstart + stacktop[-1].intValue;
	stacktop -= 1;
}
break;

#elif defined(IL_CVM_PREFIX)

case COP_PREFIX_ENTER_TRY:
{
	/* Enter a try block */
#if 0
	CVMFrame *frame = CreateFrame(thread);
	frame->type = CVM_FRAME_TRY;
	frame->pc =
		(ILInt32)((pc + IL_READ_INT32(pc + 2)) - pcstart);
	frame->method = method;
#endif
	MODIFY_PC_AND_STACK(6, 0);
}
break;

case COP_PREFIX_EXIT_TRY:
{
	/* Exit a try block */
#if 0
	--(thread->frameStackSize);
#endif
	MODIFY_PC_AND_STACK(2, 0);
}
break;

case COP_PREFIX_THROW:
{
	/* Throw an exception */
throwException:
#if 0
	/* Move the exception object down the stack */
	stackbase[0].ptrValue = stacktop[-1].ptrValue;
	stacktop = stackbase + 1;

	/* Find the next higher exception or exit frame */
#endif
	MODIFY_PC_AND_STACK(2, 0);
}
break;

#endif /* IL_CVM_PREFIX */
