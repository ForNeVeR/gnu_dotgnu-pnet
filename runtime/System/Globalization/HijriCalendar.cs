/*
 * HijriCalendar.cs - Implementation of the
 *        "System.Globalization.HijriCalendar" class.
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

namespace System.Globalization
{

using System;

public class HijriCalendar : Calendar
{
	// The Hijri era.
	public static readonly int HijriEra = 1;

	// Useful constants.
	private const int DefaultTwoDigitMax = 1451;

	// Constructor.
	[TODO]
	public HijriCalendar()
			{
				// TODO
			}

	// Get a list of eras for the calendar.
	public override int[] Eras
			{
				get
				{
					int[] eras = new int [1];
					eras[0] = HijriEra;
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
					else
					{
						// Set the default value.
						base.TwoDigitYearMax = DefaultTwoDigitMax;
						return DefaultTwoDigitMax;
					}
				}
				set
				{
					if(value < 100 || value > 9999)
					{
						throw new ArgumentOutOfRangeException
							("year", _("ArgRange_Year"));
					}
					base.TwoDigitYearMax = value;
				}
			}

	// Add a time period to a DateTime value.
	[TODO]
	public override DateTime AddMonths(DateTime time, int months)
			{
				// TODO
				return time;
			}
	public override DateTime AddYears(DateTime time, int years)
			{
				// TODO
				return time;
			}

	// Extract the components from a DateTime value.
	[TODO]
	public override int GetDayOfMonth(DateTime time)
			{
				// TODO
				return 0;
			}
	[TODO]
	public override System.DayOfWeek GetDayOfWeek(DateTime time)
			{
				// TODO
				return DayOfWeek.Sunday;
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
				if(era != CurrentEra && era != HijriEra)
				{
					throw new ArgumentException(_("Arg_InvalidEra"));
				}
				// TODO
				return 0;
			}

	// Get the number of days in a particular year.
	public override int GetDaysInYear(int year, int era)
			{
				if(year < 1 || year > 9999)
				{
					throw new ArgumentOutOfRangeException
						("year", _("ArgRange_Year"));
				}
				if(era != CurrentEra && era != HijriEra)
				{
					throw new ArgumentException(_("Arg_InvalidEra"));
				}
				// TODO
				return 365;
			}

	// Get the era for a specific DateTime value.
	public override int GetEra(DateTime time)
			{
				return HijriEra;
			}

	// Get the number of months in a specific year.
	public override int GetMonthsInYear(int year, int era)
			{
				if(year < 1 || year > 9999)
				{
					throw new ArgumentOutOfRangeException
						("year", _("ArgRange_Year"));
				}
				if(era != CurrentEra && era != HijriEra)
				{
					throw new ArgumentException(_("Arg_InvalidEra"));
				}
				// TODO
				return 12;
			}

	// Determine if a particular day is a leap day.
	[TODO]
	public override bool IsLeapDay(int year, int month, int day, int era)
			{
				if(day < 1 || day > GetDaysInMonth(year, month, era))
				{
					throw new ArgumentOutOfRangeException
						("day", _("ArgRange_Day"));
				}
				// TODO
				return false;
			}

	// Determine if a particular month is a leap month.
	[TODO]
	public override bool IsLeapMonth(int year, int month, int era)
			{
				if(month < 1 || month > 12)
				{
					throw new ArgumentOutOfRangeException
						("month", _("ArgRange_Month"));
				}
				// TODO
				return false;
			}

	// Determine if a particular year is a leap year.
	[TODO]
	public override bool IsLeapYear(int year, int era)
			{
				if(year < 1 || year > 9999)
				{
					throw new ArgumentOutOfRangeException
						("year", _("ArgRange_Year"));
				}
				if(era != CurrentEra && era != HijriEra)
				{
					throw new ArgumentException(_("Arg_InvalidEra"));
				}
				// TODO
				return false;
			}

	// Convert a particular time into a DateTime value.
	[TODO]
	public override DateTime ToDateTime(int year, int month, int day,
										int hour, int minute, int second,
										int millisecond, int era)
			{
				if(era != CurrentEra && era != HijriEra)
				{
					throw new ArgumentException(_("Arg_InvalidEra"));
				}
				// TODO
				return new DateTime(year, month, day, hour,
									minute, second, millisecond);
			}

}; // class HijriCalendar

}; // namespace System.Globalization
