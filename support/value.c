/*
 * value.c - Primitive operations on values.
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

#include "il_values.h"
#ifdef HAVE_MATH_H
#include <math.h>
#endif
#ifdef IL_NATIVE_WIN32
#include <float.h>
#define isnan(value)	_isnan((value))
#define HAVE_ISNAN 1
#endif

#ifdef	__cplusplus
extern	"C" {
#endif

int ILValueConvBoolean(ILValue *value)
{
	if(IL_TYPE_IS_I4(value->valueType))
	{
		value->un.i4Value = (value->un.i4Value != 0);
	}
	else if(IL_TYPE_IS_I8(value->valueType))
	{
		value->un.i4Value = (value->un.i8Value != 0);
	}
	else if(IL_TYPE_IS_I(value->valueType))
	{
		value->un.i4Value = (value->un.iValue != 0);
	}
	else if(IL_TYPE_IS_F(value->valueType))
	{
		value->un.i4Value =
			(value->un.fValue != (ILNativeFloat)0.0);
	}
	else if(IL_TYPE_IS_PTR(value->valueType))
	{
		value->un.i4Value = (value->un.oValue != 0);
	}
	else
	{
		return ILVALUE_RESULT_BAD_TYPE;
	}
	value->valueType = IL_TYPE_BOOLEAN;
	return ILVALUE_RESULT_OK;
}

int ILValueConvI1(ILValue *value)
{
	if(IL_TYPE_IS_I4(value->valueType))
	{
		value->un.i4Value = (ILInt32)(ILInt8)(value->un.i4Value);
	}
	else if(IL_TYPE_IS_I8(value->valueType))
	{
		value->un.i4Value = (ILInt32)(ILInt8)(value->un.i8Value);
	}
	else if(IL_TYPE_IS_I(value->valueType))
	{
		value->un.i4Value = (ILInt32)(ILInt8)(value->un.iValue);
	}
	else if(IL_TYPE_IS_F(value->valueType))
	{
		value->un.i4Value = (ILInt32)(ILInt8)(ILInt32)(value->un.fValue);
	}
	else
	{
		return ILVALUE_RESULT_BAD_TYPE;
	}
	value->valueType = IL_TYPE_INT8;
	return ILVALUE_RESULT_OK;
}

int ILValueConvU1(ILValue *value)
{
	if(IL_TYPE_IS_I4(value->valueType))
	{
		value->un.i4Value = (ILInt32)(ILUInt8)(value->un.i4Value);
	}
	else if(IL_TYPE_IS_I8(value->valueType))
	{
		value->un.i4Value = (ILInt32)(ILUInt8)(value->un.i8Value);
	}
	else if(IL_TYPE_IS_I(value->valueType))
	{
		value->un.i4Value = (ILInt32)(ILUInt8)(value->un.iValue);
	}
	else if(IL_TYPE_IS_F(value->valueType))
	{
		value->un.i4Value = (ILInt32)(ILUInt8)(ILUInt32)(value->un.fValue);
	}
	else
	{
		return ILVALUE_RESULT_BAD_TYPE;
	}
	value->valueType = IL_TYPE_UINT8;
	return ILVALUE_RESULT_OK;
}

int ILValueConvI2(ILValue *value)
{
	if(IL_TYPE_IS_I4(value->valueType))
	{
		value->un.i4Value = (ILInt32)(ILInt16)(value->un.i4Value);
	}
	else if(IL_TYPE_IS_I8(value->valueType))
	{
		value->un.i4Value = (ILInt32)(ILInt16)(value->un.i8Value);
	}
	else if(IL_TYPE_IS_I(value->valueType))
	{
		value->un.i4Value = (ILInt32)(ILInt16)(value->un.iValue);
	}
	else if(IL_TYPE_IS_F(value->valueType))
	{
		value->un.i4Value = (ILInt32)(ILInt16)(ILInt32)(value->un.fValue);
	}
	else
	{
		return ILVALUE_RESULT_BAD_TYPE;
	}
	value->valueType = IL_TYPE_INT16;
	return ILVALUE_RESULT_OK;
}

int ILValueConvU2(ILValue *value)
{
	if(IL_TYPE_IS_I4(value->valueType))
	{
		value->un.i4Value = (ILInt32)(ILUInt16)(value->un.i4Value);
	}
	else if(IL_TYPE_IS_I8(value->valueType))
	{
		value->un.i4Value = (ILInt32)(ILUInt16)(value->un.i8Value);
	}
	else if(IL_TYPE_IS_I(value->valueType))
	{
		value->un.i4Value = (ILInt32)(ILUInt16)(value->un.iValue);
	}
	else if(IL_TYPE_IS_F(value->valueType))
	{
		value->un.i4Value = (ILInt32)(ILUInt16)(ILUInt32)(value->un.fValue);
	}
	else
	{
		return ILVALUE_RESULT_BAD_TYPE;
	}
	value->valueType = IL_TYPE_UINT16;
	return ILVALUE_RESULT_OK;
}

int ILValueConvI4(ILValue *value)
{
	if(IL_TYPE_IS_I8(value->valueType))
	{
		value->un.i4Value = (ILInt32)(value->un.i8Value);
	}
	else if(IL_TYPE_IS_I(value->valueType))
	{
		value->un.i4Value = (ILInt32)(value->un.iValue);
	}
	else if(IL_TYPE_IS_F(value->valueType))
	{
		value->un.i4Value = (ILInt32)(value->un.fValue);
	}
	else if(!IL_TYPE_IS_I4(value->valueType))
	{
		return ILVALUE_RESULT_BAD_TYPE;
	}
	value->valueType = IL_TYPE_INT32;
	return ILVALUE_RESULT_OK;
}

int ILValueConvU4(ILValue *value)
{
	if(IL_TYPE_IS_I8(value->valueType))
	{
		value->un.i4Value = (ILInt32)(ILUInt32)(value->un.i8Value);
	}
	else if(IL_TYPE_IS_I(value->valueType))
	{
		value->un.i4Value =
			(ILInt32)(ILUInt32)(ILNativeUInt)(value->un.iValue);
	}
	else if(IL_TYPE_IS_F(value->valueType))
	{
		value->un.i4Value = (ILInt32)(ILUInt32)(value->un.fValue);
	}
	else if(!IL_TYPE_IS_I4(value->valueType))
	{
		return ILVALUE_RESULT_BAD_TYPE;
	}
	value->valueType = IL_TYPE_UINT32;
	return ILVALUE_RESULT_OK;
}

int ILValueConvI8(ILValue *value)
{
	if(IL_TYPE_IS_I4(value->valueType))
	{
		value->un.i8Value = (ILInt64)(value->un.i4Value);
	}
	else if(IL_TYPE_IS_I(value->valueType))
	{
		value->un.i8Value = (ILInt64)(value->un.iValue);
	}
	else if(IL_TYPE_IS_F(value->valueType))
	{
		value->un.i8Value = (ILInt64)(value->un.fValue);
	}
	else if(!IL_TYPE_IS_I8(value->valueType))
	{
		return ILVALUE_RESULT_BAD_TYPE;
	}
	value->valueType = IL_TYPE_INT64;
	return ILVALUE_RESULT_OK;
}

int ILValueConvU8(ILValue *value)
{
	if(IL_TYPE_IS_I4(value->valueType))
	{
		value->un.i8Value = (ILInt64)(ILUInt64)(ILUInt32)(value->un.i4Value);
	}
	else if(IL_TYPE_IS_I(value->valueType))
	{
		value->un.i8Value =
			(ILInt64)(ILUInt64)(ILNativeUInt)(value->un.iValue);
	}
	else if(IL_TYPE_IS_F(value->valueType))
	{
		value->un.i4Value = (ILInt32)(ILUInt32)(value->un.fValue);
	}
	else if(!IL_TYPE_IS_I8(value->valueType))
	{
		return ILVALUE_RESULT_BAD_TYPE;
	}
	value->valueType = IL_TYPE_UINT64;
	return ILVALUE_RESULT_OK;
}

int ILValueConvI(ILValue *value)
{
	if(IL_TYPE_IS_I4(value->valueType))
	{
		value->un.iValue = (ILNativeInt)(value->un.i4Value);
	}
	else if(IL_TYPE_IS_I8(value->valueType))
	{
		value->un.iValue = (ILNativeInt)(value->un.i8Value);
	}
	else if(IL_TYPE_IS_F(value->valueType))
	{
		value->un.iValue = (ILNativeInt)(value->un.fValue);
	}
	else if(!IL_TYPE_IS_I(value->valueType))
	{
		return ILVALUE_RESULT_BAD_TYPE;
	}
	value->valueType = IL_TYPE_NATIVE_INT;
	return ILVALUE_RESULT_OK;
}

int ILValueConvU(ILValue *value)
{
	if(IL_TYPE_IS_I4(value->valueType))
	{
		value->un.iValue =
			(ILNativeInt)(ILNativeUInt)(ILUInt32)(value->un.i4Value);
	}
	else if(IL_TYPE_IS_I8(value->valueType))
	{
		value->un.iValue =
			(ILNativeInt)(ILNativeUInt)(ILUInt64)(value->un.i8Value);
	}
	else if(IL_TYPE_IS_F(value->valueType))
	{
		value->un.iValue = (ILNativeInt)(ILNativeUInt)(value->un.fValue);
	}
	else if(!IL_TYPE_IS_I(value->valueType))
	{
		return ILVALUE_RESULT_BAD_TYPE;
	}
	value->valueType = IL_TYPE_NATIVE_UINT;
	return ILVALUE_RESULT_OK;
}

int ILValueConvR4(ILValue *value)
{
	if(IL_TYPE_IS_I4(value->valueType))
	{
		value->un.fValue = (ILNativeFloat)(ILFloat)(value->un.i4Value);
	}
	else if(IL_TYPE_IS_I8(value->valueType))
	{
		value->un.fValue = (ILNativeFloat)(ILFloat)(value->un.i8Value);
	}
	else if(IL_TYPE_IS_I(value->valueType))
	{
		value->un.fValue = (ILNativeFloat)(ILFloat)(value->un.iValue);
	}
	else if(!IL_TYPE_IS_F(value->valueType))
	{
		return ILVALUE_RESULT_BAD_TYPE;
	}
	value->valueType = IL_TYPE_FLOAT32;
	return ILVALUE_RESULT_OK;
}

int ILValueConvR8(ILValue *value)
{
	if(IL_TYPE_IS_I4(value->valueType))
	{
		value->un.fValue = (ILNativeFloat)(ILDouble)(value->un.i4Value);
	}
	else if(IL_TYPE_IS_I8(value->valueType))
	{
		value->un.fValue = (ILNativeFloat)(ILDouble)(value->un.i8Value);
	}
	else if(IL_TYPE_IS_I(value->valueType))
	{
		value->un.fValue = (ILNativeFloat)(ILDouble)(value->un.iValue);
	}
	else if(!IL_TYPE_IS_F(value->valueType))
	{
		return ILVALUE_RESULT_BAD_TYPE;
	}
	value->valueType = IL_TYPE_FLOAT64;
	return ILVALUE_RESULT_OK;
}

int ILValueConvR(ILValue *value)
{
	if(IL_TYPE_IS_I4(value->valueType))
	{
		value->un.fValue = (ILNativeFloat)(value->un.i4Value);
	}
	else if(IL_TYPE_IS_I8(value->valueType))
	{
		value->un.fValue = (ILNativeFloat)(value->un.i8Value);
	}
	else if(IL_TYPE_IS_I(value->valueType))
	{
		value->un.fValue = (ILNativeFloat)(value->un.iValue);
	}
	else if(!IL_TYPE_IS_F(value->valueType))
	{
		return ILVALUE_RESULT_BAD_TYPE;
	}
	value->valueType = IL_TYPE_NATIVE_FLOAT;
	return ILVALUE_RESULT_OK;
}

int ILValueConvRUn(ILValue *value)
{
	if(IL_TYPE_IS_I4(value->valueType))
	{
		value->un.fValue = (ILNativeFloat)(ILUInt32)(value->un.i4Value);
	}
	else if(IL_TYPE_IS_I8(value->valueType))
	{
	#ifndef IL_NATIVE_WIN32
		value->un.fValue = (ILNativeFloat)(ILUInt64)(value->un.i8Value);
	#else
		/* Visual Studio cannot convert from unsigned __int64 to
		   double in one go, so we need to do it via signed __int64 */
		if(value->un.i8Value <= (ILUInt64)IL_MAX_INT64)
		{
			value->un.fValue = (ILNativeFloat)(value->un.i8Value);
		}
		else
		{
			value->un.fValue = ((ILNativeFloat)(value->un.i8Value +
											    IL_MIN_INT64)) +
							   (ILNativeFloat)(9223372036854775808.0);
		}
	#endif
	}
	else if(IL_TYPE_IS_I(value->valueType))
	{
		value->un.fValue = (ILNativeFloat)(ILNativeUInt)(value->un.iValue);
	}
	else if(!IL_TYPE_IS_F(value->valueType))
	{
		return ILVALUE_RESULT_BAD_TYPE;
	}
	value->valueType = IL_TYPE_NATIVE_FLOAT;
	return ILVALUE_RESULT_OK;
}

