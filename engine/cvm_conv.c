/*
 * cvm_conv.c - Opcodes for converting between data types.
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
 * Convert "ulong" into "native float".
 *
 * Some platforms cannot perform the conversion directly,
 * so we need to do it in stages.
 */
static IL_INLINE ILNativeFloat LU2F(ILUInt64 value)
{
	return ILUInt64ToFloat(value);
}

/*
 * Convert "native float" into "ulong".
 *
 * Some platforms cannot perform the conversion directly,
 * so we need to do it in stages.
 */
static IL_INLINE ILUInt64 F2LU(ILNativeFloat value)
{
	return ILFloatToUInt64(value);
}

/*
 * Convert "long" into "int" with overflow testing.
 */
static IL_INLINE int L2IOvf(CVMWord *posn)
{
	ILInt64 longValue = ReadLong(posn);
	if(longValue >= (ILInt64)IL_MIN_INT32 &&
	   longValue <= (ILInt64)IL_MAX_INT32)
	{
		posn->intValue = (ILInt32)longValue;
		return 1;
	}
	else
	{
		return 0;
	}
}

/*
 * Convert "long" into "uint" with overflow testing.
 */
static IL_INLINE int L2UIOvf(CVMWord *posn)
{
	ILInt64 longValue = ReadLong(posn);
	if(longValue >= 0 &&
	   longValue <= (ILInt64)IL_MAX_UINT32)
	{
		posn->uintValue = (ILUInt32)longValue;
		return 1;
	}
	else
	{
		return 0;
	}
}

/*
 * Convert "ulong" into "int" with overflow testing.
 */
static IL_INLINE int LU2IOvf(CVMWord *posn)
{
	ILUInt64 ulongValue = ReadULong(posn);
	if(ulongValue <= (ILUInt64)IL_MAX_INT32)
	{
		posn->intValue = (ILInt32)ulongValue;
		return 1;
	}
	else
	{
		return 0;
	}
}

/*
 * Convert "ulong" into "uint" with overflow testing.
 */
static IL_INLINE int LU2UIOvf(CVMWord *posn)
{
	ILUInt64 ulongValue = ReadULong(posn);
	if(ulongValue <= (ILUInt64)IL_MAX_UINT32)
	{
		posn->uintValue = (ILUInt32)ulongValue;
		return 1;
	}
	else
	{
		return 0;
	}
}

/*
 * Convert "native float" into "int" with overflow testing.
 */
static IL_INLINE int F2IOvf(CVMWord *posn)
{
	ILNativeFloat value = ReadFloat(posn);
	if(FLOAT_IS_FINITE(value))
	{
		if(value > (ILNativeFloat)(-2147483649.0) &&
		   value < (ILNativeFloat)2147483648.0)
		{
			posn->intValue = (ILInt32)value;
			return 1;
		}
	}
	return 0;
}

/*
 * Convert "native float" into "uint" with overflow testing.
 */
static IL_INLINE int F2UIOvf(CVMWord *posn)
{
	ILNativeFloat value = ReadFloat(posn);
	if(FLOAT_IS_FINITE(value))
	{
		if(value >= (ILNativeFloat)0.0 &&
		   value < (ILNativeFloat)4294967296.0)
		{
			posn->uintValue = (ILUInt32)value;
			return 1;
		}
	}
	return 0;
}

/*
 * Convert "native float" into "long" with overflow testing.
 */
static IL_INLINE int F2LOvf(CVMWord *posn)
{
	ILInt64 result;
	if(ILFloatToInt64Ovf(&result, ReadFloat(posn)))
	{
		WriteLong(posn, result);
		return 1;
	}
	else
	{
		return 0;
	}
}

/*
 * Convert "native float" into "ulong" with overflow testing.
 */
static IL_INLINE int F2LUOvf(CVMWord *posn)
{
	ILUInt64 result;
	if(ILFloatToUInt64Ovf(&result, ReadFloat(posn)))
	{
		WriteULong(posn, result);
		return 1;
	}
	else
	{
		return 0;
	}
}

#elif defined(IL_CVM_LOCALS)

ILInt32 position;

#elif defined(IL_CVM_MAIN)

/**
 * <opcode name="i2b" group="Conversion operators">
 *   <operation>Convert <code>int32</code> to <code>int8</code></operation>
 *
 *   <format>i2b</format>
 *
 *   <form name="i2b" code="COP_I2B"/>
 *
 *   <before>..., value</before>
 *   <after>..., result</after>
 *
 *   <description>The <i>value</i> is popped from the stack as
 *   type <code>int32</code>.  The <code>int32</code> <i>result</i>
 *   is formed by truncating <i>value</i> to 8 bits and then
 *   sign-extending it to 32 bits.  The <i>result</i> is pushed
 *   onto the stack.</description>
 * </opcode>
 */
VMCASE(COP_I2B):
{
	/* Convert from integer to signed byte */
	stacktop[-1].intValue = (ILInt32)(ILInt8)(stacktop[-1].intValue);
	MODIFY_PC_AND_STACK(1, 0);
}
VMBREAK;

/**
 * <opcode name="i2ub" group="Conversion operators">
 *   <operation>Convert <code>int32</code> to <code>uint8</code></operation>
 *
 *   <format>i2ub</format>
 *
 *   <form name="i2ub" code="COP_I2UB"/>
 *
 *   <before>..., value</before>
 *   <after>..., result</after>
 *
 *   <description>The <i>value</i> is popped from the stack as
 *   type <code>int32</code>.  The <code>int32</code> <i>result</i>
 *   is formed by truncating <i>value</i> to 8 bits and then
 *   zero-extending it to 32 bits.  The <i>result</i> is pushed
 *   onto the stack.</description>
 * </opcode>
 */
VMCASE(COP_I2UB):
{
	/* Convert from integer to unsigned byte */
	stacktop[-1].intValue = (ILInt32)(ILUInt8)(stacktop[-1].intValue);
	MODIFY_PC_AND_STACK(1, 0);
}
VMBREAK;

/**
 * <opcode name="i2s" group="Conversion operators">
 *   <operation>Convert <code>int32</code> to <code>int16</code></operation>
 *
 *   <format>i2s</format>
 *
 *   <form name="i2s" code="COP_I2S"/>
 *
 *   <before>..., value</before>
 *   <after>..., result</after>
 *
 *   <description>The <i>value</i> is popped from the stack as
 *   type <code>int32</code>.  The <code>int32</code> <i>result</i>
 *   is formed by truncating <i>value</i> to 16 bits and then
 *   sign-extending it to 32 bits.  The <i>result</i> is pushed
 *   onto the stack.</description>
 * </opcode>
 */
VMCASE(COP_I2S):
{
	/* Convert from integer to signed short */
	stacktop[-1].intValue = (ILInt32)(ILInt16)(stacktop[-1].intValue);
	MODIFY_PC_AND_STACK(1, 0);
}
VMBREAK;

/**
 * <opcode name="i2us" group="Conversion operators">
 *   <operation>Convert <code>int32</code> to <code>uint16</code></operation>
 *
 *   <format>i2us</format>
 *
 *   <form name="i2us" code="COP_I2US"/>
 *
 *   <before>..., value</before>
 *   <after>..., result</after>
 *
 *   <description>The <i>value</i> is popped from the stack as
 *   type <code>int32</code>.  The <code>int32</code> <i>result</i>
 *   is formed by truncating <i>value</i> to 16 bits and then
 *   zero-extending it to 32 bits.  The <i>result</i> is pushed
 *   onto the stack.</description>
 * </opcode>
 */
VMCASE(COP_I2US):
{
	/* Convert from integer to unsigned short */
	stacktop[-1].intValue = (ILInt32)(ILUInt16)(stacktop[-1].intValue);
	MODIFY_PC_AND_STACK(1, 0);
}
VMBREAK;

/**
 * <opcode name="i2l" group="Conversion operators">
 *   <operation>Convert <code>int32</code> to <code>int64</code></operation>
 *
 *   <format>i2l</format>
 *
 *   <form name="i2l" code="COP_I2L"/>
 *
 *   <before>..., value</before>
 *   <after>..., result</after>
 *
 *   <description>The <i>value</i> is popped from the stack as
 *   type <code>int32</code>.  The <code>int64</code> <i>result</i>
 *   is formed by sign-extending <i>value</i> to 64 bits.
 *   The <i>result</i> is pushed onto the stack.</description>
 * </opcode>
 */
