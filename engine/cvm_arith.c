/*
 * cvm_arith.c - Opcodes for performing arithmetic.
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
 * Check to see if a floating point number is finite.
 */
#ifdef HAVE_FINITE
	#define	FLOAT_IS_FINITE(value)	(finite((value)))
#else
	#if defined(HAVE_ISNAN) && defined(HAVE_ISINF)
		#define	FLOAT_IS_FINITE(value)	(!isnan((value)) && \
										  isinf((value)) == 0)
	#else
		#error "Don't know how to determine if floating point numbers are finite"
	#endif
#endif

/*
 * Integer add with overflow detection.
 */
static IL_INLINE int IAddOvf(ILInt32 *result, ILInt32 a, ILInt32 b)
{
	if(a >= 0 && b >= 0)
	{
		return ((*result = a + b) >= a);
	}
	else if(a < 0 && b < 0)
	{
		return ((*result = a + b) < a);
	}
	else
	{
		*result = a + b;
		return 1;
	}
}

/*
 * Unsigned integer add with overflow detection.
 */
static IL_INLINE int IUAddOvf(ILUInt32 *result, ILUInt32 a, ILUInt32 b)
{
	return ((*result = a + b) >= a);
}

/*
 * Integer subtract with overflow detection.
 */
static IL_INLINE int ISubOvf(ILInt32 *result, ILInt32 a, ILInt32 b)
{
	if(a >= 0 && b >= 0)
	{
		*result = a - b;
		return 1;
	}
	else if(a < 0 && b < 0)
	{
		*result = a - b;
		return 1;
	}
	else if(a < 0)
	{
		return ((*result = a - b) <= a);
	}
	else
	{
		return ((*result = a - b) >= a);
	}
}

/*
 * Unsigned integer subtract with overflow detection.
 */
static IL_INLINE int IUSubOvf(ILUInt32 *result, ILUInt32 a, ILUInt32 b)
{
	return ((*result = a - b) <= a);
}

/*
 * Integer multiply with overflow detection.
 */
static IL_INLINE int IMulOvf(ILInt32 *result, ILInt32 a, ILInt32 b)
{
	ILInt64 temp = ((ILInt64)a) * ((ILInt64)b);
	if(temp >= (ILInt64)IL_MIN_INT32 && temp <= (ILInt64)IL_MAX_INT32)
	{
		*result = (ILInt32)temp;
		return 1;
	}
	else
	{
		return 0;
	}
}

/*
 * Unsigned integer multiply with overflow detection.
 */
static IL_INLINE int IUMulOvf(ILUInt32 *result, ILUInt32 a, ILUInt32 b)
{
	ILUInt64 temp = ((ILUInt64)a) * ((ILUInt64)b);
	if(temp <= (ILUInt64)IL_MAX_UINT32)
	{
		*result = (ILUInt32)temp;
		return 1;
	}
	else
	{
		return 0;
	}
}

/*
 * Long addition with overflow detection.
 */
static IL_INLINE int LAddOvf(CVMWord *a, CVMWord *b)
{
	ILInt64 tempa, tempb, result;
	tempa = ReadLong(a);
	tempb = ReadLong(b);
	if(tempa >= 0 && tempb >= 0)
	{
		if((result = tempa + tempb) < tempa)
		{
			return 0;
		}
	}
	else if(tempa < 0 && tempb < 0)
	{
		if((result = tempa + tempb) >= tempa)
		{
			return 0;
		}
	}
	else
	{
		result = tempa + tempb;
	}
	WriteLong(a, result);
	return 1;
}

/*
 * Unsigned long addition with overflow detection.
 */
static IL_INLINE int LUAddOvf(CVMWord *a, CVMWord *b)
{
	ILUInt64 tempa, tempb, result;
	tempa = ReadULong(a);
	tempb = ReadULong(b);
	if((result = tempa + tempb) < tempa)
	{
		return 0;
	}
	WriteULong(a, result);
	return 1;
}

/*
 * Long subtract with overflow detection.
 */
static IL_INLINE int LSubOvf(CVMWord *a, CVMWord *b)
{
	ILInt64 tempa, tempb, result;
	tempa = ReadLong(a);
	tempb = ReadLong(b);
	if(tempa >= 0 && tempb >= 0)
	{
		result = tempa - tempb;
	}
	else if(tempa < 0 && tempb < 0)
	{
		result = tempa - tempb;
	}
	else if(tempa < 0)
	{
		if((result = tempa - tempb) > tempa)
		{
			return 0;
		}
	}
	else
	{
		if((result = tempa - tempb) < tempa)
		{
			return 0;
		}
	}
	WriteLong(a, result);
	return 1;
}

/*
 * Unsigned long subtract with overflow detection.
 */
static IL_INLINE int LUSubOvf(CVMWord *a, CVMWord *b)
{
	ILUInt64 tempa, tempb, result;
	tempa = ReadULong(a);
	tempb = ReadULong(b);
	if((result = tempa - tempb) > tempa)
	{
		return 0;
	}
	WriteULong(a, result);
	return 1;
}

/*
 * Long multiply with overflow detection.
 */
static IL_INLINE int LMulOvf(CVMWord *a, CVMWord *b)
{
	ILInt64 result;
	if(!ILInt64MulOvf(&result, ReadLong(a), ReadLong(b)))
	{
		return 0;
	}
	WriteLong(a, result);
	return 1;
}

/*
 * Unsigned long multiply with overflow detection.
 */
static IL_INLINE int LUMulOvf(CVMWord *a, CVMWord *b)
{
	ILUInt64 result;
	if(!ILUInt64MulOvf(&result, ReadULong(a), ReadULong(b)))
	{
		return 0;
	}
	WriteULong(a, result);
	return 1;
}

/*
 * Divide two long values.
 */
static IL_INLINE int LDiv(CVMWord *a, CVMWord *b)
{
	ILInt64 tempa = ReadLong(a);
	ILInt64 tempb = ReadLong(b);
	if(!tempb)
	{
		return 0;
	}
	else if(tempb == ((ILInt64)(-1L)) && tempa == IL_MIN_INT64)
	{
		return -1;
	}
	else
	{
		WriteLong(a, tempa / tempb);
		return 1;
	}
}

/*
 * Divide two unsigned long values.
 */
static IL_INLINE int LUDiv(CVMWord *a, CVMWord *b)
{
	ILUInt64 tempa = ReadULong(a);
	ILUInt64 tempb = ReadULong(b);
	if(!tempb)
	{
		return 0;
	}
	else
	{
		WriteULong(a, tempa / tempb);
		return 1;
	}
}

/*
 * Remainder two long values.
 */
static IL_INLINE int LRem(CVMWord *a, CVMWord *b)
{
	ILInt64 tempa = ReadLong(a);
	ILInt64 tempb = ReadLong(b);
	if(!tempb)
	{
		return 0;
	}
	else if(tempb == ((ILInt64)(-1L)) && tempa == IL_MIN_INT64)
	{
		return -1;
	}
	else
	{
		WriteLong(a, tempa % tempb);
		return 1;
	}
}

/*
 * Remainder two unsigned long values.
 */
static IL_INLINE int LURem(CVMWord *a, CVMWord *b)
{
	ILUInt64 tempa = ReadULong(a);
	ILUInt64 tempb = ReadULong(b);
	if(!tempb)
	{
		return 0;
	}
	else
	{
		WriteULong(a, tempa % tempb);
		return 1;
	}
}

/*
 * Remainder two "native float" values.
 */
static IL_INLINE ILNativeFloat FRem(ILNativeFloat a, ILNativeFloat b)
{
	return ILNativeFloatRem(a, b);
}

#elif defined(IL_CVM_LOCALS)

/* No locals required */

#elif defined(IL_CVM_MAIN)

/**
 * <opcode name="iadd" group="Arithmetic operators">
 *   <operation>Add <code>int32</code></operation>
 *
 *   <format>iadd</format>
 *
 *   <form name="iadd" code="COP_IADD"/>
 *
 *   <before>..., value1, value2</before>
 *   <after>..., result</after>
 *
 *   <description>Both <i>value1</i> and <i>value2</i>
 *   are popped from the stack as type <code>int32</code>.  The
 *   <code>int32</code> <i>result</i> is <i>value1 + value2</i>.
 *   The <i>result</i> is pushed onto the stack.<p/>
 *
 *   The result is the 32 low-order bits of the true mathematical
 *   result in a sufficiently wide two's-complement format, represented
 *   as a value of type <code>int32</code>.  If overflow occurs, then
 *   the sign of the result may not be the same as the sign of the
 *   mathematical sum of the two values.</description>
 *
 *   <notes>The <i>iadd</i> instruction can also be used to add
 *   values of type <code>uint32</code>.</notes>
 * </opcode>
 */
VMCASE(COP_IADD):
{
	/* Integer add */
	stacktop[-2].intValue += stacktop[-1].intValue;
	MODIFY_PC_AND_STACK(1, -1);
}
VMBREAK;