int ILValueConvOvfI1(ILValue *value)
{
	if(IL_TYPE_IS_I4(value->valueType))
	{
		if(value->un.i4Value < ((ILInt32)(-128)) ||
		   value->un.i4Value > ((ILInt32)127))
		{
			return ILVALUE_RESULT_OVERFLOW;
		}
		value->un.i4Value = (ILInt32)(ILInt8)(value->un.i4Value);
	}
	else if(IL_TYPE_IS_I8(value->valueType))
	{
		if(value->un.i8Value < ((ILInt64)(-128)) ||
		   value->un.i8Value > ((ILInt64)127))
		{
			return ILVALUE_RESULT_OVERFLOW;
		}
		value->un.i4Value = (ILInt32)(ILInt8)(value->un.i8Value);
	}
	else if(IL_TYPE_IS_I(value->valueType))
	{
		if(value->un.iValue < ((ILNativeInt)(-128)) ||
		   value->un.iValue > ((ILNativeInt)127))
		{
			return ILVALUE_RESULT_OVERFLOW;
		}
		value->un.i4Value = (ILInt32)(ILInt8)(value->un.iValue);
	}
	else if(IL_TYPE_IS_F(value->valueType))
	{
		if(value->un.fValue < ((ILNativeFloat)(-128)) ||
		   value->un.fValue > ((ILNativeFloat)127))
		{
			return ILVALUE_RESULT_OVERFLOW;
		}
		value->un.i4Value = (ILInt32)(ILInt8)(ILInt32)(value->un.fValue);
	}
	else
	{
		return ILVALUE_RESULT_BAD_TYPE;
	}
	value->valueType = IL_TYPE_INT8;
	return ILVALUE_RESULT_OK;
}

int ILValueConvOvfU1(ILValue *value)
{
	if(IL_TYPE_IS_I4(value->valueType))
	{
		if(value->un.i4Value < ((ILInt32)0) ||
		   value->un.i4Value > ((ILInt32)255))
		{
			return ILVALUE_RESULT_OVERFLOW;
		}
		value->un.i4Value = (ILInt32)(ILUInt8)(value->un.i4Value);
	}
	else if(IL_TYPE_IS_I8(value->valueType))
	{
		if(value->un.i8Value < ((ILInt64)0) ||
		   value->un.i8Value > ((ILInt64)255))
		{
			return ILVALUE_RESULT_OVERFLOW;
		}
		value->un.i4Value = (ILInt32)(ILUInt8)(value->un.i8Value);
	}
	else if(IL_TYPE_IS_I(value->valueType))
	{
		if(value->un.iValue < ((ILNativeInt)0) ||
		   value->un.iValue > ((ILNativeInt)255))
		{
			return ILVALUE_RESULT_OVERFLOW;
		}
		value->un.i4Value = (ILInt32)(ILUInt8)(value->un.iValue);
	}
	else if(IL_TYPE_IS_F(value->valueType))
	{
		if(value->un.fValue < ((ILNativeFloat)0) ||
		   value->un.fValue > ((ILNativeFloat)255))
		{
			return ILVALUE_RESULT_OVERFLOW;
		}
		value->un.i4Value = (ILInt32)(ILUInt8)(ILUInt32)(value->un.fValue);
	}
	else
	{
		return ILVALUE_RESULT_BAD_TYPE;
	}
	value->valueType = IL_TYPE_UINT8;
	return ILVALUE_RESULT_OK;
}

int ILValueConvOvfI2(ILValue *value)
{
	if(IL_TYPE_IS_I4(value->valueType))
	{
		if(value->un.i4Value < ((ILInt32)-32768) ||
		   value->un.i4Value > ((ILInt32)32767))
		{
			return ILVALUE_RESULT_OVERFLOW;
		}
		value->un.i4Value = (ILInt32)(ILInt16)(value->un.i4Value);
	}
	else if(IL_TYPE_IS_I8(value->valueType))
	{
		if(value->un.i8Value < ((ILInt64)-32768) ||
		   value->un.i8Value > ((ILInt64)32767))
		{
			return ILVALUE_RESULT_OVERFLOW;
		}
		value->un.i4Value = (ILInt32)(ILInt16)(value->un.i8Value);
	}
	else if(IL_TYPE_IS_I(value->valueType))
	{
		if(value->un.iValue < ((ILNativeInt)-32768) ||
		   value->un.iValue > ((ILNativeInt)32767))
		{
			return ILVALUE_RESULT_OVERFLOW;
		}
		value->un.i4Value = (ILInt32)(ILInt16)(value->un.iValue);
	}
	else if(IL_TYPE_IS_F(value->valueType))
	{
		if(value->un.fValue < ((ILNativeFloat)-32768) ||
		   value->un.fValue > ((ILNativeFloat)32767))
		{
			return ILVALUE_RESULT_OVERFLOW;
		}
		value->un.i4Value = (ILInt32)(ILInt16)(ILInt32)(value->un.fValue);
	}
	else
	{
		return ILVALUE_RESULT_BAD_TYPE;
	}
	value->valueType = IL_TYPE_INT16;
	return ILVALUE_RESULT_OK;
}

int ILValueConvOvfU2(ILValue *value)
{
	if(IL_TYPE_IS_I4(value->valueType))
	{
		if(value->un.i4Value < ((ILInt32)0) ||
		   value->un.i4Value > ((ILInt32)65535))
		{
			return ILVALUE_RESULT_OVERFLOW;
		}
		value->un.i4Value = (ILInt32)(ILUInt16)(value->un.i4Value);
	}
	else if(IL_TYPE_IS_I8(value->valueType))
	{
		if(value->un.i8Value < ((ILInt64)0) ||
		   value->un.i8Value > ((ILInt64)65535))
		{
			return ILVALUE_RESULT_OVERFLOW;
		}
		value->un.i4Value = (ILInt32)(ILUInt16)(value->un.i8Value);
	}
	else if(IL_TYPE_IS_I(value->valueType))
	{
		if(value->un.iValue < ((ILNativeInt)0) ||
		   value->un.iValue > ((ILNativeInt)65535))
		{
			return ILVALUE_RESULT_OVERFLOW;
		}
		value->un.i4Value = (ILInt32)(ILUInt16)(value->un.iValue);
	}
	else if(IL_TYPE_IS_F(value->valueType))
	{
		if(value->un.fValue < ((ILNativeFloat)0) ||
		   value->un.fValue > ((ILNativeFloat)65535))
		{
			return ILVALUE_RESULT_OVERFLOW;
		}
		value->un.i4Value = (ILInt32)(ILUInt16)(ILUInt32)(value->un.fValue);
	}
	else
	{
		return ILVALUE_RESULT_BAD_TYPE;
	}
	value->valueType = IL_TYPE_UINT16;
	return ILVALUE_RESULT_OK;
}

int ILValueConvOvfI4(ILValue *value)
{
	if(IL_TYPE_IS_I8(value->valueType))
	{
		if(value->un.i8Value < ((ILInt64)IL_MIN_INT32) ||
		   value->un.i8Value > ((ILInt64)IL_MAX_INT32))
		{
			return ILVALUE_RESULT_OVERFLOW;
		}
		value->un.i4Value = (ILInt32)(value->un.i8Value);
	}
	else if(IL_TYPE_IS_I(value->valueType))
	{
		if(value->un.iValue < ((ILNativeInt)IL_MIN_INT32) ||
		   value->un.iValue > ((ILNativeInt)IL_MAX_INT32))
		{
			return ILVALUE_RESULT_OVERFLOW;
		}
		value->un.i4Value = (ILInt32)(value->un.iValue);
	}
	else if(IL_TYPE_IS_F(value->valueType))
	{
		if(value->un.fValue < ((ILNativeFloat)IL_MIN_INT32) ||
		   value->un.fValue > ((ILNativeFloat)IL_MAX_INT32))
		{
			return ILVALUE_RESULT_OVERFLOW;
		}
		value->un.i4Value = (ILInt32)(value->un.fValue);
	}
	else if(!IL_TYPE_IS_I4(value->valueType))
	{
		return ILVALUE_RESULT_BAD_TYPE;
	}
	value->valueType = IL_TYPE_INT32;
	return ILVALUE_RESULT_OK;
}

