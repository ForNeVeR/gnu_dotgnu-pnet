/*
 * CID0049.cs - ta culture handler.
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

// Generated from "ta.txt".

namespace I18N.Other
{

using System;
using System.Globalization;
using I18N.Common;

public class CID0049 : RootCulture
{
	private CultureName cultureName;

	public CID0049()
		: base(0x0049, CultureNameTable.GetNameInfoByID(0x0049)) {}
	public CID0049(int culture, CultureName cultureName)
		: base(culture, cultureName) {}

	public override DateTimeFormatInfo DateTimeFormat
	{
		get
		{
			DateTimeFormatInfo dfi = base.DateTimeFormat;
			dfi.AMDesignator = "\u0b95\u0bbe\u0bb2\u0bc8";
			dfi.PMDesignator = "\u0bae\u0bbe\u0bb2\u0bc8";
			dfi.AbbreviatedDayNames = new String[] {"\u0b9e\u0bbe", "\u0ba4\u0bbf", "\u0b9a\u0bc6", "\u0baa\u0bc1", "\u0bb5\u0bbf", "\u0bb5\u0bc6", "\u0b9a"};
			dfi.DayNames = new String[] {"\u0b9e\u0bbe\u0baf\u0bbf\u0bb1\u0bc1", "\u0ba4\u0bbf\u0b99\u0bcd\u0b95\u0bb3\u0bcd", "\u0b9a\u0bc6\u0bb5\u0bcd\u0bb5\u0bbe\u0baf\u0bcd", "\u0baa\u0bc1\u0ba4\u0ba9\u0bcd", "\u0bb5\u0bbf\u0baf\u0bbe\u0bb4\u0ba9\u0bcd", "\u0bb5\u0bc6\u0bb3\u0bcd\u0bb3\u0bbf", "\u0b9a\u0ba9\u0bbf"};
			dfi.AbbreviatedMonthNames = new String[] {"\u0b9c\u0ba9.", "\u0baa\u0bc6\u0baa\u0bcd.", "\u0bae\u0bbe\u0bb0\u0bcd.", "\u0b8f\u0baa\u0bcd.", "\u0bae\u0bc7", "\u0b9c\u0bc2\u0ba9\u0bcd", "\u0b9c\u0bc2\u0bb2\u0bc8", "\u0b86\u0b95.", "\u0b9a\u0bc6\u0baa\u0bcd.", "\u0b85\u0b95\u0bcd.", "\u0ba8\u0bb5.", "\u0b9f\u0bbf\u0b9a.", ""};
			dfi.MonthNames = new String[] {"\u0b9c\u0ba9\u0bb5\u0bb0\u0bbf", "\u0baa\u0bc6\u0baa\u0bcd\u0bb0\u0bb5\u0bb0\u0bbf", "\u0bae\u0bbe\u0bb0\u0bcd\u0b9a\u0bcd", "\u0b8f\u0baa\u0bcd\u0bb0\u0bb2\u0bcd", "\u0bae\u0bc7", "\u0b9c\u0bc2\u0ba9\u0bcd", "\u0b9c\u0bc2\u0bb2\u0bc8", "\u0b86\u0b95\u0bb8\u0bcd\u0b9f\u0bcd", "\u0b9a\u0bc6\u0baa\u0bcd\u0b9f\u0bae\u0bcd\u0baa\u0bb0\u0bcd", "\u0b85\u0b95\u0bcd\u0b9f\u0bcb\u0baa\u0bb0\u0bcd", "\u0ba8\u0bb5\u0bae\u0bcd\u0baa\u0bb0\u0bcd", "\u0b9f\u0bbf\u0b9a\u0bae\u0bcd\u0baa\u0bb0\u0bcd", ""};
			return dfi;
		}
		set
		{
			base.DateTimeFormat = value; // not used
		}
	}

}; // class CID0049

public class CNta : CID0049
{
	public CNta() : base() {}

}; // class CNta

}; // namespace I18N.Other
