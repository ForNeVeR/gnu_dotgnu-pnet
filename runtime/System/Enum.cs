/*
 * Enum.cs - Implementation of the "System.Enum" class.
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

public abstract class Enum : ValueType, IComparable, IFormattable
#if !ECMA_COMPAT
	, IConvertible
#endif
{

	// Implement the IComparable interface.
	public int CompareTo(Object target)
			{
				Type type;
				if(target == null)
				{
					throw new ArgumentNullException("target");
				}
				type = GetType();
				if(type != target.GetType())
				{
					throw new ArgumentException(_("Arg_MustBeSameEnum"));
				}
				if(IsSigned(type))
				{
					long value1 = GetSigned(type);
					long value2 = ((Enum)target).GetSigned(type);
					if(value1 < value2)
					{
						return -1;
					}
					else if(value1 > value2)
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
					ulong uvalue1 = GetUnsigned(type);
					ulong uvalue2 = ((Enum)target).GetUnsigned(type);
					if(uvalue1 < uvalue2)
					{
						return -1;
					}
					else if(uvalue1 > uvalue2)
					{
						return 1;
					}
					else
					{
						return 0;
					}
				}
			}

	// Test for equality.
	public override bool Equals(Object obj)
			{
				Enum oenum = (obj as Enum);
				if(oenum != null)
				{
					Type type = GetType();
					if(type != oenum.GetType())
					{
						return false;
					}
					if(IsSigned(type))
					{
						return (GetSigned(type) == oenum.GetSigned(type));
					}
					else
					{
						return (GetUnsigned(type) == oenum.GetUnsigned(type));
					}
				}
				else
				{
					return false;
				}
			}

	// Format an enumerated value.
	public static String Format(Type enumType, Object value, String format)
			{
				// TODO
				return "";
			}

	// Get the hash code for this enumerated value.
	public override int GetHashCode()
			{
				Type type = GetType();
				if(IsSigned(type))
				{
					long value = GetSigned(type);
					return unchecked((int)(value ^ (value >> 16)));
				}
				else
				{
					ulong uvalue = GetUnsigned(type);
					return unchecked((int)(uvalue ^ (uvalue >> 16)));
				}
			}

	// Get the name of an enumerated constant value.
	public static String GetName(Type enumType, Object value)
			{
				// TODO
				return "";
			}

	// Get the names of all enumerated constants in a type.
	public static String[] GetNames(Type enumType)
			{
				// TODO
				return null;
			}

	// Get the underlying type for an enumerated type.
	public static Type GetUnderlyingType(Type enumType)
			{
				// TODO
				return null;
			}

	// Get the values of all enumerated constants for an enumerated type.
	public static Array GetValues(Type enumType)
			{
				// TODO
				return null;
			}

	// Determine if a specified constant is defined in an enumerated type.
	public static bool IsDefined(Type enumType, Object value)
			{
				// TODO
				return false;
			}

	// Parse an enumerated constant specification.
	public static Object Parse(Type enumType, String value)
			{
				return Parse(enumType, value, false);
			}
	public static Object Parse(Type enumType, String value, bool ignoreCase)
			{
				// TODO
				return null;
			}

	// Convert a constant into an enumerated value object.
	public static Object ToObject(Type enumType, Object value)
			{
				// TODO
				return null;
			}
	public static Object ToObject(Type enumType, sbyte value)
			{
				// TODO
				return null;
			}
	public static Object ToObject(Type enumType, byte value)
			{
				// TODO
				return null;
			}
	public static Object ToObject(Type enumType, short value)
			{
				// TODO
				return null;
			}
	public static Object ToObject(Type enumType, ushort value)
			{
				// TODO
				return null;
			}
	public static Object ToObject(Type enumType, int value)
			{
				// TODO
				return null;
			}
	public static Object ToObject(Type enumType, uint value)
			{
				// TODO
				return null;
			}
	public static Object ToObject(Type enumType, long value)
			{
				// TODO
				return null;
			}
	public static Object ToObject(Type enumType, ulong value)
			{
				// TODO
				return null;
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
	public virtual String ToString(String format, IFormatProvider provider)
			{
				return Format(GetType(), this, format);
			}

#if !ECMA_COMPAT

	// Implementation of the IConvertible interface.
	public TypeCode GetTypeCode()
			{
				Type type = GetUnderlyingType(GetType());
				if(type == typeof(System.SByte))
				{
					return TypeCode.SByte;
				}
				else if(type == typeof(System.Byte))
				{
					return TypeCode.Byte;
				}
				else if(type == typeof(System.Int16))
				{
					return TypeCode.Int16;
				}
				else if(type == typeof(System.UInt16))
				{
					return TypeCode.UInt16;
				}
				else if(type == typeof(System.Int32))
				{
					return TypeCode.Int32;
				}
				else if(type == typeof(System.Int64))
				{
					return TypeCode.Int64;
				}
				else
				{
					return TypeCode.UInt64;
				}
			}
	bool IConvertible.ToBoolean(IFormatProvider provider)
			{
				throw new InvalidCastException
					(String.Format
						(_("InvalidCast_FromTo"), "Enum", "Boolean"));
			}
	byte IConvertible.ToByte(IFormatProvider provider)
			{
				return Convert.ToByte(GetValue());
			}
	sbyte IConvertible.ToSByte(IFormatProvider provider)
			{
				return Convert.ToSByte(GetValue());
			}
	short IConvertible.ToInt16(IFormatProvider provider)
			{
				return Convert.ToInt16(GetValue());
			}
	ushort IConvertible.ToUInt16(IFormatProvider provider)
			{
				return Convert.ToUInt16(GetValue());
			}
	char IConvertible.ToChar(IFormatProvider provider)
			{
				throw new InvalidCastException
					(String.Format
						(_("InvalidCast_FromTo"), "Enum", "Char"));
			}
	int IConvertible.ToInt32(IFormatProvider provider)
			{
				return Convert.ToInt32(GetValue());
			}
	uint IConvertible.ToUInt32(IFormatProvider provider)
			{
				return Convert.ToUInt32(GetValue());
			}
	long IConvertible.ToInt64(IFormatProvider provider)
			{
				return Convert.ToInt64(GetValue());
			}
	ulong IConvertible.ToUInt64(IFormatProvider provider)
			{
				return Convert.ToUInt64(GetValue());
			}
	float IConvertible.ToSingle(IFormatProvider provider)
			{
				throw new InvalidCastException
					(String.Format
						(_("InvalidCast_FromTo"), "Enum", "Single"));
			}
	double IConvertible.ToDouble(IFormatProvider provider)
			{
				throw new InvalidCastException
					(String.Format
						(_("InvalidCast_FromTo"), "Enum", "Double"));
			}
	Decimal IConvertible.ToDecimal(IFormatProvider provider)
			{
				throw new InvalidCastException
					(String.Format
						(_("InvalidCast_FromTo"), "Enum", "DateTime"));
			}
	DateTime IConvertible.ToDateTime(IFormatProvider provider)
			{
				throw new InvalidCastException
					(String.Format
						(_("InvalidCast_FromTo"), "Byte", "DateTime"));
			}
	Object IConvertible.ToType(Type conversionType, IFormatProvider provider)
			{
				return Convert.DefaultToType(this, conversionType, provider);
			}

#endif // !ECMA_COMPAT

	// Determine if an enumerated type is signed.
	private static bool IsSigned(Type type)
			{
				Type utype = GetUnderlyingType(type);
				return (utype == typeof(System.SByte) ||
				        utype == typeof(System.Int16) ||
				        utype == typeof(System.Int32) ||
				        utype == typeof(System.Int64));
			}

	// Get the signed value of an enumerated value.
	private long GetSigned(Type type)
			{
				// TODO.
				return 0;
			}

	// Get the unsigned value of an enumerated value.
	private ulong GetUnsigned(Type type)
			{
				// TODO.
				return 0;
			}

	// Get the boxed version of an enumerated value.
	private Object GetValue()
			{
				Type type = GetUnderlyingType(GetType());
				if(type == typeof(System.SByte))
				{
					return unchecked((sbyte)GetSigned(type));
				}
				else if(type == typeof(System.Byte))
				{
					return unchecked((byte)GetUnsigned(type));
				}
				else if(type == typeof(System.Int16))
				{
					return unchecked((short)GetSigned(type));
				}
				else if(type == typeof(System.UInt16))
				{
					return unchecked((ushort)GetUnsigned(type));
				}
				else if(type == typeof(System.Int32))
				{
					return unchecked((int)GetSigned(type));
				}
				else if(type == typeof(System.UInt32))
				{
					return unchecked((uint)GetUnsigned(type));
				}
				else if(type == typeof(System.Int64))
				{
					return GetSigned(type);
				}
				else
				{
					return GetUnsigned(type);
				}
			}

}; // class Enum

}; // namespace System