int ILValueConvOvfU4(ILValue *value)
{
	if(IL_TYPE_IS_I4(value->valueType))
	{
		if(value->un.i4Value < (ILInt32)0)
		{
			return ILVALUE_RESULT_OVERFLOW;
		}
	}
	else if(IL_TYPE_IS_I8(value->valueType))
	{
		if(value->un.i8Value < ((ILInt64)0) ||
		   value->un.i8Value > ((ILInt64)IL_MAX_UINT32))
		{
			return ILVALUE_RESULT_OVERFLOW;
		}
		value->un.i4Value = (ILInt32)(ILUInt32)(value->un.i8Value);
	}
	else if(IL_TYPE_IS_I(value->valueType))
	{
	#ifdef IL_NATIVE_INT32
		if(value->un.iValue < ((ILNativeInt)0))
	#else
		if(value->un.iValue < ((ILNativeInt)0) ||
		   value->un.iValue > ((ILNativeInt)IL_MAX_UINT32))
	#endif
		{
			return ILVALUE_RESULT_OVERFLOW;
		}
		value->un.i4Value =
			(ILInt32)(ILUInt32)(ILNativeUInt)(value->un.iValue);
	}
	else if(IL_TYPE_IS_F(value->valueType))
	{
		if(value->un.fValue < ((ILNativeFloat)0) ||
		   value->un.fValue > ((ILNativeFloat)IL_MAX_UINT32))
		{
			return ILVALUE_RESULT_OVERFLOW;
		}
		value->un.i4Value = (ILInt32)(ILUInt32)(value->un.fValue);
	}
	else
	{
		return ILVALUE_RESULT_BAD_TYPE;
	}
	value->valueType = IL_TYPE_UINT32;
	return ILVALUE_RESULT_OK;
}

int ILValueConvOvfI8(ILValue *value)
{
	if(IL_TYPE_IS_I4(value->valueType))
	{
		value->un.i8Value = (ILInt64)(value->un.i4Value);
	}
	else if(IL_TYPE_IS_I(value->valueType))
	{
		value->un.i8Value = (ILInt64)(value->un.iValue);
	}
	else if(IL_TYPE_IS_F(value->valueType))
	{
		if(value->un.fValue < ((ILNativeFloat)IL_MIN_INT64) ||
		   value->un.fValue > ((ILNativeFloat)IL_MAX_INT64))
		{
			return ILVALUE_RESULT_OVERFLOW;
		}
		value->un.i8Value = (ILInt64)(value->un.fValue);
	}
	else if(!IL_TYPE_IS_I8(value->valueType))
	{
		return ILVALUE_RESULT_BAD_TYPE;
	}
	value->valueType = IL_TYPE_INT64;
	return ILVALUE_RESULT_OK;
}

int ILValueConvOvfU8(ILValue *value)
{
	if(IL_TYPE_IS_I4(value->valueType))
	{
		if(value->un.i4Value < ((ILInt32)0))
		{
			return ILVALUE_RESULT_OVERFLOW;
		}
		value->un.i8Value = (ILInt64)(ILUInt64)(ILUInt32)(value->un.i4Value);
	}
	else if(IL_TYPE_IS_I8(value->valueType))
	{
		if(value->un.i8Value < ((ILInt32)0))
		{
			return ILVALUE_RESULT_OVERFLOW;
		}
	}
	else if(IL_TYPE_IS_I(value->valueType))
	{
		if(value->un.iValue < ((ILNativeInt)0))
		{
			return ILVALUE_RESULT_OVERFLOW;
		}
		value->un.i8Value =
			(ILInt64)(ILUInt64)(ILNativeUInt)(value->un.iValue);
	}
	else if(IL_TYPE_IS_F(value->valueType))
	{
		if(value->un.fValue < ((ILNativeFloat)0) ||
		   value->un.fValue > ((ILNativeFloat)IL_MAX_UINT64))
		{
			return ILVALUE_RESULT_OVERFLOW;
		}
		value->un.i4Value = (ILInt32)(ILUInt32)(value->un.fValue);
	}
	else
	{
		return ILVALUE_RESULT_BAD_TYPE;
	}
	value->valueType = IL_TYPE_UINT64;
	return ILVALUE_RESULT_OK;
}

int ILValueConvOvfI(ILValue *value)
{
	if(IL_TYPE_IS_I4(value->valueType))
	{
		value->un.iValue = (ILNativeInt)(value->un.i4Value);
	}
	else if(IL_TYPE_IS_I8(value->valueType))
	{
	#ifdef IL_NATIVE_INT32
		if(value->un.i8Value < ((ILInt64)IL_MIN_INT32) ||
		   value->un.i8Value > ((ILInt64)IL_MAX_INT32))
		{
			return ILVALUE_RESULT_OVERFLOW;
		}
	#endif
		value->un.iValue = (ILNativeInt)(value->un.i8Value);
	}
	else if(IL_TYPE_IS_F(value->valueType))
	{
		if(value->un.fValue < ((ILNativeFloat)IL_MIN_NATIVE_INT) ||
		   value->un.fValue > ((ILNativeFloat)IL_MAX_NATIVE_INT))
		{
			return ILVALUE_RESULT_OVERFLOW;
		}
		value->un.iValue = (ILNativeInt)(value->un.fValue);
	}
	else if(!IL_TYPE_IS_I(value->valueType))
	{
		return ILVALUE_RESULT_BAD_TYPE;
	}
	value->valueType = IL_TYPE_NATIVE_INT;
	return ILVALUE_RESULT_OK;
}

int ILValueConvOvfU(ILValue *value)
{
	if(IL_TYPE_IS_I4(value->valueType))
	{
		if(value->un.i4Value < (ILInt32)0)
		{
			return ILVALUE_RESULT_OVERFLOW;
		}
		value->un.iValue =
			(ILNativeInt)(ILNativeUInt)(ILUInt32)(value->un.i4Value);
	}
	else if(IL_TYPE_IS_I8(value->valueType))
	{
	#ifdef IL_NATIVE_INT32
		if(value->un.i8Value < ((ILInt64)0) ||
		   value->un.i8Value > ((ILInt64)IL_MAX_UINT32))
	#else
		if(value->un.i8Value < ((ILInt64)0))
	#endif
		{
			return ILVALUE_RESULT_OVERFLOW;
		}
		value->un.iValue =
			(ILNativeInt)(ILNativeUInt)(ILUInt64)(value->un.i8Value);
	}
	else if(IL_TYPE_IS_I(value->valueType))
	{
		if(value->un.iValue < (ILNativeInt)0)
		{
			return ILVALUE_RESULT_OVERFLOW;
		}
	}
	else if(IL_TYPE_IS_F(value->valueType))
	{
		if(value->un.fValue < ((ILNativeFloat)0) ||
		   value->un.fValue > ((ILNativeFloat)IL_MAX_NATIVE_UINT))
		{
			return ILVALUE_RESULT_OVERFLOW;
		}
		value->un.iValue = (ILNativeInt)(ILNativeUInt)(value->un.fValue);
	}
	else
	{
		return ILVALUE_RESULT_BAD_TYPE;
	}
	value->valueType = IL_TYPE_NATIVE_UINT;
	return ILVALUE_RESULT_OK;
}

int ILValueConvOvfI1Un(ILValue *value)
{
	if(IL_TYPE_IS_I4(value->valueType))
	{
		if(((ILUInt32)(value->un.i4Value)) > ((ILUInt32)127))
		{
			return ILVALUE_RESULT_OVERFLOW;
		}
		value->un.i4Value = (ILInt32)(ILInt8)(value->un.i4Value);
	}
	else if(IL_TYPE_IS_I8(value->valueType))
	{
		if(((ILUInt64)(value->un.i8Value)) > ((ILUInt64)127))
		{
			return ILVALUE_RESULT_OVERFLOW;
		}
		value->un.i4Value = (ILInt32)(ILInt8)(value->un.i8Value);
	}
	else if(IL_TYPE_IS_I(value->valueType))
	{
		if(((ILNativeUInt)(value->un.iValue)) > ((ILNativeUInt)127))
		{
			return ILVALUE_RESULT_OVERFLOW;
		}
		value->un.i4Value = (ILInt32)(ILInt8)(value->un.iValue);
	}
	else if(IL_TYPE_IS_F(value->valueType))
	{
		if(value->un.fValue < ((ILNativeFloat)(-128)) ||
		   value->un.fValue > ((ILNativeFloat)127))
		{
			return ILVALUE_RESULT_OVERFLOW;
		}
		value->un.i4Value = (ILInt32)(ILInt8)(ILInt32)(value->un.fValue);
	}
	else
	{
		return ILVALUE_RESULT_BAD_TYPE;
	}
	value->valueType = IL_TYPE_INT8;
	return ILVALUE_RESULT_OK;
}

int ILValueConvOvfU1Un(ILValue *value)
{
	if(IL_TYPE_IS_I4(value->valueType))
	{
		if(((ILUInt32)(value->un.i4Value)) > ((ILUInt32)255))
		{
			return ILVALUE_RESULT_OVERFLOW;
		}
		value->un.i4Value = (ILInt32)(ILUInt8)(value->un.i4Value);
	}
	else if(IL_TYPE_IS_I8(value->valueType))
	{
		if(((ILUInt64)(value->un.i8Value)) > ((ILUInt64)255))
		{
			return ILVALUE_RESULT_OVERFLOW;
		}
		value->un.i4Value = (ILInt32)(ILUInt8)(value->un.i8Value);
	}
	else if(IL_TYPE_IS_I(value->valueType))
	{
		if(((ILNativeUInt)(value->un.iValue)) > ((ILNativeUInt)255))
		{
			return ILVALUE_RESULT_OVERFLOW;
		}
		value->un.i4Value = (ILInt32)(ILUInt8)(value->un.iValue);
	}
	else if(IL_TYPE_IS_F(value->valueType))
	{
		if(value->un.fValue < ((ILNativeFloat)0) ||
		   value->un.fValue > ((ILNativeFloat)255))
		{
			return ILVALUE_RESULT_OVERFLOW;
		}
		value->un.i4Value = (ILInt32)(ILUInt8)(ILUInt32)(value->un.fValue);
	}
	else
	{
		return ILVALUE_RESULT_BAD_TYPE;
	}
	value->valueType = IL_TYPE_UINT8;
	return ILVALUE_RESULT_OK;
}

int ILValueConvOvfI2Un(ILValue *value)
{
	if(IL_TYPE_IS_I4(value->valueType))
	{
		if(((ILUInt32)(value->un.i4Value)) > ((ILUInt32)32767))
		{
			return ILVALUE_RESULT_OVERFLOW;
		}
		value->un.i4Value = (ILInt32)(ILInt16)(value->un.i4Value);
	}
	else if(IL_TYPE_IS_I8(value->valueType))
	{
		if(((ILUInt64)(value->un.i8Value)) > ((ILUInt64)32767))
		{
			return ILVALUE_RESULT_OVERFLOW;
		}
		value->un.i4Value = (ILInt32)(ILInt16)(value->un.i8Value);
	}
	else if(IL_TYPE_IS_I(value->valueType))
	{
		if(((ILNativeUInt)(value->un.iValue)) > ((ILNativeUInt)32767))
		{
			return ILVALUE_RESULT_OVERFLOW;
		}
		value->un.i4Value = (ILInt32)(ILInt16)(value->un.iValue);
	}
	else if(IL_TYPE_IS_F(value->valueType))
	{
		if(value->un.fValue < ((ILNativeFloat)-32768) ||
		   value->un.fValue > ((ILNativeFloat)32767))
		{
			return ILVALUE_RESULT_OVERFLOW;
		}
		value->un.i4Value = (ILInt32)(ILInt16)(ILInt32)(value->un.fValue);
	}
	else
	{
		return ILVALUE_RESULT_BAD_TYPE;
	}
	value->valueType = IL_TYPE_INT16;
	return ILVALUE_RESULT_OK;
}

