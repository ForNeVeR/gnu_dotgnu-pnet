/*
 * Calendar.cs - Implementation of the
 *        "System.Globalization.Calendar" class.
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

namespace System.Globalization
{

using System;

public abstract class Calendar
{

	// Current era value.
	public const int CurrentEra = 0;

	// Internal state.
	private int twoDigitYearMax;

	// Constructor.
	protected Calendar()
			{
				twoDigitYearMax = -1;
			}

	// Get a list of eras for the calendar.
	public abstract int[] Eras { get; }

	// Set the last year of a 100-year range for 2-digit processing.
	public virtual int TwoDigitYearMax
			{
				get
				{
					return twoDigitYearMax;
				}
				set
				{
					twoDigitYearMax = value;
				}
			}

	// Internal version of the default "Add*" methods.
	private static DateTime DefaultAdd
				(DateTime time, int value, long multiplier)
			{
				long ticks;

				// Get the final date tick value.  We use the
				// "checked" block to detect 64-bit overflow
				// when computing the tick value.
				checked
				{
					try
					{
						ticks = ((long)value) * multiplier + time.Ticks;
					}
					catch(OverflowException)
					{
						throw new ArgumentException(_("Arg_DateTimeRange"));
					}
				}

				// Construct and return a new DateTime object,
				// which will also check for out of range values.
				return new DateTime(ticks);
			}

	// Add a time period to a DateTime value.
	public virtual DateTime AddDays(DateTime time, int days)
			{
				return DefaultAdd(time, days, TimeSpan.TicksPerDay);
			}
	public virtual DateTime AddHours(DateTime time, int hours)
			{
				return DefaultAdd(time, hours, TimeSpan.TicksPerHour);
			}
	public virtual DateTime AddMilliseconds(DateTime time, double milliseconds)
			{
				long ticks;

				// Get the final date tick value.  We use the
				// "checked" block to detect overflow when
				// computing the tick value.
				checked
				{
					try
					{
						ticks = ((long)(milliseconds *
										(double)TimeSpan.TicksPerMillisecond))
								+ time.Ticks;
					}
					catch(OverflowException)
					{
						throw new ArgumentException(_("Arg_DateTimeRange"));
					}
				}

				// Construct and return a new DateTime object,
				// which will also check for out of range values.
				return new DateTime(ticks);
			}
	public virtual DateTime AddMinutes(DateTime time, int minutes)
			{
				return DefaultAdd(time, minutes, TimeSpan.TicksPerMinute);
			}
	public abstract DateTime AddMonths(DateTime time, int months);
	public virtual DateTime AddSeconds(DateTime time, int seconds)
			{
				return DefaultAdd(time, seconds, TimeSpan.TicksPerSecond);
			}
	public virtual DateTime AddWeeks(DateTime time, int weeks)
			{
				return DefaultAdd(time, weeks,
								  TimeSpan.TicksPerDay * 7L);
			}
	public abstract DateTime AddYears(DateTime time, int years);

	// Extract the components from a DateTime value.
	public abstract int GetDayOfMonth(DateTime time);
	public abstract int GetDayOfWeek(DateTime time);
	public abstract int GetDayOfYear(DateTime time);
	public virtual int GetHour(DateTime time)
			{
				return time.Hour;
			}
	public virtual double GetMilliseconds(DateTime time)
			{
				return (double)(time.Millisecond);
			}
	public virtual int GetMinute(DateTime time)
			{
				return time.Minute;
			}
	public abstract int GetMonth(DateTime time);
	public virtual int GetSecond(DateTime time)
			{
				return time.Second;
			}
	public abstract int GetYear(DateTime time);

	// Get the number of days in a particular month.
	public virtual int GetDaysInMonth(int year, int month)
			{
				return GetDaysInMonth(year, month, CurrentEra);
			}
	public abstract int GetDaysInMonth(int year, int month, int era);

	// Get the number of days in a particular year.
	public virtual int GetDaysInYear(int year)
			{
				return GetDaysInYear(year, CurrentEra);
			}
	public abstract int GetDaysInYear(int year, int era);

	// Get the era for a specific DateTime value.
	public abstract int GetEra(DateTime time);

	// Get the number of months in a specific year.
	public virtual int GetMonthsInYear(int year)
			{
				return GetMonthsInYear(year, CurrentEra);
			}
	public abstract int GetMonthsInYear(int year, int era);

	// Get the week of the year that a particular date falls within.
	[TODO]
	public virtual int GetWeekOfYear(DateTime time,
									 CalendarWeekRule rule,
									 DayOfWeek firstDayOfWeek)
			{
				// TODO
				return 0;
			}

	// Determine if a particular day is a leap day.
	public virtual bool IsLeapDay(int year, int month, int day)
			{
				return IsLeapDay(year, month, day, CurrentEra);
			}
	public abstract bool IsLeapDay(int year, int month, int day, int era);

	// Determine if a particular month is a leap month.
	public virtual bool IsLeapMonth(int year, int month)
			{
				return IsLeapMonth(year, month, CurrentEra);
			}
	public abstract bool IsLeapMonth(int year, int month, int era);

	// Determine if a particular year is a leap year.
	public virtual bool IsLeapYear(int year)
			{
				return IsLeapYear(year, CurrentEra);
			}
	public abstract bool IsLeapYear(int year, int era);

	// Convert a particular time into a DateTime value.
	public virtual DateTime ToDateTime(int year, int month, int day,
										int hour, int minute, int second,
										int millisecond)
			{
				return ToDateTime(year, month, day, hour, minute, second,
								  millisecond, CurrentEra);
			}
	public abstract DateTime ToDateTime(int year, int month, int day,
										int hour, int minute, int second,
										int millisecond, int era);

	// Convert a two-digit year value into a four-digit year value.
	public virtual int ToFourDigitYear(int year)
			{
				if(year < 0)
				{
					// Invalid year value.
					throw new ArgumentException(_("ArgRange_NonNegative"));
				}
				else if(year >= 100)
				{
					// Assume that the year is already 4 digits long.
					return year;
				}
				else
				{
					// Adjust the year using the "TwoDigitYearMax" value.
					int yearMax = TwoDigitYearMax;
					int yearMaxInCentury = (yearMax % 100);
					if(year <= yearMaxInCentury)
					{
						return (yearMax - yearMaxInCentury) + year;
					}
					else
					{
						return (yearMax - 100) + (year - yearMaxInCentury);
					}
				}
			}

}; // class Calendar

}; // namespace System.Globalization