VMCASE(COP_I2L):
{
	/* Convert from integer to long */
	WriteLong(&(stacktop[-1]), (ILInt64)(stacktop[-1].intValue));
	MODIFY_PC_AND_STACK(1, CVM_WORDS_PER_LONG - 1);
}
VMBREAK;

/**
 * <opcode name="iu2l" group="Conversion operators">
 *   <operation>Convert <code>uint32</code> to <code>int64</code></operation>
 *
 *   <format>iu2l</format>
 *
 *   <form name="iu2l" code="COP_IU2L"/>
 *
 *   <before>..., value</before>
 *   <after>..., result</after>
 *
 *   <description>The <i>value</i> is popped from the stack as
 *   type <code>uint32</code>.  The <code>int64</code> <i>result</i>
 *   is formed by zero-extending <i>value</i> to 64 bits.
 *   The <i>result</i> is pushed onto the stack.</description>
 * </opcode>
 */
VMCASE(COP_IU2L):
{
	/* Convert from unsigned integer to long */
	WriteLong(&(stacktop[-1]), (ILInt64)(stacktop[-1].uintValue));
	MODIFY_PC_AND_STACK(1, CVM_WORDS_PER_LONG - 1);
}
VMBREAK;

/**
 * <opcode name="i2f" group="Conversion operators">
 *   <operation>Convert <code>int32</code> to
 *				<code>native float</code></operation>
 *
 *   <format>i2f</format>
 *
 *   <form name="i2f" code="COP_I2F"/>
 *
 *   <before>..., value</before>
 *   <after>..., result</after>
 *
 *   <description>The <i>value</i> is popped from the stack as
 *   type <code>int32</code>, and converted into a <code>native float</code>
 *   <i>result</i>.  The <i>result</i> is pushed onto the stack.</description>
 * </opcode>
 */
VMCASE(COP_I2F):
{
	/* Convert from integer to "native float" */
	WriteFloat(&(stacktop[-1]), (ILNativeFloat)(stacktop[-1].intValue));
	MODIFY_PC_AND_STACK(1, CVM_WORDS_PER_NATIVE_FLOAT - 1);
}
VMBREAK;

/**
 * <opcode name="iu2f" group="Conversion operators">
 *   <operation>Convert <code>uint32</code> to
 *				<code>native float</code></operation>
 *
 *   <format>iu2f</format>
 *
 *   <form name="iu2f" code="COP_IU2F"/>
 *
 *   <before>..., value</before>
 *   <after>..., result</after>
 *
 *   <description>The <i>value</i> is popped from the stack as
 *   type <code>uint32</code>, and converted into a <code>native float</code>
 *   <i>result</i>.  The <i>result</i> is pushed onto the stack.</description>
 * </opcode>
 */
VMCASE(COP_IU2F):
{
	/* Convert from unsigned integer to "native float" */
	WriteFloat(&(stacktop[-1]), (ILNativeFloat)(stacktop[-1].uintValue));
	MODIFY_PC_AND_STACK(1, CVM_WORDS_PER_NATIVE_FLOAT - 1);
}
VMBREAK;

/**
 * <opcode name="l2i" group="Conversion operators">
 *   <operation>Convert <code>int64</code> to <code>int32</code></operation>
 *
 *   <format>l2i</format>
 *
 *   <form name="l2i" code="COP_L2I"/>
 *
 *   <before>..., value</before>
 *   <after>..., result</after>
 *
 *   <description>The <i>value</i> is popped from the stack as
 *   type <code>int64</code>.  The <code>int32</code> <i>result</i>
 *   is formed by truncating <i>value</i> to 32 bits.
 *   The <i>result</i> is pushed onto the stack.</description>
 * </opcode>
 */
VMCASE(COP_L2I):
{
	/* Convert from long to integer */
	stacktop[-CVM_WORDS_PER_LONG].intValue =
		(ILInt32)(ReadLong(&(stacktop[-CVM_WORDS_PER_LONG])));
	MODIFY_PC_AND_STACK(1, -(CVM_WORDS_PER_LONG - 1));
}
VMBREAK;

/**
 * <opcode name="l2f" group="Conversion operators">
 *   <operation>Convert <code>int64</code> to
 *				<code>native float</code></operation>
 *
 *   <format>l2f</format>
 *
 *   <form name="l2f" code="COP_L2F"/>
 *
 *   <before>..., value</before>
 *   <after>..., result</after>
 *
 *   <description>The <i>value</i> is popped from the stack as
 *   type <code>int64</code>, and converted into a <code>native float</code>
 *   <i>result</i>.  The <i>result</i> is pushed onto the stack.</description>
 *
 *   <notes>The precision of the <code>native float</code> type is
 *   platform-dependent.  On some platforms, it may be sufficient to
 *   represent all <code>int64</code> values, and on other platforms it
 *   may round large values.  Programs should not rely upon precise
 *   conversions from <code>int64</code> to <code>native float</code>.</notes>
 * </opcode>
 */
VMCASE(COP_L2F):
{
	/* Convert from long to "native float" */
	WriteFloat(&(stacktop[-CVM_WORDS_PER_LONG]),
	   (ILNativeFloat)(ReadLong(&(stacktop[-CVM_WORDS_PER_LONG]))));
	MODIFY_PC_AND_STACK(1, CVM_WORDS_PER_NATIVE_FLOAT -
						   CVM_WORDS_PER_LONG);
}
VMBREAK;

/**
 * <opcode name="lu2f" group="Conversion operators">
 *   <operation>Convert <code>uint64</code> to
 *				<code>native float</code></operation>
 *
 *   <format>lu2f</format>
 *
 *   <form name="lu2f" code="COP_LU2F"/>
 *
 *   <before>..., value</before>
 *   <after>..., result</after>
 *
 *   <description>The <i>value</i> is popped from the stack as
 *   type <code>uint64</code>, and converted into a <code>native float</code>
 *   <i>result</i>.  The <i>result</i> is pushed onto the stack.</description>
 *
 *   <notes>The precision of the <code>native float</code> type is
 *   platform-dependent.  On some platforms, it may be sufficient to
 *   represent all <code>uint64</code> values, and on other platforms it
 *   may round large values.  Programs should not rely upon precise
 *   conversions from <code>uint64</code> to <code>native float</code>.</notes>
 * </opcode>
 */
VMCASE(COP_LU2F):
{
	/* Convert from unsigned long to "native float" */
	WriteFloat(&(stacktop[-CVM_WORDS_PER_LONG]),
			   LU2F(ReadULong(&(stacktop[-CVM_WORDS_PER_LONG]))));
	MODIFY_PC_AND_STACK(1, CVM_WORDS_PER_NATIVE_FLOAT -
						   CVM_WORDS_PER_LONG);
}
VMBREAK;

/**
 * <opcode name="f2i" group="Conversion operators">
 *   <operation>Convert <code>native float</code> to
 *				<code>int32</code></operation>
 *
 *   <format>f2i</format>
 *
 *   <form name="f2i" code="COP_F2I"/>
 *
 *   <before>..., value</before>
 *   <after>..., result</after>
 *
 *   <description>The <i>value</i> is popped from the stack as
 *   type <code>native float</code>, and converted into an <code>int32</code>
 *   <i>result</i>.  The <i>result</i> is pushed onto the stack.</description>
 * </opcode>
 */
VMCASE(COP_F2I):
{
	/* Convert from "native float" to integer */
	stacktop[-CVM_WORDS_PER_NATIVE_FLOAT].intValue = (ILInt32)
		(ReadFloat(&(stacktop[-CVM_WORDS_PER_NATIVE_FLOAT])));
	MODIFY_PC_AND_STACK(1, -(CVM_WORDS_PER_NATIVE_FLOAT - 1));
}
VMBREAK;