int ILValueConvOvfU2Un(ILValue *value)
{
	if(IL_TYPE_IS_I4(value->valueType))
	{
		if(((ILUInt32)(value->un.i4Value)) > ((ILUInt32)65535))
		{
			return ILVALUE_RESULT_OVERFLOW;
		}
		value->un.i4Value = (ILInt32)(ILUInt16)(value->un.i4Value);
	}
	else if(IL_TYPE_IS_I8(value->valueType))
	{
		if(((ILUInt64)(value->un.i8Value)) > ((ILUInt64)65535))
		{
			return ILVALUE_RESULT_OVERFLOW;
		}
		value->un.i4Value = (ILInt32)(ILUInt16)(value->un.i8Value);
	}
	else if(IL_TYPE_IS_I(value->valueType))
	{
		if(((ILNativeUInt)(value->un.iValue)) > ((ILNativeUInt)65535))
		{
			return ILVALUE_RESULT_OVERFLOW;
		}
		value->un.i4Value = (ILInt32)(ILUInt16)(value->un.iValue);
	}
	else if(IL_TYPE_IS_F(value->valueType))
	{
		if(value->un.fValue < ((ILNativeFloat)0) ||
		   value->un.fValue > ((ILNativeFloat)65535))
		{
			return ILVALUE_RESULT_OVERFLOW;
		}
		value->un.i4Value = (ILInt32)(ILUInt16)(ILUInt32)(value->un.fValue);
	}
	else
	{
		return ILVALUE_RESULT_BAD_TYPE;
	}
	value->valueType = IL_TYPE_UINT16;
	return ILVALUE_RESULT_OK;
}

int ILValueConvOvfI4Un(ILValue *value)
{
	if(IL_TYPE_IS_I4(value->valueType))
	{
		if(((ILUInt32)(value->un.i4Value)) > ((ILUInt32)IL_MAX_INT32))
		{
			return ILVALUE_RESULT_OVERFLOW;
		}
	}
	else if(IL_TYPE_IS_I8(value->valueType))
	{
		if(((ILUInt64)(value->un.i8Value)) > ((ILUInt64)IL_MAX_INT32))
		{
			return ILVALUE_RESULT_OVERFLOW;
		}
		value->un.i4Value = (ILInt32)(value->un.i8Value);
	}
	else if(IL_TYPE_IS_I(value->valueType))
	{
		if(((ILNativeUInt)(value->un.iValue)) > ((ILNativeUInt)IL_MAX_INT32))
		{
			return ILVALUE_RESULT_OVERFLOW;
		}
		value->un.i4Value = (ILInt32)(value->un.iValue);
	}
	else if(IL_TYPE_IS_F(value->valueType))
	{
		if(value->un.fValue < ((ILNativeFloat)IL_MIN_INT32) ||
		   value->un.fValue > ((ILNativeFloat)IL_MAX_INT32))
		{
			return ILVALUE_RESULT_OVERFLOW;
		}
		value->un.i4Value = (ILInt32)(value->un.fValue);
	}
	else
	{
		return ILVALUE_RESULT_BAD_TYPE;
	}
	value->valueType = IL_TYPE_INT32;
	return ILVALUE_RESULT_OK;
}

int ILValueConvOvfU4Un(ILValue *value)
{
	if(IL_TYPE_IS_I8(value->valueType))
	{
		if(((ILUInt64)(value->un.i8Value)) > ((ILUInt64)IL_MAX_UINT32))
		{
			return ILVALUE_RESULT_OVERFLOW;
		}
		value->un.i4Value = (ILInt32)(ILUInt32)(value->un.i8Value);
	}
	else if(IL_TYPE_IS_I(value->valueType))
	{
	#ifdef IL_NATIVE_INT64
		if(((ILNativeUInt)(value->un.iValue)) > ((ILNativeUInt)IL_MAX_UINT32))
		{
			return ILVALUE_RESULT_OVERFLOW;
		}
	#endif
		value->un.i4Value =
			(ILInt32)(ILUInt32)(ILNativeUInt)(value->un.iValue);
	}
	else if(IL_TYPE_IS_F(value->valueType))
	{
		if(value->un.fValue < ((ILNativeFloat)0) ||
		   value->un.fValue > ((ILNativeFloat)IL_MAX_UINT32))
		{
			return ILVALUE_RESULT_OVERFLOW;
		}
		value->un.i4Value = (ILInt32)(ILUInt32)(value->un.fValue);
	}
	else if(!IL_TYPE_IS_I4(value->valueType))
	{
		return ILVALUE_RESULT_BAD_TYPE;
	}
	value->valueType = IL_TYPE_UINT32;
	return ILVALUE_RESULT_OK;
}

int ILValueConvOvfI8Un(ILValue *value)
{
	if(IL_TYPE_IS_I4(value->valueType))
	{
		value->un.i8Value = (ILInt64)(ILUInt32)(value->un.i4Value);
	}
	else if(IL_TYPE_IS_I(value->valueType))
	{
	#ifdef IL_NATIVE_INT64
		if(((ILNativeUInt)(value->un.iValue)) > ((ILNativeUInt)IL_MAX_INT64))
		{
			return ILVALUE_RESULT_OVERFLOW;
		}
	#endif
		value->un.i8Value = (ILInt64)(ILNativeUInt)(value->un.iValue);
	}
	else if(IL_TYPE_IS_I8(value->valueType))
	{
		if(((ILUInt64)(value->un.i8Value)) > ((ILUInt64)IL_MAX_INT64))
		{
			return ILVALUE_RESULT_OVERFLOW;
		}
	}
	else if(IL_TYPE_IS_F(value->valueType))
	{
		if(value->un.fValue < ((ILNativeFloat)IL_MIN_INT64) ||
		   value->un.fValue > ((ILNativeFloat)IL_MAX_INT64))
		{
			return ILVALUE_RESULT_OVERFLOW;
		}
		value->un.i8Value = (ILInt64)(value->un.fValue);
	}
	else
	{
		return ILVALUE_RESULT_BAD_TYPE;
	}
	value->valueType = IL_TYPE_INT64;
	return ILVALUE_RESULT_OK;
}

int ILValueConvOvfU8Un(ILValue *value)
{
	if(IL_TYPE_IS_I4(value->valueType))
	{
		value->un.i8Value = (ILInt64)(ILUInt64)(ILUInt32)(value->un.i4Value);
	}
	else if(IL_TYPE_IS_I(value->valueType))
	{
		value->un.i8Value =
			(ILInt64)(ILUInt64)(ILNativeUInt)(value->un.iValue);
	}
	else if(IL_TYPE_IS_F(value->valueType))
	{
		if(value->un.fValue < ((ILNativeFloat)0) ||
		   value->un.fValue > ((ILNativeFloat)IL_MAX_UINT64))
		{
			return ILVALUE_RESULT_OVERFLOW;
		}
		value->un.i4Value = (ILInt32)(ILUInt32)(value->un.fValue);
	}
	else if(!IL_TYPE_IS_I8(value->valueType))
	{
		return ILVALUE_RESULT_BAD_TYPE;
	}
	value->valueType = IL_TYPE_UINT64;
	return ILVALUE_RESULT_OK;
}

int ILValueConvOvfIUn(ILValue *value)
{
	if(IL_TYPE_IS_I4(value->valueType))
	{
	#ifdef IL_NATIVE_INT32
		if(((ILNativeUInt)(value->un.iValue)) > ((ILNativeUInt)IL_MAX_INT32))
		{
			return ILVALUE_RESULT_OVERFLOW;
		}
	#endif
		value->un.iValue = (ILNativeInt)(ILUInt32)(value->un.i4Value);
	}
	else if(IL_TYPE_IS_I8(value->valueType))
	{
		if(((ILUInt64)(value->un.i8Value)) > ((ILUInt64)IL_MAX_NATIVE_INT))
		{
			return ILVALUE_RESULT_OVERFLOW;
		}
		value->un.iValue = (ILNativeInt)(value->un.i8Value);
	}
	else if(IL_TYPE_IS_I(value->valueType))
	{
		if(((ILNativeUInt)(value->un.iValue)) >
				((ILNativeUInt)IL_MAX_NATIVE_INT))
		{
			return ILVALUE_RESULT_OVERFLOW;
		}
	}
	else if(IL_TYPE_IS_F(value->valueType))
	{
		if(value->un.fValue < ((ILNativeFloat)IL_MIN_NATIVE_INT) ||
		   value->un.fValue > ((ILNativeFloat)IL_MAX_NATIVE_INT))
		{
			return ILVALUE_RESULT_OVERFLOW;
		}
		value->un.iValue = (ILNativeInt)(value->un.fValue);
	}
	else
	{
		return ILVALUE_RESULT_BAD_TYPE;
	}
	value->valueType = IL_TYPE_NATIVE_INT;
	return ILVALUE_RESULT_OK;
}

int ILValueConvOvfUUn(ILValue *value)
{
	if(IL_TYPE_IS_I4(value->valueType))
	{
		value->un.iValue =
			(ILNativeInt)(ILNativeUInt)(ILUInt32)(value->un.i4Value);
	}
	else if(IL_TYPE_IS_I8(value->valueType))
	{
	#ifdef IL_NATIVE_INT32
		if(((ILUInt64)(value->un.i8Value)) > ((ILInt64)IL_MAX_NATIVE_UINT))
		{
			return ILVALUE_RESULT_OVERFLOW;
		}
	#endif
		value->un.iValue =
			(ILNativeInt)(ILNativeUInt)(ILUInt64)(value->un.i8Value);
	}
	else if(IL_TYPE_IS_F(value->valueType))
	{
		if(value->un.fValue < ((ILNativeFloat)0) ||
		   value->un.fValue > ((ILNativeFloat)IL_MAX_NATIVE_UINT))
		{
			return ILVALUE_RESULT_OVERFLOW;
		}
		value->un.iValue = (ILNativeInt)(ILNativeUInt)(value->un.fValue);
	}
	else if(!IL_TYPE_IS_I(value->valueType))
	{
		return ILVALUE_RESULT_BAD_TYPE;
	}
	value->valueType = IL_TYPE_NATIVE_UINT;
	return ILVALUE_RESULT_OK;
}

/*
 * Coerce two values to a common type prior to a binary operation.
 */
