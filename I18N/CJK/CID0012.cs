/*
 * CID0012.cs - ko culture handler.
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

// Generated from "ko.txt".

namespace I18N.CJK
{

using System;
using System.Globalization;
using I18N.Common;

public class CID0012 : RootCulture
{
	private CultureName cultureName;

	public CID0012()
		: base(0x0012, CultureNameTable.GetNameInfoByID(0x0012)) {}
	public CID0012(int culture, CultureName cultureName)
		: base(culture, cultureName) {}

	public override DateTimeFormatInfo DateTimeFormat
	{
		get
		{
			DateTimeFormatInfo dfi = base.DateTimeFormat;
			dfi.AMDesignator = "\uC624\uC804";
			dfi.PMDesignator = "\uC624\uD6C4";
			dfi.AbbreviatedDayNames = new String[] {"\uC77C", "\uC6D4", "\uD654", "\uC218", "\uBAA9", "\uAE08", "\uD1A0"};
			dfi.DayNames = new String[] {"\uC77C\uC694\uC77C", "\uC6D4\uC694\uC77C", "\uD654\uC694\uC77C", "\uC218\uC694\uC77C", "\uBAA9\uC694\uC77C", "\uAE08\uC694\uC77C", "\uD1A0\uC694\uC77C"};
			dfi.AbbreviatedMonthNames = new String[] {"1\uC6D4", "2\uC6D4", "3\uC6D4", "4\uC6D4", "5\uC6D4", "6\uC6D4", "7\uC6D4", "8\uC6D4", "9\uC6D4", "10\uC6D4", "11\uC6D4", "12\uC6D4", ""};
			dfi.MonthNames = new String[] {"1\uC6D4", "2\uC6D4", "3\uC6D4", "4\uC6D4", "5\uC6D4", "6\uC6D4", "7\uC6D4", "8\uC6D4", "9\uC6D4", "10\uC6D4", "11\uC6D4", "12\uC6D4", ""};
			dfi.DateSeparator = "-";
			dfi.TimeSeparator = ":";
			dfi.LongDatePattern = "yyyy'\uB144' M'\uC6D4' d'\uC77C' dd";
			dfi.LongTimePattern = "tt hh'\uC2DC'mm'\uBD84'ss'\uCD08'";
			dfi.ShortDatePattern = "yy-MM-dd";
			dfi.ShortTimePattern = "tt h:mm";
			dfi.FullDateTimePattern = "yyyy'\uB144' M'\uC6D4' d'\uC77C' dddd tt hh'\uC2DC'mm'\uBD84'ss'\uCD08' z";
#if !ECMA_COMPAT
			dfi.I18NSetDateTimePatterns(new String[] {
				"d:yy-MM-dd",
				"D:yyyy'\uB144' M'\uC6D4' d'\uC77C' dddd",
				"f:yyyy'\uB144' M'\uC6D4' d'\uC77C' dddd tt hh'\uC2DC'mm'\uBD84'ss'\uCD08' z",
				"f:yyyy'\uB144' M'\uC6D4' d'\uC77C' dddd tt hh'\uC2DC'mm'\uBD84'ss'\uCD08'",
				"f:yyyy'\uB144' M'\uC6D4' d'\uC77C' dddd tt h:mm:ss",
				"f:yyyy'\uB144' M'\uC6D4' d'\uC77C' dddd tt h:mm",
				"F:yyyy'\uB144' M'\uC6D4' d'\uC77C' dddd HH:mm:ss",
				"g:yy-MM-dd tt hh'\uC2DC'mm'\uBD84'ss'\uCD08' z",
				"g:yy-MM-dd tt hh'\uC2DC'mm'\uBD84'ss'\uCD08'",
				"g:yy-MM-dd tt h:mm:ss",
				"g:yy-MM-dd tt h:mm",
				"G:yy-MM-dd HH:mm:ss",
				"m:MMMM dd",
				"M:MMMM dd",
				"r:ddd, dd MMM yyyy HH':'mm':'ss 'GMT'",
				"R:ddd, dd MMM yyyy HH':'mm':'ss 'GMT'",
				"s:yyyy'-'MM'-'dd'T'HH':'mm':'ss",
				"t:tt hh'\uC2DC'mm'\uBD84'ss'\uCD08' z",
				"t:tt hh'\uC2DC'mm'\uBD84'ss'\uCD08'",
				"t:tt h:mm:ss",
				"t:tt h:mm",
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

}; // class CID0012

public class CNko : CID0012
{
	public CNko() : base() {}

}; // class CNko

}; // namespace I18N.CJK