/**
 * <opcode name="f2iu" group="Conversion operators">
 *   <operation>Convert <code>native float</code> to
 *				<code>uint32</code></operation>
 *
 *   <format>f2iu</format>
 *
 *   <form name="f2iu" code="COP_F2IU"/>
 *
 *   <before>..., value</before>
 *   <after>..., result</after>
 *
 *   <description>The <i>value</i> is popped from the stack as
 *   type <code>native float</code>, and converted into an <code>uint32</code>
 *   <i>result</i>.  The <i>result</i> is pushed onto the stack.</description>
 * </opcode>
 */
VMCASE(COP_F2IU):
{
	/* Convert from "native float" to unsigned integer */
	stacktop[-CVM_WORDS_PER_NATIVE_FLOAT].uintValue = (ILUInt32)
		(ReadFloat(&(stacktop[-CVM_WORDS_PER_NATIVE_FLOAT])));
	MODIFY_PC_AND_STACK(1, -(CVM_WORDS_PER_NATIVE_FLOAT - 1));
}
VMBREAK;

/**
 * <opcode name="f2l" group="Conversion operators">
 *   <operation>Convert <code>native float</code> to
 *				<code>int64</code></operation>
 *
 *   <format>f2l</format>
 *
 *   <form name="f2l" code="COP_F2L"/>
 *
 *   <before>..., value</before>
 *   <after>..., result</after>
 *
 *   <description>The <i>value</i> is popped from the stack as
 *   type <code>native float</code>, and converted into an <code>int64</code>
 *   <i>result</i>.  The <i>result</i> is pushed onto the stack.</description>
 * </opcode>
 */
VMCASE(COP_F2L):
{
	/* Convert from "native float" to long */
	WriteLong(&(stacktop[-CVM_WORDS_PER_NATIVE_FLOAT]), (ILInt64)
		ReadFloat(&(stacktop[-CVM_WORDS_PER_NATIVE_FLOAT])));
	MODIFY_PC_AND_STACK(1, -(CVM_WORDS_PER_NATIVE_FLOAT -
							 CVM_WORDS_PER_LONG));
}
VMBREAK;

/**
 * <opcode name="f2lu" group="Conversion operators">
 *   <operation>Convert <code>native float</code> to
 *				<code>uint64</code></operation>
 *
 *   <format>f2lu</format>
 *
 *   <form name="f2lu" code="COP_F2LU"/>
 *
 *   <before>..., value</before>
 *   <after>..., result</after>
 *
 *   <description>The <i>value</i> is popped from the stack as
 *   type <code>native float</code>, and converted into an <code>uint64</code>
 *   <i>result</i>.  The <i>result</i> is pushed onto the stack.</description>
 * </opcode>
 */
VMCASE(COP_F2LU):
{
	/* Convert from "native float" to unsigned long */
	WriteULong(&(stacktop[-CVM_WORDS_PER_NATIVE_FLOAT]),
			   F2LU(ReadFloat(&(stacktop[-CVM_WORDS_PER_NATIVE_FLOAT]))));
	MODIFY_PC_AND_STACK(1, -(CVM_WORDS_PER_NATIVE_FLOAT -
							 CVM_WORDS_PER_LONG));
}
VMBREAK;

/**
 * <opcode name="f2f" group="Conversion operators">
 *   <operation>Convert <code>native float</code> to
 *				<code>float32</code></operation>
 *
 *   <format>f2f</format>
 *
 *   <form name="f2f" code="COP_F2F"/>
 *
 *   <before>..., value</before>
 *   <after>..., result</after>
 *
 *   <description>The <i>value</i> is popped from the stack as
 *   type <code>native float</code>, truncated to <code>float32</code>,
 *   and then converted into a <code>native float</code> <i>result</i>.
 *   The <i>result</i> is pushed onto the stack.</description>
 * </opcode>
 */
VMCASE(COP_F2F):
{
	/* Convert from "native float" to "float" */
	WriteFloat(&(stacktop[-CVM_WORDS_PER_NATIVE_FLOAT]),
		(ILNativeFloat)(ILFloat)
		ReadFloat(&(stacktop[-CVM_WORDS_PER_NATIVE_FLOAT])));
	MODIFY_PC_AND_STACK(1, 0);
}
VMBREAK;

/**
 * <opcode name="f2d" group="Conversion operators">
 *   <operation>Convert <code>native float</code> to
 *				<code>float64</code></operation>
 *
 *   <format>f2d</format>
 *
 *   <form name="f2d" code="COP_F2D"/>
 *
 *   <before>..., value</before>
 *   <after>..., result</after>
 *
 *   <description>The <i>value</i> is popped from the stack as
 *   type <code>native float</code>, truncated to <code>float64</code>,
 *   and then converted into a <code>native float</code> <i>result</i>.
 *   The <i>result</i> is pushed onto the stack.</description>
 * </opcode>
 */
VMCASE(COP_F2D):
{
	/* Convert from "native float" to "double" */
	WriteFloat(&(stacktop[-CVM_WORDS_PER_NATIVE_FLOAT]),
		(ILNativeFloat)(ILDouble)
		ReadFloat(&(stacktop[-CVM_WORDS_PER_NATIVE_FLOAT])));
	MODIFY_PC_AND_STACK(1, 0);
}
VMBREAK;

/**
 * <opcode name="i2p_lower" group="Conversion operators">
 *   <operation>Convert <code>uint32</code> to <code>ptr</code>
 *				at some point lower down on the stack</operation>
 *
 *   <format>i2p_lower<fsep/>N[1]</format>
 *   <format>wide<fsep/>i2p_lower<fsep/>N[4]</format>
 *
 *   <form name="f2d" code="COP_F2D"/>
 *
 *   <before>..., value, val1, ..., valN</before>
 *   <after>..., result, val1, ..., valN</after>
 *
 *   <description>The <i>value</i> at stack word <i>N</i> positions
 *   down from the top of the stack is converted from <code>uint32</code>
 *   into a <code>ptr</code> <i>result</i>.  <i>N == 0</i> indicates
 *   that <i>value</i> is on the top of the stack.</description>
 *
 *   <notes>This is typically used to convert CIL "I4" values into "I" values
 *   for use in unmanaged pointer operations.</notes>
 * </opcode>
 */
VMCASE(COP_I2P_LOWER):
{
	/* Convert an I4 value into a pointer value that
	   is lower down on the stack */
	position = -(((ILInt32)(pc[1])) + 1);
	stacktop[position].ptrValue = (void *)(stacktop[position].uintValue);
	MODIFY_PC_AND_STACK(2, 0);
}
VMBREAK;

/**
 * <opcode name="fix_i4_i" group="Conversion operators">
 *   <operation>Convert a <code>int32</code>/<code>native int</code> pair
 *   of values into <code>native int</code>/<code>native int</code></operation>
 *
 *   <format>fix_i4_i</format>
 *
 *   <form name="fix_i4_i" code="COP_FIX_I4_I"/>
 *
 *   <before>..., value1, value2</before>
 *   <after>..., result, value2</after>
 *
 *   <description>Both <i>value1</i> and <i>value2</i> are popped from
 *   the stack as types <code>int32</code> and <code>native int</code>
 *   respectively.  The <code>native int</code> <i>result</i> is formed
 *   by sign-extending <i>value1</i>.  Then, <i>result</i> and <i>value2</i>
 *   are pushed onto the stack.</description>
 *
 *   <notes>This is typically used to promote CIL I4 values to I when
 *   used with a binary arithmetic operation.<p/>
 *
 *   On 32-bit platforms, this instruction will typically do nothing
 *   because the <code>int32</code> and <code>native int</code> types
 *   will be identical.<p/>
 *
 *   There is no <i>fix_i_i4</i> instruction because <i>i2l</i> can
 *   be used to acheive the same result on 64-bit platforms.</notes>
 * </opcode>
 */
VMCASE(COP_FIX_I4_I):
{
	/* Fix a (I4, I) pair on the stack to be (I, I) */
#ifdef IL_NATIVE_INT64
	WriteLong(&(stacktop[-1]),
		ReadLong(&(stacktop[-CVM_WORDS_PER_LONG])));
	WriteLong(&(stacktop[-(CVM_WORDS_PER_LONG + 1)]),
		(ILInt64)(stacktop[-(CVM_WORDS_PER_LONG + 1)].intValue));
	MODIFY_PC_AND_STACK(1, CVM_WORDS_PER_LONG - 1);
#else
	MODIFY_PC_AND_STACK(1, 0);
#endif
}
VMBREAK;

