/*
 * CID0057.cs - kok culture handler.
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

// Generated from "kok.txt".

namespace I18N.Other
{

using System;
using System.Globalization;
using I18N.Common;

public class CID0057 : RootCulture
{
	private CultureName cultureName;

	public CID0057()
		: base(0x0057, CultureNameTable.GetNameInfoByID(0x0057)) {}
	public CID0057(int culture, CultureName cultureName)
		: base(culture, cultureName) {}

	public override DateTimeFormatInfo DateTimeFormat
	{
		get
		{
			DateTimeFormatInfo dfi = base.DateTimeFormat;
			dfi.AMDesignator = "\u092e.\u092a\u0942.";
			dfi.PMDesignator = "\u092e.\u0928\u0902.";
			dfi.AbbreviatedDayNames = new String[] {"\u0930\u0935\u093f", "\u0938\u094b\u092e", "\u092e\u0902\u0917\u0933", "\u092c\u0941\u0927", "\u0917\u0941\u0930\u0941", "\u0936\u0941\u0915\u094d\u0930", "\u0936\u0928\u093f"};
			dfi.DayNames = new String[] {"\u0906\u0926\u093f\u0924\u094d\u092f\u0935\u093e\u0930", "\u0938\u094b\u092e\u0935\u093e\u0930", "\u092e\u0902\u0917\u0933\u093e\u0930", "\u092c\u0941\u0927\u0935\u093e\u0930", "\u0917\u0941\u0930\u0941\u0935\u093e\u0930", "\u0936\u0941\u0915\u094d\u0930\u0935\u093e\u0930", "\u0936\u0928\u093f\u0935\u093e\u0930"};
			dfi.AbbreviatedMonthNames = new String[] {"\u091c\u093e\u0928\u0947\u0935\u093e\u0930\u0940", "\u092b\u0947\u092c\u0943\u0935\u093e\u0930\u0940", "\u092e\u093e\u0930\u094d\u091a", "\u090f\u092a\u094d\u0930\u093f\u0932", "\u092e\u0947", "\u091c\u0942\u0928", "\u091c\u0941\u0932\u0948", "\u0913\u0917\u0938\u094d\u091f", "\u0938\u0947\u092a\u094d\u091f\u0947\u0902\u092c\u0930", "\u0913\u0915\u094d\u091f\u094b\u092c\u0930", "\u0928\u094b\u0935\u094d\u0939\u0947\u0902\u092c\u0930", "\u0921\u093f\u0938\u0947\u0902\u092c\u0930", ""};
			dfi.MonthNames = new String[] {"\u091c\u093e\u0928\u0947\u0935\u093e\u0930\u0940", "\u092b\u0947\u092c\u094d\u0930\u0941\u0935\u093e\u0930\u0940", "\u092e\u093e\u0930\u094d\u091a", "\u090f\u092a\u094d\u0930\u093f\u0932", "\u092e\u0947", "\u091c\u0942\u0928", "\u091c\u0941\u0932\u0948", "\u0913\u0917\u0938\u094d\u091f", "\u0938\u0947\u092a\u094d\u091f\u0947\u0902\u092c\u0930", "\u0913\u0915\u094d\u091f\u094b\u092c\u0930", "\u0928\u094b\u0935\u094d\u0939\u0947\u0902\u092c\u0930", "\u0921\u093f\u0938\u0947\u0902\u092c\u0930", ""};
			return dfi;
		}
		set
		{
			base.DateTimeFormat = value; // not used
		}
	}

}; // class CID0057

public class CNkok : CID0057
{
	public CNkok() : base() {}

}; // class CNkok

}; // namespace I18N.Other
