/*
 * DateTime.cs - Implementation of the "System.DateTime" class.
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

using System.Globalization;
using Platform;

public struct DateTime : IComparable, IConvertible, IFormattable
{

	private long value__;

	// Days in each month in regular years.
	private static readonly int[] daysForEachMonth
		= {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

	// Days before each month in regular years.
	private static readonly int[] daysBeforeMonth
		= {0,
		   31,
		   31 + 28,
		   31 + 28 + 31,
		   31 + 28 + 31 + 30,
		   31 + 28 + 31 + 30 + 31,
		   31 + 28 + 31 + 30 + 31 + 30,
		   31 + 28 + 31 + 30 + 31 + 30 + 31,
		   31 + 28 + 31 + 30 + 31 + 30 + 31 + 31,
		   31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30,
		   31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31,
		   31 + 28 + 31 + 30 + 31 + 30 + 31 + 31 + 30 + 31 + 30};

	// Internal useful constants.
	private const long ticksPerDay = (10000000L * 60L * 60L * 24L);

	// Maximum microseconds value.
	private static readonly long maxTicks =
			(DateToTicks(9999, 12, 31) + ticksPerDay - 1);

	// Public constants.
	public static readonly DateTime MinValue = new DateTime(0);
	public static readonly DateTime MaxValue = new DateTime(maxTicks);

	// Convert a YMD date into a number of ticks.
	private static long DateToTicks(int year, int month, int day)
			{
				int daysInMonth;
				long result;
				bool isLeap;
				if(year >= 1 && year <= 9999 &&
				   month >= 1 && month <= 12)
				{
					isLeap = IsLeapYear(year);
					daysInMonth = daysForEachMonth[month - 1];
					if(month == 2 && isLeap)
					{
						++daysInMonth;
					}
					if(day >= 1 && day <= daysInMonth)
					{
						unchecked
						{
							--year;
							result = (long)(year * 365 + year / 4 -
											year / 100 + year / 400);
							result += (long)(daysBeforeMonth[month - 1]);
							if(month > 2 && isLeap)
							{
								++result;
							}
							return (result + (long)(day - 1)) * ticksPerDay;
						}
					}
				}
				throw new ArgumentOutOfRangeException
					(Environment.GetResourceString("ArgRange_YMD"));
			}

	// Convert a time into a number of ticks.
	private static long TimeToTicks(int hour, int minute, int second)
			{
				if(hour >= 0 && hour <= 23 &&
				   minute >= 0 && minute <= 59 &&
				   second >= 0 && minute <= 59)
				{
					unchecked
					{
						return ((long)(hour * 3600 + minute * 60 + second))
									* 10000000L;
					}
				}
				throw new ArgumentOutOfRangeException
					(Environment.GetResourceString("ArgRange_HMS"));
			}

	// Constructors.
	public DateTime(long ticks)
			{
				if(ticks >= 0 && ticks <= maxTicks)
				{
					value__ = ticks;
				}
				else
				{
					throw new ArgumentOutOfRangeException
						("ticks",
						 Environment.GetResourceString
						 	("ArgRange_DateTimeTicks"));
				}
			}
	public DateTime(int year, int month, int day)
			{
				value__ = DateToTicks(year, month, day);
			}
	public DateTime(int year, int month, int day,
					int hour, int minute, int second)
			{
				value__ = DateToTicks(year, month, day) +
						  TimeToTicks(hour, minute, second);
			}
	public DateTime(int year, int month, int day,
					int hour, int minute, int second, int millisecond)
			{
				unchecked
				{
					value__ = DateToTicks(year, month, day) +
							  TimeToTicks(hour, minute, second) +
							  ((long)millisecond) * 10000L;
					if(value__ < 0 || value__ > maxTicks)
					{
						throw new ArgumentException
							(Environment.GetResourceString
								("Arg_DateTimeRange"));
					}
				}
			}

	// Determine if a year is a leap year.
	public static bool IsLeapYear(int year)
			{
				return ((year % 4) == 0 &&
				        ((year % 100) != 0 || (year % 400) == 0));
			}

// TODO: Calendar support.

	// Override inherited methods.
	public override int GetHashCode()
			{
				unchecked
				{
					return (int)(value__ ^ (value__ >> 32));
				}
			}
	public override bool Equals(Object value)
			{
				if(value is DateTime)
				{
					return (value__ == ((DateTime)value).value__);
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
	public String ToString(String format, IFormatProvider provider)
			{
				// TODO
				return "";
			}

	// Parsing methods.
	public static DateTime Parse(String s)
			{
				// TODO.
				return new DateTime(0);
			}
	public static DateTime Parse(String s, IFormatProvider provider)
			{
				// TODO.
				return new DateTime(0);
			}
#if false
	public static DateTime Parse(String s, IFormatProvider provider,
								 DateTimeStyles style)
			{
				// TODO.
				return new DateTime(0);
			}
#endif

	// Implementation of the IComparable interface.
	public int CompareTo(Object value)
			{
				if(value != null)
				{
					if(!(value is DateTime))
					{
						throw new ArgumentException
							(Environment.GetResourceString
								("Arg_MustBeDateTime"));
					}
					long value2 = ((DateTime)value).value__;
					if(value__ < value2)
					{
						return -1;
					}
					else if(value__ > value2)
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
					return 1;
				}
			}

	// Implementation of IConvertible interface.
	public TypeCode GetTypeCode()
			{
				return TypeCode.DateTime;
			}
	Object IConvertible.ToType(Type ct, IFormatProvider provider)
			{
				return Convert.DefaultToType(this, ct, provider);
			}
	Boolean IConvertible.ToBoolean(IFormatProvider provider)
			{
				throw new InvalidCastException
					(String.Format
						(Environment.GetResourceString("InvalidCast_FromTo"),
		 			     "DateTime", "Boolean"));
			}
	Byte IConvertible.ToByte(IFormatProvider provider)
			{
				throw new InvalidCastException
					(String.Format
						(Environment.GetResourceString("InvalidCast_FromTo"),
		 			     "DateTime", "Byte"));
			}
	SByte IConvertible.ToSByte(IFormatProvider provider)
			{
				throw new InvalidCastException
					(String.Format
						(Environment.GetResourceString("InvalidCast_FromTo"),
		 			     "DateTime", "SByte"));
			}
	Int16 IConvertible.ToInt16(IFormatProvider provider)
			{
				throw new InvalidCastException
					(String.Format
						(Environment.GetResourceString("InvalidCast_FromTo"),
		 			     "DateTime", "Int16"));
			}
	UInt16 IConvertible.ToUInt16(IFormatProvider provider)
			{
				throw new InvalidCastException
					(String.Format
						(Environment.GetResourceString("InvalidCast_FromTo"),
		 			     "DateTime", "UInt16"));
			}
	Int32 IConvertible.ToInt32(IFormatProvider provider)
			{
				throw new InvalidCastException
					(String.Format
						(Environment.GetResourceString("InvalidCast_FromTo"),
		 			     "DateTime", "Int32"));
			}
	UInt32 IConvertible.ToUInt32(IFormatProvider provider)
			{
				throw new InvalidCastException
					(String.Format
						(Environment.GetResourceString("InvalidCast_FromTo"),
		 			     "DateTime", "UInt32"));
			}
	Int64 IConvertible.ToInt64(IFormatProvider provider) 
			{
				throw new InvalidCastException
					(String.Format
						(Environment.GetResourceString("InvalidCast_FromTo"),
		 			     "DateTime", "Int64"));
			}
	UInt64 IConvertible.ToUInt64(IFormatProvider provider)
			{
				throw new InvalidCastException
					(String.Format
						(Environment.GetResourceString("InvalidCast_FromTo"),
		 			     "DateTime", "UInt64"));
			}
	Char IConvertible.ToChar(IFormatProvider provider)
			{
				throw new InvalidCastException
					(String.Format
						(Environment.GetResourceString("InvalidCast_FromTo"),
		 			     "DateTime", "Char"));
			}
	Single IConvertible.ToSingle(IFormatProvider provider)
			{
				throw new InvalidCastException
					(String.Format
						(Environment.GetResourceString("InvalidCast_FromTo"),
		 			     "DateTime", "Single"));
			}
	Double IConvertible.ToDouble(IFormatProvider provider)
			{
				throw new InvalidCastException
					(String.Format
						(Environment.GetResourceString("InvalidCast_FromTo"),
		 			     "DateTime", "Double"));
			}
	Decimal IConvertible.ToDecimal(IFormatProvider provider)
			{
				throw new InvalidCastException
					(String.Format
						(Environment.GetResourceString("InvalidCast_FromTo"),
		 			     "DateTime", "Decimal"));
			}
	DateTime IConvertible.ToDateTime(IFormatProvider provider)
			{
				return this;
			}
	public String ToString(IFormatProvider provider)
			{
				return ToString(null, provider);
			}

	// Properties.
	public DateTime Date
			{
				get
				{
					return new DateTime(value__ - (value__ % ticksPerDay));
				}
			}
	public int Day
			{
				get
				{
					// TODO
					return 1;
				}
			}
	public int Month
			{
				get
				{
					// TODO
					return 1;
				}
			}
	public int Year
			{
				get
				{
					// TODO
					return 1;
				}
			}
	public int Hour
			{
				get
				{
					unchecked
					{
						return (int)((value__ / (ticksPerDay / 24)) % 24);
					}
				}
			}
	public int Minute
			{
				get
				{
					unchecked
					{
						return (int)((value__ / (ticksPerDay / (24 * 60))) % 60);
					}
				}
			}
	public int Second
			{
				get
				{
					unchecked
					{
						return (int)((value__ / 10000000L) % 60);
					}
				}
			}
	public int Millisecond
			{
				get
				{
					unchecked
					{
						return (int)((value__ / 10000L) % 1000L);
					}
				}
			}
	public int DayOfWeek
			{
				get
				{
					unchecked
					{
						return (int)(((value__ / ticksPerDay) + 1) % 7);
					}
				}
			}
	public int DayOfYear
			{
				get
				{
					// TODO
					return 1;
				}
			}
	public long Ticks
			{
				get
				{
					return value__;
				}
			}
	public static DateTime Now
			{
				get
				{
					return new DateTime(TimeMethods.GetCurrentTime());
				}
			}
	public static DateTime UtcNow
			{
				get
				{
					return new DateTime(TimeMethods.GetCurrentUtcTime());
				}
			}
	public static DateTime Today
			{
				get
				{
					long value = TimeMethods.GetCurrentTime();
					return new DateTime(value - (value % ticksPerDay));
				}
			}

	// Operators.
	public static bool operator==(DateTime d1, DateTime d2)
			{ return (d1.value__ == d2.value__); }
	public static bool operator!=(DateTime d1, DateTime d2)
			{ return (d1.value__ != d2.value__); }
	public static bool operator<(DateTime d1, DateTime d2)
			{ return (d1.value__ < d2.value__); }
	public static bool operator>(DateTime d1, DateTime d2)
			{ return (d1.value__ > d2.value__); }
	public static bool operator<=(DateTime d1, DateTime d2)
			{ return (d1.value__ <= d2.value__); }
	public static bool operator>=(DateTime d1, DateTime d2)
			{ return (d1.value__ >= d2.value__); }
	public static DateTime operator+(DateTime d, TimeSpan t)
			{ return new DateTime(d.value__ + t.value__); }
	public static DateTime operator-(DateTime d, TimeSpan t)
			{ return new DateTime(d.value__ - t.value__); }
	public static TimeSpan operator-(DateTime d1, DateTime d2)
			{ return new TimeSpan(d1.value__ - d2.value__); }

	// Named operators.
	public DateTime Add(TimeSpan t)
			{ return new DateTime(value__ + t.value__); }
	public DateTime Subtract(TimeSpan t)
			{ return new DateTime(value__ - t.value__); }
	public TimeSpan Subtract(DateTime d)
			{ return new TimeSpan(value__ - d.value__); }

// TODO

}; // class DateTime

}; // namespace System