/**
 * <opcode name="fix_i4_u" group="Conversion operators">
 *   <operation>Convert a <code>uint32</code>/<code>native uint</code> pair of
 *   values into <code>native uint</code>/<code>native uint</code></operation>
 *
 *   <format>fix_i4_u</format>
 *
 *   <form name="fix_i4_u" code="COP_FIX_I4_U"/>
 *
 *   <before>..., value1, value2</before>
 *   <after>..., result, value2</after>
 *
 *   <description>Both <i>value1</i> and <i>value2</i> are popped from
 *   the stack as types <code>uint32</code> and <code>native uint</code>
 *   respectively.  The <code>native uint</code> <i>result</i> is formed
 *   by zero-extending <i>value1</i>.  Then, <i>result</i> and <i>value2</i>
 *   are pushed onto the stack.</description>
 *
 *   <notes>This is typically used to promote CIL I4 values to U when
 *   used with a binary arithmetic operation.<p/>
 *
 *   On 32-bit platforms, this instruction will typically do nothing
 *   because the <code>uint32</code> and <code>native uint</code> types
 *   will be identical.<p/>
 *
 *   There is no <i>fix_u_i4</i> instruction because <i>iu2l</i> can
 *   be used to acheive the same result on 64-bit platforms.</notes>
 * </opcode>
 */
VMCASE(COP_FIX_I4_U):
{
	/* Fix a (I4, U) pair on the stack to be (U, U) */
#ifdef IL_NATIVE_INT64
	WriteULong(&(stacktop[-1]),
		ReadULong(&(stacktop[-CVM_WORDS_PER_LONG])));
	WriteULong(&(stacktop[-(CVM_WORDS_PER_LONG + 1)]),
		(ILUInt64)(stacktop[-(CVM_WORDS_PER_LONG + 1)].uintValue));
	MODIFY_PC_AND_STACK(1, CVM_WORDS_PER_LONG - 1);
#else
	MODIFY_PC_AND_STACK(1, 0);
#endif
}
VMBREAK;

#elif defined(IL_CVM_WIDE)

case COP_I2P_LOWER:
{
	/* Convert an I4 value into a pointer value that
	   is lower down on the stack */
	position = -(IL_READ_INT32(pc + 2) + 1);
	stacktop[position].ptrValue = (void *)(stacktop[position].uintValue);
	MODIFY_PC_AND_STACK(6, 0);
}
VMBREAK;

#elif defined(IL_CVM_PREFIX)

/**
 * <opcode name="i2b_ovf" group="Conversion operators">
 *   <operation>Convert <code>int32</code> to <code>int8</code>
 *              with overflow detection</operation>
 *
 *   <format>prefix<fsep/>i2b_ovf</format>
 *
 *   <form name="i2b_ovf" code="COP_PREFIX_I2B_OVF"/>
 *
 *   <before>..., value</before>
 *   <after>..., result</after>
 *
 *   <description>The <i>value</i> is popped from the stack as
 *   type <code>int32</code>.  The <code>int32</code> <i>result</i>
 *   is formed by truncating <i>value</i> to 8 bits and then
 *   sign-extending it to 32 bits.  The <i>result</i> is pushed
 *   onto the stack.  If <i>result</i> does not have the same numeric
 *   value as <i>value</i>, then <code>System.OverflowException</code>
 *   is thrown.</description>
 *
 *   <exceptions>
 *     <exception name="System.OverflowException">Raised if <i>value</i>
 *     is out of range for <code>int8</code>.</exception>
 *   </exceptions>
 * </opcode>
 */
VMCASE(COP_PREFIX_I2B_OVF):
{
	/* Convert "int" into "sbyte" with overflow testing */
	if(stacktop[-1].intValue >= -128 && stacktop[-1].intValue <= 127)
	{
		MODIFY_PC_AND_STACK(2, 0);
	}
	else
	{
		OVERFLOW_EXCEPTION();
	}
}
VMBREAK;

/**
 * <opcode name="i2ub_ovf" group="Conversion operators">
 *   <operation>Convert <code>int32</code> to <code>uint8</code>
 *              with overflow detection</operation>
 *
 *   <format>prefix<fsep/>i2ub_ovf</format>
 *
 *   <form name="i2ub_ovf" code="COP_PREFIX_I2UB_OVF"/>
 *
 *   <before>..., value</before>
 *   <after>..., result</after>
 *
 *   <description>The <i>value</i> is popped from the stack as
 *   type <code>int32</code>.  The <code>int32</code> <i>result</i>
 *   is formed by truncating <i>value</i> to 8 bits and then
 *   zero-extending it to 32 bits.  The <i>result</i> is pushed
 *   onto the stack.  If <i>result</i> does not have the same numeric
 *   value as <i>value</i>, then <code>System.OverflowException</code>
 *   is thrown.</description>
 *
 *   <exceptions>
 *     <exception name="System.OverflowException">Raised if <i>value</i>
 *     is out of range for <code>uint8</code>.</exception>
 *   </exceptions>
 * </opcode>
 */
VMCASE(COP_PREFIX_I2UB_OVF):
{
	/* Convert "int" into "byte" with overflow testing */
	if(stacktop[-1].intValue >= 0 && stacktop[-1].intValue <= 255)
	{
		MODIFY_PC_AND_STACK(2, 0);
	}
	else
	{
		OVERFLOW_EXCEPTION();
	}
}
VMBREAK;

/**
 * <opcode name="iu2b_ovf" group="Conversion operators">
 *   <operation>Convert <code>uint32</code> to <code>int8</code>
 *              with overflow detection</operation>
 *
 *   <format>prefix<fsep/>iu2b_ovf</format>
 *
 *   <form name="iu2b_ovf" code="COP_PREFIX_IU2B_OVF"/>
 *
 *   <before>..., value</before>
 *   <after>..., result</after>
 *
 *   <description>The <i>value</i> is popped from the stack as
 *   type <code>uint32</code>.  If <i>value</i> is greater than 127,
 *   then <code>System.OverflowException</code> is thrown.  Otherwise
 *   <i>result</i> is <i>value</i>.  The <i>result</i> is pushed onto
 *   the stack.</description>
 *
 *   <exceptions>
 *     <exception name="System.OverflowException">Raised if <i>value</i>
 *     is out of range for <code>int8</code>.</exception>
 *   </exceptions>
 * </opcode>
 */
VMCASE(COP_PREFIX_IU2B_OVF):
{
	/* Convert "uint" into "sbyte" with overflow testing */
	if(stacktop[-1].intValue >= 0 && stacktop[-1].intValue <= 127)
	{
		MODIFY_PC_AND_STACK(2, 0);
	}
	else
	{
		OVERFLOW_EXCEPTION();
	}
}
VMBREAK;

/**
 * <opcode name="iu2ub_ovf" group="Conversion operators">
 *   <operation>Convert <code>uint32</code> to <code>uint8</code>
 *              with overflow detection</operation>
 *
 *   <format>prefix<fsep/>iu2ub_ovf</format>
 *
 *   <form name="iu2ub_ovf" code="COP_PREFIX_IU2UB_OVF"/>
 *
 *   <before>..., value</before>
 *   <after>..., result</after>
 *
 *   <description>The <i>value</i> is popped from the stack as
 *   type <code>uint32</code>.  If <i>value</i> is greater than 255,
 *   then <code>System.OverflowException</code> is thrown.  Otherwise
 *   <i>result</i> is <i>value</i>.  The <i>result</i> is pushed onto
 *   the stack.</description>
 *
 *   <exceptions>
 *     <exception name="System.OverflowException">Raised if <i>value</i>
 *     is out of range for <code>uint8</code>.</exception>
 *   </exceptions>
 * </opcode>
 */
VMCASE(COP_PREFIX_IU2UB_OVF):
{
	/* Convert "uint" into "byte" with overflow testing */
	if(stacktop[-1].intValue >= 0 && stacktop[-1].intValue <= 255)
	{
		MODIFY_PC_AND_STACK(2, 0);
	}
	else
	{
		OVERFLOW_EXCEPTION();
	}
}
VMBREAK;