static int CoerceBinary(ILValue *value1, ILValue *value2)
{
	if(IL_TYPE_IS_F(value1->valueType))
	{
		if(IL_TYPE_IS_F(value2->valueType))
		{
			return ILVALUE_RESULT_OK;
		}
		else
		{
			return ILValueConvR(value2);
		}
	}
	else if(IL_TYPE_IS_F(value2->valueType))
	{
		return ILValueConvR(value1);
	}
	else if(IL_TYPE_IS_I8(value1->valueType))
	{
		if(IL_TYPE_IS_I8(value2->valueType))
		{
			return ILVALUE_RESULT_OK;
		}
		else
		{
			return ILValueConvI8(value2);
		}
	}
	else if(IL_TYPE_IS_I8(value2->valueType))
	{
		return ILValueConvI8(value1);
	}
	else if(IL_TYPE_IS_I(value1->valueType))
	{
		if(IL_TYPE_IS_I(value2->valueType))
		{
			return ILVALUE_RESULT_OK;
		}
		else
		{
			return ILValueConvI(value2);
		}
	}
	else if(IL_TYPE_IS_I(value2->valueType))
	{
		return ILValueConvI(value1);
	}
	else if(IL_TYPE_IS_I4(value1->valueType))
	{
		if(IL_TYPE_IS_I4(value2->valueType))
		{
			return ILVALUE_RESULT_OK;
		}
		else
		{
			return ILValueConvI4(value2);
		}
	}
	else if(IL_TYPE_IS_I4(value2->valueType))
	{
		return ILValueConvI4(value1);
	}
	else
	{
		return ILVALUE_RESULT_BAD_TYPE;
	}
}

/*
 * Coerce two values to a common type prior to an unsigned binary operation.
 */
static int CoerceBinaryUn(ILValue *value1, ILValue *value2)
{
	if(IL_TYPE_IS_F(value1->valueType))
	{
		if(IL_TYPE_IS_F(value2->valueType))
		{
			return ILVALUE_RESULT_OK;
		}
		else
		{
			return ILValueConvRUn(value2);
		}
	}
	else if(IL_TYPE_IS_F(value2->valueType))
	{
		return ILValueConvRUn(value1);
	}
	else if(IL_TYPE_IS_I8(value1->valueType))
	{
		if(IL_TYPE_IS_I8(value2->valueType))
		{
			return ILVALUE_RESULT_OK;
		}
		else
		{
			return ILValueConvU8(value2);
		}
	}
	else if(IL_TYPE_IS_I8(value2->valueType))
	{
		return ILValueConvU8(value1);
	}
	else if(IL_TYPE_IS_I(value1->valueType))
	{
		if(IL_TYPE_IS_I(value2->valueType))
		{
			return ILVALUE_RESULT_OK;
		}
		else
		{
			return ILValueConvU(value2);
		}
	}
	else if(IL_TYPE_IS_I(value2->valueType))
	{
		return ILValueConvU(value1);
	}
	else if(IL_TYPE_IS_I4(value1->valueType))
	{
		if(IL_TYPE_IS_I4(value2->valueType))
		{
			return ILVALUE_RESULT_OK;
		}
		else
		{
			return ILValueConvU4(value2);
		}
	}
	else if(IL_TYPE_IS_I4(value2->valueType))
	{
		return ILValueConvU4(value1);
	}
	else
	{
		return ILVALUE_RESULT_BAD_TYPE;
	}
}

/*
 * Coerce two values to a common type prior to a binary operation,
 * and use overflow testing during the conversion.
 */
static int CoerceBinaryOvf(ILValue *value1, ILValue *value2)
{
	if(IL_TYPE_IS_F(value1->valueType))
	{
		if(IL_TYPE_IS_F(value2->valueType))
		{
			return ILVALUE_RESULT_OK;
		}
		else
		{
			return ILValueConvR(value2);
		}
	}
	else if(IL_TYPE_IS_F(value2->valueType))
	{
		return ILValueConvR(value1);
	}
	else if(IL_TYPE_IS_I8(value1->valueType))
	{
		if(IL_TYPE_IS_I8(value2->valueType))
		{
			return ILVALUE_RESULT_OK;
		}
		else
		{
			return ILValueConvOvfI8(value2);
		}
	}
	else if(IL_TYPE_IS_I8(value2->valueType))
	{
		return ILValueConvOvfI8(value1);
	}
	else if(IL_TYPE_IS_I(value1->valueType))
	{
		if(IL_TYPE_IS_I(value2->valueType))
		{
			return ILVALUE_RESULT_OK;
		}
		else
		{
			return ILValueConvOvfI(value2);
		}
	}
	else if(IL_TYPE_IS_I(value2->valueType))
	{
		return ILValueConvOvfI(value1);
	}
	else if(IL_TYPE_IS_I4(value1->valueType))
	{
		if(IL_TYPE_IS_I4(value2->valueType))
		{
			return ILVALUE_RESULT_OK;
		}
		else
		{
			return ILValueConvOvfI4(value2);
		}
	}
	else if(IL_TYPE_IS_I4(value2->valueType))
	{
		return ILValueConvOvfI4(value1);
	}
	else
	{
		return ILVALUE_RESULT_BAD_TYPE;
	}
}

/*
 * Coerce two values to a common type prior to an unsigned binary operation,
 * and use overflow testing during the conversion.
 */
static int CoerceBinaryOvfUn(ILValue *value1, ILValue *value2)
{
	if(IL_TYPE_IS_F(value1->valueType))
	{
		if(IL_TYPE_IS_F(value2->valueType))
		{
			return ILVALUE_RESULT_OK;
		}
		else
		{
			return ILValueConvRUn(value2);
		}
	}
	else if(IL_TYPE_IS_F(value2->valueType))
	{
		return ILValueConvRUn(value1);
	}
	else if(IL_TYPE_IS_I8(value1->valueType))
	{
		if(IL_TYPE_IS_I8(value2->valueType))
		{
			return ILVALUE_RESULT_OK;
		}
		else
		{
			return ILValueConvOvfU8(value2);
		}
	}
	else if(IL_TYPE_IS_I8(value2->valueType))
	{
		return ILValueConvOvfU8(value1);
	}
	else if(IL_TYPE_IS_I(value1->valueType))
	{
		if(IL_TYPE_IS_I(value2->valueType))
		{
			return ILVALUE_RESULT_OK;
		}
		else
		{
			return ILValueConvOvfU(value2);
		}
	}
	else if(IL_TYPE_IS_I(value2->valueType))
	{
		return ILValueConvOvfU(value1);
	}
	else if(IL_TYPE_IS_I4(value1->valueType))
	{
		if(IL_TYPE_IS_I4(value2->valueType))
		{
			return ILVALUE_RESULT_OK;
		}
		else
		{
			return ILValueConvOvfU4(value2);
		}
	}
	else if(IL_TYPE_IS_I4(value2->valueType))
	{
		return ILValueConvOvfU4(value1);
	}
	else
	{
		return ILVALUE_RESULT_BAD_TYPE;
	}
}

int ILValueAdd(ILValue *value1, ILValue *value2)
{
	int result = CoerceBinary(value1, value2);
	if(result == ILVALUE_RESULT_OK)
	{
		if(IL_TYPE_IS_I4(value1->valueType))
		{
			value1->un.i4Value += value2->un.i4Value;
			value1->valueType = IL_TYPE_INT32;
		}
		else if(IL_TYPE_IS_I8(value1->valueType))
		{
			value1->un.i8Value += value2->un.i8Value;
			value1->valueType = IL_TYPE_INT64;
		}
		else if(IL_TYPE_IS_I(value1->valueType))
		{
			value1->un.iValue += value2->un.iValue;
			value1->valueType = IL_TYPE_NATIVE_INT;
		}
		else
		{
			value1->un.fValue += value2->un.fValue;
			value1->valueType = IL_TYPE_NATIVE_FLOAT;
		}
	}
	return result;
}

int ILValueAddUn(ILValue *value1, ILValue *value2)
{
	int result = CoerceBinaryUn(value1, value2);
	if(result == ILVALUE_RESULT_OK)
	{
		if(IL_TYPE_IS_I4(value1->valueType))
		{
			value1->un.i4Value += value2->un.i4Value;
			value1->valueType = IL_TYPE_UINT32;
		}
		else if(IL_TYPE_IS_I8(value1->valueType))
		{
			value1->un.i8Value += value2->un.i8Value;
			value1->valueType = IL_TYPE_UINT64;
		}
		else if(IL_TYPE_IS_I(value1->valueType))
		{
			value1->un.iValue += value2->un.iValue;
			value1->valueType = IL_TYPE_NATIVE_UINT;
		}
		else
		{
			value1->un.fValue += value2->un.fValue;
			value1->valueType = IL_TYPE_NATIVE_FLOAT;
		}
	}
	return result;
}

int ILValueAddOvf(ILValue *value1, ILValue *value2)
{
	int result = CoerceBinaryOvf(value1, value2);
	if(result == ILVALUE_RESULT_OK)
	{
		if(IL_TYPE_IS_I4(value1->valueType))
		{
			if(value1->un.i4Value >= 0 && value2->un.i4Value >= 0)
			{
				if((value1->un.i4Value += value2->un.i4Value)
						< value2->un.i4Value)
				{
					return ILVALUE_RESULT_OVERFLOW;
				}
			}
			else if(value1->un.i4Value >= 0 || value2->un.i4Value >= 0)
			{
				value1->un.i4Value += value2->un.i4Value;
			}
			else
			{
				if((value1->un.i4Value += value2->un.i4Value)
						> value2->un.i4Value)
				{
					return ILVALUE_RESULT_OVERFLOW;
				}
			}
			value1->valueType = IL_TYPE_INT32;
		}
		else if(IL_TYPE_IS_I8(value1->valueType))
		{
			if(value1->un.i8Value >= 0 && value2->un.i8Value >= 0)
			{
				if((value1->un.i8Value += value2->un.i8Value)
						< value2->un.i8Value)
				{
					return ILVALUE_RESULT_OVERFLOW;
				}
			}
			else if(value1->un.i8Value >= 0 || value2->un.i8Value >= 0)
			{
				value1->un.i8Value += value2->un.i8Value;
			}
			else
			{
				if((value1->un.i8Value += value2->un.i8Value)
						> value2->un.i8Value)
				{
					return ILVALUE_RESULT_OVERFLOW;
				}
			}
			value1->valueType = IL_TYPE_INT64;
		}
		else if(IL_TYPE_IS_I(value1->valueType))
		{
			if(value1->un.iValue >= 0 && value2->un.iValue >= 0)
			{
				if((value1->un.iValue += value2->un.iValue)
						< value2->un.iValue)
				{
					return ILVALUE_RESULT_OVERFLOW;
				}
			}
			else if(value1->un.iValue >= 0 || value2->un.iValue >= 0)
			{
				value1->un.iValue += value2->un.iValue;
			}
			else
			{
				if((value1->un.iValue += value2->un.iValue)
						> value2->un.iValue)
				{
					return ILVALUE_RESULT_OVERFLOW;
				}
			}
			value1->valueType = IL_TYPE_NATIVE_INT;
		}
		else
		{
			value1->un.fValue += value2->un.fValue;
			value1->valueType = IL_TYPE_NATIVE_FLOAT;
		}
	}
	return result;
}

