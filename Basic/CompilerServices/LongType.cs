/*
 * LongType.cs - Implementation of the
 *			"Microsoft.VisualBasic.LongType" class.
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

[StandardModule]
#if CONFIG_COMPONENT_MODEL
[EditorBrowsable(EditorBrowsableState.Never)]
#endif
public sealed class LongType
{
	// This class cannot be instantiated.
	private LongType() {}

	// Convert a decimal value into a long.
	public static long DecimalToLong(IConvertible ValueInterface)
			{
				return Convert.ToInt64(ValueInterface.ToDecimal(null));
			}

	// Convert an object into a long value.
	public static long FromObject(Object Value)
			{
				if(Value != null)
				{
					IConvertible ic = (Value as IConvertible);
					if(ic != null)
					{
						if(ic.GetTypeCode() != TypeCode.String)
						{
							return ic.ToInt64(null);
						}
						else
						{
							return FromString(ic.ToString(null));
						}
					}
					else
					{
						throw new InvalidCastException
							(String.Format
								(S._("VB_InvalidCast"),
								 Value.GetType(), "System.Int64"));
					}
				}
				else
				{
					return 0;
				}
			}

	// Try to convert a value using hex or octal forms into "long".
	internal static bool TryConvertHexOct(String str, out long value)
			{
				int len = str.Length;
				int posn = 0;
				char ch;
				while(posn < len)
				{
					ch = str[posn];
					if(ch == '&')
					{
						break;
					}
					else if(ch == ' ' || ch == '\u3000')
					{
						++posn;
					}
					else
					{
						value = 0;
						return false;
					}
				}
				++posn;
				if(posn >= len)
				{
					value = 0;
					return false;
				}
				str = Utils.FixDigits(str);
				ch = str[posn];
				if(ch == 'h' || ch == 'H')
				{
					// Recognize a hexadecimal value.
					value = Convert.ToInt64(str.Substring(posn + 1), 16);
					return true;
				}
				else if(ch == 'o' || ch == 'O')
				{
					// Recognize an octal value.
					value = Convert.ToInt64(str.Substring(posn + 1), 8);
					return true;
				}
				else
				{
					// Not a legitimate number format.
					throw new FormatException(S._("VB_InvalidHexOrOct"));
				}
			}

	// Convert a string into a long value.
	public static long FromString(String Value)
			{
				if(Value != null)
				{
					try
					{
						long lvalue;
						if(TryConvertHexOct(Value, out lvalue))
						{
							return lvalue;
						}
						return Convert.ToInt64
							(Math.Round(DoubleType.Parse(Value)));
					}
					catch(OverflowException)
					{
						throw new InvalidCastException
							(String.Format
								(S._("VB_InvalidCast"),
								 "System.String", "System.Int64"));
					}
				}
				else
				{
					return 0;
				}
			}

}; // class LongType

}; // namespace Microsoft.VisualBasic.CompilerServices
