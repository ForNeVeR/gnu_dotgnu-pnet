/*
 * cg_utils.c - Handy utilities for the code generator.
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

#include "cg_nodes.h"

#ifdef	__cplusplus
extern	"C" {
#endif

void ILGenInt32(ILGenInfo *info, ILInt32 value)
{
	if(value == (ILInt32)(-1))
	{
		ILGenSimple(info, IL_OP_LDC_I4_M1);
	}
	else if(value >= (ILInt32)0 && value <= (ILInt32)8)
	{
		ILGenSimple(info, IL_OP_LDC_I4_0 + value);
	}
	else if(value >= (ILInt32)(-128) && value <= (ILInt32)127)
	{
		ILGenByteInsn(info, IL_OP_LDC_I4_S, (int)value);
	}
	else
	{
		ILGenWordInsn(info, IL_OP_LDC_I4, (ILUInt32)value);
	}
}

void ILGenUInt32(ILGenInfo *info, ILUInt32 value)
{
	if(value == (ILUInt32)(ILInt32)(-1))
	{
		ILGenSimple(info, IL_OP_LDC_I4_M1);
	}
	else if(value <= (ILUInt32)8)
	{
		ILGenSimple(info, IL_OP_LDC_I4_0 + value);
	}
	else if(value <= (ILUInt32)127 || value >= (ILUInt32)0xFFFFFF80)
	{
		ILGenByteInsn(info, IL_OP_LDC_I4_S, (int)(signed char)(value & 0xFF));
	}
	else
	{
		ILGenWordInsn(info, IL_OP_LDC_I4, value);
	}
}

void ILGenInt64(ILGenInfo *info, ILInt64 value)
{
	if(value >= -(((ILInt64)1) << 31) &&
	   value < (((ILInt64)1) << 31))
	{
		ILGenInt32(info, (ILInt32)value);
		ILGenSimple(info, IL_OP_CONV_I8);
	}
	else
	{
		ILGenDWordInsn(info, IL_OP_LDC_I8, (ILUInt64)value);
	}
}

void ILGenUInt64(ILGenInfo *info, ILUInt64 value)
{
	if(value < (((ILUInt64)1) << 32))
	{
		ILGenUInt32(info, (ILUInt32)value);
		ILGenSimple(info, IL_OP_CONV_U8);
	}
	else if(value >= -(((ILInt64)1) << 31))
	{
		ILGenInt32(info, (ILInt32)value);
		ILGenSimple(info, IL_OP_CONV_I8);
	}
	else
	{
		ILGenDWordInsn(info, IL_OP_LDC_I8, value);
	}
}

void ILGenIntNative(ILGenInfo *info, ILInt32 value)
{
	ILGenInt32(info, value);
	ILGenSimple(info, IL_OP_CONV_I);
}

void ILGenUIntNative(ILGenInfo *info, ILUInt32 value)
{
	ILGenUInt32(info, value);
	ILGenSimple(info, IL_OP_CONV_U);
}

void ILGenLoadLocal(ILGenInfo *info, unsigned num)
{
	if(num < 4)
	{
		ILGenSimple(info, IL_OP_LDLOC_0 + num);
	}
	else if(num < 256)
	{
		ILGenByteInsn(info, IL_OP_LDLOC_S, (int)num);
	}
	else
	{
		ILGenShortInsn(info, IL_OP_PREFIX + IL_PREFIX_OP_LDLOC, (ILUInt32)num);
	}
}

void ILGenStoreLocal(ILGenInfo *info, unsigned num)
{
	if(num < 4)
	{
		ILGenSimple(info, IL_OP_STLOC_0 + num);
	}
	else if(num < 256)
	{
		ILGenByteInsn(info, IL_OP_STLOC_S, (int)num);
	}
	else
	{
		ILGenShortInsn(info, IL_OP_PREFIX + IL_PREFIX_OP_STLOC, (ILUInt32)num);
	}
}

void ILGenLoadLocalAddr(ILGenInfo *info, unsigned num)
{
	if(num < 256)
	{
		ILGenByteInsn(info, IL_OP_LDLOCA_S, (int)num);
	}
	else
	{
		ILGenShortInsn(info, IL_OP_PREFIX + IL_PREFIX_OP_LDLOCA, (ILUInt32)num);
	}
}

void ILGenLoadArg(ILGenInfo *info, unsigned num)
{
	if(num < 4)
	{
		ILGenSimple(info, IL_OP_LDARG_0 + num);
	}
	else if(num < 256)
	{
		ILGenByteInsn(info, IL_OP_LDARG_S, (int)num);
	}
	else
	{
		ILGenShortInsn(info, IL_OP_PREFIX + IL_PREFIX_OP_LDARG, (ILUInt32)num);
	}
}

void ILGenStoreArg(ILGenInfo *info, unsigned num)
{
	if(num < 256)
	{
		ILGenByteInsn(info, IL_OP_STARG_S, (int)num);
	}
	else
	{
		ILGenShortInsn(info, IL_OP_PREFIX + IL_PREFIX_OP_STARG, (ILUInt32)num);
	}
}

void ILGenLoadArgAddr(ILGenInfo *info, unsigned num)
{
	if(num < 256)
	{
		ILGenByteInsn(info, IL_OP_LDARGA_S, (int)num);
	}
	else
	{
		ILGenShortInsn(info, IL_OP_PREFIX + IL_PREFIX_OP_LDARGA, (ILUInt32)num);
	}
}

void ILGenConst(ILGenInfo *info, ILEvalValue *value)
{
	switch(value->valueType)
	{
		case ILMachineType_Void:		break;

		case ILMachineType_Boolean:
		{
			ILGenSimple(info, (value->un.i4Value ? IL_OP_LDC_I4_0
												 : IL_OP_LDC_I4_1));
			ILGenAdjust(info, 1);
		}
		break;

		case ILMachineType_Int8:
		case ILMachineType_Int16:
		case ILMachineType_Int32:
		{
			ILGenInt32(info, value->un.i4Value);
			ILGenAdjust(info, 1);
		}
		break;

		case ILMachineType_UInt8:
		case ILMachineType_UInt16:
		case ILMachineType_UInt32:
		case ILMachineType_Char:
		{
			ILGenUInt32(info, (ILUInt32)(value->un.i4Value));
			ILGenAdjust(info, 1);
		}
		break;

		case ILMachineType_Int64:
		{
			ILGenInt64(info, value->un.i8Value);
			ILGenAdjust(info, 1);
		}
		break;

		case ILMachineType_UInt64:
		{
			ILGenUInt64(info, (ILUInt64)(value->un.i8Value));
			ILGenAdjust(info, 1);
		}
		break;

		case ILMachineType_NativeInt:
		{
			ILGenIntNative(info, (ILNativeInt)(value->un.i4Value));
			ILGenAdjust(info, 1);
		}
		break;

		case ILMachineType_NativeUInt:
		{
			ILGenUIntNative(info, (ILNativeUInt)(value->un.i4Value));
			ILGenAdjust(info, 1);
		}
		break;

		case ILMachineType_Float32:
		{
			ILGenLoadFloat32(info, value->un.r4Value);
			ILGenAdjust(info, 1);
		}
		break;

		case ILMachineType_Float64:
		case ILMachineType_NativeFloat:
		{
			if(value->un.r8Value ==
					(ILDouble)(ILFloat)(value->un.r8Value))
			{
				ILGenLoadFloat32(info, (ILFloat)(value->un.r8Value));
			}
			else
			{
				ILGenLoadFloat64(info, value->un.r8Value);
			}
			ILGenAdjust(info, 1);
		}
		break;

		default:
		{
			ILGenSimple(info, IL_OP_LDNULL);
			ILGenAdjust(info, 1);
		}
		break;
	}
}

#ifdef	__cplusplus
};
#endif