/**
 * <opcode name="i2s_ovf" group="Conversion operators">
 *   <operation>Convert <code>int32</code> to <code>int16</code>
 *              with overflow detection</operation>
 *
 *   <format>prefix<fsep/>i2s_ovf</format>
 *
 *   <form name="i2s_ovf" code="COP_PREFIX_I2S_OVF"/>
 *
 *   <before>..., value</before>
 *   <after>..., result</after>
 *
 *   <description>The <i>value</i> is popped from the stack as
 *   type <code>int32</code>.  The <code>int32</code> <i>result</i>
 *   is formed by truncating <i>value</i> to 16 bits and then
 *   sign-extending it to 32 bits.  The <i>result</i> is pushed
 *   onto the stack.  If <i>result</i> does not have the same numeric
 *   value as <i>value</i>, then <code>System.OverflowException</code>
 *   is thrown.</description>
 *
 *   <exceptions>
 *     <exception name="System.OverflowException">Raised if <i>value</i>
 *     is out of range for <code>int16</code>.</exception>
 *   </exceptions>
 * </opcode>
 */
VMCASE(COP_PREFIX_I2S_OVF):
{
	/* Convert "int" into "short" with overflow testing */
	if(stacktop[-1].intValue >= -32768 && stacktop[-1].intValue <= 32767)
	{
		MODIFY_PC_AND_STACK(2, 0);
	}
	else
	{
		OVERFLOW_EXCEPTION();
	}
}
VMBREAK;

/**
 * <opcode name="i2us_ovf" group="Conversion operators">
 *   <operation>Convert <code>int32</code> to <code>uint16</code>
 *              with overflow detection</operation>
 *
 *   <format>prefix<fsep/>i2us_ovf</format>
 *
 *   <form name="i2us_ovf" code="COP_PREFIX_I2US_OVF"/>
 *
 *   <before>..., value</before>
 *   <after>..., result</after>
 *
 *   <description>The <i>value</i> is popped from the stack as
 *   type <code>int32</code>.  The <code>int32</code> <i>result</i>
 *   is formed by truncating <i>value</i> to 16 bits and then
 *   zero-extending it to 32 bits.  The <i>result</i> is pushed
 *   onto the stack.  If <i>result</i> does not have the same numeric
 *   value as <i>value</i>, then <code>System.OverflowException</code>
 *   is thrown.</description>
 *
 *   <exceptions>
 *     <exception name="System.OverflowException">Raised if <i>value</i>
 *     is out of range for <code>uint16</code>.</exception>
 *   </exceptions>
 * </opcode>
 */
VMCASE(COP_PREFIX_I2US_OVF):
{
	/* Convert "int" into "ushort" with overflow testing */
	if(stacktop[-1].intValue >= 0 && stacktop[-1].intValue <= 65535)
	{
		MODIFY_PC_AND_STACK(2, 0);
	}
	else
	{
		OVERFLOW_EXCEPTION();
	}
}
VMBREAK;

/**
 * <opcode name="iu2s_ovf" group="Conversion operators">
 *   <operation>Convert <code>uint32</code> to <code>int16</code>
 *              with overflow detection</operation>
 *
 *   <format>prefix<fsep/>iu2s_ovf</format>
 *
 *   <form name="iu2s_ovf" code="COP_PREFIX_IU2S_OVF"/>
 *
 *   <before>..., value</before>
 *   <after>..., result</after>
 *
 *   <description>The <i>value</i> is popped from the stack as
 *   type <code>uint32</code>.  If <i>value</i> is greater than 32767,
 *   then <code>System.OverflowException</code> is thrown.  Otherwise
 *   <i>result</i> is <i>value</i>.  The <i>result</i> is pushed onto
 *   the stack.</description>
 *
 *   <exceptions>
 *     <exception name="System.OverflowException">Raised if <i>value</i>
 *     is out of range for <code>int16</code>.</exception>
 *   </exceptions>
 * </opcode>
 */
VMCASE(COP_PREFIX_IU2S_OVF):
{
	/* Convert "uint" into "short" with overflow testing */
	if(stacktop[-1].intValue >= 0 && stacktop[-1].intValue <= 32767)
	{
		MODIFY_PC_AND_STACK(2, 0);
	}
	else
	{
		OVERFLOW_EXCEPTION();
	}
}
VMBREAK;

/**
 * <opcode name="iu2us_ovf" group="Conversion operators">
 *   <operation>Convert <code>uint32</code> to <code>uint16</code>
 *              with overflow detection</operation>
 *
 *   <format>prefix<fsep/>iu2us_ovf</format>
 *
 *   <form name="iu2us_ovf" code="COP_PREFIX_IU2US_OVF"/>
 *
 *   <before>..., value</before>
 *   <after>..., result</after>
 *
 *   <description>The <i>value</i> is popped from the stack as
 *   type <code>uint32</code>.  If <i>value</i> is greater than 65535,
 *   then <code>System.OverflowException</code> is thrown.  Otherwise
 *   <i>result</i> is <i>value</i>.  The <i>result</i> is pushed onto
 *   the stack.</description>
 *
 *   <exceptions>
 *     <exception name="System.OverflowException">Raised if <i>value</i>
 *     is out of range for <code>uint16</code>.</exception>
 *   </exceptions>
 * </opcode>
 */
VMCASE(COP_PREFIX_IU2US_OVF):
{
	/* Convert "uint" into "ushort" with overflow testing */
	if(stacktop[-1].intValue >= 0 && stacktop[-1].intValue <= 65535)
	{
		MODIFY_PC_AND_STACK(2, 0);
	}
	else
	{
		OVERFLOW_EXCEPTION();
	}
}
VMBREAK;

/**
 * <opcode name="i2iu_ovf" group="Conversion operators">
 *   <operation>Convert <code>int32</code> to <code>uint32</code>
 *              with overflow detection</operation>
 *
 *   <format>prefix<fsep/>i2iu_ovf</format>
 *
 *   <form name="i2iu_ovf" code="COP_PREFIX_I2IU_OVF"/>
 *
 *   <before>..., value</before>
 *   <after>..., result</after>
 *
 *   <description>The <i>value</i> is popped from the stack as
 *   type <code>int32</code>.  If <i>value</i> is less than zero,
 *   then <code>System.OverflowException</code> is thrown.  Otherwise
 *   <i>result</i> is <i>value</i>.  The <i>result</i> is pushed onto
 *   the stack.</description>
 *
 *   <exceptions>
 *     <exception name="System.OverflowException">Raised if <i>value</i>
 *     is out of range for <code>uint32</code>.</exception>
 *   </exceptions>
 * </opcode>
 */
VMCASE(COP_PREFIX_I2IU_OVF):
{
	/* Convert "int" into "uint" with overflow testing */
	if(stacktop[-1].intValue >= 0)
	{
		MODIFY_PC_AND_STACK(2, 0);
	}
	else
	{
		OVERFLOW_EXCEPTION();
	}
}
VMBREAK;

/**
 * <opcode name="iu2i_ovf" group="Conversion operators">
 *   <operation>Convert <code>uint32</code> to <code>int32</code>
 *              with overflow detection</operation>
 *
 *   <format>prefix<fsep/>iu2i_ovf</format>
 *
 *   <form name="iu2i_ovf" code="COP_PREFIX_IU2I_OVF"/>
 *
 *   <before>..., value</before>
 *   <after>..., result</after>
 *
 *   <description>The <i>value</i> is popped from the stack as
 *   type <code>uint32</code>.  If <i>value</i> is greater than 2147483647,
 *   then <code>System.OverflowException</code> is thrown.  Otherwise
 *   <i>result</i> is <i>value</i>.  The <i>result</i> is pushed onto
 *   the stack.</description>
 *
 *   <exceptions>
 *     <exception name="System.OverflowException">Raised if <i>value</i>
 *     is out of range for <code>int32</code>.</exception>
 *   </exceptions>
 * </opcode>
 */