/**
 * <opcode name="iadd_ovf" group="Arithmetic operators">
 *   <operation>Add <code>int32</code> with overflow detection</operation>
 *
 *   <format>iadd_ovf</format>
 *
 *   <form name="iadd_ovf" code="COP_IADD_OVF"/>
 *
 *   <before>..., value1, value2</before>
 *   <after>..., result</after>
 *
 *   <description>Both <i>value1</i> and <i>value2</i>
 *   are popped from the stack as type <code>int32</code>.  The
 *   <code>int32</code> <i>result</i> is <i>value1 + value2</i>.
 *   The <i>result</i> is pushed onto the stack.<p/>
 *
 *   The result is the 32 low-order bits of the true mathematical
 *   result in a sufficiently wide two's-complement format, represented
 *   as a value of type <code>int32</code>.  If overflow occurs, then
 *   <code>System.OverflowException</code> is thrown.</description>
 *
 *   <exceptions>
 *     <exception name="System.OverflowException">Raised if
 *     the true mathemetical result is too large to be represented
 *     as a value of type <code>int32</code>.</exception>
 *   </exceptions>
 * </opcode>
 */
VMCASE(COP_IADD_OVF):
{
	/* Integer add with overflow detection */
	if(IAddOvf(&(stacktop[-2].intValue),
			   stacktop[-2].intValue, stacktop[-1].intValue))
	{
		MODIFY_PC_AND_STACK(1, -1);
	}
	else
	{
		OVERFLOW_EXCEPTION();
	}
}
VMBREAK;

/**
 * <opcode name="iadd_ovf_un" group="Arithmetic operators">
 *   <operation>Add <code>uint32</code> with overflow detection</operation>
 *
 *   <format>iadd_ovf_un</format>
 *
 *   <form name="iadd_ovf_un" code="COP_IADD_OVF_UN"/>
 *
 *   <before>..., value1, value2</before>
 *   <after>..., result</after>
 *
 *   <description>Both <i>value1</i> and <i>value2</i>
 *   are popped from the stack as type <code>uint32</code>.  The
 *   <code>uint32</code> <i>result</i> is <i>value1 + value2</i>.
 *   The <i>result</i> is pushed onto the stack.<p/>
 *
 *   The result is the 32 low-order bits of the true mathematical
 *   result in a sufficiently wide unsigned format, represented
 *   as a value of type <code>uint32</code>.  If overflow occurs, then
 *   <code>System.OverflowException</code> is thrown.</description>
 *
 *   <exceptions>
 *     <exception name="System.OverflowException">Raised if
 *     the true mathemetical result is too large to be represented
 *     as a value of type <code>uint32</code>.</exception>
 *   </exceptions>
 * </opcode>
 */
VMCASE(COP_IADD_OVF_UN):
{
	/* Unsigned integer add with overflow detection */
	if(IUAddOvf(&(stacktop[-2].uintValue),
			    stacktop[-2].uintValue, stacktop[-1].uintValue))
	{
		MODIFY_PC_AND_STACK(1, -1);
	}
	else
	{
		OVERFLOW_EXCEPTION();
	}
}
VMBREAK;

/**
 * <opcode name="isub" group="Arithmetic operators">
 *   <operation>Subtract <code>int32</code></operation>
 *
 *   <format>isub</format>
 *
 *   <form name="isub" code="COP_ISUB"/>
 *
 *   <before>..., value1, value2</before>
 *   <after>..., result</after>
 *
 *   <description>Both <i>value1</i> and <i>value2</i>
 *   are popped from the stack as type <code>int32</code>.  The
 *   <code>int32</code> <i>result</i> is <i>value1 - value2</i>.
 *   The <i>result</i> is pushed onto the stack.<p/>
 *
 *   The result is the 32 low-order bits of the true mathematical
 *   result in a sufficiently wide two's-complement format, represented
 *   as a value of type <code>int32</code>.  If overflow occurs, then
 *   the sign of the result may not be the same as the sign of the
 *   mathematical difference of the two values.</description>
 *
 *   <notes>The <i>isub</i> instruction can also be used to subtract
 *   values of type <code>uint32</code>.</notes>
 * </opcode>
 */
VMCASE(COP_ISUB):
{
	/* Integer subtract */
	stacktop[-2].intValue -= stacktop[-1].intValue;
	MODIFY_PC_AND_STACK(1, -1);
}
VMBREAK;

/**
 * <opcode name="isub_ovf" group="Arithmetic operators">
 *   <operation>Subtract <code>int32</code> with overflow detection</operation>
 *
 *   <format>isub_ovf</format>
 *
 *   <form name="isub_ovf" code="COP_ISUB_OVF"/>
 *
 *   <before>..., value1, value2</before>
 *   <after>..., result</after>
 *
 *   <description>Both <i>value1</i> and <i>value2</i>
 *   are popped from the stack as type <code>int32</code>.  The
 *   <code>int32</code> <i>result</i> is <i>value1 - value2</i>.
 *   The <i>result</i> is pushed onto the stack.<p/>
 *
 *   The result is the 32 low-order bits of the true mathematical
 *   result in a sufficiently wide two's-complement format, represented
 *   as a value of type <code>int32</code>.  If overflow occurs, then
 *   <code>System.OverflowException</code> is thrown.</description>
 *
 *   <exceptions>
 *     <exception name="System.OverflowException">Raised if
 *     the true mathemetical result is too large to be represented
 *     as a value of type <code>int32</code>.</exception>
 *   </exceptions>
 * </opcode>
 */
VMCASE(COP_ISUB_OVF):
{
	/* Integer subtract with overflow detection */
	if(ISubOvf(&(stacktop[-2].intValue),
			   stacktop[-2].intValue, stacktop[-1].intValue))
	{
		MODIFY_PC_AND_STACK(1, -1);
	}
	else
	{
		OVERFLOW_EXCEPTION();
	}
}
VMBREAK;

/**
 * <opcode name="isub_ovf_un" group="Arithmetic operators">
 *   <operation>Subtract <code>uint32</code> with overflow detection</operation>
 *
 *   <format>isub_ovf_un</format>
 *
 *   <form name="isub_ovf_un" code="COP_ISUB_OVF_UN"/>
 *
 *   <before>..., value1, value2</before>
 *   <after>..., result</after>
 *
 *   <description>Both <i>value1</i> and <i>value2</i>
 *   are popped from the stack as type <code>uint32</code>.  The
 *   <code>uint32</code> <i>result</i> is <i>value1 - value2</i>.
 *   The <i>result</i> is pushed onto the stack.<p/>
 *
 *   The result is the 32 low-order bits of the true mathematical
 *   result in a sufficiently wide unsigned format, represented
 *   as a value of type <code>uint32</code>.  If overflow occurs, then
 *   <code>System.OverflowException</code> is thrown.</description>
 *
 *   <exceptions>
 *     <exception name="System.OverflowException">Raised if
 *     the true mathemetical result is too large to be represented
 *     as a value of type <code>uint32</code>.</exception>
 *   </exceptions>
 * </opcode>
 */
VMCASE(COP_ISUB_OVF_UN):
{
	/* Unsigned integer subtract with overflow detection */
	if(IUSubOvf(&(stacktop[-2].uintValue),
			    stacktop[-2].uintValue, stacktop[-1].uintValue))
	{
		MODIFY_PC_AND_STACK(1, -1);
	}
	else
	{
		OVERFLOW_EXCEPTION();
	}
}
VMBREAK;

/**
 * <opcode name="imul" group="Arithmetic operators">
 *   <operation>Multiply <code>int32</code></operation>
 *
 *   <format>imul</format>
 *
 *   <form name="imul" code="COP_IMUL"/>
 *
 *   <before>..., value1, value2</before>
 *   <after>..., result</after>
 *
 *   <description>Both <i>value1</i> and <i>value2</i>
 *   are popped from the stack as type <code>int32</code>.  The
 *   <code>int32</code> <i>result</i> is <i>value1 * value2</i>.
 *   The <i>result</i> is pushed onto the stack.<p/>
 *
 *   The result is the 32 low-order bits of the true mathematical
 *   result in a sufficiently wide two's-complement format, represented
 *   as a value of type <code>int32</code>.  If overflow occurs, then
 *   the sign of the result may not be the same as the sign of the
 *   mathematical multiplication of the two values.</description>
 *
 *   <notes>The <i>imul</i> instruction can also be used to multiply
 *   values of type <code>uint32</code>.</notes>
 * </opcode>
 */
VMCASE(COP_IMUL):
{
	/* Integer (and unsigned integer) multiply */
	stacktop[-2].intValue *= stacktop[-1].intValue;
	MODIFY_PC_AND_STACK(1, -1);
}
VMBREAK;

