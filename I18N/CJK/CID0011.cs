/*
 * CID0011.cs - ja culture handler.
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

// Generated from "ja.txt".

namespace I18N.CJK
{

using System;
using System.Globalization;
using I18N.Common;

public class CID0011 : RootCulture
{
	private CultureName cultureName;

	public CID0011()
		: base(0x0011, CultureNameTable.GetNameInfoByID(0x0011)) {}
	public CID0011(int culture, CultureName cultureName)
		: base(culture, cultureName) {}

	public override DateTimeFormatInfo DateTimeFormat
	{
		get
		{
			DateTimeFormatInfo dfi = base.DateTimeFormat;
			dfi.AMDesignator = "\u5348\u524D";
			dfi.PMDesignator = "\u5348\u5F8C";
			dfi.AbbreviatedDayNames = new String[] {"\u65E5", "\u6708", "\u706B", "\u6C34", "\u6728", "\u91D1", "\u571F"};
			dfi.DayNames = new String[] {"\u65E5\u66DC\u65E5", "\u6708\u66DC\u65E5", "\u706B\u66DC\u65E5", "\u6C34\u66DC\u65E5", "\u6728\u66DC\u65E5", "\u91D1\u66DC\u65E5", "\u571F\u66DC\u65E5"};
			dfi.AbbreviatedMonthNames = new String[] {"1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", ""};
			dfi.MonthNames = new String[] {"1\u6708", "2\u6708", "3\u6708", "4\u6708", "5\u6708", "6\u6708", "7\u6708", "8\u6708", "9\u6708", "10\u6708", "11\u6708", "12\u6708", ""};
			dfi.DateSeparator = "/";
			dfi.TimeSeparator = ":";
			dfi.LongDatePattern = "yyyy/MM/dd";
			dfi.LongTimePattern = "H:mm:ss:z";
			dfi.ShortDatePattern = "yy/MM/dd";
			dfi.ShortTimePattern = "H:mm";
			dfi.FullDateTimePattern = "yyyy'\u5d74'M'\u6708'd'\u65d5' H'\u6642'mm'\u5206'ss'\u79D2'z";
#if !ECMA_COMPAT
			dfi.I18NSetDateTimePatterns(new String[] {
				"d:yy/MM/dd",
				"D:yyyy'\u5d74'M'\u6708'd'\u65d5'",
				"f:yyyy'\u5d74'M'\u6708'd'\u65d5' H'\u6642'mm'\u5206'ss'\u79D2'z",
				"f:yyyy'\u5d74'M'\u6708'd'\u65d5' H:mm:ss:z",
				"f:yyyy'\u5d74'M'\u6708'd'\u65d5' H:mm:ss",
				"f:yyyy'\u5d74'M'\u6708'd'\u65d5' H:mm",
				"F:yyyy'\u5d74'M'\u6708'd'\u65d5' HH:mm:ss",
				"g:yy/MM/dd H'\u6642'mm'\u5206'ss'\u79D2'z",
				"g:yy/MM/dd H:mm:ss:z",
				"g:yy/MM/dd H:mm:ss",
				"g:yy/MM/dd H:mm",
				"G:yy/MM/dd HH:mm:ss",
				"m:MMMM dd",
				"M:MMMM dd",
				"r:ddd, dd MMM yyyy HH':'mm':'ss 'GMT'",
				"R:ddd, dd MMM yyyy HH':'mm':'ss 'GMT'",
				"s:yyyy'-'MM'-'dd'T'HH':'mm':'ss",
				"t:H'\u6642'mm'\u5206'ss'\u79D2'z",
				"t:H:mm:ss:z",
				"t:H:mm:ss",
				"t:H:mm",
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

}; // class CID0011

public class CNja : CID0011
{
	public CNja() : base() {}

}; // class CNja

}; // namespace I18N.CJK
