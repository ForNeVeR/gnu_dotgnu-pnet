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

case COP_I2B:
{
	/* Convert from integer to signed byte */
	stacktop[-1].intValue = (ILInt32)(ILInt8)(stacktop[-1].intValue);
	MODIFY_PC_AND_STACK(1, 0);
}
break;

case COP_I2UB:
{
	/* Convert from integer to unsigned byte */
	stacktop[-1].intValue = (ILInt32)(ILUInt8)(stacktop[-1].intValue);
	MODIFY_PC_AND_STACK(1, 0);
}
break;

case COP_I2S:
{
	/* Convert from integer to signed short */
	stacktop[-1].intValue = (ILInt32)(ILInt16)(stacktop[-1].intValue);
	MODIFY_PC_AND_STACK(1, 0);
}
break;

case COP_I2US:
{
	/* Convert from integer to unsigned short */
	stacktop[-1].intValue = (ILInt32)(ILUInt16)(stacktop[-1].intValue);
	MODIFY_PC_AND_STACK(1, 0);
}
break;

case COP_I2L:
{
	/* Convert from integer to long */
	WriteLong(&(stacktop[-1]), (ILInt64)(stacktop[-1].intValue));
	MODIFY_PC_AND_STACK(1, CVM_WORDS_PER_LONG - 1);
}
break;

case COP_IU2L:
{
	/* Convert from unsigned integer to long */
	WriteLong(&(stacktop[-1]), (ILInt64)(stacktop[-1].uintValue));
	MODIFY_PC_AND_STACK(1, CVM_WORDS_PER_LONG - 1);
}
break;

case COP_I2F:
{
	/* Convert from integer to "native float" */
	WriteFloat(&(stacktop[-1]), (ILNativeFloat)(stacktop[-1].intValue));
	MODIFY_PC_AND_STACK(1, CVM_WORDS_PER_NATIVE_FLOAT - 1);
}
break;

case COP_IU2F:
{
	/* Convert from unsigned integer to "native float" */
	WriteFloat(&(stacktop[-1]), (ILNativeFloat)(stacktop[-1].uintValue));
	MODIFY_PC_AND_STACK(1, CVM_WORDS_PER_NATIVE_FLOAT - 1);
}
break;

case COP_L2I:
{
	/* Convert from long to integer */
	stacktop[-CVM_WORDS_PER_LONG].intValue =
		(ILInt32)(ReadLong(&(stacktop[-CVM_WORDS_PER_LONG])));
	MODIFY_PC_AND_STACK(1, -(CVM_WORDS_PER_LONG - 1));
}
break;

case COP_L2F:
{
	/* Convert from long to "native float" */
	WriteFloat(&(stacktop[-CVM_WORDS_PER_LONG]),
	   (ILNativeFloat)(ReadLong(&(stacktop[-CVM_WORDS_PER_LONG]))));
	MODIFY_PC_AND_STACK(1, CVM_WORDS_PER_NATIVE_FLOAT -
						   CVM_WORDS_PER_LONG);
}
break;

case COP_LU2F:
{
	/* Convert from unsigned long to "native float" */
	WriteFloat(&(stacktop[-CVM_WORDS_PER_LONG]),
			   LU2F(ReadULong(&(stacktop[-CVM_WORDS_PER_LONG]))));
	MODIFY_PC_AND_STACK(1, CVM_WORDS_PER_NATIVE_FLOAT -
						   CVM_WORDS_PER_LONG);
}
break;

case COP_F2I:
{
	/* Convert from "native float" to integer */
	stacktop[-CVM_WORDS_PER_NATIVE_FLOAT].intValue = (ILInt32)
		(ReadFloat(&(stacktop[-CVM_WORDS_PER_NATIVE_FLOAT])));
	MODIFY_PC_AND_STACK(1, -(CVM_WORDS_PER_NATIVE_FLOAT - 1));
}
break;

case COP_F2IU:
{
	/* Convert from "native float" to unsigned integer */
	stacktop[-CVM_WORDS_PER_NATIVE_FLOAT].uintValue = (ILUInt32)
		(ReadFloat(&(stacktop[-CVM_WORDS_PER_NATIVE_FLOAT])));
	MODIFY_PC_AND_STACK(1, -(CVM_WORDS_PER_NATIVE_FLOAT - 1));
}
break;

case COP_F2L:
{
	/* Convert from "native float" to long */
	WriteLong(&(stacktop[-CVM_WORDS_PER_NATIVE_FLOAT]), (ILInt64)
		ReadFloat(&(stacktop[-CVM_WORDS_PER_NATIVE_FLOAT])));
	MODIFY_PC_AND_STACK(1, -(CVM_WORDS_PER_NATIVE_FLOAT -
							 CVM_WORDS_PER_LONG));
}
break;

