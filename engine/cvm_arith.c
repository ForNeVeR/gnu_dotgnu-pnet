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
 * Multiply two uint64 values and check for overflow.
 * This would be easier if there was a portable way
 * to multiply two 64-bit values to get a 128-bit
 * result.
 */
static int MulUInt64(ILUInt64 value1, ILUInt64 value2, ILUInt64 *product)
{
	ILUInt32 high1, low1, high2, low2, orig;
	ILUInt64 temp;
	ILUInt32 result1, result2, result3, result4;
	high1 = (ILUInt32)(value1 >> 32);
	low1  = (ILUInt32)value1;
	high2 = (ILUInt32)(value2 >> 32);
	low2  = (ILUInt32)value2;
	temp = low1 * low2;
	result1 = (ILUInt32)temp;
	result2 = (ILUInt32)(temp >> 32);
	temp = low1 * high2;
	orig = result2;
	result2 += (ILUInt32)temp;
	if(result2 < orig)
		result3 = (((ILUInt32)(temp >> 32)) + 1);
	else
		result3 = ((ILUInt32)(temp >> 32));
	temp = high1 * low2;
	orig = result2;
	result2 += (ILUInt32)temp;
	if(result2 < orig)
	{
		orig = result3;
		result3 += (((ILUInt32)(temp >> 32)) + 1);
		if(result3 < orig)
			result4 = 1;
		else
			result4 = 0;
	}
	else
	{
		orig = result3;
		result3 += ((ILUInt32)(temp >> 32));
		if(result3 < orig)
			result4 = 1;
		else
			result4 = 0;
	}
	temp = high1 * high2;
	orig = result3;
	result3 += (ILUInt32)temp;
	if(result3 < orig)
		result4 += ((ILUInt32)(temp >> 32)) + 1;
	else
		result4 += ((ILUInt32)(temp >> 32));
	if(result3 != 0 || result4 != 0)
	{
		return 0;
	}
	*product = (((ILUInt64)result2) << 32) | ((ILUInt64)result1);
	return 1;
}

/*
 * Multiply two int64 values and check for overflow.
 * This would be easier if there was a portable way
 * to multiply two 64-bit values to get a 128-bit
 * result.
 */
static int MulInt64(ILInt64 value1, ILInt64 value2, ILInt64 *product)
{
	ILUInt64 temp;
	if(value1 >= 0 && value2 >= 0)
	{
		/* Both values are positive */
		if(!MulUInt64((ILUInt64)value1, (ILUInt64)value2, &temp))
		{
			return 0;
		}
		if(temp > ((ILUInt64)IL_MAX_INT64))
		{
			return 0;
		}
		*product = (ILInt64)temp;
		return 1;
	}
	else if(value1 >= 0)
	{
		/* The first value is positive */
		if(!MulUInt64((ILUInt64)value1, (ILUInt64)-value2, &temp))
		{
			return 0;
		}
		if(temp > (((ILUInt64)IL_MAX_INT64) + 1))
		{
			return 0;
		}
		*product = -((ILInt64)temp);
		return 1;
	}
	else if(value2 >= 0)
	{
		/* The second value is positive */
		if(!MulUInt64((ILUInt64)-value1, (ILUInt64)value2, &temp))
		{
			return 0;
		}
		if(temp > (((ILUInt64)IL_MAX_INT64) + 1))
		{
			return 0;
		}
		*product = -((ILInt64)temp);
		return 1;
	}
	else
	{
		/* Both values are negative */
		if(!MulUInt64((ILUInt64)-value1, (ILUInt64)-value2, &temp))
		{
			return 0;
		}
		if(temp > ((ILUInt64)IL_MAX_INT64))
		{
			return 0;
		}
		*product = (ILInt64)temp;
		return 1;
	}
}

/*
 * Long multiply with overflow detection.
 */
static IL_INLINE int LMulOvf(CVMWord *a, CVMWord *b)
{
	ILInt64 result;
	if(!MulInt64(ReadLong(a), ReadLong(b), &result))
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
	if(!MulUInt64(ReadULong(a), ReadULong(b), &result))
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
#ifdef HAVE_FMOD
	return fmod(a, b);
#else
	/* We can't compute an answer, so default to NaN */
	return (ILNativeFloat)(0.0 / 0.0);
#endif
}

#elif defined(IL_CVM_LOCALS)

/* No locals required */

#elif defined(IL_CVM_MAIN)

case COP_IADD:
{
	/* Integer add */
	stacktop[-2].intValue += stacktop[-1].intValue;
	MODIFY_PC_AND_STACK(1, -1);
}
break;

case COP_IADD_OVF:
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
break;

