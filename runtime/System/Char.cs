/*
 * Char.cs - Implementation of the "System.Char" class.
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

namespace System
{

using System.Globalization;

public struct Char : IComparable, IConvertible
{
	private char value__;

	public const char Empty    = '\u0000';
	public const char MaxValue = '\uFFFF';
	public const char MinValue = '\u0000';

	// Override inherited methods.
	public override int GetHashCode()
			{ return unchecked(((int)value__) | (((int)value__) << 15)); }
	public override bool Equals(Object value)
			{
				if(value is Char)
				{
					return (value__ == ((Char)value).value__);
				}
				else
				{
					return false;
				}
			}

	// String conversion.
	public override String ToString()
			{
				return ToString(value__);
			}
	public static String ToString(char value)
			{
				char[] array = new char[1];
				array[0] = value;
				return new String(array, 0, 1);
			}

	// Parsing methods.
	public static char FromString(String s)
			{
				if(s != null)
				{
					if(s.Length == 1)
					{
						return s[0];
					}
					else
					{
						throw new FormatException
							(Environment.GetResourceString
								("Format_NeedSingleChar"));
					}
				}
				else
				{
					throw new ArgumentNullException("s");
				}
			}

	// Implementation of the IComparable interface.
	public int CompareTo(Object value)
			{
				if(value != null)
				{
					if(!(value is Char))
					{
						throw new ArgumentException
							(Environment.GetResourceString("Arg_MustBeChar"));
					}
					return ((int)value__) - ((int)(((Char)value).value__));
				}
				else
				{
					return 1;
				}
			}

	// Implementation of IConvertible interface.
	public TypeCode GetTypeCode() { return TypeCode.Char; }
	public Object ToType(Type ct) { return Convert.DefaultToType(this, ct); }
	public Boolean ToBoolean()     { return Convert.ToBoolean(value__); }
	public Byte ToByte()           { return Convert.ToByte(value__); }
	public SByte ToSByte()         { return Convert.ToSByte(value__); }
	public Int16 ToInt16()         { return Convert.ToInt16(value__); }
	public UInt16 ToUInt16()	   { return Convert.ToUInt16(value__); }
	public Int32 ToInt32()         { return Convert.ToInt32(value__); }
	public UInt32 ToUInt32()       { return Convert.ToUInt32(value__); }
	public Int64 ToInt64()         { return Convert.ToInt64(value__); }
	public UInt64 ToUInt64()       { return Convert.ToUInt64(value__); }
	public Char ToChar()           { return value__; }
	public Single ToSingle()
			{
				throw new InvalidCastException
					(String.Format
						(Environment.GetResourceString("InvalidCast_FromTo"),
	 			         "Char", "Single"));
			}
	public Double ToDouble()
			{
				throw new InvalidCastException
					(String.Format
						(Environment.GetResourceString("InvalidCast_FromTo"),
	 			         "Char", "Double"));
			}
	public Decimal ToDecimal()
			{
				throw new InvalidCastException
					(String.Format
						(Environment.GetResourceString("InvalidCast_FromTo"),
	 			         "Char", "Decimal"));
			}
	public DateTime ToDateTime()
			{
				throw new InvalidCastException
					(String.Format
						(Environment.GetResourceString("InvalidCast_FromTo"),
	 			         "Char", "DateTime"));
			}

	// Category testing.
	public static bool IsDigit(char value)
			{ return CharacterInfo.IsDigit(value); }
	public static bool IsISOControl(char value)
			{
				uint val = unchecked((uint)value);
				if(val < 32 || (val >= 127 && val < 160))
				{
					return true;
				}
				else
				{
					return false;
				}
			}
	public static bool IsLetter(char value)
			{ return CharacterInfo.IsLetter(value); }
	public static bool IsLetterOrDigit(char value)
			{ /* TODO */ return false; }
	public static bool IsLower(char value)
			{ return CharacterInfo.IsLower(value); }
	public static bool IsPrintable(char value)
			{ /* TODO */ return false; }
	public static bool IsPunctuation(char value)
			{ return CharacterInfo.IsPunctuation(value); }
	public static bool IsTitleCase(char value)
			{ return CharacterInfo.IsTitleCase(value); }
	public static bool IsUnicodeIdentifierPart(char value)
			{ return false; }
	public static bool IsUnicodeIdentifierStart(char value)
			{ return false; }
	public static bool IsUpper(char value)
			{ return CharacterInfo.IsUpper(value); }
	public static bool IsWhiteSpace(char value)
			{ return CharacterInfo.IsWhiteSpace(value); }

	// Case conversion.
	public static char ToLower(char value, CultureInfo culture)
			{
				if(culture != null)
				{
					return culture.TextInfo.ToLower(value);
				}
				else
				{
					throw new ArgumentNullException("culture");
				}
			}
	public static char ToLower(char value)
			{ return ToLower(value, CultureInfo.CurrentCulture); }
	public static char ToUpper(char value, CultureInfo culture)
			{
				if(culture != null)
				{
					return culture.TextInfo.ToUpper(value);
				}
				else
				{
					throw new ArgumentNullException("culture");
				}
			}
	public static char ToUpper(char value)
			{ return ToUpper(value, CultureInfo.CurrentCulture); }

}; // class Char

}; // namespace System
