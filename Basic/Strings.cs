/*
 * Strings.cs - Implementation of the
 *			"Microsoft.VisualBasic.Strings" class.
 *
 * Copyright (C) 2003, 2004  Southern Storm Software, Pty Ltd.
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
using System.ComponentModel;
using System.Globalization;
using System.Text;
using System.Runtime.InteropServices;
using Microsoft.VisualBasic.CompilerServices;

[StandardModule]
public sealed class Strings
{
	// This class cannot be instantiated.
	private Strings() {}

	// Get a character code.
	public static int Asc(char String)
			{
				if(((int)String) < 0x80)
				{
					return (int)String;
				}
				char[] charBuf = new char [] {String};
				byte[] byteBuf = Encoding.Default.GetBytes(charBuf);
				if(byteBuf.Length == 0)
				{
					return 0;
				}
				else if(byteBuf.Length == 1)
				{
					return byteBuf[0];
				}
				else
				{
					return (int)(short)((byteBuf[0] << 8) | byteBuf[1]);
				}
			}
	public static int Asc(String String)
			{
				if(String == null || String.Length == 0)
				{
					throw new ArgumentException(S._("VB_EmptyString"));
				}
				return Asc(String[0]);
			}
	public static int AscW(char String)
			{
				return (int)String;
			}
	public static int AscW(String String)
			{
				if(String == null || String.Length == 0)
				{
					throw new ArgumentException(S._("VB_EmptyString"));
				}
				return (int)(String[0]);
			}

	// Convert an integer character code into a character.
	public static char Chr(int CharCode)
			{
				byte[] byteBuf;
				char[] charBuf;
				if(CharCode >= 0 && CharCode <= 127)
				{
					return (char)CharCode;
				}
				else if(CharCode >= 128 && CharCode <= 255)
				{
					byteBuf = new byte [] {(byte)CharCode};
				}
				else if(CharCode >= -32768 && CharCode <= 65535)
				{
					byteBuf = new byte []
						{(byte)(CharCode >> 8), (byte)CharCode};
				}
				else
				{
					throw new ArgumentException(S._("VB_ChrRange"));
				}
				charBuf = Encoding.Default.GetChars(byteBuf);
				if(charBuf.Length == 0)
				{
					return '\u0000';
				}
				else
				{
					return charBuf[0];
				}
			}
	public static char ChrW(int CharCode)
			{
				if(CharCode >= -32768 && CharCode <= 65535)
				{
					return (char)CharCode;
				}
				else
				{
					throw new ArgumentException(S._("VB_ChrRange"));
				}
			}

	// Filter an array of strings on a condition.
	[OptionCompare]
	public static String[] Filter
				(Object[] source, String Match,
				 [Optional][DefaultValue(true)] bool Include,
				 [Optional][DefaultValue(CompareMethod.Binary)]
				 		CompareMethod Compare)
			{
				if(source == null)
				{
					throw new ArgumentNullException("source");
				}
				String[] strSource = new String [source.Length];
				int posn;
				for(posn = 0; posn < source.Length; ++posn)
				{
					strSource[posn] = StringType.FromObject(source[posn]);
				}
				return Filter(strSource, Match, Include, Compare);
			}
	[OptionCompare]
	public static String[] Filter
				(String[] source, String Match,
				 [Optional][DefaultValue(true)] bool Include,
				 [Optional][DefaultValue(CompareMethod.Binary)]
				 		CompareMethod Compare)
			{
				if(Match == null || Match.Length == 0)
				{
					return null;
				}
				int count = FilterInternal
					(source, Match, Include, Compare, null);
				String[] result = new String [count];
				FilterInternal(source, Match, Include, Compare, result);
				return result;
			}
	private static int FilterInternal
				(String[] source, String Match,
				 bool Include, CompareMethod Compare, String[] result)
			{
				int count = 0;
				CompareInfo compare = CultureInfo.CurrentCulture.CompareInfo;
				foreach(String value in source)
				{
					if(value == null)
					{
						continue;
					}
					if(Include)
					{
						if(Compare == CompareMethod.Binary)
						{
							if(value.IndexOf(Match) != -1)
							{
								if(result != null)
								{
									result[count] = value;
								}
								++count;
							}
						}
						else if(compare.IndexOf
								(value, Match, CompareOptions.IgnoreCase) != -1)
						{
							if(result != null)
							{
								result[count] = value;
							}
							++count;
						}
					}
					else if(Compare == CompareMethod.Binary)
					{
						if(value == Match)
						{
							if(result != null)
							{
								result[count] = value;
							}
							++count;
						}
					}
					else if(compare.Compare
							(value, Match, CompareOptions.IgnoreCase) == 0)
					{
						if(result != null)
						{
							result[count] = value;
						}
						++count;
					}
				}
				return count;
			}

	// Format a currency value.
	[TODO]
	public static String FormatCurrency
				(Object Expression,
				 [Optional][DefaultValue(-1)] int NumDigitsAfterDecimal,
				 [Optional][DefaultValue(TriState.UseDefault)]
				 		TriState IncludeLeadingDigit,
				 [Optional][DefaultValue(TriState.UseDefault)]
				 		TriState UseParensForNegativeNumbers,
				 [Optional][DefaultValue(TriState.UseDefault)]
				 		TriState GroupDigits)
			{
				// TODO
				return null;
			}

	// Format a DateTime value.
	[TODO]
	public static String FormatDateTime
				(DateTime Expression,
				 [Optional][DefaultValue(DateFormat.GeneralDate)]
				 		DateFormat NamedFormat)
			{
				// TODO
				return null;
			}

	// Format a numeric value.
	[TODO]
	public static String FormatNumber
				(Object Expression,
				 [Optional][DefaultValue(-1)] int NumDigitsAfterDecimal,
				 [Optional][DefaultValue(TriState.UseDefault)]
				 		TriState IncludeLeadingDigit,
				 [Optional][DefaultValue(TriState.UseDefault)]
				 		TriState UseParensForNegativeNumbers,
				 [Optional][DefaultValue(TriState.UseDefault)]
				 		TriState GroupDigits)
			{
				// TODO
				return null;
			}

	// Format a percentage value.
	[TODO]
	public static String FormatPercent
				(Object Expression,
				 [Optional][DefaultValue(-1)] int NumDigitsAfterDecimal,
				 [Optional][DefaultValue(TriState.UseDefault)]
				 		TriState IncludeLeadingDigit,
				 [Optional][DefaultValue(TriState.UseDefault)]
				 		TriState UseParensForNegativeNumbers,
				 [Optional][DefaultValue(TriState.UseDefault)]
				 		TriState GroupDigits)
			{
				// TODO
				return null;
			}

	// Get a particular character within a string.
	public static char GetChar(String str, int Index)
			{
				if(str == null)
				{
					throw new ArgumentException(S._("VB_ValueIsNull"), "str");
				}
				if(Index < 1 || Index > str.Length)
				{
					throw new ArgumentException
						(S._("VB_InvalidStringIndex"), "Index");
				}
				return str[Index - 1];
			}

	// Join an array of strings together.
	public static String Join(Object[] SourceArray,
							  [Optional][DefaultValue(" ")] String Delimiter)
			{
				if(SourceArray == null)
				{
					throw new ArgumentNullException("SourceArray");
				}
				String[] strSource = new String [SourceArray.Length];
				int posn;
				for(posn = 0; posn < SourceArray.Length; ++posn)
				{
					strSource[posn] = StringType.FromObject(SourceArray[posn]);
				}
				return String.Join(Delimiter, strSource);
			}
	public static String Join(String[] SourceArray,
							  [Optional][DefaultValue(" ")] String Delimiter)
			{
				if(SourceArray == null)
				{
					return null;
				}
				return String.Join(Delimiter, SourceArray);
			}

	// Convert a string or character to lower case.
	public static String LCase(String Value)
			{
				if(Value != null)
				{
					return CultureInfo.CurrentCulture.TextInfo.ToLower(Value);
				}
				else
				{
					return null;
				}
			}
	public static char LCase(char Value)
			{
				return CultureInfo.CurrentCulture.TextInfo.ToLower(Value);
			}

	// Convert a string or character to upper case.
	public static String UCase(String Value)
			{
				if(Value != null)
				{
					return CultureInfo.CurrentCulture.TextInfo.ToUpper(Value);
				}
				else
				{
					return null;
				}
			}
	public static char UCase(char Value)
			{
				return CultureInfo.CurrentCulture.TextInfo.ToUpper(Value);
			}

	// Align a string within a given field length.
	public static String LSet(String Source, int Length)
			{
				if(Source == null)
				{
					return new String(' ', Length);
				}
				if(Source.Length >= Length)
				{
					return Source.Substring(0, Length);
				}
				else
				{
					return Source.PadRight(Length);
				}
			}
	public static String RSet(String Source, int Length)
			{
				if(Source == null)
				{
					return new String(' ', Length);
				}
				if(Source.Length >= Length)
				{
					return Source.Substring(0, Length);
				}
				else
				{
					return Source.PadLeft(Length);
				}
			}

	// Trim white space from a string.
	public static String LTrim(String str)
			{
				if(str != null)
				{
					return str.TrimStart(null);
				}
				else
				{
					return String.Empty;
				}
			}
	public static String RTrim(String str)
			{
				if(str != null)
				{
					return str.TrimEnd(null);
				}
				else
				{
					return String.Empty;
				}
			}
	public static String Trim(String str)
			{
				if(str != null)
				{
					return str.Trim();
				}
				else
				{
					return String.Empty;
				}
			}

	// Get the left, middle, or right parts of a string.
	public static String Left(String Str, int Length)
			{
				if(Length < 0)
				{
					throw new ArgumentException
						(S._("VB_NonNegative"), "Length");
				}
				if(Str == null)
				{
					return String.Empty;
				}
				if(Str.Length <= Length)
				{
					return Str;
				}
				else
				{
					return Str.Substring(0, Length);
				}
			}
	public static String Mid(String Str, int Start)
			{
				if(Str == null)
				{
					return null;
				}
				else
				{
					return Mid(Str, Start, Str.Length);
				}
			}
	public static String Mid(String Str, int Start, int Length)
			{
				if(Start < 1)
				{
					throw new ArgumentException
						(S._("VB_InvalidStringIndex"), "Start");
				}
				if(Length < 0)
				{
					throw new ArgumentException
						(S._("VB_NonNegative"), "Length");
				}
				int len = Str.Length;
				--Start;
				if(Start >= len)
				{
					return String.Empty;
				}
				if((Start + Length) < len)
				{
					len = Start + Length;
				}
				return Str.Substring(Start, len - Start);
			}
	public static String Right(String Str, int Length)
			{
				if(Length < 0)
				{
					throw new ArgumentException
						(S._("VB_NonNegative"), "Length");
				}
				if(Str == null)
				{
					return String.Empty;
				}
				int len = Str.Length;
				if(len <= Length)
				{
					return Str;
				}
				else
				{
					return Str.Substring(len - Length, Length);
				}
			}

	// Get the number of bytes to store a value, or the length of a string.
	public static int Len(bool Expression)
			{
				return 2;
			}
	public static int Len(byte Expression)
			{
				return 1;
			}
	public static int Len(short Expression)
			{
				return 2;
			}
	public static int Len(char Expression)
			{
				return 2;
			}
	public static int Len(int Expression)
			{
				return 4;
			}
	public static int Len(long Expression)
			{
				return 8;
			}
	public static int Len(float Expression)
			{
				return 4;
			}
	public static int Len(double Expression)
			{
				return 8;
			}
	public static int Len(DateTime Expression)
			{
				return 8;
			}
	public static int Len(Decimal Expression)
			{
				return 16;
			}
	public static int Len(String Expression)
			{
				if(Expression != null)
				{
					return Expression.Length;
				}
				else
				{
					return 0;
				}
			}
	[TODO]
	public static int Len(Object Expression)
			{
				if(Expression == null)
				{
					return 0;
				}
				switch(ObjectType.GetTypeCode(Expression))
				{
					case TypeCode.SByte:
					case TypeCode.Byte:			return 1;

					case TypeCode.Boolean:
					case TypeCode.Char:
					case TypeCode.Int16:
					case TypeCode.UInt16:		return 2;

					case TypeCode.Single:
					case TypeCode.Int32:
					case TypeCode.UInt32:		return 4;

					case TypeCode.Double:
					case TypeCode.DateTime:
					case TypeCode.Int64:
					case TypeCode.UInt64:		return 8;

					case TypeCode.Decimal:		return 16;

					case TypeCode.String:
						return Len(StringType.FromObject(Expression));
				}
				if(Expression is char[])
				{
					return ((char[])Expression).Length;
				}
				else if(Expression is ValueType)
				{
					// TODO
				}
				throw new ArgumentException
					(S._("VB_CouldntHandle"), "Expression");
			}

	// Perform string replacement.
	[TODO]
	public static String Replace
				(String Expression, String Find, String Replacement,
				 [Optional][DefaultValue(1)] int Start,
				 [Optional][DefaultValue(-1)] int Count,
				 [Optional][DefaultValue(CompareMethod.Binary)]
				 		CompareMethod Compare)
			{
				// TODO
				return Expression;
			}

	// Create a string with "Number" spaces in it.
	public static String Spaces(int Number)
			{
				if(Number < 0)
				{
					throw new ArgumentException
						(S._("VB_NonNegative"), "Number");
				}
				else
				{
					return new String(' ', Number);
				}
			}

	// Split a string using a particular delimiter.
	[TODO]
	public static String[] Split
				(String Expression,
				 [Optional][DefaultValue(" ")] String Delimiter,
				 [Optional][DefaultValue(-1)] int Limit,
				 [Optional][DefaultValue(CompareMethod.Binary)]
				 		CompareMethod Compare)
			{
				// TODO
				return null;
			}

	// Compare two strings.
	public static int StrComp
				(String String1, String String2,
				 [Optional][DefaultValue(CompareMethod.Binary)]
				 		CompareMethod Compare)
			{
				if(Compare != CompareMethod.Binary &&
				   Compare != CompareMethod.Text)
				{
					throw new ArgumentException
						(S._("VB_InvalidCompareMethod"), "Compare");
				}
				return StringType.StrCmp
					(String1, String2, (Compare == CompareMethod.Text));
			}

	// Perform a string conversion.
	[TODO]
	public static String StrConv
				(String str, VbStrConv Conversion,
				 [Optional][DefaultValue(0)] int LocaleID)
			{
				// TODO
				return null;
			}

	// Duplicate a character a particular number of times.
	public static String StrDup(int Number, char Character)
			{
				if(Number >= 0)
				{
					return new String(Character, Number);
				}
				else
				{
					throw new ArgumentNullException
						(S._("VB_NonNegative"), "Number");
				}
			}
	public static String StrDup(int Number, String Character)
			{
				if(Number >= 0)
				{
					if(Character != null && Character.Length > 0)
					{
						return new String(Character[0], Number);
					}
					else
					{
						throw new ArgumentException
							(S._("VB_EmptyString"), "Character");
					}
				}
				else
				{
					throw new ArgumentException
						(S._("VB_NonNegative"), "Number");
				}
			}
	public static String StrDup(int Number, Object Character)
			{
				if(Number < 0)
				{
					throw new ArgumentException
						(S._("VB_NonNegative"), "Number");
				}
				if(Character == null)
				{
					throw new ArgumentNullException("Character");
				}
				else if(Character is String)
				{
					return StrDup(Number, (String)Character);
				}
				else if(Character is Char)
				{
					return StrDup(Number, (Char)Character);
				}
				else
				{
					try
					{
						return StrDup(Number, CharType.FromObject(Character));
					}
					catch(Exception)
					{
						throw new ArgumentException
							(S._("VB_NotAChar"), "Character");
					}
				}
			}

	// Reverse the contents of a string.
	public static String StrReverse(String Expression)
			{
				if(Expression == null || Expression.Length == 0)
				{
					return String.Empty;
				}
				StringBuilder builder = new StringBuilder();
				int posn;
				for(posn = Expression.Length - 1; posn >= 0; --posn)
				{
					builder.Append(Expression[posn]);
				}
				return builder.ToString();
			}

}; // class Strings

}; // namespace Microsoft.VisualBasic
