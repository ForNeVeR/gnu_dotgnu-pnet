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

	public const int Empty = 0;
	public const int True  = 1;
	public const int False = 0;
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
	public static String ToString(bool value)
			{ return (value ? TrueString : FalseString); }

	// Parsing methods.
	public static bool FromString(String value)
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
	public static bool Parse(String s)
			{ return FromString(s); }

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

	// Implementation of IConvertible interface.
	public TypeCode GetTypeCode() { return TypeCode.Boolean; }
	public Object ToType(Type ct) { return Convert.DefaultToType(this, ct); }
	public Boolean ToBoolean()     { return value__; }
	public Byte ToByte()           { return Convert.ToByte(value__); }
	public SByte ToSByte()         { return Convert.ToSByte(value__); }
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
		 			     "Boolean", "DateTime"));
			}

}; // class Boolean

}; // namespace System
