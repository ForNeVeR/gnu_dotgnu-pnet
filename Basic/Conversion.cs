/*
 * Conversion.cs - Implementation of the
 *			"Microsoft.VisualBasic.Conversion" class.
 *
 * Copyright (C) 2003  Southern Storm Software, Pty Ltd.
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

namespace Microsoft.VisualBasic
{

using System;
using Microsoft.VisualBasic.CompilerServices;

[StandardModule]
public sealed class Conversion
{
	// This class cannot be instantiated.
	private Conversion() {}

	// Get the error message for a particular error number.
	[TODO]
	public static String ErrorToString()
			{
				// TODO
				return null;
			}
	[TODO]
	public static String ErrorToString(int errornumber)
			{
				// TODO
				return null;
			}

	// Get the "fix" integer version of a value.
	public static short Fix(short Number)
			{
				return Number;
			}
	public static int Fix(int Number)
			{
				return Number;
			}
	public static long Fix(long Number)
			{
				return Number;
			}
	public static double Fix(double Number)
			{
				if(Number >= 0.0)
				{
					return Math.Floor(Number);
				}
				else
				{
					return Math.Ceiling(Number);
				}
			}
	public static float Fix(float Number)
			{
				if(Number >= 0.0)
				{
					return (float)(Math.Floor(Number));
				}
				else
				{
					return (float)(Math.Ceiling(Number));
				}
			}
	public static Decimal Fix(Decimal Number)
			{
				return Decimal.Truncate(Number);
			}
	[TODO]
	public static Object Fix(Object Number)
			{
				// TODO
				return Number;
			}

	// Get the hexadecimal form of a value.
	public static String Hex(byte Number)
			{
				return Number.ToString("X");
			}
	public static String Hex(short Number)
			{
				return Number.ToString("X");
			}
	public static String Hex(int Number)
			{
				return Number.ToString("X");
			}
	public static String Hex(long Number)
			{
				return Number.ToString("X");
			}
	[TODO]
	public static String Hex(Object Number)
			{
				// TODO
				return null;
			}

	// Get the integer version of a value.
	public static short Int(short Number)
			{
				return Number;
			}
	public static int Int(int Number)
			{
				return Number;
			}
	public static long Int(long Number)
			{
				return Number;
			}
	public static double Int(double Number)
			{
				return Math.Floor(Number);
			}
	public static float Int(float Number)
			{
				return (float)(Math.Floor(Number));
			}
	public static Decimal Int(Decimal Number)
			{
				return Decimal.Floor(Number);
			}
	[TODO]
	public static Object Int(Object Number)
			{
				// TODO
				return Number;
			}

	// Get the octal form of a value.
	public static String Oct(byte Number)
			{
				return Oct((long)Number);
			}
	public static String Oct(short Number)
			{
				return Oct((long)(ushort)Number);
			}
	public static String Oct(int Number)
			{
				return Oct((long)Number);
			}
	[TODO]
	public static String Oct(long Number)
			{
				// TODO
				return null;
			}
	[TODO]
	public static String Oct(Object Number)
			{
				// TODO
				return null;
			}

	// Convert an object into a string.
	[TODO]
	public static String Str(Object Number)
			{
				// TODO
				return null;
			}

	// Get the numeric value of a string.
	[TODO]
	public static double Val(String InputStr)
			{
				// TODO
				return 0.0;
			}
	public static double Val(Object Expression)
			{
				return Val(Str(Expression));
			}
	public static int Val(char Expression)
			{
				if(Expression >= '0' && Expression <= '9')
				{
					return (int)(Expression - '0');
				}
				else
				{
					return 0;
				}
			}

}; // class Conversion

}; // namespace Microsoft.VisualBasic