/**
 * <opcode name="imul_ovf" group="Arithmetic operators">
 *   <operation>Multiply <code>int32</code> with overflow detection</operation>
 *
 *   <format>imul_ovf</format>
 *
 *   <form name="imul_ovf" code="COP_IMUL_OVF"/>
 *
 *   <before>..., value1, value2</before>
 *   <after>..., result</after>
 *
 *   <description>Both <i>value1</i> and <i>value2</i>
 *   are popped from the stack as type <code>int32</code>.  The
 *   <code>int32</code> <i>result</i> is <i>value1 * value2</i>.
 *   The <i>result</i> is pushed onto the stack.<p/>
 *
 *   The result is the 32 low-order bits of the true mathematical
 *   result in a sufficiently wide two's-complement format, represented
 *   as a value of type <code>int32</code>.  If overflow occurs, then
 *   <code>System.OverflowException</code> is thrown.</description>
 *
 *   <exceptions>
 *     <exception name="System.OverflowException">Raised if
 *     the true mathemetical result is too large to be represented
 *     as a value of type <code>int32</code>.</exception>
 *   </exceptions>
 * </opcode>
 */
VMCASE(COP_IMUL_OVF):
{
	/* Integer multiply with overflow detection */
	if(IMulOvf(&(stacktop[-2].intValue),
			   stacktop[-2].intValue, stacktop[-1].intValue))
	{
		MODIFY_PC_AND_STACK(1, -1);
	}
	else
	{
		OVERFLOW_EXCEPTION();
	}
}
VMBREAK;

/**
 * <opcode name="imul_ovf_un" group="Arithmetic operators">
 *   <operation>Multiply <code>uint32</code> with overflow detection</operation>
 *
 *   <format>imul_ovf_un</format>
 *
 *   <form name="imul_ovf_un" code="COP_IMUL_OVF_UN"/>
 *
 *   <before>..., value1, value2</before>
 *   <after>..., result</after>
 *
 *   <description>Both <i>value1</i> and <i>value2</i>
 *   are popped from the stack as type <code>uint32</code>.  The
 *   <code>uint32</code> <i>result</i> is <i>value1 * value2</i>.
 *   The <i>result</i> is pushed onto the stack.<p/>
 *
 *   The result is the 32 low-order bits of the true mathematical
 *   result in a sufficiently wide unsigned format, represented
 *   as a value of type <code>uint32</code>.  If overflow occurs, then
 *   <code>System.OverflowException</code> is thrown.</description>
 *
 *   <exceptions>
 *     <exception name="System.OverflowException">Raised if
 *     the true mathemetical result is too large to be represented
 *     as a value of type <code>uint32</code>.</exception>
 *   </exceptions>
 * </opcode>
 */
VMCASE(COP_IMUL_OVF_UN):
{
	/* Unsigned integer multiply with overflow detection */
	if(IUMulOvf(&(stacktop[-2].uintValue),
			    stacktop[-2].uintValue, stacktop[-1].uintValue))
	{
		MODIFY_PC_AND_STACK(1, -1);
	}
	else
	{
		OVERFLOW_EXCEPTION();
	}
}
VMBREAK;

/**
 * <opcode name="idiv" group="Arithmetic operators">
 *   <operation>Divide <code>int32</code></operation>
 *
 *   <format>idiv</format>
 *
 *   <form name="idiv" code="COP_IDIV"/>
 *
 *   <before>..., value1, value2</before>
 *   <after>..., result</after>
 *
 *   <description>Both <i>value1</i> and <i>value2</i>
 *   are popped from the stack as type <code>int32</code>.  The
 *   <code>int32</code> <i>result</i> is <i>value1 / value2</i>.
 *   The <i>result</i> is pushed onto the stack.</description>
 *
 *   <exceptions>
 *     <exception name="System.DivideByZeroException">Raised if
 *     <i>value2</i> is zero.</exception>
 *     <exception name="System.ArithmeticException">Raised if
 *     <i>value1</i> is -1 and <i>value2</i> is -2147483648.</exception>
 *   </exceptions>
 * </opcode>
 */
VMCASE(COP_IDIV):
{
	/* Integer divide */
	if(stacktop[-1].intValue != 0)
	{
		if(stacktop[-1].intValue != -1 ||
		   stacktop[-2].intValue != IL_MIN_INT32)
		{
			stacktop[-2].intValue /= stacktop[-1].intValue;
			MODIFY_PC_AND_STACK(1, -1);
		}
		else
		{
			ARITHMETIC_EXCEPTION();
		}
	}
	else
	{
		ZERO_DIV_EXCEPTION();
	}
}
VMBREAK;

/**
 * <opcode name="idiv_un" group="Arithmetic operators">
 *   <operation>Divide <code>uint32</code></operation>
 *
 *   <format>idiv_un</format>
 *
 *   <form name="idiv_un" code="COP_IDIV_UN"/>
 *
 *   <before>..., value1, value2</before>
 *   <after>..., result</after>
 *
 *   <description>Both <i>value1</i> and <i>value2</i>
 *   are popped from the stack as type <code>uint32</code>.  The
 *   <code>uint32</code> <i>result</i> is <i>value1 / value2</i>.
 *   The <i>result</i> is pushed onto the stack.</description>
 *
 *   <exceptions>
 *     <exception name="System.DivideByZeroException">Raised if
 *     <i>value2</i> is zero.</exception>
 *   </exceptions>
 * </opcode>
 */
VMCASE(COP_IDIV_UN):
{
	/* Unsigned integer divide */
	if(stacktop[-1].uintValue != 0)
	{
		stacktop[-2].uintValue /= stacktop[-1].uintValue;
		MODIFY_PC_AND_STACK(1, -1);
	}
	else
	{
		ZERO_DIV_EXCEPTION();
	}
}
VMBREAK;

/**
 * <opcode name="irem" group="Arithmetic operators">
 *   <operation>Remainder <code>int32</code></operation>
 *
 *   <format>irem</format>
 *
 *   <form name="irem" code="COP_IREM"/>
 *
 *   <before>..., value1, value2</before>
 *   <after>..., result</after>
 *
 *   <description>Both <i>value1</i> and <i>value2</i>
 *   are popped from the stack as type <code>int32</code>.  The
 *   <code>int32</code> <i>result</i> is <i>value1 % value2</i>.
 *   The <i>result</i> is pushed onto the stack.</description>
 *
 *   <exceptions>
 *     <exception name="System.DivideByZeroException">Raised if
 *     <i>value2</i> is zero.</exception>
 *     <exception name="System.ArithmeticException">Raised if
 *     <i>value1</i> is -1 and <i>value2</i> is -2147483648.</exception>
 *   </exceptions>
 * </opcode>
 */
VMCASE(COP_IREM):
{
	/* Integer remainder */
	if(stacktop[-1].intValue != 0)
	{
		if(stacktop[-1].intValue != -1 ||
		   stacktop[-2].intValue != IL_MIN_INT32)
		{
			stacktop[-2].intValue %= stacktop[-1].intValue;
			MODIFY_PC_AND_STACK(1, -1);
		}
		else
		{
			ARITHMETIC_EXCEPTION();
		}
	}
	else
	{
		ZERO_DIV_EXCEPTION();
	}
}
VMBREAK;

/**
 * <opcode name="irem_un" group="Arithmetic operators">
 *   <operation>Remainder <code>uint32</code></operation>
 *
 *   <format>irem_un</format>
 *
 *   <form name="irem_un" code="COP_IREM_UN"/>
 *
 *   <before>..., value1, value2</before>
 *   <after>..., result</after>
 *
 *   <description>Both <i>value1</i> and <i>value2</i>
 *   are popped from the stack as type <code>uint32</code>.  The
 *   <code>uint32</code> <i>result</i> is <i>value1 % value2</i>.
 *   The <i>result</i> is pushed onto the stack.</description>
 *
 *   <exceptions>
 *     <exception name="System.DivideByZeroException">Raised if
 *     <i>value2</i> is zero.</exception>
 *   </exceptions>
 * </opcode>
 */
VMCASE(COP_IREM_UN):
{
	/* Unsigned integer remainder */
	if(stacktop[-1].uintValue != 0)
	{
		stacktop[-2].uintValue %= stacktop[-1].uintValue;
		MODIFY_PC_AND_STACK(1, -1);
	}
	else
	{
		ZERO_DIV_EXCEPTION();
	}
}
VMBREAK;

/**
 * <opcode name="ineg" group="Arithmetic operators">
 *   <operation>Negate <code>int32</code></operation>
 *
 *   <format>ineg</format>
 *
 *   <form name="ineg" code="COP_INEG"/>
 *
 *   <before>..., value</before>
 *   <after>..., result</after>
 *
 *   <description>The <i>value</i> is popped from the stack
 *   as type <code>int32</code>.  The <code>int32</code> <i>result</i>
 *   is <i>-value</i>.  The <i>result</i> is pushed onto
 *   the stack.</description>
 *
 *   <notes>To perform negation with overflow detection, use
 *   <i>isub_ovf</i> with the first argument set to 0 and the
 *   second argument set to <i>value</i>.</notes>
 * </opcode>
 */
