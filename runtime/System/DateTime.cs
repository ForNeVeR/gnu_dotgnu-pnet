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
using System.Runtime.InteropServices;
using Platform;

// ECMA requires that this have "auto" layout.
[StructLayout(LayoutKind.Auto)]
public struct DateTime : IComparable, IFormattable
#if !ECMA_COMPAT
	, IConvertible
#endif
{

	private long value__;

	// Days in each month in regular years.
	internal static readonly int[] daysForEachMonth
		= {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

	// Days before each month in regular years.
	internal static readonly int[] daysBeforeMonth
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

	// Convert a year into a number of ticks.
	private static long YearToTicks(int year)
			{
				--year;
				return (long)(year * 365 + year / 4 -
							  year / 100 + year / 400);
			}

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
							result = YearToTicks(year);
							result += (long)(daysBeforeMonth[month - 1]);
							if(month > 2 && isLeap)
							{
								++result;
							}
							return (result + (long)(day - 1)) * ticksPerDay;
						}
					}
				}
				throw new ArgumentOutOfRangeException(_("ArgRange_YMD"));
			}

	// Convert a time into a number of ticks.
	internal static long TimeToTicks(int hour, int minute, int second)
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
				throw new ArgumentOutOfRangeException(_("ArgRange_HMS"));
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
						("ticks", _("ArgRange_DateTimeTicks"));
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
						throw new ArgumentException(_("Arg_DateTimeRange"));
					}
				}
			}

#if !ECMA_COMPAT
	// Non-ECMA constructors.
	public DateTime(int year, int month, int day,
					int hour, int minute, int second, Calendar calendar)
			: this(year, month, day, hour, minute, second, 0, calendar)
			{
				// Nothing to do here
			}
	public DateTime(int year, int month, int day, Calendar calendar)
			: this(year, month, day, 0, 0, 0, 0, calendar)
			{
				// Nothing to do here
			}
	public DateTime(int year, int month, int day,
					int hour, int minute, int second, int millisecond,
				    Calendar calendar)
			{
				if(calendar == null)
				{
					throw new ArgumentNullException("calendar");
				}
				value__ = calendar.ToDateTime(year, month, day, hour,
											  minute, second, millisecond,
											  Calendar.CurrentEra)
								.Ticks;
			}
#endif // !ECMA_COMPAT

	// Determine if a year is a leap year.
	public static bool IsLeapYear(int year)
			{
				return ((year % 4) == 0 &&
				        ((year % 100) != 0 || (year % 400) == 0));
			}

	// Determine the number of days in a month.
	public static int DaysInMonth(int year, int month)
			{
				if(year < 1 || year > 9999)
				{
					throw new ArgumentOutOfRangeException
						("year", _("ArgRange_Year"));
				}
				if(month < 1 || month > 12)
				{
					throw new ArgumentOutOfRangeException
						("month", _("ArgRange_Month"));
				}
				if(IsLeapYear(year) && month == 2)
				{
					return 29;
				}
				else
				{
					return daysForEachMonth[month - 1];
				}
			}

#if !ECMA_COMPAT

	// The base time for Windows file time values (Midnight, Jan 1, 1601).
	private static readonly DateTime FileTimeBase = new DateTime(1601, 1, 1);

	// Convert a Windows file time into a DateTime value.
	public static DateTime FromFileTime(long fileTime)
			{
				checked
				{
					try
					{
						return new DateTime(fileTime + FileTimeBase.Ticks);
					}
					catch(OverflowException)
					{
						throw new ArgumentOutOfRangeException
							("fileTime", _("ArgRange_DateTimeRange"));
					}
				}
			}

	// Convert a DateTime value into a Windows file time.
	public long ToFileTime()
			{
				long time = value__ - FileTimeBase.Ticks;
				if(time < 0)
				{
					throw new ArgumentOutOfRangeException
						("this", _("ArgRange_DateTimeRange"));
				}
				else
				{
					return time;
				}
			}

	// Base time for OLE Automation values (Midnight, Dec 30, 1899).
	private static readonly DateTime OLEBaseTime = new DateTime(1899, 12, 30);

	// Convert an OLE Automation date into a DateTime value.
	public static DateTime FromOADate(double d)
			{
				// Convert the value into ticks, while checking for overflow.
				long ticks;
				checked
				{
					try
					{
						ticks = ((long)(d * (double)TimeSpan.TicksPerDay)) +
								OLEBaseTime.Ticks;
					}
					catch(OverflowException)
					{
						throw new ArgumentOutOfRangeException
							("d", _("ArgRange_DateTimeRange"));
					}
				}

				// Convert the ticks into a DateTime value, which will
				// perform additional range checking.
				return new DateTime(ticks);
			}

	// Convert this DateTime value into an OLE Automation date.
	public double ToOADate()
			{
				if(value__ == 0)
				{
					// Special case for uninitialized DateTime values.
					return 0.0;
				}
				else
				{
					return (((double)value__) / ((double)TimeSpan.TicksPerDay))
								- ((double)OLEBaseTime.Ticks);
				}
			}

