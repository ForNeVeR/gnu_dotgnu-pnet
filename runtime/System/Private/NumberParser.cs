/*
 * NumberParser.cs - Implementation of "System.Private.NumberParser".
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

internal sealed class NumberParser
{
	// Parse a number in a specific radix.
	private static bool StringToNumber(String value, int radix,
									   out ulong result, out bool sign)
	{
		int len;
		int posn;
		char ch;
		uint digit, low, high;
		ulong tempa, tempb;
		bool noOverflow = true;

		// Bail out if the argument is null.
		if(value == null)
		{
			throw new ArgumentNullException("value");
		}

		// Parse the sign, if present.
		len = value.Length;
		posn = 0;
		if(posn < len)
		{
			ch = value[posn];
			if(ch == '-')
			{
				sign = true;
				++posn;
			}
			else if(ch == '+')
			{
				sign = false;
				++posn;
			}
			else
			{
				sign = false;
			}
		}
		else
		{
			sign = false;
		}
		if(posn >= len)
		{
			throw new FormatException(_("Format_Integer"));
		}

		// Parse the main part of the number.
		low = 0;
		high = 0;
		do
		{
			// Get the next digit from the string.
			ch = value[posn];
			if(ch >= '0' && ch <= '9')
			{
				if(radix < 10 && ch >= ('0' + radix))
				{
					break;
				}
				digit = (uint)(ch - '0');
			}
			else if(ch >= 'A' && ch <= 'Z')
			{
				if(radix <= 10 || ch >= ('A' + (radix - 10)))
				{
					break;
				}
				digit = (uint)(ch - 'A' + 10);
			}
			else if(ch >= 'a' && ch <= 'z')
			{
				if(radix <= 10 || ch >= ('a' + (radix - 10)))
				{
					break;
				}
				digit = (uint)(ch - 'a' + 10);
			}
			else
			{
				break;
			}

			// Combine the digit with the result, and check for overflow.
			if(noOverflow)
			{
				tempa = ((ulong)low) * ((ulong)radix);
				tempb = ((ulong)high) * ((ulong)radix);
				tempb += (tempa >> 32);
				if(tempb > ((ulong)0xFFFFFFFF))
				{
					// Overflow has occurred.
					noOverflow = false;
				}
				else
				{
					tempa = (tempa & 0xFFFFFFFF) + ((ulong)digit);
					tempb += (tempa >> 32);
					if(tempb > ((ulong)0xFFFFFFFF))
					{
						// Overflow has occurred.
						noOverflow = false;
					}
					else
					{
						low = unchecked((uint)tempa);
						high = unchecked((uint)tempb);
					}
				}
			}

			// Advance to the next character.
			++posn;
		}
		while(posn < len);

		// If we are not at the end of the string, then report an error.
		if(posn < len)
		{
			throw new FormatException(_("Format_Integer"));
		}

		// Done.
		result = (((ulong)high) << 32) | ((ulong)low);
		return noOverflow;
	}

	public static int StringToInt32(String value, int radix, int ovfValue)
	{
		ulong result;
		bool sign;

		// Parse the number.
		if(StringToNumber(value, radix, out result, out sign))
		{
			if(!sign)
			{
				if(result <= 2147483647)
				{
					return unchecked((int)result);
				}
			}
			else
			{
				if(result <= 2147483648)
				{
					return unchecked((int)-((long)result));
				}
			}
		}

		// If we get here, then an overflow has occurred.
		if(ovfValue != 0)
		{
			// Let the caller figure out how to report the error.
			return ovfValue;
		}
		else
		{
			// Parsing Int32 itself, so report the error now.
			throw new OverflowException(_("Overflow_Int32"));
		}
	}

	public static uint StringToUInt32(String value, int radix, uint ovfValue)
	{
		ulong result;
		bool sign;

		// Parse the number.
		if(StringToNumber(value, radix, out result, out sign))
		{
			if(!sign)
			{
				if(result <= 4294967295)
				{
					return unchecked((uint)result);
				}
			}
		}

		// If we get here, then an overflow has occurred.
		if(ovfValue != 0)
		{
			// Let the caller figure out how to report the error.
			return ovfValue;
		}
		else
		{
			// Parsing UInt32 itself, so report the error now.
			throw new OverflowException(_("Overflow_UInt32"));
		}
	}

	public static long StringToInt64(String value, int radix)
	{
		ulong result;
		bool sign;

		// Parse the number.
		if(StringToNumber(value, radix, out result, out sign))
		{
			if(!sign)
			{
				if(result <= 9223372036854775807)
				{
					return unchecked((long)result);
				}
			}
			else
			{
				if(result <= 9223372036854775808)
				{
					return unchecked(-((long)result));
				}
			}
		}

		// If we get here, then an overflow has occurred.
		throw new OverflowException(_("Overflow_Int64"));
	}

	public static ulong StringToUInt64(String value, int radix)
	{
		ulong result;
		bool sign;

		// Parse the number.
		if(StringToNumber(value, radix, out result, out sign))
		{
			if(!sign)
			{
				return result;
			}
		}

		// If we get here, then an overflow has occurred.
		throw new OverflowException(_("Overflow_UInt64"));
	}

	// Validate a parse style for integer parses.
	public static void ValidateIntegerStyle(NumberStyles style)
	{
		if((style & NumberStyles.AllowHexSpecifier) != 0)
		{
			if((style & ~(NumberStyles.AllowHexSpecifier |
						  NumberStyles.AllowLeadingWhite |
						  NumberStyles.AllowTrailingWhite)) != 0)
			{
				throw new ArgumentException(_("Arg_InvalidHexStyle"));
			}
		}
	}

	// Parse integer values using localized number format information.
	private static bool ParseNumber(String s, NumberStyles style,
									NumberFormatInfo nfi,
									out ulong result, out bool sign)
	{
		int posn, len;
		char ch;
		uint digit, low, high;
		ulong tempa, tempb;
		bool noOverflow = true;

		// Validate the parameters.
		if(s == null)
		{
			throw new ArgumentNullException("s");
		}

		// Get the number format information to use.
		nfi = NumberFormatInfo.GetInstance(nfi);
		String posSign = nfi.PositiveSign;
		String negSign = nfi.NegativeSign;
		String currency = nfi.CurrencySymbol;
		String thousandsSep1 = nfi.CurrencyGroupSeparator;
		String thousandsSep2 = nfi.NumberGroupSeparator;

		// Skip leading white space.
		posn = 0;
		len = s.Length;
		if(posn < len)
		{
			do
			{
				ch = s[posn];
				if(!Char.IsWhiteSpace(ch))
				{
					break;
				}
				if((style & NumberStyles.AllowLeadingWhite) == 0)
				{
					throw new FormatException(_("Format_Integer"));
				}
				++posn;
			}
			while(posn < len);
		}

		// Check for leading currency and sign information.
		sign = false;
		while(posn < len)
		{
			ch = s[posn];
			if((style & NumberStyles.AllowCurrencySymbol) != 0 &&
			   ch == currency[0])
			{
				++posn;
			}
			else if((style & NumberStyles.AllowParentheses) != 0 && ch == '(')
			{
				sign = true;
				++posn;
			}
			else if((style & NumberStyles.AllowLeadingSign) != 0 &&
			        ch == negSign[0])
			{
				sign = true;
				++posn;
			}
			else if((style & NumberStyles.AllowLeadingSign) != 0 &&
					ch == posSign[0])
			{
				sign = false;
				++posn;
			}
			else if((style & NumberStyles.AllowThousands) != 0 &&
					(ch == thousandsSep1[0] ||
					 ch == thousandsSep2[0]))
			{
				++posn;
			}
			else if(Char.IsWhiteSpace(ch))
			{
				++posn;
			}
			else if(ch >= '0' && ch <= '9')
			{
				break;
			}
			else
			{
				throw new FormatException(_("Format_Integer"));
			}
		}

		// Bail out if the string is empty.
		if(posn >= len)
		{
			throw new FormatException(_("Format_Integer"));
		}

		// Parse the main part of the number.
		low = 0;
		high = 0;
		if((style & NumberStyles.AllowHexSpecifier) != 0 && (posn + 2) <= len &&
		   s[posn] == '0' &&
		   (s[posn + 1] == 'x' || s[posn + 1] == 'X'))
		{
			// Parse a hexadecimal value.
			posn += 2;
			if(posn >= len)
			{
				throw new FormatException(_("Format_Integer"));
			}
			do
			{
				// Get the next digit from the string.
				ch = s[posn];
				if(ch >= '0' && ch <= '9')
				{
					digit = (uint)(ch - '0');
				}
				else if(ch >= 'A' && ch <= 'F')
				{
					digit = (uint)(ch - 'A' + 10);
				}
				else if(ch >= 'a' && ch <= 'f')
				{
					digit = (uint)(ch - 'a' + 10);
				}
				else if((style & NumberStyles.AllowThousands) != 0 &&
						(ch == thousandsSep1[0] ||
						 ch == thousandsSep2[0]))
				{
					// Ignore thousands separators in the string.
					++posn;
					continue;
				}
				else
				{
					break;
				}
	
				// Combine the digit with the result, and check for overflow.
				if(noOverflow)
				{
					tempa = ((ulong)low) * ((ulong)16);
					tempb = ((ulong)high) * ((ulong)16);
					tempb += (tempa >> 32);
					if(tempb > ((ulong)0xFFFFFFFF))
					{
						// Overflow has occurred.
						noOverflow = false;
					}
					else
					{
						tempa = (tempa & 0xFFFFFFFF) + ((ulong)digit);
						tempb += (tempa >> 32);
						if(tempb > ((ulong)0xFFFFFFFF))
						{
							// Overflow has occurred.
							noOverflow = false;
						}
						else
						{
							low = unchecked((uint)tempa);
							high = unchecked((uint)tempb);
						}
					}
				}
	
				// Advance to the next character.
				++posn;
			}
			while(posn < len);
		}
		else
		{
			// Parse a decimal value.
			do
			{
				// Get the next digit from the string.
				ch = s[posn];
				if(ch >= '0' && ch <= '9')
				{
					digit = (uint)(ch - '0');
				}
				else if((style & NumberStyles.AllowThousands) != 0 &&
						(ch == thousandsSep1[0] ||
						 ch == thousandsSep2[0]))
				{
					// Ignore thousands separators in the string.
					++posn;
					continue;
				}
				else
				{
					break;
				}
	
				// Combine the digit with the result, and check for overflow.
				if(noOverflow)
				{
					tempa = ((ulong)low) * ((ulong)10);
					tempb = ((ulong)high) * ((ulong)10);
					tempb += (tempa >> 32);
					if(tempb > ((ulong)0xFFFFFFFF))
					{
						// Overflow has occurred.
						noOverflow = false;
					}
					else
					{
						tempa = (tempa & 0xFFFFFFFF) + ((ulong)digit);
						tempb += (tempa >> 32);
						if(tempb > ((ulong)0xFFFFFFFF))
						{
							// Overflow has occurred.
							noOverflow = false;
						}
						else
						{
							low = unchecked((uint)tempa);
							high = unchecked((uint)tempb);
						}
					}
				}
	
				// Advance to the next character.
				++posn;
			}
			while(posn < len);
		}

		// Process trailing sign information and white space.
		if(posn < len)
		{
			bool lastWasWhite = false;
			do
			{
				ch = s[posn];
				if((style & NumberStyles.AllowParentheses) != 0 &&
				   ch == ')')
				{
					lastWasWhite = false;
					++posn;
				}
				else if((style & NumberStyles.AllowCurrencySymbol) != 0 &&
				        ch == currency[0])
				{
					lastWasWhite = false;
					++posn;
				}
				else if((style & NumberStyles.AllowTrailingSign) != 0 &&
				        ch == negSign[0])
				{
					sign = true;
					lastWasWhite = false;
					++posn;
				}
				else if((style & NumberStyles.AllowTrailingSign) != 0 &&
				        ch == posSign[0])
				{
					sign = false;
					lastWasWhite = false;
					++posn;
				}
				else if(Char.IsWhiteSpace(ch))
				{
					lastWasWhite = true;
					++posn;
				}
				else
				{
					break;
				}
			}
			while(posn < len);
			if(posn < len ||
			   (lastWasWhite && (style & NumberStyles.AllowTrailingWhite) == 0))
			{
				throw new FormatException(_("Format_Integer"));
			}
		}

		// Return the results to the caller.
		result = (((ulong)high) << 32) | ((ulong)low);
		return noOverflow;
	}

	public static int ParseInt32(String s, NumberStyles style,
								 NumberFormatInfo nfi,
								 int ovfValue)
	{
		ulong result;
		bool sign;

		// Parse the number.
		if(ParseNumber(s, style, nfi, out result, out sign))
		{
			if(!sign)
			{
				if(result <= 2147483647)
				{
					return unchecked((int)result);
				}
			}
			else
			{
				if(result <= 2147483648)
				{
					return unchecked((int)-((long)result));
				}
			}
		}

		// If we get here, then an overflow has occurred.
		if(ovfValue != 0)
		{
			// Let the caller figure out how to report the error.
			return ovfValue;
		}
		else
		{
			// Parsing Int32 itself, so report the error now.
			throw new OverflowException(_("Overflow_Int32"));
		}
	}

	public static uint ParseUInt32(String s, NumberStyles style,
								   NumberFormatInfo nfi,
								   uint ovfValue)
	{
		ulong result;
		bool sign;

		// Parse the number.
		if(ParseNumber(s, style, nfi, out result, out sign))
		{
			if(!sign)
			{
				if(result <= 4294967295)
				{
					return unchecked((uint)result);
				}
			}
		}

		// If we get here, then an overflow has occurred.
		if(ovfValue != 0)
		{
			// Let the caller figure out how to report the error.
			return ovfValue;
		}
		else
		{
			// Parsing UInt32 itself, so report the error now.
			throw new OverflowException(_("Overflow_UInt32"));
		}
	}

	public static long ParseInt64(String s, NumberStyles style,
								  NumberFormatInfo nfi)
	{
		ulong result;
		bool sign;

		// Parse the number.
		if(ParseNumber(s, style, nfi, out result, out sign))
		{
			if(!sign)
			{
				if(result <= 9223372036854775807)
				{
					return unchecked((long)result);
				}
			}
			else
			{
				if(result <= 9223372036854775808)
				{
					return unchecked(-((long)result));
				}
			}
		}

		// If we get here, then an overflow has occurred.
		throw new OverflowException(_("Overflow_Int64"));
	}

	public static ulong ParseUInt64(String s, NumberStyles style,
								    NumberFormatInfo nfi,
								    ulong ovfValue)
	{
		ulong result;
		bool sign;

		// Parse the number.
		if(ParseNumber(s, style, nfi, out result, out sign))
		{
			if(!sign)
			{
				return result;
			}
		}

		// If we get here, then an overflow has occurred.
		throw new OverflowException(_("Overflow_UInt64"));
	}

	public static Decimal ParseDecimal(String s, NumberStyles style,
								       NumberFormatInfo nfi)
	{
		return 0.0m;
	}

	public static float ParseSingle(String s, NumberStyles style,
								    NumberFormatInfo nfi)
	{
		return 0.0f;
	}

	public static double ParseDouble(String s, NumberStyles style,
								     NumberFormatInfo nfi)
	{
		return 0.0d;
	}

}; // class NumberParser

}; // namespace System.Private