VMCASE(COP_INEG):
{
	/* Integer negate */
	stacktop[-1].intValue = -(stacktop[-1].intValue);
	MODIFY_PC_AND_STACK(1, 0);
}
VMBREAK;

/**
 * <opcode name="ladd" group="Arithmetic operators">
 *   <operation>Add <code>int64</code></operation>
 *
 *   <format>ladd</format>
 *
 *   <form name="ladd" code="COP_LADD"/>
 *
 *   <before>..., value1, value2</before>
 *   <after>..., result</after>
 *
 *   <description>Both <i>value1</i> and <i>value2</i>
 *   are popped from the stack as type <code>int64</code>.  The
 *   <code>int64</code> <i>result</i> is <i>value1 + value2</i>.
 *   The <i>result</i> is pushed onto the stack.<p/>
 *
 *   The result is the 64 low-order bits of the true mathematical
 *   result in a sufficiently wide two's-complement format, represented
 *   as a value of type <code>int64</code>.  If overflow occurs, then
 *   the sign of the result may not be the same as the sign of the
 *   mathematical sum of the two values.</description>
 *
 *   <notes>The <i>ladd</i> instruction can also be used to add
 *   values of type <code>uint64</code>.<p/>
 *
 *   Values of type <code>int64</code> typically occupy 2 stack slots
 *   on 32-bit machines and 1 stack slot on 64-bit machines, although
 *   this is layout not fixed.  When we say that <i>value1</i> and
 *   <i>value2</i> are popped, we assume that the correct number of
 *   stack slots for the machine are popped.  Similarly when <i>result</i>
 *   is pushed.</notes>
 * </opcode>
 */
VMCASE(COP_LADD):
{
	/* Long add */
	WriteLong(&(stacktop[-(CVM_WORDS_PER_LONG * 2)]),
		ReadLong(&(stacktop[-(CVM_WORDS_PER_LONG * 2)])) +
		ReadLong(&(stacktop[-CVM_WORDS_PER_LONG])));
	MODIFY_PC_AND_STACK(1, -CVM_WORDS_PER_LONG);
}
VMBREAK;

/**
 * <opcode name="ladd_ovf" group="Arithmetic operators">
 *   <operation>Add <code>int64</code> with overflow detection</operation>
 *
 *   <format>ladd_ovf</format>
 *
 *   <form name="ladd_ovf" code="COP_LADD_OVF"/>
 *
 *   <before>..., value1, value2</before>
 *   <after>..., result</after>
 *
 *   <description>Both <i>value1</i> and <i>value2</i>
 *   are popped from the stack as type <code>int64</code>.  The
 *   <code>int64</code> <i>result</i> is <i>value1 + value2</i>.
 *   The <i>result</i> is pushed onto the stack.<p/>
 *
 *   The result is the 64 low-order bits of the true mathematical
 *   result in a sufficiently wide two's-complement format, represented
 *   as a value of type <code>int64</code>.  If overflow occurs, then
 *   <code>System.OverflowException</code> is thrown.</description>
 *
 *   <exceptions>
 *     <exception name="System.OverflowException">Raised if
 *     the true mathemetical result is too large to be represented
 *     as a value of type <code>int64</code>.</exception>
 *   </exceptions>
 * </opcode>
 */
VMCASE(COP_LADD_OVF):
{
	/* Long add with overflow detection */
	if(LAddOvf(&(stacktop[-(CVM_WORDS_PER_LONG * 2)]),
		       &(stacktop[-CVM_WORDS_PER_LONG])))
	{
		MODIFY_PC_AND_STACK(1, -CVM_WORDS_PER_LONG);
	}
	else
	{
		OVERFLOW_EXCEPTION();
	}
}
VMBREAK;

/**
 * <opcode name="ladd_ovf_un" group="Arithmetic operators">
 *   <operation>Add <code>uint64</code> with overflow detection</operation>
 *
 *   <format>ladd_ovf_un</format>
 *
 *   <form name="ladd_ovf_un" code="COP_LADD_OVF_UN"/>
 *
 *   <before>..., value1, value2</before>
 *   <after>..., result</after>
 *
 *   <description>Both <i>value1</i> and <i>value2</i>
 *   are popped from the stack as type <code>uint64</code>.  The
 *   <code>uint64</code> <i>result</i> is <i>value1 + value2</i>.
 *   The <i>result</i> is pushed onto the stack.<p/>
 *
 *   The result is the 64 low-order bits of the true mathematical
 *   result in a sufficiently wide unsignedcomplement format, represented
 *   as a value of type <code>uint64</code>.  If overflow occurs, then
 *   <code>System.OverflowException</code> is thrown.</description>
 *
 *   <exceptions>
 *     <exception name="System.OverflowException">Raised if
 *     the true mathemetical result is too large to be represented
 *     as a value of type <code>uint64</code>.</exception>
 *   </exceptions>
 * </opcode>
 */
VMCASE(COP_LADD_OVF_UN):
{
	/* Unsigned long add with overflow detection */
	if(LUAddOvf(&(stacktop[-(CVM_WORDS_PER_LONG * 2)]),
		        &(stacktop[-CVM_WORDS_PER_LONG])))
	{
		MODIFY_PC_AND_STACK(1, -CVM_WORDS_PER_LONG);
	}
	else
	{
		OVERFLOW_EXCEPTION();
	}
}
VMBREAK;

/**
 * <opcode name="lsub" group="Arithmetic operators">
 *   <operation>Subtract <code>int64</code></operation>
 *
 *   <format>lsub</format>
 *
 *   <form name="lsub" code="COP_LSUB"/>
 *
 *   <before>..., value1, value2</before>
 *   <after>..., result</after>
 *
 *   <description>Both <i>value1</i> and <i>value2</i>
 *   are popped from the stack as type <code>int64</code>.  The
 *   <code>int64</code> <i>result</i> is <i>value1 - value2</i>.
 *   The <i>result</i> is pushed onto the stack.<p/>
 *
 *   The result is the 64 low-order bits of the true mathematical
 *   result in a sufficiently wide two's-complement format, represented
 *   as a value of type <code>int64</code>.  If overflow occurs, then
 *   the sign of the result may not be the same as the sign of the
 *   mathematical difference of the two values.</description>
 *
 *   <notes>The <i>lsub</i> instruction can also be used to subtract
 *   values of type <code>uint64</code>.</notes>
 * </opcode>
 */
VMCASE(COP_LSUB):
{
	/* Long subtract */
	WriteLong(&(stacktop[-(CVM_WORDS_PER_LONG * 2)]),
		ReadLong(&(stacktop[-(CVM_WORDS_PER_LONG * 2)])) -
		ReadLong(&(stacktop[-CVM_WORDS_PER_LONG])));
	MODIFY_PC_AND_STACK(1, -CVM_WORDS_PER_LONG);
}
VMBREAK;

/**
 * <opcode name="lsub_ovf" group="Arithmetic operators">
 *   <operation>Subtract <code>int64</code> with overflow detection</operation>
 *
 *   <format>lsub_ovf</format>
 *
 *   <form name="lsub_ovf" code="COP_LSUB_OVF"/>
 *
 *   <before>..., value1, value2</before>
 *   <after>..., result</after>
 *
 *   <description>Both <i>value1</i> and <i>value2</i>
 *   are popped from the stack as type <code>int64</code>.  The
 *   <code>int64</code> <i>result</i> is <i>value1 - value2</i>.
 *   The <i>result</i> is pushed onto the stack.<p/>
 *
 *   The result is the 64 low-order bits of the true mathematical
 *   result in a sufficiently wide two's-complement format, represented
 *   as a value of type <code>int64</code>.  If overflow occurs, then
 *   <code>System.OverflowException</code> is thrown.</description>
 *
 *   <exceptions>
 *     <exception name="System.OverflowException">Raised if
 *     the true mathemetical result is too large to be represented
 *     as a value of type <code>int64</code>.</exception>
 *   </exceptions>
 * </opcode>
 */
VMCASE(COP_LSUB_OVF):
{
	/* Long subtract with overflow detection */
	if(!LSubOvf(&(stacktop[-(CVM_WORDS_PER_LONG * 2)]),
		        &(stacktop[-CVM_WORDS_PER_LONG])))
	{
		MODIFY_PC_AND_STACK(1, -CVM_WORDS_PER_LONG);
	}
	else
	{
		OVERFLOW_EXCEPTION();
	}
}
VMBREAK;

