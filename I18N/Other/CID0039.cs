/*
 * CID0039.cs - hi culture handler.
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

// Generated from "hi.txt".

namespace I18N.Other
{

using System;
using System.Globalization;
using I18N.Common;

public class CID0039 : RootCulture
{
	private CultureName cultureName;

	public CID0039()
		: base(0x0039, CultureNameTable.GetNameInfoByID(0x0039)) {}
	public CID0039(int culture, CultureName cultureName)
		: base(culture, cultureName) {}

	public override DateTimeFormatInfo DateTimeFormat
	{
		get
		{
			DateTimeFormatInfo dfi = base.DateTimeFormat;
			dfi.AMDesignator = "\u092a\u0942\u0930\u094d\u0935\u093e\u0939\u094d\u0928";
			dfi.PMDesignator = "\u0905\u092a\u0930\u093e\u0939\u094d\u0928";
			dfi.AbbreviatedDayNames = new String[] {"\u0930\u0935\u093f", "\u0938\u094b\u092e", "\u092e\u0902\u0917\u0932", "\u092c\u0941\u0927", "\u0917\u0941\u0930\u0941", "\u0936\u0941\u0915\u094d\u0930", "\u0936\u0928\u093f"};
			dfi.DayNames = new String[] {"\u0930\u0935\u093f\u0935\u093e\u0930", "\u0938\u094b\u092e\u0935\u093e\u0930", "\u092e\u0902\u0917\u0932\u0935\u093e\u0930", "\u092c\u0941\u0927\u0935\u093e\u0930", "\u0917\u0941\u0930\u0941\u0935\u093e\u0930", "\u0936\u0941\u0915\u094d\u0930\u0935\u093e\u0930", "\u0936\u0928\u093f\u0935\u093e\u0930"};
			dfi.AbbreviatedMonthNames = new String[] {"\u091c\u0928\u0935\u0930\u0940", "\u092b\u0930\u0935\u0930\u0940", "\u092e\u093e\u0930\u094d\u091a", "\u0905\u092a\u094d\u0930\u0948\u0932", "\u092e\u0908", "\u091c\u0942\u0928", "\u091c\u0941\u0932\u093e\u0908", "\u0905\u0917\u0938\u094d\u0924", "\u0938\u093F\u0924\u092E\u094D\u092C\u0930", "\u0905\u0915\u094d\u0924\u0942\u092c\u0930", "\u0928\u0935\u092E\u094D\u092C\u0930", "\u0926\u093F\u0938\u092E\u094D\u092C\u0930", ""};
			dfi.MonthNames = new String[] {"\u091c\u0928\u0935\u0930\u0940", "\u092b\u0930\u0935\u0930\u0940", "\u092e\u093e\u0930\u094d\u091a", "\u0905\u092a\u094d\u0930\u0948\u0932", "\u092e\u0908", "\u091c\u0942\u0928", "\u091c\u0941\u0932\u093e\u0908", "\u0905\u0917\u0938\u094d\u0924", "\u0938\u093F\u0924\u092E\u094D\u092C\u0930", "\u0905\u0915\u094d\u0924\u0942\u092c\u0930", "\u0928\u0935\u092E\u094D\u092C\u0930", "\u0926\u093F\u0938\u092E\u094D\u092C\u0930", ""};
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

}; // class CID0039

public class CNhi : CID0039
{
	public CNhi() : base() {}

}; // class CNhi

}; // namespace I18N.Other