case COP_F2LU:
{
	/* Convert from "native float" to unsigned long */
	WriteULong(&(stacktop[-CVM_WORDS_PER_NATIVE_FLOAT]),
			   F2LU(ReadFloat(&(stacktop[-CVM_WORDS_PER_NATIVE_FLOAT]))));
	MODIFY_PC_AND_STACK(1, -(CVM_WORDS_PER_NATIVE_FLOAT -
							 CVM_WORDS_PER_LONG));
}
break;

case COP_F2F:
{
	/* Convert from "native float" to "float" */
	WriteFloat(&(stacktop[-CVM_WORDS_PER_NATIVE_FLOAT]),
		(ILNativeFloat)(ILFloat)
		ReadFloat(&(stacktop[-CVM_WORDS_PER_NATIVE_FLOAT])));
	MODIFY_PC_AND_STACK(1, 0);
}
break;

case COP_F2D:
{
	/* Convert from "native float" to "double" */
	WriteFloat(&(stacktop[-CVM_WORDS_PER_NATIVE_FLOAT]),
		(ILNativeFloat)(ILDouble)
		ReadFloat(&(stacktop[-CVM_WORDS_PER_NATIVE_FLOAT])));
	MODIFY_PC_AND_STACK(1, 0);
}
break;

case COP_I2P_LOWER:
{
	/* Convert an I4 value into a pointer value that
	   is lower down on the stack */
	position = -(((ILInt32)(pc[1])) + 1);
	stacktop[position].ptrValue = (void *)(stacktop[position].uintValue);
	MODIFY_PC_AND_STACK(2, 0);
}
break;

case COP_FIX_I4_I:
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
break;

case COP_FIX_I4_U:
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
break;

#elif defined(IL_CVM_WIDE)

case COP_I2P_LOWER:
{
	/* Convert an I4 value into a pointer value that
	   is lower down on the stack */
	position = -(IL_READ_INT32(pc + 2) + 1);
	stacktop[position].ptrValue = (void *)(stacktop[position].uintValue);
	MODIFY_PC_AND_STACK(6, 0);
}
break;

#elif defined(IL_CVM_PREFIX)

case COP_PREFIX_I2B_OVF:
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
break;

case COP_PREFIX_I2UB_OVF:
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
break;

case COP_PREFIX_IU2B_OVF:
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
break;

case COP_PREFIX_IU2UB_OVF:
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
break;

case COP_PREFIX_I2S_OVF:
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
break;

case COP_PREFIX_I2US_OVF:
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
break;

case COP_PREFIX_IU2S_OVF:
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
break;

case COP_PREFIX_IU2US_OVF:
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
break;

case COP_PREFIX_I2IU_OVF:
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
break;

case COP_PREFIX_IU2I_OVF:
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
break;

case COP_PREFIX_I2UL_OVF:
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
break;

case COP_PREFIX_L2I_OVF:
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
break;

case COP_PREFIX_L2UI_OVF:
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
break;

case COP_PREFIX_LU2I_OVF:
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
break;

case COP_PREFIX_LU2IU_OVF:
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
break;

case COP_PREFIX_L2UL_OVF:
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
break;

case COP_PREFIX_LU2L_OVF:
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
break;

case COP_PREFIX_F2I_OVF:
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
break;

case COP_PREFIX_F2IU_OVF:
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
break;

case COP_PREFIX_F2L_OVF:
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
break;

case COP_PREFIX_F2LU_OVF:
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
break;

case COP_PREFIX_I2B_ALIGNED:
{
	/* Convert a 32-bit value into a byte and align it on a word boundary */
	*((ILInt8 *)(stacktop - 1)) = (ILInt8)(stacktop[-1].intValue);
	MODIFY_PC_AND_STACK(2, 0);
}
break;

case COP_PREFIX_I2S_ALIGNED:
{
	/* Convert a 32-bit value into a short and align it on a word boundary */
	*((ILInt16 *)(stacktop - 1)) = (ILInt16)(stacktop[-1].intValue);
	MODIFY_PC_AND_STACK(2, 0);
}
break;

case COP_PREFIX_F2F_ALIGNED:
{
	/* Convert a native float into a float32 and align it on a word boundary */
	*((ILFloat *)(stacktop - CVM_WORDS_PER_NATIVE_FLOAT)) =
			   ReadFloat(stacktop - CVM_WORDS_PER_NATIVE_FLOAT);
	MODIFY_PC_AND_STACK(2, CVM_WORDS_PER_FLOAT - CVM_WORDS_PER_NATIVE_FLOAT);
}
break;

case COP_PREFIX_F2D_ALIGNED:
{
	/* Convert a native float into a float64 and align it on a word boundary */
	WriteDouble(stacktop - CVM_WORDS_PER_NATIVE_FLOAT,
			    ReadFloat(stacktop - CVM_WORDS_PER_NATIVE_FLOAT));
	MODIFY_PC_AND_STACK(2, CVM_WORDS_PER_DOUBLE - CVM_WORDS_PER_NATIVE_FLOAT);
}
break;

case COP_PREFIX_STR2ANSI:
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
break;

case COP_PREFIX_STR2UTF8:
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
break;

#endif /* IL_CVM_PREFIX */