#endif // !ECMA_COMPAT

	// Get a hash code for this DateTime value.
	public override int GetHashCode()
			{
				unchecked
				{
					return (int)(value__ ^ (value__ >> 32));
				}
			}

	// Compare DateTime values for equality.
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
	public static bool Equals(DateTime d1, DateTime d2)
			{
				return (d1.value__ == d2.value__);
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
	[TODO]
	public String ToString(String format, IFormatProvider provider)
			{
				// TODO
				return "";
			}

	// Parsing methods.
	[TODO]
	public static DateTime Parse(String s)
			{
				// TODO.
				return new DateTime(0);
			}
	[TODO]
	public static DateTime Parse(String s, IFormatProvider provider)
			{
				// TODO.
				return new DateTime(0);
			}
	[TODO]
	public static DateTime Parse(String s, IFormatProvider provider,
								 DateTimeStyles style)
			{
				// TODO.
				return new DateTime(0);
			}
	[TODO]
	public static DateTime ParseExact(String s, String format,
									  IFormatProvider provider)
			{
				// TODO.
				return new DateTime(0);
			}
	[TODO]
	public static DateTime ParseExact(String s, String format,
									  IFormatProvider provider,
									  DateTimeStyles style)
			{
				// TODO.
				return new DateTime(0);
			}
	[TODO]
	public static DateTime ParseExact(String s, String[] formats,
									  IFormatProvider provider,
									  DateTimeStyles style)
			{
				// TODO.
				return new DateTime(0);
			}

	public static int Compare(DateTime t1, DateTime t2)
			{
				if(t1.value__ < t2.value__)
				{
					return -1;
				}
				else if(t1.value__ > t2.value__)
				{
					return 1;
				}
				else
				{
					return 0;
				}
			}

	// Implementation of the IComparable interface.
	public int CompareTo(Object value)
			{
				if(value != null)
				{
					if(!(value is DateTime))
					{
						throw new ArgumentException(_("Arg_MustBeDateTime"));
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

#if !ECMA_COMPAT

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
						(_("InvalidCast_FromTo"), "DateTime", "Boolean"));
			}
	Byte IConvertible.ToByte(IFormatProvider provider)
			{
				throw new InvalidCastException
					(String.Format
						(_("InvalidCast_FromTo"), "DateTime", "Byte"));
			}
	SByte IConvertible.ToSByte(IFormatProvider provider)
			{
				throw new InvalidCastException
					(String.Format
						(_("InvalidCast_FromTo"), "DateTime", "SByte"));
			}
	Int16 IConvertible.ToInt16(IFormatProvider provider)
			{
				throw new InvalidCastException
					(String.Format
						(_("InvalidCast_FromTo"), "DateTime", "Int16"));
			}
	UInt16 IConvertible.ToUInt16(IFormatProvider provider)
			{
				throw new InvalidCastException
					(String.Format
						(_("InvalidCast_FromTo"), "DateTime", "UInt16"));
			}
	Int32 IConvertible.ToInt32(IFormatProvider provider)
			{
				throw new InvalidCastException
					(String.Format
						(_("InvalidCast_FromTo"), "DateTime", "Int32"));
			}
	UInt32 IConvertible.ToUInt32(IFormatProvider provider)
			{
				throw new InvalidCastException
					(String.Format
						(_("InvalidCast_FromTo"), "DateTime", "UInt32"));
			}
	Int64 IConvertible.ToInt64(IFormatProvider provider) 
			{
				throw new InvalidCastException
					(String.Format
						(_("InvalidCast_FromTo"), "DateTime", "Int64"));
			}
	UInt64 IConvertible.ToUInt64(IFormatProvider provider)
			{
				throw new InvalidCastException
					(String.Format
						(_("InvalidCast_FromTo"), "DateTime", "UInt64"));
			}
	Char IConvertible.ToChar(IFormatProvider provider)
			{
				throw new InvalidCastException
					(String.Format
						(_("InvalidCast_FromTo"), "DateTime", "Char"));
			}
	Single IConvertible.ToSingle(IFormatProvider provider)
			{
				throw new InvalidCastException
					(String.Format
						(_("InvalidCast_FromTo"), "DateTime", "Single"));
			}
	Double IConvertible.ToDouble(IFormatProvider provider)
			{
				throw new InvalidCastException
					(String.Format
						(_("InvalidCast_FromTo"), "DateTime", "Double"));
			}
	Decimal IConvertible.ToDecimal(IFormatProvider provider)
			{
				throw new InvalidCastException
					(String.Format
						(_("InvalidCast_FromTo"), "DateTime", "Decimal"));
			}
	DateTime IConvertible.ToDateTime(IFormatProvider provider)
			{
				return this;
			}

#endif // !ECMA_COMPAT

	// The number of days in 400 years.
	private const int DaysPer400Years = (400 * 365 + 97);

	// The number of days in 100 years, when the 100th is not a leap year.
	private const int DaysPer100Years = (100 * 365 + 24);

	// The number of days in 4 years, including the leap day.
	private const int DaysPer4Years = (4 * 365 + 1);

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
					// Get the current year.
					int year = Year;

					// Get the tick offset within the year for the day.
					long ticks = value__ - YearToTicks(year);

					// Convert the tick offset into days.
					int days = unchecked((int)(ticks / TimeSpan.TicksPerDay));

					// Adjust for Jan and Feb in leap years.
					if(IsLeapYear(year))
					{
						if(days < 31)
						{
							return days + 1;
						}
						else if(days < (31 + 29))
						{
							return days - 30;
						}
						--days;
					}

					// Search for the starting month.
					int month = 1;
					while(month < 12 && days >= daysBeforeMonth[month])
					{
						++month;
					}
					return days - daysBeforeMonth[month - 1] + 1;
				}
			}
	public int Month
			{
				get
				{
					// Get the current year.
					int year = Year;

					// Get the tick offset within the year for the day.
					long ticks = value__ - YearToTicks(year);

					// Convert the tick offset into days.
					int days = unchecked((int)(ticks / TimeSpan.TicksPerDay));

					// Adjust for Jan and Feb in leap years.
					if(IsLeapYear(year))
					{
						if(days < 31)
						{
							return 1;
						}
						else if(days < (31 + 29))
						{
							return 2;
						}
						--days;
					}

					// Search for the starting month.
					int month = 1;
					while(month < 12 && days >= daysBeforeMonth[month])
					{
						++month;
					}
					return month;
				}
			}
	public int Year
			{
				get
				{
					// Note: there is probably a tricky mathematical
					// formula for doing this, but this version is a
					// lot easier to understand and debug.

					// Convert the tick count into a day value.
					int days = unchecked((int)(value__ / TimeSpan.TicksPerDay));

					// Determine the 400-year cycle that contains the date.
					int yearBase = ((days / DaysPer400Years) * 400) + 1;
					int yearOffset = days % DaysPer400Years;

					// Determine the 100-year cycle that contains the date.
					if(yearOffset >= DaysPer100Years * 3)
					{
						yearBase += 300;
						yearOffset -= DaysPer100Years * 3;
						if(yearOffset >= DaysPer100Years)
						{
							// The date is at the end of a 400-year cycle.
							return yearBase + 399;
						}
					}
					else if(yearOffset >= DaysPer100Years * 2)
					{
						yearBase += 200;
						yearOffset -= DaysPer100Years * 2;
					}
					else if(yearOffset >= DaysPer100Years)
					{
						yearBase += 100;
						yearOffset -= DaysPer100Years;
					}

					// Determine the 4-year cycle that contains the date.
					int temp = yearOffset / DaysPer4Years;
					yearBase += temp * 4;
					yearOffset -= temp * DaysPer4Years;

					// Determine the year out of the 4-year cycle.
					if(yearOffset >= 365 * 3)
					{
						return yearBase + 3;
					}
					else if(yearOffset >= 365 * 2)
					{
						return yearBase + 2;
					}
					else if(yearOffset >= 365)
					{
						return yearBase + 1;
					}
					else
					{
						return yearBase;
					}
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
					// Get the tick offset within the year for the day.
					long ticks = value__ - YearToTicks(Year);

					// Convert the tick offset into days.
					return unchecked((int)(ticks / TimeSpan.TicksPerDay)) + 1;
				}
			}
	public long Ticks
			{
				get
				{
					return value__;
				}
			}
	public TimeSpan TimeOfDay
			{
				get
				{
					return new TimeSpan(value__ % TimeSpan.TicksPerDay);
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

	// Internal version of the "Add*" methods.
	private DateTime DoAdd(double value, long multiplier)
			{
				// Let the runtime engine do the hard work of detecting
				// most of the overflow conditions.
				checked
				{
					try
					{
						return new DateTime
							(value__ + ((long)(value * (double)multiplier)));
					}
					catch(OverflowException)
					{
						throw new ArgumentException(_("Arg_DateTimeRange"));
					}
				}
			}

	// Add certain periods of time to this DateTime value.
	public DateTime AddDays(double value)
			{
				return DoAdd(value, TimeSpan.TicksPerDay);
			}
	public DateTime AddHours(double value)
			{
				return DoAdd(value, TimeSpan.TicksPerHour);
			}
	public DateTime AddMilliseconds(double value)
			{
				return DoAdd(value, TimeSpan.TicksPerMillisecond);
			}
	public DateTime AddMinutes(double value)
			{
				return DoAdd(value, TimeSpan.TicksPerMinute);
			}
	public DateTime AddMonths(int months)
			{
				// Crack open this DateTime value.
				int year = Year;
				int month = Month;
				int day = Day;
				long ticks = (value__ % TimeSpan.TicksPerDay);
				int hour = (int)(ticks / TimeSpan.TicksPerHour);
				int minute = (int)((ticks / TimeSpan.TicksPerMinute) % 60);
				int second = (int)((ticks / TimeSpan.TicksPerSecond) % 60);
				int fractions = (int)(ticks % TimeSpan.TicksPerSecond);

				// Adjust the month and year values and rebuild.
				if(months < 0)
				{
					months = -months;
					year -= months / 12;
					month -= months % 12;
					if(month < 1)
					{
						--year;
						month += 12;
					}
				}
				else
				{
					year += months / 12;
					month += months % 12;
					if(month > 12)
					{
						++year;
						month -= 12;
					}
				}
				return new DateTime(year, month, day,
									hour, minute, second, fractions);
			}
	public DateTime AddSeconds(double value)
			{
				return DoAdd(value, TimeSpan.TicksPerSecond);
			}
	public DateTime AddTicks(long value)
			{
				return new DateTime(value__ + value);
			}
	public DateTime AddYears(int years)
			{
				// Crack open this DateTime value.
				int year = Year;
				int month = Month;
				int day = Day;
				long ticks = (value__ % TimeSpan.TicksPerDay);
				int hour = (int)(ticks / TimeSpan.TicksPerHour);
				int minute = (int)((ticks / TimeSpan.TicksPerMinute) % 60);
				int second = (int)((ticks / TimeSpan.TicksPerSecond) % 60);
				int fractions = (int)(ticks % TimeSpan.TicksPerSecond);

				// Adjust the year value and rebuild.
				return new DateTime(year + years, month, day,
									hour, minute, second, fractions);
			}

	// Get this DateTime value in a variety of formats.
	[TODO]
	public String[] GetDateTimeFormats()
			{
				// TODO
				return null;
			}
	[TODO]
	public String[] GetDateTimeFormats(char format)
			{
				// TODO
				return null;
			}
	[TODO]
	public String[] GetDateTimeFormats(IFormatProvider provider)
			{
				// TODO
				return null;
			}
	[TODO]
	public String[] GetDateTimeFormats(char format, IFormatProvider provider)
			{
				// TODO
				return null;
			}

	// Convert this DateTime value into local time.
	public DateTime ToLocalTime()
			{
				return new DateTime
					(value__ - ((long)(TimeMethods.GetTimeZoneAdjust())) *
									TimeSpan.TicksPerSecond);
			}

	// Convert this DateTime value into UTC time.
	public DateTime ToUniversalTime()
			{
				return new DateTime
					(value__ + ((long)(TimeMethods.GetTimeZoneAdjust())) *
									TimeSpan.TicksPerSecond);
			}

	// Convert this DateTime value into a long date string.
	public String ToLongDateString()
			{
				return ToString("D", null);
			}

	// Convert this DateTime value into a long time string.
	public String ToLongTimeString()
			{
				return ToString("T", null);
			}

	// Convert this DateTime value into a short date string.
	public String ToShortDateString()
			{
				return ToString("d", null);
			}

	// Convert this DateTime value into a short time string.
	public String ToShortTimeString()
			{
				return ToString("t", null);
			}

}; // class DateTime

}; // namespace System
