/*
 * RoundTripFormatter.cs - Implementation of the
 *          "System.Private.NumberFormat.RoundTripFormatter" class.
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
using System.Globalization;
using System.Text;

internal class RoundTripFormatter : Formatter
{
	public RoundTripFormatter(int precision)
	{
		this.precision = 0;
	}

	public override string Format(Object o, IFormatProvider provider)
	{
#if CONFIG_EXTENDED_NUMERICS
		//  Calculate precision
		int precision;
		if (o is Single) precision = 7;
		else if (o is Double) precision = 15;
		else throw new FormatException(_("Format_TypeException"));

		//  Get initial number
		string rawnumber = FormatAnyRound(o, precision, provider);
		StringBuilder ret = new StringBuilder();

		if (rawnumber[0] == '-')
		{
			ret.Append(NumberFormatInfo(provider).NegativeSign);
			rawnumber = rawnumber.Substring(1);
		}

		//  Create portion before the decimal point
		if (rawnumber[0] == '.') ret.Append('0');
		else ret.Append(rawnumber.Substring(0,rawnumber.IndexOf('.')));

		//  Insert decimal point
		ret.Append(NumberFormatInfo(provider).NumberDecimalSeparator);

		//  Append the portion of the number following the decimal point
		int decpt = rawnumber.IndexOf('.')+1;	
		ret.Append(rawnumber.Substring(decpt,
								Math.Min(rawnumber.Length-decpt, precision)));

		return ret.ToString();
#else
		throw new FormatException(_("Format_TypeException"));
#endif
	}		
} // class RoundTripFormatter

} // namespace System.Private.NumberFormat

