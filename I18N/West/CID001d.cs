/*
 * CID001d.cs - sv culture handler.
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

// Generated from "sv.txt".

namespace I18N.West
{

using System;
using System.Globalization;
using I18N.Common;

public class CID001d : RootCulture
{
	private CultureName cultureName;

	public CID001d()
		: base(0x001D, CultureNameTable.GetNameInfoByID(0x001D)) {}
	public CID001d(int culture, CultureName cultureName)
		: base(culture, cultureName) {}

	public override DateTimeFormatInfo DateTimeFormat
	{
		get
		{
			DateTimeFormatInfo dfi = base.DateTimeFormat;
			dfi.AbbreviatedDayNames = new String[] {"s\u00F6", "m\u00E5", "ti", "on", "to", "fr", "l\u00F6"};
			dfi.DayNames = new String[] {"s\u00F6ndag", "m\u00E5ndag", "tisdag", "onsdag", "torsdag", "fredag", "l\u00F6rdag"};
			dfi.AbbreviatedMonthNames = new String[] {"jan", "feb", "mar", "apr", "maj", "jun", "jul", "aug", "sep", "okt", "nov", "dec", ""};
			dfi.MonthNames = new String[] {"januari", "februari", "mars", "april", "maj", "juni", "juli", "augusti", "september", "oktober", "november", "december", ""};
			dfi.DateSeparator = "-";
			dfi.TimeSeparator = ":";
			dfi.LongDatePattern = "'den 'd MMMM yyyy";
			dfi.LongTimePattern = "HH:mm:ss z";
			dfi.ShortDatePattern = "yyyy-MM-dd";
			dfi.ShortTimePattern = "HH:mm";
			dfi.FullDateTimePattern = "'den 'd MMMM yyyy 'kl 'H:mm z";
#if !ECMA_COMPAT
			dfi.I18NSetDateTimePatterns(new String[] {
				"d:yyyy-MM-dd",
				"D:'den 'd MMMM yyyy",
				"f:'den 'd MMMM yyyy 'kl 'H:mm z",
				"f:'den 'd MMMM yyyy HH:mm:ss z",
				"f:'den 'd MMMM yyyy HH:mm:ss",
				"f:'den 'd MMMM yyyy HH:mm",
				"F:'den 'd MMMM yyyy HH:mm:ss",
				"g:yyyy-MM-dd 'kl 'H:mm z",
				"g:yyyy-MM-dd HH:mm:ss z",
				"g:yyyy-MM-dd HH:mm:ss",
				"g:yyyy-MM-dd HH:mm",
				"G:yyyy-MM-dd HH:mm:ss",
				"m:MMMM dd",
				"M:MMMM dd",
				"r:ddd, dd MMM yyyy HH':'mm':'ss 'GMT'",
				"R:ddd, dd MMM yyyy HH':'mm':'ss 'GMT'",
				"s:yyyy'-'MM'-'dd'T'HH':'mm':'ss",
				"t:'kl 'H:mm z",
				"t:HH:mm:ss z",
				"t:HH:mm:ss",
				"t:HH:mm",
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
			NumberFormatInfo nfi = base.NumberFormat;
			nfi.CurrencyDecimalSeparator = ",";
			nfi.CurrencyGroupSeparator = "\u00A0";
			nfi.NumberGroupSeparator = "\u00A0";
			nfi.PercentGroupSeparator = "\u00A0";
			nfi.NegativeSign = "-";
			nfi.NumberDecimalSeparator = ",";
			nfi.PercentDecimalSeparator = ",";
			nfi.PercentSymbol = "%";
			nfi.PerMilleSymbol = "\u2030";
			return nfi;
		}
		set
		{
			base.NumberFormat = value; // not used
		}
	}

}; // class CID001d

public class CNsv : CID001d
{
	public CNsv() : base() {}

}; // class CNsv

}; // namespace I18N.West
