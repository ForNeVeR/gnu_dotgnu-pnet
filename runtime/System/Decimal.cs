/*
 * Decimal.cs - Implementation of the "System.Decimal" class.
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
using System.Runtime.CompilerServices;

public struct Decimal : IComparable, IFormattable, IConvertible
{
	private int flags, high, middle, low;

	public static readonly decimal Zero = 0.0m;
	public static readonly decimal One = 1.0m;
	public static readonly decimal MinusOne = -1.0m;
	public static readonly decimal MaxValue = new Decimal(-1, -1, -1, true, 0);
	public static readonly decimal MinValue = new Decimal(-1, -1, -1, false, 0);
	private const int ScaleShift = 16;

	// Public routines that are imported from the runtime engine.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static decimal Add(decimal x, decimal y);
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static int Compare(decimal x, decimal y);
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static decimal Divide(decimal x, decimal y);
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static decimal Floor(decimal x);
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static decimal Remainder(decimal x, decimal y);
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static decimal Multiply(decimal x, decimal y);
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static decimal Negate(decimal x);
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static decimal Round(decimal x, int decimals);
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static decimal Subtract(decimal x, decimal y);
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static decimal Truncate(decimal x);

	// Public routines that can be implemented in IL bytecode.
	public static decimal Abs(decimal value)
			{
				return new Decimal(value.low, value.middle,
								   value.high, value.flags & 0x7FFFFFFF);
			}
	public static bool Equals(decimal x, decimal y)
			{
				return (Compare(x, y) == 0);
			}
	public static decimal Max(decimal x, decimal y)
			{
				if(Compare(x, y) > 0)
					return x;
				else
					return y;
			}
	public static decimal Min(decimal x, decimal y)
			{
				if(Compare(x, y) < 0)
					return x;
				else
					return y;
			}
	[Obsolete("Use Decimal.Remainder instead")]
	public static decimal Mod(decimal x, decimal y)
			{
				return Remainder(x, y);
			}

	// Declared operators.
	public static decimal operator++(decimal x)
				{ return x + 1.0m; }
	public static decimal operator--(decimal x)
				{ return x - 1.0m; }
	public static decimal operator+(decimal x)
				{ return x; }
	public static decimal operator-(decimal x)
				{ return Negate(x); }
	public static decimal operator*(decimal x, decimal y)
				{ return Multiply(x, y); }
	public static decimal operator/(decimal x, decimal y)
				{ return Divide(x, y); }
	public static decimal operator%(decimal x, decimal y)
				{ return Remainder(x, y); }
	public static decimal operator+(decimal x, decimal y)
				{ return Add(x, y); }
	public static decimal operator-(decimal x, decimal y)
				{ return Subtract(x, y); }
	public static bool operator==(decimal x, decimal y)
				{ return (Compare(x, y) == 0); }
	public static bool operator!=(decimal x, decimal y)
				{ return (Compare(x, y) != 0); }
	public static bool operator<(decimal x, decimal y)
				{ return (Compare(x, y) < 0); }
	public static bool operator>(decimal x, decimal y)
				{ return (Compare(x, y) > 0); }
	public static bool operator<=(decimal x, decimal y)
				{ return (Compare(x, y) <= 0); }
	public static bool operator>=(decimal x, decimal y)
				{ return (Compare(x, y) >= 0); }
	public static implicit operator decimal(sbyte x)
				{ return new Decimal((int)x); }
	public static implicit operator decimal(byte x)
				{ return new Decimal((uint)x); }
	public static implicit operator decimal(short x)
				{ return new Decimal((int)x); }
	public static implicit operator decimal(ushort x)
				{ return new Decimal((uint)x); }
	public static implicit operator decimal(int x)
				{ return new Decimal(x); }
	public static implicit operator decimal(uint x)
				{ return new Decimal(x); }
	public static implicit operator decimal(long x)
				{ return new Decimal(x); }
	public static implicit operator decimal(ulong x)
				{ return new Decimal(x); }
	public static implicit operator decimal(char x)
				{ return new Decimal((uint)(ushort)x); }
	public static explicit operator decimal(float x)
				{ return new Decimal(x); }
	public static explicit operator decimal(double x)
				{ return new Decimal(x); }
	public static explicit operator sbyte(decimal x)
				{ return ToSByte(x); }
	public static explicit operator byte(decimal x)
				{ return ToByte(x); }
	public static explicit operator short(decimal x)
				{ return ToInt16(x); }
	public static explicit operator ushort(decimal x)
				{ return ToUInt16(x); }
	public static explicit operator int(decimal x)
				{ return ToInt32(x); }
	public static explicit operator uint(decimal x)
				{ return ToUInt32(x); }
	public static explicit operator long(decimal x)
				{ return ToInt64(x); }
	public static explicit operator ulong(decimal x)
				{ return ToUInt64(x); }
	public static explicit operator char(decimal x)
				{ return (char)(ToUInt16(x)); }
	public static explicit operator float(decimal x)
				{ return ToSingle(x); }
	public static explicit operator double(decimal x)
				{ return ToDouble(x); }

	// Constructors.
	public Decimal(int value)
			{
				if(value >= 0)
				{
					low = value;
					flags = 0;
				}
				else
				{
					low = unchecked((int)(uint)(-value));
					flags = unchecked((int)(-0x80000000));
				}
				middle = 0;
				high = 0;
			}
	public Decimal(uint value)
			{
				low = unchecked((int)value);
				middle = 0;
				high = 0;
				flags = 0;
			}
	public Decimal(long value)
			{
				if(value >= 0)
				{
					low = unchecked((int)value);
					middle = unchecked((int)(value >> 32));
					flags = 0;
				}
				else
				{
					value = -value;
					low = unchecked((int)value);
					middle = unchecked((int)(value >> 32));
					flags = unchecked((int)(-0x80000000));
				}
				high = 0;
			}
	public Decimal(ulong value)
			{
				low = unchecked((int)value);
				middle = unchecked((int)(value >> 32));
				high = 0;
				flags = 0;
			}
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public Decimal(float value);
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public Decimal(double value);
	internal Decimal(Currency value)
			{
				decimal temp = Currency.ToDecimal(value);
				low = temp.low;
				middle = temp.middle;
				high = temp.high;
				flags = temp.flags;
			}
	public Decimal(int[] bits)
			{
				if(bits == null)
				{
					throw new ArgumentNullException("bits");
				}
				if(bits.Length == 4 &&
				   (bits[3] & 0x7F00FFFF) == 0 &&
				   (bits[3] & 0x00FF0000) <= (28 << ScaleShift))
				{
					low = bits[0];
					middle = bits[1];
					high = bits[2];
					flags = bits[3];
				}
				else
				{
					throw new ArgumentException(_("Arg_DecBitCtor"));
				}
			}
	public Decimal(int _low, int _middle, int _high, bool _isneg, byte _scale)
			{
				if(_scale <= 28)
				{
					low = _low;
					middle = _middle;
					high = _high;
					flags = (((int)_scale) << ScaleShift) |
							(_isneg ? unchecked((int)0x80000000) : 0);
				}
				else
				{
					throw new ArgumentOutOfRangeException
						(_("ArgRange_DecimalScale"));
				}
			}
	private Decimal(int _low, int _middle, int _high, int _flags)
			{
				low = _low;
				middle = _middle;
				high = _high;
				flags = _flags;
			}

	// Override inherited methods.
	public override int GetHashCode()
			{ return (low ^ middle ^ high ^ flags); }
	public override bool Equals(Object obj)
			{
				if(obj is Decimal)
				{
					return (Compare((decimal)this, (decimal)obj) == 0);
				}
				else
				{
					return false;
				}
			}

	// String conversion.
	public override String ToString()
			{
				return ToString(null, null);
			}
	public String ToString(String format)
			{
				return ToString(format, null);
			}
	public String ToString(String format, IFormatProvider provider)
			{
				return NumberFormatter.FormatDecimal
							(this, format,
							 NumberFormatInfo.GetInstance(provider));
			}

	// Parsing methods.
	public static decimal Parse(String s, NumberStyles style,
							    IFormatProvider provider)
			{
				return NumberParser.ParseDecimal
						(s, style, NumberFormatInfo.GetInstance(provider));
			}
	public static decimal Parse(String s)
			{
				return Parse(s, NumberStyles.Currency, null);
			}
	public static decimal Parse(String s, NumberStyles style)
			{
				return Parse(s, style, null);
			}
	public static decimal Parse(String s, IFormatProvider provider)
			{
				return Parse(s, NumberStyles.Currency, provider);
			}

	// Implementation of the IComparable interface.
	public int CompareTo(Object value)
			{
				if(value != null)
				{
					if(value is Decimal)
					{
			  			return Compare((decimal)this, (decimal)value);
					}
					else
					{
						throw new ArgumentException(_("Arg_MustBeDecimal"));
					}
				}
				else
				{
					return 1;
				}
			}

	// Implementation of the IConvertible interface.
	public TypeCode GetTypeCode()
			{
				return TypeCode.Decimal;
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
				throw new InvalidCastException
					(String.Format
						(_("InvalidCast_FromTo"), "Decimal", "Char"));
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
				return this;
			}
	DateTime IConvertible.ToDateTime(IFormatProvider provider)
			{
				throw new InvalidCastException
					(String.Format
						(_("InvalidCast_FromTo"), "Decimal", "DateTime"));
			}
	public String ToString(IFormatProvider provider)
			{
				return ToString(null, provider);
			}
	Object IConvertible.ToType(Type conversionType, IFormatProvider provider)
			{
				return Convert.DefaultToType(this, conversionType, provider);
			}

	// Static conversion methods.
	private static int TruncateToInt32(decimal value)
			{
				decimal temp = Truncate(value);
				if(temp.flags >= 0)
				{
					return temp.low;
				}
				else
				{
					return unchecked(-(temp.low));
				}
			}
	public static byte ToByte(decimal value)
			{
				if(value >= 0.0m && value <= 255.0m)
				{
					return unchecked((byte)(TruncateToInt32(value)));
				}
				else
				{
					throw new OverflowException(_("Overflow_Byte"));
				}
			}
	public static sbyte ToSByte(decimal value)
			{
				if(value >= -128.0m && value <= 127.0m)
				{
					return unchecked((sbyte)(TruncateToInt32(value)));
				}
				else
				{
					throw new OverflowException(_("Overflow_SByte"));
				}
			}
	public static short ToInt16(decimal value)
			{
				if(value >= -32768.0m && value <= 32767.0m)
				{
					return unchecked((short)(TruncateToInt32(value)));
				}
				else
				{
					throw new OverflowException(_("Overflow_Int16"));
				}
			}
	public static ushort ToUInt16(decimal value)
			{
				if(value >= 0.0m && value <= 65535.0m)
				{
					return unchecked((ushort)(TruncateToInt32(value)));
				}
				else
				{
					throw new OverflowException(_("Overflow_UInt16"));
				}
			}
	public static int ToInt32(decimal value)
			{
				if(value >= -2147483648.0m && value <= 2147483647.0m)
				{
					return unchecked(TruncateToInt32(value));
				}
				else
				{
					throw new OverflowException(_("Overflow_Int32"));
				}
			}
	public static uint ToUInt32(decimal value)
			{
				if(value >= 0.0m && value <= 4294967295.0m)
				{
					decimal temp = Truncate(value);
					return unchecked((uint)(temp.low));
				}
				else
				{
					throw new OverflowException(_("Overflow_UInt32"));
				}
			}
	public static long ToInt64(decimal value)
			{
				if(value >= -9223372036854775808.0m &&
				   value <= 9223372036854775807.0m)
				{
					decimal temp = Truncate(value);
					if(temp.flags >= 0)
					{
						return unchecked((((long)(temp.middle)) << 32) |
										  ((long)(ulong)(uint)(temp.low)));
					}
					else
					{
						return unchecked(-((((long)(temp.middle)) << 32) |
										    ((long)(ulong)(uint)(temp.low))));
					}
				}
				else
				{
					throw new OverflowException(_("Overflow_Int64"));
				}
			}
	public static ulong ToUInt64(decimal value)
			{
				if(value >= 0.0m && value <= 18446744073709551615.0m)
				{
					decimal temp = Truncate(value);
					return unchecked((((ulong)(uint)(temp.middle)) << 32) |
									  ((ulong)(uint)(temp.low)));
				}
				else
				{
					throw new OverflowException(_("Overflow_UInt64"));
				}
			}
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static float ToSingle(decimal value);
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static double ToDouble(decimal value);
	public static Currency ToCurrency(decimal value)
			{
				return new Currency(value);
			}

	// Get the bits of a decimal value.
	public static int[] GetBits(decimal x)
			{
				int[] result = new int [4];
				result[0] = x.low;
				result[1] = x.middle;
				result[2] = x.high;
				result[3] = x.flags;
				return result;
			}

}; // class Decimal

}; // namespace System