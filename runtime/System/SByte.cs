/*
 * SByte.cs - Implementation of the "System.SByte" class.
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

public struct SByte : IComparable, IFormattable, IConvertible
{
	private sbyte value__;

	public const sbyte MaxValue = 127;
	public const sbyte MinValue = -128;
	public const sbyte Empty    = 0;

	// Override inherited methods.
	public override int GetHashCode()
			{
				return unchecked((((int)value__) ^ (((int)value__) << 8)));
			}
	public override bool Equals(Object value)
			{
				if(value is SByte)
				{
					return (value__ == ((SByte)value).value__);
				}
				else
				{
					return false;
				}
			}

	// String conversion.
	public override String ToString()
			{ return Format(value__, null, NumberFormatInfo.InvariantInfo); }
	public static String ToString(sbyte value)
			{ return Format(value, null, NumberFormatInfo.InvariantInfo); }

	// Formatting methods.
	public static String Format(sbyte value, String format,
								NumberFormatInfo nfi)
			{
				unchecked
				{
					if(value >= 0)
					{
						return NumberFormatter.FormatFixedPoint
									((ulong)value, 0, 0, false, format, nfi);
					}
					else
					{
						return NumberFormatter.FormatFixedPoint
									((ulong)(-((int)value)), 0, 0,
									 true, format, nfi);
					}
				}
			}
	public String Format(String format, IServiceObjectProvider isop)
			{
				return Format(value__, format,
							  NumberFormatInfo.GetInstance(isop));
			}
	public static String Format(sbyte value, String format)
			{
				return Format(value, format, null);
			}

	// Parsing methods.
	public static sbyte FromString(String s)
			{
				return Parse(s, NumberStyles.Integer,
							 NumberFormatInfo.InvariantInfo);
			}
	public static sbyte Parse(String s, NumberStyles style,
							  NumberFormatInfo nfi)
			{
				NumberParser.ValidateIntegerStyle(style);
				return Convert.ToSByte(NumberParser.ParseInt32
										(s, style, nfi, 128));
			}
	public static sbyte Parse(String s)
			{
				return Parse(s, NumberStyles.Integer, null);
			}
	public static sbyte Parse(String s, NumberStyles style)
			{
				return Parse(s, style, null);
			}

	// Implementation of the IComparable interface.
	public int CompareTo(Object value)
			{
				if(value != null)
				{
					if(!(value is SByte))
					{
						throw new ArgumentException
							(Environment.GetResourceString("Arg_MustBeSByte"));
					}
					return ((int)value__) - ((int)((SByte)value).value__);
				}
				else
				{
					return 1;
				}
			}

	// Implementation of IConvertible interface.
	public TypeCode GetTypeCode() { return TypeCode.SByte; }
	public Object ToType(Type ct) { return Convert.DefaultToType(this, ct); }
	public Boolean ToBoolean()     { return Convert.ToBoolean(value__); }
	public Byte ToByte()           { return Convert.ToByte(value__); }
	public SByte ToSByte()         { return value__; }
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
		 			     "SByte", "DateTime"));
			}

}; // class SByte

}; // namespace System