/**
 * <opcode name="lsub_ovf_un" group="Arithmetic operators">
 *   <operation>Subtract <code>uint64</code> with overflow detection</operation>
 *
 *   <format>lsub_ovf_un</format>
 *
 *   <form name="lsub_ovf_un" code="COP_LSUB_OVF_UN"/>
 *
 *   <before>..., value1, value2</before>
 *   <after>..., result</after>
 *
 *   <description>Both <i>value1</i> and <i>value2</i>
 *   are popped from the stack as type <code>uint64</code>.  The
 *   <code>uint64</code> <i>result</i> is <i>value1 - value2</i>.
 *   The <i>result</i> is pushed onto the stack.<p/>
 *
 *   The result is the 64 low-order bits of the true mathematical
 *   result in a sufficiently wide unsignedcomplement format, represented
 *   as a value of type <code>uint64</code>.  If overflow occurs, then
 *   <code>System.OverflowException</code> is thrown.</description>
 *
 *   <exceptions>
 *     <exception name="System.OverflowException">Raised if
 *     the true mathemetical result is too large to be represented
 *     as a value of type <code>uint64</code>.</exception>
 *   </exceptions>
 * </opcode>
 */
VMCASE(COP_LSUB_OVF_UN):
{
	/* Unsigned long subtract with overflow detection */
	if(!LUSubOvf(&(stacktop[-(CVM_WORDS_PER_LONG * 2)]),
		         &(stacktop[-CVM_WORDS_PER_LONG])))
	{
		MODIFY_PC_AND_STACK(1, -CVM_WORDS_PER_LONG);
	}
	else
	{
		OVERFLOW_EXCEPTION();
	}
}
VMBREAK;

/**
 * <opcode name="lmul" group="Arithmetic operators">
 *   <operation>Multiply <code>int64</code></operation>
 *
 *   <format>lmul</format>
 *
 *   <form name="lmul" code="COP_LMUL"/>
 *
 *   <before>..., value1, value2</before>
 *   <after>..., result</after>
 *
 *   <description>Both <i>value1</i> and <i>value2</i>
 *   are popped from the stack as type <code>int64</code>.  The
 *   <code>int64</code> <i>result</i> is <i>value1 * value2</i>.
 *   The <i>result</i> is pushed onto the stack.<p/>
 *
 *   The result is the 64 low-order bits of the true mathematical
 *   result in a sufficiently wide two's-complement format, represented
 *   as a value of type <code>int64</code>.  If overflow occurs, then
 *   the sign of the result may not be the same as the sign of the
 *   mathematical multiplication of the two values.</description>
 *
 *   <notes>The <i>lmul</i> instruction can also be used to multiply
 *   values of type <code>uint64</code>.</notes>
 * </opcode>
 */
VMCASE(COP_LMUL):
{
	/* Long (and unsigned long) multiply */
	WriteLong(&(stacktop[-(CVM_WORDS_PER_LONG * 2)]),
		ReadLong(&(stacktop[-(CVM_WORDS_PER_LONG * 2)])) *
		ReadLong(&(stacktop[-CVM_WORDS_PER_LONG])));
	MODIFY_PC_AND_STACK(1, -CVM_WORDS_PER_LONG);
}
VMBREAK;

/**
 * <opcode name="lmul_ovf" group="Arithmetic operators">
 *   <operation>Multiply <code>int64</code> with overflow detection</operation>
 *
 *   <format>lmul_ovf</format>
 *
 *   <form name="lmul_ovf" code="COP_LMUL_OVF"/>
 *
 *   <before>..., value1, value2</before>
 *   <after>..., result</after>
 *
 *   <description>Both <i>value1</i> and <i>value2</i>
 *   are popped from the stack as type <code>int64</code>.  The
 *   <code>int64</code> <i>result</i> is <i>value1 * value2</i>.
 *   The <i>result</i> is pushed onto the stack.<p/>
 *
 *   The result is the 64 low-order bits of the true mathematical
 *   result in a sufficiently wide two's-complement format, represented
 *   as a value of type <code>int64</code>.  If overflow occurs, then
 *   <code>System.OverflowException</code> is thrown.</description>
 *
 *   <exceptions>
 *     <exception name="System.OverflowException">Raised if
 *     the true mathemetical result is too large to be represented
 *     as a value of type <code>int64</code>.</exception>
 *   </exceptions>
 * </opcode>
 */
VMCASE(COP_LMUL_OVF):
{
	/* Long multiply with overflow detection */
	if(!LMulOvf(&(stacktop[-(CVM_WORDS_PER_LONG * 2)]),
		        &(stacktop[-CVM_WORDS_PER_LONG])))
	{
		MODIFY_PC_AND_STACK(1, -CVM_WORDS_PER_LONG);
	}
	else
	{
		OVERFLOW_EXCEPTION();
	}
}
VMBREAK;

/**
 * <opcode name="lmul_ovf_un" group="Arithmetic operators">
 *   <operation>Multiply <code>uint64</code> with overflow detection</operation>
 *
 *   <format>lmul_ovf_un</format>
 *
 *   <form name="lmul_ovf_un" code="COP_LMUL_OVF_UN"/>
 *
 *   <before>..., value1, value2</before>
 *   <after>..., result</after>
 *
 *   <description>Both <i>value1</i> and <i>value2</i>
 *   are popped from the stack as type <code>uint64</code>.  The
 *   <code>uint64</code> <i>result</i> is <i>value1 * value2</i>.
 *   The <i>result</i> is pushed onto the stack.<p/>
 *
 *   The result is the 64 low-order bits of the true mathematical
 *   result in a sufficiently wide unsignedcomplement format, represented
 *   as a value of type <code>uint64</code>.  If overflow occurs, then
 *   <code>System.OverflowException</code> is thrown.</description>
 *
 *   <exceptions>
 *     <exception name="System.OverflowException">Raised if
 *     the true mathemetical result is too large to be represented
 *     as a value of type <code>uint64</code>.</exception>
 *   </exceptions>
 * </opcode>
 */
VMCASE(COP_LMUL_OVF_UN):
{
	/* Unsigned long multiply with overflow detection */
	if(!LUMulOvf(&(stacktop[-(CVM_WORDS_PER_LONG * 2)]),
		         &(stacktop[-CVM_WORDS_PER_LONG])))
	{
		MODIFY_PC_AND_STACK(1, -CVM_WORDS_PER_LONG);
	}
	else
	{
		OVERFLOW_EXCEPTION();
	}
}
VMBREAK;

/**
 * <opcode name="ldiv" group="Arithmetic operators">
 *   <operation>Divide <code>int64</code></operation>
 *
 *   <format>ldiv</format>
 *
 *   <form name="ldiv" code="COP_LDIV"/>
 *
 *   <before>..., value1, value2</before>
 *   <after>..., result</after>
 *
 *   <description>Both <i>value1</i> and <i>value2</i>
 *   are popped from the stack as type <code>int64</code>.  The
 *   <code>int64</code> <i>result</i> is <i>value1 / value2</i>.
 *   The <i>result</i> is pushed onto the stack.</description>
 *
 *   <exceptions>
 *     <exception name="System.DivideByZeroException">Raised if
 *     <i>value2</i> is zero.</exception>
 *     <exception name="System.ArithmeticException">Raised if
 *     <i>value1</i> is -1 and <i>value2</i> is -9223372036854775808.
 *     </exception>
 *   </exceptions>
 * </opcode>
 */
VMCASE(COP_LDIV):
{
	/* Long divide */
	divResult = LDiv(&(stacktop[-(CVM_WORDS_PER_LONG * 2)]),
		 			 &(stacktop[-CVM_WORDS_PER_LONG]));
	if(divResult > 0)
	{
		MODIFY_PC_AND_STACK(1, -CVM_WORDS_PER_LONG);
	}
	else if(!divResult)
	{
		ZERO_DIV_EXCEPTION();
	}
	else
	{
		ARITHMETIC_EXCEPTION();
	}
}
VMBREAK;

/**
 * <opcode name="ldiv_un" group="Arithmetic operators">
 *   <operation>Divide <code>uint64</code></operation>
 *
 *   <format>ldiv_un</format>
 *
 *   <form name="ldiv_un" code="COP_LDIV_UN"/>
 *
 *   <before>..., value1, value2</before>
 *   <after>..., result</after>
 *
 *   <description>Both <i>value1</i> and <i>value2</i>
 *   are popped from the stack as type <code>uint64</code>.  The
 *   <code>uint64</code> <i>result</i> is <i>value1 / value2</i>.
 *   The <i>result</i> is pushed onto the stack.</description>
 *
 *   <exceptions>
 *     <exception name="System.DivideByZeroException">Raised if
 *     <i>value2</i> is zero.</exception>
 *   </exceptions>
 * </opcode>
 */
VMCASE(COP_LDIV_UN):
{
	/* Unsigned long divide */
	if(LUDiv(&(stacktop[-(CVM_WORDS_PER_LONG * 2)]),
 			 &(stacktop[-CVM_WORDS_PER_LONG])))
	{
		MODIFY_PC_AND_STACK(1, -CVM_WORDS_PER_LONG);
	}
	else
	{
		ZERO_DIV_EXCEPTION();
	}
}
VMBREAK;

