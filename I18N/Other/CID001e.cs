/*
 * CID001e.cs - th culture handler.
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

// Generated from "th.txt".

namespace I18N.Other
{

using System;
using System.Globalization;
using I18N.Common;

public class CID001e : RootCulture
{
	private CultureName cultureName;

	public CID001e()
		: base(0x001E, CultureNameTable.GetNameInfoByID(0x001E)) {}
	public CID001e(int culture, CultureName cultureName)
		: base(culture, cultureName) {}

	public override DateTimeFormatInfo DateTimeFormat
	{
		get
		{
			DateTimeFormatInfo dfi = base.DateTimeFormat;
			dfi.AMDesignator = "\u0E01\u0E48\u0E2D\u0E19\u0E40\u0E17\u0E35\u0E48\u0E22\u0E07";
			dfi.PMDesignator = "\u0E2B\u0E25\u0E31\u0E07\u0E40\u0E17\u0E35\u0E48\u0E22\u0E07";
			dfi.AbbreviatedDayNames = new String[] {"\u0E2D\u0E32.", "\u0E08.", "\u0E2D.", "\u0E1E.", "\u0E1E\u0E24.", "\u0E28.", "\u0E2A."};
			dfi.DayNames = new String[] {"\u0E27\u0E31\u0E19\u0E2D\u0E32\u0E17\u0E34\u0E15\u0E22\u0E4C", "\u0E27\u0E31\u0E19\u0E08\u0E31\u0E19\u0E17\u0E23\u0E4C", "\u0E27\u0E31\u0E19\u0E2D\u0E31\u0E07\u0E04\u0E32\u0E23", "\u0E27\u0E31\u0E19\u0E1E\u0E38\u0E18", "\u0E27\u0E31\u0E19\u0E1E\u0E24\u0E2B\u0E31\u0E2A\u0E1A\u0E14\u0E35", "\u0E27\u0E31\u0E19\u0E28\u0E38\u0E01\u0E23\u0E4C", "\u0E27\u0E31\u0E19\u0E40\u0E2A\u0E32\u0E23\u0E4C"};
			dfi.AbbreviatedMonthNames = new String[] {"\u0E21.\u0E04.", "\u0E01.\u0E1E.", "\u0E21\u0E35.\u0E04.", "\u0E40\u0E21.\u0E22.", "\u0E1E.\u0E04.", "\u0E21\u0E34.\u0E22.", "\u0E01.\u0E04.", "\u0E2A.\u0E04.", "\u0E01.\u0E22.", "\u0E15.\u0E04.", "\u0E1E.\u0E22.", "\u0E18.\u0E04.", ""};
			dfi.MonthNames = new String[] {"\u0E21\u0E01\u0E23\u0E32\u0E04\u0E21", "\u0E01\u0E38\u0E21\u0E20\u0E32\u0E1E\u0E31\u0E19\u0E18\u0E4C", "\u0E21\u0E35\u0E19\u0E32\u0E04\u0E21", "\u0E40\u0E21\u0E29\u0E32\u0E22\u0E19", "\u0E1E\u0E24\u0E29\u0E20\u0E32\u0E04\u0E21", "\u0E21\u0E34\u0E16\u0E38\u0E19\u0E32\u0E22\u0E19", "\u0E01\u0E23\u0E01\u0E0E\u0E32\u0E04\u0E21", "\u0E2A\u0E34\u0E07\u0E2B\u0E32\u0E04\u0E21", "\u0E01\u0E31\u0E19\u0E22\u0E32\u0E22\u0E19", "\u0E15\u0E38\u0E25\u0E32\u0E04\u0E21", "\u0E1E\u0E24\u0E28\u0E08\u0E34\u0E01\u0E32\u0E22\u0E19", "\u0E18\u0E31\u0E19\u0E27\u0E32\u0E04\u0E21", ""};
			dfi.DateSeparator = "/";
			dfi.TimeSeparator = ":";
			dfi.LongDatePattern = "dddd'\u0d17\u0d35\u0d48 'd MMMM G yyyy";
			dfi.LongTimePattern = "\u0d34\u0d19\u0d32\u0d17\u0d35'";
			dfi.ShortDatePattern = "d MMM yyyy";
			dfi.ShortTimePattern = "H:mm:ss";
			dfi.FullDateTimePattern = "H:mm H' \u0d19\u0d32\u0d2C\u0d34\u0d01\u0d32 'm' \u0d19\u0d32\u0d17\u0d35 'ss' \u0d27";
#if !ECMA_COMPAT
			dfi.I18NSetDateTimePatterns(new String[] {
				"d:d MMM yyyy",
				"D:H:mm",
				"f:H:mm H' \u0d19\u0d32\u0d2C\u0d34\u0d01\u0d32 'm' \u0d19\u0d32\u0d17\u0d35 'ss' \u0d27",
				"f:H:mm \u0d34\u0d19\u0d32\u0d17\u0d35'",
				"f:H:mm H' \u0d19\u0d32\u0d2C\u0d34\u0d01\u0d32 'm' \u0d19\u0d32\u0d17\u0d35'",
				"f:H:mm H:mm:ss",
				"F:H:mm HH:mm:ss",
				"g:d MMM yyyy H' \u0d19\u0d32\u0d2C\u0d34\u0d01\u0d32 'm' \u0d19\u0d32\u0d17\u0d35 'ss' \u0d27",
				"g:d MMM yyyy \u0d34\u0d19\u0d32\u0d17\u0d35'",
				"g:d MMM yyyy H' \u0d19\u0d32\u0d2C\u0d34\u0d01\u0d32 'm' \u0d19\u0d32\u0d17\u0d35'",
				"g:d MMM yyyy H:mm:ss",
				"G:d MMM yyyy HH:mm:ss",
				"m:MMMM dd",
				"M:MMMM dd",
				"r:ddd, dd MMM yyyy HH':'mm':'ss 'GMT'",
				"R:ddd, dd MMM yyyy HH':'mm':'ss 'GMT'",
				"s:yyyy'-'MM'-'dd'T'HH':'mm':'ss",
				"t:H' \u0d19\u0d32\u0d2C\u0d34\u0d01\u0d32 'm' \u0d19\u0d32\u0d17\u0d35 'ss' \u0d27",
				"t:\u0d34\u0d19\u0d32\u0d17\u0d35'",
				"t:H' \u0d19\u0d32\u0d2C\u0d34\u0d01\u0d32 'm' \u0d19\u0d32\u0d17\u0d35'",
				"t:H:mm:ss",
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

}; // class CID001e

public class CNth : CID001e
{
	public CNth() : base() {}

}; // class CNth

}; // namespace I18N.Other
