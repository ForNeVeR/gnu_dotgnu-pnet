/*
 * UInt32.cs - Implementation of the "System.UInt32" class.
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

using System.Private;
using System.Globalization;

public struct UInt32 : IComparable, IFormattable, IConvertible
{
	private uint value__;

	public const uint MaxValue = 0xFFFFFFFF;
	public const uint MinValue = 0;
	public const uint Empty    = 0;

	// Override inherited methods.
	public override int GetHashCode()
			{
				return unchecked(((int)value__) & 0x7FFFFFFF);
			}
	public override bool Equals(Object value)
			{
				if(value is UInt32)
				{
					return (value__ == ((UInt32)value).value__);
				}
				else
				{
					return false;
				}
			}

	// String conversion.
	public override String ToString()
			{ return Format(value__, null, NumberFormatInfo.InvariantInfo); }
	public static String ToString(uint value)
			{ return Format(value, null, NumberFormatInfo.InvariantInfo); }

	// Formatting methods.
	public static String Format(uint value, String format,
								NumberFormatInfo nfi)
			{
				return NumberFormatter.FormatFixedPoint((ulong)value, 0, 0,
														false, format, nfi);
			}
	public String Format(String format, IServiceObjectProvider isop)
			{
				return NumberFormatter.FormatFixedPoint((ulong)value__, 0, 0,
														false, format, isop);
			}
	public static String Format(uint value, String format)
			{
				return NumberFormatter.FormatFixedPoint((ulong)value, 0, 0,
														false, format, null);
			}

	// Parsing methods.
	public static uint FromString(String s)
			{
				return Parse(s, NumberStyles.Integer,
							 NumberFormatInfo.InvariantInfo);
			}
	public static uint Parse(String s, NumberStyles style,
							 NumberFormatInfo nfi)
			{
				NumberParser.ValidateIntegerStyle(style);
				return NumberParser.ParseUInt32(s, style, nfi, 0);
			}
	public static uint Parse(String s)
			{
				return Parse(s, NumberStyles.Integer, null);
			}
	public static uint Parse(String s, NumberStyles style)
			{
				return Parse(s, style, null);
			}

	// Implementation of the IComparable interface.
	public int CompareTo(Object value)
			{
				if(value != null)
				{
					if(!(value is UInt32))
					{
						throw new ArgumentException
							(Environment.GetResourceString("Arg_MustBeUInt32"));
					}
					uint value2 = ((UInt32)value).value__;
					if(value__ < value2)
					{
						return -1;
					}
					else if(value__ > value2)
					{
						return 1;
					}
					else
					{
						return 0;
					}
				}
				else
				{
					return 1;
				}
			}

	// Implementation of IConvertible interface.
	public TypeCode GetTypeCode() { return TypeCode.UInt32; }
	public Object ToType(Type ct) { return Convert.DefaultToType(this, ct); }
	public Boolean ToBoolean()     { return Convert.ToBoolean(value__); }
	public Byte ToByte()           { return Convert.ToByte(value__); }
	public SByte ToSByte()         { return Convert.ToSByte(value__); }
	public Int16 ToInt16()         { return Convert.ToInt16(value__); }
	public UInt16 ToUInt16()	   { return Convert.ToUInt16(value__); }
	public Int32 ToInt32()         { return Convert.ToInt32(value__); }
	public UInt32 ToUInt32()       { return value__; }
	public Int64 ToInt64()         { return Convert.ToInt64(value__); }
	public UInt64 ToUInt64()       { return Convert.ToUInt64(value__); }
	public Char ToChar()           { return Convert.ToChar(value__); }
	public Single ToSingle()       { return Convert.ToSingle(value__); }
	public Double ToDouble()       { return Convert.ToDouble(value__); }
	public Decimal ToDecimal()     { return Convert.ToDecimal(value__); }
	public DateTime ToDateTime()
			{
				throw new InvalidCastException
					(String.Format
						(Environment.GetResourceString("InvalidCast_FromTo"),
		 			     "UInt32", "DateTime"));
			}

}; // class UInt32

}; // namespace System