case COP_IADD_OVF_UN:
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
break;

case COP_ISUB:
{
	/* Integer subtract */
	stacktop[-2].intValue -= stacktop[-1].intValue;
	MODIFY_PC_AND_STACK(1, -1);
}
break;

case COP_ISUB_OVF:
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
break;

case COP_ISUB_OVF_UN:
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
break;

case COP_IMUL:
{
	/* Integer (and unsigned integer) multiply */
	stacktop[-2].intValue *= stacktop[-1].intValue;
	MODIFY_PC_AND_STACK(1, -1);
}
break;

case COP_IMUL_OVF:
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
break;

case COP_IMUL_OVF_UN:
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
break;

case COP_IDIV:
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
break;

case COP_IDIV_UN:
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
break;

case COP_IREM:
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
break;

case COP_IREM_UN:
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
break;

case COP_INEG:
{
	/* Integer negate */
	stacktop[-1].intValue = -(stacktop[-1].intValue);
	MODIFY_PC_AND_STACK(1, 0);
}
break;

case COP_LADD:
{
	/* Long add */
	WriteLong(&(stacktop[-(CVM_WORDS_PER_LONG * 2)]),
		ReadLong(&(stacktop[-(CVM_WORDS_PER_LONG * 2)])) +
		ReadLong(&(stacktop[-CVM_WORDS_PER_LONG])));
	MODIFY_PC_AND_STACK(1, -CVM_WORDS_PER_LONG);
}
break;

case COP_LADD_OVF:
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
break;

case COP_LADD_OVF_UN:
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
break;

case COP_LSUB:
{
	/* Long subtract */
	WriteLong(&(stacktop[-(CVM_WORDS_PER_LONG * 2)]),
		ReadLong(&(stacktop[-(CVM_WORDS_PER_LONG * 2)])) -
		ReadLong(&(stacktop[-CVM_WORDS_PER_LONG])));
	MODIFY_PC_AND_STACK(1, -CVM_WORDS_PER_LONG);
}
break;

case COP_LSUB_OVF:
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
break;

case COP_LSUB_OVF_UN:
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
break;

case COP_LMUL:
{
	/* Long (and unsigned long) multiply */
	WriteLong(&(stacktop[-(CVM_WORDS_PER_LONG * 2)]),
		ReadLong(&(stacktop[-(CVM_WORDS_PER_LONG * 2)])) *
		ReadLong(&(stacktop[-CVM_WORDS_PER_LONG])));
	MODIFY_PC_AND_STACK(1, -CVM_WORDS_PER_LONG);
}
break;

case COP_LMUL_OVF:
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
break;

case COP_LMUL_OVF_UN:
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
break;

case COP_LDIV:
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
break;

case COP_LDIV_UN:
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
break;

case COP_LREM:
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
break;

case COP_LREM_UN:
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
break;

case COP_LNEG:
{
	/* Long negate */
	WriteLong(&(stacktop[-CVM_WORDS_PER_LONG]),
		-(ReadLong(&(stacktop[-CVM_WORDS_PER_LONG]))));
	MODIFY_PC_AND_STACK(1, 0);
}
break;

#define	COP_FLOAT_OP(name,op)	\
case COP_##name: \
{ \
	WriteFloat(&(stacktop[-(CVM_WORDS_PER_NATIVE_FLOAT * 2)]), \
		ReadFloat(&(stacktop[-(CVM_WORDS_PER_NATIVE_FLOAT * 2)])) \
		op ReadFloat(&(stacktop[-CVM_WORDS_PER_NATIVE_FLOAT]))); \
	MODIFY_PC_AND_STACK(1, -CVM_WORDS_PER_NATIVE_FLOAT); \
} \
break

/* Binary floating point operations */
COP_FLOAT_OP(FADD, +);
COP_FLOAT_OP(FSUB, -);
COP_FLOAT_OP(FMUL, *);
COP_FLOAT_OP(FDIV, /);

case COP_FREM:
{
	/* Floating point remainder */
	WriteFloat(&(stacktop[-(CVM_WORDS_PER_NATIVE_FLOAT * 2)]),
	  FRem(ReadFloat(&(stacktop
	  					[-(CVM_WORDS_PER_NATIVE_FLOAT * 2)])),
		   ReadFloat(&(stacktop[-CVM_WORDS_PER_NATIVE_FLOAT]))));
	MODIFY_PC_AND_STACK(1, -CVM_WORDS_PER_NATIVE_FLOAT);
}
break;

