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

	public const char MaxValue = '\uFFFF';
	public const char MinValue = '\u0000';

	// Override inherited methods.
	public override int GetHashCode()
			{ return unchecked(((int)value__) | (((int)value__) << 16)); }
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
				return new String(value, 1);
			}

	// Parsing methods.
	public static char Parse(String s)
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
	public TypeCode GetTypeCode()
			{
				return TypeCode.Char;
			}
	bool IConvertible.ToBoolean(IFormatProvider provider)
			{
				throw new InvalidCastException
					(String.Format
						(Environment.GetResourceString("InvalidCast_FromTo"),
	 			         "Char", "Boolean"));
			}
	byte IConvertible.ToByte(IFormatProvider provider)
			{
				return Convert.ToByte(value__);
			}
	sbyte IConvertible.ToSByte(IFormatProvider provider)
			{
				return Convert.ToSByte(value__);
			}
	short IConvertible.ToInt16(IFormatProvider provider)
			{
				return Convert.ToInt16(value__);
			}
	ushort IConvertible.ToUInt16(IFormatProvider provider)
			{
				return Convert.ToUInt16(value__);
			}
	char IConvertible.ToChar(IFormatProvider provider)
			{
				return value__;
			}
	int IConvertible.ToInt32(IFormatProvider provider)
			{
				return Convert.ToInt32(value__);
			}
	uint IConvertible.ToUInt32(IFormatProvider provider)
			{
				return Convert.ToUInt32(value__);
			}
	long IConvertible.ToInt64(IFormatProvider provider)
			{
				return Convert.ToInt64(value__);
			}
	ulong IConvertible.ToUInt64(IFormatProvider provider)
			{
				return Convert.ToUInt64(value__);
			}
	float IConvertible.ToSingle(IFormatProvider provider)
			{
				throw new InvalidCastException
					(String.Format
						(Environment.GetResourceString("InvalidCast_FromTo"),
	 			         "Char", "Single"));
			}
	double IConvertible.ToDouble(IFormatProvider provider)
			{
				throw new InvalidCastException
					(String.Format
						(Environment.GetResourceString("InvalidCast_FromTo"),
	 			         "Char", "Double"));
			}
	Decimal IConvertible.ToDecimal(IFormatProvider provider)
			{
				throw new InvalidCastException
					(String.Format
						(Environment.GetResourceString("InvalidCast_FromTo"),
	 			         "Char", "Decimal"));
			}
	DateTime IConvertible.ToDateTime(IFormatProvider provider)
			{
				throw new InvalidCastException
					(String.Format
						(Environment.GetResourceString("InvalidCast_FromTo"),
		 			     "Char", "DateTime"));
			}
	public String ToString(IFormatProvider provider)
			{
				return ToString(value__);
			}
	Object IConvertible.ToType(Type conversionType, IFormatProvider provider)
			{
				return Convert.DefaultToType(this, conversionType, provider);
			}

	// Get the numeric value associated with a character.
	public static double GetNumericValue(char c)
			{
				if(c >= '0' && c <= '9')
				{
					return (double)(int)c;
				}
				else
				{
					return Platform.SysCharInfo.GetNumericValue(c);
				}
			}
	public static double GetNumericValue(String s, int index)
			{
				if(s == null)
				{
					throw new ArgumentNullException("s");
				}
				return GetNumericValue(s[index]);
			}

	// Get the Unicode category for a character.
	public static UnicodeCategory GetUnicodeCategory(char c)
			{
				return Platform.SysCharInfo.GetUnicodeCategory(c);
			}
	public static UnicodeCategory GetUnicodeCategory(String s, int index)
			{
				if(s == null)
				{
					throw new ArgumentNullException("s");
				}
				return Platform.SysCharInfo.GetUnicodeCategory(s[index]);
			}

	// Category testing.
	public static bool IsControl(char c)
			{
				return (GetUnicodeCategory(c) == UnicodeCategory.Control);
			}
	public static bool IsControl(String s, int index)
			{
				return (GetUnicodeCategory(s, index) ==
							UnicodeCategory.Control);
			}
	public static bool IsDigit(char c)
			{
				return (GetUnicodeCategory(c) ==
							UnicodeCategory.DecimalDigitNumber);
			}
	public static bool IsDigit(String s, int index)
			{
				return (GetUnicodeCategory(s, index) ==
							UnicodeCategory.DecimalDigitNumber);
			}
	public static bool IsLetter(char c)
			{
				UnicodeCategory category = GetUnicodeCategory(c);
				return (category == UnicodeCategory.UppercaseLetter ||
						category == UnicodeCategory.LowercaseLetter ||
						category == UnicodeCategory.TitlecaseLetter ||
						category == UnicodeCategory.ModifierLetter ||
						category == UnicodeCategory.OtherLetter);
			}
	public static bool IsLetter(String s, int index)
			{
				if(s == null)
				{
					throw new ArgumentNullException("s");
				}
				return IsLetter(s[index]);
			}
	public static bool IsLetterOrDigit(char c)
			{
				UnicodeCategory category = GetUnicodeCategory(c);
				return (category == UnicodeCategory.UppercaseLetter ||
						category == UnicodeCategory.LowercaseLetter ||
						category == UnicodeCategory.TitlecaseLetter ||
						category == UnicodeCategory.ModifierLetter ||
						category == UnicodeCategory.OtherLetter ||
						category == UnicodeCategory.DecimalDigitNumber);
			}
	public static bool IsLetterOrDigit(String s, int index)
			{
				if(s == null)
				{
					throw new ArgumentNullException("s");
				}
				return IsLetterOrDigit(s[index]);
			}
	public static bool IsLower(char c)
			{
				return (GetUnicodeCategory(c) ==
							UnicodeCategory.LowercaseLetter);
			}
	public static bool IsLower(String s, int index)
			{
				return (GetUnicodeCategory(s, index) ==
							UnicodeCategory.LowercaseLetter);
			}
	public static bool IsNumber(char c)
			{
				UnicodeCategory category = GetUnicodeCategory(c);
				return (category == UnicodeCategory.DecimalDigitNumber ||
						category == UnicodeCategory.LetterNumber ||
						category == UnicodeCategory.OtherNumber);
			}
	public static bool IsNumber(String s, int index)
			{
				if(s == null)
				{
					throw new ArgumentNullException("s");
				}
				return IsNumber(s[index]);
			}
	public static bool IsPunctuation(char c)
			{
				UnicodeCategory category = GetUnicodeCategory(c);
				return (category == UnicodeCategory.ConnectorPunctuation ||
						category == UnicodeCategory.DashPunctuation ||
						category == UnicodeCategory.OpenPunctuation ||
						category == UnicodeCategory.ClosePunctuation ||
						category == UnicodeCategory.InitialQuotePunctuation ||
						category == UnicodeCategory.FinalQuotePunctuation ||
						category == UnicodeCategory.OtherPunctuation);
			}
	public static bool IsPunctuation(String s, int index)
			{
				if(s == null)
				{
					throw new ArgumentNullException("s");
				}
				return IsPunctuation(s[index]);
			}
	public static bool IsSeparator(char c)
			{
				UnicodeCategory category = GetUnicodeCategory(c);
				return (category == UnicodeCategory.SpaceSeparator ||
						category == UnicodeCategory.LineSeparator ||
						category == UnicodeCategory.ParagraphSeparator);
			}
	public static bool IsSeparator(String s, int index)
			{
				if(s == null)
				{
					throw new ArgumentNullException("s");
				}
				return IsSeparator(s[index]);
			}
	public static bool IsSurrogate(char c)
			{
				return (GetUnicodeCategory(c) ==
							UnicodeCategory.Surrogate);
			}
	public static bool IsSurrogate(String s, int index)
			{
				return (GetUnicodeCategory(s, index) ==
							UnicodeCategory.Surrogate);
			}
	public static bool IsSymbol(char c)
			{
				UnicodeCategory category = GetUnicodeCategory(c);
				return (category == UnicodeCategory.MathSymbol ||
						category == UnicodeCategory.CurrencySymbol ||
						category == UnicodeCategory.ModifierSymbol ||
						category == UnicodeCategory.OtherSymbol);
			}
	public static bool IsSymbol(String s, int index)
			{
				if(s == null)
				{
					throw new ArgumentNullException("s");
				}
				return IsSymbol(s[index]);
			}
	public static bool IsUpper(char c)
			{
				return (GetUnicodeCategory(c) ==
							UnicodeCategory.UppercaseLetter);
			}
	public static bool IsUpper(String s, int index)
			{
				return (GetUnicodeCategory(s, index) ==
							UnicodeCategory.UppercaseLetter);
			}
	public static bool IsWhiteSpace(char c)
			{
				if(c == '\u0009' || c == '\u000a' || c == '\u000b' ||
				   c == '\u000c' || c == '\u000d' || c == '\u0085' ||
				   c == '\u2028' || c == '\u2029')
				{
					return true;
				}
				return (GetUnicodeCategory(c) ==
							UnicodeCategory.SpaceSeparator);
			}
	public static bool IsWhiteSpace(String s, int index)
			{
				if(s == null)
				{
					throw new ArgumentNullException("s");
				}
				return IsWhiteSpace(s[index]);
			}

	// Case conversion.
	public static char ToLower(char c, CultureInfo culture)
			{
				if(culture != null)
				{
					return culture.TextInfo.ToLower(c);
				}
				else
				{
					throw new ArgumentNullException("culture");
				}
			}
	public static char ToLower(char c)
			{
				return CultureInfo.CurrentCulture.TextInfo.ToLower(c);
			}
	public static char ToUpper(char c, CultureInfo culture)
			{
				if(culture != null)
				{
					return culture.TextInfo.ToUpper(c);
				}
				else
				{
					throw new ArgumentNullException("culture");
				}
			}
	public static char ToUpper(char c)
			{
				return CultureInfo.CurrentCulture.TextInfo.ToUpper(c);
			}

}; // class Char

}; // namespace System
