/*
 * NumberFormatter.cs - Implementation of the
 *          "System.Private.NumberFormatter" class.
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

namespace System.Private
{

using System;
using System.Globalization;

internal sealed class NumberFormatter
{
	// Add prefix and suffix information for a number.
	private static String FormatPrefixAndSuffix(String value, bool isneg,
											    String format,
											    NumberFormatInfo nfi)
	{
		if(format.Equals("C"))
		{
			if(isneg)
			{
				switch(nfi.CurrencyNegativePattern)
				{
					case 0:
						return "(" + nfi.CurrencySymbol + value + ")";

					case 1:
						return nfi.NegativeSign + nfi.CurrencySymbol + value;

					case 2:
						return nfi.CurrencySymbol + nfi.NegativeSign + value;

					case 3:
						return nfi.CurrencySymbol + value + nfi.NegativeSign;

					case 4:
						return "(" + value + nfi.CurrencySymbol + ")";

					case 5:
						return nfi.NegativeSign + value + nfi.CurrencySymbol;

					case 6:
						return value + nfi.NegativeSign;

					case 7:
						return value + nfi.CurrencySymbol + nfi.NegativeSign;

					case 8:
						return nfi.NegativeSign + value + " " +
							   nfi.CurrencySymbol;

					case 9:
						return nfi.NegativeSign + nfi.CurrencySymbol + " " +
							   value;

					case 10:
						return value + " " + nfi.CurrencySymbol +
							   nfi.NegativeSign;

					case 11:
						return nfi.CurrencySymbol + " " + value +
							   nfi.NegativeSign;

					case 12:
						return nfi.CurrencySymbol + " " + nfi.NegativeSign +
							   value;

					case 13:
						return value + nfi.NegativeSign + " " +
							   nfi.CurrencySymbol;

					case 14:
						return "(" + nfi.CurrencySymbol + " " + value + ")";

					case 15:
						return "(" + value + " " + nfi.CurrencySymbol + ")";
				}
			}
			else
			{
				switch(nfi.CurrencyPositivePattern)
				{
					case 0:
						return nfi.CurrencySymbol + value;

					case 1:
						return value + nfi.CurrencySymbol;

					case 2:
						return nfi.CurrencySymbol + " " + value;

					case 3:
						return value + " " + nfi.CurrencySymbol;
				}
			}
		}
		else if(format.Equals("P"))
		{
			// TO DO: these formatting conventions are a guess.
			// Microsoft's documentation (as at Feb 2001) does
			// not fully explain the values.
			if(isneg)
			{
				switch(nfi.PercentNegativePattern)
				{
					case 0:
						return "(" + value + nfi.PercentSymbol + ")";

					case 1:
						return nfi.NegativeSign + value + nfi.PercentSymbol;

					case 2:
						return value + nfi.PercentSymbol + nfi.NegativeSign;
				}
			}
			else
			{
				switch(nfi.PercentPositivePattern)
				{
					case 0:
						return value + nfi.PercentSymbol;

					case 1:
						return nfi.PercentSymbol + value;

					case 2:
						return value + " " + nfi.PercentSymbol;
				}
			}
		}
		else
		{
			if(isneg)
			{
				switch(nfi.NumberNegativePattern)
				{
					case 0:
						return "(" + value + ")";

					case 1:
						return nfi.NegativeSign + value;

					case 2:
						return nfi.NegativeSign + " " + value;

					case 3:
						return value + nfi.NegativeSign;

					case 4:
						return value + " " + nfi.NegativeSign;
				}
			}
			else
			{
				return value;
			}
		}

		// Shouldn't get here, but return something interesting anyway.
		if(isneg)
		{
			return nfi.NegativeSign + value;
		}
		else
		{
			return value;
		}
	}

	// Format a fixed-point integer value using a standard format code.
	public static String FormatFixedPoint(ulong value, ulong decimals,
									      int numDecimals, bool isneg,
									      String format,
									      NumberFormatInfo nfi)
	{
		// Sanity check the format value.
		if(format == null || format.Equals(""))
		{
			format = "G";
		}

		// Get the basic number formatting information.
		nfi = NumberFormatInfo.GetInstance(nfi);
		int decimalDigits;
		String decimalSeparator;
		int[] groupSize;
		String groupSeparator;
		if(format.Equals("C"))
		{
			// Use the currency information.
			decimalDigits = nfi.CurrencyDecimalDigits;
			decimalSeparator = nfi.CurrencyDecimalSeparator;
			groupSize = nfi.CurrencyGroupSize;
			groupSeparator = nfi.CurrencyGroupSeparator;
		}
		else if(format.Equals("P"))
		{
			// Use the percentage information.
			decimalDigits = nfi.PercentDecimalDigits;
			decimalSeparator = nfi.PercentDecimalSeparator;
			groupSize = nfi.PercentGroupSize;
			groupSeparator = nfi.PercentGroupSeparator;
		}
		else
		{
			// Use the number information.
			decimalDigits = nfi.NumberDecimalDigits;
			decimalSeparator = nfi.NumberDecimalSeparator;
			groupSize = nfi.NumberGroupSize;
			groupSeparator = nfi.NumberGroupSeparator;
		}

		// Format the basic part of the number.
		String basic;
		if(value == 0)
		{
			basic = "0";
		}
		else
		{
			basic = "";
			while(value != 0)
			{
				basic = ((char)((value % 10) + 48)) + basic;
				value /= 10;
			}
		}

		// Format the decimal part of the number.
		if(decimalDigits != 0)
		{
			basic = basic + decimalSeparator;
			if(numDecimals >= 20)
			{
				// Sanity check: too many decimals, so round it off.
				numDecimals = 0;
			}
			int temp;
			ulong powOfTen = 1;
			for(temp = 0; temp < numDecimals; ++temp)
			{
				powOfTen *= 10;
			}
			while(decimalDigits > 0 && numDecimals > 0)
			{
				decimals %= powOfTen;
				powOfTen /= 10;
				basic = basic + ((char)((decimals / powOfTen) + 48));
				--decimalDigits;
				--numDecimals;
			}
			while(decimalDigits > 0)
			{
				basic = basic + "0";
				--decimalDigits;
			}
		}

		// Add extra formatting for negatives, percent signs, currency, etc.
		return FormatPrefixAndSuffix(basic, isneg, format, nfi);
	}

	// Format an integer in base 10.
	private static String FormatBase10(ulong value, bool isneg)
	{
		String basic;
		if(value == 0)
		{
			basic = "0";
		}
		else
		{
			basic = "";
			while(value != 0)
			{
				basic = ((char)((value % 10) + (int)'0')) + basic;
				value /= 10;
			}
		}
		if(isneg)
		{
			return "-" + basic;
		}
		else
		{
			return basic;
		}
	}

	// Format a decimal value.
	public static String FormatDecimal(Decimal value, String format,
									   NumberFormatInfo nfi)
	{
		return "";
	}

	// Format a float value.
	public static String FormatSingle(float value, String format,
									  NumberFormatInfo nfi)
	{
		return "";
	}

	// Format a double value.
	public static String FormatDouble(double value, String format,
									  NumberFormatInfo nfi)
	{
		return "";
	}

	// Format a number in a specific base.
	public static String FormatInBase(long value, int toBase, int numBits)
	{
		char[] buf;
		int posn;
		int digit;
		if(toBase == 2)
		{
			buf = new char[numBits];
			posn = numBits - 1;
			while(posn >= 0)
			{
				if((value & 1) != 0)
				{
					buf[posn--] = '1';
				}
				else
				{
					buf[posn--] = '0';
				}
				value >>= 1;
			}
		}
		else if(toBase == 8)
		{
			buf = new char[(numBits + 2) / 3];
			posn = ((numBits + 2) / 3) - 1;
			while(posn >= 0)
			{
				buf[posn--] = unchecked((char)((value % 8) + (long)'0'));
				value >>= 3;
			}
		}
		else if(toBase == 10)
		{
			if(value < 0)
			{
				return FormatBase10(unchecked((ulong)(-value)), true);
			}
			else
			{
				return FormatBase10(unchecked((ulong)value), false);
			}
		}
		else if(toBase == 16)
		{
			buf = new char[numBits / 4];
			posn = (numBits / 4) - 1;
			while(posn >= 0)
			{
				digit = unchecked((int)(value % 16));
				if(digit < 10)
				{
					buf[posn--] = unchecked((char)(digit + (int)'0'));
				}
				else
				{
					buf[posn--] = unchecked((char)(digit - 10 + (int)'A'));
				}
				value >>= 4;
			}
		}
		else
		{
			throw new ArgumentException
				(Environment.GetResourceString("Arg_InvalidBase"));
		}
		return new String(buf);
	}

	// Format an unsigned number in a specific base.
	public static String FormatInBaseUnsigned(ulong value, int toBase,
											  int numBits)
	{
		if(toBase != 10)
		{
			return FormatInBase(unchecked((long)value), toBase, numBits);
		}
		else
		{
			return FormatBase10(value, false);
		}
	}

}; // class NumberFormatter

}; // namespace System.Private
