/*
 * Strings.cs - Implementation of the
 *			"Microsoft.VisualBasic.Strings" class.
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
using System.ComponentModel;
using System.Globalization;
using System.Runtime.InteropServices;
using Microsoft.VisualBasic.CompilerServices;

[StandardModule]
public sealed class Strings
{
	// This class cannot be instantiated.
	private Strings() {}

	// Get a character code.  In our world, everything is Unicode.
	public static int Asc(char String)
			{
				return (int)String;
			}
	public static int Asc(String String)
			{
				if(String == null || String.Length == 0)
				{
					throw new ArgumentException(S._("VB_EmptyString"));
				}
				return (int)(String[0]);
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
				return (char)CharCode;
			}
	public static char ChrW(int CharCode)
			{
				return (char)CharCode;
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

}; // class Strings

}; // namespace Microsoft.VisualBasic
