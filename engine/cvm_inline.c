/*
 * cvm_inline.c - Opcodes for inlined methods.
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

/*
 * Test two strings for equality.
 */
static IL_INLINE int StringEquals(System_String *str1,
								  System_String *str2)
{
	if(str1)
	{
		if(str2 && str1->length == str2->length &&
		   (str1->length == 0 ||
		    !IL_MEMCMP(StringToBuffer(str1), StringToBuffer(str2),
		   			   str1->length * 2)))
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return (str2 == 0);
	}
}

#elif defined(IL_CVM_LOCALS)

/* No locals required */

#elif defined(IL_CVM_MAIN)

/* Nothing in the main table */

#elif defined(IL_CVM_PREFIX)

case COP_PREFIX_STRING_CONCAT_2:
{
	/* Concatenate two strings */
	COPY_STATE_TO_THREAD();
	stacktop[-2].ptrValue =
		ILStringConcat(thread,
					   (ILString *)(stacktop[-2].ptrValue),
					   (ILString *)(stacktop[-1].ptrValue));
	RESTORE_STATE_FROM_THREAD();
	MODIFY_PC_AND_STACK(2, -1);
}
break;

case COP_PREFIX_STRING_CONCAT_3:
{
	/* Concatenate three strings */
	COPY_STATE_TO_THREAD();
	stacktop[-3].ptrValue =
		ILStringConcat3(thread,
						(ILString *)(stacktop[-3].ptrValue),
					    (ILString *)(stacktop[-2].ptrValue),
					    (ILString *)(stacktop[-1].ptrValue));
	RESTORE_STATE_FROM_THREAD();
	MODIFY_PC_AND_STACK(2, -2);
}
break;

case COP_PREFIX_STRING_CONCAT_4:
{
	/* Concatenate four strings */
	COPY_STATE_TO_THREAD();
	stacktop[-4].ptrValue =
		ILStringConcat4(thread,
						(ILString *)(stacktop[-4].ptrValue),
					    (ILString *)(stacktop[-3].ptrValue),
					    (ILString *)(stacktop[-2].ptrValue),
					    (ILString *)(stacktop[-1].ptrValue));
	RESTORE_STATE_FROM_THREAD();
	MODIFY_PC_AND_STACK(2, -3);
}
break;

case COP_PREFIX_STRING_EQ:
{
	/* Test two strings for equality */
	stacktop[-2].intValue =
		StringEquals((System_String *)(stacktop[-2].ptrValue),
					 (System_String *)(stacktop[-1].ptrValue));
	MODIFY_PC_AND_STACK(2, -1);
}
break;

case COP_PREFIX_STRING_NE:
{
	/* Test two strings for inequality */
	stacktop[-2].intValue =
		!StringEquals((System_String *)(stacktop[-2].ptrValue),
					  (System_String *)(stacktop[-1].ptrValue));
	MODIFY_PC_AND_STACK(2, -1);
}
break;

case COP_PREFIX_STRING_GET_CHAR:
{
	/* Get a character from a string */
	tempptr = stacktop[-2].ptrValue;
	if(tempptr != 0)
	{
		tempNum = stacktop[-1].uintValue;
		if(tempNum < ((System_String *)tempptr)->length)
		{
			stacktop[-2].uintValue =
				(ILUInt32)(StringToBuffer(tempptr))[tempNum];
			MODIFY_PC_AND_STACK(2, -1);
		}
		else
		{
			ARRAY_INDEX_EXCEPTION();
		}
	}
	else
	{
		NULL_POINTER_EXCEPTION();
	}
}
break;

#endif /* IL_CVM_PREFIX */
