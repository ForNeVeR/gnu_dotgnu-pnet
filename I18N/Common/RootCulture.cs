/*
 * RootCulture.cs - root culture handler.
 *
 * Copyright (c) 2003  Southern Storm Software, Pty Ltd
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

// Generated from "root.txt".

namespace I18N.Common
{

using System;
using System.Globalization;

public class RootCulture : CultureInfo
{
	private CultureName cultureName;

	public RootCulture(int culture, CultureName cultureName)
		: base(0x40000000 + culture)
	{
		this.cultureName = cultureName;
	}

	public override String EnglishName
	{
		get
		{
			return cultureName.englishName;
		}
	}
	public override String Name
	{
		get
		{
			return cultureName.name;
		}
	}
	public override String NativeName
	{
		get
		{
			return cultureName.nativeName;
		}
	}
	public override String ThreeLetterISOLanguageName
	{
		get
		{
			return cultureName.threeLetterISOName;
		}
	}
	public override String ThreeLetterWindowsLanguageName
	{
		get
		{
			return cultureName.threeLetterWindowsName;
		}
	}
	public override String TwoLetterISOLanguageName
	{
		get
		{
			return cultureName.twoLetterISOName;
		}
	}

	public override DateTimeFormatInfo DateTimeFormat
	{
		get
		{
			DateTimeFormatInfo dfi = new DateTimeFormatInfo();
			dfi.AMDesignator = "AM";
			dfi.PMDesignator = "PM";
			dfi.AbbreviatedDayNames = new String[] {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
			dfi.DayNames = new String[] {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
			dfi.AbbreviatedMonthNames = new String[] {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec", ""};
			dfi.MonthNames = new String[] {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December", ""};
			dfi.DateSeparator = "/";
			dfi.TimeSeparator = ":";
			dfi.LongDatePattern = "MMMM d, yyyy";
			dfi.LongTimePattern = "h:mm:ss tt z";
			dfi.ShortDatePattern = "M/d/yy";
			dfi.ShortTimePattern = "h:mm tt";
			dfi.FullDateTimePattern = "dddd, MMMM d, yyyy h:mm:ss tt z";
#if !ECMA_COMPAT
			dfi.I18NSetDateTimePatterns(new String[] {
				"d:M/d/yy",
				"D:dddd, MMMM d, yyyy",
				"f:dddd, MMMM d, yyyy h:mm:ss tt z",
				"f:dddd, MMMM d, yyyy h:mm:ss tt z",
				"f:dddd, MMMM d, yyyy h:mm:ss tt",
				"f:dddd, MMMM d, yyyy h:mm tt",
				"F:dddd, MMMM d, yyyy HH:mm:ss",
				"g:M/d/yy h:mm:ss tt z",
				"g:M/d/yy h:mm:ss tt z",
				"g:M/d/yy h:mm:ss tt",
				"g:M/d/yy h:mm tt",
				"G:M/d/yy HH:mm:ss",
				"m:MMMM dd",
				"M:MMMM dd",
				"r:ddd, dd MMM yyyy HH':'mm':'ss 'GMT'",
				"R:ddd, dd MMM yyyy HH':'mm':'ss 'GMT'",
				"s:yyyy'-'MM'-'dd'T'HH':'mm':'ss",
				"t:h:mm:ss tt z",
				"t:h:mm:ss tt z",
				"t:h:mm:ss tt",
				"t:h:mm tt",
				"T:HH:mm:ss",
				"u:yyyy'-'MM'-'dd HH':'mm':'ss'Z'",
				"U:dddd, dd MMMM yyyy HH:mm:ss",
				"y:yyyy MMMM",
				"Y:yyyy MMMM",
			});
#endif // !ECMA_COMPAT
			return dfi;
		}
		set
		{
			base.DateTimeFormat = value; // not used
		}
	}

	public override NumberFormatInfo NumberFormat
	{
		get
		{
			NumberFormatInfo nfi = new NumberFormatInfo();
			nfi.CurrencyDecimalSeparator = ".";
			nfi.CurrencyGroupSeparator = ",";
			nfi.NumberGroupSeparator = ",";
			nfi.PercentGroupSeparator = ",";
			nfi.NegativeSign = "-";
			nfi.NumberDecimalSeparator = ".";
			nfi.PercentDecimalSeparator = ".";
			nfi.PercentSymbol = "%";
			nfi.PerMilleSymbol = "\u2030";
			return nfi;
		}
		set
		{
			base.NumberFormat = value; // not used
		}
	}

}; // class RootCulture

}; // namespace I18N.Common
