/*
 * cvm_compare.c - Opcodes for comparing values on the stack.
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

#elif defined(IL_CVM_PREFIX)

case COP_PREFIX_ICMP:
{
	/* Compare integer values */
	if(stacktop[-2].intValue < stacktop[-1].intValue)
	{
		stacktop[-2].intValue = -1;
	}
	else if(stacktop[-2].intValue > stacktop[-1].intValue)
	{
		stacktop[-2].intValue = 1;
	}
	else
	{
		stacktop[-2].intValue = 0;
	}
	MODIFY_PC_AND_STACK(2, -1);
}
break;

case COP_PREFIX_ICMP_UN:
{
	/* Compare unsigned integer values */
	if(stacktop[-2].uintValue < stacktop[-1].uintValue)
	{
		stacktop[-2].intValue = -1;
	}
	else if(stacktop[-2].uintValue > stacktop[-1].uintValue)
	{
		stacktop[-2].intValue = 1;
	}
	else
	{
		stacktop[-2].intValue = 0;
	}
	MODIFY_PC_AND_STACK(2, -1);
}
break;

case COP_PREFIX_LCMP:
{
	/* Compare long values */
	stacktop[-(CVM_WORDS_PER_LONG * 2)].intValue =
		LCmp(&(stacktop[-(CVM_WORDS_PER_LONG * 2)]),
		     &(stacktop[-CVM_WORDS_PER_LONG]));
	MODIFY_PC_AND_STACK(2, -(CVM_WORDS_PER_LONG * 2) + 1);
}
break;

case COP_PREFIX_LCMP_UN:
{
	/* Compare unsigned long values */
	stacktop[-(CVM_WORDS_PER_LONG * 2)].intValue =
		LUCmp(&(stacktop[-(CVM_WORDS_PER_LONG * 2)]),
		      &(stacktop[-CVM_WORDS_PER_LONG]));
	MODIFY_PC_AND_STACK(2, -(CVM_WORDS_PER_LONG * 2) + 1);
}
break;

case COP_PREFIX_FCMPL:
{
	/* Compare float values */
	stacktop[-(CVM_WORDS_PER_NATIVE_FLOAT * 2)].intValue =
		FCmp(&(stacktop[-(CVM_WORDS_PER_NATIVE_FLOAT * 2)]),
		     &(stacktop[-CVM_WORDS_PER_NATIVE_FLOAT]), -1);
	MODIFY_PC_AND_STACK
		(2, -(CVM_WORDS_PER_NATIVE_FLOAT * 2) + 1);
}
break;

case COP_PREFIX_FCMPG:
{
	/* Compare float values */
	stacktop[-(CVM_WORDS_PER_NATIVE_FLOAT * 2)].intValue =
		FCmp(&(stacktop[-(CVM_WORDS_PER_NATIVE_FLOAT * 2)]),
		     &(stacktop[-CVM_WORDS_PER_NATIVE_FLOAT]), 1);
	MODIFY_PC_AND_STACK
		(2, -(CVM_WORDS_PER_NATIVE_FLOAT * 2) + 1);
}
break;

case COP_PREFIX_PCMP:
{
	/* Compare pointer values */
	if(stacktop[-2].ptrValue < stacktop[-1].ptrValue)
	{
		stacktop[-2].intValue = -1;
	}
	else if(stacktop[-2].ptrValue > stacktop[-1].ptrValue)
	{
		stacktop[-2].intValue = 1;
	}
	else
	{
		stacktop[-2].intValue = 0;
	}
	MODIFY_PC_AND_STACK(2, -1);
}
break;

case COP_PREFIX_SETEQ:
{
	/* Set true if the stack top is zero */
	stacktop[-1].intValue = (stacktop[-1].intValue == 0);
	MODIFY_PC_AND_STACK(2, 0);
}
break;

case COP_PREFIX_SETNE:
{
	/* Set true if the stack top is non-zero */
	stacktop[-1].intValue = (stacktop[-1].intValue != 0);
	MODIFY_PC_AND_STACK(2, 0);
}
break;

case COP_PREFIX_SETLT:
{
	/* Set true if the stack top is less than zero */
	stacktop[-1].intValue = (stacktop[-1].intValue < 0);
	MODIFY_PC_AND_STACK(2, 0);
}
break;

case COP_PREFIX_SETLE:
{
	/* Set true if the stack top is less or equal to zero */
	stacktop[-1].intValue = (stacktop[-1].intValue <= 0);
	MODIFY_PC_AND_STACK(2, 0);
}
break;

case COP_PREFIX_SETGT:
{
	/* Set true if the stack top is greater than zero */
	stacktop[-1].intValue = (stacktop[-1].intValue > 0);
	MODIFY_PC_AND_STACK(2, 0);
}
break;

case COP_PREFIX_SETGE:
{
	/* Set true if the stack top greater or equal to zero */
	stacktop[-1].intValue = (stacktop[-1].intValue >= 0);
	MODIFY_PC_AND_STACK(2, 0);
}
break;

#endif /* IL_CVM_PREFIX */
