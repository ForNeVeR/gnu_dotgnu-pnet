/*
 * UInt16.cs - Implementation of the "System.UInt16" class.
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

public struct UInt16 : IComparable, IFormattable, IConvertible
{
	private ushort value__;

	public const ushort MaxValue = 65535;
	public const ushort MinValue = 0;
	public const ushort Empty    = 0;

	// Override inherited methods.
	public override int GetHashCode()
			{ return (int)value__; }
	public override bool Equals(Object value)
			{
				if(value is UInt16)
				{
					return (value__ == ((UInt16)value).value__);
				}
				else
				{
					return false;
				}
			}

	// String conversion.
	public override String ToString()
			{ return Format(value__, null, NumberFormatInfo.InvariantInfo); }
	public static String ToString(ushort value)
			{ return Format(value, null, NumberFormatInfo.InvariantInfo); }

	// Formatting methods.
	public static String Format(ushort value, String format,
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
	public static String Format(ushort value, String format)
			{
				return NumberFormatter.FormatFixedPoint((ulong)value, 0, 0,
														false, format, null);
			}

	// Parsing methods.
	public static ushort FromString(String s)
			{
				return Parse(s, NumberStyles.Integer,
							 NumberFormatInfo.InvariantInfo);
			}
	public static ushort Parse(String s, NumberStyles style,
							   NumberFormatInfo nfi)
			{
				NumberParser.ValidateIntegerStyle(style);
				return Convert.ToUInt16(NumberParser.ParseUInt32
										(s, style, nfi, 65536));
			}
	public static ushort Parse(String s)
			{
				return Parse(s, NumberStyles.Integer, null);
			}
	public static ushort Parse(String s, NumberStyles style)
			{
				return Parse(s, style, null);
			}

	// Implementation of the IComparable interface.
	public int CompareTo(Object value)
			{
				if(value != null)
				{
					if(!(value is UInt16))
					{
						throw new ArgumentException
							(Environment.GetResourceString("Arg_MustBeUInt16"));
					}
					return ((int)value__) - ((int)((UInt16)value).value__);
				}
				else
				{
					return 1;
				}
			}

	// Implementation of IConvertible interface.
	public TypeCode GetTypeCode() { return TypeCode.UInt16; }
	public Object ToType(Type ct) { return Convert.DefaultToType(this, ct); }
	public Boolean ToBoolean()     { return Convert.ToBoolean(value__); }
	public Byte ToByte()           { return Convert.ToByte(value__); }
	public SByte ToSByte()         { return Convert.ToSByte(value__); }
	public Int16 ToInt16()         { return Convert.ToInt16(value__); }
	public UInt16 ToUInt16()	   { return value__; }
	public Int32 ToInt32()         { return Convert.ToInt32(value__); }
	public UInt32 ToUInt32()       { return Convert.ToUInt32(value__); }
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
		 			     "UInt16", "DateTime"));
			}

}; // class UInt16

}; // namespace System
