/*
 * Currency.cs - Implementation of the "System.Currency" class.
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

using System.Globalization;

// Note: This class is obsolete.  Use Decimal instead.
// The implementation is provided in case older code uses it.

public struct Currency : IComparable, IFormattable
{

	private long value__;

	public static readonly Currency MaxValue =
			new Currency(0x7FFFFFFFFFFFFFFF, 0);
	public static readonly Currency MinValue =
			new Currency(unchecked((long)0x8000000000000000), 0);
	public static readonly Currency Empty = new Currency(0, 0);
	public static readonly Currency Zero = new Currency(0, 0);
	public static readonly Currency One = new Currency(10000, 0);
	public static readonly Currency MinusOne = new Currency(-10000, 0);
	public const long Scale = 10000;
	private const long MinCurrency = -(0x7FFFFFFFFFFFFFFF / 10000);
	private const long MaxCurrency = (0x7FFFFFFFFFFFFFFF / 10000);

	// Constructors.
	public Currency(int value)
			{
				value__ = ((long)value) * Scale;
			}
	public Currency(uint value)
			{
				value__ = ((long)value) * Scale;
			}
	public Currency(long value)
			{
				if(value >= MinCurrency && value <= MaxCurrency)
				{
					value__ = unchecked(value * Scale);
				}
				else
				{
					throw new OverflowException
						(Environment.GetResourceString("Overflow_Currency"));
				}
			}
	public Currency(ulong value)
			{
				if(value <= unchecked((ulong)MaxCurrency))
				{
					value__ = unchecked(((long)value) * Scale);
				}
				else
				{
					throw new OverflowException
						(Environment.GetResourceString("Overflow_Currency"));
				}
			}
	public Currency(Decimal value)
			{
				try
				{
					checked
					{
						value__ = (long)(value * 10000.0m);
					}
				}
				catch(OverflowException)
				{
					// Trap overflow exceptions from the conversion
					// and turn them into Currency overflows.
					throw new OverflowException
						(Environment.GetResourceString("Overflow_Currency"));
				}
			}
	public Currency(float value)
			{
				// Use the Decimal class to do the hard work.
				try
				{
					value__ = (long)(((Decimal)value) * 10000.0m);
				}
				catch(OverflowException)
				{
					// Trap overflow exceptions from the Decimal
					// class and turn them into Currency overflows.
					throw new OverflowException
						(Environment.GetResourceString("Overflow_Currency"));
				}
			}
	public Currency(double value)
			{
				// Use the Decimal class to do the hard work.
				try
				{
					value__ = (long)(((Decimal)value) * 10000.0m);
				}
				catch(OverflowException)
				{
					// Trap overflow exceptions from the Decimal
					// class and turn them into Currency overflows.
					throw new OverflowException
						(Environment.GetResourceString("Overflow_Currency"));
				}
			}
	private Currency(long value, int dummy)
			{
				value__ = value;
			}

	// Arithmetic operators.
	public static Currency Abs(Currency x)
			{
				if(x.value__ >= 0)
				{
					return x;
				}
				else if(x.value__ == unchecked((long)0x8000000000000000))
				{
					return new Currency(0x7FFFFFFFFFFFFFFF, 0);
				}
				else
				{
					return new Currency(unchecked(-(x.value__)), 0);
				}
			}
	public static Currency Add(Currency x, Currency y)
			{
				try
				{
					checked
					{
						return new Currency((x.value__ + y.value__), 0);
					}
				}
				catch(OverflowException)
				{
					// Turn the runtime overflow for the add into
					// an overflow exception for Currency.
					throw new OverflowException
						(Environment.GetResourceString("Overflow_Currency"));
				}
			}
	public static Currency Subtract(Currency x, Currency y)
			{
				try
				{
					checked
					{
						return new Currency((x.value__ - y.value__), 0);
					}
				}
				catch(OverflowException)
				{
					// Turn the runtime overflow for the subtract into
					// an overflow exception for Currency.
					throw new OverflowException
						(Environment.GetResourceString("Overflow_Currency"));
				}
			}
	public static Decimal Multiply(Currency x, Currency y)
			{
				return Decimal.Multiply(ToDecimal(x), ToDecimal(y));
			}
	public static Decimal Divide(Currency x, Currency y)
			{
				return Decimal.Divide(ToDecimal(x), ToDecimal(y));
			}
	public static Currency Floor(Currency x)
			{
				if(x.value__ >= 0)
				{
					return new Currency((x.value__ / Scale) * Scale, 0);
				}
				else if(x.value__ >= (MinCurrency + Scale))
				{
					return new Currency(((x.value__ - (Scale - 1))
											/ Scale) * Scale, 0);
				}
				else if(x.value__ >= MinCurrency)
				{
					return new Currency(MinCurrency, 0);
				}
				else
				{
					throw new OverflowException
						(Environment.GetResourceString("Overflow_Currency"));
				}
			}
	public static Currency Truncate(Currency x)
			{
				return new Currency((x.value__ / Scale) * Scale, 0);
			}
	public static Currency Negate(Currency x)
			{
				if(x.value__ != unchecked((long)0x8000000000000000))
				{
					return new Currency(unchecked(-(x.value__)), 0);
				}
				else
				{
					throw new OverflowException
						(Environment.GetResourceString
							("Overflow_NegateTwosCompNum"));
				}
			}
	public static Currency Round(Currency x, int decimals)
			{
				// Use the Decimal class to do the hard work.
				try
				{
					Decimal result = Decimal.Round(ToDecimal(x), decimals);
					return new Currency((long)(result * 10000.0m));
				}
				catch(OverflowException)
				{
					// Trap overflow exceptions from the Decimal
					// class and turn them into Currency overflows.
					throw new OverflowException
						(Environment.GetResourceString("Overflow_Currency"));
				}
			}

	// Conversions.
	public static Currency FromOACurrency(long value)
			{
				return new Currency(value, 0);
			}
	public static long ToOACurrency(Currency value)
			{
				return value.value__;
			}
	public static Decimal ToDecimal(Currency value)
			{
				return (((Decimal)(value.value__)) / 10000.0m);
			}
	public static float ToSingle(Currency value)
			{
				return ToDecimal(value).ToSingle();
			}
	public static double ToDouble(Currency value)
			{
				return ToDecimal(value).ToDouble();
			}
	public static int ToInt32(Currency value)
			{
				if(value.value__ >= unchecked(((long)-0x80000000) * Scale) &&
				   value.value__ <= unchecked(((long)0x7FFFFFFF) * Scale))
				{
					return unchecked((int)(value.value__ / Scale));
				}
				else
				{
					throw new OverflowException
						(Environment.GetResourceString("Overflow_Int32"));
				}
			}
	public static uint ToUInt32(Currency value)
			{
				if(value.value__ >= 0 &&
				   value.value__ <= unchecked(((long)0xFFFFFFFF) * Scale))
				{
					return unchecked((uint)(value.value__ / Scale));
				}
				else
				{
					throw new OverflowException
						(Environment.GetResourceString("Overflow_UInt32"));
				}
			}
	public static long ToInt64(Currency value)
			{
				return (value.value__ / Scale);
			}
	public static ulong ToUInt64(Currency value)
			{
				if(value.value__ >= 0)
				{
					return unchecked((ulong)(value.value__ / Scale));
				}
				else
				{
					throw new OverflowException
						(Environment.GetResourceString("Overflow_UInt64"));
				}
			}

	// Comparisons.
	public static Currency Min(Currency x, Currency y)
			{
				if(x.value__ < y.value__)
				{
					return x;
				}
				else
				{
					return y;
				}
			}
	public static Currency Max(Currency x, Currency y)
			{
				if(x.value__ > y.value__)
				{
					return x;
				}
				else
				{
					return y;
				}
			}
	public static bool Equals(Currency x, Currency y)
			{
				return (x.value__ == y.value__);
			}
	public static int Compare(Currency x, Currency y)
			{
				if(x.value__ < y.value__)
				{
					return -1;
				}
				else if(x.value__ > y.value__)
				{
					return 1;
				}
				else
				{
					return 0;
				}
			}

	// Override inherited methods.
	public override int GetHashCode()
			{
				return unchecked(((int)(value__ ^ (value__ >> 32)))
										& 0x7FFFFFFF);
			}
	public override bool Equals(Object value)
			{
				if(value is Currency)
				{
					return (value__ == ((Currency)value).value__);
				}
				else
				{
					return false;
				}
			}

	// String conversion.
	public override String ToString()
			{ return Format(this, null, NumberFormatInfo.InvariantInfo); }
	public static String ToString(Currency value)
			{ return Format(value, null, NumberFormatInfo.InvariantInfo); }

	// Formatting methods.
	public static String Format(Currency value, String format,
								NumberFormatInfo nfi)
			{
				return Decimal.Format(ToDecimal(value), format, nfi);
			}
	public String Format(String format, IServiceObjectProvider isop)
			{
				return ToDecimal(this).Format(format, isop);
			}
	public static String Format(Currency value, String format)
			{
				return Decimal.Format(ToDecimal(value), format, null);
			}

	// Parsing methods.
	public static Currency FromString(String s)
			{
				return Parse(s, NumberStyles.Currency,
							 NumberFormatInfo.InvariantInfo);
			}
	public static Currency Parse(String s, NumberStyles style,
							     NumberFormatInfo nfi)
			{
				return Decimal.ToCurrency(Decimal.Parse(s, style, nfi));
			}
	public static Currency Parse(String s)
			{
				return Parse(s, NumberStyles.Currency, null);
			}
	public static Currency Parse(String s, NumberStyles style)
			{
				return Parse(s, style, null);
			}

	// Implementation of the IComparable interface.
	public int CompareTo(Object value)
			{
				if(value != null)
				{
					if(!(value is Currency))
					{
						throw new ArgumentException
							(Environment.GetResourceString
								("Arg_MustBeCurrency"));
					}
					long value2 = ((Currency)value).value__;
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

	// Operators.
	public static Currency operator-(Currency x)
			{ return Negate(x); }
	public static Currency operator+(Currency x)
			{ return x; }
	public static Currency operator+(Currency x, Currency y)
			{ return Add(x, y); }
	public static Currency operator-(Currency x, Currency y)
			{ return Subtract(x, y); }
	public static Currency operator*(Currency x, Currency y)
			{ return (Currency)(Multiply(x, y)); }
	public static Currency operator/(Currency x, Currency y)
			{ return (Currency)(Divide(x, y)); }
	public static bool operator==(Currency x, Currency y)
			{ return Equals(x, y); }
	public static bool operator!=(Currency x, Currency y)
			{ return !Equals(x, y); }
	public static bool operator<(Currency x, Currency y)
			{ return (Compare(x, y) < 0); }
	public static bool operator<=(Currency x, Currency y)
			{ return (Compare(x, y) <= 0); }
	public static bool operator>(Currency x, Currency y)
			{ return (Compare(x, y) > 0); }
	public static bool operator>=(Currency x, Currency y)
			{ return (Compare(x, y) >= 0); }
	public static Currency operator--(Currency x)
			{ return Subtract(x, One); }
	public static Currency operator++(Currency x)
			{ return Add(x, One); }
	public static implicit operator Currency(byte x)
			{ return new Currency((int)x); }
	public static implicit operator Currency(sbyte x)
			{ return new Currency((int)x); }
	public static implicit operator Currency(short x)
			{ return new Currency((int)x); }
	public static implicit operator Currency(ushort x)
			{ return new Currency((int)x); }
	public static implicit operator Currency(char x)
			{ return new Currency((int)x); }
	public static implicit operator Currency(int x)
			{ return new Currency(x); }
	public static implicit operator Currency(uint x)
			{ return new Currency(x); }
	public static explicit operator Currency(long x)
			{ return new Currency(x); }
	public static explicit operator Currency(ulong x)
			{ return new Currency(x); }
	public static explicit operator Currency(float x)
			{ return new Currency(x); }
	public static explicit operator Currency(double x)
			{ return new Currency(x); }
	public static explicit operator Currency(Decimal x)
			{ return new Currency(x); }
	public static explicit operator byte(Currency x)
			{
				if(x.value__ >= 0 &&
				   x.value__ <= unchecked(((long)255) * Scale))
				{
					return unchecked((byte)(x.value__ / Scale));
				}
				else
				{
					throw new OverflowException
						(Environment.GetResourceString("Overflow_Byte"));
				}
			}
	public static explicit operator sbyte(Currency x)
			{
				if(x.value__ >= unchecked(((long)-128) * Scale) &&
				   x.value__ <= unchecked(((long)127) * Scale))
				{
					return unchecked((sbyte)(x.value__ / Scale));
				}
				else
				{
					throw new OverflowException
						(Environment.GetResourceString("Overflow_SByte"));
				}
			}
	public static explicit operator short(Currency x)
			{
				if(x.value__ >= unchecked(((long)-32768) * Scale) &&
				   x.value__ <= unchecked(((long)32767) * Scale))
				{
					return unchecked((short)(x.value__ / Scale));
				}
				else
				{
					throw new OverflowException
						(Environment.GetResourceString("Overflow_Int16"));
				}
			}
	public static explicit operator ushort(Currency x)
			{
				if(x.value__ >= 0 &&
				   x.value__ <= unchecked(((long)65535) * Scale))
				{
					return unchecked((ushort)(x.value__ / Scale));
				}
				else
				{
					throw new OverflowException
						(Environment.GetResourceString("Overflow_UInt16"));
				}
			}
	public static explicit operator int(Currency x)
			{
				return ToInt32(x);
			}
	public static explicit operator uint(Currency x)
			{
				return ToUInt32(x);
			}
	public static explicit operator long(Currency x)
			{
				return ToInt64(x);
			}
	public static explicit operator ulong(Currency x)
			{
				return ToUInt64(x);
			}
	public static explicit operator float(Currency x)
			{
				return ToSingle(x);
			}
	public static explicit operator double(Currency x)
			{
				return ToDouble(x);
			}

}; // class Currency

}; // namespace System
