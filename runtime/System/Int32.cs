/*
 * Int32.cs - Implementation of the "System.Int32" class.
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

public struct Int32 : IComparable, IFormattable, IConvertible
{
	private int value__;

	public const int MaxValue = 0x7FFFFFFF;
	public const int MinValue = unchecked((int)(-0x80000000));
	public const int Empty    = 0;

	// Override inherited methods.
	public override int GetHashCode()
			{ return (value__ & 0x7FFFFFFF); }
	public override bool Equals(Object value)
			{
				if(value is Int32)
				{
					return (value__ == ((Int32)value).value__);
				}
				else
				{
					return false;
				}
			}

	// String conversion.
	public override String ToString()
			{ return Format(value__, null, NumberFormatInfo.InvariantInfo); }
	public static String ToString(int value)
			{ return Format(value, null, NumberFormatInfo.InvariantInfo); }

	// Formatting methods.
	public static String Format(int value, String format,
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
									((ulong)(uint)(-value), 0, 0,
									 true, format, nfi);
					}
				}
			}
	public String Format(String format, IServiceObjectProvider isop)
			{
				return Format(value__, format,
							  NumberFormatInfo.GetInstance(isop));
			}
	public static String Format(int value, String format)
			{
				return Format(value, format, null);
			}

	// Parsing methods.
	public static int FromString(String s)
			{
				return Parse(s, NumberStyles.Integer,
							 NumberFormatInfo.InvariantInfo);
			}
	public static int Parse(String s, NumberStyles style,
							NumberFormatInfo nfi)
			{
				NumberParser.ValidateIntegerStyle(style);
				return NumberParser.ParseInt32(s, style, nfi, 0);
			}
	public static int Parse(String s)
			{
				return Parse(s, NumberStyles.Integer, null);
			}
	public static int Parse(String s, NumberStyles style)
			{
				return Parse(s, style, null);
			}

	// Implementation of the IComparable interface.
	public int CompareTo(Object value)
			{
				if(value != null)
				{
					if(!(value is Int32))
					{
						throw new ArgumentException
							(Environment.GetResourceString("Arg_MustBeInt32"));
					}
					return ((int)value__) - ((int)((Int32)value).value__);
				}
				else
				{
					return 1;
				}
			}

	// Implementation of IConvertible interface.
	public TypeCode GetTypeCode() { return TypeCode.Int32; }
	public Object ToType(Type ct) { return Convert.DefaultToType(this, ct); }
	public Boolean ToBoolean()     { return Convert.ToBoolean(value__); }
	public Byte ToByte()           { return Convert.ToByte(value__); }
	public SByte ToSByte()         { return Convert.ToSByte(value__); }
	public Int16 ToInt16()         { return Convert.ToInt16(value__); }
	public UInt16 ToUInt16()	   { return Convert.ToUInt16(value__); }
	public Int32 ToInt32()         { return value__; }
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
		 			     "Int32", "DateTime"));
			}

}; // class Int32

}; // namespace System
