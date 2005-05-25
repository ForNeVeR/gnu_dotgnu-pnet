/*
 * String.cs - Implementation of the "System.String" class.
 *
 * Copyright (C) 2001, 2003  Southern Storm Software, Pty Ltd.
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
using System.Runtime.CompilerServices;
using System.Text;

public sealed class String : IComparable, ICloneable, IEnumerable
#if !ECMA_COMPAT
	, IConvertible
#endif
{

	// Internal string state.
	[NonSerialized]
	internal int capacity;			// Total capacity of the string buffer.
	[NonSerialized]
	internal int length;			// Actual length of the string.
	[NonSerialized]
	private char firstChar;			// First character in the string.

	// Private constants
	private static readonly char[] curlyBraces = { '{', '}' };

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
	[CLSCompliant(false)]
	extern unsafe public String(char *value, int startIndex, int length);

	[MethodImpl(MethodImplOptions.InternalCall)]
	[CLSCompliant(false)]
	extern unsafe public String(char *value);

	[MethodImpl(MethodImplOptions.InternalCall)]
	[CLSCompliant(false)]
#if CONFIG_RUNTIME_INFRA
	public
#else
	internal
#endif
	extern unsafe String(sbyte *value, int startIndex,
						 int length, Encoding enc);

	[MethodImpl(MethodImplOptions.InternalCall)]
	[CLSCompliant(false)]
#if ECMA_COMPAT
	internal
#else
	public
#endif
	extern unsafe String(sbyte *value, int startIndex, int length);

	[MethodImpl(MethodImplOptions.InternalCall)]
	[CLSCompliant(false)]
#if ECMA_COMPAT
	internal
#else
	public
#endif
	extern unsafe String(sbyte *value);

	// Implement the ICloneable interface.
	public Object Clone()
			{
				return this;
			}

	// Compare two strings.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static int Compare(String strA, String strB);

	// Internal version of "Compare", with all parameters.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern internal static int CompareInternal
				(String strA, int indexA, int lengthA,
				 String strB, int indexB, int lengthB,
				 bool ignoreCase);

	// Compare two strings while optionally ignoring case.
	public static int Compare(String strA, String strB, bool ignoreCase)
			{
				return CultureInfo.CurrentCulture.CompareInfo
							.Compare(strA, strB,
									 (ignoreCase ? CompareOptions.IgnoreCase
									 			 : CompareOptions.None));
			}

#if !ECMA_COMPAT
	// Compare two strings using a particular culture's comparison rules.
	public static int Compare(String strA, String strB,
							  bool ignoreCase, CultureInfo culture)
			{
				if(culture == null)
				{
					throw new ArgumentNullException("culture");
				}
				return culture.CompareInfo
							.Compare(strA, strB,
									 (ignoreCase ? CompareOptions.IgnoreCase
									 			 : CompareOptions.None));
			}
#endif

	// Validate sub-string ranges for "Compare".
	private static void ValidateCompare(String strA, int indexA,
									    String strB, int indexB,
									    int length)
			{
				if(indexA < 0)
				{
					throw new ArgumentOutOfRangeException
						("indexA", _("ArgRange_StringIndex"));
				}
				if(indexB < 0)
				{
					throw new ArgumentOutOfRangeException
						("indexB", _("ArgRange_StringIndex"));
				}
				if(length < 0)
				{
					throw new ArgumentOutOfRangeException
						("length", _("ArgRange_StringRange"));
				}
				if(strA != null)
				{
					if(indexA >= strA.Length)
					{
						throw new ArgumentOutOfRangeException
							("indexA", _("ArgRange_StringIndex"));
					}
					if(length > (strA.Length - indexA))
					{
						throw new ArgumentOutOfRangeException
							("length", _("ArgRange_StringRange"));
					}
				}
				else
				{
					if(indexA > 0)
					{
						throw new ArgumentOutOfRangeException
							("indexA", _("ArgRange_StringIndex"));
					}
					if(length > 0)
					{
						throw new ArgumentOutOfRangeException
							("length", _("ArgRange_StringRange"));
					}
				}
				if(strB != null)
				{
					if(indexB >= strB.Length)
					{
						throw new ArgumentOutOfRangeException
							("indexB", _("ArgRange_StringIndex"));
					}
					if(length > (strB.Length - indexB))
					{
						throw new ArgumentOutOfRangeException
							("length", _("ArgRange_StringRange"));
					}
				}
				else
				{
					if(indexB > 0)
					{
						throw new ArgumentOutOfRangeException
							("indexB", _("ArgRange_StringIndex"));
					}
					if(length > 0)
					{
						throw new ArgumentOutOfRangeException
							("length", _("ArgRange_StringRange"));
					}
				}
			}

	// Compare two sub-strings.
	public static int Compare(String strA, int indexA,
							  String strB, int indexB,
							  int length)
			{
#if !ECMA_COMPAT
				int lengthA = length;
				int lengthB = length;

				if(strA == null)
				{
					if(indexA != 0)
					{
						throw new ArgumentOutOfRangeException
							("indexA", _("ArgRange_StringIndex"));
					}
					lengthA = 0;
				}
				else
				{
					if(indexA < 0 || indexA >= strA.Length)
					{
						throw new ArgumentOutOfRangeException
							("indexA", _("ArgRange_StringIndex"));
					}
					int adj = strA.Length - indexA - lengthA;
					if(adj < 0)
					{ 
						lengthA += adj;
					}
					if(lengthA < 0)
					{
						lengthA = 0;
					}
				}

				if(strB == null)
				{
					indexB = 0;
					lengthB = 0;
				}
				else
				{
					if(indexB < 0 || indexB >= strB.Length)
					{
						indexB = 0;
						lengthB = 0;
					}
					else
					{
						int adj = strB.Length - indexB - lengthB;
						if(adj < 0)
						{ 
							lengthB += adj;
						}
						if(lengthB < 0)
						{
							lengthB = 0;
						}
					}
				}
				return CultureInfo.CurrentCulture.CompareInfo
							.Compare(strA, indexA, lengthA,
									 strB, indexB, lengthB,
									 CompareOptions.None);
#else
				return CultureInfo.CurrentCulture.CompareInfo
							.Compare(strA, indexA, length,
									 strB, indexB, length,
									 CompareOptions.None);
#endif
			}

	// Compare two sub-strings while optionally ignoring case.
	public static int Compare(String strA, int indexA,
							  String strB, int indexB,
							  int length, bool ignoreCase)
			{
#if !ECMA_COMPAT
				int lengthA = length;
				int lengthB = length;

				if(strA == null)
				{
					if(indexA != 0)
					{
						throw new ArgumentOutOfRangeException
							("indexA", _("ArgRange_StringIndex"));
					}
					lengthA = 0;
				}
				else
				{
					if(indexA < 0 || indexA >= strA.Length)
					{
						throw new ArgumentOutOfRangeException
							("indexA", _("ArgRange_StringIndex"));
					}
					int adj = strA.Length - indexA - lengthA;
					if(adj < 0)
					{ 
						lengthA += adj;
					}
					if(lengthA < 0)
					{
						lengthA = 0;
					}
				}

				if(strB == null)
				{
					indexB = 0;
					lengthB = 0;
				}
				else
				{
					if(indexB < 0 || indexB >= strB.Length)
					{
						indexB = 0;
						lengthB =0;
					}
					else
					{
						int adj = strB.Length - indexB - lengthB;
						if(adj < 0)
						{ 
							lengthB += adj;
						}
						if(lengthB < 0)
						{
							lengthB = 0;
						}
					}
				}
				return CultureInfo.CurrentCulture.CompareInfo
							.Compare(strA, indexA, lengthA,
									 strB, indexB, lengthB,
									 (ignoreCase ? CompareOptions.IgnoreCase
									 			 : CompareOptions.None));
#else
				return CultureInfo.CurrentCulture.CompareInfo
							.Compare(strA, indexA, length,
									 strB, indexB, length,
									 (ignoreCase ? CompareOptions.IgnoreCase
									 			 : CompareOptions.None));
#endif
			}

	// Compare two sub-strings with a particular culture's comparison rules.
#if ECMA_COMPAT
	internal
#else
	public
#endif
	static int Compare(String strA, int indexA,
			  		   String strB, int indexB,
			  		   int length, bool ignoreCase,
			  		   CultureInfo culture)
			{
				if(culture == null)
				{
					throw new ArgumentNullException("culture");
				}
				return culture.CompareInfo
							.Compare(strA, indexA, length,
									 strB, indexB, length,
									 (ignoreCase ? CompareOptions.IgnoreCase
									 			 : CompareOptions.None));
			}

	// Internal version of "CompareOrdinal", with all parameters.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern private static int InternalOrdinal
				(String strA, int indexA, int lengthA,
				 String strB, int indexB, int lengthB);

	// Compare two strings by ordinal character value.
	public static int CompareOrdinal(String strA, String strB)
			{
				return InternalOrdinal
							(strA, 0, ((strA != null) ? strA.Length : 0),
							 strB, 0, ((strB != null) ? strB.Length : 0));
			}

	// Compare two sub-strings by ordinal character value.
	public static int CompareOrdinal(String strA, int indexA,
							         String strB, int indexB,
							         int length)
			{
				ValidateCompare(strA, indexA, strB, indexB, length);
				return InternalOrdinal(strA, indexA, length,
							    	   strB, indexB, length);
			}

	// Implement the IComparable interface.
	public int CompareTo(Object value)
			{
				if(!(value is String))
				{
					throw new ArgumentException(_("Arg_MustBeString"));
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

#if !ECMA_COMPAT
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
#endif

	// Methods that are supplied by the runtime to assist with string building.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern internal static String NewString(int length);
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern internal static String NewBuilder(String value, int length);
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern internal static void Copy(String dest, int destPos, String src);
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern internal static void Copy(String dest, int destPos,
									 String src, int srcPos,
									 int length);

	// Insert or remove space from a string that is being used as a builder.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern internal static void InsertSpace(String str, int srcPos,
											int destPos);
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern internal static void RemoveSpace(String str, int index, int length);

	// Internal helper routines for string concatenation.
	private static String ConcatInternal2(String str1, String str2)
			{
				int str1Len = str1.Length;
				int str2Len = str2.Length;
				String result = NewString(str1Len + str2Len);
				Copy(result, 0, str1);
				Copy(result, str1Len, str2);
				return result;
			}
	private static String ConcatInternal3(String str1, String str2,
										  String str3)
			{
				int str1Len = str1.Length;
				int str2Len = str2.Length;
				int str3Len = str3.Length;
				String result = NewString(str1Len + str2Len + str3Len);
				Copy(result, 0, str1);
				Copy(result, str1Len, str2);
				Copy(result, str1Len + str2Len, str3);
				return result;
			}
	private static String ConcatArrayInternal(String[] strings, int len)
			{
				String result = NewString(len);
				int posn, outposn;
				outposn = 0;
				for(posn = 0; posn < strings.Length; ++posn)
				{
					if(strings[posn] != null)
					{
						Copy(result, outposn, strings[posn]);
						outposn += strings[posn].Length;
					}
				}
				return result;
			}

#if !ECMA_COMPAT
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
#endif

	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static String Concat(String str1, String str2);

	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static String Concat(String str1, String str2, String str3);

#if !ECMA_COMPAT
	public static String Concat(String str1, String str2,
								String str3, String str4)
			{
				int str1Len = (str1 != null ? str1.Length : 0);
				int str2Len = (str2 != null ? str2.Length : 0);
				int str3Len = (str3 != null ? str3.Length : 0);
				int str4Len = (str4 != null ? str4.Length : 0);
				String result = NewString(str1Len + str2Len +
										  str3Len + str4Len);
				if(str1 != null)
				{
					Copy(result, 0, str1);
				}
				if(str2 != null)
				{
					Copy(result, str1Len, str2);
				}
				if(str3 != null)
				{
					Copy(result, str1Len + str2Len, str3);
				}
				if(str4 != null)
				{
					Copy(result, str1Len + str2Len + str3Len, str4);
				}
				return result;
			}
#endif
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
#if !ECMA_COMPAT
	[CLSCompliant(false)]
	public static String Concat(Object obj1, Object obj2,
								Object obj3, Object obj4,
								__arglist)
			{
				ArgIterator iter = new ArgIterator(__arglist);
				String[] list = new String [4 + iter.GetRemainingCount()];
				list[0] = (obj1 != null ? obj1.ToString() : null);
				list[1] = (obj2 != null ? obj2.ToString() : null);
				list[2] = (obj3 != null ? obj3.ToString() : null);
				list[3] = (obj4 != null ? obj4.ToString() : null);
				int posn = 4;
				Object obj;
				while(posn < list.Length)
				{
					obj = TypedReference.ToObject(iter.GetNextArg());
					if(obj != null)
					{
						list[posn] = obj.ToString();
					}
					else
					{
						list[posn] = null;
					}
					++posn;
				}
				return Concat(list);
			}
#endif
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
					String result = NewString(str.Length);
					Copy(result, 0, str);
					return result;
				}
				else
				{
					throw new ArgumentNullException("str");
				}
			}

	// Internal version of "CopyTo".
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern private void CopyToChecked(int sourceIndex, char[] destination,
					   		          int destinationIndex, int count);

	// Copy the contents of a string to an array.
	public void CopyTo(int sourceIndex, char[] destination,
					   int destinationIndex, int count)
			{
				if(destination == null)
				{
					throw new ArgumentNullException("destination");
				}
				if(sourceIndex < 0 || sourceIndex > length)
				{
					throw new ArgumentOutOfRangeException
						("sourceIndex", _("ArgRange_StringIndex"));
				}
				else if(destinationIndex < 0 ||
						destinationIndex > destination.Length)
				{
					throw new ArgumentOutOfRangeException
						("destinationIndex", _("ArgRange_Array"));
				}
				else if((length - sourceIndex) < count ||
						(destination.Length - destinationIndex) < count)
				{
					throw new ArgumentOutOfRangeException
						("count", _("ArgRange_StringRange"));
				}
				else
				{
					CopyToChecked(sourceIndex, destination,
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
				//  Rhys changed the compile so all C# is callvirt.
				//  This code is now unnecessary.  It had been for ECMA
				//  compliance.
				// if(this == null) throw new NullReferenceException(); 
				
				if(obj is String)
				{
					return Equals(this, (String)obj);
				}
				else
				{
					return false;
				}
			}

	// Determine if two strings are equal.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static bool Equals(String a, String b);

#if !ECMA_COMPAT

	// Determine if this string is the same as another.
	public bool Equals(String value)
			{
				return Equals(this, value);
			}

#endif // !ECMA_COMPAT

	// Format a single-argument string.  According to the ECMA spec
	// and the MS documentation, this should throw an exception
	// if "arg0" is null.  However, this is inconsistent with the
	// behaviour of all other implementations, which don't throw null.
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

	// Extract an integer value from a format string.
	private static int GetFormatInteger(String format, int len, ref int posn)
			{
				int temp = posn;
				uint value = 0;
				char ch = format[temp++];
				if(ch < '0' || ch > '9')
				{
					return -1;
				}
				value = ((uint)ch) - ((uint)'0');
				while(temp < len)
				{
					ch = format[temp];
					if(ch < '0' || ch > '9')
					{
						posn = temp;
						return (int)value;
					}
					else
					{
						value = value * ((uint)10) + ((uint)ch) - ((uint)'0');
						if(value >= (uint)0x80000000)
						{
							return -1;
						}
					}
					temp++;
				}
				return -1;
			}

	// Format a string that contains a number of argument substitutions,
	// and using a particular format provider.
	public static String Format(IFormatProvider provider, String format,
								params Object[] args)
			{
				// Validate the parameters.
				if(format == null)
				{
					throw new ArgumentNullException("format");
				}
				if(args == null)
				{
					throw new ArgumentNullException("args");
				}

				// Construct a new string builder.
				StringBuilder sb = new StringBuilder();

				// Search for format specifiers and replace them.
				int len = format.Length;
				int posn = 0;
				int next, argNum, width;
				Object arg;
				String specifier;
				String formatted;
				for(
					next = format.IndexOfAny(curlyBraces, posn, len - posn);
					next != -1;
					next = format.IndexOfAny(curlyBraces, posn, len - posn))
				{
					// Append everything up to this point to the builder.
					sb.Append(format, posn, next - posn);

					// Take care of escape sequences before trying anything
					// fancy...
					if(format[next] == '{' && format[next+1] == '{')
					{
						sb.Append('{');
                        posn = next + 2;
                        continue;
					}

					if(format[next] == '}')
					{
						sb.Append('}');
						posn = next + 1;
						if (format[posn] == '}') posn++;
						continue;
					}
					posn = next + 1;

					// Extract the specifier.
					if(posn >= len)
					{
						throw new FormatException (_("Format_FormatString"));
					}
					
					// Get the argument number.
					argNum = GetFormatInteger(format, len, ref posn);

					if(argNum == -1)
					{
						throw new FormatException (_("Format_FormatString"));
					}

					if(format[posn] == ',')
					{
						++posn;
						if(posn >= len)
						{
							throw new FormatException
												(_("Format_FormatString"));
						}
						if(format[posn] == '-')
						{
							++posn;
							width = GetFormatInteger(format, len, ref posn);
							if(width == -1)
							{
								throw new FormatException
												(_("Format_FormatString"));
							}
							width = -width;
						}
						else
						{
							width = GetFormatInteger(format, len, ref posn);
							if(width == -1)
							{
								throw new FormatException
												(_("Format_FormatString"));
							}
						}
					}
					else
					{
						width = 0;
					}

					if(format[posn] == ':')
					{
						++posn;
						if(posn >= len)
						{
							throw new FormatException
												(_("Format_FormatString"));
						}
						next = format.IndexOf('}', posn, len - posn);
						if(next == -1)
						{
							throw new FormatException
												(_("Format_FormatString"));
						}
						specifier = format.Substring(posn, next - posn);
						posn = next;
					}
					else
					{
						specifier = null;
					}
					if(format[posn] != '}')
					{
						throw new FormatException (_("Format_FormatString"));
					}
					++posn;

					// Get the formatted string version of the argument.
					if(argNum >= args.Length)
					{
						throw new FormatException
												(_("Format_FormatArgNumber"));
					}
					arg = args[argNum];
					if(arg != null)
					{
						if(arg is IFormattable)
						{
							formatted = ((IFormattable)arg).ToString
									(specifier, provider);
						}
						else
						{
							formatted = arg.ToString();
						}
						if(formatted == null)
						{
							formatted = String.Empty;
						}
					}
					else
					{
						formatted = String.Empty;
					}

					// Format the string into place.
					if(width >= 0)
					{
						// Right-justify the string.
						if(width > formatted.Length)
						{
							sb.Append(' ', width - formatted.Length);
						}
						sb.Append(formatted);
					}
					else // width < 0
					{
						// Left-justify the string.
						sb.Append(formatted);
						width = -width;
						if(width > formatted.Length)
						{
							sb.Append(' ', width - formatted.Length);
						}
					}
				} // for (...; next != -1; ...)

				// Append the last non-specifier part to the builder.
				sb.Append(format, posn, len - posn);

				// Convert the builder into a string and return it.
				return sb.ToString();
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
				if(value == null)
				{
					throw new ArgumentNullException("value");
				}
				if(startIndex < 0)
				{
					throw new ArgumentOutOfRangeException
						("startIndex", _("ArgRange_StringIndex"));
				}
				if(count < 0 || (length - startIndex) < count)
				{
					throw new ArgumentOutOfRangeException
						("count", _("ArgRange_StringRange"));
				}
				return FindInRange
					(startIndex, startIndex + count - value.Length, 1, value);
			}

	// Internal helper for string range searching.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern private int FindInRange(int srcFirst, int srcLast,
								   int step, String dest);

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
						("startIndex", _("ArgRange_StringIndex"));
				}
				valueLen = value.Length;
				result = NewString(length + valueLen);
				Copy(result, 0, this, 0, startIndex);
				Copy(result, startIndex, value, 0, valueLen);
				Copy(result, startIndex + valueLen, this,
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
						("startIndex", _("ArgRange_Array"));
				}
				else if(count < 0 || (value.Length - startIndex) < count)
				{
					throw new ArgumentOutOfRangeException
						("count", _("ArgRange_Array"));
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
				result = NewString(resultLen);
				resultLen = 0;
				for(posn = 0; posn < count; ++posn)
				{
					if(posn != 0 && sepLen != 0)
					{
						Copy(result, resultLen, separator);
						resultLen += sepLen;
					}
					if((tempStr = value[startIndex + posn]) != null)
					{
						Copy(result, resultLen, tempStr);
						resultLen += tempStr.Length;
					}
				}
				return result;
			}

	// Get the last index of a specific character within the string.
	public int LastIndexOf(char value)
			{
				if(length==0)
				{
					return -1;
				}
				return LastIndexOf(value, length - 1, length);
			}
	public int LastIndexOf(char value, int startIndex)
			{
				if (startIndex > length)
				{
					throw new ArgumentOutOfRangeException(
								"startIndex", _("ArgRange_StringIndex"));
				}
				return LastIndexOf(value, startIndex, startIndex + 1);
			}
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public int LastIndexOf(char value, int startIndex, int count);

	// Get the last index of a specific sub-string within the string.
	public int LastIndexOf(String value)
			{
				return LastIndexOf(value, length - 1, length);
			}
	public int LastIndexOf(String value, int startIndex)
			{
				if (startIndex >= length)
				{
					throw new ArgumentOutOfRangeException
								("startIndex", _("ArgRange_StringIndex"));
				}
				return LastIndexOf(value, startIndex, startIndex + 1);
			}
	public int LastIndexOf(String value, int startIndex, int count)
			{
				if(value == null)
				{
					throw new ArgumentNullException("value");
				}
				if(startIndex < 0)
				{
					throw new ArgumentOutOfRangeException
						("startIndex", _("ArgRange_StringIndex"));
				}
				if(count < 0 || (startIndex - count) < -1)
				{
					throw new ArgumentOutOfRangeException
						("count", _("ArgRange_StringRange"));
				}

				if (value.length == 0) return 0;

				return FindInRange
					(startIndex - value.Length + 1,
					 startIndex - count + 1, -1, value);
			}

	// Get the last index of any character within an array.
	public int LastIndexOfAny(char[] anyOf)
			{
				return LastIndexOfAny(anyOf, length - 1, length);
			}
	public int LastIndexOfAny(char[] anyOf, int startIndex)
			{
				if (startIndex >= length)
				{
					throw new ArgumentOutOfRangeException
								("startIndex", _("ArgRange_StringIndex"));
				}
				return LastIndexOfAny(anyOf, startIndex, startIndex + 1);
			}
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public int LastIndexOfAny(char[] anyOf, int startIndex, int count);

	// Fill the contents of a sub-string with a particular character.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern internal static void CharFill(String str, int start,
										 int count, char ch);
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern internal static void CharFill(String str, int start, char[] chars,
									     int index, int count);

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
					throw new ArgumentException(_("ArgRange_NonNegative"));
				}
				else if(totalWidth <= length)
				{
					// Create a new string with a copy of the current.
					result = NewString(length);
					Copy(result, 0, this);
					return result;
				}
				else
				{
					// Copy the string and pad it.
					result = NewString(totalWidth);
					CharFill(result, 0, totalWidth - length, paddingChar);
					Copy(result, totalWidth - length, this);
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
					throw new ArgumentException(_("ArgRange_NonNegative"));
				}
				else if(totalWidth <= length)
				{
					// Create a new string with a copy of the current.
					result = NewString(length);
					Copy(result, 0, this);
					return result;
				}
				else
				{
					// Copy the string and pad it.
					result = NewString(totalWidth);
					Copy(result, 0, this);
					CharFill(result, length, totalWidth - length, paddingChar);
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
						("startIndex", _("ArgRange_StringIndex"));
				}
				else if(count < 0 || (length - startIndex) < count)
				{
					throw new ArgumentOutOfRangeException
						("count", _("ArgRange_StringRange"));
				}
				result = NewString(length - count);
				Copy(result, 0, this, 0, startIndex);
				Copy(result, startIndex, this, startIndex + count,
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
	public String[] Split(char[] separator, int count)
			{
				int numStrings;
				int posn, len;
				String[] array;
				int arrayPosn;
				int start;

				// Validate the parameters.
				if(count < 0)
				{
					throw new ArgumentOutOfRangeException
						("count", _("ArgRange_NonNegative"));
				}
				else if(count == 0)
				{
					// Return the entire string in a single-element array.
					array = new String [1];
					array[0] = this;
					return array;
				}

				// Count the number of sub-strings.
				if(separator == null || separator.Length == 0)
				{
					separator = WhitespaceChars;
				}
				numStrings = 1;
				len = Length;
				posn = 0;
				while(posn < len)
				{
					posn = IndexOfAny(separator, posn, len - posn);
					if(posn != -1)
					{
						++numStrings;
						++posn;
					}
					else
					{
						break;
					}
				}

				// Allocate the final array.
				if(numStrings > count)
				{
					array = new String [count];
				}
				else
				{
					array = new String [numStrings];
				}

				// Construct the elements for the array.
				arrayPosn = 0;
				len = Length;
				start = 0;
				posn = 0;
				while(posn < len && arrayPosn < (count-1))
				{
					posn = IndexOfAny(separator, start, len - start);
					if(posn == -1)
					{
						break;
					}
					array[arrayPosn] = Substring(start, posn - start);
					++arrayPosn;
					start = posn + 1;
				}
				array[arrayPosn] = Substring(start);

				// Return the final array to the caller.
				return array;
			}

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
						("startIndex", _("ArgRange_StringIndex"));
				}
				else if(length < 0 || (Length - startIndex) < length)
				{
					throw new ArgumentOutOfRangeException
						("length", _("ArgRange_StringRange"));
				}
				if(length == 0)
				{
					return Empty;
				}
				else
				{
					result = NewString(length);
					Copy(result, 0, this, startIndex, length);
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
						("startIndex", _("ArgRange_StringIndex"));
				}
				else if(length < 0 || (Length - startIndex) < length)
				{
					throw new ArgumentOutOfRangeException
						("length", _("ArgRange_StringRange"));
				}
				result = new char [length];
				CopyToChecked(startIndex, result, 0, length);
				return result;
			}

	// Convert a string into lower case.
	public String ToLower()
			{
				return CultureInfo.CurrentCulture.TextInfo.ToLower(this);
			}
#if !ECMA_COMPAT
	public String ToLower(CultureInfo culture)
			{
				if(culture == null)
				{
					return CultureInfo.CurrentCulture.TextInfo.ToLower(this);
				}
				else
				{
					return culture.TextInfo.ToLower(this);
				}
			}
#endif

	// Convert a string into upper case.
	public String ToUpper()
			{
				return CultureInfo.CurrentCulture.TextInfo.ToUpper(this);
			}
#if !ECMA_COMPAT
	public String ToUpper(CultureInfo culture)
			{
				if(culture == null)
				{
					return CultureInfo.CurrentCulture.TextInfo.ToUpper(this);
				}
				else
				{
					return culture.TextInfo.ToUpper(this);
				}
			}
#endif

	// Override the inherited ToString method.
	public override String ToString()
			{
				return this;
			}

	// Other string conversions.
	public String ToString(IFormatProvider provider)
			{
				return ToString();
			}

	// List of whitespace characters in Unicode.
	private static readonly char[] WhitespaceChars =
		{'\u0009', '\u000A', '\u000B', '\u000C', '\u000D', '\u0020',
		 '\u00A0', '\u2001', '\u2002', '\u2003', '\u2004', '\u2005',
		 '\u2006', '\u2007', '\u2008', '\u2009', '\u200A', '\u200B',
		 '\u3000', '\uFEFF'};

	// Flags used by "Trim".
	private const int TrimFlag_Front = 1;
	private const int TrimFlag_End   = 2;

	// Internal helper for trimming whitespace.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern private String Trim(char[] trimChars, int trimFlags);

	// Trim the whitespace from the front and end of a string.
	public String Trim()
			{
				return Trim(WhitespaceChars, TrimFlag_Front | TrimFlag_End);
			}

	// Trim specific characters from the front and end of a string.
	public String Trim(params char[] trimChars)
			{
				if(trimChars != null)
				{
					return Trim(trimChars, TrimFlag_Front | TrimFlag_End);
				}
				else
				{
					return Trim(WhitespaceChars, TrimFlag_Front | TrimFlag_End);
				}
			}

	// Trim specific characters from the end of a string.
	public String TrimEnd(params char[] trimChars)
			{
				if((trimChars != null) && (trimChars.Length > 0))
				{
					return Trim(trimChars, TrimFlag_End);
				}
				else
				{
					return Trim(WhitespaceChars, TrimFlag_End);
				}
			}

	// Trim specific characters from the start of a string.
	public String TrimStart(params char[] trimChars)
			{
				if((trimChars != null) && (trimChars.Length > 0))
				{
					return Trim(trimChars, TrimFlag_Front);
				}
				else
				{
					return Trim(WhitespaceChars, TrimFlag_Front);
				}
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
	extern internal char GetChar(int posn);
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern internal void SetChar(int posn, char value);

	// Get a specific character from the current string.
	// The "IndexerName" attribute ensures that the get
	// accessor method is named according to the ECMA spec.
	[IndexerName("Chars")]
	public char this[int posn]
			{
				get
				{
					return GetChar(posn);
				}
			}

#if !ECMA_COMPAT

	// Implementation of the IConvertible interface.
	public TypeCode GetTypeCode()
			{
				return TypeCode.String;
			}
	bool IConvertible.ToBoolean(IFormatProvider provider)
			{
				return Convert.ToBoolean(this);
			}
	byte IConvertible.ToByte(IFormatProvider provider)
			{
				return Convert.ToByte(this);
			}
	sbyte IConvertible.ToSByte(IFormatProvider provider)
			{
				return Convert.ToSByte(this);
			}
	short IConvertible.ToInt16(IFormatProvider provider)
			{
				return Convert.ToInt16(this);
			}
	ushort IConvertible.ToUInt16(IFormatProvider provider)
			{
				return Convert.ToUInt16(this);
			}
	char IConvertible.ToChar(IFormatProvider provider)
			{
				return Convert.ToChar(this);
			}
	int IConvertible.ToInt32(IFormatProvider provider)
			{
				return Convert.ToInt32(this);
			}
	uint IConvertible.ToUInt32(IFormatProvider provider)
			{
				return Convert.ToUInt32(this);
			}
	long IConvertible.ToInt64(IFormatProvider provider)
			{
				return Convert.ToInt64(this);
			}
	ulong IConvertible.ToUInt64(IFormatProvider provider)
			{
				return Convert.ToUInt64(this);
			}
	float IConvertible.ToSingle(IFormatProvider provider)
			{
				return Convert.ToSingle(this);
			}
	double IConvertible.ToDouble(IFormatProvider provider)
			{
				return Convert.ToDouble(this);
			}
	Decimal IConvertible.ToDecimal(IFormatProvider provider)
			{
				return Convert.ToDecimal(this);
			}
	DateTime IConvertible.ToDateTime(IFormatProvider provider)
			{
				return Convert.ToDateTime(this);
			}
	Object IConvertible.ToType(Type conversionType, IFormatProvider provider)
			{
				return Convert.DefaultToType(this, conversionType,
											 provider, true);
			}

#endif // !ECMA_COMPAT

}; // class String

}; // namespace System
