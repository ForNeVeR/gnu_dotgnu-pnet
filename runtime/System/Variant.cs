/*
 * Variant.cs - Implementation of the "System.Variant" class.
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

using System.Reflection;

internal struct Variant
{

	// Variant types.
	internal const int CV_EMPTY                = 0;
	internal const int CV_VOID                 = 1;
	internal const int CV_BOOLEAN              = 2;
	internal const int CV_CHAR                 = 3;
	internal const int CV_I1                   = 4;
	internal const int CV_U1                   = 5;
	internal const int CV_I2                   = 6;
	internal const int CV_U2                   = 7;
	internal const int CV_I4                   = 8;
	internal const int CV_U4                   = 9;
	internal const int CV_I8                   = 10;
	internal const int CV_U8                   = 11;
	internal const int CV_R4                   = 12;
	internal const int CV_R8                   = 13;
	internal const int CV_STRING               = 14;
	internal const int CV_PTR                  = 15;
	internal const int CV_DATETIME             = 16;
	internal const int CV_TIMESPAN             = 17;
	internal const int CV_OBJECT               = 18;
	internal const int CV_DECIMAL              = 19;
	internal const int CV_CURRENCY             = 20;
	internal const int CV_ENUM                 = 21;
	internal const int CV_MISSING              = 22;
	internal const int CV_NULL                 = 23;
	internal const int CV_LAST                 = 24;

	// Variant flags.
	internal const int TypeCodeBitMask		   = 0x0000FFFF;
	internal const int ArrayBitMask            = 0x00010000;
	internal const int EnumMask                = 0x00F00000;
	internal const int VTBitMask               = unchecked((int)0xFF000000);
	internal const int VTBitShift              = 24;
	internal const int EnumI1                  = 0x00100000;
	internal const int EnumU1                  = 0x00200000;
	internal const int EnumI2                  = 0x00300000;
	internal const int EnumU2                  = 0x00400000;
	internal const int EnumI4                  = 0x00500000;
	internal const int EnumU4                  = 0x00600000;
	internal const int EnumI8                  = 0x00700000;
	internal const int EnumU8                  = 0x00800000;

	// Class types that are associated with each of the variant types.
	internal static readonly Type[] ClassTypes =
		{typeof(System.Empty), typeof(void), typeof(System.Boolean),
		 typeof(System.Char), typeof(System.SByte), typeof(System.Byte),
		 typeof(System.Int16), typeof(System.UInt16), typeof(System.Int32),
		 typeof(System.UInt32), typeof(System.Int64), typeof(System.UInt64),
		 typeof(System.Single), typeof(System.Double), typeof(System.String),
		 typeof(void), typeof(System.DateTime), typeof(System.TimeSpan),
		 typeof(System.Object), typeof(System.Decimal),
		 typeof(System.Currency), typeof(System.Object),
		 typeof(System.Reflection.Missing), typeof(System.DBNull)};

	// Special variant values.
	public static readonly Variant DBNull =
				new Variant(CV_NULL, System.DBNull.Value);
	public static readonly Variant Empty;
	public static readonly Variant Missing =
				new Variant(CV_MISSING, Type.Missing);

	// Internal state that is used to store variants.
	private int    flags__;
	private int    data1__;
	private int    data2__;
	private Object ref__;

	// Constructors.
	private Variant(int flags, Object reference)
			{
				flags__ = flags;
				data1__ = 0;
				data2__ = 0;
				ref__   = reference;
			}
	public Variant(bool value)
			{
				flags__ = CV_BOOLEAN;
				data1__ = (value ? 1 : 0);
				data2__ = 0;
				ref__   = null;
			}
	public Variant(char value)
			{
				flags__ = CV_CHAR;
				data1__ = (int)value;
				data2__ = 0;
				ref__   = null;
			}
	public Variant(byte value)
			{
				flags__ = CV_U1;
				data1__ = (int)value;
				data2__ = 0;
				ref__   = null;
			}
	public Variant(sbyte value)
			{
				flags__ = CV_I1;
				data1__ = (int)value;
				data2__ = 0;
				ref__   = null;
			}
	public Variant(short value)
			{
				flags__ = CV_I2;
				data1__ = (int)value;
				data2__ = 0;
				ref__   = null;
			}
	public Variant(ushort value)
			{
				flags__ = CV_U2;
				data1__ = (int)value;
				data2__ = 0;
				ref__   = null;
			}
	public Variant(int value)
			{
				flags__ = CV_I4;
				data1__ = value;
				data2__ = 0;
				ref__   = null;
			}
	public Variant(uint value)
			{
				flags__ = CV_U4;
				data1__ = unchecked((int)value);
				data2__ = 0;
				ref__   = null;
			}
	public Variant(long value)
			{
				flags__ = CV_I8;
				data1__ = unchecked((int)value);
				data2__ = unchecked((int)(value >> 32));
				ref__   = null;
			}
	public Variant(ulong value)
			{
				flags__ = CV_U8;
				data1__ = unchecked((int)value);
				data2__ = unchecked((int)(value >> 32));
				ref__   = null;
			}
	public Variant(float value)
			{
				flags__ = CV_R4;
				data1__ = 0;
				data2__ = 0;
				ref__   = (Object)(value);
			}
	public Variant(double value)
			{
				flags__ = CV_R8;
				data1__ = 0;
				data2__ = 0;
				ref__   = (Object)(value);
			}
	public Variant(DateTime value)
			{
				long ticks = value.Ticks;
				flags__ = CV_DATETIME;
				data1__ = unchecked((int)ticks);
				data2__ = unchecked((int)(ticks >> 32));
				ref__   = null;
			}
	public Variant(Currency value)
			{
				long curr = Currency.ToOACurrency(value);
				flags__ = CV_CURRENCY;
				data1__ = unchecked((int)curr);
				data2__ = unchecked((int)(curr >> 32));
				ref__   = null;
			}
	public Variant(Decimal value)
			{
				flags__ = CV_DECIMAL;
				data1__ = 0;
				data2__ = 0;
				ref__   = (Object)value;
			}
	public Variant(byte value, Type enumType)
			{
				flags__ = CV_ENUM;
				data1__ = unchecked((int)(value));
				data2__ = 0;
				ref__   = enumType;
			}
	public Variant(sbyte value, Type enumType)
			{
				flags__ = CV_ENUM;
				data1__ = unchecked((int)(value));
				data2__ = 0;
				ref__   = enumType;
			}
	public Variant(short value, Type enumType)
			{
				flags__ = CV_ENUM;
				data1__ = unchecked((int)(value));
				data2__ = 0;
				ref__   = enumType;
			}
	public Variant(ushort value, Type enumType)
			{
				flags__ = CV_ENUM;
				data1__ = unchecked((int)(value));
				data2__ = 0;
				ref__   = enumType;
			}
	public Variant(int value, Type enumType)
			{
				flags__ = CV_ENUM;
				data1__ = value;
				data2__ = 0;
				ref__   = enumType;
			}
	public Variant(uint value, Type enumType)
			{
				flags__ = CV_ENUM;
				data1__ = unchecked((int)(value));
				data2__ = 0;
				ref__   = enumType;
			}
	public Variant(long value, Type enumType)
			{
				flags__ = CV_ENUM;
				data1__ = unchecked((int)(value));
				data2__ = unchecked((int)(value >> 32));
				ref__   = enumType;
			}
	public Variant(ulong value, Type enumType)
			{
				flags__ = CV_ENUM;
				data1__ = unchecked((int)(value));
				data2__ = unchecked((int)(value >> 32));
				ref__   = enumType;
			}
	public Variant(Object value)
			{
				data1__ = 0;
				data2__ = 0;
				if(value is String)
				{
					flags__ = CV_STRING;
					ref__   = value;
				}
				else if(value == null)
				{
					this = Empty;
				}
				else if(value == System.DBNull.Value)
				{
					this = DBNull;
				}
				else if(value == Type.Missing)
				{
					this = Missing;
				}
				else
				{
					// TODO
					flags__ = CV_EMPTY;
					ref__   = null;
				}
			}
	unsafe public Variant(void *value, Type pointerType)
			{
				flags__ = CV_PTR;
				data1__ = unchecked((int)((long)value));
				data2__ = unchecked((int)(((long)value) >> 32));
				ref__   = pointerType;
			}

}; // class Variant

}; // namespace System
