/*
 * CID000b.cs - fi culture handler.
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

// Generated from "fi.txt".

namespace I18N.West
{

using System;
using System.Globalization;
using I18N.Common;

public class CID000b : RootCulture
{
	private CultureName cultureName;

	public CID000b()
		: base(0x000B, CultureNameTable.GetNameInfoByID(0x000B)) {}
	public CID000b(int culture, CultureName cultureName)
		: base(culture, cultureName) {}

	public override DateTimeFormatInfo DateTimeFormat
	{
		get
		{
			DateTimeFormatInfo dfi = base.DateTimeFormat;
			dfi.AbbreviatedDayNames = new String[] {"su", "ma", "ti", "ke", "to", "pe", "la"};
			dfi.DayNames = new String[] {"sunnuntai", "maanantai", "tiistai", "keskiviikko", "torstai", "perjantai", "lauantai"};
			dfi.AbbreviatedMonthNames = new String[] {"tammi", "helmi", "maalis", "huhti", "touko", "kes\u00E4", "hein\u00E4", "elo", "syys", "loka", "marras", "joulu", ""};
			dfi.MonthNames = new String[] {"tammikuu", "helmikuu", "maaliskuu", "huhtikuu", "toukokuu", "kes\u00E4kuu", "hein\u00E4kuu", "elokuu", "syyskuu", "lokakuu", "marraskuu", "joulukuu", ""};
			dfi.DateSeparator = ".";
			dfi.TimeSeparator = ":";
			dfi.LongDatePattern = "d. MMMM'ttt 'yyyy";
			dfi.LongTimePattern = "HH:mm:ss z";
			dfi.ShortDatePattern = "d.M.yyyy";
			dfi.ShortTimePattern = "HH:mm";
			dfi.FullDateTimePattern = "d. MMMM'ttt 'yyyy HH:mm:ss z";
#if !ECMA_COMPAT
			dfi.I18NSetDateTimePatterns(new String[] {
				"d:d.M.yyyy",
				"D:d. MMMM'ttt 'yyyy",
				"f:d. MMMM'ttt 'yyyy HH:mm:ss z",
				"f:d. MMMM'ttt 'yyyy HH:mm:ss z",
				"f:d. MMMM'ttt 'yyyy HH:mm:ss",
				"f:d. MMMM'ttt 'yyyy HH:mm",
				"F:d. MMMM'ttt 'yyyy HH:mm:ss",
				"g:d.M.yyyy HH:mm:ss z",
				"g:d.M.yyyy HH:mm:ss z",
				"g:d.M.yyyy HH:mm:ss",
				"g:d.M.yyyy HH:mm",
				"G:d.M.yyyy HH:mm:ss",
				"m:MMMM dd",
				"M:MMMM dd",
				"r:ddd, dd MMM yyyy HH':'mm':'ss 'GMT'",
				"R:ddd, dd MMM yyyy HH':'mm':'ss 'GMT'",
				"s:yyyy'-'MM'-'dd'T'HH':'mm':'ss",
				"t:HH:mm:ss z",
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

}; // class CID000b

public class CNfi : CID000b
{
	public CNfi() : base() {}

}; // class CNfi

}; // namespace I18N.West
