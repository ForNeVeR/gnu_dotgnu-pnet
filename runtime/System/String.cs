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

public class String
{

// TO DO

	public static readonly String Empty = "";

	public String(char[] array, int index, int len)
			{
			}

	public static String Format(String format, String arg0)
			{ return format; }
	public static String Format(String format, String arg0, String arg1)
			{ return format; }

	public static String Join(String separator, String[] strings)
			{
				return strings[0];
			}

	public int Length
			{
				get
				{
					return 0;
				}
			}

	public char this[int posn]
			{
				get
				{
					return ' ';
				}
			}

	public String Trim() { return this; }

	public static int Compare(String str1, String str2, bool ignoreCase)
			{
				return -1;
			}

	// Internal helper routines for string concatenation.
	private static String ConcatInternal(String str1, String str2)
			{
				// TODO
				return str1;
			}
	private static String ConcatArrayInternal(String[] strings, int len)
			{
				// TODO
				return strings[0];
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
						return ConcatInternal(str1, str2);
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
							// TODO
							return str1;
						}
						else
						{
							return ConcatInternal(str1, str2);
						}
					}
					else if(str3 != null)
					{
						return ConcatInternal(str1, str3);
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
						return ConcatInternal(str2, str3);
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
				// TODO
				return Concat(Concat(str1, str2), Concat(str3, str4));
			}
	public static String Concat(String[] values)
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
				// TODO
				return Concat(Concat(obj1, obj2), Concat(obj3, obj4));
			}
	public static String Concat(Object[] args)
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

}; // class String

}; // namespace System
