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
 * Compare two long values.
 */
static IL_INLINE ILInt32 LCmp(CVMWord *a, CVMWord *b)
{
	ILInt64 tempa = ReadLong(a);
	ILInt64 tempb = ReadLong(b);
	if(tempa < tempb)
	{
		return -1;
	}
	else if(tempa > tempb)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

/*
 * Compare two unsigned long values.
 */
static IL_INLINE ILInt32 LUCmp(CVMWord *a, CVMWord *b)
{
	ILUInt64 tempa = ReadULong(a);
	ILUInt64 tempb = ReadULong(b);
	if(tempa < tempb)
	{
		return -1;
	}
	else if(tempa > tempb)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

/*
 * Compare two native float values.
 */
static IL_INLINE ILInt32 FCmp(CVMWord *a, CVMWord *b, ILInt32 nanResult)
{
	ILNativeFloat tempa = ReadFloat(a);
	ILNativeFloat tempb = ReadFloat(b);
	if(isnan(tempa) || isnan(tempb))
	{
		return nanResult;
	}
	else if(tempa < tempb)
	{
		return -1;
	}
	else if(tempa > tempb)
	{
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
		(ILNativeFloat)(ReadULong
							(&(stacktop[-CVM_WORDS_PER_LONG]))));
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
	WriteULong(&(stacktop[-CVM_WORDS_PER_NATIVE_FLOAT]), (ILUInt64)
		ReadFloat(&(stacktop[-CVM_WORDS_PER_NATIVE_FLOAT])));
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
	/* TODO */
	MODIFY_PC_AND_STACK(2, 0);
}
break;

case COP_PREFIX_I2UB_OVF:
{
	/* TODO */
	MODIFY_PC_AND_STACK(2, 0);
}
break;

case COP_PREFIX_IU2B_OVF:
{
	/* TODO */
	MODIFY_PC_AND_STACK(2, 0);
}
break;

case COP_PREFIX_IU2UB_OVF:
{
	/* TODO */
	MODIFY_PC_AND_STACK(2, 0);
}
break;

case COP_PREFIX_I2S_OVF:
{
	/* TODO */
	MODIFY_PC_AND_STACK(2, 0);
}
break;

case COP_PREFIX_I2US_OVF:
{
	/* TODO */
	MODIFY_PC_AND_STACK(2, 0);
}
break;

case COP_PREFIX_IU2S_OVF:
{
	/* TODO */
	MODIFY_PC_AND_STACK(2, 0);
}
break;

case COP_PREFIX_IU2US_OVF:
{
	/* TODO */
	MODIFY_PC_AND_STACK(2, 0);
}
break;

case COP_PREFIX_I2IU_OVF:
{
	/* TODO */
	MODIFY_PC_AND_STACK(2, 0);
}
break;

case COP_PREFIX_IU2I_OVF:
{
	/* TODO */
	MODIFY_PC_AND_STACK(2, 0);
}
break;

case COP_PREFIX_I2UL_OVF:
{
	/* TODO */
	MODIFY_PC_AND_STACK(2, 0);
}
break;

case COP_PREFIX_L2I_OVF:
{
	/* TODO */
	MODIFY_PC_AND_STACK(2, 0);
}
break;

case COP_PREFIX_L2UI_OVF:
{
	/* TODO */
	MODIFY_PC_AND_STACK(2, 0);
}
break;

case COP_PREFIX_LU2I_OVF:
{
	/* TODO */
	MODIFY_PC_AND_STACK(2, 0);
}
break;

case COP_PREFIX_LU2IU_OVF:
{
	/* TODO */
	MODIFY_PC_AND_STACK(2, 0);
}
break;

case COP_PREFIX_L2UL_OVF:
{
	/* TODO */
	MODIFY_PC_AND_STACK(2, 0);
}
break;

case COP_PREFIX_LU2L_OVF:
{
	/* TODO */
	MODIFY_PC_AND_STACK(2, 0);
}
break;

case COP_PREFIX_F2I_OVF:
{
	/* TODO */
	MODIFY_PC_AND_STACK(2, 0);
}
break;

case COP_PREFIX_F2IU_OVF:
{
	/* TODO */
	MODIFY_PC_AND_STACK(2, 0);
}
break;

case COP_PREFIX_F2L_OVF:
{
	/* TODO */
	MODIFY_PC_AND_STACK(2, 0);
}
break;

case COP_PREFIX_F2LU_OVF:
{
	/* TODO */
	MODIFY_PC_AND_STACK(2, 0);
}
break;

#endif /* IL_CVM_PREFIX */
