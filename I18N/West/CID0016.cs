/*
 * CID0016.cs - pt culture handler.
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

// Generated from "pt.txt".

namespace I18N.West
{

using System;
using System.Globalization;
using I18N.Common;

public class CID0016 : RootCulture
{
	private CultureName cultureName;

	public CID0016()
		: base(0x0016, CultureNameTable.GetNameInfoByID(0x0016)) {}
	public CID0016(int culture, CultureName cultureName)
		: base(culture, cultureName) {}

	public override DateTimeFormatInfo DateTimeFormat
	{
		get
		{
			DateTimeFormatInfo dfi = base.DateTimeFormat;
			dfi.AbbreviatedDayNames = new String[] {"Dom", "Seg", "Ter", "Qua", "Qui", "Sex", "S\u00E1b"};
			dfi.DayNames = new String[] {"Domingo", "Segunda-feira", "Ter\u00E7a-feira", "Quarta-feira", "Quinta-feira", "Sexta-feira", "S\u00E1bado"};
			dfi.AbbreviatedMonthNames = new String[] {"Jan", "Fev", "Mar", "Abr", "Mai", "Jun", "Jul", "Ago", "Set", "Out", "Nov", "Dez", ""};
			dfi.MonthNames = new String[] {"Janeiro", "Fevereiro", "Mar\u00E7o", "Abril", "Maio", "Junho", "Julho", "Agosto", "Setembro", "Outubro", "Novembro", "Dezembro", ""};
			dfi.DateSeparator = "-";
			dfi.TimeSeparator = ":";
			dfi.LongDatePattern = "d' de 'MMMM' de 'yyyy";
			dfi.LongTimePattern = "HH:mm:ss z";
			dfi.ShortDatePattern = "dd-MM-yyyy";
			dfi.ShortTimePattern = "HH:mm";
			dfi.FullDateTimePattern = "dddd, d' de 'MMMM' de 'yyyy HH'H'mm'm' z";
#if !ECMA_COMPAT
			dfi.I18NSetDateTimePatterns(new String[] {
				"d:dd-MM-yyyy",
				"D:dddd, d' de 'MMMM' de 'yyyy",
				"f:dddd, d' de 'MMMM' de 'yyyy HH'H'mm'm' z",
				"f:dddd, d' de 'MMMM' de 'yyyy HH:mm:ss z",
				"f:dddd, d' de 'MMMM' de 'yyyy HH:mm:ss",
				"f:dddd, d' de 'MMMM' de 'yyyy HH:mm",
				"F:dddd, d' de 'MMMM' de 'yyyy HH:mm:ss",
				"g:dd-MM-yyyy HH'H'mm'm' z",
				"g:dd-MM-yyyy HH:mm:ss z",
				"g:dd-MM-yyyy HH:mm:ss",
				"g:dd-MM-yyyy HH:mm",
				"G:dd-MM-yyyy HH:mm:ss",
				"m:MMMM dd",
				"M:MMMM dd",
				"r:ddd, dd MMM yyyy HH':'mm':'ss 'GMT'",
				"R:ddd, dd MMM yyyy HH':'mm':'ss 'GMT'",
				"s:yyyy'-'MM'-'dd'T'HH':'mm':'ss",
				"t:HH'H'mm'm' z",
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
			nfi.CurrencyGroupSeparator = ".";
			nfi.NumberGroupSeparator = ".";
			nfi.PercentGroupSeparator = ".";
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

}; // class CID0016

public class CNpt : CID0016
{
	public CNpt() : base() {}

}; // class CNpt

}; // namespace I18N.West
