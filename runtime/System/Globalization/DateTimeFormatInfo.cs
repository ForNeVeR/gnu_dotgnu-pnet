/*
 * DateTimeFormatInfo.cs - Implementation of the
 *		"System.Globalization.DateTimeFormatInfo" class.
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

public sealed class DateTimeFormatInfo : ICloneable, IFormatProvider
{

	// Internal state.
	private static DateTimeFormatInfo invariantInfo;
	private bool readOnly;
	private String amDesignator;
	private String pmDesignator;
	private String[] abbreviatedDayNames;
	private String[] dayNames;
	private String[] abbreviatedMonthNames;
	private String[] monthNames;
	private String dateSeparator;
	private String timeSeparator;
	private String fullDateTimePattern;
	private String longDatePattern;
	private String longTimePattern;
	private String monthDayPattern;
	private String shortDatePattern;
	private String shortTimePattern;
	private String yearMonthPattern;
#if !ECMA_COMPAT
	private Calendar calendar;
	private CalendarWeekRule calendarWeekRule;
	private DayOfWeek firstDayOfWeek;
#endif // !ECMA_COMPAT

	// Invariant abbreviated day names.
	private static readonly String[] invAbbrevDayNames =
		{"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

	// Invariant day names.
	private static readonly String[] invDayNames =
		{"Sunday", "Monday", "Tuesday", "Wednesday",
		 "Thursday", "Friday", "Saturday"};

	// Invariant abbreviated month names.
	private static readonly String[] invAbbrevMonthNames =
		{"Jan", "Feb", "Mar", "Apr", "May", "Jun",
		 "Jul", "Aug", "Sep", "Oct", "Nov", "Dec", ""};

	// Invariant month names.
	private static readonly String[] invMonthNames =
		{"January", "February", "March", "April", "May", "June",
		 "July", "August", "September", "October", "November",
		 "December", ""};

	// Constructor.
	public DateTimeFormatInfo()
			{
				readOnly = false;
				amDesignator = "AM";
				pmDesignator = "PM";
				abbreviatedDayNames = invAbbrevDayNames;
				dayNames = invDayNames;
				abbreviatedMonthNames = invAbbrevMonthNames;
				monthNames = invMonthNames;
				dateSeparator = "/";
				timeSeparator = ":";
				fullDateTimePattern = "dddd, dd MMMM yyyy HH:mm:ss";
				longDatePattern = "dddd, dd MMMM yyyy";
				longTimePattern = "HH:mm:ss";
				monthDayPattern = "MMMM dd";
				shortDatePattern = "MM/dd/yyyy";
				shortTimePattern = "HH:mm";
				yearMonthPattern = "yyyy MMMM";
			#if !ECMA_COMPAT
				calendar = new GregorianCalendar();
				calendarWeekRule = CalendarWeekRule.FirstDay;
				firstDayOfWeek = DayOfWeek.Sunday;
			#endif // !ECMA_COMPAT
			}

	// Internal constructor for getting information from a culture.
	[TODO]
	internal DateTimeFormatInfo(CultureInfo culture)
			: this()
			{
				// We currently have the invariant defaults loaded.
				// Call the runtime engine to get culture-specific data.
				// TODO
			}

	// Get the invariant date time format information.
	public static DateTimeFormatInfo InvariantInfo
			{
				get
				{
					lock(typeof(DateTimeFormatInfo))
					{
						if(invariantInfo == null)
						{
							invariantInfo = new DateTimeFormatInfo();
							invariantInfo.readOnly = true;
						}
						return invariantInfo;
					}
				}
			}

	// Get the date time format information for the current thread's culture.
	public static DateTimeFormatInfo CurrentInfo
			{
				get
				{
					return CultureInfo.CurrentCulture.DateTimeFormat;
				}
			}

	// Implement the ICloneable interface.
	public Object Clone()
			{
				return MemberwiseClone();
			}

	// Get the abbreviated name for a month.
	public String GetAbbreviatedMonthName(int month)
			{
				if(month >= 1 && month <= 13)
				{
					return abbreviatedMonthNames[month - 1];
				}
				else
				{
					throw new ArgumentOutOfRangeException
						("month", _("ArgRange_Month"));
				}
			}

	// Get a value that represents an era name.
	[TODO]
	public int GetEra(String eraName)
			{
				// TODO
				return -1;
			}

	// Get the name of a particular era.
	[TODO]
	public String GetEraName(int era)
			{
				// TODO
				return null;
			}

	// Implement the IFormatProvider interface.
	public Object GetFormat(Type formatType)
			{
				if(formatType == typeof(DateTimeFormatInfo))
				{
					return this;
				}
				else
				{
					return CurrentInfo;
				}
			}

	// Get the date time format information associated with "provider".
#if ECMA_COMPAT
	internal
#else
	public
#endif
	static DateTimeFormatInfo GetInstance(IFormatProvider provider)
			{
				if(provider != null)
				{
					Object obj = provider.GetFormat(typeof(DateTimeFormatInfo));
					if(obj != null)
					{
						return (DateTimeFormatInfo)obj;
					}
				}
				return CurrentInfo;
			}

	// Get the full name of a specific month.
	public String GetMonthName(int month)
			{
				if(month >= 1 && month <= 13)
				{
					return monthNames[month - 1];
				}
				else
				{
					throw new ArgumentOutOfRangeException
						("month", _("ArgRange_Month"));
				}
			}

	// Create a read-only copy of a DateTimeFormatInfo object.
	public static DateTimeFormatInfo ReadOnly(DateTimeFormatInfo dtfi)
			{
				if(dtfi == null)
				{
					throw new ArgumentNullException("dtfi");
				}
				else if(dtfi.IsReadOnly)
				{
					return dtfi;
				}
				else
				{
					DateTimeFormatInfo newDtfi;
					newDtfi = (DateTimeFormatInfo)(dtfi.Clone());
					newDtfi.readOnly = true;
					return newDtfi;
				}
			}

#if !ECMA_COMPAT

	// Get the abbreviated name of a week day.
	public String GetAbbreviatedDayName(DayOfWeek dayOfWeek)
			{
				if(dayOfWeek >= DayOfWeek.Sunday &&
				   dayOfWeek <= DayOfWeek.Saturday)
				{
					return abbreviatedDayNames[(int)dayOfWeek];
				}
				else
				{
					throw new ArgumentOutOfRangeException
						("dayOfWeek", _("Arg_DayOfWeek"));
				}
			}

	// Get the full name of a week day.
	public String GetDayName(DayOfWeek dayOfWeek)
			{
				if(dayOfWeek >= DayOfWeek.Sunday &&
				   dayOfWeek <= DayOfWeek.Saturday)
				{
					return dayNames[(int)dayOfWeek];
				}
				else
				{
					throw new ArgumentOutOfRangeException
						("dayOfWeek", _("Arg_DayOfWeek"));
				}
			}

	// Get the abbreviated name of an era.
	[TODO]
	public String GetAbbreviatedEraName(int era)
			{
				// TODO
				return null;
			}

	// Get all date time patterns.
	[TODO]
	public String[] GetAllDateTimePatterns()
			{
				// TODO
				String[] array = new String [1];
				array[0] = FullDateTimePattern;
				return array;
			}
	[TODO]
	public String[] GetAllDateTimePatterns(char format)
			{
				// TODO
				return new String [0];
			}

#endif // !ECMA_COMPAT

	// Properties.
	public String AMDesignator
			{
				get
				{
					return amDesignator;
				}
				set
				{
					if(value == null)
					{
						throw new ArgumentNullException("value");
					}
					if(readOnly)
					{
						throw new InvalidOperationException
							(_("Invalid_ReadOnly"));
					}
					amDesignator = value;
				}
			}
	public String PMDesignator
			{
				get
				{
					return pmDesignator;
				}
				set
				{
					if(value == null)
					{
						throw new ArgumentNullException("value");
					}
					if(readOnly)
					{
						throw new InvalidOperationException
							(_("Invalid_ReadOnly"));
					}
					pmDesignator = value;
				}
			}
	public String[] AbbreviatedDayNames
			{
				get
				{
					return abbreviatedDayNames;
				}
				set
				{
					if(value == null)
					{
						throw new ArgumentNullException("value");
					}
					else if(value.Length != 7)
					{
						throw new ArgumentException(_("Arg_Array7Elems"));
					}
					CheckForNulls(value);
					if(readOnly)
					{
						throw new InvalidOperationException
							(_("Invalid_ReadOnly"));
					}
					abbreviatedDayNames = value;
				}
			}
	public String[] DayNames
			{
				get
				{
					return dayNames;
				}
				set
				{
					if(value == null)
					{
						throw new ArgumentNullException("value");
					}
					else if(value.Length != 7)
					{
						throw new ArgumentException(_("Arg_Array7Elems"));
					}
					CheckForNulls(value);
					if(readOnly)
					{
						throw new InvalidOperationException
							(_("Invalid_ReadOnly"));
					}
					dayNames = value;
				}
			}
	public String[] AbbreviatedMonthNames
			{
				get
				{
					return abbreviatedMonthNames;
				}
				set
				{
					if(value == null)
					{
						throw new ArgumentNullException("value");
					}
					else if(value.Length != 13)
					{
						throw new ArgumentException(_("Arg_Array13Elems"));
					}
					CheckForNulls(value);
					if(readOnly)
					{
						throw new InvalidOperationException
							(_("Invalid_ReadOnly"));
					}
					abbreviatedMonthNames = value;
				}
			}
	public String[] MonthNames
			{
				get
				{
					return monthNames;
				}
				set
				{
					if(value == null)
					{
						throw new ArgumentNullException("value");
					}
					else if(value.Length != 13)
					{
						throw new ArgumentException(_("Arg_Array13Elems"));
					}
					CheckForNulls(value);
					if(readOnly)
					{
						throw new InvalidOperationException
							(_("Invalid_ReadOnly"));
					}
					monthNames = value;
				}
			}
	public String DateSeparator
			{
				get
				{
					return dateSeparator;
				}
				set
				{
					if(value == null)
					{
						throw new ArgumentNullException("value");
					}
					if(readOnly)
					{
						throw new InvalidOperationException
							(_("Invalid_ReadOnly"));
					}
					dateSeparator = value;
				}
			}
	public String TimeSeparator
			{
				get
				{
					return timeSeparator;
				}
				set
				{
					if(value == null)
					{
						throw new ArgumentNullException("value");
					}
					if(readOnly)
					{
						throw new InvalidOperationException
							(_("Invalid_ReadOnly"));
					}
					timeSeparator = value;
				}
			}
	public bool IsReadOnly
			{
				get
				{
					return readOnly;
				}
			}
	public String FullDateTimePattern
			{
				get
				{
					return fullDateTimePattern;
				}
				set
				{
					if(value == null)
					{
						throw new ArgumentNullException("value");
					}
					if(readOnly)
					{
						throw new InvalidOperationException
							(_("Invalid_ReadOnly"));
					}
					fullDateTimePattern = value;
				}
			}
	public String LongDatePattern
			{
				get
				{
					return longDatePattern;
				}
				set
				{
					if(value == null)
					{
						throw new ArgumentNullException("value");
					}
					if(readOnly)
					{
						throw new InvalidOperationException
							(_("Invalid_ReadOnly"));
					}
					longDatePattern = value;
				}
			}
	public String LongTimePattern
			{
				get
				{
					return longTimePattern;
				}
				set
				{
					if(value == null)
					{
						throw new ArgumentNullException("value");
					}
					if(readOnly)
					{
						throw new InvalidOperationException
							(_("Invalid_ReadOnly"));
					}
					longTimePattern = value;
				}
			}
	public String MonthDayPattern
			{
				get
				{
					return monthDayPattern;
				}
				set
				{
					if(value == null)
					{
						throw new ArgumentNullException("value");
					}
					if(readOnly)
					{
						throw new InvalidOperationException
							(_("Invalid_ReadOnly"));
					}
					monthDayPattern = value;
				}
			}
	public String ShortDatePattern
			{
				get
				{
					return shortDatePattern;
				}
				set
				{
					if(value == null)
					{
						throw new ArgumentNullException("value");
					}
					if(readOnly)
					{
						throw new InvalidOperationException
							(_("Invalid_ReadOnly"));
					}
					shortDatePattern = value;
				}
			}
	public String ShortTimePattern
			{
				get
				{
					return shortTimePattern;
				}
				set
				{
					if(value == null)
					{
						throw new ArgumentNullException("value");
					}
					if(readOnly)
					{
						throw new InvalidOperationException
							(_("Invalid_ReadOnly"));
					}
					shortTimePattern = value;
				}
			}
	public String YearMonthPattern
			{
				get
				{
					return yearMonthPattern;
				}
				set
				{
					if(value == null)
					{
						throw new ArgumentNullException("value");
					}
					if(readOnly)
					{
						throw new InvalidOperationException
							(_("Invalid_ReadOnly"));
					}
					yearMonthPattern = value;
				}
			}

#if !ECMA_COMPAT

	// Non-ECMA properties.
	public Calendar Calendar
			{
				get
				{
					return calendar;
				}
				set
				{
					if(value == null)
					{
						throw new ArgumentNullException("value");
					}
					if(readOnly)
					{
						throw new InvalidOperationException
							(_("Invalid_ReadOnly"));
					}

					// Validate the calendar against the current thread.
					if(value is GregorianCalendar)
					{
						// We can always use the Gregorian calendar.
						calendar = value;
						return;
					}
					Calendar temp = CultureInfo.CurrentCulture.Calendar;
					if(temp != null &&
					   temp.GetType().IsAssignableFrom(value.GetType()))
					{
						calendar = value;
						return;
					}
					Calendar[] opt =
						CultureInfo.CurrentCulture.OptionalCalendars;
					if(opt != null)
					{
						int posn;
						for(posn = 0; posn < opt.Length; ++posn)
						{
							temp = opt[posn];
							if(temp != null &&
							   temp.GetType().IsAssignableFrom(value.GetType()))
							{
								calendar = value;
								return;
							}
						}
					}

					// The calendar is invalid for the current culture.
					throw new ArgumentException(_("Arg_InvalidCalendar"));
				}
			}
	public CalendarWeekRule CalendarWeekRule
			{
				get
				{
					return calendarWeekRule;
				}
				set
				{
					if(value != CalendarWeekRule.FirstDay &&
					   value != CalendarWeekRule.FirstFullWeek &&
					   value != CalendarWeekRule.FirstFourDayWeek)
					{
						throw new ArgumentOutOfRangeException
							(_("Arg_CalendarWeekRule"));
					}
					if(readOnly)
					{
						throw new InvalidOperationException
							(_("Invalid_ReadOnly"));
					}
					calendarWeekRule = value;
				}
			}
	public DayOfWeek FirstDayOfWeek
			{
				get
				{
					return firstDayOfWeek;
				}
				set
				{
					if(value < DayOfWeek.Sunday || value > DayOfWeek.Saturday)
					{
						throw new ArgumentOutOfRangeException
							(_("Arg_DayOfWeek"));
					}
					if(readOnly)
					{
						throw new InvalidOperationException
							(_("Invalid_ReadOnly"));
					}
					firstDayOfWeek = value;
				}
			}
	public String RFC1123Pattern
			{
				get
				{
					return "ddd, dd MMM yyyy HH':'mm':'ss 'GMT'";
				}
			}
	public String SortableDateTimePattern
			{
				get
				{
					return "yyyy'-'MM'-'dd'T'HH':'mm':'ss";
				}
			}
	public String UniversalSortableDateTimePattern
			{
				get
				{
					return "yyyy'-'MM'-'dd HH':'mm':'ss'Z'";
				}
			}

#endif // !ECMA_COMPAT

	// Check for null strings in an array.
	private static void CheckForNulls(String[] value)
			{
				int index;
				for(index = 0; index < value.Length; ++index)
				{
					if(value[index] == null)
					{
						throw new ArgumentNullException
							("value[" + index.ToString() + "]");
					}
				}
			}

}; // class DateTimeFormatInfo

}; // namespace System.Globalization
