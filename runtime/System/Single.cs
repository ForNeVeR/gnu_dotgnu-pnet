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
using System.Runtime.CompilerServices;

public struct Single : IComparable, IFormattable
#if !ECMA_COMPAT
	, IConvertible
#endif
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

	// String conversion.
	public override String ToString()
			{
				return ToString(null, null);
			}
	public String ToString(String format)
			{
				return ToString(format, null);
			}
	public String ToString(IFormatProvider provider)
			{
				return ToString(null, provider);
			}
	public String ToString(String format, IFormatProvider provider)
			{
				return NumberFormatter.FormatSingle
						(value__, format,
						 NumberFormatInfo.GetInstance(provider));
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
	public static float Parse(String s, NumberStyles style,
							  IFormatProvider provider)
			{
				NumberFormatInfo nfi = NumberFormatInfo.GetInstance(provider);
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
			{
				return Parse(s, NumberStyles.Float |
							 NumberStyles.AllowThousands, null);
		 	}
	public static float Parse(String s, IFormatProvider provider)
			{
				return Parse(s, NumberStyles.Float |
							 NumberStyles.AllowThousands, provider);
		 	}
	public static float Parse(String s, NumberStyles style)
			{
				return Parse(s, style, null);
			}

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
						throw new ArgumentException(_("Arg_MustBeSingle"));
					}
				}
				else
				{
					return 1;
				}
			}

#if !ECMA_COMPAT

	// Implementation of the IConvertible interface.
	public TypeCode GetTypeCode()
			{
				return TypeCode.Single;
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
						(_("InvalidCast_FromTo"), "Single", "Char"));
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
				return value__;
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
						(_("InvalidCast_FromTo"), "Single", "DateTime"));
			}
	Object IConvertible.ToType(Type conversionType, IFormatProvider provider)
			{
				return Convert.DefaultToType(this, conversionType, provider);
			}

#endif // !ECMA_COMPAT

}; // class Single

}; // namespace System
