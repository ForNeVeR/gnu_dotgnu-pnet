/*
 * Double.cs - Implementation of the "System.Double" class.
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

public struct Double : IComparable, IFormattable, IConvertible
{
	private double value__;

#if __CSCC__
	public const double MinValue         = __builtin_constant("double_min");
	public const double Epsilon          = __builtin_constant("double_epsilon");
	public const double MaxValue         = __builtin_constant("double_max");
#else
	public const double MinValue         = -1.7976931348623157E+308;
	public const double Epsilon          = 4.94065645841247e-324;
	public const double MaxValue         = 1.7976931348623157E+308;
#endif
	public const double PositiveInfinity = (1.0 / 0.0);
	public const double NegativeInfinity = (-1.0 / 0.0);
	public const double NaN              = (0.0 / 0.0);

	// Override inherited methods.
	public override int GetHashCode()
			{
				if(value__ >= 0.0)
				{
					return unchecked((int)value__);
				}
				else
				{
					return unchecked(-(int)value__);
				}
			}
	public override bool Equals(Object value)
			{
				if(value is Double)
				{
					return (value__ == ((Double)value).value__);
				}
				else
				{
					return false;
				}
			}

	// Value testing methods.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static bool IsNaN(double d);
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static bool IsInfinity(double d);
	public static bool IsPositiveInfinity(double d)
				{ return (IsInfinity(d) && d > 0.0); }
	public static bool IsNegativeInfinity(double d)
				{ return (IsInfinity(d) && d < 0.0); }

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
				return NumberFormatter.FormatDouble
						(value__, format,
						 NumberFormatInfo.GetInstance(provider));
			}

	// Parsing methods.
	public static double Parse(String s, NumberStyles style,
							   IFormatProvider provider)
			{
				NumberFormatInfo nfi = NumberFormatInfo.GetInstance(provider);
				try
				{
					return NumberParser.ParseDouble(s, style, nfi);
				}
				catch(FormatException)
				{
					String temp = s.Trim();
					if(temp.Equals(nfi.PositiveInfinitySymbol))
					{
						return PositiveInfinity;
					}
					else if(temp.Equals(nfi.NegativeInfinitySymbol))
					{
						return NegativeInfinity;
					}
					else if(temp.Equals(nfi.NaNSymbol))
					{
						return NaN;
					}
					throw;
				}
			}
	public static double Parse(String s)
			{
				return Parse(s, NumberStyles.Float |
							 NumberStyles.AllowThousands, null);
		 	}
	public static double Parse(String s, IFormatProvider provider)
			{
				return Parse(s, NumberStyles.Float |
							 NumberStyles.AllowThousands, provider);
		 	}
	public static double Parse(String s, NumberStyles style)
			{
				return Parse(s, style, null);
			}

	// Implementation of the IComparable interface.
	public int CompareTo(Object value)
			{
				if(value != null)
				{
					if(value is Double)
					{
						double val1 = value__;
			  			double val2 = ((Double)value).value__;
						if(val1 < val2)
						{
							return -1;
						}
						else if(val1 > val2)
						{
							return 1;
						}
						else if(val1 == val2)
						{
							return 0;
						}
						else if(IsNaN(val1))
						{
							if(IsNaN(val2))
							{
								return 0;
							}
							else
							{
								return -1;
							}
						}
						else
						{
							return 1;
						}
					}
					else
					{
						throw new ArgumentException(_("Arg_MustBeDouble"));
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
				return TypeCode.Double;
			}
	bool IConvertible.ToBoolean(IFormatProvider provider)
			{
				return Convert.ToBoolean(value__);
			}
	byte IConvertible.ToByte(IFormatProvider provider)
			{
				return Convert.ToByte(value__);
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
				throw new InvalidCastException
					(String.Format
						(_("InvalidCast_FromTo"), "Double", "Char"));
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
				return value__;
			}
	Decimal IConvertible.ToDecimal(IFormatProvider provider)
			{
				return Convert.ToDecimal(value__);
			}
	DateTime IConvertible.ToDateTime(IFormatProvider provider)
			{
				throw new InvalidCastException
					(String.Format
						(_("InvalidCast_FromTo"), "Double", "DateTime"));
			}
	public String ToString(IFormatProvider provider)
			{
				return ToString(null, provider);
			}
	Object IConvertible.ToType(Type conversionType, IFormatProvider provider)
			{
				return Convert.DefaultToType(this, conversionType, provider);
			}

}; // class Double

}; // namespace System