VMCASE(COP_PREFIX_IU2I_OVF):
{
	/* Convert "uint" into "int" with overflow testing */
	if(stacktop[-1].intValue >= 0)
	{
		MODIFY_PC_AND_STACK(2, 0);
	}
	else
	{
		OVERFLOW_EXCEPTION();
	}
}
VMBREAK;

/**
 * <opcode name="i2ul_ovf" group="Conversion operators">
 *   <operation>Convert <code>int32</code> to <code>uint64</code>
 *              with overflow detection</operation>
 *
 *   <format>prefix<fsep/>i2ul_ovf</format>
 *
 *   <form name="i2ul_ovf" code="COP_PREFIX_I2UL_OVF"/>
 *
 *   <before>..., value</before>
 *   <after>..., result</after>
 *
 *   <description>The <i>value</i> is popped from the stack as
 *   type <code>int32</code>.  If <i>value</i> is negative,
 *   then <code>System.OverflowException</code> is thrown.  Otherwise
 *   <i>result</i> is <i>value</i>, zero-extended to 64 bits.
 *   The <i>result</i> is pushed onto the stack.</description>
 *
 *   <exceptions>
 *     <exception name="System.OverflowException">Raised if <i>value</i>
 *     is out of range for <code>uint64</code>.</exception>
 *   </exceptions>
 * </opcode>
 */
VMCASE(COP_PREFIX_I2UL_OVF):
{
	/* Convert "int" into "ulong" with overflow testing */
	if(stacktop[-1].intValue >= 0)
	{
		WriteLong(&(stacktop[-1]), (ILInt64)(stacktop[-1].intValue));
		MODIFY_PC_AND_STACK(2, CVM_WORDS_PER_LONG - 1);
	}
	else
	{
		OVERFLOW_EXCEPTION();
	}
}
VMBREAK;

/**
 * <opcode name="l2i_ovf" group="Conversion operators">
 *   <operation>Convert <code>int64</code> to <code>int32</code>
 *              with overflow detection</operation>
 *
 *   <format>prefix<fsep/>l2i_ovf</format>
 *
 *   <form name="l2i_ovf" code="COP_PREFIX_L2I_OVF"/>
 *
 *   <before>..., value</before>
 *   <after>..., result</after>
 *
 *   <description>The <i>value</i> is popped from the stack as
 *   type <code>int64</code>.  If <i>value</i> is less than -2147483648,
 *   or greater than 2147483647, then <code>System.OverflowException</code>
 *   is thrown.  Otherwise <i>result</i> is <i>value</i>, truncated
 *   to 32 bits.  The <i>result</i> is pushed onto the stack.</description>
 *
 *   <exceptions>
 *     <exception name="System.OverflowException">Raised if <i>value</i>
 *     is out of range for <code>int32</code>.</exception>
 *   </exceptions>
 * </opcode>
 */
VMCASE(COP_PREFIX_L2I_OVF):
{
	/* Convert "long" into "int" with overflow testing */
	if(L2IOvf(&(stacktop[-CVM_WORDS_PER_LONG])))
	{
		MODIFY_PC_AND_STACK(2, 1 - CVM_WORDS_PER_LONG);
	}
	else
	{
		OVERFLOW_EXCEPTION();
	}
}
VMBREAK;

/**
 * <opcode name="l2ui_ovf" group="Conversion operators">
 *   <operation>Convert <code>int64</code> to <code>uint32</code>
 *              with overflow detection</operation>
 *
 *   <format>prefix<fsep/>l2ui_ovf</format>
 *
 *   <form name="l2ui_ovf" code="COP_PREFIX_L2UI_OVF"/>
 *
 *   <before>..., value</before>
 *   <after>..., result</after>
 *
 *   <description>The <i>value</i> is popped from the stack as
 *   type <code>int64</code>.  If <i>value</i> is less than zero or
 *   greater than 4294967295, then <code>System.OverflowException</code>
 *   is thrown.  Otherwise <i>result</i> is <i>value</i>, truncated
 *   to 32 bits.  The <i>result</i> is pushed onto the stack.</description>
 *
 *   <exceptions>
 *     <exception name="System.OverflowException">Raised if <i>value</i>
 *     is out of range for <code>uint32</code>.</exception>
 *   </exceptions>
 * </opcode>
 */
VMCASE(COP_PREFIX_L2UI_OVF):
{
	/* Convert "long" into "uint" with overflow testing */
	if(L2UIOvf(&(stacktop[-CVM_WORDS_PER_LONG])))
	{
		MODIFY_PC_AND_STACK(2, 1 - CVM_WORDS_PER_LONG);
	}
	else
	{
		OVERFLOW_EXCEPTION();
	}
}
VMBREAK;

/**
 * <opcode name="lu2i_ovf" group="Conversion operators">
 *   <operation>Convert <code>uint64</code> to <code>int32</code>
 *              with overflow detection</operation>
 *
 *   <format>prefix<fsep/>lu2i_ovf</format>
 *
 *   <form name="lu2i_ovf" code="COP_PREFIX_LU2I_OVF"/>
 *
 *   <before>..., value</before>
 *   <after>..., result</after>
 *
 *   <description>The <i>value</i> is popped from the stack as
 *   type <code>uint64</code>.  If <i>value</i> is greater than
 *   2147483647, then <code>System.OverflowException</code>
 *   is thrown.  Otherwise <i>result</i> is <i>value</i>, truncated
 *   to 32 bits.  The <i>result</i> is pushed onto the stack.</description>
 *
 *   <exceptions>
 *     <exception name="System.OverflowException">Raised if <i>value</i>
 *     is out of range for <code>int32</code>.</exception>
 *   </exceptions>
 * </opcode>
 */
VMCASE(COP_PREFIX_LU2I_OVF):
{
	/* Convert "ulong" into "int" with overflow testing */
	if(LU2IOvf(&(stacktop[-CVM_WORDS_PER_LONG])))
	{
		MODIFY_PC_AND_STACK(2, 1 - CVM_WORDS_PER_LONG);
	}
	else
	{
		OVERFLOW_EXCEPTION();
	}
}
VMBREAK;

/**
 * <opcode name="lu2iu_ovf" group="Conversion operators">
 *   <operation>Convert <code>uint64</code> to <code>uint32</code>
 *              with overflow detection</operation>
 *
 *   <format>prefix<fsep/>lu2iu_ovf</format>
 *
 *   <form name="lu2iu_ovf" code="COP_PREFIX_LU2IU_OVF"/>
 *
 *   <before>..., value</before>
 *   <after>..., result</after>
 *
 *   <description>The <i>value</i> is popped from the stack as
 *   type <code>uint64</code>.  If <i>value</i> is greater than
 *   4294967295, then <code>System.OverflowException</code>
 *   is thrown.  Otherwise <i>result</i> is <i>value</i>, truncated
 *   to 32 bits.  The <i>result</i> is pushed onto the stack.</description>
 *
 *   <exceptions>
 *     <exception name="System.OverflowException">Raised if <i>value</i>
 *     is out of range for <code>uint32</code>.</exception>
 *   </exceptions>
 * </opcode>
 */
VMCASE(COP_PREFIX_LU2IU_OVF):
{
	/* Convert "ulong" into "uint" with overflow testing */
	if(LU2UIOvf(&(stacktop[-CVM_WORDS_PER_LONG])))
	{
		MODIFY_PC_AND_STACK(2, 1 - CVM_WORDS_PER_LONG);
	}
	else
	{
		OVERFLOW_EXCEPTION();
	}
}
VMBREAK;

/**
 * <opcode name="l2ul_ovf" group="Conversion operators">
 *   <operation>Convert <code>int64</code> to <code>uint64</code>
 *              with overflow detection</operation>
 *
 *   <format>prefix<fsep/>l2ul_ovf</format>
 *
 *   <form name="l2ul_ovf" code="COP_PREFIX_L2UL_OVF"/>
 *
 *   <before>..., value</before>
 *   <after>..., result</after>
 *
 *   <description>The <i>value</i> is popped from the stack as
 *   type <code>int64</code>.  If <i>value</i> is less than zero,
 *   then <code>System.OverflowException</code> is thrown.
 *   Otherwise <i>result</i> is <i>value</i>.  The <i>result</i>
 *   is pushed onto the stack.</description>
 *
 *   <exceptions>
 *     <exception name="System.OverflowException">Raised if <i>value</i>
 *     is out of range for <code>uint64</code>.</exception>
 *   </exceptions>
 * </opcode>
 */
