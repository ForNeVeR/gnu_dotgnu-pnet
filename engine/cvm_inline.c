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

System_Text_StringBuilder *builder;

#elif defined(IL_CVM_MAIN)

/* Nothing in the main table */

#elif defined(IL_CVM_PREFIX)

/**
 * <opcode name="string_concat_2" group="Inline methods">
 *   <operation>Concatenate two strings</operation>
 *
 *   <format>prefix<fsep/>string_concat_2</format>
 *   <dformat>{string_concat_2}</dformat>
 *
 *   <form name="string_concat_2" code="COP_PREFIX_STRING_CONCAT_2"/>
 *
 *   <before>..., value1, value2</before>
 *   <after>..., result</after>
 *
 *   <description>Both <i>value1</i> and <i>value2</i> are popped
 *   from the stack as type <code>string</code>.  The <i>result</i>
 *   is the <code>string</code> that results from concatenating
 *   <i>value1</i> and <i>value2</i>.  The <i>result</i> is pushed
 *   onto the stack.</description>
 *
 *   <notes>This instruction is used to inline calls to the
 *   <code>String.Concat(String, String)</code> method.</notes>
 * </opcode>
 */
VMCASE(COP_PREFIX_STRING_CONCAT_2):
{
	/* Concatenate two strings */
	COPY_STATE_TO_THREAD();
	stacktop[-2].ptrValue =
		ILStringConcat(thread,
					   (ILString *)(stacktop[-2].ptrValue),
					   (ILString *)(stacktop[-1].ptrValue));
	RESTORE_STATE_FROM_THREAD();
	MODIFY_PC_AND_STACK(CVMP_LEN_NONE, -1);
}
VMBREAK(COP_PREFIX_STRING_CONCAT_2);

/**
 * <opcode name="string_concat_3" group="Inline methods">
 *   <operation>Concatenate three strings</operation>
 *
 *   <format>prefix<fsep/>string_concat_3</format>
 *   <dformat>{string_concat_3}</dformat>
 *
 *   <form name="string_concat_3" code="COP_PREFIX_STRING_CONCAT_3"/>
 *
 *   <before>..., value1, value2, value3</before>
 *   <after>..., result</after>
 *
 *   <description>The values <i>value1</i>, <i>value2</i>, and <i>value3</i>
 *   are popped from the stack as type <code>string</code>.  The <i>result</i>
 *   is the <code>string</code> that results from concatenating
 *   <i>value1</i>, <i>value2</i>, and <i>value3</i>.  The <i>result</i>
 *   is pushed onto the stack.</description>
 *
 *   <notes>This instruction is used to inline calls to the
 *   <code>String.Concat(String, String, String)</code> method.</notes>
 * </opcode>
 */
VMCASE(COP_PREFIX_STRING_CONCAT_3):
{
	/* Concatenate three strings */
	COPY_STATE_TO_THREAD();
	stacktop[-3].ptrValue =
		ILStringConcat3(thread,
						(ILString *)(stacktop[-3].ptrValue),
					    (ILString *)(stacktop[-2].ptrValue),
					    (ILString *)(stacktop[-1].ptrValue));
	RESTORE_STATE_FROM_THREAD();
	MODIFY_PC_AND_STACK(CVMP_LEN_NONE, -2);
}
VMBREAK(COP_PREFIX_STRING_CONCAT_3);

/**
 * <opcode name="string_concat_4" group="Inline methods">
 *   <operation>Concatenate four strings</operation>
 *
 *   <format>prefix<fsep/>string_concat_4</format>
 *   <dformat>{string_concat_4}</dformat>
 *
 *   <form name="string_concat_4" code="COP_PREFIX_STRING_CONCAT_4"/>
 *
 *   <before>..., value1, value2, value3, value4</before>
 *   <after>..., result</after>
 *
 *   <description>The values <i>value1</i>, <i>value2</i>, <i>value3</i>,
 *   and <i>value4</i> are popped from the stack as type <code>string</code>.
 *   The <i>result</i> is the <code>string</code> that results from
 *   concatenating <i>value1</i>, <i>value2</i>, <i>value3</i>, and
 *   <i>value4</i>.  The <i>result</i> is pushed onto the stack.</description>
 *
 *   <notes>This instruction is used to inline calls to the
 *   <code>String.Concat(String, String, String, String)</code> method.</notes>
 * </opcode>
 */
