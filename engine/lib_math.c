/*
 * lib_math.c - Internalcall methods for "System.Math", "Single", and "Double".
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

/*
 * Define the following macro to test compiling without "libm" support.
 */
/*#define	IL_NO_LIBM*/

/*
 * Turn off "libm" if requested.
 */
#ifdef 	IL_NO_LIBM
#undef	HAVE_FINITE
#undef	HAVE_ISNAN
#undef	HAVE_ISINF
#undef	HAVE_CEIL
#undef	HAVE_FLOOR
#undef	HAVE_REMAINDER
#undef	HAVE_ACOS
#undef	HAVE_ASIN
#undef	HAVE_ATAN
#undef	HAVE_ATAN2
#undef	HAVE_COS
#undef	HAVE_COSH
#undef	HAVE_EXP
#undef	HAVE_LOG
#undef	HAVE_LOG10
#undef	HAVE_POW
#undef	HAVE_SIN
#undef	HAVE_SINH
#undef	HAVE_SQRT
#undef	HAVE_TAN
#undef	HAVE_TANH
#endif

/*
 * Useful constant that represents "not a number".
 */
#define	NOT_A_NUMBER	((ILDouble)(0.0 / 0.0))

/*
 * Determine if a double value is finite.
 */
#if defined(HAVE_FINITE)
#define	FiniteDouble(x)		(finite((x)))
#elif defined(HAVE_ISNAN) && defined(HAVE_ISINF)
#define	FiniteDouble(x)		(!isnan((x)) && isinf((x)) == 0)
#else
static int FiniteDouble(double x)
{
	unsigned char buf[8];
	IL_WRITE_DOUBLE(buf, (ILDouble)x);
	return ((IL_READ_UINT32(buf + 4) & (ILUInt32)0x7FF00000)
					!= (ILUInt32)0x7FF00000);
}
#endif

/*
 * Largest integer that can be represented in an IEEE double / 2.
 */
#define	LARGEST_INT		4503599627370496.0

/*
 * Get the floor of a double value.
 */
#ifdef HAVE_CEIL
#define	FloorDouble(x)		(floor((x)))
#else
static double FloorDouble(double x)
{
	double temp;
	if(!FiniteDouble(x))
	{
		return x;
	}
	else if(x < 0.0)
	{
		temp = -(FloorDouble(-x));
		if(temp != x)
		{
			return temp - 1.0;
		}
		else
		{
			return x;
		}
	}
	else
	{
		/* Note: "y" must be volatile so that the compiler
		   does not optimise the operations away */
		volatile double y = LARGEST_INT + x;
		y -= x;
		if(x < y)
		{
			return y - 1.0;
		}
		else
		{
			return y;
		}
	}
}
#endif

/*
 * Get the ceiling of a double value.
 */
#ifdef HAVE_CEIL
#define	CeilDouble(x)		(ceil((x)))
#else
static double CeilDouble(double x)
{
	if(!FiniteDouble(x))
	{
		return x;
	}
	else if(x < 0.0)
	{
		return -(FloorDouble(-x));
	}
	else
	{
		double temp = FloorDouble(x);
		if(temp != x)
		{
			return temp + 1.0;
		}
		else
		{
			return x;
		}
	}
}
#endif

/*
 * Compute the IEEE remainder of two double values.
 */
#ifdef HAVE_REMAINDER
#define	RemainderDouble(x,y)		(remainder((x), (y)))
#else
static double RemainderDouble(double x, double y)
{
	/* TODO */
}
#endif

/*
 * Round a "double" value to the nearest integer, using the
 * "round half even" rounding mode.
 */
static double RoundDouble(double x)
{
	double above, below;
	if(!FiniteDouble(x))
	{
		return x;
	}
	above = CeilDouble(x);
	below = FloorDouble(x);
	if((above - x) < 0.5)
	{
		return above;
	}
	else if((x - below) < 0.5)
	{
		return below;
	}
	else if(RemainderDouble(above, 2.0) == 0.0)
	{
		return above;
	}
	else
	{
		return below;
	}
}

/*
 * public static double Acos(double d);
 */
static ILDouble System_Math_Acos(ILExecThread *thread, ILDouble d)
{
#ifdef HAVE_ACOS
	return (ILDouble)(acos((double)d));
#else
	return NOT_A_NUMBER;
#endif
}

/*
 * public static double Asin(double d);
 */
static ILDouble System_Math_Asin(ILExecThread *thread, ILDouble d)
{
#ifdef HAVE_ASIN
	return (ILDouble)(asin((double)d));
#else
	return NOT_A_NUMBER;
#endif
}

/*
 * public static double Atan(double d);
 */
static ILDouble System_Math_Atan(ILExecThread *thread, ILDouble d)
{
#ifdef HAVE_ATAN
	return (ILDouble)(atan((double)d));
#else
	return NOT_A_NUMBER;
#endif
}

