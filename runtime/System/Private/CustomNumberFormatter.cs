/*
 * CustomNumberFormatter.cs - Implementation of "System.Private.CustomNumberFormatter".
 *
 * Copyright (C) 2002 Richard Baumann.
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

using System;
using System.Text;
using System.Globalization;

namespace System.Private
{

internal sealed class CustomNumberFormatter
{
	private CustomNumberFormat[] sections = null;
	private FormatNumber num = null;
	private NumberFormatInfo info = null;
	private String format = null;
	private String orig;

	internal FormatNumber Number
	{
		get { return new FormatNumber(num); }
		set { num = new FormatNumber(value); }
	}
	internal NumberFormatInfo Info
	{
		get { return ((NumberFormatInfo) info.Clone()); }
		set
		{
			if (value == null)
			{
				info = new NumberFormatInfo();
			}
			else
			{
				info = ((NumberFormatInfo) value.Clone());
			}
			format = null;
		}
	}
	internal String Orig
	{
		get { return orig; }
		set
		{
			if (value == null) { orig = ""; }
			else { orig = value; }
			format = null;
			String[] sects = SplitSections(orig);
			sections = new CustomNumberFormat[sects.Length];
			for (int i = 0; i < sects.Length; i++)
			{
				sections[i] = new CustomNumberFormat(sects[i],info);
			}
		}
	}
	internal String Format
	{
		get
		{
			if (format != null) { return format; }
			format = GetFormat(num,sections,info);
			return format;
		}
	}



	internal CustomNumberFormatter(FormatNumber num, String format, NumberFormatInfo nfi) : base()
	{
		Number = num;
		Info = nfi;
		Orig = format;
	}

	private static String[] SplitSections(String format)
	{
		int i,j;
		char[] chars = format.ToCharArray();
		bool[] quotes = new bool[2] { false, false }; // 0 = apos, 1 = quot
		StringBuilder[] tmp = new StringBuilder[4] { null, null, null, null };
		tmp[0] = new StringBuilder();

		for (i = 0, j = 0; i < chars.Length; i++)
		{
			if (chars[i] == ';' && !quotes[0] && !quotes[1])
			{
				if (j == 3)
				{
					if (i+1 != chars.Length)
					{
						throw new FormatException();
					}
				}
				else
				{
					j++;
					tmp[j] = new StringBuilder();
				}
			}
			else
			{
				if (chars[i] == '\'' && !quotes[1])
				{
					quotes[0] = !quotes[0];
				}
				else if (chars[i] == '"' && !quotes[0])
				{
					quotes[1] = !quotes[1];
				}
				tmp[j].Append(chars[i]);
			}
		}

		String[] s = new String[j];
		for (j = 0; j < s.Length; j++)
		{
			s[j] = tmp[j].ToString();
		}
		return s;
	}

	private static int FindSection(FormatNumber num, CustomNumberFormat[] sections)
	{
		if (num.Sign == 0 && sections.Length == 3) { return 2; }
		else if (num.Sign < 0 && sections.Length > 1 && sections[1].Format != "") { return 1; }
		else { return 0; }
	}

	private static String GetFormat(FormatNumber num, CustomNumberFormat[] sections, NumberFormatInfo info) { return GetFormat(num,sections,info,true); }
	private static String GetFormat(FormatNumber num, CustomNumberFormat[] sections, NumberFormatInfo info, bool copy)
	{
		int section = FindSection(num,sections);
		CustomNumberFormat sec = sections[section];
		FormatNumber n = num.Scale(sec.Scale,copy);

		if (sec.Engineering == null)
		{
			if (sec.AfterDotMax < n.Fraction.Length)
			{
				n = n.Round(sec.AfterDotMax,false,false);
			}

			if (n.Sign != num.Sign)
			{
				return GetFormat(n,sections,info,false);
			}

			StringBuilder w = new StringBuilder(n.Whole);
			StringBuilder f = new StringBuilder(n.Fraction);
			while (sec.BeforeDotMin > w.Length)
			{
				w.Insert(0,"0");
			}
			while (sec.AfterDotMin > f.Length)
			{
				f.Append("0");
			}

			NumberFormatter.InsertGroupSeparators(w,info.NumberGroupSeparator,info.NumberGroupSizes,sec.Groups);

			StringBuilder s = new StringBuilder();
			sec.Info = info;
			s.Append(sec.BeforeNum);
			if (section == 0)
			{
				s.Append(info.NegativeSign);
			}
			s.Append(w.ToString());
			if (sec.HasDot)
			{
				s.Append(info.NumberDecimalSeparator);
				s.Append(f.ToString());
			}
			s.Append(sec.AfterNum);
			return s.ToString();
		}
		else if (sec.BeforeDotMin > 1 || sec.BeforeDotMax > 1)
		{
			throw new FormatException(); // TODO: add custom msg
		}
		else
		{
			// return engineering_format();
			int exp;
			n = n.SciForm(out exp,false);

			if (sec.AfterDotMax < n.Fraction.Length)
			{
				n = n.Round(sec.AfterDotMax,false,false);
			}

			if (n.Sign != num.Sign)
			{
				return GetFormat(n,sections,info,false);
			}

			StringBuilder f = new StringBuilder(n.Fraction);
			while (sec.AfterDotMin > f.Length)
			{
				f.Append("0");
			}

			char esymbol = sec.Engineering[0];
			bool etype = false; // only neg signs
			int zcount = sec.Engineering.Length - 1;
			if (sec.Engineering[1] != '0')
			{
				zcount--;
				if (sec.Engineering[1] == '+')
				{
					etype = true; // neg and pos signs
				}
			}

			StringBuilder e = new StringBuilder();
			for (int i = exp; i != 0; i /= 10)
			{
				e.Insert(0,((char)((i % 10) + (int)'0')));
			}
			while (e.Length < zcount)
			{
				e.Insert(0,"0");
			}
			if (exp < 0)
			{
				e.Insert(0,info.NegativeSign);
			}
			else if (etype)
			{
				e.Insert(0,info.PositiveSign);
			}
			e.Insert(0,esymbol);

			StringBuilder s = new StringBuilder();
			sec.Info = info;
			s.Append(sec.BeforeNum);
			if (section == 0)
			{
				s.Append(info.NegativeSign);
			}
			s.Append(n.Whole);
			if (sec.HasDot)
			{
				s.Append(info.NumberDecimalSeparator);
				s.Append(f.ToString());
			}
			s.Append(e.ToString());
			s.Append(sec.AfterNum);
			return s.ToString();
		}
	}

}; // class CustomNumberFormat

}; // namespace System.Private
