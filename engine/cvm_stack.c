/*
 * cvm_stack.c - Opcodes for accessing the stack.
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

case COP_DUP:
{
	/* Duplicate the top-most word on the stack */
	stacktop[0] = stacktop[-1];
	MODIFY_PC_AND_STACK(1, 1);
}
break;

case COP_DUP2:
{
	/* Duplicate the top two words on the stack */
	stacktop[0] = stacktop[-2];
	stacktop[1] = stacktop[-1];
	MODIFY_PC_AND_STACK(1, 2);
}
break;

case COP_DUP_N:
{
	/* Duplicate the top N words on the stack */
	IL_MEMCPY(&(stacktop[0]), &(stacktop[-((int)(pc[1]))]),
			  sizeof(CVMWord) * ((unsigned)(pc[1])));
	MODIFY_PC_AND_STACK(2, (unsigned)(pc[1]));
}
break;

case COP_DUP_WORD_N:
{
	/* Duplicate a word which is N words down the stack */
	stacktop[0] = stacktop[-(((int)(pc[1])) + 1)];
	MODIFY_PC_AND_STACK(2, 1);
}
break;

case COP_POP:
{
	/* Pop the top-most word from the stack */
	MODIFY_PC_AND_STACK(1, -1);
}
break;

case COP_POP2:
{
	/* Pop the top two words from the stack */
	MODIFY_PC_AND_STACK(1, -2);
}
break;

case COP_POP_N:
{
	/* Pop the top N words from the stack */
	MODIFY_PC_AND_STACK(2, -((int)(pc[1])));
}
break;

case COP_SQUASH:
{
	/* Squash N words out of the stack, M words down the stack */
	IL_MEMMOVE(&(stacktop[-(((int)(pc[1])) + ((int)(pc[2])))]),
			   &(stacktop[-((int)(pc[1]))]),
			   sizeof(CVMWord *) * ((int)(pc[1])));
	stacktop -= (int)(pc[2]);
	pc += 3;
}
break;

case COP_CKHEIGHT:
{
	/* Check the height of the stack to ensure that we have
	   at least 8 stack slots available.  This instruction
	   occupies 5 bytes because it is normally overlaid on
	   top of a longer CKHEIGHT_N instruction */
	if(((ILUInt32)(stackmax - stacktop)) >= 8)
	{
		MODIFY_PC_AND_STACK(5, 0);
	}
	else
	{
		STACK_OVERFLOW_EXCEPTION();
	}
}
break;

case COP_CKHEIGHT_N:
{
	/* Check the height of the stack to ensure that we have
	   at least N stack slots available */
	if(((ILUInt32)(stackmax - stacktop)) >= IL_READ_UINT32(pc + 1))
	{
		MODIFY_PC_AND_STACK(5, 0);
	}
	else
	{
		STACK_OVERFLOW_EXCEPTION();
	}
}
break;

case COP_SET_NUM_ARGS:
{
	/* Set the number of argument stack slots */
	frame = stacktop - ((ILUInt32)(pc[1]));
	MODIFY_PC_AND_STACK(2, 0);
}
break;

#elif defined(IL_CVM_WIDE)

case COP_DUP_N:
{
	/* Wide version of "dup_n" */
	tempNum = IL_READ_UINT32(pc + 2);
	IL_MEMCPY(&(stacktop[0]), &(stacktop[-((int)tempNum)]),
			  sizeof(CVMWord) * tempNum);
	MODIFY_PC_AND_STACK(6, tempNum);
}
break;

case COP_DUP_WORD_N:
{
	/* Wide version of "dup_word_n" */
	stacktop[0] = stacktop[-(((int)IL_READ_UINT32(pc + 2)) + 1)];
	MODIFY_PC_AND_STACK(6, 1);
}
break;

case COP_POP_N:
{
	/* Wide version of "pop_n" */
	MODIFY_PC_AND_STACK(6, -((int)IL_READ_UINT32(pc + 2)));
}
break;

case COP_SQUASH:
{
	/* Wide version of "squash" */
	tempNum = IL_READ_UINT32(pc + 2);
	tempptr = stacktop - tempNum - IL_READ_UINT32(pc + 6);
	IL_MEMMOVE(tempptr, stacktop - tempNum, sizeof(CVMWord *) * tempNum);
	stacktop -= IL_READ_UINT32(pc + 6);
	pc += 8;
}
break;

case COP_SET_NUM_ARGS:
{
	/* Wide version of "set_num_args" */
	frame = stacktop - IL_READ_UINT32(pc + 2);
	MODIFY_PC_AND_STACK(6, 0);
}
break;

#endif /* IL_CVM_WIDE */
