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

/**
 * <opcode name="dup">
 *   <operation>Duplicate the top of stack</operation>
 *
 *   <format>dup</format>
 *
 *   <form name="dup" code="COP_DUP"/>
 *
 *   <before>..., value</before>
 *   <after>..., value, value</after>
 *
 *   <description>Pop the single-word <i>value</i> from
 *   the stack, and then push it twice.</description>
 * </opcode>
 */
case COP_DUP:
{
	/* Duplicate the top-most word on the stack */
	stacktop[0] = stacktop[-1];
	MODIFY_PC_AND_STACK(1, 1);
}
break;

/**
 * <opcode name="dup2">
 *   <operation>Duplicate the top two stack words</operation>
 *
 *   <format>dup2</format>
 *
 *   <form name="dup2" code="COP_DUP2"/>
 *
 *   <before>..., value1, value2</before>
 *   <after>..., value1, value2, value1, value2</after>
 *
 *   <description>Pop the words <i>value1</i> and <i>value2</i> from
 *   the stack and then push them twice.</description>
 * </opcode>
 */
case COP_DUP2:
{
	/* Duplicate the top two words on the stack */
	stacktop[0] = stacktop[-2];
	stacktop[1] = stacktop[-1];
	MODIFY_PC_AND_STACK(1, 2);
}
break;

/**
 * <opcode name="dup_n">
 *   <operation>Duplicate the top <i>N</i> stack words</operation>
 *
 *   <format>dup_n<fsep/>N[1]</format>
 *   <format>wide<fsep/>dup_n<fsep/>N[4]</format>
 *
 *   <form name="dup_n" code="COP_DUP_N"/>
 *
 *   <before>..., value1, ..., valueN</before>
 *   <after>..., value1, ..., valueN, value1, ..., valueN</after>
 *
 *   <description>Pop the top <i>N</i> words from the stack and then
 *   push them twice.</description>
 *
 *   <notes>This is typically used for value type instances that are larger
 *   than 2 words in size.</notes>
 * </opcode>
 */
case COP_DUP_N:
{
	/* Duplicate the top N words on the stack */
	IL_MEMCPY(&(stacktop[0]), &(stacktop[-((int)(pc[1]))]),
			  sizeof(CVMWord) * ((unsigned)(pc[1])));
	stacktop += (unsigned)(pc[1]);
	pc += 2;
}
break;

/**
 * <opcode name="dup_word_n">
 *   <operation>Duplicate a stack word that is <i>N</i> words
 *				down the stack</operation>
 *
 *   <format>dup_word_n<fsep/>N[1]</format>
 *   <format>wide<fsep/>dup_word_n<fsep/>N[4]</format>
 *
 *   <form name="dup_word_n" code="COP_DUP_WORD_N"/>
 *
 *   <before>..., value, word1, ..., wordN</before>
 *   <after>..., value, word1, ..., wordN, value</after>
 *
 *   <description>Retrieve the <i>value</i> that is <i>N</i> words
 *   down the stack and push it onto the top of the stack.</description>
 * </opcode>
 */
case COP_DUP_WORD_N:
{
	/* Duplicate a word which is N words down the stack */
	stacktop[0] = stacktop[-(((int)(pc[1])) + 1)];
	MODIFY_PC_AND_STACK(2, 1);
}
break;

/**
 * <opcode name="pop">
 *   <operation>Pop the top-most word from the stack</operation>
 *
 *   <format>pop</format>
 *
 *   <form name="pop" code="COP_POP"/>
 *
 *   <before>..., value</before>
 *   <after>...</after>
 *
 *   <description>Pop the single-word <i>value</i> from
 *   the stack.</description>
 * </opcode>
 */
case COP_POP:
{
	/* Pop the top-most word from the stack */
	MODIFY_PC_AND_STACK(1, -1);
}
break;

/**
 * <opcode name="pop2">
 *   <operation>Pop the top two words from the stack</operation>
 *
 *   <format>pop2</format>
 *
 *   <form name="pop2" code="COP_POP2"/>
 *
 *   <before>..., value1, value2</before>
 *   <after>...</after>
 *
 *   <description>Pop the two stack words <i>value1</i> and <i>value2</i>
 *   from the stack.</description>
 * </opcode>
 */
