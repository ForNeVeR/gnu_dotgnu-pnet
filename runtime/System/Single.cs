/*
 * Single.cs - Implementation of the "System.Single" class.
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

public struct Single : IComparable, IFormattable, IConvertible
{
	private float value__;

#if __CSCC__
	public const float MinValue         = __builtin_constant("float_min");
	public const float Epsilon          = __builtin_constant("float_epsilon");
	public const float MaxValue         = __builtin_constant("float_max");
#else
	public const float MinValue         = -3.40282346638528859e38f;
	public const float Epsilon          = 1.4e-45f;
	public const float MaxValue         = 3.40282346638528859e38f;
#endif
	public const float PositiveInfinity = (1.0f / 0.0f);
	public const float NegativeInfinity = (-1.0f / 0.0f);
	public const float NaN              = (0.0f / 0.0f);
	public const float Empty            = 0.0f;

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
				if(value is Single)
				{
					return (value__ == ((Single)value).value__);
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
	extern public static bool IsNaN(float f);
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static bool IsInfinity(float f);
	public static bool IsPositiveInfinity(float f)
				{ return (IsInfinity(f) && f > 0.0); }
	public static bool IsNegativeInfinity(float f)
				{ return (IsInfinity(f) && f < 0.0); }

	// Parsing methods.
	public static float FromString(String s)
			{ return Parse(s, NumberStyles.Float,
						   NumberFormatInfo.InvariantInfo); }
	public static float Parse(String s, NumberStyles style,
							  NumberFormatInfo nfi)
			{
				if(nfi == null)
				{
					nfi = NumberFormatInfo.CurrentInfo;
				}
				try
				{
					return NumberParser.ParseSingle(s, style, nfi);
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
	public static float Parse(String s)
			{ return Parse(s, NumberStyles.Float |
							  NumberStyles.AllowThousands, null); }
	public static float Parse(String s, NumberStyles style)
			{ return Parse(s, style, null); }

	// Implementation of the IComparable interface.
	public int CompareTo(Object value)
			{
				if(value != null)
				{
					if(value is Single)
					{
						float val1 = value__;
			  			float val2 = ((Single)value).value__;
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
							(Environment.GetResourceString("Arg_MustBeSingle"));
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
				return NumberFormatter.FormatSingle
						(value__, format,
						 NumberFormatInfo.GetInstance(isop));
			}

	// Other formatting methods.
	public static String ToString(float value)
			{ return Format(value, null, NumberFormatInfo.InvariantInfo); }
	public static String Format(float value, String format)
			{ return Format(value, format, null); }
	public static String Format(float value, String format,
								NumberFormatInfo nfi)
			{
				return NumberFormatter.FormatSingle
						(value, format, NumberFormatInfo.GetInstance(nfi));
			}

	// Implementation of IConvertible interface.
	public TypeCode GetTypeCode() { return TypeCode.Single; }
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
		 			     "Single", "Char"));
			}
	public Single ToSingle()       { return value__; }
	public Double ToDouble()       { return Convert.ToDouble(value__); }
	public Decimal ToDecimal()     { return Convert.ToDecimal(value__); }
	public DateTime ToDateTime()
			{
				throw new InvalidCastException
					(String.Format
						(Environment.GetResourceString("InvalidCast_FromTo"),
		 			     "Single", "DateTime"));
			}

}; // class Single

}; // namespace System