VMCASE(COP_PREFIX_STRING_CONCAT_4):
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
	MODIFY_PC_AND_STACK(CVMP_LEN_NONE, -3);
}
VMBREAK(COP_PREFIX_STRING_CONCAT_4);

/**
 * <opcode name="string_eq" group="Inline methods">
 *   <operation>Test two strings for equality</operation>
 *
 *   <format>prefix<fsep/>string_eq</format>
 *   <dformat>{string_eq}</dformat>
 *
 *   <form name="string_eq" code="COP_PREFIX_STRING_EQ"/>
 *
 *   <before>..., value1, value2</before>
 *   <after>..., result</after>
 *
 *   <description>Both <i>value1</i> and <i>value2</i> are popped
 *   from the stack as type <code>string</code>.  The <i>result</i>
 *   is the <code>int32</code> that results from comparing
 *   <i>value1</i> and <i>value2</i>: 1 if they are equal, and 0
 *   if they are not equal.  The <i>result</i> is pushed
 *   onto the stack.</description>
 *
 *   <notes>This instruction is used to inline calls to the
 *   <code>String.op_Equality(String, String)</code> method.</notes>
 * </opcode>
 */
VMCASE(COP_PREFIX_STRING_EQ):
{
	/* Test two strings for equality */
	stacktop[-2].intValue =
		StringEquals((System_String *)(stacktop[-2].ptrValue),
					 (System_String *)(stacktop[-1].ptrValue));
	MODIFY_PC_AND_STACK(CVMP_LEN_NONE, -1);
}
VMBREAK(COP_PREFIX_STRING_EQ);

/**
 * <opcode name="string_ne" group="Inline methods">
 *   <operation>Test two strings for inequality</operation>
 *
 *   <format>prefix<fsep/>string_ne</format>
 *   <dformat>{string_ne}</dformat>
 *
 *   <form name="string_ne" code="COP_PREFIX_STRING_NE"/>
 *
 *   <before>..., value1, value2</before>
 *   <after>..., result</after>
 *
 *   <description>Both <i>value1</i> and <i>value2</i> are popped
 *   from the stack as type <code>string</code>.  The <i>result</i>
 *   is the <code>int32</code> that results from comparing
 *   <i>value1</i> and <i>value2</i>: 1 if they are not equal, and 0
 *   if they are equal.  The <i>result</i> is pushed
 *   onto the stack.</description>
 *
 *   <notes>This instruction is used to inline calls to the
 *   <code>String.op_Inequality(String, String)</code> method.</notes>
 * </opcode>
 */
VMCASE(COP_PREFIX_STRING_NE):
{
	/* Test two strings for inequality */
	stacktop[-2].intValue =
		!StringEquals((System_String *)(stacktop[-2].ptrValue),
					  (System_String *)(stacktop[-1].ptrValue));
	MODIFY_PC_AND_STACK(CVMP_LEN_NONE, -1);
}
VMBREAK(COP_PREFIX_STRING_NE);

/**
 * <opcode name="string_get_char" group="Inline methods">
 *   <operation>Get a particular character from a string</operation>
 *
 *   <format>prefix<fsep/>string_get_char</format>
 *   <dformat>{string_get_char}</dformat>
 *
 *   <form name="string_get_char" code="COP_PREFIX_STRING_GET_CHAR"/>
 *
 *   <before>..., value1, value2</before>
 *   <after>..., result</after>
 *
 *   <description>Both <i>value1</i> and <i>value2</i> are popped
 *   from the stack as the types <code>string</code> and <code>int32</code>
 *   respectively.  The <i>result</i> is the <code>int32</code> that
 *   results from fetching the character at position <i>value2</i>
 *   within the string <i>value1</i>.
 *   <code>System.IndexOutOfRangeException</code> will be thrown if
 *   <i>value2</i> is an invalid index.</description>
 *
 *   <notes>This instruction is used to inline calls to the
 *   <code>String.get_Chars(int)</code> method.</notes>
 *
 *   <exceptions>
 *     <exception name="System.NullReferenceException">Raised if
 *     <i>value1</i> is <code>null</code>.</exception>
 *     <exception name="System.IndexOutOfRangeException">Raised if
 *     <i>value2</i> is not a valid character index for the string
 *     <i>value1</i>.</exception>
 *   </exceptions>
 * </opcode>
 */