int ILValueAddOvfUn(ILValue *value1, ILValue *value2)
{
	int result = CoerceBinaryOvfUn(value1, value2);
	if(result == ILVALUE_RESULT_OK)
	{
		if(IL_TYPE_IS_I4(value1->valueType))
		{
			if(((ILUInt32)(value1->un.i4Value += value2->un.i4Value))
					< ((ILUInt32)(value2->un.i4Value)))
			{
				return ILVALUE_RESULT_OVERFLOW;
			}
			value1->valueType = IL_TYPE_UINT32;
		}
		else if(IL_TYPE_IS_I8(value1->valueType))
		{
			if(((ILUInt64)(value1->un.i8Value += value2->un.i8Value))
					< ((ILUInt64)(value2->un.i8Value)))
			{
				return ILVALUE_RESULT_OVERFLOW;
			}
			value1->valueType = IL_TYPE_UINT64;
		}
		else if(IL_TYPE_IS_I(value1->valueType))
		{
			if(((ILNativeUInt)(value1->un.iValue += value2->un.iValue))
					< ((ILNativeUInt)(value2->un.iValue)))
			{
				return ILVALUE_RESULT_OVERFLOW;
			}
			value1->valueType = IL_TYPE_NATIVE_UINT;
		}
		else
		{
			value1->un.fValue += value2->un.fValue;
			value1->valueType = IL_TYPE_NATIVE_FLOAT;
		}
	}
	return result;
}

int ILValueSub(ILValue *value1, ILValue *value2)
{
	int result = CoerceBinary(value1, value2);
	if(result == ILVALUE_RESULT_OK)
	{
		if(IL_TYPE_IS_I4(value1->valueType))
		{
			value1->un.i4Value -= value2->un.i4Value;
			value1->valueType = IL_TYPE_INT32;
		}
		else if(IL_TYPE_IS_I8(value1->valueType))
		{
			value1->un.i8Value -= value2->un.i8Value;
			value1->valueType = IL_TYPE_INT64;
		}
		else if(IL_TYPE_IS_I(value1->valueType))
		{
			value1->un.iValue -= value2->un.iValue;
			value1->valueType = IL_TYPE_NATIVE_INT;
		}
		else
		{
			value1->un.fValue -= value2->un.fValue;
			value1->valueType = IL_TYPE_NATIVE_FLOAT;
		}
	}
	return result;
}

int ILValueSubUn(ILValue *value1, ILValue *value2)
{
	int result = CoerceBinaryUn(value1, value2);
	if(result == ILVALUE_RESULT_OK)
	{
		if(IL_TYPE_IS_I4(value1->valueType))
		{
			value1->un.i4Value -= value2->un.i4Value;
			value1->valueType = IL_TYPE_UINT32;
		}
		else if(IL_TYPE_IS_I8(value1->valueType))
		{
			value1->un.i8Value -= value2->un.i8Value;
			value1->valueType = IL_TYPE_UINT64;
		}
		else if(IL_TYPE_IS_I(value1->valueType))
		{
			value1->un.iValue -= value2->un.iValue;
			value1->valueType = IL_TYPE_NATIVE_UINT;
		}
		else
		{
			value1->un.fValue -= value2->un.fValue;
			value1->valueType = IL_TYPE_NATIVE_FLOAT;
		}
	}
	return result;
}

int ILValueSubOvf(ILValue *value1, ILValue *value2)
{
	int result = CoerceBinaryOvf(value1, value2);
	if(result == ILVALUE_RESULT_OK)
	{
		if(IL_TYPE_IS_I4(value1->valueType))
		{
			ILInt32 orig;
			if(value1->un.i4Value >= 0 && value2->un.i4Value >= 0)
			{
				orig = value1->un.i4Value;
				if((value1->un.i4Value -= value2->un.i4Value) > orig)
				{
					return ILVALUE_RESULT_OVERFLOW;
				}
			}
			else if(value1->un.i4Value >= 0)
			{
				orig = value1->un.i4Value;
				if((value1->un.i4Value -= value2->un.i4Value) < orig)
				{
					return ILVALUE_RESULT_OVERFLOW;
				}
			}
			else if(value2->un.i4Value >= 0)
			{
				orig = value1->un.i4Value;
				if((value1->un.i4Value -= value2->un.i4Value) > orig)
				{
					return ILVALUE_RESULT_OVERFLOW;
				}
			}
			else
			{
				value1->un.i4Value -= value2->un.i4Value;
			}
			value1->valueType = IL_TYPE_INT32;
		}
		else if(IL_TYPE_IS_I8(value1->valueType))
		{
			ILInt64 orig;
			if(value1->un.i8Value >= 0 && value2->un.i8Value >= 0)
			{
				orig = value1->un.i8Value;
				if((value1->un.i8Value -= value2->un.i8Value) > orig)
				{
					return ILVALUE_RESULT_OVERFLOW;
				}
			}
			else if(value1->un.i8Value >= 0)
			{
				orig = value1->un.i8Value;
				if((value1->un.i8Value -= value2->un.i8Value) < orig)
				{
					return ILVALUE_RESULT_OVERFLOW;
				}
			}
			else if(value2->un.i8Value >= 0)
			{
				orig = value1->un.i8Value;
				if((value1->un.i8Value -= value2->un.i8Value) > orig)
				{
					return ILVALUE_RESULT_OVERFLOW;
				}
			}
			else
			{
				value1->un.i8Value -= value2->un.i8Value;
			}
			value1->valueType = IL_TYPE_INT32;
		}
		else if(IL_TYPE_IS_I(value1->valueType))
		{
			ILNativeInt orig;
			if(value1->un.iValue >= 0 && value2->un.iValue >= 0)
			{
				orig = value1->un.iValue;
				if((value1->un.iValue -= value2->un.iValue) > orig)
				{
					return ILVALUE_RESULT_OVERFLOW;
				}
			}
			else if(value1->un.iValue >= 0)
			{
				orig = value1->un.iValue;
				if((value1->un.iValue -= value2->un.iValue) < orig)
				{
					return ILVALUE_RESULT_OVERFLOW;
				}
			}
			else if(value2->un.iValue >= 0)
			{
				orig = value1->un.iValue;
				if((value1->un.iValue -= value2->un.iValue) > orig)
				{
					return ILVALUE_RESULT_OVERFLOW;
				}
			}
			else
			{
				value1->un.iValue -= value2->un.iValue;
			}
			value1->valueType = IL_TYPE_INT32;
		}
		else
		{
			value1->un.fValue -= value2->un.fValue;
			value1->valueType = IL_TYPE_NATIVE_FLOAT;
		}
	}
	return result;
}

int ILValueSubOvfUn(ILValue *value1, ILValue *value2)
{
	int result = CoerceBinaryOvfUn(value1, value2);
	if(result == ILVALUE_RESULT_OK)
	{
		if(IL_TYPE_IS_I4(value1->valueType))
		{
			ILUInt32 orig = (ILUInt32)(value1->un.i4Value);
			if(((ILUInt32)(value1->un.i4Value -= value2->un.i4Value)) > orig)
			{
				return ILVALUE_RESULT_OVERFLOW;
			}
			value1->valueType = IL_TYPE_UINT32;
		}
		else if(IL_TYPE_IS_I8(value1->valueType))
		{
			ILUInt64 orig = (ILUInt64)(value1->un.i8Value);
			if(((ILUInt64)(value1->un.i8Value += value2->un.i8Value)) > orig)
			{
				return ILVALUE_RESULT_OVERFLOW;
			}
			value1->valueType = IL_TYPE_UINT64;
		}
		else if(IL_TYPE_IS_I(value1->valueType))
		{
			ILNativeUInt orig = (ILNativeUInt)(value1->un.iValue);
			if(((ILNativeUInt)(value1->un.iValue += value2->un.iValue)) > orig)
			{
				return ILVALUE_RESULT_OVERFLOW;
			}
			value1->valueType = IL_TYPE_NATIVE_UINT;
		}
		else
		{
			value1->un.fValue -= value2->un.fValue;
			value1->valueType = IL_TYPE_NATIVE_FLOAT;
		}
	}
	return result;
}

int ILValueMul(ILValue *value1, ILValue *value2)
{
	int result = CoerceBinary(value1, value2);
	if(result == ILVALUE_RESULT_OK)
	{
		if(IL_TYPE_IS_I4(value1->valueType))
		{
			value1->un.i4Value *= value2->un.i4Value;
			value1->valueType = IL_TYPE_INT32;
		}
		else if(IL_TYPE_IS_I8(value1->valueType))
		{
			value1->un.i8Value *= value2->un.i8Value;
			value1->valueType = IL_TYPE_INT64;
		}
		else if(IL_TYPE_IS_I(value1->valueType))
		{
			value1->un.iValue *= value2->un.iValue;
			value1->valueType = IL_TYPE_NATIVE_INT;
		}
		else
		{
			value1->un.fValue *= value2->un.fValue;
			value1->valueType = IL_TYPE_NATIVE_FLOAT;
		}
	}
	return result;
}

