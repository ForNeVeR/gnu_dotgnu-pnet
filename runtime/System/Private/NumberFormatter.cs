/*
 * NumberFormatter.cs - Implementation of the
 *          "System.Private.NumberFormatter" class.
 *
 * Copyright (C) 2001  Southern Storm Software, Pty Ltd.
 * Copyright (C) 2002  Richard Baumann.
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
using System.Text;
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
						return value + nfi.NegativeSign + nfi.CurrencySymbol;

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
			// Fixed these... Rich ;)
			if(isneg)
			{
				switch(nfi.PercentNegativePattern)
				{
					case 0:
						return nfi.NegativeSign + value + " " + nfi.PercentSymbol;

					case 1:
						return nfi.NegativeSign + value + nfi.PercentSymbol;

					case 2:
						return nfi.NegativeSign + nfi.PercentSymbol + value;
				}
			}
			else
			{
				switch(nfi.PercentPositivePattern)
				{
					case 0:
						return value + " " + nfi.PercentSymbol;

					case 1:
						return value + nfi.PercentSymbol;

					case 2:
						return nfi.PercentSymbol + value;
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
			groupSize = nfi.CurrencyGroupSizes;
			groupSeparator = nfi.CurrencyGroupSeparator;
		}
		else if(format.Equals("P"))
		{
			// Use the percentage information.
			decimalDigits = nfi.PercentDecimalDigits;
			decimalSeparator = nfi.PercentDecimalSeparator;
			groupSize = nfi.PercentGroupSizes;
			groupSeparator = nfi.PercentGroupSeparator;
		}
		else
		{
			// Use the number information.
			decimalDigits = nfi.NumberDecimalDigits;
			decimalSeparator = nfi.NumberDecimalSeparator;
			groupSize = nfi.NumberGroupSizes;
			groupSeparator = nfi.NumberGroupSeparator;
		}

		// FIXME: turn off the decimal digits because it is
		// giving the wrong results for integers.
		decimalDigits = 0;

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

	private static String stdTypes = "CcDdEeFfGgNnPpRrXxZz";
	private static bool IsStandardType(char type)
	{
		return (stdTypes.IndexOf(type) != -1);
	}
	private static bool ParseFormat(String format, out int precision, out char type)
	{
		precision = -1;
		if (format == null)
		{
			type = 'G';
			return true;
		}
		if (format.Length == 0)
		{
			type = '\0'; // have to assign something
			return false;
		}
		char[] chars = format.ToCharArray();
		type = chars[0];
		if (IsStandardType(type))
		{
			if (chars.Length == 1)
			{
				return true;
			}
			if (chars.Length == 2 && (chars[1] >= '0' && chars[1] <= '9'))
			{
				precision = chars[1] - '0';
				return true;
			}
			if (chars.Length == 3 && (chars[1] >= '0' && chars[1] <= '9') && (chars[2] >= '0' && chars[2] <= '9'))
			{
				precision = ((chars[1] - '0') * 10) + (chars[2] - '0');
				return true;
			}
			return false;
		}
		return false;
	}

	internal static void InsertGroupSeparators(StringBuilder src, String sep, int[] groups) { InsertGroupSeparators(src,sep,groups,-1); }
	internal static void InsertGroupSeparators(StringBuilder src, String sep, int[] groups, int maxgroups)
	{
		if (maxgroups < 0)
		{
			int i = 0;
			int j = src.Length - groups[0]; // position of sep insertion, and bounds check var
			int last = groups[groups.Length-1];
			while (i < groups.Length && groups[i] != 0 && j > 0)
			{
				src.Insert(j,sep);
				j -= groups[i];
				i++;
				if (i == groups.Length && last != 0)
				{
					i--; // if the last group needs to be repeated, stay on it until j <= 0
				}
			}
		}
		else
		{
			int i = 0;
			int j = src.Length - groups[0]; // position of sep insertion, and bounds check var
			int last = groups[groups.Length-1];
			for (int k = 0; k < maxgroups && i < groups.Length && groups[i] != 0 && j > 0; k++)
			{
				src.Insert(j,sep);
				j -= groups[i];
				i++;
				if (i == groups.Length && last != 0)
				{
					i--; // if the last group needs to be repeated, stay on it until j <= 0
				}
			}
		}
	}

	private static String CurrencyFormat(FormatNumber num, int precision, NumberFormatInfo info)
	{
		if (precision < 0) { precision = info.CurrencyDecimalDigits; }
		if (num.Fraction.Length > precision) { num.Round(precision,false,false); }
		StringBuilder n = new StringBuilder(num.Whole);
		InsertGroupSeparators(n,info.CurrencyGroupSeparator,info.CurrencyGroupSizes);
		if (precision > 0)
		{
			n.Append(info.CurrencyDecimalSeparator);
			n.Append(num.Fraction);
		}
		return FormatPrefixAndSuffix(n.ToString(),(num.Sign < 0),"C",info);
	}
	private static String DecimalFormat(FormatNumber num, int precision, NumberFormatInfo info)
	{
		StringBuilder n = new StringBuilder(num.Whole);
		while (precision > n.Length) { n.Insert(0,"0"); }
		if (num.Sign < 0) { n.Insert(0,info.NegativeSign); }
		return n.ToString();
	}
	private static String ExponentialFormat(FormatNumber num, int precision, NumberFormatInfo info, bool useLowerCase)
	{
		int exp;
		num.SciForm(ref exp,false);
		StringBuilder e = new StringBuilder(new FormatNumber(exp).Whole);
		while (e.Length < 3) { e.Insert(0,"0"); }
		if (exp < 0) { e.Insert(0,info.NegativeSign); }
		else { e.Insert(0,info.PositiveSign); }
		e.Insert(0,(useLowerCase ? "e" : "E"));

		if (precision < 0) { precision = 6; }
		if (num.Fraction.Length > precision) { num.Round(precision,false,false); }
		StringBuilder n = new StringBuilder(num.Whole);
		if (num.Sign < 0) { n.Insert(0,info.NegativeSign); }
		if (precision > 0)
		{
			n.Append(info.NumberDecimalSeparator);
			n.Append(num.Fraction);
		}
		n.Append(e.ToString());
		return n.ToString();
	}
	private static String FixedPointFormat(FormatNumber num, int precision, NumberFormatInfo info)
	{
		if (precision < 0) { precision = info.NumberDecimalDigits; }
		if (num.Fraction.Length > precision) { num.Round(precision,false,false); }
		StringBuilder n = new StringBuilder(num.Whole);
		if (num.Sign < 0) { n.Insert(0,info.NegativeSign); }
		if (precision > 0)
		{
			n.Append(info.NumberDecimalSeparator);
			n.Append(num.Fraction);
		}
		return n.ToString();
	}
	private static String NumberFormat(FormatNumber num, int precision, NumberFormatInfo info)
	{
		// NOTE: The ECMA likes to be unclear. They say this must have at least one non-zero
		// digit before the decimal separator but does that mean it should throw a
		// FormatException(), scale the number (what about a zero?), or something else? A
		// standard is useless if it lacks clarity (we really need a GNU standards body, so
		// we can ignore the vague and incompetent). I'm ignoring their suggestion here, at
		// least for now; besides it being unclear, it seems, to me, to be rather stupid.
		// - Rich
		if (precision < 0) { precision = info.NumberDecimalDigits; }
		if (num.Fraction.Length > precision) { num.Round(precision,false,false); }
		StringBuilder n = new StringBuilder(num.Whole);
		InsertGroupSeparators(n,info.NumberGroupSeparator,info.NumberGroupSizes);
		if (precision > 0)
		{
			n.Append(info.NumberDecimalSeparator);
			n.Append(num.Fraction);
		}
		return FormatPrefixAndSuffix(n.ToString(),(num.Sign < 0),"N",info);
	}
	private static String PercentFormat(FormatNumber num, int precision, NumberFormatInfo info)
	{
		if (precision < 0) { precision = info.PercentDecimalDigits; }
		if (num.Fraction.Length > precision) { num.Round(precision,false,false); }
		StringBuilder n = new StringBuilder(num.Whole);
		InsertGroupSeparators(n,info.PercentGroupSeparator,info.PercentGroupSizes);
		if (precision > 0)
		{
			n.Append(info.PercentDecimalSeparator);
			n.Append(num.Fraction);
		}
		return FormatPrefixAndSuffix(n.ToString(),(num.Sign < 0),"P",info);
	}
	private static String RoundTripFormat(FormatNumber num, NumberFormatInfo info)
	{
		StringBuilder n = new StringBuilder(num.Whole);
		if (num.Sign < 0) { n.Insert(0,info.NegativeSign); }
		n.Append(info.NumberDecimalSeparator);
		n.Append(num.Fraction);
		return n.ToString();
	}
	private static String NormalizedFormat(FormatNumber num, NumberFormatInfo info)
	{
		// NOTE: The ECMA strikes again. Considering this language was designed to be
		// used over the inet, you would think they would actually spend more than ten
		// minutes to finish the i18n spec.; apparently that's asking too much.
		// - Rich
		StringBuilder n = new StringBuilder(num.Whole);
		if (num.Sign < 0) { n.Insert(0,info.NegativeSign); }
		StringBuilder f = new StringBuilder(num.Fraction);
		int i = f.Length - 1;
		while (i >= 0 && f[i] == '0') { i--; }
		i++; // index of first trailing zero
		if (i < f.Length) { f.Remove(i,f.Length - i); }
		if (f.Length > 0)
		{
			n.Append(info.NumberDecimalSeparator);
			n.Append(f.ToString());
		}
		return n.ToString();
	}

	public static String FormatByte(byte value, String format,
	                                NumberFormatInfo nfi)
	{
		int precision;
		char type;
		bool custom = !ParseFormat(format,out precision,out type);
		if (custom)
		{
			return (new CustomNumberFormatter(new FormatNumber(value),format,nfi)).Format;
		}
		else if (type == 'C' || type == 'c')
		{
			return CurrencyFormat(new FormatNumber(value),precision,nfi);
		}
		else if (type == 'D' || type == 'd')
		{
			return DecimalFormat(new FormatNumber(value),precision,nfi);
		}
		else if (type == 'E' || type == 'e')
		{
			return ExponentialFormat(new FormatNumber(value),precision,nfi,type == 'e');
		}
		else if (type == 'F' || type == 'f')
		{
			return FixedPointFormat(new FormatNumber(value),precision,nfi);
		}
		else if (type == 'G' || type == 'g')
		{
			FormatNumber num = new FormatNumber(value);
			if (precision < 0) { precision = 3; }
			int exp;
			num.SciForm(ref exp,true);
			if (exp < precision && exp > -5) { return FixedPointFormat(num,precision,nfi); }
			else { return ExponentialFormat(num,precision,nfi,type == 'g'); }
		}
		else if (type == 'N' || type == 'n')
		{
			return NumberFormat(new FormatNumber(value),precision,nfi);
		}
		else if (type == 'P' || type == 'p')
		{
			return PercentFormat(new FormatNumber(value),precision,nfi);
		}
		else if (type == 'X' || type == 'x')
		{
			return FormatInBaseUnsigned(unchecked((ulong)value),16,8,type == 'x');
		}
		else
		{
			throw new FormatException(); // TODO: add custom msg
		}
	}

	public static String FormatShort(short value, String format,
	                                 NumberFormatInfo nfi)
	{
		int precision;
		char type;
		bool custom = !ParseFormat(format,out precision,out type);
		if (custom)
		{
			return (new CustomNumberFormatter(new FormatNumber(value),format,nfi)).Format;
		}
		else if (type == 'C' || type == 'c')
		{
			return CurrencyFormat(new FormatNumber(value),precision,nfi);
		}
		else if (type == 'D' || type == 'd')
		{
			return DecimalFormat(new FormatNumber(value),precision,nfi);
		}
		else if (type == 'E' || type == 'e')
		{
			return ExponentialFormat(new FormatNumber(value),precision,nfi,type == 'e');
		}
		else if (type == 'F' || type == 'f')
		{
			return FixedPointFormat(new FormatNumber(value),precision,nfi);
		}
		else if (type == 'G' || type == 'g')
		{
			FormatNumber num = new FormatNumber(value);
			if (precision < 0) { precision = 5; }
			int exp;
			num.SciForm(ref exp,true);
			if (exp < precision && exp > -5) { return FixedPointFormat(num,precision,nfi); }
			else { return ExponentialFormat(num,precision,nfi,type == 'g'); }
		}
		else if (type == 'N' || type == 'n')
		{
			return NumberFormat(new FormatNumber(value),precision,nfi);
		}
		else if (type == 'P' || type == 'p')
		{
			return PercentFormat(new FormatNumber(value),precision,nfi);
		}
		else if (type == 'X' || type == 'x')
		{
			return FormatInBase(unchecked((long)value),16,16,type == 'x');
		}
		else
		{
			throw new FormatException(); // TODO: add custom msg
		}
	}

	public static String FormatInt(int value, String format,
	                               NumberFormatInfo nfi)
	{
		int precision;
		char type;
		bool custom = !ParseFormat(format,out precision,out type);
		if (custom)
		{
			return (new CustomNumberFormatter(new FormatNumber(value),format,nfi)).Format;
		}
		else if (type == 'C' || type == 'c')
		{
			return CurrencyFormat(new FormatNumber(value),precision,nfi);
		}
		else if (type == 'D' || type == 'd')
		{
			return DecimalFormat(new FormatNumber(value),precision,nfi);
		}
		else if (type == 'E' || type == 'e')
		{
			return ExponentialFormat(new FormatNumber(value),precision,nfi,type == 'e');
		}
		else if (type == 'F' || type == 'f')
		{
			return FixedPointFormat(new FormatNumber(value),precision,nfi);
		}
		else if (type == 'G' || type == 'g')
		{
			FormatNumber num = new FormatNumber(value);
			if (precision < 0) { precision = 10; }
			int exp;
			num.SciForm(ref exp,true);
			if (exp < precision && exp > -5) { return FixedPointFormat(num,precision,nfi); }
			else { return ExponentialFormat(num,precision,nfi,type == 'g'); }
		}
		else if (type == 'N' || type == 'n')
		{
			return NumberFormat(new FormatNumber(value),precision,nfi);
		}
		else if (type == 'P' || type == 'p')
		{
			return PercentFormat(new FormatNumber(value),precision,nfi);
		}
		else if (type == 'X' || type == 'x')
		{
			return FormatInBase(unchecked((long)value),16,32,type == 'x');
		}
		else
		{
			throw new FormatException(); // TODO: add custom msg
		}
	}

	public static String FormatLong(long value, String format,
	                                NumberFormatInfo nfi)
	{
		int precision;
		char type;
		bool custom = !ParseFormat(format,out precision,out type);
		if (custom)
		{
			return (new CustomNumberFormatter(new FormatNumber(value),format,nfi)).Format;
		}
		else if (type == 'C' || type == 'c')
		{
			return CurrencyFormat(new FormatNumber(value),precision,nfi);
		}
		else if (type == 'D' || type == 'd')
		{
			return DecimalFormat(new FormatNumber(value),precision,nfi);
		}
		else if (type == 'E' || type == 'e')
		{
			return ExponentialFormat(new FormatNumber(value),precision,nfi,type == 'e');
		}
		else if (type == 'F' || type == 'f')
		{
			return FixedPointFormat(new FormatNumber(value),precision,nfi);
		}
		else if (type == 'G' || type == 'g')
		{
			FormatNumber num = new FormatNumber(value);
			if (precision < 0) { precision = 19; }
			int exp;
			num.SciForm(ref exp,true);
			if (exp < precision && exp > -5) { return FixedPointFormat(num,precision,nfi); }
			else { return ExponentialFormat(num,precision,nfi,type == 'g'); }
		}
		else if (type == 'N' || type == 'n')
		{
			return NumberFormat(new FormatNumber(value),precision,nfi);
		}
		else if (type == 'P' || type == 'p')
		{
			return PercentFormat(new FormatNumber(value),precision,nfi);
		}
		else if (type == 'X' || type == 'x')
		{
			return FormatInBase(unchecked((long)value),16,64,type == 'x');
		}
		else
		{
			throw new FormatException(); // TODO: add custom msg
		}
	}

	public static String FormatSByte(sbyte value, String format,
	                                 NumberFormatInfo nfi)
	{
		int precision;
		char type;
		bool custom = !ParseFormat(format,out precision,out type);
		if (custom)
		{
			return (new CustomNumberFormatter(new FormatNumber(value),format,nfi)).Format;
		}
		else if (type == 'C' || type == 'c')
		{
			return CurrencyFormat(new FormatNumber(value),precision,nfi);
		}
		else if (type == 'D' || type == 'd')
		{
			return DecimalFormat(new FormatNumber(value),precision,nfi);
		}
		else if (type == 'E' || type == 'e')
		{
			return ExponentialFormat(new FormatNumber(value),precision,nfi,type == 'e');
		}
		else if (type == 'F' || type == 'f')
		{
			return FixedPointFormat(new FormatNumber(value),precision,nfi);
		}
		else if (type == 'G' || type == 'g')
		{
			FormatNumber num = new FormatNumber(value);
			if (precision < 0) { precision = 3; }
			int exp;
			num.SciForm(ref exp,true);
			if (exp < precision && exp > -5) { return FixedPointFormat(num,precision,nfi); }
			else { return ExponentialFormat(num,precision,nfi,type == 'g'); }
		}
		else if (type == 'N' || type == 'n')
		{
			return NumberFormat(new FormatNumber(value),precision,nfi);
		}
		else if (type == 'P' || type == 'p')
		{
			return PercentFormat(new FormatNumber(value),precision,nfi);
		}
		else if (type == 'X' || type == 'x')
		{
			return FormatInBase(unchecked((long)value),16,8,type == 'x');
		}
		else
		{
			throw new FormatException(); // TODO: add custom msg
		}
	}

	public static String FormatUShort(ushort value, String format,
	                                  NumberFormatInfo nfi)
	{
		int precision;
		char type;
		bool custom = !ParseFormat(format,out precision,out type);
		if (custom)
		{
			return (new CustomNumberFormatter(new FormatNumber(value),format,nfi)).Format;
		}
		else if (type == 'C' || type == 'c')
		{
			return CurrencyFormat(new FormatNumber(value),precision,nfi);
		}
		else if (type == 'D' || type == 'd')
		{
			return DecimalFormat(new FormatNumber(value),precision,nfi);
		}
		else if (type == 'E' || type == 'e')
		{
			return ExponentialFormat(new FormatNumber(value),precision,nfi,type == 'e');
		}
		else if (type == 'F' || type == 'f')
		{
			return FixedPointFormat(new FormatNumber(value),precision,nfi);
		}
		else if (type == 'G' || type == 'g')
		{
			FormatNumber num = new FormatNumber(value);
			if (precision < 0) { precision = 5; }
			int exp;
			num.SciForm(ref exp,true);
			if (exp < precision && exp > -5) { return FixedPointFormat(num,precision,nfi); }
			else { return ExponentialFormat(num,precision,nfi,type == 'g'); }
		}
		else if (type == 'N' || type == 'n')
		{
			return NumberFormat(new FormatNumber(value),precision,nfi);
		}
		else if (type == 'P' || type == 'p')
		{
			return PercentFormat(new FormatNumber(value),precision,nfi);
		}
		else if (type == 'X' || type == 'x')
		{
			return FormatInBaseUnsigned(unchecked((ulong)value),16,16,type == 'x');
		}
		else
		{
			throw new FormatException(); // TODO: add custom msg
		}
	}

	public static String FormatUInt(uint value, String format,
	                                NumberFormatInfo nfi)
	{
		int precision;
		char type;
		bool custom = !ParseFormat(format,out precision,out type);
		if (custom)
		{
			return (new CustomNumberFormatter(new FormatNumber(value),format,nfi)).Format;
		}
		else if (type == 'C' || type == 'c')
		{
			return CurrencyFormat(new FormatNumber(value),precision,nfi);
		}
		else if (type == 'D' || type == 'd')
		{
			return DecimalFormat(new FormatNumber(value),precision,nfi);
		}
		else if (type == 'E' || type == 'e')
		{
			return ExponentialFormat(new FormatNumber(value),precision,nfi,type == 'e');
		}
		else if (type == 'F' || type == 'f')
		{
			return FixedPointFormat(new FormatNumber(value),precision,nfi);
		}
		else if (type == 'G' || type == 'g')
		{
			FormatNumber num = new FormatNumber(value);
			if (precision < 0) { precision = 10; }
			int exp;
			num.SciForm(ref exp,true);
			if (exp < precision && exp > -5) { return FixedPointFormat(num,precision,nfi); }
			else { return ExponentialFormat(num,precision,nfi,type == 'g'); }
		}
		else if (type == 'N' || type == 'n')
		{
			return NumberFormat(new FormatNumber(value),precision,nfi);
		}
		else if (type == 'P' || type == 'p')
		{
			return PercentFormat(new FormatNumber(value),precision,nfi);
		}
		else if (type == 'X' || type == 'x')
		{
			return FormatInBaseUnsigned(unchecked((ulong)value),16,32,type == 'x');
		}
		else
		{
			throw new FormatException(); // TODO: add custom msg
		}
	}

	public static String FormatULong(ulong value, String format,
	                                 NumberFormatInfo nfi)
	{
		int precision;
		char type;
		bool custom = !ParseFormat(format,out precision,out type);
		if (custom)
		{
			return (new CustomNumberFormatter(new FormatNumber(value),format,nfi)).Format;
		}
		else if (type == 'C' || type == 'c')
		{
			return CurrencyFormat(new FormatNumber(value),precision,nfi);
		}
		else if (type == 'D' || type == 'd')
		{
			return DecimalFormat(new FormatNumber(value),precision,nfi);
		}
		else if (type == 'E' || type == 'e')
		{
			return ExponentialFormat(new FormatNumber(value),precision,nfi,type == 'e');
		}
		else if (type == 'F' || type == 'f')
		{
			return FixedPointFormat(new FormatNumber(value),precision,nfi);
		}
		else if (type == 'G' || type == 'g')
		{
			FormatNumber num = new FormatNumber(value);
			if (precision < 0) { precision = 19; }
			int exp;
			num.SciForm(ref exp,true);
			if (exp < precision && exp > -5) { return FixedPointFormat(num,precision,nfi); }
			else { return ExponentialFormat(num,precision,nfi,type == 'g'); }
		}
		else if (type == 'N' || type == 'n')
		{
			return NumberFormat(new FormatNumber(value),precision,nfi);
		}
		else if (type == 'P' || type == 'p')
		{
			return PercentFormat(new FormatNumber(value),precision,nfi);
		}
		else if (type == 'X' || type == 'x')
		{
			return FormatInBaseUnsigned(unchecked((ulong)value),16,64,type == 'x');
		}
		else
		{
			throw new FormatException(); // TODO: add custom msg
		}
	}

	// Format a decimal value.
	public static String FormatDecimal(Decimal value, String format,
									   NumberFormatInfo nfi)
	{
		int precision;
		char type;
		bool custom = !ParseFormat(format,out precision,out type);
		if (custom)
		{
			return (new CustomNumberFormatter(new FormatNumber(value),format,nfi)).Format;
		}
		else if (type == 'C' || type == 'c')
		{
			return CurrencyFormat(new FormatNumber(value),precision,nfi);
		}
		else if (type == 'E' || type == 'e')
		{
			return ExponentialFormat(new FormatNumber(value),precision,nfi,type == 'e');
		}
		else if (type == 'F' || type == 'f')
		{
			return FixedPointFormat(new FormatNumber(value),precision,nfi);
		}
		else if (type == 'G' || type == 'g')
		{
			FormatNumber num = new FormatNumber(value);
			if (precision < 0) { precision = 29; }
			int exp;
			num.SciForm(ref exp,true);
			if (exp < precision && exp > -5) { return FixedPointFormat(num,precision,nfi); }
			else { return ExponentialFormat(num,precision,nfi,type == 'g'); }
		}
		else if (type == 'N' || type == 'n')
		{
			return NumberFormat(new FormatNumber(value),precision,nfi);
		}
		else if (type == 'P' || type == 'p')
		{
			return PercentFormat(new FormatNumber(value),precision,nfi);
		}
		else if (type == 'Z' || type == 'z')
		{
			return NormalizedFormat(new FormatNumber(value),nfi);
		}
		else
		{
			throw new FormatException(); // TODO: add custom msg
		}
	}

	// Format a float value.
	public static String FormatSingle(float value, String format,
									  NumberFormatInfo nfi)
	{
		int precision;
		char type;
		bool custom = !ParseFormat(format,out precision,out type);
		if (custom)
		{
			return (new CustomNumberFormatter(new FormatNumber(value),format,nfi)).Format;
		}
		else if (type == 'C' || type == 'c')
		{
			return CurrencyFormat(new FormatNumber(value),precision,nfi);
		}
		else if (type == 'E' || type == 'e')
		{
			return ExponentialFormat(new FormatNumber(value),precision,nfi,type == 'e');
		}
		else if (type == 'F' || type == 'f')
		{
			return FixedPointFormat(new FormatNumber(value),precision,nfi);
		}
		else if (type == 'G' || type == 'g')
		{
			FormatNumber num = new FormatNumber(value);
			if (precision < 0) { precision = 7; }
			int exp;
			num.SciForm(ref exp,true);
			if (exp < precision && exp > -5) { return FixedPointFormat(num,precision,nfi); }
			else { return ExponentialFormat(num,precision,nfi,type == 'g'); }
		}
		else if (type == 'N' || type == 'n')
		{
			return NumberFormat(new FormatNumber(value),precision,nfi);
		}
		else if (type == 'P' || type == 'p')
		{
			return PercentFormat(new FormatNumber(value),precision,nfi);
		}
		else if (type == 'R' || type == 'r')
		{
			return RoundTripFormat(new FormatNumber(value,true),nfi);
		}
		else
		{
			throw new FormatException(); // TODO: add custom msg
		}
	}

	// Format a double value.
	public static String FormatDouble(double value, String format,
									  NumberFormatInfo nfi)
	{
		int precision;
		char type;
		bool custom = !ParseFormat(format,out precision,out type);
		if (custom)
		{
			return (new CustomNumberFormatter(new FormatNumber(value),format,nfi)).Format;
		}
		else if (type == 'C' || type == 'c')
		{
			return CurrencyFormat(new FormatNumber(value),precision,nfi);
		}
		else if (type == 'E' || type == 'e')
		{
			return ExponentialFormat(new FormatNumber(value),precision,nfi,type == 'e');
		}
		else if (type == 'F' || type == 'f')
		{
			return FixedPointFormat(new FormatNumber(value),precision,nfi);
		}
		else if (type == 'G' || type == 'g')
		{
			FormatNumber num = new FormatNumber(value);
			if (precision < 0) { precision = 15; }
			int exp;
			num.SciForm(ref exp,true);
			if (exp < precision && exp > -5) { return FixedPointFormat(num,precision,nfi); }
			else { return ExponentialFormat(num,precision,nfi,type == 'g'); }
		}
		else if (type == 'N' || type == 'n')
		{
			return NumberFormat(new FormatNumber(value),precision,nfi);
		}
		else if (type == 'P' || type == 'p')
		{
			return PercentFormat(new FormatNumber(value),precision,nfi);
		}
		else if (type == 'R' || type == 'r')
		{
			return RoundTripFormat(new FormatNumber(value,true),nfi);
		}
		else
		{
			throw new FormatException(); // TODO: add custom msg
		}
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
			throw new ArgumentException(_("Arg_InvalidBase"));
		}
		return new String(buf);
	}
	public static String FormatInBase(long value, int toBase, int numBits,
	                                  bool useLowerCase)
	{
		if (!useLowerCase || toBase != 16)
		{
			return FormatInBase(value,toBase,numBits);
		}

		char[] buf;
		int posn;
		int digit;
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
				buf[posn--] = unchecked((char)(digit - 10 + (int)'a'));
			}
			value >>= 4;
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
	public static String FormatInBaseUnsigned(ulong value, int toBase,
	                                          int numBits, bool useLowerCase)
	{
		if(toBase != 10)
		{
			return FormatInBase(unchecked((long)value), toBase, numBits, useLowerCase);
		}
		else
		{
			return FormatBase10(value, false);
		}
	}

}; // class NumberFormatter

}; // namespace System.Private
