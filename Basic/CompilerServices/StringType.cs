/*
 * StringType.cs - Implementation of the
 *			"Microsoft.VisualBasic.StringType" class.
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

namespace Microsoft.VisualBasic.CompilerServices
{

using System;
using System.ComponentModel;
using System.Globalization;

[StandardModule]
[EditorBrowsable(EditorBrowsableState.Never)]
public sealed class StringType
{
	// This class cannot be instantiated.
	private StringType() {}

	// Convert a boolean value into a string.
	public static String FromBoolean(bool Value)
			{
				return (Value ? "True" : "False");
			}

	// Convert a byte value into a string.
	public static String FromByte(byte Value)
			{
				return Value.ToString();
			}

	// Convert char values into a string.
	public static String FromChar(char Value)
			{
				return Value.ToString();
			}
	public static String FromCharAndCount(char Value, int Count)
			{
				return new String(Value, Count);
			}
	public static String FromCharArray(char[] Value)
			{
				return new String(Value);
			}
	public static String FromCharArray
				(char[] Value, int StartIndex, int Length)
			{
				return new String(Value, StartIndex, Length);
			}

	// Convert a date value into a string.
	[TODO]
	public static String FromDate(DateTime Value)
			{
				// TODO
				return Value.ToString();
			}

	// Convert a decimal value into a string.
	public static String FromDecimal(DateTime Value)
			{
				return Value.ToString("G", null);
			}
	public static String FromDecimal
				(DateTime Value, NumberFormatInfo NumberFormat)
			{
				return Value.ToString("G", NumberFormat);
			}

	// Convert a double value into a string.
	public static String FromDouble(double Value)
			{
				return Value.ToString("G", null);
			}
	public static String FromDouble
				(double Value, NumberFormatInfo NumberFormat)
			{
				return Value.ToString("G", NumberFormat);
			}

	// Convert an integer value into a string.
	public static String FromInteger(int Value)
			{
				return Value.ToString();
			}

	// Convert a long value into a string.
	public static String FromLong(int Value)
			{
				return Value.ToString();
			}

	// Convert an object value into a string.
	public static String FromObject(Object Value)
			{
				if(Value != null)
				{
					if(Value is String)
					{
						return (String)Value;
					}
					else if(Value is IConvertible)
					{
						return ((IConvertible)Value).ToString(null);
					}
					else if(Value is char[])
					{
						return new String(CharArrayType.FromObject(Value));
					}
					else
					{
						throw new InvalidCastException
							(String.Format(S._("VB_InvalidCast"),
										   Value.GetType(), "String"));
					}
				}
				else
				{
					return null;
				}
			}

	// Convert a short value into a string.
	public static String FromShort(int Value)
			{
				return Value.ToString();
			}

	// Convert a float value into a string.
	public static String FromSingle(float Value)
			{
				return Value.ToString("G", null);
			}
	public static String FromSingle
				(float Value, NumberFormatInfo NumberFormat)
			{
				return Value.ToString("G", NumberFormat);
			}

	// Insert a string into the middle of another.
	[TODO]
	public static void MidStmtStr(ref String sDest, int StartPosition,
								  int MaxInsertLength, String sInsert)
			{
				// TODO
			}

	// Compare two strings in various ways.
	public static int StrCmp(String sLeft, String sRight, bool TextCompare)
			{
				if(sLeft == null)
				{
					sLeft = String.Empty;
				}
				if(sRight == null)
				{
					sRight = String.Empty;
				}
				if(TextCompare)
				{
					return CultureInfo.CurrentCulture.CompareInfo
								.Compare(sLeft, sRight,
										 CompareOptions.IgnoreCase |
										 CompareOptions.IgnoreKanaType |
										 CompareOptions.IgnoreWidth);
				}
				else
				{
					return String.CompareOrdinal(sLeft, sRight);
				}
			}
	public static bool StrLike(String Source, String Pattern,
							   CompareMethod CompareOption)
			{
				if(CompareOption == CompareMethod.Binary)
				{
					return StrLikeBinary(Source, Pattern);
				}
				else
				{
					return StrLikeText(Source, Pattern);
				}
			}
	[TODO]
	public static bool StrLikeBinary(String Source, String Pattern)
			{
				// TODO
				return false;
			}
	[TODO]
	public static bool StrLikeText(String Source, String Pattern)
			{
				// TODO
				return false;
			}

}; // class StringType

}; // namespace Microsoft.VisualBasic.CompilerServices