case COP_FNEG:
{
	/* Floating point negate */
	WriteFloat(&(stacktop[-CVM_WORDS_PER_NATIVE_FLOAT]),
	  	-ReadFloat(&(stacktop[-CVM_WORDS_PER_NATIVE_FLOAT])));
	MODIFY_PC_AND_STACK(1, 0);
}
break;

case COP_IAND:
{
	/* Integer bitwise AND */
	stacktop[-2].intValue &= stacktop[-1].intValue;
	MODIFY_PC_AND_STACK(1, -1);
}
break;

case COP_IOR:
{
	/* Integer bitwise OR */
	stacktop[-2].intValue |= stacktop[-1].intValue;
	MODIFY_PC_AND_STACK(1, -1);
}
break;

case COP_IXOR:
{
	/* Integer bitwise XOR */
	stacktop[-2].intValue ^= stacktop[-1].intValue;
	MODIFY_PC_AND_STACK(1, -1);
}
break;

case COP_INOT:
{
	/* Integer bitwise NOT */
	stacktop[-1].intValue = ~(stacktop[-1].intValue);
	MODIFY_PC_AND_STACK(1, 0);
}
break;

case COP_ISHL:
{
	/* Integer shift left */
	stacktop[-2].intValue <<= (stacktop[-1].uintValue & 0x1F);
	MODIFY_PC_AND_STACK(1, -1);
}
break;

case COP_ISHR:
{
	/* Integer shift right */
	stacktop[-2].intValue >>= (stacktop[-1].uintValue & 0x1F);
	MODIFY_PC_AND_STACK(1, -1);
}
break;

case COP_ISHR_UN:
{
	/* Unsigned integer shift right */
	stacktop[-2].uintValue >>= (stacktop[-1].uintValue & 0x1F);
	MODIFY_PC_AND_STACK(1, -1);
}
break;

case COP_LAND:
{
	/* Long bitwise AND */
	WriteLong(&(stacktop[-(CVM_WORDS_PER_LONG * 2)]),
		ReadLong(&(stacktop[-(CVM_WORDS_PER_LONG * 2)])) &
		ReadLong(&(stacktop[-CVM_WORDS_PER_LONG])));
	MODIFY_PC_AND_STACK(1, -CVM_WORDS_PER_LONG);
}
break;

case COP_LOR:
{
	/* Long bitwise OR */
	WriteLong(&(stacktop[-(CVM_WORDS_PER_LONG * 2)]),
		ReadLong(&(stacktop[-(CVM_WORDS_PER_LONG * 2)])) |
		ReadLong(&(stacktop[-CVM_WORDS_PER_LONG])));
	MODIFY_PC_AND_STACK(1, -CVM_WORDS_PER_LONG);
}
break;

case COP_LXOR:
{
	/* Long bitwise XOR */
	WriteLong(&(stacktop[-(CVM_WORDS_PER_LONG * 2)]),
		ReadLong(&(stacktop[-(CVM_WORDS_PER_LONG * 2)])) ^
		ReadLong(&(stacktop[-CVM_WORDS_PER_LONG])));
	MODIFY_PC_AND_STACK(1, -CVM_WORDS_PER_LONG);
}
break;

case COP_LNOT:
{
	/* Long bitwise NOT */
	WriteLong(&(stacktop[-CVM_WORDS_PER_LONG]),
		~(ReadLong(&(stacktop[-CVM_WORDS_PER_LONG]))));
	MODIFY_PC_AND_STACK(1, 0);
}
break;

case COP_LSHL:
{
	/* Long shift left */
	WriteLong(&(stacktop[-(CVM_WORDS_PER_LONG + 1)]),
		ReadLong(&(stacktop[-(CVM_WORDS_PER_LONG + 1)])) <<
	    (stacktop[-1].uintValue & 0x3F));
	MODIFY_PC_AND_STACK(1, -1);
}
break;

case COP_LSHR:
{
	/* Long shift right */
	WriteLong(&(stacktop[-(CVM_WORDS_PER_LONG + 1)]),
		ReadLong(&(stacktop[-(CVM_WORDS_PER_LONG + 1)])) >>
	    (stacktop[-1].uintValue & 0x3F));
	MODIFY_PC_AND_STACK(1, -1);
}
break;

case COP_LSHR_UN:
{
	/* Unsigned long shift right */
	WriteULong(&(stacktop[-(CVM_WORDS_PER_LONG + 1)]),
		ReadULong(&(stacktop[-(CVM_WORDS_PER_LONG + 1)])) >>
	    (stacktop[-1].uintValue & 0x3F));
	MODIFY_PC_AND_STACK(1, -1);
}
break;

#elif defined(IL_CVM_PREFIX)

case COP_PREFIX_CKFINITE:
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
break;

#endif /* IL_CVM_PREFIX */
