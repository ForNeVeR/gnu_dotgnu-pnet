/*
 * c_typesize.c - Determine the sizes of standard C types.
 *
 * Copyright (C) 2002  Southern Storm Software, Pty Ltd.
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

#include <cscc/c/c_internal.h>
#include "il_align.h"

#ifdef	__cplusplus
extern	"C" {
#endif

ILUInt32 CTypeShortSize;
ILUInt32 CTypeShortAlign;
ILUInt32 CTypeIntSize;
ILUInt32 CTypeIntAlign;
ILUInt32 CTypeLongSize;
ILUInt32 CTypeLongAlign;
ILUInt32 CTypeLongLongSize;
ILUInt32 CTypeLongLongAlign;
ILUInt32 CTypeFloatSize;
ILUInt32 CTypeFloatAlign;
ILUInt32 CTypeDoubleSize;
ILUInt32 CTypeDoubleAlign;
ILUInt32 CTypeLongDoubleSize;
ILUInt32 CTypeLongDoubleAlign;
ILUInt32 CTypeNativeIntSize;
ILUInt32 CTypeNativeIntAlign;
ILUInt32 CTypePtrSize;
ILUInt32 CTypePtrAlign;
ILUInt32 CTypeAlignModifiers;
int CTypeNativeLayout;

_IL_ALIGN_CHECK_TYPE(short, short);

typedef union
{
	int	value;
	int bit : 1;

} BitFieldTest;

static void TypeSizeDetect(int isNative, int is32BitOnly)
{
	BitFieldTest test;

	if(isNative)
	{
		/* Detect the memory model from the machine that we are running on */
		CTypeShortSize = sizeof(short);
		CTypeShortAlign = _IL_ALIGN_FOR_TYPE(short);
		CTypeIntSize = sizeof(ILInt32);
		CTypeIntAlign = _IL_ALIGN_FOR_TYPE(int);
		CTypeLongSize = sizeof(ILInt32);
		CTypeLongAlign = _IL_ALIGN_FOR_TYPE(int);
		CTypeLongLongSize = sizeof(ILInt64);
		CTypeLongLongSize = _IL_ALIGN_FOR_TYPE(long);
		CTypeFloatSize = sizeof(ILFloat);
		CTypeFloatSize = _IL_ALIGN_FOR_TYPE(float);
		CTypeDoubleSize = sizeof(ILDouble);
		CTypeDoubleAlign = _IL_ALIGN_FOR_TYPE(double);
		CTypeLongDoubleSize = CTYPE_DYNAMIC;
		CTypeLongDoubleAlign = _IL_ALIGN_FOR_TYPE(long_double);
		CTypeNativeIntSize = sizeof(void *);
		CTypeNativeIntAlign = _IL_ALIGN_FOR_TYPE(void_p);
		CTypePtrSize = sizeof(void *);
		CTypePtrAlign = _IL_ALIGN_FOR_TYPE(void_p);
		CTypeNativeLayout = 1;

		/* Determine the modifier flags */
		CTypeAlignModifiers = 0;
		if(CTypeShortAlign == 1)
		{
			CTypeAlignModifiers |= C_ALIGNMOD_SHORT_1;
		}
		if(CTypeIntAlign == 1)
		{
			CTypeAlignModifiers |= C_ALIGNMOD_INT_1;
		}
		else if(CTypeIntAlign == 2)
		{
			CTypeAlignModifiers |= C_ALIGNMOD_INT_2;
		}
		if(CTypeLongLongAlign == 1)
		{
			CTypeAlignModifiers |= C_ALIGNMOD_LONG_1;
		}
		else if(CTypeLongLongAlign == 2)
		{
			CTypeAlignModifiers |= C_ALIGNMOD_LONG_2;
		}
		else if(CTypeLongLongAlign == 4)
		{
			CTypeAlignModifiers |= C_ALIGNMOD_LONG_4;
		}
		if(CTypeFloatAlign == 1)
		{
			CTypeAlignModifiers |= C_ALIGNMOD_FLOAT_1;
		}
		else if(CTypeFloatAlign == 2)
		{
			CTypeAlignModifiers |= C_ALIGNMOD_FLOAT_2;
		}
		if(CTypeDoubleAlign == 1)
		{
			CTypeAlignModifiers |= C_ALIGNMOD_DOUBLE_1;
		}
		else if(CTypeDoubleAlign == 2)
		{
			CTypeAlignModifiers |= C_ALIGNMOD_DOUBLE_2;
		}
		else if(CTypeDoubleAlign == 4)
		{
			CTypeAlignModifiers |= C_ALIGNMOD_DOUBLE_4;
		}
		if(CTypeLongDoubleAlign == 1)
		{
			CTypeAlignModifiers |= C_ALIGNMOD_LDOUBLE_1;
		}
		else if(CTypeLongDoubleAlign == 2)
		{
			CTypeAlignModifiers |= C_ALIGNMOD_LDOUBLE_2;
		}
		else if(CTypeLongDoubleAlign == 4)
		{
			CTypeAlignModifiers |= C_ALIGNMOD_LDOUBLE_4;
		}
		else if(CTypeLongDoubleAlign == 8)
		{
			CTypeAlignModifiers |= C_ALIGNMOD_LDOUBLE_8;
		}
		else if(CTypeLongDoubleAlign == 16)
		{
			CTypeAlignModifiers |= C_ALIGNMOD_LDOUBLE_16;
		}
		if(CTypePtrAlign == 1)
		{
			CTypeAlignModifiers |= C_ALIGNMOD_PTR_1;
		}
		else if(CTypePtrAlign == 2)
		{
			CTypeAlignModifiers |= C_ALIGNMOD_PTR_2;
		}
		else if(CTypePtrAlign == 4 && !is32BitOnly)
		{
			CTypeAlignModifiers |= C_ALIGNMOD_PTR_4;
		}
		test.value = 1;
		if(!(test.bit))
		{
			CTypeAlignModifiers |= C_ALIGNMOD_BITFLD_BIG;
		}
	}
	else if(is32BitOnly)
	{
		/* Use "Model 32" */
		CTypeShortSize = 2;
		CTypeShortAlign = 2;
		CTypeIntSize = 4;
		CTypeIntAlign = 4;
		CTypeLongSize = 4;
		CTypeLongAlign = 4;
		CTypeLongLongSize = 8;
		CTypeLongLongSize = 8;
		CTypeFloatSize = 4;
		CTypeFloatSize = 4;
		CTypeDoubleSize = 8;
		CTypeDoubleAlign = 8;
		CTypeLongDoubleSize = CTYPE_DYNAMIC;
		CTypeLongDoubleAlign = 16;
		CTypeNativeIntSize = 4;
		CTypeNativeIntAlign = 4;
		CTypePtrSize = 4;
		CTypePtrAlign = 4;
		CTypeAlignModifiers = 0;
		CTypeNativeLayout = 0;
	}
	else
	{
		/* Use "Model 64" */
		CTypeShortSize = 2;
		CTypeShortAlign = 2;
		CTypeIntSize = 4;
		CTypeIntAlign = 4;
		CTypeLongSize = 8;
		CTypeLongAlign = 8;
		CTypeLongLongSize = 8;
		CTypeLongLongSize = 8;
		CTypeFloatSize = 4;
		CTypeFloatSize = 4;
		CTypeDoubleSize = 8;
		CTypeDoubleAlign = 8;
		CTypeLongDoubleSize = CTYPE_DYNAMIC;
		CTypeLongDoubleAlign = 16;
		CTypeNativeIntSize = CTYPE_DYNAMIC;
		CTypeNativeIntAlign = 8;
		CTypePtrSize = 8;
		CTypePtrAlign = 8;
		CTypeAlignModifiers = 0;
		CTypeNativeLayout = 0;
	}
}

void CTypeSizeDetect(void)
{
	if(CCStringListContains(machine_flags, num_machine_flags, "native-types"))
	{
		TypeSizeDetect(1, (sizeof(void *) == 4));
	}
	else if(CCStringListContains(machine_flags, num_machine_flags,
								 "32bit-only"))
	{
		TypeSizeDetect(0, 1);
	}
	else
	{
		TypeSizeDetect(0, 0);
	}
}

static ILUInt32 nativeLevel = 0;

void CTypeSizePushNative(void)
{
	++nativeLevel;
	TypeSizeDetect(1, (sizeof(void *) == 4));
}

void CTypeSizePopNative(void)
{
	if(nativeLevel > 0)
	{
		--nativeLevel;
		if(nativeLevel != 0)
		{
			TypeSizeDetect(1, (sizeof(void *) == 4));
		}
		else
		{
			CTypeSizeDetect();
		}
	}
}

#ifdef	__cplusplus
};
#endif
