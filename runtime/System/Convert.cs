/*
 * Convert.cs - Implementation of the "System.Convert" class.
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

public sealed class Convert
{
#if !ECMA_COMPAT
	internal static readonly Type[] ConvertTypes =
		{typeof(Empty), typeof(Object), typeof(System.DBNull),
		 typeof(Boolean), typeof(Char), typeof(SByte), typeof(Byte),
		 typeof(Int16), typeof(UInt16), typeof(Int32), typeof(UInt32),
		 typeof(Int64), typeof(UInt64), typeof(Single), typeof(Double),
		 typeof(Decimal), typeof(DateTime), typeof(Object), typeof(String)};
	public static readonly Object DBNull = System.DBNull.Value;
#endif // !ECMA_COMPAT

	// Convert various types into Boolean.
	public static bool ToBoolean(bool value) { return value; }
	public static bool ToBoolean(byte value) { return (value != 0); }
	[CLSCompliant(false)]
	public static bool ToBoolean(sbyte value) { return (value != 0); }
	public static bool ToBoolean(short value) { return (value != 0); }
	[CLSCompliant(false)]
	public static bool ToBoolean(ushort value) { return (value != 0); }
	public static bool ToBoolean(int value) { return (value != 0); }
	[CLSCompliant(false)]
	public static bool ToBoolean(uint value) { return (value != 0); }
	public static bool ToBoolean(long value) { return (value != 0); }
	[CLSCompliant(false)]
	public static bool ToBoolean(ulong value) { return (value != 0); }
	public static bool ToBoolean(float value) { return (value != 0.0); }
	public static bool ToBoolean(double value) { return (value != 0.0); }
	public static bool ToBoolean(String value)
			{
				return Boolean.Parse(value);
			}
	public static bool ToBoolean(Decimal value)
			{
				return (value != 0.0m);
			}
#if !ECMA_COMPAT
	public static bool ToBoolean(char value)
			{
				return ((IConvertible)value).ToBoolean(null);
			}
	public static bool ToBoolean(DateTime value)
			{
				return ((IConvertible)value).ToBoolean(null);
			}
	public static bool ToBoolean(Object value)
			{
				if(value != null)
				{
					return ((IConvertible)value).ToBoolean(null);
				}
				else
				{
					return false;
				}
			}
	public static bool ToBoolean(Object value, IFormatProvider provider)
			{
				if(value != null)
				{
					return ((IConvertible)value).ToBoolean(provider);
				}
				else
				{
					return false;
				}
			}
	public static bool ToBoolean(String value, IFormatProvider provider)
			{
				return Boolean.Parse(value);
			}
#endif

	// Convert various types into Byte.
	public static byte ToByte(bool value) { return (byte)(value ? 1 : 0); }
	public static byte ToByte(byte value) { return value; }
	[CLSCompliant(false)]
	public static byte ToByte(sbyte value)
			{
				if(value >= 0)
				{
					return unchecked((byte)value);
				}
				else
				{
					throw new OverflowException(_("Overflow_Byte"));
				}
			}
	public static byte ToByte(short value)
			{
				if(value >= 0 && value <= 255)
				{
					return unchecked((byte)value);
				}
				else
				{
					throw new OverflowException(_("Overflow_Byte"));
				}
			}
	[CLSCompliant(false)]
	public static byte ToByte(ushort value)
			{
				if(value <= 255)
				{
					return unchecked((byte)value);
				}
				else
				{
					throw new OverflowException(_("Overflow_Byte"));
				}
			}
	public static byte ToByte(int value)
			{
				if(value >= 0 && value <= 255)
				{
					return unchecked((byte)value);
				}
				else
				{
					throw new OverflowException(_("Overflow_Byte"));
				}
			}
	[CLSCompliant(false)]
	public static byte ToByte(uint value)
			{
				if(value <= 255)
				{
					return unchecked((byte)value);
				}
				else
				{
					throw new OverflowException(_("Overflow_Byte"));
				}
			}
	public static byte ToByte(long value)
			{
				if(value >= 0 && value <= 255)
				{
					return unchecked((byte)value);
				}
				else
				{
					throw new OverflowException(_("Overflow_Byte"));
				}
			}
	[CLSCompliant(false)]
	public static byte ToByte(ulong value)
			{
				if(value <= 255)
				{
					return unchecked((byte)value);
				}
				else
				{
					throw new OverflowException(_("Overflow_Byte"));
				}
			}
	public static byte ToByte(char value)
			{
				if(value <= 255)
				{
					return unchecked((byte)value);
				}
				else
				{
					throw new OverflowException(_("Overflow_Byte"));
				}
			}
	public static byte ToByte(float value)
			{
				return ToByte((double)value);
			}
	public static byte ToByte(double value)
			{
				return ToByte(ToInt32(value));
			}
	public static byte ToByte(Decimal value)
			{
				return Decimal.ToByte(Decimal.Round(value, 0));
			}
	public static byte ToByte(String value)
			{
				return Byte.Parse(value);
			}
	public static byte ToByte(String value, IFormatProvider provider)
			{
				return Byte.Parse(value, provider);
			}
#if !ECMA_COMPAT
	public static byte ToByte(Object value)
			{
				if(value != null)
				{
					return ((IConvertible)value).ToByte(null);
				}
				else
				{
					return 0;
				}
			}
	public static byte ToByte(Object value, IFormatProvider provider)
			{
				if(value != null)
				{
					return ((IConvertible)value).ToByte(provider);
				}
				else
				{
					return 0;
				}
			}
	public static byte ToByte(String value, int fromBase)
			{
				if(fromBase == 2 || fromBase == 8 ||
				   fromBase == 10 || fromBase == 16)
				{
					return ToByte(NumberParser.StringToUInt32
										(value, fromBase, 256));
				}
				else
				{
					throw new ArgumentException(_("Arg_InvalidBase"));
				}
			}
#endif // !ECMA_COMPAT

	// Convert various types into SByte.
	[CLSCompliant(false)]
	public static sbyte ToSByte(bool value) { return (sbyte)(value ? 1 : 0); }
	[CLSCompliant(false)]
	public static sbyte ToSByte(byte value)
			{
				if(value <= 127)
				{
					return unchecked((sbyte)value);
				}
				else
				{
					throw new OverflowException(_("Overflow_SByte"));
				}
			}
	[CLSCompliant(false)]
	public static sbyte ToSByte(sbyte value)	{ return value; }
	[CLSCompliant(false)]
	public static sbyte ToSByte(short value)
			{
				if(value >= -128 && value <= 127)
				{
					return unchecked((sbyte)value);
				}
				else
				{
					throw new OverflowException(_("Overflow_SByte"));
				}
			}
	[CLSCompliant(false)]
	public static sbyte ToSByte(ushort value)
			{
				if(value <= 127)
				{
					return unchecked((sbyte)value);
				}
				else
				{
					throw new OverflowException(_("Overflow_SByte"));
				}
			}
	[CLSCompliant(false)]
	public static sbyte ToSByte(int value)
			{
				if(value >= -128 && value <= 127)
				{
					return unchecked((sbyte)value);
				}
				else
				{
					throw new OverflowException(_("Overflow_SByte"));
				}
			}
	[CLSCompliant(false)]
	public static sbyte ToSByte(uint value)
			{
				if(value <= 127)
				{
					return unchecked((sbyte)value);
				}
				else
				{
					throw new OverflowException(_("Overflow_SByte"));
				}
			}
	[CLSCompliant(false)]
	public static sbyte ToSByte(long value)
			{
				if(value >= -127 && value <= 127)
				{
					return unchecked((sbyte)value);
				}
				else
				{
					throw new OverflowException(_("Overflow_SByte"));
				}
			}
	[CLSCompliant(false)]
	public static sbyte ToSByte(ulong value)
			{
				if(value <= 127)
				{
					return unchecked((sbyte)value);
				}
				else
				{
					throw new OverflowException(_("Overflow_SByte"));
				}
			}
	[CLSCompliant(false)]
	public static sbyte ToSByte(char value)
			{
				if(value <= 127)
				{
					return unchecked((sbyte)value);
				}
				else
				{
					throw new OverflowException(_("Overflow_SByte"));
				}
			}
	[CLSCompliant(false)]
	public static sbyte ToSByte(float value)
			{
				return ToSByte((double)value);
			}
	[CLSCompliant(false)]
	public static sbyte ToSByte(double value)
			{
				return ToSByte(ToInt32(value));
			}
	[CLSCompliant(false)]
	public static sbyte ToSByte(Decimal value)
			{
				return Decimal.ToSByte(Decimal.Round(value, 0));
			}
	[CLSCompliant(false)]
	public static sbyte ToSByte(String value)
			{
				return SByte.Parse(value);
			}
	[CLSCompliant(false)]
	public static sbyte ToSByte(String value, IFormatProvider provider)
			{
				return SByte.Parse(value, provider);
			}
#if !ECMA_COMPAT
	[CLSCompliant(false)]
	public static sbyte ToSByte(Object value)
			{
				if(value != null)
				{
					return ((IConvertible)value).ToSByte(null);
				}
				else
				{
					return 0;
				}
			}
	[CLSCompliant(false)]
	public static sbyte ToSByte(Object value, IFormatProvider provider)
			{
				if(value != null)
				{
					return ((IConvertible)value).ToSByte(provider);
				}
				else
				{
					return 0;
				}
			}
	[CLSCompliant(false)]
	public static sbyte ToSByte(String value, int fromBase)
			{
				if(fromBase == 2 || fromBase == 8 ||
				   fromBase == 10 || fromBase == 16)
				{
					return ToSByte(NumberParser.StringToInt32
										(value, fromBase, 128));
				}
				else
				{
					throw new ArgumentException(_("Arg_InvalidBase"));
				}
			}
#endif // !ECMA_COMPAT

	// Convert various types into Int16.
	public static short ToInt16(bool value) { return (short)(value ? 1 : 0); }
	public static short ToInt16(byte value)
			{
				return unchecked((short)value);
			}
	[CLSCompliant(false)]
	public static short ToInt16(sbyte value)
			{
				return unchecked((short)value);
			}
	public static short ToInt16(short value) { return value; }
	[CLSCompliant(false)]
	public static short ToInt16(ushort value)
			{
				if(value <= 32767)
				{
					return unchecked((short)value);
				}
				else
				{
					throw new OverflowException(_("Overflow_Int16"));
				}
			}
	public static short ToInt16(int value)
			{
				if(value >= -32768 && value <= 32767)
				{
					return unchecked((short)value);
				}
				else
				{
					throw new OverflowException(_("Overflow_Int16"));
				}
			}
	[CLSCompliant(false)]
	public static short ToInt16(uint value)
			{
				if(value <= 32767)
				{
					return unchecked((short)value);
				}
				else
				{
					throw new OverflowException(_("Overflow_Int16"));
				}
			}
	public static short ToInt16(long value)
			{
				if(value >= -32768 && value <= 32767)
				{
					return unchecked((short)value);
				}
				else
				{
					throw new OverflowException(_("Overflow_Int16"));
				}
			}
	[CLSCompliant(false)]
	public static short ToInt16(ulong value)
			{
				if(value <= 32767)
				{
					return unchecked((short)value);
				}
				else
				{
					throw new OverflowException(_("Overflow_Int16"));
				}
			}
	public static short ToInt16(char value)
			{
				if(value <= 32767)
				{
					return unchecked((short)value);
				}
				else
				{
					throw new OverflowException(_("Overflow_Int16"));
				}
			}
	public static short ToInt16(float value)
			{
				return ToInt16((double)value);
			}
	public static short ToInt16(double value)
			{
				return ToInt16(ToInt32(value));
			}
	public static short ToInt16(Decimal value)
			{
				return Decimal.ToInt16(Decimal.Round(value, 0));
			}
	public static short ToInt16(String value)
			{
				return Int16.Parse(value);
			}
	public static short ToInt16(String value, IFormatProvider provider)
			{
				return Int16.Parse(value, provider);
			}
#if !ECMA_COMPAT
	public static short ToInt16(Object value)
			{
				if(value != null)
				{
					return ((IConvertible)value).ToInt16(null);
				}
				else
				{
					return 0;
				}
			}
	public static short ToInt16(Object value, IFormatProvider provider)
			{
				if(value != null)
				{
					return ((IConvertible)value).ToInt16(provider);
				}
				else
				{
					return 0;
				}
			}
	public static short ToInt16(String value, int fromBase)
			{
				if(fromBase == 2 || fromBase == 8 ||
				   fromBase == 10 || fromBase == 16)
				{
					return ToInt16(NumberParser.StringToInt32
										(value, fromBase, 32768));
				}
				else
				{
					throw new ArgumentException(_("Arg_InvalidBase"));
				}
			}
#endif // !ECMA_COMPAT

	// Convert various types into UInt16.
	[CLSCompliant(false)]
	public static ushort ToUInt16(bool value)
			{
				return (ushort)(value ? 1 : 0);
			}
	[CLSCompliant(false)]
	public static ushort ToUInt16(byte value)
			{
				return unchecked((ushort)value);
			}
	[CLSCompliant(false)]
	public static ushort ToUInt16(sbyte value)
			{
				if(value >= 0)
				{
					return unchecked((ushort)value);
				}
				else
				{
					throw new OverflowException(_("Overflow_UInt16"));
				}
			}
	[CLSCompliant(false)]
	public static ushort ToUInt16(short value)
			{
				if(value >= 0)
				{
					return unchecked((ushort)value);
				}
				else
				{
					throw new OverflowException(_("Overflow_UInt16"));
				}
			}
	[CLSCompliant(false)]
	public static ushort ToUInt16(ushort value) { return value; }
	[CLSCompliant(false)]
	public static ushort ToUInt16(int value)
			{
				if(value >= 0 && value <= 65535)
				{
					return unchecked((ushort)value);
				}
				else
				{
					throw new OverflowException(_("Overflow_UInt16"));
				}
			}
	[CLSCompliant(false)]
	public static ushort ToUInt16(uint value)
			{
				if(value <= 65535)
				{
					return unchecked((ushort)value);
				}
				else
				{
					throw new OverflowException(_("Overflow_UInt16"));
				}
			}
	[CLSCompliant(false)]
	public static ushort ToUInt16(long value)
			{
				if(value >= 0 && value <= 65535)
				{
					return unchecked((ushort)value);
				}
				else
				{
					throw new OverflowException(_("Overflow_UInt16"));
				}
			}
	[CLSCompliant(false)]
	public static ushort ToUInt16(ulong value)
			{
				if(value <= 65535)
				{
					return unchecked((ushort)value);
				}
				else
				{
					throw new OverflowException(_("Overflow_UInt16"));
				}
			}
	[CLSCompliant(false)]
	public static ushort ToUInt16(char value)
			{
				return unchecked((ushort)value);
			}
	[CLSCompliant(false)]
	public static ushort ToUInt16(float value)
			{
				return ToUInt16((double)value);
			}
	[CLSCompliant(false)]
	public static ushort ToUInt16(double value)
			{
				return ToUInt16(ToInt32(value));
			}
	[CLSCompliant(false)]
	public static ushort ToUInt16(Decimal value)
			{
				return Decimal.ToUInt16(Decimal.Round(value, 0));
			}
	[CLSCompliant(false)]
	public static ushort ToUInt16(String value)
			{
				return UInt16.Parse(value);
			}
	[CLSCompliant(false)]
	public static ushort ToUInt16(String value, IFormatProvider provider)
			{
				return UInt16.Parse(value, provider);
			}
#if !ECMA_COMPAT
	[CLSCompliant(false)]
	public static ushort ToUInt16(Object value)
			{
				if(value != null)
				{
					return ((IConvertible)value).ToUInt16(null);
				}
				else
				{
					return 0;
				}
			}
	[CLSCompliant(false)]
	public static ushort ToUInt16(Object value, IFormatProvider provider)
			{
				if(value != null)
				{
					return ((IConvertible)value).ToUInt16(provider);
				}
				else
				{
					return 0;
				}
			}
	[CLSCompliant(false)]
	public static ushort ToUInt16(String value, int fromBase)
			{
				if(fromBase == 2 || fromBase == 8 ||
				   fromBase == 10 || fromBase == 16)
				{
					return ToUInt16(NumberParser.StringToUInt32
										(value, fromBase, 65536));
				}
				else
				{
					throw new ArgumentException(_("Arg_InvalidBase"));
				}
			}
#endif // !ECMA_COMPAT

	// Convert various types into Int32.
	public static int ToInt32(bool value) { return (value ? 1 : 0); }
	public static int ToInt32(byte value)
			{
				return unchecked((int)value);
			}
	[CLSCompliant(false)]
	public static int ToInt32(sbyte value)
			{
				return unchecked((int)value);
			}
	public static int ToInt32(short value)
			{
				return unchecked((int)value);
			}
	[CLSCompliant(false)]
	public static int ToInt32(ushort value)
			{
				return unchecked((int)value);
			}
	public static int ToInt32(int value) { return value; }
	[CLSCompliant(false)]
	public static int ToInt32(uint value)
			{
				if(value <= 2147483647)
				{
					return unchecked((int)value);
				}
				else
				{
					throw new OverflowException(_("Overflow_Int32"));
				}
			}
	public static int ToInt32(long value)
			{
				if(value >= -2147483648 && value <= 2147483647)
				{
					return unchecked((int)value);
				}
				else
				{
					throw new OverflowException(_("Overflow_Int32"));
				}
			}
	[CLSCompliant(false)]
	public static int ToInt32(ulong value)
			{
				if(value <= 2147483647)
				{
					return unchecked((int)value);
				}
				else
				{
					throw new OverflowException(_("Overflow_Int32"));
				}
			}
	public static int ToInt32(char value)
			{
				return unchecked((int)value);
			}
	public static int ToInt32(float value)
			{
				return ToInt32((double)value);
			}
	public static int ToInt32(double value)
			{
				// Let the runtime engine do the hard work
				// of detecting when overflow occurs.
				try
				{
					return checked((int)value);
				}
				catch(OverflowException)
				{
					// Convert the runtime exception into
					// one with a localized error string.
					throw new OverflowException(_("Overflow_Int32"));
				}
			}
	public static int ToInt32(Decimal value)
			{
				return Decimal.ToInt32(Decimal.Round(value, 0));
			}
	public static int ToInt32(String value)
			{
				return Int32.Parse(value);
			}
	public static int ToInt32(String value, IFormatProvider provider)
			{
				return Int32.Parse(value, provider);
			}
#if !ECMA_COMPAT
	public static int ToInt32(Object value)
			{
				if(value != null)
				{
					return ((IConvertible)value).ToInt32(null);
				}
				else
				{
					return 0;
				}
			}
	public static int ToInt32(Object value, IFormatProvider provider)
			{
				if(value != null)
				{
					return ((IConvertible)value).ToInt32(provider);
				}
				else
				{
					return 0;
				}
			}
	public static int ToInt32(String value, int fromBase)
			{
				if(fromBase == 2 || fromBase == 8 ||
				   fromBase == 10 || fromBase == 16)
				{
					return NumberParser.StringToInt32(value, fromBase, 0);
				}
				else
				{
					throw new ArgumentException(_("Arg_InvalidBase"));
				}
			}
#endif // !ECMA_COMPAT

	// Convert various types into UInt32.
	[CLSCompliant(false)]
	public static uint ToUInt32(bool value) { return (uint)(value ? 1 : 0); }
	[CLSCompliant(false)]
	public static uint ToUInt32(byte value)
			{
				return unchecked((uint)value);
			}
	[CLSCompliant(false)]
	public static uint ToUInt32(sbyte value)
			{
				if(value >= 0)
				{
					return unchecked((uint)value);
				}
				else
				{
					throw new OverflowException(_("Overflow_UInt32"));
				}
			}
	[CLSCompliant(false)]
	public static uint ToUInt32(short value)
			{
				if(value >= 0)
				{
					return unchecked((uint)value);
				}
				else
				{
					throw new OverflowException(_("Overflow_UInt32"));
				}
			}
	[CLSCompliant(false)]
	public static uint ToUInt32(ushort value)
			{
				return unchecked((uint)value);
			}
	[CLSCompliant(false)]
	public static uint ToUInt32(int value)
			{
				if(value >= 0)
				{
					return unchecked((uint)value);
				}
				else
				{
					throw new OverflowException(_("Overflow_UInt32"));
				}
			}
	[CLSCompliant(false)]
	public static uint ToUInt32(uint value) { return value; }
	[CLSCompliant(false)]
	public static uint ToUInt32(long value)
			{
				if(value >= 0 && value <= 4294967295)
				{
					return unchecked((uint)value);
				}
				else
				{
					throw new OverflowException(_("Overflow_UInt32"));
				}
			}
	[CLSCompliant(false)]
	public static uint ToUInt32(ulong value)
			{
				if(value <= 4294967295)
				{
					return unchecked((uint)value);
				}
				else
				{
					throw new OverflowException(_("Overflow_UInt32"));
				}
			}
	[CLSCompliant(false)]
	public static uint ToUInt32(char value)
			{
				return unchecked((uint)value);
			}
	[CLSCompliant(false)]
	public static uint ToUInt32(float value)
			{
				return ToUInt32((double)value);
			}
	[CLSCompliant(false)]
	public static uint ToUInt32(double value)
			{
				// Let the runtime engine do the hard work
				// of detecting when overflow occurs.
				try
				{
					return checked((uint)value);
				}
				catch(OverflowException)
				{
					// Convert the runtime exception into
					// one with a localized error string.
					throw new OverflowException(_("Overflow_UInt32"));
				}
			}
	[CLSCompliant(false)]
	public static uint ToUInt32(Decimal value)
			{
				return Decimal.ToUInt32(Decimal.Round(value, 0));
			}
	[CLSCompliant(false)]
	public static uint ToUInt32(String value)
			{
				return UInt32.Parse(value);
			}
	[CLSCompliant(false)]
	public static uint ToUInt32(String value, IFormatProvider provider)
			{
				return UInt32.Parse(value, provider);
			}
#if !ECMA_COMPAT
	[CLSCompliant(false)]
	public static uint ToUInt32(Object value)
			{
				if(value != null)
				{
					return ((IConvertible)value).ToUInt32(null);
				}
				else
				{
					return 0;
				}
			}
	[CLSCompliant(false)]
	public static uint ToUInt32(Object value, IFormatProvider provider)
			{
				if(value != null)
				{
					return ((IConvertible)value).ToUInt32(provider);
				}
				else
				{
					return 0;
				}
			}
	[CLSCompliant(false)]
	public static uint ToUInt32(String value, int fromBase)
			{
				if(fromBase == 2 || fromBase == 8 ||
				   fromBase == 10 || fromBase == 16)
				{
					return NumberParser.StringToUInt32(value, fromBase, 0);
				}
				else
				{
					throw new ArgumentException(_("Arg_InvalidBase"));
				}
			}
#endif // !ECMA_COMPAT

	// Convert various types into Int64.
	public static long ToInt64(bool value) { return (value ? 1 : 0); }
	public static long ToInt64(byte value)
			{
				return unchecked((long)value);
			}
	[CLSCompliant(false)]
	public static long ToInt64(sbyte value)
			{
				return unchecked((long)value);
			}
	public static long ToInt64(short value)
			{
				return unchecked((long)value);
			}
	[CLSCompliant(false)]
	public static long ToInt64(ushort value)
			{
				return unchecked((long)value);
			}
	public static long ToInt64(int value)
			{
				return unchecked((long)value);
			}
	[CLSCompliant(false)]
	public static long ToInt64(uint value)
			{
				return unchecked((long)value);
			}
	public static long ToInt64(long value) { return value; }
	[CLSCompliant(false)]
	public static long ToInt64(ulong value)
			{
				if(value <= 9223372036854775807)
				{
					return unchecked((long)value);
				}
				else
				{
					throw new OverflowException(_("Overflow_Int64"));
				}
			}
	public static long ToInt64(char value)
			{
				return unchecked((long)value);
			}
	public static long ToInt64(float value)
			{
				return ToInt64((double)value);
			}
	public static long ToInt64(double value)
			{
				// Let the runtime engine do the hard work
				// of detecting when overflow occurs.
				try
				{
					return checked((long)value);
				}
				catch(OverflowException)
				{
					// Convert the runtime exception into
					// one with a localized error string.
					throw new OverflowException(_("Overflow_Int64"));
				}
			}
	public static long ToInt64(Decimal value)
			{
				return Decimal.ToInt64(Decimal.Round(value, 0));
			}
	public static long ToInt64(String value)
			{
				return Int64.Parse(value);
			}
	public static long ToInt64(String value, IFormatProvider provider)
			{
				return Int64.Parse(value, provider);
			}
#if !ECMA_COMPAT
	public static long ToInt64(Object value)
			{
				if(value != null)
				{
					return ((IConvertible)value).ToInt64(null);
				}
				else
				{
					return 0;
				}
			}
	public static long ToInt64(Object value, IFormatProvider provider)
			{
				if(value != null)
				{
					return ((IConvertible)value).ToInt64(provider);
				}
				else
				{
					return 0;
				}
			}
	public static long ToInt64(String value, int fromBase)
			{
				if(fromBase == 2 || fromBase == 8 ||
				   fromBase == 10 || fromBase == 16)
				{
					return NumberParser.StringToInt64(value, fromBase);
				}
				else
				{
					throw new ArgumentException(_("Arg_InvalidBase"));
				}
			}
#endif // !ECMA_COMPAT

	// Convert various types into UInt64.
	[CLSCompliant(false)]
	public static ulong ToUInt64(bool value) { return (ulong)(value ? 1 : 0); }
	[CLSCompliant(false)]
	public static ulong ToUInt64(byte value)
			{
				return unchecked((ulong)value);
			}
	[CLSCompliant(false)]
	public static ulong ToUInt64(sbyte value)
			{
				if(value >= 0)
				{
					return unchecked((ulong)value);
				}
				else
				{
					throw new OverflowException(_("Overflow_UInt64"));
				}
			}
	[CLSCompliant(false)]
	public static ulong ToUInt64(short value)
			{
				if(value >= 0)
				{
					return unchecked((ulong)value);
				}
				else
				{
					throw new OverflowException(_("Overflow_UInt64"));
				}
			}
	[CLSCompliant(false)]
	public static ulong ToUInt64(ushort value)
			{
				return unchecked((ulong)value);
			}
	[CLSCompliant(false)]
	public static ulong ToUInt64(int value)
			{
				if(value >= 0)
				{
					return unchecked((ulong)value);
				}
				else
				{
					throw new OverflowException(_("Overflow_UInt64"));
				}
			}
	[CLSCompliant(false)]
	public static ulong ToUInt64(uint value)
			{
				return unchecked((ulong)value);
			}
	[CLSCompliant(false)]
	public static ulong ToUInt64(long value)
			{
				if(value >= 0)
				{
					return unchecked((ulong)value);
				}
				else
				{
					throw new OverflowException(_("Overflow_UInt64"));
				}
			}
	[CLSCompliant(false)]
	public static ulong ToUInt64(ulong value) { return value; }
	[CLSCompliant(false)]
	public static ulong ToUInt64(char value)
			{
				return unchecked((ulong)value);
			}
	[CLSCompliant(false)]
	public static ulong ToUInt64(float value)
			{
				return ToUInt64((double)value);
			}
	[CLSCompliant(false)]
	public static ulong ToUInt64(double value)
			{
				// Let the runtime engine do the hard work
				// of detecting when overflow occurs.
				try
				{
					return checked((ulong)value);
				}
				catch(OverflowException)
				{
					// Convert the runtime exception into
					// one with a localized error string.
					throw new OverflowException(_("Overflow_UInt64"));
				}
			}
	[CLSCompliant(false)]
	public static ulong ToUInt64(Decimal value)
			{
				return Decimal.ToUInt64(Decimal.Round(value, 0));
			}
	[CLSCompliant(false)]
	public static ulong ToUInt64(String value)
			{
				return UInt64.Parse(value);
			}
	[CLSCompliant(false)]
	public static ulong ToUInt64(String value, IFormatProvider provider)
			{
				return UInt64.Parse(value, provider);
			}
#if !ECMA_COMPAT
	[CLSCompliant(false)]
	public static ulong ToUInt64(Object value)
			{
				if(value != null)
				{
					return ((IConvertible)value).ToUInt64(null);
				}
				else
				{
					return 0;
				}
			}
	[CLSCompliant(false)]
	public static ulong ToUInt64(Object value, IFormatProvider provider)
			{
				if(value != null)
				{
					return ((IConvertible)value).ToUInt64(provider);
				}
				else
				{
					return 0;
				}
			}
	[CLSCompliant(false)]
	public static ulong ToUInt64(String value, int fromBase)
			{
				if(fromBase == 2 || fromBase == 8 ||
				   fromBase == 10 || fromBase == 16)
				{
					return NumberParser.StringToUInt64(value, fromBase);
				}
				else
				{
					throw new ArgumentException(_("Arg_InvalidBase"));
				}
			}
#endif // !ECMA_COMPAT

	// Convert various types into Char.
	public static char ToChar(byte value)
			{
				return unchecked((char)value);
			}
	[CLSCompliant(false)]
	public static char ToChar(sbyte value)
			{
				if(value >= 0)
				{
					return unchecked((char)value);
				}
				else
				{
					throw new OverflowException(_("Overflow_Char"));
				}
			}
	public static char ToChar(short value)
			{
				if(value >= 0)
				{
					return unchecked((char)value);
				}
				else
				{
					throw new OverflowException(_("Overflow_Char"));
				}
			}
	[CLSCompliant(false)]
	public static char ToChar(ushort value)
			{
				return unchecked((char)value);
			}
	public static char ToChar(int value)
			{
				if(value >= 0 && value <= 65535)
				{
					return unchecked((char)(ushort)value);
				}
				else
				{
					throw new OverflowException(_("Overflow_Char"));
				}
			}
	[CLSCompliant(false)]
	public static char ToChar(uint value)
			{
				if(value <= 65535)
				{
					return unchecked((char)value);
				}
				else
				{
					throw new OverflowException(_("Overflow_Char"));
				}
			}
	public static char ToChar(long value)
			{
				if(value >= 0 && value <= 65535)
				{
					return unchecked((char)value);
				}
				else
				{
					throw new OverflowException(_("Overflow_Char"));
				}
			}
	[CLSCompliant(false)]
	public static char ToChar(ulong value)
			{
				if(value <= 65535)
				{
					return unchecked((char)value);
				}
				else
				{
					throw new OverflowException(_("Overflow_Char"));
				}
			}
	public static char ToChar(char value) { return value; }
	public static char ToChar(String value)
			{
				if(value != null)
				{
					if(value.Length == 1)
					{
						return value[0];
					}
					else
					{
						throw new FormatException(_("Format_NeedSingleChar"));
					}
				}
				else
				{
					throw new ArgumentNullException("value");
				}
			}
#if !ECMA_COMPAT
	public static char ToChar(float value)
			{
				return ((IConvertible)value).ToChar(null);
			}
	public static char ToChar(double value)
			{
				return ((IConvertible)value).ToChar(null);
			}
	public static char ToChar(Decimal value)
			{
				return ((IConvertible)value).ToChar(null);
			}
	public static char ToChar(Object value)
			{
				if(value != null)
				{
					return ((IConvertible)value).ToChar(null);
				}
				else
				{
					return '\u0000';
				}
			}
	public static char ToChar(Object value, IFormatProvider provider)
			{
				if(value != null)
				{
					return ((IConvertible)value).ToChar(provider);
				}
				else
				{
					return '\u0000';
				}
			}
	public static char ToChar(String value, IFormatProvider provider)
			{
				return ToChar(value);
			}
#endif // !ECMA_COMPAT

	// Convert various types into Single.
	public static float ToSingle(bool value) { return (value ? 1.0f : 0.0f); }
	public static float ToSingle(byte value)
			{
				return unchecked((float)value);
			}
	[CLSCompliant(false)]
	public static float ToSingle(sbyte value)
			{
				return unchecked((float)value);
			}
	public static float ToSingle(short value)
			{
				return unchecked((float)value);
			}
	[CLSCompliant(false)]
	public static float ToSingle(ushort value)
			{
				return unchecked((float)value);
			}
	public static float ToSingle(int value)
			{
				return unchecked((float)value);
			}
	[CLSCompliant(false)]
	public static float ToSingle(uint value)
			{
				return unchecked((float)value);
			}
	public static float ToSingle(long value)
			{
				return unchecked((float)value);
			}
	[CLSCompliant(false)]
	public static float ToSingle(ulong value)
			{
				return unchecked((float)value);
			}
	public static float ToSingle(float value) { return value; }
	public static float ToSingle(double value)
			{
				return unchecked((float)value);
			}
	public static float ToSingle(Decimal value)
			{
				return (float)value;
			}
	public static float ToSingle(String value)
			{
				return Single.Parse(value);
			}
	public static float ToSingle(String value, IFormatProvider provider)
			{
				return Single.Parse(value, provider);
			}
#if !ECMA_COMPAT
	public static float ToSingle(char value)
			{
				return unchecked((float)value);
			}
	public static float ToSingle(Object value)
			{
				if(value != null)
				{
					return ((IConvertible)value).ToSingle(null);
				}
				else
				{
					return 0;
				}
			}
	public static float ToSingle(Object value, IFormatProvider provider)
			{
				if(value != null)
				{
					return ((IConvertible)value).ToSingle(provider);
				}
				else
				{
					return 0;
				}
			}
#endif // !ECMA_COMPAT

	// Convert various types into Double.
	public static double ToDouble(bool value) { return (value ? 1.0d : 0.0d); }
	public static double ToDouble(byte value)
			{
				return unchecked((double)value);
			}
	[CLSCompliant(false)]
	public static double ToDouble(sbyte value)
			{
				return unchecked((double)value);
			}
	public static double ToDouble(short value)
			{
				return unchecked((double)value);
			}
	[CLSCompliant(false)]
	public static double ToDouble(ushort value)
			{
				return unchecked((double)value);
			}
	public static double ToDouble(int value)
			{
				return unchecked((double)value);
			}
	[CLSCompliant(false)]
	public static double ToDouble(uint value)
			{
				return unchecked((double)value);
			}
	public static double ToDouble(long value)
			{
				return unchecked((double)value);
			}
	[CLSCompliant(false)]
	public static double ToDouble(ulong value)
			{
				return unchecked((double)value);
			}
	public static double ToDouble(float value)
			{
				return unchecked((double)value);
			}
	public static double ToDouble(double value) { return value; }
	public static double ToDouble(Decimal value)
			{
				return (double)value;
			}
	public static double ToDouble(String value)
			{
				return Double.Parse(value);
			}
	public static double ToDouble(String value, IFormatProvider provider)
			{
				return Double.Parse(value, provider);
			}
#if !ECMA_COMPAT
	public static double ToDouble(char value)
			{
				return unchecked((double)value);
			}
	public static double ToDouble(Object value)
			{
				if(value != null)
				{
					return ((IConvertible)value).ToDouble(null);
				}
				else
				{
					return 0;
				}
			}
	public static double ToDouble(Object value, IFormatProvider provider)
			{
				if(value != null)
				{
					return ((IConvertible)value).ToDouble(provider);
				}
				else
				{
					return 0;
				}
			}
#endif // !ECMA_COMPAT

	// Convert various types into String.
	public static String ToString(bool value)
			{
				return value.ToString();
			}
	public static String ToString(byte value)
			{
				return value.ToString();
			}
	public static String ToString(byte value, IFormatProvider provider)
			{
				return value.ToString(provider);
			}
	[CLSCompliant(false)]
	public static String ToString(sbyte value)
			{
				return value.ToString();
			}
	[CLSCompliant(false)]
	public static String ToString(sbyte value, IFormatProvider provider)
			{
				return value.ToString(provider);
			}
	public static String ToString(short value)
			{
				return value.ToString();
			}
	public static String ToString(short value, IFormatProvider provider)
			{
				return value.ToString(provider);
			}
	[CLSCompliant(false)]
	public static String ToString(ushort value)
			{
				return value.ToString();
			}
	[CLSCompliant(false)]
	public static String ToString(ushort value, IFormatProvider provider)
			{
				return value.ToString(provider);
			}
	public static String ToString(int value)
			{
				return value.ToString();
			}
	public static String ToString(int value, IFormatProvider provider)
			{
				return value.ToString(provider);
			}
	[CLSCompliant(false)]
	public static String ToString(uint value)
			{
				return value.ToString();
			}
	[CLSCompliant(false)]
	public static String ToString(uint value, IFormatProvider provider)
			{
				return value.ToString(provider);
			}
	public static String ToString(long value)
			{
				return value.ToString();
			}
	public static String ToString(long value, IFormatProvider provider)
			{
				return value.ToString(provider);
			}
	[CLSCompliant(false)]
	public static String ToString(ulong value)
			{
				return value.ToString();
			}
	[CLSCompliant(false)]
	public static String ToString(ulong value, IFormatProvider provider)
			{
				return value.ToString(provider);
			}
	public static String ToString(char value)
			{
				return value.ToString();
			}
	public static String ToString(float value)
			{
				return value.ToString();
			}
	public static String ToString(float value, IFormatProvider provider)
			{
				return value.ToString(provider);
			}
	public static String ToString(double value)
			{
				return value.ToString();
			}
	public static String ToString(double value, IFormatProvider provider)
			{
				return value.ToString(provider);
			}
	public static String ToString(Decimal value)
			{
				return value.ToString();
			}
	public static String ToString(Decimal value, IFormatProvider provider)
			{
				return value.ToString(provider);
			}
	public static String ToString(DateTime value)
			{
				return value.ToString();
			}
	public static String ToString(DateTime value, IFormatProvider provider)
			{
				return value.ToString(provider);
			}
	public static String ToString(String value)
			{
				return value;
			}
#if !ECMA_COMPAT
	public static String ToString(bool value, IFormatProvider provider)
			{
				return value.ToString(provider);
			}
	public static String ToString(char value, IFormatProvider provider)
			{
				return value.ToString(provider);
			}
	public static String ToString(byte value, int toBase)
			{
				return NumberFormatter.FormatInBase((long)value, toBase, 8);
			}
	[CLSCompliant(false)]
	public static String ToString(sbyte value, int toBase)
			{
				return NumberFormatter.FormatInBase((long)value, toBase, 8);
			}
	public static String ToString(short value, int toBase)
			{
				return NumberFormatter.FormatInBase((long)value, toBase, 16);
			}
	[CLSCompliant(false)]
	public static String ToString(ushort value, int toBase)
			{
				return NumberFormatter.FormatInBase((long)value, toBase, 16);
			}
	public static String ToString(int value, int toBase)
			{
				return NumberFormatter.FormatInBase((long)value, toBase, 32);
			}
	[CLSCompliant(false)]
	public static String ToString(uint value, int toBase)
			{
				return NumberFormatter.FormatInBase((long)value, toBase, 32);
			}
	public static String ToString(long value, int toBase)
			{
				return NumberFormatter.FormatInBase((long)value, toBase, 64);
			}
	[CLSCompliant(false)]
	public static String ToString(ulong value, int toBase)
			{
				return NumberFormatter.FormatInBaseUnsigned
							(value, toBase, 64);
			}
	public static String ToString(Object value)
			{
				if(value != null)
				{
					return value.ToString();
				}
				else
				{
					return String.Empty;
				}
			}
	public static String ToString(Object value, IFormatProvider provider)
			{
				IConvertible iconv = (value as IConvertible);
				if(iconv != null)
				{
					return iconv.ToString(provider);
				}
				else if(value != null)
				{
					return value.ToString();
				}
				else
				{
					return String.Empty;
				}
			}
#endif // !ECMA_COMPAT

	// Convert various types into DateTime.
	public static DateTime ToDateTime(DateTime value) { return value; }
	public static DateTime ToDateTime(String value)
			{
				return DateTime.Parse(value);
			}
	public static DateTime ToDateTime(String value, IFormatProvider provider)
			{
				return DateTime.Parse(value, provider);
			}
#if !ECMA_COMPAT
	public static DateTime ToDateTime(Object value)
			{
				if(value != null)
				{
					return ((IConvertible)value).ToDateTime(null);
				}
				else
				{
					return DateTime.MinValue;
				}
			}
#endif // !ECMA_COMPAT

	// Convert various types into Decimal.
	public static Decimal ToDecimal(bool value)
			{
				return (value ? 1.0m : 0.0m);
			}
	public static Decimal ToDecimal(byte value)
			{
				return (Decimal)value;
			}
	[CLSCompliant(false)]
	public static Decimal ToDecimal(sbyte value)
			{
				return (Decimal)value;
			}
	public static Decimal ToDecimal(short value)
			{
				return (Decimal)value;
			}
	[CLSCompliant(false)]
	public static Decimal ToDecimal(ushort value)
			{
				return (Decimal)value;
			}
	public static Decimal ToDecimal(int value)
			{
				return (Decimal)value;
			}
	[CLSCompliant(false)]
	public static Decimal ToDecimal(uint value)
			{
				return (Decimal)value;
			}
	public static Decimal ToDecimal(long value)
			{
				return (Decimal)value;
			}
	[CLSCompliant(false)]
	public static Decimal ToDecimal(ulong value)
			{
				return (Decimal)value;
			}
	public static Decimal ToDecimal(float value)
			{
				return (Decimal)value;
			}
	public static Decimal ToDecimal(double value)
			{
				return (Decimal)value;
			}
	public static Decimal ToDecimal(Decimal value) { return value; }
	public static Decimal ToDecimal(String value)
			{
				return Decimal.Parse(value);
			}
	public static Decimal ToDecimal(String value, IFormatProvider provider)
			{
				return Decimal.Parse(value, provider);
			}
#if !ECMA_COMPAT
	public static Decimal ToDecimal(char value)
			{
				return ((IConvertible)value).ToDecimal(null);
			}
	public static Decimal ToDecimal(Object value)
			{
				if(value != null)
				{
					return ((IConvertible)value).ToDecimal(null);
				}
				else
				{
					return new Decimal(0);
				}
			}
#endif // !ECMA_COMPAT

#if !ECMA_COMPAT

	// Change the type of an object.
	public static Object ChangeType(Object value, Type conversionType)
			{
				if(conversionType is IConvertible)
				{
					return ((IConvertible)value).ToType(conversionType, null);
				}
				else if(value != null || conversionType != null)
				{
					if(value.GetType() == conversionType)
					{
						return value;
					}
					else
					{
						throw new InvalidCastException
							(_("InvalidCast_IConvertible"));
					}
				}
				else
				{
					return null;
				}
			}
	public static Object ChangeType(Object value, TypeCode typeCode)
			{
				if(value is IConvertible)
				{
					IConvertible iconv = (IConvertible)value;
					switch(typeCode)
					{
						case TypeCode.Empty:
						{
							throw new InvalidCastException
								(_("InvalidCast_Empty"));
						}
						/* Not reached */

						case TypeCode.Object:
						{
							return value;
						}
						/* Not reached */

						case TypeCode.DBNull:
						{
							throw new InvalidCastException
								(_("InvalidCast_DBNull"));
						}
						/* Not reached */

						case TypeCode.Boolean:
						{
							return (Object)(iconv.ToBoolean(null));
						}
						/* Not reached */

						case TypeCode.Char:
						{
							return (Object)(iconv.ToChar(null));
						}
						/* Not reached */

						case TypeCode.SByte:
						{
							return (Object)(iconv.ToSByte(null));
						}
						/* Not reached */

						case TypeCode.Byte:
						{
							return (Object)(iconv.ToByte(null));
						}
						/* Not reached */

						case TypeCode.Int16:
						{
							return (Object)(iconv.ToInt16(null));
						}
						/* Not reached */

						case TypeCode.UInt16:
						{
							return (Object)(iconv.ToUInt16(null));
						}
						/* Not reached */

						case TypeCode.Int32:
						{
							return (Object)(iconv.ToInt32(null));
						}
						/* Not reached */

						case TypeCode.UInt32:
						{
							return (Object)(iconv.ToUInt32(null));
						}
						/* Not reached */

						case TypeCode.Int64:
						{
							return (Object)(iconv.ToInt64(null));
						}
						/* Not reached */

						case TypeCode.UInt64:
						{
							return (Object)(iconv.ToUInt64(null));
						}
						/* Not reached */

						case TypeCode.Single:
						{
							return (Object)(iconv.ToSingle(null));
						}
						/* Not reached */

						case TypeCode.Double:
						{
							return (Object)(iconv.ToDouble(null));
						}
						/* Not reached */

						case TypeCode.Decimal:
						{
							return (Object)(iconv.ToDecimal(null));
						}
						/* Not reached */

						case TypeCode.DateTime:
						{
							return (Object)(iconv.ToDateTime(null));
						}
						/* Not reached */

						case TypeCode.String:
						{
							return (Object)(iconv.ToString(null));
						}
						/* Not reached */

						default:
						{
							throw new ArgumentException
								(_("Arg_UnknownTypeCode"));
						}
						/* Not reached */
					}
				}
				else if(value != null || typeCode != TypeCode.Empty)
				{
					throw new InvalidCastException
						(_("InvalidCast_IConvertible"));
				}
				else
				{
					return null;
				}
			}

	// Default implementation of the "ToType" methods in
	// the primitive classes like Byte, Int32, Boolean, etc.
	internal static Object DefaultToType(IConvertible obj, Type targetType,
										 IFormatProvider provider)
			{
				if(targetType != null)
				{
					if(obj.GetType() == targetType)
					{
						return obj;
					}
					else if(targetType == ConvertTypes[(int)TypeCode.Boolean])
					{
						return (Object)(obj.ToBoolean(provider));
					}
					else if(targetType == ConvertTypes[(int)TypeCode.Char])
					{
						return (Object)(obj.ToChar(provider));
					}
					else if(targetType == ConvertTypes[(int)TypeCode.SByte])
					{
						return (Object)(obj.ToSByte(provider));
					}
					else if(targetType == ConvertTypes[(int)TypeCode.Byte])
					{
						return (Object)(obj.ToByte(provider));
					}
					else if(targetType == ConvertTypes[(int)TypeCode.Int16])
					{
						return (Object)(obj.ToInt16(provider));
					}
					else if(targetType == ConvertTypes[(int)TypeCode.UInt16])
					{
						return (Object)(obj.ToUInt16(provider));
					}
					else if(targetType == ConvertTypes[(int)TypeCode.Int32])
					{
						return (Object)(obj.ToInt32(provider));
					}
					else if(targetType == ConvertTypes[(int)TypeCode.UInt32])
					{
						return (Object)(obj.ToUInt32(provider));
					}
					else if(targetType == ConvertTypes[(int)TypeCode.Int64])
					{
						return (Object)(obj.ToInt64(provider));
					}
					else if(targetType == ConvertTypes[(int)TypeCode.UInt64])
					{
						return (Object)(obj.ToUInt64(provider));
					}
					else if(targetType == ConvertTypes[(int)TypeCode.Single])
					{
						return (Object)(obj.ToSingle(provider));
					}
					else if(targetType == ConvertTypes[(int)TypeCode.Double])
					{
						return (Object)(obj.ToDouble(provider));
					}
					else if(targetType == ConvertTypes[(int)TypeCode.Decimal])
					{
						return (Object)(obj.ToDecimal(provider));
					}
					else if(targetType == ConvertTypes[(int)TypeCode.DateTime])
					{
						return (Object)(obj.ToDateTime(provider));
					}
					else if(targetType == ConvertTypes[(int)TypeCode.String])
					{
						return (Object)(obj.ToString(provider));
					}
					else if(targetType == ConvertTypes[(int)TypeCode.Object])
					{
						return obj;
					}
					else if(targetType == ConvertTypes[(int)TypeCode.Empty])
					{
						throw new InvalidCastException
							(_("InvalidCast_Empty"));
					}
					else if(targetType == ConvertTypes[(int)TypeCode.DBNull])
					{
						throw new InvalidCastException
							(_("InvalidCast_DBNull"));
					}
					else
					{
						throw new InvalidCastException
							(String.Format
								(_("InvalidCast_FromTo"),
		 					     obj.GetType().FullName, targetType.FullName));
					}
				}
				else
				{
					throw new ArgumentNullException("targetType");
				}
			}

	// Miscellaneous methods.
	public static bool IsDBNull(Object value)
			{
				if(value is IConvertible)
				{
					return (((IConvertible)value).GetTypeCode() ==
									TypeCode.DBNull);
				}
				else
				{
					return false;
				}
			}
	public static bool IsEmpty(Object value)
			{
				return (value == null);
			}
	public static TypeCode GetTypeCode(Object value)
			{
				if(value != null)
				{
					if(value is IConvertible)
					{
						return ((IConvertible)value).GetTypeCode();
					}
					else
					{
						return TypeCode.Object;
					}
				}
				else
				{
					return TypeCode.Empty;
				}
			}

#endif // !ECMA_COMPAT

}; // class Convert

}; // namespace System