/*
 * public static double Atan2(double y, double x);
 */
static ILDouble System_Math_Atan2(ILExecThread *thread, ILDouble y, ILDouble x)
{
#ifdef HAVE_ATAN2
	return (ILDouble)(atan2((double)y, (double)x));
#else
	return NOT_A_NUMBER;
#endif
}

/*
 * public static double Ceiling(double a);
 */
static ILDouble System_Math_Ceiling(ILExecThread *thread, ILDouble a)
{
#ifdef HAVE_CEIL
	return (ILDouble)(ceil((double)a));
#else
	return NOT_A_NUMBER;
#endif
}

/*
 * public static double Cos(double d);
 */
static ILDouble System_Math_Cos(ILExecThread *thread, ILDouble d)
{
#ifdef HAVE_COS
	return (ILDouble)(cos((double)d));
#else
	return NOT_A_NUMBER;
#endif
}

/*
 * public static double Cosh(double value);
 */
static ILDouble System_Math_Cosh(ILExecThread *thread, ILDouble value)
{
#ifdef HAVE_COSH
	return (ILDouble)(cosh((double)value));
#else
	return NOT_A_NUMBER;
#endif
}

/*
 * public static double Exp(double d);
 */
static ILDouble System_Math_Exp(ILExecThread *thread, ILDouble d)
{
#ifdef HAVE_EXP
	return (ILDouble)(exp((double)d));
#else
	return NOT_A_NUMBER;
#endif
}

/*
 * public static double Floor(double d);
 */
static ILDouble System_Math_Floor(ILExecThread *thread, ILDouble d)
{
#ifdef HAVE_FLOOR
	return (ILDouble)(floor((double)d));
#else
	return NOT_A_NUMBER;
#endif
}

/*
 * public static double IEEERemainder(double x, double y);
 */
static ILDouble System_Math_IEEERemainder(ILExecThread *thread,
										  ILDouble x, ILDouble y)
{
#ifdef HAVE_REMAINDER
	return (ILDouble)(remainder((double)x, (double)y));
#else
	return NOT_A_NUMBER;
#endif
}

/*
 * public static double Log(double d);
 */
static ILDouble System_Math_Log(ILExecThread *thread, ILDouble d)
{
#ifdef HAVE_LOG
	return (ILDouble)(log((double)d));
#else
	return NOT_A_NUMBER;
#endif
}

/*
 * public static double Log10(double d);
 */
static ILDouble System_Math_Log10(ILExecThread *thread, ILDouble d)
{
#ifdef HAVE_LOG10
	return (ILDouble)(log10((double)d));
#else
	return NOT_A_NUMBER;
#endif
}

/*
 * public static double Pow(double x, double y);
 */
static ILDouble System_Math_Pow(ILExecThread *thread, ILDouble x, ILDouble y)
{
#ifdef HAVE_POW
	return (ILDouble)(pow((double)x, (double)y));
#else
	return NOT_A_NUMBER;
#endif
}

/*
 * public static double Round(double a);
 */
static ILDouble System_Math_Round(ILExecThread *thread, ILDouble a)
{
	return (ILDouble)RoundDouble((double)a);
}

/*
 * private static double InternalRound(double value, int digits);
 */
static ILDouble System_Math_InternalRound(ILExecThread *thread,
										  ILDouble value, ILInt32 digits)
{
	double rounded;
	double power;
	rounded = RoundDouble((double)value);
	if(digits == 0 || rounded == (double)value)
	{
		/* Simple rounding, or the value is already an integer */
		return rounded;
	}
	else
	{
	#ifdef HAVE_POW
		power = pow(1.0, (double)digits);
	#else
		power = 1.0;
		while(digits > 0)
		{
			power *= 10.0;
			--digits;
		}
	#endif
		return (RoundDouble(((double)value) * power) / power);
	}
}

/*
 * public static double Sin(double a);
 */
static ILDouble System_Math_Sin(ILExecThread *thread, ILDouble a)
{
#ifdef HAVE_SIN
	return (ILDouble)(sin((double)a));
#else
	return NOT_A_NUMBER;
#endif
}

/*
 * public static double Sinh(double a);
 */
static ILDouble System_Math_Sinh(ILExecThread *thread, ILDouble a)
{
#ifdef HAVE_SINH
	return (ILDouble)(sinh((double)a));
#else
	return NOT_A_NUMBER;
#endif
}

/*
 * public static double Sqrt(double a);
 */
static ILDouble System_Math_Sqrt(ILExecThread *thread, ILDouble a)
{
#ifdef HAVE_SQRT
	return (ILDouble)(sqrt((double)a));
#else
	return NOT_A_NUMBER;
#endif
}

/*
 * public static double Tan(double a);
 */