VMCASE(COP_PREFIX_L2UL_OVF):
{
	/* Convert "long" into "ulong" with overflow testing */
	if(ReadLong(&(stacktop[-CVM_WORDS_PER_LONG])) >= 0)
	{
		MODIFY_PC_AND_STACK(2, 0);
	}
	else
	{
		OVERFLOW_EXCEPTION();
	}
}
VMBREAK;

/**
 * <opcode name="lu2l_ovf" group="Conversion operators">
 *   <operation>Convert <code>uint64</code> to <code>int64</code>
 *              with overflow detection</operation>
 *
 *   <format>prefix<fsep/>lu2l_ovf</format>
 *
 *   <form name="lu2l_ovf" code="COP_PREFIX_LU2L_OVF"/>
 *
 *   <before>..., value</before>
 *   <after>..., result</after>
 *
 *   <description>The <i>value</i> is popped from the stack as
 *   type <code>uint64</code>.  If <i>value</i> is greater than
 *   9223372036854775807, then <code>System.OverflowException</code>
 *   is thrown.  Otherwise <i>result</i> is <i>value</i>.  The <i>result</i>
 *   is pushed onto the stack.</description>
 *
 *   <exceptions>
 *     <exception name="System.OverflowException">Raised if <i>value</i>
 *     is out of range for <code>int64</code>.</exception>
 *   </exceptions>
 * </opcode>
 */
VMCASE(COP_PREFIX_LU2L_OVF):
{
	/* Convert "ulong" into "long" with overflow testing */
	if(ReadULong(&(stacktop[-CVM_WORDS_PER_LONG])) <= (ILUInt64)IL_MAX_INT64)
	{
		MODIFY_PC_AND_STACK(2, 0);
	}
	else
	{
		OVERFLOW_EXCEPTION();
	}
}
VMBREAK;

/**
 * <opcode name="f2i_ovf" group="Conversion operators">
 *   <operation>Convert <code>native float</code> to <code>int32</code>
 *              with overflow detection</operation>
 *
 *   <format>prefix<fsep/>f2i_ovf</format>
 *
 *   <form name="f2i_ovf" code="COP_PREFIX_F2I_OVF"/>
 *
 *   <before>..., value</before>
 *   <after>..., result</after>
 *
 *   <description>The <i>value</i> is popped from the stack as
 *   type <code>native float</code>.  If <i>value</i> is not representable
 *   as a 32-bit integer, then <code>System.OverflowException</code>
 *   is thrown.  Otherwise <i>result</i> is <i>value</i>, converted to
 *   <code>int32</code>.  The <i>result</i> is pushed onto
 *   the stack.</description>
 *
 *   <exceptions>
 *     <exception name="System.OverflowException">Raised if <i>value</i>
 *     is out of range for <code>int32</code>.</exception>
 *   </exceptions>
 * </opcode>
 */
VMCASE(COP_PREFIX_F2I_OVF):
{
	/* Convert "native float" into "int" with overflow testing */
	if(F2IOvf(&(stacktop[-CVM_WORDS_PER_NATIVE_FLOAT])))
	{
		MODIFY_PC_AND_STACK(2, 1 - CVM_WORDS_PER_NATIVE_FLOAT);
	}
	else
	{
		OVERFLOW_EXCEPTION();
	}
}
VMBREAK;

/**
 * <opcode name="f2iu_ovf" group="Conversion operators">
 *   <operation>Convert <code>native float</code> to <code>uint32</code>
 *              with overflow detection</operation>
 *
 *   <format>prefix<fsep/>f2iu_ovf</format>
 *
 *   <form name="f2iu_ovf" code="COP_PREFIX_F2IU_OVF"/>
 *
 *   <before>..., value</before>
 *   <after>..., result</after>
 *
 *   <description>The <i>value</i> is popped from the stack as
 *   type <code>native float</code>.  If <i>value</i> is not representable
 *   as an unsigned 32-bit integer, then <code>System.OverflowException</code>
 *   is thrown.  Otherwise <i>result</i> is <i>value</i>, converted to
 *   <code>uint32</code>.  The <i>result</i> is pushed onto
 *   the stack.</description>
 *
 *   <exceptions>
 *     <exception name="System.OverflowException">Raised if <i>value</i>
 *     is out of range for <code>uint32</code>.</exception>
 *   </exceptions>
 * </opcode>
 */
VMCASE(COP_PREFIX_F2IU_OVF):
{
	/* Convert "native float" into "uint" with overflow testing */
	if(F2UIOvf(&(stacktop[-CVM_WORDS_PER_NATIVE_FLOAT])))
	{
		MODIFY_PC_AND_STACK(2, 1 - CVM_WORDS_PER_NATIVE_FLOAT);
	}
	else
	{
		OVERFLOW_EXCEPTION();
	}
}
VMBREAK;

/**
 * <opcode name="f2l_ovf" group="Conversion operators">
 *   <operation>Convert <code>native float</code> to <code>int64</code>
 *              with overflow detection</operation>
 *
 *   <format>prefix<fsep/>f2l_ovf</format>
 *
 *   <form name="f2l_ovf" code="COP_PREFIX_F2L_OVF"/>
 *
 *   <before>..., value</before>
 *   <after>..., result</after>
 *
 *   <description>The <i>value</i> is popped from the stack as
 *   type <code>native float</code>.  If <i>value</i> is not representable
 *   as a 64-bit integer, then <code>System.OverflowException</code>
 *   is thrown.  Otherwise <i>result</i> is <i>value</i>, converted to
 *   <code>int64</code>.  The <i>result</i> is pushed onto
 *   the stack.</description>
 *
 *   <exceptions>
 *     <exception name="System.OverflowException">Raised if <i>value</i>
 *     is out of range for <code>int64</code>.</exception>
 *   </exceptions>
 * </opcode>
 */
VMCASE(COP_PREFIX_F2L_OVF):
{
	/* Convert "native float" into "long" with overflow testing */
	if(F2LOvf(&(stacktop[-CVM_WORDS_PER_NATIVE_FLOAT])))
	{
		MODIFY_PC_AND_STACK(2, CVM_WORDS_PER_LONG - CVM_WORDS_PER_NATIVE_FLOAT);
	}
	else
	{
		OVERFLOW_EXCEPTION();
	}
}
VMBREAK;

/**
 * <opcode name="f2lu_ovf" group="Conversion operators">
 *   <operation>Convert <code>native float</code> to <code>uint64</code>
 *              with overflow detection</operation>
 *
 *   <format>prefix<fsep/>f2lu_ovf</format>
 *
 *   <form name="f2lu_ovf" code="COP_PREFIX_F2LU_OVF"/>
 *
 *   <before>..., value</before>
 *   <after>..., result</after>
 *
 *   <description>The <i>value</i> is popped from the stack as
 *   type <code>native float</code>.  If <i>value</i> is not representable
 *   as an unsigned 64-bit integer, then <code>System.OverflowException</code>
 *   is thrown.  Otherwise <i>result</i> is <i>value</i>, converted to
 *   <code>uint64</code>.  The <i>result</i> is pushed onto
 *   the stack.</description>
 *
 *   <exceptions>
 *     <exception name="System.OverflowException">Raised if <i>value</i>
 *     is out of range for <code>uint64</code>.</exception>
 *   </exceptions>
 * </opcode>
 */
VMCASE(COP_PREFIX_F2LU_OVF):
{
	/* Convert "native float" into "long" with overflow testing */
	if(F2LUOvf(&(stacktop[-CVM_WORDS_PER_NATIVE_FLOAT])))
	{
		MODIFY_PC_AND_STACK(2, CVM_WORDS_PER_LONG - CVM_WORDS_PER_NATIVE_FLOAT);
	}
	else
	{
		OVERFLOW_EXCEPTION();
	}
}
VMBREAK;

