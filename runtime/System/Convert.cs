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
	internal static readonly Type[] ConvertTypes =
		{typeof(Empty), typeof(Object), typeof(System.DBNull),
		 typeof(Boolean), typeof(Char), typeof(SByte), typeof(Byte),
		 typeof(Int16), typeof(UInt16), typeof(Int32), typeof(UInt32),
		 typeof(Int64), typeof(UInt64), typeof(Single), typeof(Double),
		 typeof(Decimal), typeof(DateTime), typeof(Object), typeof(String)};
	public static readonly Object DBNull = System.DBNull.Value;

	// Convert various types into Boolean.
	public static bool ToBoolean(bool value) { return value; }
	public static bool ToBoolean(byte value) { return (value != 0); }
	public static bool ToBoolean(sbyte value) { return (value != 0); }
	public static bool ToBoolean(short value) { return (value != 0); }
	public static bool ToBoolean(ushort value) { return (value != 0); }
	public static bool ToBoolean(int value) { return (value != 0); }
	public static bool ToBoolean(uint value) { return (value != 0); }
	public static bool ToBoolean(long value) { return (value != 0); }
	public static bool ToBoolean(ulong value) { return (value != 0); }
	public static bool ToBoolean(char value) { return value.ToBoolean(); }
	public static bool ToBoolean(float value) { return (value != 0.0); }
	public static bool ToBoolean(double value) { return (value != 0.0); }
	public static bool ToBoolean(Object value)
			{
				if(value != null)
				{
					return ((IConvertible)value).ToBoolean();
				}
				else
				{
					return false;
				}
			}
	public static bool ToBoolean(String value)
			{
				return Boolean.FromString(value);
			}

	// Convert various types into Byte.
	public static byte ToByte(bool value) { return (byte)(value ? 1 : 0); }
	public static byte ToByte(byte value) { return value; }
	public static byte ToByte(sbyte value)
			{
				if(value >= 0)
				{
					return unchecked((byte)value);
				}
				else
				{
					throw new OverflowException
						(Environment.GetResourceString("Overflow_Byte"));
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
					throw new OverflowException
						(Environment.GetResourceString("Overflow_Byte"));
				}
			}
	public static byte ToByte(ushort value)
			{
				if(value <= 255)
				{
					return unchecked((byte)value);
				}
				else
				{
					throw new OverflowException
						(Environment.GetResourceString("Overflow_Byte"));
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
					throw new OverflowException
						(Environment.GetResourceString("Overflow_Byte"));
				}
			}
	public static byte ToByte(uint value)
			{
				if(value <= 255)
				{
					return unchecked((byte)value);
				}
				else
				{
					throw new OverflowException
						(Environment.GetResourceString("Overflow_Byte"));
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
					throw new OverflowException
						(Environment.GetResourceString("Overflow_Byte"));
				}
			}
	public static byte ToByte(ulong value)
			{
				if(value <= 255)
				{
					return unchecked((byte)value);
				}
				else
				{
					throw new OverflowException
						(Environment.GetResourceString("Overflow_Byte"));
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
					throw new OverflowException
						(Environment.GetResourceString("Overflow_Byte"));
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
				return (Decimal.Round(value, 0)).ToByte();
			}
	public static byte ToByte(Object value)
			{
				if(value != null)
				{
					return ((IConvertible)value).ToByte();
				}
				else
				{
					return 0;
				}
			}
	public static byte ToByte(String value)
			{
				return Byte.FromString(value);
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
					throw new ArgumentException
						(Environment.GetResourceString("Arg_InvalidBase"));
				}
			}

	// Convert various types into SByte.
	public static sbyte ToSByte(bool value) { return (sbyte)(value ? 1 : 0); }
	public static sbyte ToSByte(byte value)
			{
				if(value <= 127)
				{
					return unchecked((sbyte)value);
				}
				else
				{
					throw new OverflowException
						(Environment.GetResourceString("Overflow_SByte"));
				}
			}
	public static sbyte ToSByte(sbyte value)	{ return value; }
	public static sbyte ToSByte(short value)
			{
				if(value >= -128 && value <= 127)
				{
					return unchecked((sbyte)value);
				}
				else
				{
					throw new OverflowException
						(Environment.GetResourceString("Overflow_SByte"));
				}
			}
	public static sbyte ToSByte(ushort value)
			{
				if(value <= 127)
				{
					return unchecked((sbyte)value);
				}
				else
				{
					throw new OverflowException
						(Environment.GetResourceString("Overflow_SByte"));
				}
			}
	public static sbyte ToSByte(int value)
			{
				if(value >= -128 && value <= 127)
				{
					return unchecked((sbyte)value);
				}
				else
				{
					throw new OverflowException
						(Environment.GetResourceString("Overflow_SByte"));
				}
			}
	public static sbyte ToSByte(uint value)
			{
				if(value <= 127)
				{
					return unchecked((sbyte)value);
				}
				else
				{
					throw new OverflowException
						(Environment.GetResourceString("Overflow_SByte"));
				}
			}
	public static sbyte ToSByte(long value)
			{
				if(value >= -127 && value <= 127)
				{
					return unchecked((sbyte)value);
				}
				else
				{
					throw new OverflowException
						(Environment.GetResourceString("Overflow_SByte"));
				}
			}
	public static sbyte ToSByte(ulong value)
			{
				if(value <= 127)
				{
					return unchecked((sbyte)value);
				}
				else
				{
					throw new OverflowException
						(Environment.GetResourceString("Overflow_SByte"));
				}
			}
	public static sbyte ToSByte(char value)
			{
				if(value <= 127)
				{
					return unchecked((sbyte)value);
				}
				else
				{
					throw new OverflowException
						(Environment.GetResourceString("Overflow_SByte"));
				}
			}
	public static sbyte ToSByte(float value)
			{
				return ToSByte((double)value);
			}
	public static sbyte ToSByte(double value)
			{
				return ToSByte(ToInt32(value));
			}
	public static sbyte ToSByte(Decimal value)
			{
				return (Decimal.Round(value, 0)).ToSByte();
			}
	public static sbyte ToSByte(Object value)
			{
				if(value != null)
				{
					return ((IConvertible)value).ToSByte();
				}
				else
				{
					return 0;
				}
			}
	public static sbyte ToSByte(String value)
			{
				return SByte.FromString(value);
			}
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
					throw new ArgumentException
						(Environment.GetResourceString("Arg_InvalidBase"));
				}
			}

	// Convert various types into Int16.
	public static short ToInt16(bool value) { return (short)(value ? 1 : 0); }
	public static short ToInt16(byte value)
			{
				return unchecked((short)value);
			}
	public static short ToInt16(sbyte value)
			{
				return unchecked((short)value);
			}
	public static short ToInt16(short value) { return value; }
	public static short ToInt16(ushort value)
			{
				if(value <= 32767)
				{
					return unchecked((short)value);
				}
				else
				{
					throw new OverflowException
						(Environment.GetResourceString("Overflow_Int16"));
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
					throw new OverflowException
						(Environment.GetResourceString("Overflow_Int16"));
				}
			}
	public static short ToInt16(uint value)
			{
				if(value <= 32767)
				{
					return unchecked((short)value);
				}
				else
				{
					throw new OverflowException
						(Environment.GetResourceString("Overflow_Int16"));
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
					throw new OverflowException
						(Environment.GetResourceString("Overflow_Int16"));
				}
			}
	public static short ToInt16(ulong value)
			{
				if(value <= 32767)
				{
					return unchecked((short)value);
				}
				else
				{
					throw new OverflowException
						(Environment.GetResourceString("Overflow_Int16"));
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
					throw new OverflowException
						(Environment.GetResourceString("Overflow_Int16"));
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
				return (Decimal.Round(value, 0)).ToInt16();
			}
	public static short ToInt16(Object value)
			{
				if(value != null)
				{
					return ((IConvertible)value).ToInt16();
				}
				else
				{
					return 0;
				}
			}
	public static short ToInt16(String value)
			{
				return Int16.FromString(value);
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
					throw new ArgumentException
						(Environment.GetResourceString("Arg_InvalidBase"));
				}
			}

	// Convert various types into UInt16.
	public static ushort ToUInt16(bool value)
			{
				return (ushort)(value ? 1 : 0);
			}
	public static ushort ToUInt16(byte value)
			{
				return unchecked((ushort)value);
			}
	public static ushort ToUInt16(sbyte value)
			{
				if(value >= 0)
				{
					return unchecked((ushort)value);
				}
				else
				{
					throw new OverflowException
						(Environment.GetResourceString("Overflow_UInt16"));
				}
			}
	public static ushort ToUInt16(short value)
			{
				if(value >= 0)
				{
					return unchecked((ushort)value);
				}
				else
				{
					throw new OverflowException
						(Environment.GetResourceString("Overflow_UInt16"));
				}
			}
	public static ushort ToUInt16(ushort value) { return value; }
	public static ushort ToUInt16(int value)
			{
				if(value >= 0 && value <= 65535)
				{
					return unchecked((ushort)value);
				}
				else
				{
					throw new OverflowException
						(Environment.GetResourceString("Overflow_UInt16"));
				}
			}
	public static ushort ToUInt16(uint value)
			{
				if(value <= 65535)
				{
					return unchecked((ushort)value);
				}
				else
				{
					throw new OverflowException
						(Environment.GetResourceString("Overflow_UInt16"));
				}
			}
	public static ushort ToUInt16(long value)
			{
				if(value >= 0 && value <= 65535)
				{
					return unchecked((ushort)value);
				}
				else
				{
					throw new OverflowException
						(Environment.GetResourceString("Overflow_UInt16"));
				}
			}
	public static ushort ToUInt16(ulong value)
			{
				if(value <= 65535)
				{
					return unchecked((ushort)value);
				}
				else
				{
					throw new OverflowException
						(Environment.GetResourceString("Overflow_UInt16"));
				}
			}
	public static ushort ToUInt16(char value)
			{
				return unchecked((ushort)value);
			}
	public static ushort ToUInt16(float value)
			{
				return ToUInt16((double)value);
			}
	public static ushort ToUInt16(double value)
			{
				return ToUInt16(ToInt32(value));
			}
	public static ushort ToUInt16(Decimal value)
			{
				return (Decimal.Round(value, 0)).ToUInt16();
			}
	public static ushort ToUInt16(Object value)
			{
				if(value != null)
				{
					return ((IConvertible)value).ToUInt16();
				}
				else
				{
					return 0;
				}
			}
	public static ushort ToUInt16(String value)
			{
				return UInt16.FromString(value);
			}
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
					throw new ArgumentException
						(Environment.GetResourceString("Arg_InvalidBase"));
				}
			}

	// Convert various types into Int32.
	public static int ToInt32(bool value) { return (value ? 1 : 0); }
	public static int ToInt32(byte value)
			{
				return unchecked((int)value);
			}
	public static int ToInt32(sbyte value)
			{
				return unchecked((int)value);
			}
	public static int ToInt32(short value)
			{
				return unchecked((int)value);
			}
	public static int ToInt32(ushort value)
			{
				return unchecked((int)value);
			}
	public static int ToInt32(int value) { return value; }
	public static int ToInt32(uint value)
			{
				if(value <= 2147483647)
				{
					return unchecked((int)value);
				}
				else
				{
					throw new OverflowException
						(Environment.GetResourceString("Overflow_Int32"));
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
					throw new OverflowException
						(Environment.GetResourceString("Overflow_Int32"));
				}
			}
	public static int ToInt32(ulong value)
			{
				if(value <= 2147483647)
				{
					return unchecked((int)value);
				}
				else
				{
					throw new OverflowException
						(Environment.GetResourceString("Overflow_Int32"));
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
					throw new OverflowException
						(Environment.GetResourceString("Overflow_Int32"));
				}
			}
	public static int ToInt32(Decimal value)
			{
				return (Decimal.Round(value, 0)).ToInt32();
			}
	public static int ToInt32(Object value)
			{
				if(value != null)
				{
					return ((IConvertible)value).ToInt32();
				}
				else
				{
					return 0;
				}
			}
	public static int ToInt32(String value)
			{
				return Int32.FromString(value);
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
					throw new ArgumentException
						(Environment.GetResourceString("Arg_InvalidBase"));
				}
			}

	// Convert various types into UInt32.
	public static uint ToUInt32(bool value) { return (uint)(value ? 1 : 0); }
	public static uint ToUInt32(byte value)
			{
				return unchecked((uint)value);
			}
	public static uint ToUInt32(sbyte value)
			{
				if(value >= 0)
				{
					return unchecked((uint)value);
				}
				else
				{
					throw new OverflowException
						(Environment.GetResourceString("Overflow_UInt32"));
				}
			}
	public static uint ToUInt32(short value)
			{
				if(value >= 0)
				{
					return unchecked((uint)value);
				}
				else
				{
					throw new OverflowException
						(Environment.GetResourceString("Overflow_UInt32"));
				}
			}
	public static uint ToUInt32(ushort value)
			{
				return unchecked((uint)value);
			}
	public static uint ToUInt32(int value)
			{
				if(value >= 0)
				{
					return unchecked((uint)value);
				}
				else
				{
					throw new OverflowException
						(Environment.GetResourceString("Overflow_UInt32"));
				}
			}
	public static uint ToUInt32(uint value) { return value; }
	public static uint ToUInt32(long value)
			{
				if(value >= 0 && value <= 4294967295)
				{
					return unchecked((uint)value);
				}
				else
				{
					throw new OverflowException
						(Environment.GetResourceString("Overflow_UInt32"));
				}
			}
	public static uint ToUInt32(ulong value)
			{
				if(value <= 4294967295)
				{
					return unchecked((uint)value);
				}
				else
				{
					throw new OverflowException
						(Environment.GetResourceString("Overflow_UInt32"));
				}
			}
	public static uint ToUInt32(char value)
			{
				return unchecked((uint)value);
			}
	public static uint ToUInt32(float value)
			{
				return ToUInt32((double)value);
			}
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
					throw new OverflowException
						(Environment.GetResourceString("Overflow_UInt32"));
				}
			}
	public static uint ToUInt32(Decimal value)
			{
				return (Decimal.Round(value, 0)).ToUInt32();
			}
	public static uint ToUInt32(Object value)
			{
				if(value != null)
				{
					return ((IConvertible)value).ToUInt32();
				}
				else
				{
					return 0;
				}
			}
	public static uint ToUInt32(String value)
			{
				return UInt32.FromString(value);
			}
	public static uint ToUInt32(String value, int fromBase)
			{
				if(fromBase == 2 || fromBase == 8 ||
				   fromBase == 10 || fromBase == 16)
				{
					return NumberParser.StringToUInt32(value, fromBase, 0);
				}
				else
				{
					throw new ArgumentException
						(Environment.GetResourceString("Arg_InvalidBase"));
				}
			}

	// Convert various types into Int64.
	public static long ToInt64(bool value) { return (value ? 1 : 0); }
	public static long ToInt64(byte value)
			{
				return unchecked((long)value);
			}
	public static long ToInt64(sbyte value)
			{
				return unchecked((long)value);
			}
	public static long ToInt64(short value)
			{
				return unchecked((long)value);
			}
	public static long ToInt64(ushort value)
			{
				return unchecked((long)value);
			}
	public static long ToInt64(int value)
			{
				return unchecked((long)value);
			}
	public static long ToInt64(uint value)
			{
				return unchecked((long)value);
			}
	public static long ToInt64(long value) { return value; }
	public static long ToInt64(ulong value)
			{
				if(value <= 9223372036854775807)
				{
					return unchecked((long)value);
				}
				else
				{
					throw new OverflowException
						(Environment.GetResourceString("Overflow_Int64"));
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
					throw new OverflowException
						(Environment.GetResourceString("Overflow_Int64"));
				}
			}
	public static long ToInt64(Decimal value)
			{
				return (Decimal.Round(value, 0)).ToInt64();
			}
	public static long ToInt64(Object value)
			{
				if(value != null)
				{
					return ((IConvertible)value).ToInt64();
				}
				else
				{
					return 0;
				}
			}
	public static long ToInt64(String value)
			{
				return Int64.FromString(value);
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
					throw new ArgumentException
						(Environment.GetResourceString("Arg_InvalidBase"));
				}
			}

	// Convert various types into UInt64.
	public static ulong ToUInt64(bool value) { return (ulong)(value ? 1 : 0); }
	public static ulong ToUInt64(byte value)
			{
				return unchecked((ulong)value);
			}
	public static ulong ToUInt64(sbyte value)
			{
				if(value >= 0)
				{
					return unchecked((ulong)value);
				}
				else
				{
					throw new OverflowException
						(Environment.GetResourceString("Overflow_UInt64"));
				}
			}
	public static ulong ToUInt64(short value)
			{
				if(value >= 0)
				{
					return unchecked((ulong)value);
				}
				else
				{
					throw new OverflowException
						(Environment.GetResourceString("Overflow_UInt64"));
				}
			}
	public static ulong ToUInt64(ushort value)
			{
				return unchecked((ulong)value);
			}
	public static ulong ToUInt64(int value)
			{
				if(value >= 0)
				{
					return unchecked((ulong)value);
				}
				else
				{
					throw new OverflowException
						(Environment.GetResourceString("Overflow_UInt64"));
				}
			}
	public static ulong ToUInt64(uint value)
			{
				return unchecked((ulong)value);
			}
	public static ulong ToUInt64(long value)
			{
				if(value >= 0)
				{
					return unchecked((ulong)value);
				}
				else
				{
					throw new OverflowException
						(Environment.GetResourceString("Overflow_UInt64"));
				}
			}
	public static ulong ToUInt64(ulong value) { return value; }
	public static ulong ToUInt64(char value)
			{
				return unchecked((ulong)value);
			}
	public static ulong ToUInt64(float value)
			{
				return ToUInt64((double)value);
			}
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
					throw new OverflowException
						(Environment.GetResourceString("Overflow_UInt64"));
				}
			}
	public static ulong ToUInt64(Decimal value)
			{
				return (Decimal.Round(value, 0)).ToUInt64();
			}
	public static ulong ToUInt64(Object value)
			{
				if(value != null)
				{
					return ((IConvertible)value).ToUInt64();
				}
				else
				{
					return 0;
				}
			}
	public static ulong ToUInt64(String value)
			{
				return UInt64.FromString(value);
			}
	public static ulong ToUInt64(String value, int fromBase)
			{
				if(fromBase == 2 || fromBase == 8 ||
				   fromBase == 10 || fromBase == 16)
				{
					return NumberParser.StringToUInt64(value, fromBase);
				}
				else
				{
					throw new ArgumentException
						(Environment.GetResourceString("Arg_InvalidBase"));
				}
			}

	// Convert various types into Char.
	public static char ToChar(byte value)
			{
				return unchecked((char)value);
			}
	public static char ToChar(sbyte value)
			{
				if(value >= 0)
				{
					return unchecked((char)value);
				}
				else
				{
					throw new OverflowException
						(Environment.GetResourceString("Overflow_Char"));
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
					throw new OverflowException
						(Environment.GetResourceString("Overflow_Char"));
				}
			}
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
					throw new OverflowException
						(Environment.GetResourceString("Overflow_Char"));
				}
			}
	public static char ToChar(uint value)
			{
				if(value <= 65535)
				{
					return unchecked((char)value);
				}
				else
				{
					throw new OverflowException
						(Environment.GetResourceString("Overflow_Char"));
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
					throw new OverflowException
						(Environment.GetResourceString("Overflow_Char"));
				}
			}
	public static char ToChar(ulong value)
			{
				if(value <= 65535)
				{
					return unchecked((char)value);
				}
				else
				{
					throw new OverflowException
						(Environment.GetResourceString("Overflow_Char"));
				}
			}
	public static char ToChar(char value) { return value; }
	public static char ToChar(float value)
			{
				return value.ToChar();
			}
	public static char ToChar(double value)
			{
				return value.ToChar();
			}
	public static char ToChar(Decimal value)
			{
				return value.ToChar();
			}
	public static char ToChar(Object value)
			{
				if(value != null)
				{
					return ((IConvertible)value).ToChar();
				}
				else
				{
					return '\u0000';
				}
			}
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
						throw new FormatException
							(Environment.GetResourceString
								("Format_NeedSingleChar"));
					}
				}
				else
				{
					throw new ArgumentNullException("value");
				}
			}

	// Convert various types into Single.
	public static float ToSingle(byte value)
			{
				return unchecked((float)value);
			}
	public static float ToSingle(sbyte value)
			{
				return unchecked((float)value);
			}
	public static float ToSingle(short value)
			{
				return unchecked((float)value);
			}
	public static float ToSingle(ushort value)
			{
				return unchecked((float)value);
			}
	public static float ToSingle(int value)
			{
				return unchecked((float)value);
			}
	public static float ToSingle(uint value)
			{
				return unchecked((float)value);
			}
	public static float ToSingle(long value)
			{
				return unchecked((float)value);
			}
	public static float ToSingle(ulong value)
			{
				return unchecked((float)value);
			}
	public static float ToSingle(char value)
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
	public static float ToSingle(Object value)
			{
				if(value != null)
				{
					return ((IConvertible)value).ToSingle();
				}
				else
				{
					return 0;
				}
			}
	public static float ToSingle(String value)
			{
				return Single.FromString(value);
			}

	// Convert various types into Double.
	public static double ToDouble(byte value)
			{
				return unchecked((double)value);
			}
	public static double ToDouble(sbyte value)
			{
				return unchecked((double)value);
			}
	public static double ToDouble(short value)
			{
				return unchecked((double)value);
			}
	public static double ToDouble(ushort value)
			{
				return unchecked((double)value);
			}
	public static double ToDouble(int value)
			{
				return unchecked((double)value);
			}
	public static double ToDouble(uint value)
			{
				return unchecked((double)value);
			}
	public static double ToDouble(long value)
			{
				return unchecked((double)value);
			}
	public static double ToDouble(ulong value)
			{
				return unchecked((double)value);
			}
	public static double ToDouble(char value)
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
	public static double ToDouble(Object value)
			{
				if(value != null)
				{
					return ((IConvertible)value).ToDouble();
				}
				else
				{
					return 0;
				}
			}
	public static double ToDouble(String value)
			{
				return Double.FromString(value);
			}

	// Convert various types into String.
	public static String ToString(bool value)
			{
				return Boolean.ToString(value);
			}
	public static String ToString(byte value)
			{
				return Byte.ToString(value);
			}
	public static String ToString(sbyte value)
			{
				return SByte.ToString(value);
			}
	public static String ToString(short value)
			{
				return Int16.ToString(value);
			}
	public static String ToString(ushort value)
			{
				return UInt16.ToString(value);
			}
	public static String ToString(int value)
			{
				return Int32.ToString(value);
			}
	public static String ToString(uint value)
			{
				return UInt32.ToString(value);
			}
	public static String ToString(long value)
			{
				return Int64.ToString(value);
			}
	public static String ToString(ulong value)
			{
				return UInt64.ToString(value);
			}
	public static String ToString(char value)
			{
				return Char.ToString(value);
			}
	public static String ToString(float value)
			{
				return Single.ToString(value);
			}
	public static String ToString(double value)
			{
				return Double.ToString(value);
			}
	public static String ToString(Decimal value)
			{
				return Decimal.ToString(value);
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
	public static String ToString(DateTime value)
			{
				return DateTime.ToString(value);
			}
	public static String ToString(TimeSpan value)
			{
				return value.ToString();
			}

	// Convert various types into DateTime.
	public static DateTime ToDateTime(Object value)
			{
				if(value != null)
				{
					return ((IConvertible)value).ToDateTime();
				}
				else
				{
					return DateTime.Empty;
				}
			}
	public static DateTime ToDateTime(String value)
			{
				return DateTime.FromString(value);
			}

	// Convert various types into Decimal.
	public static Decimal ToDecimal(byte value)
			{
				return (Decimal)value;
			}
	public static Decimal ToDecimal(sbyte value)
			{
				return (Decimal)value;
			}
	public static Decimal ToDecimal(short value)
			{
				return (Decimal)value;
			}
	public static Decimal ToDecimal(ushort value)
			{
				return (Decimal)value;
			}
	public static Decimal ToDecimal(int value)
			{
				return (Decimal)value;
			}
	public static Decimal ToDecimal(uint value)
			{
				return (Decimal)value;
			}
	public static Decimal ToDecimal(long value)
			{
				return (Decimal)value;
			}
	public static Decimal ToDecimal(ulong value)
			{
				return (Decimal)value;
			}
	public static Decimal ToDecimal(char value)
			{
				return value.ToDecimal();
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
	public static Decimal ToDecimal(Object value)
			{
				if(value != null)
				{
					return ((IConvertible)value).ToDecimal();
				}
				else
				{
					return new Decimal(0);
				}
			}
	public static Decimal ToDecimal(String value)
			{
				return Decimal.FromString(value);
			}

	// Change the type of an object.
	public static Object ChangeType(Object value, Type conversionType)
			{
				if(conversionType is IConvertible)
				{
					return ((IConvertible)value).ToType(conversionType);
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
							(Environment.GetResourceString
								("InvalidCast_IConvertible"));
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
								(Environment.GetResourceString
									("InvalidCast_Empty"));
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
								(Environment.GetResourceString
									("InvalidCast_DBNull"));
						}
						/* Not reached */

						case TypeCode.Boolean:
						{
							return (Object)(iconv.ToBoolean());
						}
						/* Not reached */

						case TypeCode.Char:
						{
							return (Object)(iconv.ToChar());
						}
						/* Not reached */

						case TypeCode.SByte:
						{
							return (Object)(iconv.ToSByte());
						}
						/* Not reached */

						case TypeCode.Byte:
						{
							return (Object)(iconv.ToByte());
						}
						/* Not reached */

						case TypeCode.Int16:
						{
							return (Object)(iconv.ToInt16());
						}
						/* Not reached */

						case TypeCode.UInt16:
						{
							return (Object)(iconv.ToUInt16());
						}
						/* Not reached */

						case TypeCode.Int32:
						{
							return (Object)(iconv.ToInt32());
						}
						/* Not reached */

						case TypeCode.UInt32:
						{
							return (Object)(iconv.ToUInt32());
						}
						/* Not reached */

						case TypeCode.Int64:
						{
							return (Object)(iconv.ToInt64());
						}
						/* Not reached */

						case TypeCode.UInt64:
						{
							return (Object)(iconv.ToUInt64());
						}
						/* Not reached */

						case TypeCode.Single:
						{
							return (Object)(iconv.ToSingle());
						}
						/* Not reached */

						case TypeCode.Double:
						{
							return (Object)(iconv.ToDouble());
						}
						/* Not reached */

						case TypeCode.Decimal:
						{
							return (Object)(iconv.ToDecimal());
						}
						/* Not reached */

						case TypeCode.DateTime:
						{
							return (Object)(iconv.ToDateTime());
						}
						/* Not reached */

						case TypeCode.String:
						{
							return (Object)(iconv.ToString());
						}
						/* Not reached */

						default:
						{
							throw new ArgumentException
								(Environment.GetResourceString
									("Arg_UnknownTypeCode"));
						}
						/* Not reached */
					}
				}
				else if(value != null || typeCode != TypeCode.Empty)
				{
					throw new InvalidCastException
						(Environment.GetResourceString
							("InvalidCast_IConvertible"));
				}
				else
				{
					return null;
				}
			}

	// Default implementation of the "ToType" methods in
	// the primitive classes like Byte, Int32, Boolean, etc.
	internal static Object DefaultToType(IConvertible obj, Type targetType)
			{
				if(targetType != null)
				{
					if(obj.GetType() == targetType)
					{
						return obj;
					}
					else if(targetType == ConvertTypes[(int)TypeCode.Boolean])
					{
						return (Object)(obj.ToBoolean());
					}
					else if(targetType == ConvertTypes[(int)TypeCode.Char])
					{
						return (Object)(obj.ToChar());
					}
					else if(targetType == ConvertTypes[(int)TypeCode.SByte])
					{
						return (Object)(obj.ToSByte());
					}
					else if(targetType == ConvertTypes[(int)TypeCode.Byte])
					{
						return (Object)(obj.ToByte());
					}
					else if(targetType == ConvertTypes[(int)TypeCode.Int16])
					{
						return (Object)(obj.ToInt16());
					}
					else if(targetType == ConvertTypes[(int)TypeCode.UInt16])
					{
						return (Object)(obj.ToUInt16());
					}
					else if(targetType == ConvertTypes[(int)TypeCode.Int32])
					{
						return (Object)(obj.ToInt32());
					}
					else if(targetType == ConvertTypes[(int)TypeCode.UInt32])
					{
						return (Object)(obj.ToUInt32());
					}
					else if(targetType == ConvertTypes[(int)TypeCode.Int64])
					{
						return (Object)(obj.ToInt64());
					}
					else if(targetType == ConvertTypes[(int)TypeCode.UInt64])
					{
						return (Object)(obj.ToUInt64());
					}
					else if(targetType == ConvertTypes[(int)TypeCode.Single])
					{
						return (Object)(obj.ToSingle());
					}
					else if(targetType == ConvertTypes[(int)TypeCode.Double])
					{
						return (Object)(obj.ToDouble());
					}
					else if(targetType == ConvertTypes[(int)TypeCode.Decimal])
					{
						return (Object)(obj.ToDecimal());
					}
					else if(targetType == ConvertTypes[(int)TypeCode.DateTime])
					{
						return (Object)(obj.ToDateTime());
					}
					else if(targetType == ConvertTypes[(int)TypeCode.String])
					{
						return (Object)(obj.ToString());
					}
					else if(targetType == ConvertTypes[(int)TypeCode.Object])
					{
						return obj;
					}
					else if(targetType == ConvertTypes[(int)TypeCode.Empty])
					{
						throw new InvalidCastException
							(Environment.GetResourceString
								("InvalidCast_Empty"));
					}
					else if(targetType == ConvertTypes[(int)TypeCode.DBNull])
					{
						throw new InvalidCastException
							(Environment.GetResourceString
								("InvalidCast_DBNull"));
					}
					else
					{
						throw new InvalidCastException
							(String.Format
								(Environment.GetResourceString
									("InvalidCast_FromTo"),
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

}; // class Convert

}; // namespace System