case COP_POP2:
{
	/* Pop the top two words from the stack */
	MODIFY_PC_AND_STACK(1, -2);
}
break;

/**
 * <opcode name="pop_n">
 *   <operation>Pop the top <i>N</i> words from the stack</operation>
 *
 *   <format>pop_n<fsep/>N[1]</format>
 *   <format>wide<fsep/>pop_n<fsep/>N[4]</format>
 *
 *   <form name="pop_n" code="COP_POP_N"/>
 *
 *   <before>..., value1, ..., valueN</before>
 *   <after>...</after>
 *
 *   <description>Pop the top <i>N</i> stack words
 *   from the stack.</description>
 * </opcode>
 */
case COP_POP_N:
{
	/* Pop the top N words from the stack */
	stacktop -= (unsigned)(pc[1]);
	pc += 2;
}
break;

/**
 * <opcode name="squash">
 *   <operation>Squash a number of words out of the stack</operation>
 *
 *   <format>squash<fsep/>N[1]<fsep/>M[1]</format>
 *   <format>wide<fsep/>squash<fsep/>N[4]<fsep/>M[4]</format>
 *
 *   <form name="squash" code="COP_SQUASH"/>
 *
 *   <before>..., word1, ..., wordM, value1, ..., valueN</before>
 *   <after>..., value1, ..., valueN</after>
 *
 *   <description>Remove the <i>M</i> words from the stack,
 *   <i>N</i> words down the stack.</description>
 * </opcode>
 */
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

/**
 * <opcode name="ckheight">
 *   <operation>Check the height of the stack</operation>
 *
 *   <format>ckheight<fsep/>0<fsep/>0<fsep/>0<fsep/>0</format>
 *
 *   <form name="ckheight" code="COP_CKHEIGHT"/>
 *
 *   <description>Check that the stack has at least 8 words of
 *   space available for pushing values.</description>
 *
 *   <notes>This instruction is used at the start of a method to
 *   check that that there is sufficient stack space to hold the
 *   local variables and temporary stack values used by the method.<p/>
 *
 *   The opcode is followed by 4 zero bytes so that this instruction
 *   has the same length as <i>ckheight_n</i>.  This makes it easier
 *   to back-patch the height after translating the method.</notes>
 *
 *   <exceptions>
 *     <exception name="System.StackOverflowException">Raised if
 *     there is insufficient space available on the stack.</exception>
 *   </exceptions>
 * </opcode>
 */
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

/**
 * <opcode name="ckheight_n">
 *   <operation>Check the height of the stack for <i>N</i>
 *              words of available space</operation>
 *
 *   <format>ckheight_n<fsep/>N[4]</format>
 *
 *   <form name="ckheight_n" code="COP_CKHEIGHT_N"/>
 *
 *   <description>Check that the stack has at least <i>N</i> words of
 *   space available for pushing values.</description>
 *
 *   <notes>This instruction is used at the start of a method to
 *   check that that there is sufficient stack space to hold the
 *   local variables and temporary stack values used by the method.</notes>
 *
 *   <exceptions>
 *     <exception name="System.StackOverflowException">Raised if
 *     there is insufficient space available on the stack.</exception>
 *   </exceptions>
 * </opcode>
 */
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

/**
 * <opcode name="set_num_args">
 *   <operation>Set the number of arguments for the current method</operation>
 *
 *   <format>set_num_args<fsep/>N[1]</format>
 *   <format>wide<fsep/>set_num_args<fsep/>N[4]</format>
 *
 *   <form name="set_num_args" code="COP_SET_NUM_ARGS"/>
 *
 *   <description>Set the frame pointer for the current method to
 *   the address of the <i>N</i>'th word down the stack.</description>
 *
 *   <notes>This is typically the first instruction in a method, which
 *   sets up the local variable frame.</notes>
 * </opcode>
 */
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
	stacktop -= IL_READ_UINT32(pc + 2);
	pc += 6;
}
break;

case COP_SQUASH:
{
	/* Wide version of "squash" */
	tempNum = IL_READ_UINT32(pc + 2);
	tempptr = stacktop - tempNum - IL_READ_UINT32(pc + 6);
	IL_MEMMOVE(tempptr, stacktop - tempNum, sizeof(CVMWord *) * tempNum);
	stacktop -= IL_READ_UINT32(pc + 6);
	pc += 10;
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