/**
 * <opcode name="lrem" group="Arithmetic operators">
 *   <operation>Remainder <code>int64</code></operation>
 *
 *   <format>lrem</format>
 *
 *   <form name="lrem" code="COP_LREM"/>
 *
 *   <before>..., value1, value2</before>
 *   <after>..., result</after>
 *
 *   <description>Both <i>value1</i> and <i>value2</i>
 *   are popped from the stack as type <code>int64</code>.  The
 *   <code>int64</code> <i>result</i> is <i>value1 % value2</i>.
 *   The <i>result</i> is pushed onto the stack.</description>
 *
 *   <exceptions>
 *     <exception name="System.DivideByZeroException">Raised if
 *     <i>value2</i> is zero.</exception>
 *     <exception name="System.ArithmeticException">Raised if
 *     <i>value1</i> is -1 and <i>value2</i> is -9223372036854775808.
 *     </exception>
 *   </exceptions>
 * </opcode>
 */
VMCASE(COP_LREM):
{
	/* Long remainder */
	divResult = LRem(&(stacktop[-(CVM_WORDS_PER_LONG * 2)]),
		 			 &(stacktop[-CVM_WORDS_PER_LONG]));
	if(divResult > 0)
	{
		MODIFY_PC_AND_STACK(1, -CVM_WORDS_PER_LONG);
	}
	else if(!divResult)
	{
		ZERO_DIV_EXCEPTION();
	}
	else
	{
		ARITHMETIC_EXCEPTION();
	}
}
VMBREAK;

/**
 * <opcode name="lrem_un" group="Arithmetic operators">
 *   <operation>Remainder <code>uint64</code></operation>
 *
 *   <format>lrem_un</format>
 *
 *   <form name="lrem_un" code="COP_LREM_UN"/>
 *
 *   <before>..., value1, value2</before>
 *   <after>..., result</after>
 *
 *   <description>Both <i>value1</i> and <i>value2</i>
 *   are popped from the stack as type <code>uint64</code>.  The
 *   <code>uint64</code> <i>result</i> is <i>value1 % value2</i>.
 *   The <i>result</i> is pushed onto the stack.</description>
 *
 *   <exceptions>
 *     <exception name="System.DivideByZeroException">Raised if
 *     <i>value2</i> is zero.</exception>
 *   </exceptions>
 * </opcode>
 */
VMCASE(COP_LREM_UN):
{
	/* Unsigned long remainder */
	if(LURem(&(stacktop[-(CVM_WORDS_PER_LONG * 2)]),
 			 &(stacktop[-CVM_WORDS_PER_LONG])))
	{
		MODIFY_PC_AND_STACK(1, -CVM_WORDS_PER_LONG);
	}
	else
	{
		ZERO_DIV_EXCEPTION();
	}
}
VMBREAK;

/**
 * <opcode name="lneg" group="Arithmetic operators">
 *   <operation>Negate <code>int64</code></operation>
 *
 *   <format>lneg</format>
 *
 *   <form name="lneg" code="COP_LNEG"/>
 *
 *   <before>..., value</before>
 *   <after>..., result</after>
 *
 *   <description>The <i>value</i> is popped from the stack
 *   as type <code>int64</code>.  The <code>int64</code> <i>result</i>
 *   is <i>-value</i>.  The <i>result</i> is pushed onto
 *   the stack.</description>
 *
 *   <notes>To perform negation with overflow detection, use
 *   <i>lsub_ovf</i> with the first argument set to 0 and the
 *   second argument set to <i>value</i>.</notes>
 * </opcode>
 */
VMCASE(COP_LNEG):
{
	/* Long negate */
	WriteLong(&(stacktop[-CVM_WORDS_PER_LONG]),
		-(ReadLong(&(stacktop[-CVM_WORDS_PER_LONG]))));
	MODIFY_PC_AND_STACK(1, 0);
}
VMBREAK;