int ILValueMulUn(ILValue *value1, ILValue *value2)
{
	int result = CoerceBinaryUn(value1, value2);
	if(result == ILVALUE_RESULT_OK)
	{
		if(IL_TYPE_IS_I4(value1->valueType))
		{
			value1->un.i4Value =
				(ILInt32)(((ILUInt32)(value1->un.i4Value)) *
						  ((ILUInt32)(value2->un.i4Value)));
			value1->valueType = IL_TYPE_UINT32;
		}
		else if(IL_TYPE_IS_I8(value1->valueType))
		{
			value1->un.i8Value =
				(ILInt64)(((ILUInt64)(value1->un.i8Value)) *
						  ((ILUInt64)(value2->un.i8Value)));
			value1->valueType = IL_TYPE_UINT64;
		}
		else if(IL_TYPE_IS_I(value1->valueType))
		{
			value1->un.iValue =
				(ILNativeInt)(((ILNativeUInt)(value1->un.iValue)) *
						      ((ILNativeUInt)(value2->un.iValue)));
			value1->valueType = IL_TYPE_NATIVE_UINT;
		}
		else
		{
			value1->un.fValue *= value2->un.fValue;
			value1->valueType = IL_TYPE_NATIVE_FLOAT;
		}
	}
	return result;
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

int ILValueMulOvf(ILValue *value1, ILValue *value2)
{
	int result = CoerceBinaryOvf(value1, value2);
	if(result == ILVALUE_RESULT_OK)
	{
		if(IL_TYPE_IS_I4(value1->valueType))
		{
			ILInt64 product =
				((ILInt64)(value1->un.i4Value)) *
				((ILInt64)(value2->un.i4Value));
			if(product < ((ILInt64)IL_MIN_INT32) ||
			   product > ((ILInt64)IL_MAX_INT32))
			{
				return ILVALUE_RESULT_OVERFLOW;
			}
			value1->un.i4Value = (ILInt32)product;
			value1->valueType = IL_TYPE_INT32;
		}
		else if(IL_TYPE_IS_I8(value1->valueType))
		{
			ILInt64 product;
			if(!MulInt64(value1->un.i8Value, value2->un.i8Value, &product))
			{
				return ILVALUE_RESULT_OVERFLOW;
			}
			value1->un.i8Value = product;
			value1->valueType = IL_TYPE_INT64;
		}
		else if(IL_TYPE_IS_I(value1->valueType))
		{
		#ifdef IL_NATIVE_INT32
			ILInt64 product =
				((ILInt64)(value1->un.iValue)) *
				((ILInt64)(value2->un.iValue));
			if(product < ((ILInt64)IL_MIN_INT32) ||
			   product > ((ILInt64)IL_MAX_INT32))
			{
				return ILVALUE_RESULT_OVERFLOW;
			}
			value1->un.iValue = (ILInt32)product;
		#else
			ILInt64 product;
			if(!MulInt64(value1->un.iValue, value2->un.iValue, &product))
			{
				return ILVALUE_RESULT_OVERFLOW;
			}
			value1->un.iValue = product;
		#endif
			value1->valueType = IL_TYPE_NATIVE_INT;
		}
		else
		{
			value1->un.fValue *= value2->un.fValue;
			value1->valueType = IL_TYPE_NATIVE_FLOAT;
		}
	}
	return result;
}

int ILValueMulOvfUn(ILValue *value1, ILValue *value2)
{
	int result = CoerceBinaryOvfUn(value1, value2);
	if(result == ILVALUE_RESULT_OK)
	{
		if(IL_TYPE_IS_I4(value1->valueType))
		{
			ILUInt64 product =
				((ILUInt64)(ILUInt32)(value1->un.i4Value)) *
				((ILUInt64)(ILUInt32)(value2->un.i4Value));
			if(product > ((ILUInt64)IL_MAX_UINT32))
			{
				return ILVALUE_RESULT_OVERFLOW;
			}
			value1->un.i4Value = (ILInt32)(ILUInt32)product;
			value1->valueType = IL_TYPE_UINT32;
		}
		else if(IL_TYPE_IS_I8(value1->valueType))
		{
			ILUInt64 product;
			if(!MulUInt64((ILUInt64)(value1->un.i8Value),
						  (ILUInt64)(value2->un.i8Value), &product))
			{
				return ILVALUE_RESULT_OVERFLOW;
			}
			value1->un.i8Value = (ILInt64)product;
			value1->valueType = IL_TYPE_UINT64;
		}
		else if(IL_TYPE_IS_I(value1->valueType))
		{
		#ifdef IL_NATIVE_INT32
			ILUInt64 product =
				((ILUInt64)(ILUInt32)(value1->un.iValue)) *
				((ILUInt64)(ILUInt32)(value2->un.iValue));
			if(product > ((ILUInt64)IL_MAX_UINT32))
			{
				return ILVALUE_RESULT_OVERFLOW;
			}
			value1->un.iValue = (ILInt32)(ILUInt32)product;
		#else
			ILUInt64 product;
			if(!MulUInt64((ILUInt64)(value1->un.iValue),
						  (ILUInt64)(value2->un.iValue), &product))
			{
				return ILVALUE_RESULT_OVERFLOW;
			}
			value1->un.iValue = (ILInt64)product;
		#endif
			value1->valueType = IL_TYPE_NATIVE_UINT;
		}
		else
		{
			value1->un.fValue *= value2->un.fValue;
			value1->valueType = IL_TYPE_NATIVE_FLOAT;
		}
	}
	return result;
}

int ILValueDiv(ILValue *value1, ILValue *value2)
{
	int result = CoerceBinary(value1, value2);
	if(result == ILVALUE_RESULT_OK)
	{
		if(IL_TYPE_IS_I4(value1->valueType))
		{
			if(!(value2->un.i4Value))
			{
				return ILVALUE_RESULT_DIV_ZERO;
			}
			else if(value1->un.i4Value == IL_MIN_INT32 &&
			        value2->un.i4Value == (ILInt32)-1)
			{
				return ILVALUE_RESULT_ARITH;
			}
			value1->un.i4Value /= value2->un.i4Value;
			value1->valueType = IL_TYPE_INT32;
		}
		else if(IL_TYPE_IS_I8(value1->valueType))
		{
			if(!(value2->un.i8Value))
			{
				return ILVALUE_RESULT_DIV_ZERO;
			}
			else if(value1->un.i8Value == IL_MIN_INT64 &&
			        value2->un.i8Value == (ILInt64)-1)
			{
				return ILVALUE_RESULT_ARITH;
			}
			value1->un.i8Value /= value2->un.i8Value;
			value1->valueType = IL_TYPE_INT64;
		}
		else if(IL_TYPE_IS_I(value1->valueType))
		{
			if(!(value2->un.iValue))
			{
				return ILVALUE_RESULT_DIV_ZERO;
			}
			else if(value1->un.iValue == IL_MIN_NATIVE_INT &&
			        value2->un.iValue == (ILNativeInt)-1)
			{
				return ILVALUE_RESULT_ARITH;
			}
			value1->un.iValue /= value2->un.iValue;
			value1->valueType = IL_TYPE_NATIVE_INT;
		}
		else
		{
			value1->un.fValue /= value2->un.fValue;
			value1->valueType = IL_TYPE_NATIVE_FLOAT;
		}
	}
	return result;
}

int ILValueDivUn(ILValue *value1, ILValue *value2)
{
	int result = CoerceBinaryUn(value1, value2);
	if(result == ILVALUE_RESULT_OK)
	{
		if(IL_TYPE_IS_I4(value1->valueType))
		{
			if(!(value2->un.i4Value))
			{
				return ILVALUE_RESULT_DIV_ZERO;
			}
			value1->un.i4Value =
				(ILInt32)(((ILUInt32)(value1->un.i4Value)) /
						  ((ILUInt32)(value2->un.i4Value)));
			value1->valueType = IL_TYPE_UINT32;
		}
		else if(IL_TYPE_IS_I8(value1->valueType))
		{
			if(!(value2->un.i8Value))
			{
				return ILVALUE_RESULT_DIV_ZERO;
			}
			value1->un.i8Value =
				(ILInt64)(((ILUInt64)(value1->un.i8Value)) /
						  ((ILUInt64)(value2->un.i8Value)));
			value1->valueType = IL_TYPE_UINT64;
		}
		else if(IL_TYPE_IS_I(value1->valueType))
		{
			if(!(value2->un.iValue))
			{
				return ILVALUE_RESULT_DIV_ZERO;
			}
			value1->un.iValue =
				(ILNativeInt)(((ILNativeUInt)(value1->un.iValue)) /
						      ((ILNativeUInt)(value2->un.iValue)));
			value1->valueType = IL_TYPE_NATIVE_UINT;
		}
		else
		{
			value1->un.fValue /= value2->un.fValue;
			value1->valueType = IL_TYPE_NATIVE_FLOAT;
		}
	}
	return result;
}

int ILValueRem(ILValue *value1, ILValue *value2)
{
	int result = CoerceBinary(value1, value2);
	if(result == ILVALUE_RESULT_OK)
	{
		if(IL_TYPE_IS_I4(value1->valueType))
		{
			if(!(value2->un.i4Value))
			{
				return ILVALUE_RESULT_DIV_ZERO;
			}
			else if(value1->un.i4Value == IL_MIN_INT32 &&
			        value2->un.i4Value == (ILInt32)-1)
			{
				return ILVALUE_RESULT_ARITH;
			}
			value1->un.i4Value %= value2->un.i4Value;
			value1->valueType = IL_TYPE_INT32;
		}
		else if(IL_TYPE_IS_I8(value1->valueType))
		{
			if(!(value2->un.i8Value))
			{
				return ILVALUE_RESULT_DIV_ZERO;
			}
			else if(value1->un.i8Value == IL_MIN_INT64 &&
			        value2->un.i8Value == (ILInt64)-1)
			{
				return ILVALUE_RESULT_ARITH;
			}
			value1->un.i8Value %= value2->un.i8Value;
			value1->valueType = IL_TYPE_INT64;
		}
		else if(IL_TYPE_IS_I(value1->valueType))
		{
			if(!(value2->un.iValue))
			{
				return ILVALUE_RESULT_DIV_ZERO;
			}
			else if(value1->un.iValue == IL_MIN_NATIVE_INT &&
			        value2->un.iValue == (ILNativeInt)-1)
			{
				return ILVALUE_RESULT_ARITH;
			}
			value1->un.iValue %= value2->un.iValue;
			value1->valueType = IL_TYPE_NATIVE_INT;
		}
		else
		{
		#ifdef HAVE_FMOD
			value1->un.fValue =
				fmod(value1->un.fValue, value2->un.fValue);
		#else
			/* Set the result to NaN always if "fmod" doesn't exist */
			value1->un.fValue = (ILNativeFloat)(0.0 / 0.0);
		#endif
			value1->valueType = IL_TYPE_NATIVE_FLOAT;
		}
	}
	return result;
}

int ILValueRemUn(ILValue *value1, ILValue *value2)
{
	int result = CoerceBinaryUn(value1, value2);
	if(result == ILVALUE_RESULT_OK)
	{
		if(IL_TYPE_IS_I4(value1->valueType))
		{
			if(!(value2->un.i4Value))
			{
				return ILVALUE_RESULT_DIV_ZERO;
			}
			value1->un.i4Value =
				(ILInt32)(((ILUInt32)(value1->un.i4Value)) %
						  ((ILUInt32)(value2->un.i4Value)));
			value1->valueType = IL_TYPE_UINT32;
		}
		else if(IL_TYPE_IS_I8(value1->valueType))
		{
			if(!(value2->un.i8Value))
			{
				return ILVALUE_RESULT_DIV_ZERO;
			}
			value1->un.i8Value =
				(ILInt64)(((ILUInt64)(value1->un.i8Value)) %
						  ((ILUInt64)(value2->un.i8Value)));
			value1->valueType = IL_TYPE_UINT64;
		}
		else if(IL_TYPE_IS_I(value1->valueType))
		{
			if(!(value2->un.iValue))
			{
				return ILVALUE_RESULT_DIV_ZERO;
			}
			value1->un.iValue =
				(ILNativeInt)(((ILNativeUInt)(value1->un.iValue)) %
						      ((ILNativeUInt)(value2->un.iValue)));
			value1->valueType = IL_TYPE_NATIVE_UINT;
		}
		else
		{
		#ifdef HAVE_FMOD
			value1->un.fValue =
				fmod(value1->un.fValue, value2->un.fValue);
		#else
			/* Set the result to NaN always if "fmod" doesn't exist */
			value1->un.fValue = (ILNativeFloat)(0.0 / 0.0);
		#endif
			value1->valueType = IL_TYPE_NATIVE_FLOAT;
		}
	}
	return result;
}

int ILValueNeg(ILValue *value)
{
	if(IL_TYPE_IS_I4(value->valueType))
	{
		value->un.i4Value = -(value->un.i4Value);
		value->valueType = IL_TYPE_INT32;
	}
	else if(IL_TYPE_IS_I8(value->valueType))
	{
		value->un.i8Value = -(value->un.i8Value);
		value->valueType = IL_TYPE_INT64;
	}
	else if(IL_TYPE_IS_I(value->valueType))
	{
		value->un.iValue = -(value->un.iValue);
		value->valueType = IL_TYPE_NATIVE_INT;
	}
	else if(IL_TYPE_IS_F(value->valueType))
	{
		value->un.fValue = -(value->un.fValue);
		value->valueType = IL_TYPE_NATIVE_FLOAT;
	}
	else
	{
		return ILVALUE_RESULT_BAD_TYPE;
	}
	return ILVALUE_RESULT_OK;
}

/*
 * Coerce two values to a common type prior to a bitwise operation.
 */
static int CoerceBitwise(ILValue *value1, ILValue *value2)
{
	if(IL_TYPE_IS_I8(value1->valueType))
	{
		if(IL_TYPE_IS_I8(value2->valueType))
		{
			return ILVALUE_RESULT_OK;
		}
		else
		{
			return ILValueConvI8(value2);
		}
	}
	else if(IL_TYPE_IS_I8(value2->valueType))
	{
		return ILValueConvI8(value1);
	}
	else if(IL_TYPE_IS_I(value1->valueType))
	{
		if(IL_TYPE_IS_I(value2->valueType))
		{
			return ILVALUE_RESULT_OK;
		}
		else
		{
			return ILValueConvI(value2);
		}
	}
	else if(IL_TYPE_IS_I(value2->valueType))
	{
		return ILValueConvI(value1);
	}
	else if(IL_TYPE_IS_I4(value1->valueType))
	{
		if(IL_TYPE_IS_I4(value2->valueType))
		{
			return ILVALUE_RESULT_OK;
		}
		else
		{
			return ILValueConvI4(value2);
		}
	}
	else if(IL_TYPE_IS_I4(value2->valueType))
	{
		return ILValueConvI4(value1);
	}
	else
	{
		return ILVALUE_RESULT_BAD_TYPE;
	}
}

int ILValueAnd(ILValue *value1, ILValue *value2)
{
	int result = CoerceBitwise(value1, value2);
	if(result == ILVALUE_RESULT_OK)
	{
		if(IL_TYPE_IS_I4(value1->valueType))
		{
			value1->un.i4Value &= value2->un.i4Value;
			value1->valueType = IL_TYPE_INT32;
		}
		else if(IL_TYPE_IS_I8(value1->valueType))
		{
			value1->un.i8Value &= value2->un.i8Value;
			value1->valueType = IL_TYPE_INT64;
		}
		else
		{
			value1->un.iValue &= value2->un.iValue;
			value1->valueType = IL_TYPE_NATIVE_INT;
		}
	}
	return result;
}

int ILValueOr(ILValue *value1, ILValue *value2)
{
	int result = CoerceBitwise(value1, value2);
	if(result == ILVALUE_RESULT_OK)
	{
		if(IL_TYPE_IS_I4(value1->valueType))
		{
			value1->un.i4Value |= value2->un.i4Value;
			value1->valueType = IL_TYPE_INT32;
		}
		else if(IL_TYPE_IS_I8(value1->valueType))
		{
			value1->un.i8Value |= value2->un.i8Value;
			value1->valueType = IL_TYPE_INT64;
		}
		else
		{
			value1->un.iValue |= value2->un.iValue;
			value1->valueType = IL_TYPE_NATIVE_INT;
		}
	}
	return result;
}

int ILValueXor(ILValue *value1, ILValue *value2)
{
	int result = CoerceBitwise(value1, value2);
	if(result == ILVALUE_RESULT_OK)
	{
		if(IL_TYPE_IS_I4(value1->valueType))
		{
			value1->un.i4Value ^= value2->un.i4Value;
			value1->valueType = IL_TYPE_INT32;
		}
		else if(IL_TYPE_IS_I8(value1->valueType))
		{
			value1->un.i8Value ^= value2->un.i8Value;
			value1->valueType = IL_TYPE_INT64;
		}
		else
		{
			value1->un.iValue ^= value2->un.iValue;
			value1->valueType = IL_TYPE_NATIVE_INT;
		}
	}
	return result;
}

int ILValueNot(ILValue *value)
{
	if(IL_TYPE_IS_I4(value->valueType))
	{
		value->un.i4Value = ~(value->un.i4Value);
		value->valueType = IL_TYPE_INT32;
	}
	else if(IL_TYPE_IS_I8(value->valueType))
	{
		value->un.i8Value = ~(value->un.i8Value);
		value->valueType = IL_TYPE_INT64;
	}
	else if(IL_TYPE_IS_I(value->valueType))
	{
		value->un.iValue = ~(value->un.iValue);
		value->valueType = IL_TYPE_NATIVE_INT;
	}
	else
	{
		return ILVALUE_RESULT_BAD_TYPE;
	}
	return ILVALUE_RESULT_OK;
}

/*
 * Coerce two values to useful types prior to a shift operation.
 */
static int CoerceShift(ILValue *value1, ILValue *value2)
{
	if(!IL_TYPE_IS_I8(value1->valueType) &&
	   !IL_TYPE_IS_I(value1->valueType) &&
	   !IL_TYPE_IS_I4(value1->valueType))
	{
		return ILVALUE_RESULT_BAD_TYPE;
	}
	return ILValueConvU(value2);
}

int ILValueShl(ILValue *value1, ILValue *value2)
{
	int result = CoerceShift(value1, value2);
	if(result == ILVALUE_RESULT_OK)
	{
		if(IL_TYPE_IS_I4(value1->valueType))
		{
			value1->un.i4Value <<= (ILNativeUInt)(value2->un.iValue);
			value1->valueType = IL_TYPE_INT32;
		}
		else if(IL_TYPE_IS_I8(value1->valueType))
		{
			value1->un.i8Value <<= (ILNativeUInt)(value2->un.iValue);
			value1->valueType = IL_TYPE_INT64;
		}
		else
		{
			value1->un.iValue <<= (ILNativeUInt)(value2->un.iValue);
			value1->valueType = IL_TYPE_NATIVE_INT;
		}
	}
	return result;
}

int ILValueShr(ILValue *value1, ILValue *value2)
{
	int result = CoerceShift(value1, value2);
	if(result == ILVALUE_RESULT_OK)
	{
		if(IL_TYPE_IS_I4(value1->valueType))
		{
			value1->un.i4Value >>= (ILNativeUInt)(value2->un.iValue);
			value1->valueType = IL_TYPE_INT32;
		}
		else if(IL_TYPE_IS_I8(value1->valueType))
		{
			value1->un.i8Value >>= (ILNativeUInt)(value2->un.iValue);
			value1->valueType = IL_TYPE_INT64;
		}
		else
		{
			value1->un.iValue >>= (ILNativeUInt)(value2->un.iValue);
			value1->valueType = IL_TYPE_NATIVE_INT;
		}
	}
	return result;
}

int ILValueUShr(ILValue *value1, ILValue *value2)
{
	int result = CoerceShift(value1, value2);
	if(result == ILVALUE_RESULT_OK)
	{
		if(IL_TYPE_IS_I4(value1->valueType))
		{
			value1->un.i4Value =
				(ILInt32)(((ILUInt32)(value1->un.i4Value)) >>
						  ((ILNativeUInt)(value2->un.iValue)));
			value1->valueType = IL_TYPE_INT32;
		}
		else if(IL_TYPE_IS_I8(value1->valueType))
		{
			value1->un.i8Value =
				(ILInt64)(((ILUInt64)(value1->un.i8Value)) >>
						  ((ILNativeUInt)(value2->un.iValue)));
			value1->valueType = IL_TYPE_INT64;
		}
		else
		{
			value1->un.iValue =
				(ILNativeInt)(((ILNativeUInt)(value1->un.i8Value)) >>
						      ((ILNativeUInt)(value2->un.iValue)));
			value1->valueType = IL_TYPE_NATIVE_INT;
		}
	}
	return result;
}

int ILValueCompare(ILValue *value1, ILValue *value2, int lessop, int *cmp)
{
	int result = CoerceBinary(value1, value2);
	if(result == ILVALUE_RESULT_OK)
	{
		if(IL_TYPE_IS_I4(value1->valueType))
		{
			if(value1->un.i4Value < value2->un.i4Value)
				*cmp = -1;
			else if(value1->un.i4Value > value2->un.i4Value)
				*cmp = 1;
			else
				*cmp = 0;
		}
		else if(IL_TYPE_IS_I8(value1->valueType))
		{
			if(value1->un.i8Value < value2->un.i8Value)
				*cmp = -1;
			else if(value1->un.i8Value > value2->un.i8Value)
				*cmp = 1;
			else
				*cmp = 0;
		}
		else if(IL_TYPE_IS_I(value1->valueType))
		{
			if(value1->un.iValue < value2->un.iValue)
				*cmp = -1;
			else if(value1->un.iValue > value2->un.iValue)
				*cmp = 1;
			else
				*cmp = 0;
		}
		else
		{
		#ifdef HAVE_ISNAN
			if(isnan(value1->un.fValue) || isnan(value2->un.fValue))
				*cmp = (lessop ? -1 : 1);
			else
		#endif
			if(value1->un.fValue < value2->un.fValue)
				*cmp = -1;
			else if(value1->un.fValue > value2->un.fValue)
				*cmp = 1;
			else
				*cmp = 0;
		}
	}
	return result;
}

int ILValueCompareUn(ILValue *value1, ILValue *value2, int lessop, int *cmp)
{
	int result = CoerceBinaryUn(value1, value2);
	if(result == ILVALUE_RESULT_OK)
	{
		if(IL_TYPE_IS_I4(value1->valueType))
		{
			if(((ILUInt32)(value1->un.i4Value)) <
					((ILUInt32)(value2->un.i4Value)))
				*cmp = -1;
			else if(((ILUInt32)(value1->un.i4Value)) >
						((ILUInt32)(value2->un.i4Value)))
				*cmp = 1;
			else
				*cmp = 0;
		}
		else if(IL_TYPE_IS_I8(value1->valueType))
		{
			if(((ILUInt64)(value1->un.i8Value)) <
					((ILUInt64)(value2->un.i8Value)))
				*cmp = -1;
			else if(((ILUInt64)(value1->un.i8Value)) >
						((ILUInt64)(value2->un.i8Value)))
				*cmp = 1;
			else
				*cmp = 0;
		}
		else if(IL_TYPE_IS_I(value1->valueType))
		{
			if(((ILNativeUInt)(value1->un.iValue)) <
					((ILNativeUInt)(value2->un.iValue)))
				*cmp = -1;
			else if(((ILNativeUInt)(value1->un.iValue)) >
						((ILNativeUInt)(value2->un.iValue)))
				*cmp = 1;
			else
				*cmp = 0;
		}
		else
		{
		#ifdef HAVE_ISNAN
			if(isnan(value1->un.fValue) || isnan(value2->un.fValue))
				*cmp = (lessop ? -1 : 1);
			else
		#endif
			if(value1->un.fValue < value2->un.fValue)
				*cmp = -1;
			else if(value1->un.fValue > value2->un.fValue)
				*cmp = 1;
			else
				*cmp = 0;
		}
	}
	return result;
}

#ifdef	__cplusplus
};
#endif
