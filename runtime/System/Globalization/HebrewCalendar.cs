/*
 * HebrewCalendar.cs - Implementation of the
 *        "System.Globalization.HebrewCalendar" class.
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

public class HebrewCalendar : Calendar
{

	// Hebrew era value.
	public static readonly int HebrewEra = 1;

	// Useful internal constants.
	private const int DefaultTwoDigitYearMax = 5790;
	private const int MinYear = 5343;
	private const int MaxYear = 6000;

	// There are 1080 "parts" per hour.
	private const int PartsPerHour = 1080;

	// Number of "parts" in a day.
	private const int PartsPerDay = 24 * PartsPerHour;

	// Length of a lunar month in days and parts.
	private const int DaysPerMonth = 29;
	private const int DaysPerMonthFraction = 12 * PartsPerHour + 793;
	private const int DaysPerMonthParts =
			DaysPerMonth * PartsPerDay + DaysPerMonthFraction;

	// The time of the new moon in parts on the first day in the
	// Hebrew calendar (1 Tishri, year 1 which is approx 6 Oct 3761 BC).
	private const int FirstNewMoon = 11 * PartsPerHour + 204;

	// Constructor.
	public HebrewCalendar()
			{
				// Nothing to do here.
			}

	// Get a list of eras for the calendar.
	public override int[] Eras
			{
				get
				{
					int[] eras = new int [1];
					eras[0] = HebrewEra;
					return eras;
				}
			}

	// Set the last year of a 100-year range for 2-digit processing.
	public override int TwoDigitYearMax
			{
				get
				{
					int value = base.TwoDigitYearMax;
					if(value != -1)
					{
						return value;
					}
					base.TwoDigitYearMax = DefaultTwoDigitYearMax;
					return DefaultTwoDigitYearMax;
				}
				set
				{
					base.TwoDigitYearMax = value;
				}
			}

	// Determine if a year value is a Hebrew leap year.
	private static bool IsHebrewLeapYear(int year)
			{
				return (((year * 12 + 17) % 19) >= 12);
			}

	// Get the absolute day number that starts a particular year.
	private static long StartOfYear(int year)
			{
				// Get the number of months before the year.
				int months = (235 * year - 234) / 19;

				// Calculate the day number in whole and fractional parts.
				long fraction = ((long)months) * ((long)DaysPerMonthFraction) +
								((long)FirstNewMoon);
				long day = ((long)months) * 29 + (fraction / PartsPerDay);
				fraction %= PartsPerDay;

				// Adjust the start of the year for the day of the week.
				// The value "weekday == 0" indicates Monday.
				int weekday = (int)(day % 7);
				if(weekday == 2 || weekday == 4 || weekday == 6)
				{
					// Cannot start on a Sunday, Wednesday, or Friday.
					++day;
					weekday = (int)(day % 7);
				}
				if(weekday == 1 &&
				   fraction > (15 * PartsPerHour + 204) &&
				   !IsHebrewLeapYear(year))
				{
					// If the new moon falls after "15 hours, 204 parts"
					// from the previous noon on a Tuesday and it is not
					// a leap year, postpone by 2 days.
					day += 2;
				}
				else if(weekday == 0 &&
				 		fraction > (21 * PartsPerHour + 589) &&
						IsHebrewLeapYear(year - 1))
				{
					// This happens for years that are 382 days in length,
					// which is not a legal year length.
					++day;
				}

				// Return the day number to the caller.
				return day;
			}

	// Add a time period to a DateTime value.
	[TODO]
	public override DateTime AddMonths(DateTime time, int months)
			{
				// TODO
				return new DateTime(0);
			}
	[TODO]
	public override DateTime AddYears(DateTime time, int years)
			{
				// TODO
				return new DateTime(0);
			}

	// Extract the components from a DateTime value.
	[TODO]
	public override int GetDayOfMonth(DateTime time)
			{
				// TODO
				return 0;
			}
	public override int GetDayOfWeek(DateTime time)
			{
				// The Gregorian and Hebrew weekdays are identical.
				return time.DayOfWeek;
			}
	[TODO]
	public override int GetDayOfYear(DateTime time)
			{
				// TODO
				return 0;
			}
	[TODO]
	public override int GetMonth(DateTime time)
			{
				// TODO
				return 0;
			}
	[TODO]
	public override int GetYear(DateTime time)
			{
				// TODO
				return 0;
			}

	// Get the number of days in a particular month.
	[TODO]
	public override int GetDaysInMonth(int year, int month, int era)
			{
				// TODO
				return 0;
			}

	// Get the number of days in a particular year.
	public override int GetDaysInYear(int year, int era)
			{
				if(era != CurrentEra && era != HebrewEra)
				{
					throw new ArgumentException(_("Arg_InvalidEra"));
				}
				if(year < MinYear || year > MaxYear)
				{
					throw new ArgumentOutOfRangeException
						("year", _("ArgRange_Year"));
				}
				return (int)(StartOfYear(year + 1) - StartOfYear(year));
			}

	// Get the era for a specific DateTime value.
	public override int GetEra(DateTime time)
			{
				return HebrewEra;
			}

	// Get the number of months in a specific year.
	public override int GetMonthsInYear(int year, int era)
			{
				if(era != CurrentEra && era != HebrewEra)
				{
					throw new ArgumentException(_("Arg_InvalidEra"));
				}
				if(year < MinYear || year > MaxYear)
				{
					throw new ArgumentOutOfRangeException
						("year", _("ArgRange_Year"));
				}
				if(IsHebrewLeapYear(year))
				{
					return 13;
				}
				else
				{
					return 12;
				}
			}

	// Determine if a particular day is a leap day.
	[TODO]
	public override bool IsLeapDay(int year, int month, int day, int era)
			{
				// TODO
				return false;
			}

	// Determine if a particular month is a leap month.
	[TODO]
	public override bool IsLeapMonth(int year, int month, int era)
			{
				// TODO
				return false;
			}

	// Determine if a particular year is a leap year.
	public override bool IsLeapYear(int year, int era)
			{
				if(era != CurrentEra && era != HebrewEra)
				{
					throw new ArgumentException(_("Arg_InvalidEra"));
				}
				if(year < MinYear || year > MaxYear)
				{
					throw new ArgumentOutOfRangeException
						("year", _("ArgRange_Year"));
				}
				return IsHebrewLeapYear(year);
			}

	// Convert a particular time into a DateTime value.
	[TODO]
	public override DateTime ToDateTime(int year, int month, int day,
										int hour, int minute, int second,
										int millisecond, int era)
			{
				// TODO
				return new DateTime(0);
			}

}; // class HebrewCalendar

}; // namespace System.Globalization
