/*
 * HexadecimalFormatter.cs - Implementation of the
 *          "System.Private.NumberFormat.HexadecimalFormatter" class.
 *
 * Copyright (C) 2001  Southern Storm Software, Pty Ltd.
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

namespace System.Private.NumberFormat
{

using System;
using System.Text;

internal class HexadecimalFormatter : Formatter
{
	//  Hexadecimal digits -- upper and lower case.
	static private readonly char[] ucdigits =
	{'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
	static private readonly char[] lcdigits = 	
	{'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};

	//  Return values.
	private char[] digits;

	public HexadecimalFormatter(int precision, char format)
	{
		this.precision = (precision == -1) ? 1 : precision;
		switch (format)
		{
		case 'X':
			digits = ucdigits;
			break;
		case 'x':
			digits = lcdigits;
			break;
		default:
			throw new FormatException(_("Format_StringException"));
		}
	}

	private string Format (long value)
	{
		StringBuilder s = new StringBuilder(Format(unchecked((ulong) value)));

		if (value >= 0) return s.ToString();

		while (s[0] == digits[15]) s.Remove(0,1);

		return s.ToString().PadLeft(precision, digits[15]);
	}

	private string Format(ulong value)
	{
		ulong uvalue;
		StringBuilder buf = new StringBuilder();

		//  Format the number.
		for (uvalue = value; uvalue > 0; uvalue >>= 4)
		{
			buf.Insert(0, digits[uvalue % 16]);
		}

		return buf.ToString().PadLeft(precision, '0');
	}

	public override string Format(Object o, IFormatProvider provider)
	{
		long value;

		//  Type validation
		if (IsSignedInt(o))
		{
			return Format(OToLong(o));
		}
		else if (IsUnsignedInt(o))
		{
			return Format(OToUlong(o));
		}

		//  This is a bad place to be.
		throw new FormatException(_("Format_TypeException"));	
	}		
} // class HexadecimalFormatter

} // namespace System.Private.NumberFormat