static ILDouble System_Math_Tan(ILExecThread *thread, ILDouble a)
{
#ifdef HAVE_TAN
	return (ILDouble)(tan((double)a));
#else
	return NOT_A_NUMBER;
#endif
}

/*
 * public static double Tanh(double value);
 */
static ILDouble System_Math_Tanh(ILExecThread *thread, ILDouble value)
{
#ifdef HAVE_TANH
	return (ILDouble)(tanh((double)value));
#else
	return NOT_A_NUMBER;
#endif
}

/*
 * public static bool IsNaN(float f);
 */
static ILBool System_Single_IsNaN(ILExecThread *thread, ILFloat f)
{
#ifdef HAVE_ISNAN
	return isnan(f);
#else
	unsigned char buf[4];
	IL_WRITE_FLOAT(buf, f);
	return ((IL_READ_UINT32(buf) & (ILUInt32)0x7FC00000)
					== (ILUInt32)0x7FC00000);
#endif
}

/*
 * public static bool IsInfinity(float f);
 */
static ILBool System_Single_IsInfinity(ILExecThread *thread, ILFloat f)
{
#ifdef HAVE_ISINF
	return (isinf(f) != 0);
#else
	unsigned char buf[4];
	IL_WRITE_FLOAT(buf, f);
	return ((IL_READ_UINT32(buf) & (ILUInt32)0x7FC00000)
					== (ILUInt32)0x7F800000);
#endif
}

/*
 * public static bool IsNaN(double d);
 */
static ILBool System_Double_IsNaN(ILExecThread *thread, ILDouble d)
{
#ifdef HAVE_ISNAN
	return isnan(d);
#else
	unsigned char buf[8];
	IL_WRITE_DOUBLE(buf, d);
	return ((IL_READ_UINT32(buf + 4) & (ILUInt32)0x7FF80000)
					== (ILUInt32)0x7FF80000);
#endif
}

/*
 * public static bool IsInfinity(double d);
 */
static ILBool System_Double_IsInfinity(ILExecThread *thread, ILDouble d)
{
#ifdef HAVE_ISINF
	return (isinf(d) != 0);
#else
	unsigned char buf[8];
	IL_WRITE_DOUBLE(buf, d);
	return ((IL_READ_UINT32(buf + 4) & (ILUInt32)0x7FF80000)
					== (ILUInt32)0x7FF00000);
#endif
}

/*
 * Method table for the "System.Math" class.
 */
IL_METHOD_BEGIN(_ILSystemMathMethods)
	IL_METHOD("Acos",			 "(d)d",		System_Math_Acos)
	IL_METHOD("Asin",			 "(d)d",		System_Math_Asin)
	IL_METHOD("Atan",			 "(d)d",		System_Math_Atan)
	IL_METHOD("Atan2",			 "(dd)d",		System_Math_Atan2)
	IL_METHOD("Ceiling",		 "(d)d",		System_Math_Ceiling)
	IL_METHOD("Cos",			 "(d)d",		System_Math_Cos)
	IL_METHOD("Cosh",			 "(d)d",		System_Math_Cosh)
	IL_METHOD("Exp",			 "(d)d",		System_Math_Exp)
	IL_METHOD("Floor",			 "(d)d",		System_Math_Floor)
	IL_METHOD("IEEERemainder",	 "(dd)d",		System_Math_IEEERemainder)
	IL_METHOD("Log",			 "(d)d",		System_Math_Log)
	IL_METHOD("Log10",			 "(d)d",		System_Math_Log10)
	IL_METHOD("Pow",			 "(d)d",		System_Math_Pow)
	IL_METHOD("Round",			 "(d)d",		System_Math_Round)
	IL_METHOD("InternalRound",	 "(di)d",		System_Math_InternalRound)
	IL_METHOD("Sin",			 "(d)d",		System_Math_Sin)
	IL_METHOD("Sinh",			 "(d)d",		System_Math_Sinh)
	IL_METHOD("Sqrt",			 "(d)d",		System_Math_Sqrt)
	IL_METHOD("Tan",			 "(d)d",		System_Math_Tan)
	IL_METHOD("Tanh",			 "(d)d",		System_Math_Tanh)
IL_METHOD_END

/*
 * Method table for the "System.Single" class.
 */
IL_METHOD_BEGIN(_ILSystemSingleMethods)
	IL_METHOD("IsNaN",			 "(f)Z",		System_Single_IsNaN)
	IL_METHOD("IsInfinity",		 "(f)Z",		System_Single_IsInfinity)
IL_METHOD_END

/*
 * Method table for the "System.Double" class.
 */
IL_METHOD_BEGIN(_ILSystemDoubleMethods)
	IL_METHOD("IsNaN",			 "(d)Z",		System_Double_IsNaN)
	IL_METHOD("IsInfinity",		 "(d)Z",		System_Double_IsInfinity)
IL_METHOD_END

#ifdef	__cplusplus
};
#endif