VMCASE(COP_PREFIX_STRING_GET_CHAR):
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
			MODIFY_PC_AND_STACK(CVMP_LEN_NONE, -1);
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
VMBREAK(COP_PREFIX_STRING_GET_CHAR);

/**
 * <opcode name="type_from_handle" group="Inline methods">
 *   <operation>Get a type object from its runtime handle</operation>
 *
 *   <format>prefix<fsep/>type_from_handle</format>
 *   <dformat>{type_from_handle}</dformat>
 *
 *   <form name="type_from_handle" code="COP_PREFIX_TYPE_FROM_HANDLE"/>
 *
 *   <before>..., handle</before>
 *   <after>..., object</after>
 *
 *   <description>The <i>handle</i> is popped from the stack as the
 *   type <code>ptr</code>.  It is interpreted as an instance of
 *   the value type <code>System.RuntimeTypeHandle</code>.  The
 *   <i>handle</i> is converted into an <i>object</i> instance
 *   of the reference type <code>System.Type</code>.  The <i>object</i>
 *   is pushed onto the stack.</description>
 *
 *   <notes>This instruction is used to inline calls to the
 *   <code>Type.GetTypeFromHandle(RuntimeTypeHandle)</code> method.</notes>
 * </opcode>
 */
VMCASE(COP_PREFIX_TYPE_FROM_HANDLE):
{
	/* Get a character from a string */
	tempptr = stacktop[-1].ptrValue;
	if(tempptr != 0)
	{
		COPY_STATE_TO_THREAD();
		stacktop[-1].ptrValue =
			(void *)(_ILGetClrType(thread, (ILClass *)tempptr));
		RESTORE_STATE_FROM_THREAD();
	}
	MODIFY_PC_AND_STACK(CVMP_LEN_NONE, 0);
}
VMBREAK(COP_PREFIX_TYPE_FROM_HANDLE);

/**
 * <opcode name="monitor_enter" group="Inline methods">
 *   <operation>Enter a monitor on an object</operation>
 *
 *   <format>prefix<fsep/>monitor_enter</format>
 *   <dformat>{monitor_enter}</dformat>
 *
 *   <form name="monitor_enter" code="COP_PREFIX_MONITOR_ENTER"/>
 *
 *   <before>..., object</before>
 *   <after>...</after>
 *
 *   <description>The <i>object</i> is popped from the stack as the
 *   type <code>ptr</code>.  The current thread is made to enter the
 *   synchronisation monitor on <i>object</i>.  Execution continues
 *   once the monitor has been acquired.</description>
 *
 *   <notes>This instruction is used to inline calls to the
 *   <code>Monitor.Enter(Object)</code> method.</notes>
 * </opcode>
 */
VMCASE(COP_PREFIX_MONITOR_ENTER):
{
	/* Enter a monitor on an object */
	/* TODO: waiting for thread support to be completed */
	MODIFY_PC_AND_STACK(CVMP_LEN_NONE, -1);
}
VMBREAK(COP_PREFIX_MONITOR_ENTER);

/**
 * <opcode name="monitor_exit" group="Inline methods">
 *   <operation>Exit a monitor on an object</operation>
 *
 *   <format>prefix<fsep/>monitor_exit</format>
 *   <dformat>{monitor_exit}</dformat>
 *
 *   <form name="monitor_exit" code="COP_PREFIX_MONITOR_EXIT"/>
 *
 *   <before>..., object</before>
 *   <after>...</after>
 *
 *   <description>The <i>object</i> is popped from the stack as the
 *   type <code>ptr</code>.  The current thread is made to exit the
 *   synchronisation monitor on <i>object</i>.  Execution continues
 *   once the monitor has been released.</description>
 *
 *   <notes>This instruction is used to inline calls to the
 *   <code>Monitor.Exit(Object)</code> method.</notes>
 * </opcode>
 */
VMCASE(COP_PREFIX_MONITOR_EXIT):
{
	/* Exit a monitor on an object */
	/* TODO: waiting for thread support to be completed */
	MODIFY_PC_AND_STACK(CVMP_LEN_NONE, -1);
}
VMBREAK(COP_PREFIX_MONITOR_EXIT);

