/*
 * lib_misc.c - Internalcall methods for misc "System" classes.
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

#include "engine.h"
#include "lib_defs.h"
#include "il_utils.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * public static Guid NewGuid();
 */
static void Guid_NewGuid(ILExecThread *thread, void *result)
{
	ILGUIDGenerate((unsigned char *)result);
}

/*
 * Method table for the "System.Guid" class.
 */
IL_METHOD_BEGIN(_ILSystemGuidMethods)
	IL_METHOD("NewGuid", "()vSystem.Guid;", Guid_NewGuid)
IL_METHOD_END

/*
 * private static bool GetLittleEndian();
 */
static ILBool BitConverter_GetLittleEndian(ILExecThread *thread)
{
#if defined(__i386) || defined(__i386__)
	return 1;
#else
	union
	{
		unsigned char bytes[4];
		ILUInt32      value;

	} convert;
	convert.value = (ILUInt32)0x01020304;
	return (convert.bytes[0] == (unsigned char)0x04);
#endif
}

/*
 * public static long DoubleToInt64Bits(double value);
 */
static ILInt64 BitConverter_DoubleToInt64Bits(ILExecThread *thread,
											  ILDouble value)
{
	union
	{
		ILDouble input;
		ILInt64  output;

	} convert;
	convert.input = value;
	return convert.output;
}

/*
 * public static double Int64BitsToDouble(long value);
 */
static ILDouble BitConverter_Int64BitsToDouble(ILExecThread *thread,
											   ILInt64 value)
{
	union
	{
		ILInt64  input;
		ILDouble output;

	} convert;
	convert.input = value;
	return convert.output;
}

/*
 * public static int FloatToInt32Bits(float value);
 */
static ILInt32 BitConverter_FloatToInt32Bits(ILExecThread *thread,
											 ILFloat value)
{
	union
	{
		ILFloat input;
		ILInt32 output;

	} convert;
	convert.input = value;
	return convert.output;
}

/*
 * public static float Int32BitsToFloat(int value);
 */
static ILDouble BitConverter_Int32BitsToFloat(ILExecThread *thread,
											  ILInt32 value)
{
	union
	{
		ILInt32 input;
		ILFloat output;

	} convert;
	convert.input = value;
	return convert.output;
}

/*
 * Method table for the "System.BitConverter" class.
 */
IL_METHOD_BEGIN(_ILSystemBitConverterMethods)
	IL_METHOD("GetLittleEndian",   "()Z",  BitConverter_GetLittleEndian)
	IL_METHOD("DoubleToInt64Bits", "(d)l", BitConverter_DoubleToInt64Bits)
	IL_METHOD("Int64BitsToDouble", "(l)d", BitConverter_Int64BitsToDouble)
	IL_METHOD("FloatToInt32Bits",  "(f)l", BitConverter_FloatToInt32Bits)
	IL_METHOD("Int32BitsToFloat",  "(l)f", BitConverter_Int32BitsToFloat)
IL_METHOD_END

#ifdef	__cplusplus
};
#endif
