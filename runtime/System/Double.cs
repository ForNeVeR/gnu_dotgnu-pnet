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
using System.Runtime.InteropServices;

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
	public const double Empty            = 0.0;

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
	public override String ToString()
			{
				return Format(value__, null, NumberFormatInfo.InvariantInfo);
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

	// Parsing methods.
	public static double FromString(String s)
			{ return Parse(s, NumberStyles.Float,
						   NumberFormatInfo.InvariantInfo); }
	public static double Parse(String s, NumberStyles style,
							   NumberFormatInfo nfi)
			{
				if(nfi == null)
				{
					nfi = NumberFormatInfo.CurrentInfo;
				}
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
			{ return Parse(s, NumberStyles.Float |
							  NumberStyles.AllowThousands, null); }
	public static double Parse(String s, NumberStyles style)
			{ return Parse(s, style, null); }

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
						throw new ArgumentException
							(Environment.GetResourceString("Arg_MustBeDouble"));
					}
				}
				else
				{
					return 1;
				}
			}

	// Implementation of the IFormattable interface.
	public String Format(String format, IServiceObjectProvider isop)
			{
				return NumberFormatter.FormatDouble
						(value__, format,
						 NumberFormatInfo.GetInstance(isop));
			}

	// Other formatting methods.
	public static String ToString(double value)
			{ return Format(value, null, NumberFormatInfo.InvariantInfo); }
	public static String Format(double value, String format)
			{ return Format(value, format, null); }
	public static String Format(double value, String format,
								NumberFormatInfo nfi)
			{
				return NumberFormatter.FormatDouble
						(value, format, NumberFormatInfo.GetInstance(nfi));
			}

	// Implementation of IConvertible interface.
	public TypeCode GetTypeCode() { return TypeCode.Double; }
	public Object ToType(Type ct) { return Convert.DefaultToType(this, ct); }
	public Boolean ToBoolean()     { return Convert.ToBoolean(value__); }
	public Byte ToByte()           { return Convert.ToByte(value__); }
	public SByte ToSByte()         { return Convert.ToSByte(value__); }
	public Int16 ToInt16()         { return Convert.ToInt16(value__); }
	public UInt16 ToUInt16()	   { return Convert.ToUInt16(value__); }
	public Int32 ToInt32()         { return Convert.ToInt32(value__); }
	public UInt32 ToUInt32()       { return Convert.ToUInt32(value__); }
	public Int64 ToInt64()         { return Convert.ToInt64(value__); }
	public UInt64 ToUInt64()       { return Convert.ToUInt64(value__); }
	public Char ToChar()
			{
				throw new InvalidCastException
					(String.Format
						(Environment.GetResourceString("InvalidCast_FromTo"),
		 			     "Double", "Char"));
			}
	public Single ToSingle()       { return Convert.ToSingle(value__); }
	public Double ToDouble()       { return value__; }
	public Decimal ToDecimal()     { return Convert.ToDecimal(value__); }
	public DateTime ToDateTime()
			{
				throw new InvalidCastException
					(String.Format
						(Environment.GetResourceString("InvalidCast_FromTo"),
		 			     "Double", "DateTime"));
			}

}; // class Double

}; // namespace System
