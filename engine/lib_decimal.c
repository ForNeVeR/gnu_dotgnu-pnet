/*
 * decimal.c - Internalcall methods for "System.Decimal".
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
#include "il_decimal.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Rounding mode to use for all operations below.
 */
#define	DECIMAL_ROUND_MODE		IL_DECIMAL_ROUND_HALF_EVEN

/*
 * Throw a decimal overflow exception.
 */
static void ThrowDecimalOverflow(ILExecThread *thread)
{
	ILExecThreadThrowSystem(thread, "System.OverflowException",
							"Overflow_Decimal");
}

/*
 * Throw a decimal division by zero exception.
 */
static void ThrowDecimalDivZero(ILExecThread *thread)
{
	ILExecThreadThrowSystem(thread, "System.DivideByZeroException",
							"Decimal_DivZero");
}

/*
 * public Decimal(float value);
 */
static void System_Decimal_ctor_1(ILExecThread *thread,
								  ILDecimal *_this,
								  ILFloat value)
{
	if(!ILDecimalFromFloat(_this, value))
	{
		ThrowDecimalOverflow(thread);
	}
}

/*
 * public Decimal(double value);
 */
static void System_Decimal_ctor_2(ILExecThread *thread,
								  ILDecimal *_this,
								  ILDouble value)
{
	if(!ILDecimalFromDouble(_this, value))
	{
		ThrowDecimalOverflow(thread);
	}
}

/*
 * public static float ToSingle(decimal value);
 */
static ILFloat System_Decimal_ToSingle(ILExecThread *thread,
									   ILDecimal *value)
{
	return ILDecimalToFloat(value);
}

/*
 * public static double ToDouble(decimal value);
 */
static ILDouble System_Decimal_ToDouble(ILExecThread *thread,
									    ILDecimal *value)
{
	return ILDecimalToDouble(value);
}

/*
 * public static decimal Add(decimal x, decimal y);
 */
static void System_Decimal_Add(ILExecThread *thread,
							   ILDecimal *result,
							   ILDecimal *valuea,
							   ILDecimal *valueb)
{
	if(!ILDecimalAdd(result, valuea, valueb, DECIMAL_ROUND_MODE))
	{
		ThrowDecimalOverflow(thread);
	}
}

/*
 * public static int Compare(decimal x, decimal y);
 */
static ILInt32 System_Decimal_Compare(ILExecThread *thread,
							          ILDecimal *valuea,
							          ILDecimal *valueb)
{
	return ILDecimalCmp(valuea, valueb);
}

/*
 * public static decimal Divide(decimal x, decimal y);
 */
static void System_Decimal_Divide(ILExecThread *thread,
							      ILDecimal *result,
							      ILDecimal *valuea,
							      ILDecimal *valueb)
{
	int divResult;
	divResult = ILDecimalDiv(result, valuea, valueb, DECIMAL_ROUND_MODE);
	if(!divResult)
	{
		ThrowDecimalDivZero(thread);
	}
	else if(divResult < 0)
	{
		ThrowDecimalOverflow(thread);
	}
}

/*
 * public static decimal Floor(decimal x);
 */
static void System_Decimal_Floor(ILExecThread *thread,
								 ILDecimal *result,
								 ILDecimal *value)
{
	ILDecimalFloor(result, value);
}

/*
 * public static decimal Remainder(decimal x, decimal y);
 */
static void System_Decimal_Remainder(ILExecThread *thread,
							         ILDecimal *result,
							         ILDecimal *valuea,
							         ILDecimal *valueb)
{
	int divResult;
	divResult = ILDecimalRem(result, valuea, valueb, DECIMAL_ROUND_MODE);
	if(!divResult)
	{
		ThrowDecimalDivZero(thread);
	}
	else if(divResult < 0)
	{
		ThrowDecimalOverflow(thread);
	}
}

/*
 * public static decimal Multiply(decimal x, decimal y);
 */