/**
 * <opcode name="append_char" group="Inline methods">
 *   <operation>Append a character to a string builder</operation>
 *
 *   <format>prefix<fsep/>append_char<fsep/>method</format>
 *   <dformat>{append_char}<fsep/>method</dformat>
 *
 *   <form name="append_char" code="COP_PREFIX_APPEND_CHAR"/>
 *
 *   <before>..., builder, ch</before>
 *   <after>..., builder</after>
 *
 *   <description>The <i>builder</i> and <i>ch</i> are popped from the
 *   stack as the types <code>ptr</code> and <code>int32</code> respectively.
 *   The character <i>ch</i> is appended to the end of the string builder
 *   indicated by <i>builder</i>.  The <i>builder</i> is then pushed
 *   back onto the stack.</description>
 *
 *   <notes>This instruction is used to inline calls to the
 *   <code>StringBuilder.Append(char)</code> method.  The <i>method</i>
 *   argument must be a pointer to this method, because the interpreter
 *   will "bail out" to the C# class library if the append is too
 *   difficult to perform (e.g. the string must be reallocated).</notes>
 *
 *   <exceptions>
 *     <exception name="System.NullReferenceException">Raised if
 *     <i>builder</i> is <code>null</code>.</exception>
 *   </exceptions>
 * </opcode>
 */
VMCASE(COP_PREFIX_APPEND_CHAR):
{
	/* Append a character to a string builder */
	if(stacktop[-2].ptrValue)
	{
		builder = (System_Text_StringBuilder *)(stacktop[-2].ptrValue);
		if(!(builder->needsCopy) &&
		   builder->buildString->length < builder->buildString->capacity)
		{
			/* We can insert the character into the string directly */
			(StringToBuffer(builder->buildString))
				[(builder->buildString->length)++] =
					(ILUInt16)(stacktop[-1].intValue);
			MODIFY_PC_AND_STACK(CVMP_LEN_PTR, -1);
		}
		else
		{
			/* We need to reallocate the builder, so call the C# library */
			COPY_STATE_TO_THREAD();
			ILExecThreadCallVirtual(thread, CVMP_ARG_PTR(ILMethod *),
									&(stacktop[-2].ptrValue),
									stacktop[-2].ptrValue,
									(ILVaInt)(stacktop[-1].intValue));
			RESTORE_STATE_FROM_THREAD();
			MODIFY_PC_AND_STACK(CVMP_LEN_PTR, -1);
		}
	}
	else
	{
		NULL_POINTER_EXCEPTION();
	}
}
VMBREAK(COP_PREFIX_APPEND_CHAR);

/**
 * <opcode name="is_white_space" group="Inline methods">
 *   <operation>Determine if a character is white space</operation>
 *
 *   <format>prefix<fsep/>is_white_space</format>
 *   <dformat>{is_white_space}</dformat>
 *
 *   <form name="is_white_space" code="COP_PREFIX_IS_WHITE_SPACE"/>
 *
 *   <before>..., ch</before>
 *   <after>..., result</after>
 *
 *   <description>The <i>ch</i> is popped from the stack as the
 *   type <code>int32</code>.  If it is a white space character,
 *   then the <code>int32</code> <i>result</i> 1 is pushed onto
 *   the stack; otherwise 0 is pushed.</description>
 *
 *   <notes>This instruction is used to inline calls to the
 *   <code>Char.IsWhiteSpace(char)</code> method, which is used
 *   heavily in text processing code.</notes>
 * </opcode>
 */
VMCASE(COP_PREFIX_IS_WHITE_SPACE):
{
	/* Determine if a character is white space */
	position = stacktop[-1].intValue;
	if(position == 0x0009 || position == 0x0020 || position == 0x000a ||
	   position == 0x000b || position == 0x000c || position == 0x000d ||
	   position == 0x0085 || position == 0x2028 || position == 0x2029)
	{
		stacktop[-1].intValue = 1;
	}
	else if(position < 0x0080)
	{
		stacktop[-1].intValue = 0;
	}
	else
	{
		stacktop[-1].intValue =
			(ILGetUnicodeCategory((unsigned)position) ==
					ILUnicode_SpaceSeparator);
	}
	MODIFY_PC_AND_STACK(CVMP_LEN_NONE, 0);
}
VMBREAK(COP_PREFIX_IS_WHITE_SPACE);

#endif /* IL_CVM_PREFIX */
