/*
 * Byte.cs - Implementation of the "System.Byte" class.
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

public struct Byte : IComparable, IFormattable, IConvertible
{
	private byte value__;

	public const byte MaxValue = 255;
	public const byte MinValue = 0;
	public const byte Empty    = 0;

	// Override inherited methods.
	public override int GetHashCode()
			{ return (int)value__; }
	public override bool Equals(Object value)
			{
				if(value is Byte)
				{
					return (value__ == ((Byte)value).value__);
				}
				else
				{
					return false;
				}
			}

	// String conversion.
	public override String ToString()
			{ return Format(value__, null, NumberFormatInfo.InvariantInfo); }
	public static String ToString(byte value)
			{ return Format(value, null, NumberFormatInfo.InvariantInfo); }

	// Formatting methods.
	public static String Format(byte value, String format,
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
	public static String Format(byte value, String format)
			{
				return NumberFormatter.FormatFixedPoint((ulong)value, 0, 0,
														false, format, null);
			}

	// Parsing methods.
	public static byte FromString(String s)
			{
				return Parse(s, NumberStyles.Integer,
							 NumberFormatInfo.InvariantInfo);
			}
	public static byte Parse(String s, NumberStyles style,
							 NumberFormatInfo nfi)
			{
				NumberParser.ValidateIntegerStyle(style);
				return Convert.ToByte(NumberParser.ParseUInt32
										(s, style, nfi, 256));
			}
	public static byte Parse(String s)
			{
				return Parse(s, NumberStyles.Integer, null);
			}
	public static byte Parse(String s, NumberStyles style)
			{
				return Parse(s, style, null);
			}

	// Implementation of the IComparable interface.
	public int CompareTo(Object value)
			{
				if(value != null)
				{
					if(!(value is Byte))
					{
						throw new ArgumentException
							(Environment.GetResourceString("Arg_MustBeByte"));
					}
					return ((int)value__) - ((int)((Byte)value).value__);
				}
				else
				{
					return 1;
				}
			}

	// Implementation of IConvertible interface.
	public TypeCode GetTypeCode() { return TypeCode.Byte; }
	public Object ToType(Type ct) { return Convert.DefaultToType(this, ct); }
	public Boolean ToBoolean()     { return Convert.ToBoolean(value__); }
	public Byte ToByte()           { return value__; }
	public SByte ToSByte()         { return Convert.ToSByte(value__); }
	public Int16 ToInt16()         { return Convert.ToInt16(value__); }
	public UInt16 ToUInt16()	   { return Convert.ToUInt16(value__); }
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
		 			     "Byte", "DateTime"));
			}

}; // class Byte

}; // namespace System
