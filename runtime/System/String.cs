/*
 * String.cs - Implementation of the "System.String" class.
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

using System.Collections;
using System.Globalization;
using System.Runtime.InteropServices;
using System.Text;

public sealed class String : IComparable, ICloneable, IConvertible, IEnumerable
{

	// Internal string state.
	[NonSerialized]
	private int arrayLength;		// Total size of the underlying array.
	[NonSerialized]
	private int length;				// Actual length of the string.
	[NonSerialized]
	private char firstChar;			// First character in the string.

	// Public constants.
	public static readonly String Empty = "";

	// Constructors.  The storage for the string begins at
	// "firstChar".  The runtime engine is responsible for
	// allocating the necessary space during construction.

	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public String(char[] value, int startIndex, int length);

	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public String(char[] value);

	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public String(char c, int count);

	[MethodImpl(MethodImplOptions.InternalCall)]
	extern unsafe public String(char *value, int startIndex, int length);

	[MethodImpl(MethodImplOptions.InternalCall)]
	extern unsafe public String(char *value);

	//[MethodImpl(MethodImplOptions.InternalCall)]
	//extern unsafe public String(sbyte *value, int startIndex,
	//					          int length, Encoding enc);

	[MethodImpl(MethodImplOptions.InternalCall)]
	extern unsafe public String(sbyte *value, int startIndex, int length);

	[MethodImpl(MethodImplOptions.InternalCall)]
	extern unsafe public String(sbyte *value);

	// Implement the ICloneable interface.
	public Object Clone()
	{
		return this;
	}

	// Compare two strings.
	public static int Compare(String strA, String strB)
	{
		// TODO
		return 0;
	}

	// Compare two strings while optionally ignoring case.
	public static int Compare(String strA, String strB, bool ignoreCase)
	{
		// TODO
		return 0;
	}

	// Compare two strings using a particular culture's comparison rules.
	public static int Compare(String strA, String strB, bool ignoreCase,
							  CultureInfo culture)
	{
		// TODO
		return 0;
	}

	// Compare two sub-strings.
	public static int Compare(String strA, int indexA,
							  String strB, int indexB,
							  int length)
	{
		// TODO
		return 0;
	}

	// Compare two sub-strings while optionally ignoring case.
	public static int Compare(String strA, int indexA,
							  String strB, int indexB,
							  int length, bool ignoreCase)
	{
		// TODO
		return 0;
	}

	// Compare two sub-strings with a particular culture's comparison rules.
	public static int Compare(String strA, int indexA,
							  String strB, int indexB,
							  int length, bool ignoreCase,
							  CultureInfo culture)
	{
		// TODO
		return 0;
	}

	// Compare two strings by ordinal character value.
	public static int CompareOrdinal(String strA, String strB)
	{
		// TODO
		return 0;
	}

	// Compare two sub-strings by ordinal character value.
	public static int CompareOrdinal(String strA, int indexA,
									 String strB, int indexB)
	{
		// TODO
		return 0;
	}

	// Implement the IComparable interface.
	public int CompareTo(Object value)
	{
		if(!(value is String))
		{
			throw new ArgumentException
				(Environment.GetResourceString("Arg_MustBeString"));
		}
		else if(value != null)
		{
			return Compare(this, (String)value);
		}
		else
		{
			return 1;
		}
	}

	// Compare this string against another.
	public int CompareTo(String value)
	{
		if(value != null)
		{
			return Compare(this, value);
		}
		else
		{
			return 1;
		}
	}

	// Methods that are supplied by the runtime to assist with string building.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern private static String FastAllocateString(int length);
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern private static void FillString(String dest, int destPos, String src);
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern private static void FillSubstring(String dest, int destPos,
									         String src, int srcPos,
											 int length);

	// Internal helper routines for string concatenation.
	private static String ConcatInternal2(String str1, String str2)
			{
				int str1Len = str1.Length;
				int str2Len = str2.Length;
				String result = FastAllocateString(str1Len + str2Len);
				FillString(result, 0, str1);
				FillString(result, str1Len, str2);
				return result;
			}
	private static String ConcatInternal3(String str1, String str2,
										  String str3)
			{
				int str1Len = str1.Length;
				int str2Len = str2.Length;
				int str3Len = str3.Length;
				String result = FastAllocateString(str1Len + str2Len + str3Len);
				FillString(result, 0, str1);
				FillString(result, str1Len, str2);
				FillString(result, str1Len + str2Len, str3);
				return result;
			}
	private static String ConcatArrayInternal(String[] strings, int len)
			{
				String result = FastAllocateString(len);
				int posn, outposn;
				outposn = 0;
				for(posn = 0; posn < strings.Length; ++posn)
				{
					if(strings[posn] != null)
					{
						FillString(result, outposn, strings[posn]);
						outposn += strings[posn].Length;
					}
				}
				return result;
			}

	// Concatenate strings in various ways.
	public static String Concat(Object obj1)
			{
				if(obj1 != null)
				{
					return obj1.ToString();
				}
				else
				{
					return Empty;
				}
			}
	public static String Concat(String str1, String str2)
			{
				if(str1 != null)
				{
					if(str2 != null)
					{
						return ConcatInternal2(str1, str2);
					}
					else
					{
						return str1;
					}
				}
				else if(str2 != null)
				{
					return str2;
				}
				else
				{
					return Empty;
				}
			}
	public static String Concat(String str1, String str2, String str3)
			{
				if(str1 != null)
				{
					if(str2 != null)
					{
						if(str3 != null)
						{
							return ConcatInternal3(str1, str2, str3);
						}
						else
						{
							return ConcatInternal2(str1, str2);
						}
					}
					else if(str3 != null)
					{
						return ConcatInternal2(str1, str3);
					}
					else
					{
						return str1;
					}
				}
				else if(str2 != null)
				{
					if(str3 != null)
					{
						return ConcatInternal2(str2, str3);
					}
					else
					{
						return str2;
					}
				}
				else if(str3 != null)
				{
					return str3;
				}
				else
				{
					return Empty;
				}
			}
	public static String Concat(String str1, String str2,
								String str3, String str4)
			{
				int str1Len = (str1 != null ? str1.Length : 0);
				int str2Len = (str2 != null ? str2.Length : 0);
				int str3Len = (str3 != null ? str3.Length : 0);
				int str4Len = (str4 != null ? str4.Length : 0);
				String result = FastAllocateString(str1Len + str2Len +
												   str3Len + str4Len);
				if(str1 != null)
				{
					FillString(result, 0, str1);
				}
				if(str2 != null)
				{
					FillString(result, str1Len, str2);
				}
				if(str3 != null)
				{
					FillString(result, str1Len + str2Len, str3);
				}
				if(str4 != null)
				{
					FillString(result, str1Len + str2Len + str3Len, str4);
				}
				return result;
			}
	public static String Concat(params String[] values)
			{
				if(values != null)
				{
					int len = values.Length;
					int posn;
					int strLen = 0;
					for(posn = 0; posn < len; ++posn)
					{
						if(values[posn] != null)
						{
							strLen += values[posn].Length;
						}
					}
					return ConcatArrayInternal(values, strLen);
				}
				else
				{
					throw new ArgumentNullException("values");
				}
			}
	public static String Concat(Object obj1, Object obj2)
			{
				if(obj1 != null)
				{
					if(obj2 != null)
					{
						return Concat(obj1.ToString(), obj2.ToString());
					}
					else
					{
						return obj1.ToString();
					}
				}
				else if(obj2 != null)
				{
					return obj2.ToString();
				}
				else
				{
					return Empty;
				}
			}
	public static String Concat(Object obj1, Object obj2, Object obj3)
			{
				return Concat((obj1 != null ? obj1.ToString() : null),
							  (obj2 != null ? obj2.ToString() : null),
							  (obj3 != null ? obj3.ToString() : null));
			}
	public static String Concat(Object obj1, Object obj2,
								Object obj3, Object obj4)
			{
				return Concat((obj1 != null ? obj1.ToString() : null),
							  (obj2 != null ? obj2.ToString() : null),
							  (obj3 != null ? obj3.ToString() : null),
							  (obj4 != null ? obj4.ToString() : null));
			}
	public static String Concat(params Object[] args)
			{
				if(args != null)
				{
					int len = args.Length;
					String[] strings = new String[len];
					int posn;
					int strLen;
					strLen = 0;
					for(posn = 0; posn < len; ++posn)
					{
						if(args[posn] != null)
						{
							strings[posn] = args[posn].ToString();
							if(strings[posn] != null)
							{
								strLen += strings[posn].Length;
							}
						}
						else
						{
							strings[posn] = null;
						}
					}
					return ConcatArrayInternal(strings, strLen);
				}
				else
				{
					throw new ArgumentNullException("args");
				}
			}

	// Make a copy of a string.
	public static String Copy(String str)
			{
				if(str != null)
				{
					String result = FastAllocateString(str.Length);
					FillString(result, 0, str);
					return result;
				}
				else
				{
					throw new ArgumentNullException("str");
				}
			}

	// Internal version of "CopyTo".
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern private void InternalCopyTo(int sourceIndex, char[] destination,
					   		           int destinationIndex, int count);

	// Copy the contents of a string to an array.
	public void CopyTo(int sourceIndex, char[] destination,
					   int destinationIndex, int count)
			{
				if(sourceIndex < 0 || sourceIndex > length)
				{
					throw new ArgumentOutOfRangeException
						("sourceIndex",
				 		Environment.GetResourceString("ArgRange_StringIndex"));
				}
				else if(destinationIndex < 0 ||
						destinationIndex > destination.Length)
				{
					throw new ArgumentOutOfRangeException
						("destinationIndex",
				 		Environment.GetResourceString("ArgRange_Array"));
				}
				else if((length - sourceIndex) < count ||
						(destination.Length - destinationIndex) < count)
				{
					throw new ArgumentOutOfRangeException
						("count",
				 		Environment.GetResourceString("ArgRange_StringRange"));
				}
				else
				{
					InternalCopyTo(sourceIndex, destination,
								   destinationIndex, count);
				}
			}

	// Determine if this string ends with a particular string.
	public bool EndsWith(String value)
			{
				int valueLen;
				if(value == null)
				{
					throw new ArgumentNullException("value");
				}
				valueLen = value.Length;
				return (valueLen <= length &&
				        Compare(this, length - valueLen,
						  	    value, 0, valueLen) == 0);
			}

	// Override the inherited Equals method.
	public override bool Equals(Object obj)
			{
				if(obj is String)
				{
					return (Compare(this, (String)obj) == 0);
				}
				else
				{
					return false;
				}
			}

	// Determine if two strings are equal.
	public static bool Equals(String a, String b)
			{
				return (Compare(a, b) == 0);
			}

	// Determine if this string is the same as another.
	public bool Equals(String value)
			{
				if(value != null)
				{
					return (Compare(this, value) == 0);
				}
				else
				{
					return false;
				}
			}

	// Format a single-argument string.
	public static String Format(String format, Object arg0)
			{
				return Format((IFormatProvider)null, format, arg0);
			}

	// Format a double-argument string.
	public static String Format(String format, Object arg0, Object arg1)
			{
				return Format((IFormatProvider)null, format, arg0, arg1);
			}

	// Format a triple-argument string.
	public static String Format(String format, Object arg0, Object arg1,
								Object arg2)
			{
				return Format((IFormatProvider)null, format, arg0, arg1, arg2);
			}

	// Format a string that contains a number of argument substitutions.
	public static String Format(String format, params Object[] args)
			{
				return Format((IFormatProvider)null, format, args);
			}

	// Format a string that contains a number of argument substitutions,
	// and using a particular format provider.
	public static String Format(IFormatProvider provider, String format,
								params Object[] args)
			{
				if(format == null)
				{
					throw new ArgumentNullException("format");
				}
				if(args == null)
				{
					throw new ArgumentNullException("args");
				}
				// TODO
				return format;
			}

	// Get an enumerator for this string.
	public CharEnumerator GetEnumerator()
			{
				return new CharEnumerator(this);
			}

	// Implement the IEnumerable interface.
	IEnumerator IEnumerable.GetEnumerator()
			{
				return new CharEnumerator(this);
			}

	// Override the inherited GetHashCode method.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public override int GetHashCode();

	// Get the index of a specific character within the string.
	public int IndexOf(char value)
			{
				return IndexOf(value, 0, length);
			}
	public int IndexOf(char value, int startIndex)
			{
				return IndexOf(value, startIndex, length - startIndex);
			}
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public int IndexOf(char value, int startIndex, int count);

	// Get the index of a specific sub-string within the string.
	public int IndexOf(String value)
			{
				return IndexOf(value, 0, length);
			}
	public int IndexOf(String value, int startIndex)
			{
				return IndexOf(value, startIndex, length - startIndex);
			}
	public int IndexOf(String value, int startIndex, int count)
			{
				// TODO.
				return -1;
			}

	// Get the index of any character within an array.
	public int IndexOfAny(char[] anyOf)
			{
				return IndexOfAny(anyOf, 0, length);
			}
	public int IndexOfAny(char[] anyOf, int startIndex)
			{
				return IndexOfAny(anyOf, startIndex, length - startIndex);
			}
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public int IndexOfAny(char[] anyOf, int startIndex, int count);

	// Insert a string into the current string at a particular position.
	public String Insert(int startIndex, String value)
			{
				int valueLen;
				String result;
				if(value == null)
				{
					throw new ArgumentNullException("value");
				}
				if(startIndex < 0 || startIndex > length)
				{
					throw new ArgumentOutOfRangeException
						("startIndex",
						 Environment.GetResourceString("ArgRange_StringIndex"));
				}
				valueLen = value.Length;
				result = FastAllocateString(length + valueLen);
				FillSubstring(result, 0, this, 0, startIndex);
				FillSubstring(result, startIndex, value, 0, valueLen);
				FillSubstring(result, startIndex + valueLen, this,
							  startIndex, length - startIndex);
				return result;
			}

	// Intern a string.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static String Intern(String str);

	// Determine if a string is intern'ed.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static String IsInterned(String str);

	// Join multiple strings together, delimited by a separator.
	public static String Join(String separator, String[] value)
			{
				if(value == null)
				{
					throw new ArgumentNullException("value");
				}
				else
				{
					return Join(separator, value, 0, value.Length);
				}
			}
	public static String Join(String separator, String[] value,
							  int startIndex, int count)
			{
				int sepLen;
				int resultLen;
				String result;
				String tempStr;
				int posn;

				// Validate the parameters.
				if(value == null)
				{
					return Empty;
				}
				else if(startIndex < 0 || startIndex > value.Length)
				{
					throw new ArgumentOutOfRangeException
						("startIndex",
						 Environment.GetResourceString("ArgRange_Array"));
				}
				else if((value.Length - startIndex) < count)
				{
					throw new ArgumentOutOfRangeException
						("count",
						 Environment.GetResourceString("ArgRange_Array"));
				}

				// Determine the total length of the result string.
				if(separator != null)
				{
					sepLen = separator.Length;
				}
				else
				{
					sepLen = 0;
				}
				resultLen = 0;
				for(posn = 0; posn < count; ++posn)
				{
					if(posn != 0)
					{
						resultLen += sepLen;
					}
					if((tempStr = value[startIndex + posn]) != null)
					{
						resultLen += tempStr.Length;
					}
				}

				// If the final count is zero, then return Empty.
				if(resultLen == 0)
				{
					return Empty;
				}

				// Allocate a new string object and then fill it.
				result = FastAllocateString(resultLen);
				resultLen = 0;
				for(posn = 0; posn < count; ++posn)
				{
					if(posn != 0 && sepLen != 0)
					{
						FillString(result, resultLen, separator);
						resultLen += sepLen;
					}
					if((tempStr = value[startIndex + posn]) != null)
					{
						FillString(result, resultLen, tempStr);
						resultLen += tempStr.Length;
					}
				}
				return result;
			}

	// Get the last index of a specific character within the string.
	public int LastIndexOf(char value)
			{
				return LastIndexOf(value, length, length);
			}
	public int LastIndexOf(char value, int startIndex)
			{
				return LastIndexOf(value, startIndex, startIndex);
			}
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public int LastIndexOf(char value, int startIndex, int count);

	// Get the last index of a specific sub-string within the string.
	public int LastIndexOf(String value)
			{
				return LastIndexOf(value, length, length);
			}
	public int LastIndexOf(String value, int startIndex)
			{
				return LastIndexOf(value, startIndex, startIndex);
			}
	public int LastIndexOf(String value, int startIndex, int count)
			{
				// TODO.
				return -1;
			}

	// Get the last index of any character within an array.
	public int LastIndexOfAny(char[] anyOf)
			{
				return LastIndexOfAny(anyOf, length, length);
			}
	public int LastIndexOfAny(char[] anyOf, int startIndex)
			{
				return LastIndexOfAny(anyOf, startIndex, startIndex);
			}
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public int LastIndexOfAny(char[] anyOf, int startIndex, int count);

	// Fill the contents of a sub-string with a particular character.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern private static void FillChar(String str, int start,
										int count, char ch);

	// Pad a string on the left with spaces to a total width.
	public String PadLeft(int totalWidth)
			{
				return PadLeft(totalWidth, ' ');
			}
	public String PadLeft(int totalWidth, char paddingChar)
			{
				String result;
				if(totalWidth < 0)
				{
					throw new ArgumentException
						(Environment.GetResourceString("ArgRange_NonNegative"));
				}
				else if(totalWidth <= length)
				{
					// Create a new string with a copy of the current.
					result = FastAllocateString(length);
					FillString(result, 0, this);
					return result;
				}
				else
				{
					// Copy the string and pad it.
					result = FastAllocateString(totalWidth);
					FillChar(result, 0, totalWidth - length, paddingChar);
					FillString(result, totalWidth - length, this);
					return result;
				}
			}

	// Pad a string on the right with spaces to a total width.
	public String PadRight(int totalWidth)
			{
				return PadRight(totalWidth, ' ');
			}
	public String PadRight(int totalWidth, char paddingChar)
			{
				String result;
				if(totalWidth < 0)
				{
					throw new ArgumentException
						(Environment.GetResourceString("ArgRange_NonNegative"));
				}
				else if(totalWidth <= length)
				{
					// Create a new string with a copy of the current.
					result = FastAllocateString(length);
					FillString(result, 0, this);
					return result;
				}
				else
				{
					// Copy the string and pad it.
					result = FastAllocateString(totalWidth);
					FillString(result, 0, this);
					FillChar(result, length, totalWidth - length, paddingChar);
					return result;
				}
			}

	// Remove a portion of a string.
	public String Remove(int startIndex, int count)
			{
				String result;
				if(startIndex < 0 || startIndex > length)
				{
					throw new ArgumentOutOfRangeException
						("startIndex",
						 Environment.GetResourceString("ArgRange_StringIndex"));
				}
				else if((length - startIndex) < count)
				{
					throw new ArgumentOutOfRangeException
						("count",
						 Environment.GetResourceString("ArgRange_StringRange"));
				}
				result = FastAllocateString(length - count);
				FillSubstring(result, 0, this, 0, startIndex);
				FillSubstring(result, startIndex, this, startIndex + count,
							  length - (startIndex + count));
				return result;
			}

	// Replace instances of a particular character with another character.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public String Replace(char oldChar, char newChar);

	// Replace instances of a particular string with another string.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public String Replace(String oldValue, String newValue);

	// Split a string into sub-strings that are delimited by a set
	// of sub-characters.
	public String[] Split(params char[] separator)
			{
				return Split(separator, Int32.MaxValue);
			}
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public String[] Split(char[] separator, int count);

	// Determine if this string starts with a particular string.
	public bool StartsWith(String value)
			{
				int valueLen;
				if(value == null)
				{
					throw new ArgumentNullException("value");
				}
				valueLen = value.Length;
				return (valueLen <= length &&
						Compare(this, 0, value, 0, valueLen) == 0);
			}

	// Extract a sub-string.
	public String Substring(int startIndex)
			{
				return Substring(startIndex, length - startIndex);
			}
	public String Substring(int startIndex, int length)
			{
				String result;
				if(startIndex < 0 || startIndex > Length)
				{
					throw new ArgumentOutOfRangeException
						("startIndex",
						 Environment.GetResourceString("ArgRange_StringIndex"));
				}
				else if((Length - startIndex) < length)
				{
					throw new ArgumentOutOfRangeException
						("length",
						 Environment.GetResourceString("ArgRange_StringRange"));
				}
				if(length == 0)
				{
					return Empty;
				}
				else
				{
					result = FastAllocateString(length);
					FillSubstring(result, 0, this, startIndex, length);
					return result;
				}
			}

	// Convert this string into a character array.
	public char[] ToCharArray()
			{
				return ToCharArray(0, length);
			}
	public char[] ToCharArray(int startIndex, int length)
			{
				char[] result;
				if(startIndex < 0 || startIndex > Length)
				{
					throw new ArgumentOutOfRangeException
						("startIndex",
						 Environment.GetResourceString("ArgRange_StringIndex"));
				}
				else if((Length - startIndex) < length)
				{
					throw new ArgumentOutOfRangeException
						("length",
						 Environment.GetResourceString("ArgRange_StringRange"));
				}
				result = new char [length];
				InternalCopyTo(0, result, 0, length);
				return result;
			}

	// Convert a string into lower case.
	public String ToLower()
			{
				return ToLower(CultureInfo.CurrentCulture);
			}
	public String ToLower(CultureInfo culture)
			{
				// TODO
				return this;
			}

	// Convert a string into upper case.
	public String ToUpper()
			{
				return ToUpper(CultureInfo.CurrentCulture);
			}
	public String ToUpper(CultureInfo culture)
			{
				// TODO
				return this;
			}

	// Override the inherited ToString method.
	public override String ToString()
			{
				return this;
			}

	// List of whitespace characters in Unicode.
	private readonly char[] WhitespaceChars =
		{'\u0009', '\u000A', '\u000B', '\u000C', '\u000D', '\u0020',
		 '\u00A0', '\u2001', '\u2002', '\u2003', '\u2004', '\u2005',
		 '\u2006', '\u2007', '\u2008', '\u2009', '\u200A', '\u200B',
		 '\u3000', '\uFEFF'};

	// Types used by "TrimHelper".
	private const int TrimHead = 0;
	private const int TrimTail = 1;
	private const int TrimBoth = 2;

	// Internal helper for trimming whitespace.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern private String TrimHelper(char[] trimChars, int trimType);

	// Trim the whitespace from the front and end of a string.
	public String Trim()
			{
				return TrimHelper(WhitespaceChars, TrimBoth);
			}

	// Trim specific characters from the front and end of a string.
	public String Trim(params char[] trimChars)
			{
				return TrimHelper(trimChars, TrimBoth);
			}

	// Trim specific characters from the end of a string.
	public String TrimEnd(params char[] trimChars)
			{
				return TrimHelper(trimChars, TrimTail);
			}

	// Trim specific characters from the start of a string.
	public String TrimStart(params char[] trimChars)
			{
				return TrimHelper(trimChars, TrimHead);
			}

	// Operators.
	public static bool operator==(String a, String b)
			{
				return Equals(a, b);
			}
	public static bool operator!=(String a, String b)
			{
				return !Equals(a, b);
			}

	// Get the length of the current string.
	public int Length
			{
				get
				{
					return length;
				}
			}

	// Internal version of "this[n]".
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern private char InternalGetChar(int posn);

	// Get a specific character from the current string.
	public char this[int posn]
			{
				get
				{
					return InternalGetChar(posn);
				}
			}

	// Implementation of IConvertible interface.
	public TypeCode GetTypeCode()  { return TypeCode.String; }
	public Object ToType(Type ct)  { return Convert.DefaultToType(this, ct); }
	public Boolean ToBoolean()     { return Convert.ToBoolean(this); }
	public Byte ToByte()           { return Convert.ToByte(this); }
	public SByte ToSByte()         { return Convert.ToSByte(this); }
	public Int16 ToInt16()         { return Convert.ToInt16(this); }
	public UInt16 ToUInt16()	   { return Convert.ToUInt16(this); }
	public Int32 ToInt32()         { return Convert.ToInt32(this); }
	public UInt32 ToUInt32()       { return Convert.ToUInt32(this); }
	public Int64 ToInt64()         { return Convert.ToInt64(this); }
	public UInt64 ToUInt64()       { return Convert.ToUInt64(this); }
	public Char ToChar()           { return Convert.ToChar(this); }
	public Single ToSingle()       { return Convert.ToSingle(this); }
	public Double ToDouble()       { return Convert.ToDouble(this); }
	public Decimal ToDecimal()     { return Convert.ToDecimal(this); }
	public DateTime ToDateTime()   { return Convert.ToDateTime(this); }

}; // class String

}; // namespace System