/**
 * <opcode name="i2b_aligned" group="Conversion operators">
 *   <operation>Convert <code>int32</code> to <code>int8</code>, aligned
 *              on a stack word boundary</operation>
 *
 *   <format>i2b_aligned</format>
 *
 *   <form name="i2b_aligned" code="COP_PREFIX_I2B_ALIGNED"/>
 *
 *   <before>..., value</before>
 *   <after>..., result</after>
 *
 *   <description>The <i>value</i> is popped from the stack as
 *   type <code>int32</code>.  The <code>int32</code> <i>result</i>
 *   is formed by truncating <i>value</i> to 8 bits.  The <i>result</i>
 *   is stored in the top-most stack position so that it is aligned
 *   with the beginning of the stack word.</description>
 *
 *   <notes>This instruction is used to align a value prior to boxing
 *   it with the <i>box</i> instruction.</notes>
 * </opcode>
 */
VMCASE(COP_PREFIX_I2B_ALIGNED):
{
	/* Convert a 32-bit value into a byte and align it on a word boundary */
	*((ILInt8 *)(stacktop - 1)) = (ILInt8)(stacktop[-1].intValue);
	MODIFY_PC_AND_STACK(2, 0);
}
VMBREAK;

/**
 * <opcode name="i2s_aligned" group="Conversion operators">
 *   <operation>Convert <code>int32</code> to <code>int16</code>, aligned
 *              on a stack word boundary</operation>
 *
 *   <format>prefix<fsep/>i2s_aligned</format>
 *
 *   <form name="i2s_aligned" code="COP_PREFIX_I2S_ALIGNED"/>
 *
 *   <before>..., value</before>
 *   <after>..., result</after>
 *
 *   <description>The <i>value</i> is popped from the stack as
 *   type <code>int32</code>.  The <code>int32</code> <i>result</i>
 *   is formed by truncating <i>value</i> to 16 bits.  The <i>result</i>
 *   is stored in the top-most stack position so that it is aligned
 *   with the beginning of the stack word.</description>
 *
 *   <notes>This instruction is used to align a value prior to boxing
 *   it with the <i>box</i> instruction.</notes>
 * </opcode>
 */
VMCASE(COP_PREFIX_I2S_ALIGNED):
{
	/* Convert a 32-bit value into a short and align it on a word boundary */
	*((ILInt16 *)(stacktop - 1)) = (ILInt16)(stacktop[-1].intValue);
	MODIFY_PC_AND_STACK(2, 0);
}
VMBREAK;

/**
 * <opcode name="f2f_aligned" group="Conversion operators">
 *   <operation>Convert <code>native float</code> to <code>float32</code>,
 *              aligned on a stack word boundary</operation>
 *
 *   <format>prefix<fsep/>f2f_aligned</format>
 *
 *   <form name="f2f_aligned" code="COP_PREFIX_F2F_ALIGNED"/>
 *
 *   <before>..., value</before>
 *   <after>..., result</after>
 *
 *   <description>The <i>value</i> is popped from the stack as
 *   type <code>native float</code>.  The <code>float32</code> <i>result</i>
 *   is formed by truncating <i>value</i> to 32 bits.  The <i>result</i>
 *   is stored in the top-most stack position so that it is aligned
 *   with the beginning of the stack word.</description>
 *
 *   <notes>This instruction is used to align a value prior to boxing
 *   it with the <i>box</i> instruction.<p/>
 *
 *   The <code>float32</code> type may occupy less stack words than
 *   the original <code>native float</code> value.  Excess stack words
 *   are popped from the stack.</notes>
 * </opcode>
 */
VMCASE(COP_PREFIX_F2F_ALIGNED):
{
	/* Convert a native float into a float32 and align it on a word boundary */
	*((ILFloat *)(stacktop - CVM_WORDS_PER_NATIVE_FLOAT)) =
			   ReadFloat(stacktop - CVM_WORDS_PER_NATIVE_FLOAT);
	MODIFY_PC_AND_STACK(2, CVM_WORDS_PER_FLOAT - CVM_WORDS_PER_NATIVE_FLOAT);
}
VMBREAK;

/**
 * <opcode name="f2d_aligned" group="Conversion operators">
 *   <operation>Convert <code>native float</code> to <code>float64</code>,
 *              aligned on a stack word boundary</operation>
 *
 *   <format>prefix<fsep/>f2d_aligned</format>
 *
 *   <form name="f2d_aligned" code="COP_PREFIX_F2D_ALIGNED"/>
 *
 *   <before>..., value</before>
 *   <after>..., result</after>
 *
 *   <description>The <i>value</i> is popped from the stack as
 *   type <code>native float</code>.  The <code>float64</code> <i>result</i>
 *   is formed by truncating <i>value</i> to 64 bits.  The <i>result</i>
 *   is stored in the top-most stack position so that it is aligned
 *   with the beginning of the stack word.</description>
 *
 *   <notes>This instruction is used to align a value prior to boxing
 *   it with the <i>box</i> instruction.<p/>
 *
 *   The <code>float64</code> type may occupy less stack words than
 *   the original <code>native float</code> value.  Excess stack words
 *   are popped from the stack.</notes>
 * </opcode>
 */
VMCASE(COP_PREFIX_F2D_ALIGNED):
{
	/* Convert a native float into a float64 and align it on a word boundary */
	WriteDouble(stacktop - CVM_WORDS_PER_NATIVE_FLOAT,
			    ReadFloat(stacktop - CVM_WORDS_PER_NATIVE_FLOAT));
	MODIFY_PC_AND_STACK(2, CVM_WORDS_PER_DOUBLE - CVM_WORDS_PER_NATIVE_FLOAT);
}
VMBREAK;

/**
 * <opcode name="str2ansi" group="Conversion operators">
 *   <operation>Convert <code>string</code> to <code>ansi char *</code>
 *              </operation>
 *
 *   <format>prefix<fsep/>str2ansi</format>
 *
 *   <form name="str2ansi" code="COP_PREFIX_STR2ANSI"/>
 *
 *   <before>..., value</before>
 *   <after>..., result</after>
 *
 *   <description>The <i>value</i> is popped from the stack as
 *   type <code>string</code>.  The string is converted into a
 *   <i>result</i>.  character buffer using the underlying platform's
 *   current locale settings.  A pointer to the buffer is pushed onto
 *   the stack as type <code>ptr</code>.</description>
 *
 *   <notes>This instruction is used to convert C# strings into
 *   character buffers during "PInvoke" marshalling operations.</notes>
 * </opcode>
 */
VMCASE(COP_PREFIX_STR2ANSI):
{
	/* Convert a string object into an "ANSI" character buffer */
	if(stacktop[-1].ptrValue)
	{
		COPY_STATE_TO_THREAD();
		stacktop[-1].ptrValue =
			(void *)ILStringToAnsi(thread, (ILString *)(stacktop[-1].ptrValue));
		RESTORE_STATE_FROM_THREAD();
	}
	MODIFY_PC_AND_STACK(2, 0);
}
VMBREAK;

/**
 * <opcode name="str2utf8" group="Conversion operators">
 *   <operation>Convert <code>string</code> to <code>utf8 char *</code>
 *              </operation>
 *
 *   <format>prefix<fsep/>str2utf8</format>
 *
 *   <form name="str2utf8" code="COP_PREFIX_STR2UTF8"/>
 *
 *   <before>..., value</before>
 *   <after>..., result</after>
 *
 *   <description>The <i>value</i> is popped from the stack as
 *   type <code>string</code>.  The string is converted into a
 *   <i>result</i>.  character buffer using the UTF-8 encoding.
 *   A pointer to the buffer is pushed onto the stack as type
 *   <code>ptr</code>.</description>
 *
 *   <notes>This instruction is used to convert C# strings into
 *   character buffers during "PInvoke" marshalling operations.</notes>
 * </opcode>
 */
VMCASE(COP_PREFIX_STR2UTF8):
{
	/* Convert a string object into a UTF-8 character buffer */
	if(stacktop[-1].ptrValue)
	{
		COPY_STATE_TO_THREAD();
		stacktop[-1].ptrValue =
			(void *)ILStringToUTF8(thread, (ILString *)(stacktop[-1].ptrValue));
		RESTORE_STATE_FROM_THREAD();
	}
	MODIFY_PC_AND_STACK(2, 0);
}
VMBREAK;

#endif /* IL_CVM_PREFIX */