#define	COP_FLOAT_OP(name,op)	\
VMCASE(COP_##name): \
{ \
	WriteFloat(&(stacktop[-(CVM_WORDS_PER_NATIVE_FLOAT * 2)]), \
		ReadFloat(&(stacktop[-(CVM_WORDS_PER_NATIVE_FLOAT * 2)])) \
		op ReadFloat(&(stacktop[-CVM_WORDS_PER_NATIVE_FLOAT]))); \
	MODIFY_PC_AND_STACK(1, -CVM_WORDS_PER_NATIVE_FLOAT); \
} \
VMBREAK

/**
 * <opcode name="fadd" group="Arithmetic operators">
 *   <operation>Add <code>native float</code></operation>
 *
 *   <format>fadd</format>
 *
 *   <form name="fadd" code="COP_FADD"/>
 *
 *   <before>..., value1, value2</before>
 *   <after>..., result</after>
 *
 *   <description>Both <i>value1</i> and <i>value2</i>
 *   are popped from the stack as type <code>native float</code>.  The
 *   <code>native float</code> <i>result</i> is <i>value1 + value2</i>.
 *   The <i>result</i> is pushed onto the stack.</description>
 *
 *   <notes>Values of type <code>native float</code> typically occupy
 *   multiple stack slots.  The exact number of slots is machine-dependent,
 *   as is the precision of the <code>native float</code> type.<p/>
 *
 *   When we say that <i>value1</i> and <i>value2</i> are popped,
 *   we assume that the correct number of stack slots for the machine
 *   are popped.  Similarly when <i>result</i> is pushed.<p/>
 *
 *   To perform strict 32-bit floating point addition, use <i>fadd</i>
 *   followed by <i>f2f</i>.  To perform strict 64-bit floating point
 *   addition, use <i>fadd</i> followed by <i>f2d</i>.</notes>
 * </opcode>
 */
COP_FLOAT_OP(FADD, +);

/**
 * <opcode name="fsub" group="Arithmetic operators">
 *   <operation>Subtract <code>native float</code></operation>
 *
 *   <format>fsub</format>
 *
 *   <form name="fsub" code="COP_FSUB"/>
 *
 *   <before>..., value1, value2</before>
 *   <after>..., result</after>
 *
 *   <description>Both <i>value1</i> and <i>value2</i>
 *   are popped from the stack as type <code>native float</code>.  The
 *   <code>native float</code> <i>result</i> is <i>value1 - value2</i>.
 *   The <i>result</i> is pushed onto the stack.</description>
 * </opcode>
 */
COP_FLOAT_OP(FSUB, -);

/**
 * <opcode name="fmul" group="Arithmetic operators">
 *   <operation>Multiply <code>native float</code></operation>
 *
 *   <format>fmul</format>
 *
 *   <form name="fmul" code="COP_FMUL"/>
 *
 *   <before>..., value1, value2</before>
 *   <after>..., result</after>
 *
 *   <description>Both <i>value1</i> and <i>value2</i>
 *   are popped from the stack as type <code>native float</code>.  The
 *   <code>native float</code> <i>result</i> is <i>value1 * value2</i>.
 *   The <i>result</i> is pushed onto the stack.</description>
 * </opcode>
 */
COP_FLOAT_OP(FMUL, *);

/**
 * <opcode name="fdiv" group="Arithmetic operators">
 *   <operation>Divide <code>native float</code></operation>
 *
 *   <format>fdiv</format>
 *
 *   <form name="fdiv" code="COP_FDIV"/>
 *
 *   <before>..., value1, value2</before>
 *   <after>..., result</after>
 *
 *   <description>Both <i>value1</i> and <i>value2</i>
 *   are popped from the stack as type <code>native float</code>.  The
 *   <code>native float</code> <i>result</i> is <i>value1 / value2</i>.
 *   The <i>result</i> is pushed onto the stack.</description>
 * </opcode>
 */
COP_FLOAT_OP(FDIV, /);

/**
 * <opcode name="frem" group="Arithmetic operators">
 *   <operation>Remainder <code>native float</code></operation>
 *
 *   <format>frem</format>
 *
 *   <form name="frem" code="COP_FREM"/>
 *
 *   <before>..., value1, value2</before>
 *   <after>..., result</after>
 *
 *   <description>Both <i>value1</i> and <i>value2</i>
 *   are popped from the stack as type <code>native float</code>.  The
 *   <code>native float</code> <i>result</i> is <i>value1 % value2</i>.
 *   The <i>result</i> is pushed onto the stack.</description>
 *
 *   <notes>The remainder operation is similar to the C <code>fmod</code>
 *   function, not IEEE remainder.</notes>
 * </opcode>
 */
VMCASE(COP_FREM):
{
	/* Floating point remainder */
	WriteFloat(&(stacktop[-(CVM_WORDS_PER_NATIVE_FLOAT * 2)]),
	  FRem(ReadFloat(&(stacktop
	  					[-(CVM_WORDS_PER_NATIVE_FLOAT * 2)])),
		   ReadFloat(&(stacktop[-CVM_WORDS_PER_NATIVE_FLOAT]))));
	MODIFY_PC_AND_STACK(1, -CVM_WORDS_PER_NATIVE_FLOAT);
}
VMBREAK;

/**
 * <opcode name="fneg" group="Arithmetic operators">
 *   <operation>Negate <code>native float</code></operation>
 *
 *   <format>fneg</format>
 *
 *   <form name="fneg" code="COP_FNEG"/>
 *
 *   <before>..., value</before>
 *   <after>..., result</after>
 *
 *   <description>The <i>value</i> is popped from the stack as type
 *   <code>native float</code>.  The <code>native float</code>
 *   <i>result</i> is <i>-value</i>.  The <i>result</i> is pushed
 *   onto the stack.</description>
 * </opcode>
 */
VMCASE(COP_FNEG):
{
	/* Floating point negate */
	WriteFloat(&(stacktop[-CVM_WORDS_PER_NATIVE_FLOAT]),
	  	-ReadFloat(&(stacktop[-CVM_WORDS_PER_NATIVE_FLOAT])));
	MODIFY_PC_AND_STACK(1, 0);
}
VMBREAK;

/**
 * <opcode name="iand" group="Bitwise operators">
 *   <operation>Bitwise AND <code>int32</code></operation>
 *
 *   <format>iand</format>
 *
 *   <form name="iand" code="COP_IAND"/>
 *
 *   <before>..., value1, value2</before>
 *   <after>..., result</after>
 *
 *   <description>Both <i>value1</i> and <i>value2</i>
 *   are popped from the stack as type <code>int32</code>.  The
 *   <code>int32</code> <i>result</i> is <i>value1 &amp; value2</i>.
 *   The <i>result</i> is pushed onto the stack.</description>
 *
 *   <notes>The <i>iand</i> instruction can also be used to AND
 *   values of type <code>uint32</code>.</notes>
 * </opcode>
 */
VMCASE(COP_IAND):
{
	/* Integer bitwise AND */
	stacktop[-2].intValue &= stacktop[-1].intValue;
	MODIFY_PC_AND_STACK(1, -1);
}
VMBREAK;

/**
 * <opcode name="ior" group="Bitwise operators">
 *   <operation>Bitwise OR <code>int32</code></operation>
 *
 *   <format>ior</format>
 *
 *   <form name="ior" code="COP_IOR"/>
 *
 *   <before>..., value1, value2</before>
 *   <after>..., result</after>
 *
 *   <description>Both <i>value1</i> and <i>value2</i>
 *   are popped from the stack as type <code>int32</code>.  The
 *   <code>int32</code> <i>result</i> is <i>value1 | value2</i>.
 *   The <i>result</i> is pushed onto the stack.</description>
 *
 *   <notes>The <i>ior</i> instruction can also be used to OR
 *   values of type <code>uint32</code>.</notes>
 * </opcode>
 */
VMCASE(COP_IOR):
{
	/* Integer bitwise OR */
	stacktop[-2].intValue |= stacktop[-1].intValue;
	MODIFY_PC_AND_STACK(1, -1);
}
VMBREAK;

/**
 * <opcode name="ixor" group="Bitwise operators">
 *   <operation>Bitwise XOR <code>int32</code></operation>
 *
 *   <format>ixor</format>
 *
 *   <form name="ixor" code="COP_IXOR"/>
 *
 *   <before>..., value1, value2</before>
 *   <after>..., result</after>
 *
 *   <description>Both <i>value1</i> and <i>value2</i>
 *   are popped from the stack as type <code>int32</code>.  The
 *   <code>int32</code> <i>result</i> is <i>value1 ^ value2</i>.
 *   The <i>result</i> is pushed onto the stack.</description>
 *
 *   <notes>The <i>ixor</i> instruction can also be used to XOR
 *   values of type <code>uint32</code>.</notes>
 * </opcode>
 */
VMCASE(COP_IXOR):
{
	/* Integer bitwise XOR */
	stacktop[-2].intValue ^= stacktop[-1].intValue;
	MODIFY_PC_AND_STACK(1, -1);
}
VMBREAK;

/**
 * <opcode name="inot" group="Bitwise operators">
 *   <operation>Bitwise NOT <code>int32</code></operation>
 *
 *   <format>inot</format>
 *
 *   <form name="inot" code="COP_INOT"/>
 *
 *   <before>..., value</before>
 *   <after>..., result</after>
 *
 *   <description>The <i>value</i> is popped from the stack as
 *   type <code>int32</code>.  The <code>int32</code> <i>result</i>
 *   is <i>~value</i>.  The <i>result</i> is pushed onto the
 *   stack.</description>
 *
 *   <notes>The <i>inot</i> instruction can also be used to NOT
 *   values of type <code>uint32</code>.</notes>
 * </opcode>
 */
VMCASE(COP_INOT):
{
	/* Integer bitwise NOT */
	stacktop[-1].intValue = ~(stacktop[-1].intValue);
	MODIFY_PC_AND_STACK(1, 0);
}
VMBREAK;

/**
 * <opcode name="ishl" group="Bitwise operators">
 *   <operation>Left shift <code>int32</code></operation>
 *
 *   <format>ishl</format>
 *
 *   <form name="ishl" code="COP_ISHL"/>
 *
 *   <before>..., value1, value2</before>
 *   <after>..., result</after>
 *
 *   <description>Both <i>value1</i> and <i>value2</i>
 *   are popped from the stack as the types <code>int32</code>
 *   and <code>uint32</code> respectively.  The <code>int32</code>
 *   <i>result</i> is <i>(value1 &lt;&lt; (value2 &amp; 0x1F))</i>.
 *   Bits that are shifted out the top of <i>value1</i> are discarded.
 *   The <i>result</i> is pushed onto the stack.</description>
 *
 *   <notes>The <i>ishl</i> instruction can also be used to shift
 *   values of type <code>uint32</code>.</notes>
 * </opcode>
 */
VMCASE(COP_ISHL):
{
	/* Integer shift left */
	stacktop[-2].intValue <<= (stacktop[-1].uintValue & 0x1F);
	MODIFY_PC_AND_STACK(1, -1);
}
VMBREAK;

/**
 * <opcode name="ishr" group="Bitwise operators">
 *   <operation>Right arithmetic shift <code>int32</code></operation>
 *
 *   <format>ishr</format>
 *
 *   <form name="ishr" code="COP_ISHR"/>
 *
 *   <before>..., value1, value2</before>
 *   <after>..., result</after>
 *
 *   <description>Both <i>value1</i> and <i>value2</i>
 *   are popped from the stack as the types <code>int32</code>
 *   and <code>uint32</code> respectively.  The <code>int32</code>
 *   <i>result</i> is <i>(value1 &gt;&gt; (value2 &amp; 0x1F))</i>.
 *   The top-most bit of <i>value1</i> is used to fill new bits shifted
 *   in from the top.  The <i>result</i> is pushed onto the
 *   stack.</description>
 * </opcode>
 */
VMCASE(COP_ISHR):
{
	/* Integer shift right */
	stacktop[-2].intValue >>= (stacktop[-1].uintValue & 0x1F);
	MODIFY_PC_AND_STACK(1, -1);
}
VMBREAK;

/**
 * <opcode name="ishr_un" group="Bitwise operators">
 *   <operation>Right unsigned shift <code>uint32</code></operation>
 *
 *   <format>ishr_un</format>
 *
 *   <form name="ishr_un" code="COP_ISHR_UN"/>
 *
 *   <before>..., value1, value2</before>
 *   <after>..., result</after>
 *
 *   <description>Both <i>value1</i> and <i>value2</i>
 *   are popped from the stack as type <code>uint32</code>.  The
 *   <code>uint32</code> <i>result</i> is <i>(value1 &gt;&gt;
 *   (value2 &amp; 0x1F))</i>.  Zeroes are used to fill new
 *   bits shifted in from the top.  The <i>result</i> is pushed
 *   onto the stack.</description>
 * </opcode>
 */
VMCASE(COP_ISHR_UN):
{
	/* Unsigned integer shift right */
	stacktop[-2].uintValue >>= (stacktop[-1].uintValue & 0x1F);
	MODIFY_PC_AND_STACK(1, -1);
}
VMBREAK;

/**
 * <opcode name="land" group="Bitwise operators">
 *   <operation>Bitwise AND <code>int64</code></operation>
 *
 *   <format>land</format>
 *
 *   <form name="land" code="COP_LAND"/>
 *
 *   <before>..., value1, value2</before>
 *   <after>..., result</after>
 *
 *   <description>Both <i>value1</i> and <i>value2</i>
 *   are popped from the stack as type <code>int64</code>.  The
 *   <code>int64</code> <i>result</i> is <i>value1 &amp; value2</i>.
 *   The <i>result</i> is pushed onto the stack.</description>
 *
 *   <notes>The <i>land</i> instruction can also be used to AND
 *   values of type <code>uint64</code>.</notes>
 * </opcode>
 */
VMCASE(COP_LAND):
{
	/* Long bitwise AND */
	WriteLong(&(stacktop[-(CVM_WORDS_PER_LONG * 2)]),
		ReadLong(&(stacktop[-(CVM_WORDS_PER_LONG * 2)])) &
		ReadLong(&(stacktop[-CVM_WORDS_PER_LONG])));
	MODIFY_PC_AND_STACK(1, -CVM_WORDS_PER_LONG);
}
VMBREAK;

/**
 * <opcode name="lor" group="Bitwise operators">
 *   <operation>Bitwise OR <code>int64</code></operation>
 *
 *   <format>lor</format>
 *
 *   <form name="lor" code="COP_LOR"/>
 *
 *   <before>..., value1, value2</before>
 *   <after>..., result</after>
 *
 *   <description>Both <i>value1</i> and <i>value2</i>
 *   are popped from the stack as type <code>int64</code>.  The
 *   <code>int64</code> <i>result</i> is <i>value1 | value2</i>.
 *   The <i>result</i> is pushed onto the stack.</description>
 *
 *   <notes>The <i>lor</i> instruction can also be used to OR
 *   values of type <code>uint64</code>.</notes>
 * </opcode>
 */
VMCASE(COP_LOR):
{
	/* Long bitwise OR */
	WriteLong(&(stacktop[-(CVM_WORDS_PER_LONG * 2)]),
		ReadLong(&(stacktop[-(CVM_WORDS_PER_LONG * 2)])) |
		ReadLong(&(stacktop[-CVM_WORDS_PER_LONG])));
	MODIFY_PC_AND_STACK(1, -CVM_WORDS_PER_LONG);
}
VMBREAK;

/**
 * <opcode name="lxor" group="Bitwise operators">
 *   <operation>Bitwise XOR <code>int64</code></operation>
 *
 *   <format>lxor</format>
 *
 *   <form name="lxor" code="COP_LXOR"/>
 *
 *   <before>..., value1, value2</before>
 *   <after>..., result</after>
 *
 *   <description>Both <i>value1</i> and <i>value2</i>
 *   are popped from the stack as type <code>int64</code>.  The
 *   <code>int64</code> <i>result</i> is <i>value1 ^ value2</i>.
 *   The <i>result</i> is pushed onto the stack.</description>
 *
 *   <notes>The <i>lxor</i> instruction can also be used to XOR
 *   values of type <code>uint64</code>.</notes>
 * </opcode>
 */
VMCASE(COP_LXOR):
{
	/* Long bitwise XOR */
	WriteLong(&(stacktop[-(CVM_WORDS_PER_LONG * 2)]),
		ReadLong(&(stacktop[-(CVM_WORDS_PER_LONG * 2)])) ^
		ReadLong(&(stacktop[-CVM_WORDS_PER_LONG])));
	MODIFY_PC_AND_STACK(1, -CVM_WORDS_PER_LONG);
}
VMBREAK;

/**
 * <opcode name="lnot" group="Bitwise operators">
 *   <operation>Bitwise NOT <code>int64</code></operation>
 *
 *   <format>lnot</format>
 *
 *   <form name="lnot" code="COP_LNOT"/>
 *
 *   <before>..., value</before>
 *   <after>..., result</after>
 *
 *   <description>The <i>value</i> is popped from the stack as
 *   type <code>int64</code>.  The <code>int64</code> <i>result</i>
 *   is <i>~value</i>.  The <i>result</i> is pushed onto the
 *   stack.</description>
 *
 *   <notes>The <i>lnot</i> instruction can also be used to NOT
 *   values of type <code>uint64</code>.</notes>
 * </opcode>
 */
VMCASE(COP_LNOT):
{
	/* Long bitwise NOT */
	WriteLong(&(stacktop[-CVM_WORDS_PER_LONG]),
		~(ReadLong(&(stacktop[-CVM_WORDS_PER_LONG]))));
	MODIFY_PC_AND_STACK(1, 0);
}
VMBREAK;

/**
 * <opcode name="lshl" group="Bitwise operators">
 *   <operation>Left shift <code>int64</code></operation>
 *
 *   <format>lshl</format>
 *
 *   <form name="lshl" code="COP_LSHL"/>
 *
 *   <before>..., value1, value2</before>
 *   <after>..., result</after>
 *
 *   <description>Both <i>value1</i> and <i>value2</i>
 *   are popped from the stack as the types <code>int64</code>
 *   and <code>uint32</code> respectively.  The <code>int64</code>
 *   <i>result</i> is <i>(value1 &lt;&lt; (value2 &amp; 0x3F))</i>.
 *   Bits that are shifted out the top of <i>value1</i> are discarded.
 *   The <i>result</i> is pushed onto the stack.</description>
 *
 *   <notes>The <i>lshl</i> instruction can also be used to shift
 *   values of type <code>uint64</code>.</notes>
 * </opcode>
 */
VMCASE(COP_LSHL):
{
	/* Long shift left */
	WriteLong(&(stacktop[-(CVM_WORDS_PER_LONG + 1)]),
		ReadLong(&(stacktop[-(CVM_WORDS_PER_LONG + 1)])) <<
	    (stacktop[-1].uintValue & 0x3F));
	MODIFY_PC_AND_STACK(1, -1);
}
VMBREAK;

/**
 * <opcode name="lshr" group="Bitwise operators">
 *   <operation>Right arithmetic shift <code>int64</code></operation>
 *
 *   <format>lshr</format>
 *
 *   <form name="lshr" code="COP_LSHR"/>
 *
 *   <before>..., value1, value2</before>
 *   <after>..., result</after>
 *
 *   <description>Both <i>value1</i> and <i>value2</i>
 *   are popped from the stack as the types <code>int64</code>
 *   and <code>uint32</code> respectively.  The <code>int64</code>
 *   <i>result</i> is <i>(value1 &gt;&gt; (value2 &amp; 0x3F))</i>.
 *   The top-most bit of <i>value1</i> is used to fill new bits shifted
 *   in from the top.  The <i>result</i> is pushed onto the
 *   stack.</description>
 * </opcode>
 */
VMCASE(COP_LSHR):
{
	/* Long shift right */
	WriteLong(&(stacktop[-(CVM_WORDS_PER_LONG + 1)]),
		ReadLong(&(stacktop[-(CVM_WORDS_PER_LONG + 1)])) >>
	    (stacktop[-1].uintValue & 0x3F));
	MODIFY_PC_AND_STACK(1, -1);
}
VMBREAK;

/**
 * <opcode name="lshr_un" group="Bitwise operators">
 *   <operation>Right unsigned shift <code>uint64</code></operation>
 *
 *   <format>lshr_un</format>
 *
 *   <form name="lshr_un" code="COP_LSHR_UN"/>
 *
 *   <before>..., value1, value2</before>
 *   <after>..., result</after>
 *
 *   <description>Both <i>value1</i> and <i>value2</i>
 *   are popped from the stack as the types <code>uint64</code>
 *   and <code>uint32</code> respectively.  The <code>uint64</code>
 *   <i>result</i> is <i>(value1 &gt;&gt; (value2 &amp; 0x3F))</i>.
 *   Zeroes are used to fill new bits shifted in from the top.
 *   The <i>result</i> is pushed onto the stack.</description>
 * </opcode>
 */
VMCASE(COP_LSHR_UN):
{
	/* Unsigned long shift right */
	WriteULong(&(stacktop[-(CVM_WORDS_PER_LONG + 1)]),
		ReadULong(&(stacktop[-(CVM_WORDS_PER_LONG + 1)])) >>
	    (stacktop[-1].uintValue & 0x3F));
	MODIFY_PC_AND_STACK(1, -1);
}
VMBREAK;

#elif defined(IL_CVM_PREFIX)

/**
 * <opcode name="ckfinite" group="Arithmetic operators">
 *   <operation>Check <code>native float</code> for finite</operation>
 *
 *   <format>prefix<fsep/>ckfinite</format>
 *
 *   <form name="ckfinite" code="COP_PREFIX_CKFINITE"/>
 *
 *   <before>..., value</before>
 *   <after>...</after>
 *
 *   <description>The <i>value</i> is popped from the stack as type
 *   <code>native float</code>.  If <i>value</i> is not finite (i.e.
 *   it is NaN, positive infinity, or negative infinity), then
 *   <code>System.ArithmeticException</code> is thrown.</description>
 *
 *   <exceptions>
 *     <exception name="System.ArithmeticException">Raised if
 *     <i>value</i> is not finite.</exception>
 *   </exceptions>
 * </opcode>
 */
VMCASE(COP_PREFIX_CKFINITE):
{
	/* Check the top-most float value to see if it is finite */
	if(FLOAT_IS_FINITE(ReadFloat(&(stacktop[-CVM_WORDS_PER_NATIVE_FLOAT]))))
	{
		MODIFY_PC_AND_STACK(2, 0);
	}
	else
	{
		ARITHMETIC_EXCEPTION();
	}
}
VMBREAK;

#endif /* IL_CVM_PREFIX */
