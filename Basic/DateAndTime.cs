/*
 * DateAndTime.cs - Implementation of the
 *			"Microsoft.VisualBasic.DateAndTime" class.
 *
 * Copyright (C) 2003  Southern Storm Software, Pty Ltd.
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

namespace Microsoft.VisualBasic
{

using System;
using System.ComponentModel;
using System.Globalization;
using System.Runtime.InteropServices;
using Microsoft.VisualBasic.CompilerServices;

[StandardModule]
public sealed class DateAndTime
{
	// This class cannot be instantiated.
	private DateAndTime() {}

	// Add a time interval to a date value.
	[TODO]
	public static DateTime DateAdd
				(String Interval, double Number, Object DateValue)
			{
				// TODO
				return DateTime.MinValue;
			}
	public static DateTime DateAdd
				(DateInterval Interval, double Number, DateTime DateValue)
			{
				// TODO
				return DateTime.MinValue;
			}

	// Get the difference between two dates.
	[TODO]
	public static long DateDiff
				(DateInterval Interval, DateTime Date1, DateTime Date2,
				 [Optional] [DefaultValue(FirstDayOfWeek.Sunday)]
				 	FirstDayOfWeek DayOfWeek,
				 [Optional] [DefaultValue(FirstWeekOfYear.Jan1)]
				 	FirstWeekOfYear WeekOfYear)
			{
				// TODO
				return 0;
			}
	[TODO]
	public static long DateDiff
				(String Interval, Object Date1, Object Date2,
				 [Optional] [DefaultValue(FirstDayOfWeek.Sunday)]
				 	FirstDayOfWeek DayOfWeek,
				 [Optional] [DefaultValue(FirstWeekOfYear.Jan1)]
				 	FirstWeekOfYear WeekOfYear)
			{
				// TODO
				return 0;
			}

	// Get a particular date part.
	[TODO]
	public static int DatePart
				(DateInterval Interval, DateTime DateValue,
				 [Optional] [DefaultValue(FirstDayOfWeek.Sunday)]
				 	FirstDayOfWeek FirstDayOfWeekValue,
				 [Optional] [DefaultValue(FirstWeekOfYear.Jan1)]
				 	FirstWeekOfYear FirstWeekOfYearValue)
			{
				// TODO
				return 0;
			}
	[TODO]
	public static int DatePart
				(String Interval, Object DateValue,
				 [Optional] [DefaultValue(FirstDayOfWeek.Sunday)]
				 	FirstDayOfWeek FirstDayOfWeekValue,
				 [Optional] [DefaultValue(FirstWeekOfYear.Jan1)]
				 	FirstWeekOfYear FirstWeekOfYearValue)
			{
				// TODO
				return 0;
			}

	// Build a date value.
	public static DateTime DateSerial(int Year, int Month, int Day)
			{
				// Fix up the year value.
				if(Year < 0)
				{
					Year = DateTime.Now.Year + Year;
				}
				else if(Year < 30)
				{
					Year += 2000;
				}
				else if(Year < 100)
				{
					Year += 1900;
				}

				// Fix up the month value.
				while(Month < 1)
				{
					--Year;
					Month += 12;
				}
				while(Month > 12)
				{
					++Year;
					Month -= 12;
				}

				// Fix up the day value.
				int extraDays;
				int daysInMonth = DateTime.DaysInMonth(Year, Month);
				if(Day < 1)
				{
					extraDays = (Day - 1);
				}
				else if(Day > daysInMonth)
				{
					extraDays = (Day - daysInMonth);
				}
				else
				{
					extraDays = 0;
				}

				// Build and return the date.
				DateTime date = new DateTime(Year, Month, Day);

				// Adjust the date for extra days.
				if(extraDays != 0)
				{
					return date.AddDays((double)extraDays);
				}
				else
				{
					return date;
				}
			}

	// Convert a string into a date value.
	public static DateTime DateValue(String StringDate)
			{
				return (DateType.FromString(StringDate)).Date;
			}

	// Get the day from a date value.
	public static int Day(DateTime DateValue)
			{
				return CultureInfo.CurrentCulture.Calendar
							.GetDayOfMonth(DateValue);
			}

	// Get the weekday from a date value.
	[TODO]
	public static int GetDayOfWeek
				(DateTime DateValue, FirstDayOfWeek weekdayFirst)
			{
				// TODO
				return 0;
			}

	// Get the hour from a date value.
	public static int Hour(DateTime TimeValue)
			{
				return CultureInfo.CurrentCulture.Calendar.GetHour(TimeValue);
			}

	// Get the minute from a date value.
	public static int Minute(DateTime TimeValue)
			{
				return CultureInfo.CurrentCulture.Calendar.GetMinute(TimeValue);
			}

	// Get the month from a date value.
	public static int Month(DateTime DateValue)
			{
				return CultureInfo.CurrentCulture.Calendar.GetMonth(DateValue);
			}

	// Get the name of a specific month.
	[TODO]
	public static String MonthName
				(int Month, [Optional] [DefaultValue(false)] bool Abbreviate)
			{
				// TODO
				return null;
			}

	// Get the second from a date value.
	public static int Second(DateTime TimeValue)
			{
				return CultureInfo.CurrentCulture.Calendar.GetSecond(TimeValue);
			}

	// Convert hour, minute, and second values into a time value.
	public static DateTime TimeSerial(int Hour, int Minute, int Second)
			{
				return new DateTime(Hour * TimeSpan.TicksPerHour +
									Minute * TimeSpan.TicksPerMinute +
									Second * TimeSpan.TicksPerSecond);
			}

	// Extract the time portion of a value.
	public static DateTime TimeValue(String StringTime)
			{
				long ticks = (DateType.FromString(StringTime)).Ticks;
				return new DateTime(ticks % TimeSpan.TicksPerDay);
			}

	// Get the weekday from a date value.
	[TODO]
	public static int Weekday
				(DateTime DateValue,
				 [Optional] [DefaultValue(FirstDayOfWeek.Sunday)]
				 	FirstDayOfWeek DayOfWeek)
			{
				// TODO
				return 0;
			}

	// Get the name of a specific weekday.
	[TODO]
	public static String WeekdayName
				(int Weekday,
				 [Optional] [DefaultValue(false)] bool Abbreviate,
				 [Optional] [DefaultValue(FirstDayOfWeek.Sunday)]
				 		FirstDayOfWeek FirstDayOfWeekValue)
			{
				// TODO
				return null;
			}

	// Get the year from a date value.
	public static int Year(DateTime DateValue)
			{
				return CultureInfo.CurrentCulture.Calendar.GetYear(DateValue);
			}

	// Get the current date as a string.
	public static String DateString
			{
				get
				{
					return DateTime.Today.ToString
						("MM\\-dd\\-yyyy", CultureInfo.InvariantCulture);
				}
				set
				{
					// Ignored - not used in this implementation.
				}
			}

	// Get the current date and time.
	public static DateTime Now
			{
				get
				{
					return DateTime.Now;
				}
			}

	// Get the current time of day.
	public static DateTime TimeOfDay
			{
				get
				{
					return new DateTime(DateTime.Now.TimeOfDay.Ticks);
				}
				set
				{
					// Ignored - not used in this implementation.
				}
			}

	// Get the current time as a string.
	public static String TimeString
			{
				get
				{
					return (new DateTime(DateTime.Now.TimeOfDay.Ticks))
								.ToString("HH:mm:ss",
										  CultureInfo.InvariantCulture);
				}
				set
				{
					// Ignored - not used in this implementation.
				}
			}

	// Get a timer value from the current time.
	public static double Timer
			{
				get
				{
					return (double)(DateTime.Now.Ticks % TimeSpan.TicksPerDay)
								/ (double)(TimeSpan.TicksPerSecond);
				}
			}

	// Get today's date.
	public static DateTime Today
			{
				get
				{
					return DateTime.Today;
				}
				set
				{
					// Ignored - not used in this implementation.
				}
			}

}; // class DateAndTime

}; // namespace Microsoft.VisualBasic
