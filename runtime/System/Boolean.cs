/*
 * Boolean.cs - Implementation of the "System.Boolean" class.
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

public struct Boolean : IComparable, IConvertible
{
	private bool value__;

	public const String TrueString = "True";
	public const String FalseString = "False";

	// Override inherited methods.
	public override int GetHashCode()
			{ return (value__ ? 1 : 0); }
	public override bool Equals(Object value)
			{
				if(value is Boolean)
				{
					return (value__ == ((Boolean)value).value__);
				}
				else
				{
					return false;
				}
			}

	// String conversion.
	public override String ToString()
			{ return (value__ ? TrueString : FalseString); }

	// String parsing.
	public static bool Parse(String value)
			{
				if(value != null)
				{
					if(String.Compare(value, TrueString, true) == 0)
					{
						return true;
					}
					else if(String.Compare(value, FalseString, true) == 0)
					{
						return false;
					}
					value = value.Trim();
					if(String.Compare(value, TrueString, true) == 0)
					{
						return true;
					}
					else if(String.Compare(value, FalseString, true) == 0)
					{
						return false;
					}
					throw new FormatException
						(Environment.GetResourceString("Format_BadBoolean"));
				}
				else
				{
					throw new ArgumentNullException("value");
				}
			}

	// Implementation of the IComparable interface.
	public int CompareTo(Object value)
			{
				if(value != null)
				{
					if(!(value is Boolean))
					{
						throw new ArgumentException
							(Environment.GetResourceString
								("Arg_MustBeBoolean"));
					}
					bool other = ((Boolean)value).value__;
					if(value__ == other)
					{
						return 0;
					}
					else if(value__)
					{
						return 1;
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

	// Implementation of the IConvertible interface.
	public TypeCode GetTypeCode()
			{
				return TypeCode.Boolean;
			}
	bool IConvertible.ToBoolean(IFormatProvider provider)
			{
				return value__;
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
						(Environment.GetResourceString("InvalidCast_FromTo"),
		 			     "Boolean", "Char"));
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
						(Environment.GetResourceString("InvalidCast_FromTo"),
		 			     "Boolean", "DateTime"));
			}
	public String ToString(IFormatProvider provider)
			{
				return (value__ ? TrueString : FalseString);
			}
	Object IConvertible.ToType(Type conversionType, IFormatProvider provider)
			{
				return Convert.DefaultToType(this, conversionType, provider);
			}

}; // class Boolean

}; // namespace System