static void System_Decimal_Multiply(ILExecThread *thread,
							        ILDecimal *result,
							        ILDecimal *valuea,
							        ILDecimal *valueb)
{
	if(!ILDecimalMul(result, valuea, valueb, DECIMAL_ROUND_MODE))
	{
		ThrowDecimalOverflow(thread);
	}
}

/*
 * public static decimal Negate(decimal x);
 */
static void System_Decimal_Negate(ILExecThread *thread,
								  ILDecimal *result,
								  ILDecimal *value)
{
	ILDecimalNeg(result, value);
}

/*
 * public static decimal Round(decimal x, int decimals);
 */
static void System_Decimal_Round(ILExecThread *thread,
								 ILDecimal *result,
								 ILDecimal *value,
								 ILInt32 decimals)
{
	if(decimals < 0 || decimals > 28)
	{
		ILExecThreadThrowArgRange(thread, "decimals", "ArgRange_DecimalScale");
	}
	else if(!ILDecimalRound(result, value, decimals, DECIMAL_ROUND_MODE))
	{
		ThrowDecimalOverflow(thread);
	}
}

/*
 * public static decimal Subtract(decimal x, decimal y);
 */
static void System_Decimal_Subtract(ILExecThread *thread,
							        ILDecimal *result,
							        ILDecimal *valuea,
							        ILDecimal *valueb)
{
	if(!ILDecimalSub(result, valuea, valueb, DECIMAL_ROUND_MODE))
	{
		ThrowDecimalOverflow(thread);
	}
}

/*
 * public static decimal Truncate(decimal x);
 */
static void System_Decimal_Truncate(ILExecThread *thread,
								    ILDecimal *result,
								    ILDecimal *value)
{
	ILDecimalTruncate(result, value);
}

/*
 * Method table for the "System.Decimal" class.
 */
IL_METHOD_BEGIN(_ILSystemDecimalMethods)
	IL_CONSTRUCTOR(".ctor",	"(Tf)V",		System_Decimal_ctor_1, 0)
	IL_CONSTRUCTOR(".ctor",	"(Td)V",		System_Decimal_ctor_2, 0)
	IL_METHOD("ToSingle", "(vSystem.Decimal;)f",
					System_Decimal_ToSingle)
	IL_METHOD("ToDouble", "(vSystem.Decimal;)d",
					System_Decimal_ToDouble)
	IL_METHOD("Add", "(vSystem.Decimal;vSystem.Decimal;)vSystem.Decimal;",
					System_Decimal_Add)
	IL_METHOD("Compare", "(vSystem.Decimal;vSystem.Decimal;)i",
					System_Decimal_Compare)
	IL_METHOD("Divide", "(vSystem.Decimal;vSystem.Decimal;)vSystem.Decimal;",
					System_Decimal_Divide)
	IL_METHOD("Floor", "(vSystem.Decimal;)vSystem.Decimal;",
					System_Decimal_Floor)
	IL_METHOD("Remainder", "(vSystem.Decimal;vSystem.Decimal;)vSystem.Decimal;",
					System_Decimal_Remainder)
	IL_METHOD("Mod", "(vSystem.Decimal;vSystem.Decimal;)vSystem.Decimal;",
					System_Decimal_Remainder)	/* Obsolete name */
	IL_METHOD("Multiply", "(vSystem.Decimal;vSystem.Decimal;)vSystem.Decimal;",
					System_Decimal_Multiply)
	IL_METHOD("Negate", "(vSystem.Decimal;)vSystem.Decimal;",
					System_Decimal_Negate)
	IL_METHOD("Round", "(vSystem.Decimal;i)vSystem.Decimal;",
					System_Decimal_Round)
	IL_METHOD("Subtract", "(vSystem.Decimal;vSystem.Decimal;)vSystem.Decimal;",
					System_Decimal_Subtract)
	IL_METHOD("Truncate", "(vSystem.Decimal;)vSystem.Decimal;",
					System_Decimal_Truncate)
IL_METHOD_END

#ifdef	__cplusplus
};
#endif
