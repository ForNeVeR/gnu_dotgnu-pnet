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
			{
				return NumberFormatter.FormatFixedPoint
							((ulong)value__, 0, 0, false, null,
							 NumberFormatInfo.CurrentInfo);
			}
	public String ToString(String format)
			{
				return NumberFormatter.FormatFixedPoint
							((ulong)value__, 0, 0, false, format,
							 NumberFormatInfo.CurrentInfo);
			}
	public String ToString(String format, IFormatProvider provider)
			{
				return NumberFormatter.FormatFixedPoint
							((ulong)value__, 0, 0, false, format,
							 NumberFormatInfo.GetInstance(provider));
			}

	// Parsing methods.
	public static byte Parse(String s, NumberStyles style,
							 IFormatProvider provider)
			{
				NumberParser.ValidateIntegerStyle(style);
				return Convert.ToByte(NumberParser.ParseUInt32
					(s, style, NumberFormatInfo.GetInstance(provider), 256));
			}
	public static byte Parse(String s)
			{
				return Parse(s, NumberStyles.Integer, null);
			}
	public static byte Parse(String s, IFormatProvider provider)
			{
				return Parse(s, NumberStyles.Integer, provider);
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
						throw new ArgumentException(_("Arg_MustBeByte"));
					}
					return ((int)value__) - ((int)((Byte)value).value__);
				}
				else
				{
					return 1;
				}
			}

	// Implementation of the IConvertible interface.
	public TypeCode GetTypeCode()
			{
				return TypeCode.Byte;
			}
	bool IConvertible.ToBoolean(IFormatProvider provider)
			{
				return Convert.ToBoolean(value__);
			}
	byte IConvertible.ToByte(IFormatProvider provider)
			{
				return value__;
			}
	sbyte IConvertible.ToSByte(IFormatProvider provider)
			{
				return Convert.ToSByte(value__);
			}
	short IConvertible.ToInt16(IFormatProvider provider)
			{
				return Convert.ToInt16(value__);
			}
	ushort IConvertible.ToUInt16(IFormatProvider provider)
			{
				return Convert.ToUInt16(value__);
			}
	char IConvertible.ToChar(IFormatProvider provider)
			{
				return Convert.ToChar(value__);
			}
	int IConvertible.ToInt32(IFormatProvider provider)
			{
				return Convert.ToInt32(value__);
			}
	uint IConvertible.ToUInt32(IFormatProvider provider)
			{
				return Convert.ToUInt32(value__);
			}
	long IConvertible.ToInt64(IFormatProvider provider)
			{
				return Convert.ToInt64(value__);
			}
	ulong IConvertible.ToUInt64(IFormatProvider provider)
			{
				return Convert.ToUInt64(value__);
			}
	float IConvertible.ToSingle(IFormatProvider provider)
			{
				return Convert.ToSingle(value__);
			}
	double IConvertible.ToDouble(IFormatProvider provider)
			{
				return Convert.ToDouble(value__);
			}
	Decimal IConvertible.ToDecimal(IFormatProvider provider)
			{
				return Convert.ToDecimal(value__);
			}
	DateTime IConvertible.ToDateTime(IFormatProvider provider)
			{
				throw new InvalidCastException
					(String.Format
						(_("InvalidCast_FromTo"), "Byte", "DateTime"));
			}
	public String ToString(IFormatProvider provider)
			{
				return NumberFormatter.FormatFixedPoint
							((ulong)value__, 0, 0, false, null,
							 NumberFormatInfo.GetInstance(provider));
			}
	Object IConvertible.ToType(Type conversionType, IFormatProvider provider)
			{
				return Convert.DefaultToType(this, conversionType, provider);
			}

}; // class Byte

}; // namespace System
