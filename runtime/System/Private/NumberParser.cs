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
using System.Text;

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

	private static string RawHandleWhite(String s,
							NumberStyles style, NumberFormatInfo nfi)
	{
		String ret;
		if ((style & NumberStyles.AllowLeadingWhite) != 0 &&
			(style & NumberStyles.AllowTrailingWhite) != 0)
		{
			ret = s.Trim();
		}
		else if ((style & NumberStyles.AllowLeadingWhite) != 0)
		{
			ret = s.TrimStart(null);
			if (ret.Length > s.TrimEnd(null).Length)
				throw new FormatException(_("Format_UnallowedTrailingWhite"));
		}
		else if ((style & NumberStyles.AllowTrailingWhite) != 0)
		{
			ret = s.TrimEnd(null);
			if (ret.Length > s.TrimStart(null).Length)
				throw new FormatException(_("Format_UnallowedLeadingWhite"));
		}
		else
		{
			ret = s;
			if (ret.Length > s.TrimStart(null).Length)
				throw new FormatException(_("Format_UnallowedLeadingWhite"));
			if (ret.Length > s.TrimEnd(null).Length)
				throw new FormatException(_("Format_UnallowedTrailingWhite"));
		}
		return ret;
	}

	private static void RawHandleSign(StringBuilder sb,
							NumberStyles style, NumberFormatInfo nfi)
	{
		bool leadingsign = false, trailingsign = false;

		if (sb.ToString().StartsWith(nfi.PositiveSign) ||
				sb.ToString().StartsWith(nfi.NegativeSign))
		{
			leadingsign = true;
			if ((style & NumberStyles.AllowLeadingSign) == 0)
				throw new FormatException(_("Format_UnallowedLeadingSign"));
		}
	
		if (sb.ToString().EndsWith(nfi.PositiveSign) ||
				sb.ToString().EndsWith(nfi.NegativeSign))
		{
			trailingsign = true;
			if ((style & NumberStyles.AllowTrailingSign) == 0)
				throw new FormatException(_("Format_UnallowedTrailingSign"));
		}

		if (leadingsign && trailingsign)
		{
			throw new FormatException(_("Format_TwoSigns"));
		}
	}

	private static void RawHandleCurrencySymbol(StringBuilder sb,
							NumberStyles style, NumberFormatInfo nfi)
	{
		int symbolPos = sb.ToString().IndexOf(nfi.CurrencySymbol);

		//  Nothing to be done
		if (symbolPos == -1) return;

		if ((style & NumberStyles.AllowCurrencySymbol) != 0)
		{
			//  Remove the currency symbol
			sb.Remove(symbolPos,nfi.CurrencySymbol.Length);
		}
		else
		{
			//  Throw a format exception
			throw new FormatException(_("Format_UnallowedCurrencySymbol"));
		}
	}

	private static void RawHandleDecimalPoint(StringBuilder sb,
								NumberStyles style, NumberFormatInfo nfi)
	{
		//  There are three different possible decimal points,
		//  of which only one should be present.

		//  Change all possible decimal points into one.
		sb.Replace(nfi.CurrencyDecimalSeparator, nfi.NumberDecimalSeparator);
		sb.Replace(nfi.PercentDecimalSeparator, nfi.NumberDecimalSeparator);

		int decimalPos = sb.ToString().IndexOf(nfi.NumberDecimalSeparator);

		if ((style & NumberStyles.AllowDecimalPoint) != 0)
		{
			//  Test for multiple decimal points
			if (decimalPos 
				!= sb.ToString().LastIndexOf(nfi.NumberDecimalSeparator))
			{
				throw
					new FormatException(_("Format_MultipleDecimalSeparators"));
			}
		}
		else if (decimalPos > -1)
		{
			throw new FormatException(_("Format_UnallowedDecimalPoint"));
		}
	}

	private static readonly char [] Ee = {'e', 'E'};

	private static void RawHandleExponent(StringBuilder sb,
							NumberStyles style, NumberFormatInfo nfi)
	{
		string s = sb.ToString();
		int ePos = s.IndexOfAny(Ee);
		int decimalPos = s.IndexOf(nfi.NumberDecimalSeparator);
		int i;

		//  Bail out if this doesn't look like an exponential number
		if (ePos != s.LastIndexOfAny(Ee)) return;
		if (decimalPos >= ePos) return;

		//  Exponent is a leading sign plus at least one digit
		if (ePos+1 >= s.Length) return;
		if (s[ePos+1] != '+' && s[ePos+1] != '-') return;

		if (ePos + 3 >= s.Length) return;  // Have to have at least one digit
		for (i = ePos + 2; i < s.Length; i++)
		{
			if (s[i] < '0' || s[i] > '9') return;
		}

		for (i = ePos - 1; 
				i >= decimalPos + nfi.NumberDecimalSeparator.Length; 
				i--)
		{
			if (s[i] < '0' || s[i] > '9') return;
		}

		for (i = decimalPos-1;
				i <= '-' && i >= 0;
				i--)
		{
			if (s[i] < '0' || s[i] > '9') return;
		}

		if (i >= nfi.NegativeSign.Length) 
		{
			return;
		}
		else if (i >= 0 && !s.StartsWith(nfi.NegativeSign)) 
		{
			return;
		}

		//  Okay, this is an exponent.  Is it allowable?
		if ((style & NumberStyles.AllowExponent) == 0)
		{
			throw new FormatException(_("Format_UnallowedExponent"));
		}
	}

	private static void RawHandleParentheses(StringBuilder sb,
							NumberStyles style, NumberFormatInfo nfi)
	{
		int startPos = sb.ToString().IndexOf('(');
		int endPos = sb.ToString().IndexOf(')');

		//  Move along... Nothing to see here...
		if (startPos == -1 && endPos == -1) return;

		if (startPos == -1 || endPos == -1 || startPos > endPos)
			throw new FormatException(_("Format_UnbalancedParentheses"));

		if ((style & NumberStyles.AllowParentheses) != 0)
		{
			//  Replace parentheses with a leading NumberNegativeSign
			sb.Remove(startPos,1);
			sb.Remove(endPos,1);
			sb.Insert(0, nfi.NegativeSign);

			//  Test for additional parentheses... Bad!
			if (sb.ToString().IndexOf('(') != -1 ||
					sb.ToString().IndexOf(')') != -1)
			{
				throw new FormatException(_("Format_ExtraParentheses"));
			}
		}
		else
		{
			throw new FormatException(_("Format_UnallowedParentheses"));
		}
	}

	private static bool RawHandleThousandsTry(StringBuilder sb, 
			NumberFormatInfo nfi, string sep, int [] sizes)
	{
		int i, idx;

		i = 0;
		idx = sb.ToString().IndexOf(nfi.NumberDecimalSeparator);
		idx -= sep.Length;
		idx -= sizes[i];

		if (idx < 0) return false;

		while (idx > 0)
		{
			if (sb.ToString(idx, sep.Length) != sep) return false;

			sb.Remove(idx, sep.Length);

			if (i < sizes.Length - 1) i++;
			idx -= sizes[i];
		}
	
		return true;
	}

	private static string RawHandleThousands(StringBuilder sb, 
			NumberStyles style, NumberFormatInfo nfi)
	{
		//  Once again, the specification has three possible sets of
		//  group separators that could be used here.  This routine 
		//  tries each in turn until one works, then decides what to
		//  do about it.
		
		bool ok;
		StringBuilder hold;

		hold = new StringBuilder(sb.ToString());
		ok = RawHandleThousandsTry(hold, nfi,
				nfi.NumberGroupSeparator, nfi.NumberGroupSizes);

		if (!ok)
		{
			hold = new StringBuilder(sb.ToString());
			ok = RawHandleThousandsTry(hold, nfi,
					nfi.CurrencyGroupSeparator, nfi.CurrencyGroupSizes);
		}

		if (!ok)
		{
			hold = new StringBuilder(sb.ToString());
			ok = RawHandleThousandsTry(hold, nfi,
					nfi.PercentGroupSeparator, nfi.PercentGroupSizes);
		}

		if (!ok) return sb.ToString();

		//  At this point, we know we CAN separate the groups.  Now the
		//  question is, "Should we?"
		if ((style & NumberStyles.AllowThousands) != 0)
		{
			return hold.ToString();
		}
		else
		{
			throw new FormatException("Format_UnallowedThousands");
		}
	}

	private static string RawString(String s, NumberStyles style,
									NumberFormatInfo nfi)
	{
		//  Bail out and let the parser cope -- poor parser...
		if ((style & NumberStyles.AllowHexSpecifier) != 0) return s;

		//  These routines hold a significant amount of subjective
		//  format validation.  They also cleanse out information
		//  unneeded by the parser itself (e.g. whitespace, group).
		StringBuilder sb = 
			new StringBuilder(RawHandleWhite(s, style, nfi));
		RawHandleSign(sb, style, nfi);
		RawHandleCurrencySymbol(sb, style, nfi);
		RawHandleDecimalPoint(sb, style, nfi);
		RawHandleExponent(sb, style, nfi);
		RawHandleParentheses(sb, style, nfi);

		//  Leave this for last, because it needs a fairly
		//  well-formatted input
		return RawHandleThousands(sb, style, nfi);
	}

	private static bool StripSign(StringBuilder sb, NumberFormatInfo nfi)
	{
		bool isNeg;

		if (sb.ToString().StartsWith(nfi.NegativeSign))
		{
			isNeg = true;
			sb.Remove(0, nfi.NegativeSign.Length);
		}
		else if (sb.ToString().EndsWith(nfi.NegativeSign))
		{
			isNeg = true;
			sb.Remove(sb.Length = nfi.NegativeSign.length, 
					nfi.NegativeSign.length);
		}
		else if (sb.ToString().StartsWith(nfi.PositiveSign))
		{
			isNeg = false;
			sb.Remove(0, nfi.PositiveSign.Length);
		}
		else if (sb.ToString().EndsWith(nfi.PositiveSign))
		{
			isNeg = false;
			sb.Remove(sb.Length = nfi.PositiveSign.length, 
					nfi.PositiveSign.length);
		}
		else
		{
			isNeg = false;
		}
		return isNeg;
	}

	private static uint EatDigits(StringBuilder sb)
	{
		uint work = 0;

		while (sb.Length > 0 && sb[0] >= '0' && sb[0] <= '9')
		{
			work *= 10;
			work += (uint)(sb[0] - '0');
			sb.Remove(0, 1);
		}
		return work;
	}

	public static Decimal ParseDecimal(String s, NumberStyles style,
								       NumberFormatInfo nfi)
	{
		//  Decimal does not parse floating point numbers
		if ((style & NumberStyles.AllowHexSpecifier) != 0)
			throw new FormatException(_("Format_HexNotSupported"));

		StringBuilder sb = new StringBuilder(RawString(s, style, nfi));
		bool negative = StripSign(sb, nfi);
		decimal work = 0.0m;

		string str = sb.ToString();
		int stridx = 0;

		//  Parse up to the decimal
		while (stridx < str.Length 
				&& str[stridx] >= '0' && str[stridx] <= '9')
		{
			work = 10 * work + (str[stridx++] - '0');
		}

		//  Parse after the decimal
		if (stridx < str.Length && 
				str.Substring(stridx).StartsWith(nfi.NumberDecimalSeparator))
		{
			stridx += nfi.NumberDecimalSeparator.Length;

			decimal temp;
			int i, j;
			for (i = -1; 
					stridx < str.Length && 
					str[stridx] >= '0' && str[stridx] <= '9' ; 
					i--)
			{
				temp = (decimal)((uint)(str[stridx++] - '0'));
				for (j = 0; j > i; j--) temp *= 0.1m;
				work += temp;
			}	
		}

		//  Parse after the exponent
		if (stridx < str.Length 
				&& (str[stridx] == 'e' || str[stridx] == 'E') ) 
		{
			uint exp = 0;
			bool negExponent = false;
			decimal mult;

			stridx++;

			if (str.Substring(stridx).StartsWith(nfi.PositiveSign)) 
			{
				mult = 10.0m;
				stridx++;
			} 
			else if (str.Substring(stridx).StartsWith(nfi.NegativeSign)) 
			{
				mult = 0.1m;
				stridx++;
			} 
			else
			{
				mult = 10.0m;
			}

		    while (stridx < str.Length 
					&& str[stridx] >= '0' && str[stridx] <= '9')
			{
				exp = 10 * exp + (uint)(str[stridx++] - '0');
			}

			for (int i=0; i<exp; i++) work *= mult;
		}

		if (stridx < str.Length)
		{
			//  Oops.  Throw a "junk found" exception.
			throw new FormatException();
		}

		if (negative) work *= -1;

		return work;
	}

	public static float ParseSingle(String s, NumberStyles style,
								    NumberFormatInfo nfi)
	{
		// We'll let cast-checking handle overflow exceptions
		return (float)ParseDouble(s, style, nfi);
	}

	//  Array of powers of ten -- keeps precision within reason
	private static double [] powten = {
		1.0e0, 1.0e1, 1.0e2, 1.0e3, 1.0e4, 1.0e5,
		1.0e6, 1.0e7, 1.0e8, 1.0e9, 1.0e10,
		1.0e11, 1.0e12, 1.0e13, 1.0e14, 1.0e15,
		1.0e16, 1.0e17, 1.0e18, 1.0e19, 1.0e20 };

	//  Build a number 10^exponent using the above array.  This
	//  seems more precise than Math.Pow for two reasons:
	//  1.  The mantissa is a known constant.
	//  2.  The exponents are integral.	
	private static double Pow10(int exponent)
	{
		double ret = 1.0d;
		int exp = exponent;
	
		if (exp < 0)
		{
			for (; exp < -20; exp += 20) ret /= powten[20];
			ret /= powten[exp*-1];
		}
		else
		{
			for (; exp > 20; exp -=20) ret *= powten[20];
			ret *= powten[exp];
		}

		return ret;
	}
		
	public static double ParseDouble(String s, NumberStyles style,
								     NumberFormatInfo nfi)
	{
		//  Double does not parse hex numbers
		if ((style & NumberStyles.AllowHexSpecifier) != 0)
			throw new FormatException(_("Format_HexNotSupported"));

		//  Strip out the silliness
		StringBuilder sb = new StringBuilder(RawString(s, style, nfi));
		bool negative = StripSign(sb, nfi);

		//  Figure out what our exponent was, then ditch the decimal point
		int exponent = sb.ToString().IndexOf('.')-1;

		if (exponent == -2)   // Not Found
		{
			exponent = Math.Max(sb.ToString().IndexOf('e'), 
								sb.ToString().IndexOf('E')) - 1;
			if (exponent == -2)   // Not Found
				exponent = sb.Length - 1;
		}
		else sb.Remove(exponent+1,1);

		//  Remove leading zeroes that might gum up the works
		while (sb.Length > 0 && sb[0] == '0') {
			sb.Remove(0,1);
			exponent--;
		}

		//  Guard case...
		if (sb.Length == 0) return 0.0;
		string str = sb.ToString();

		//  Parse up to the exponent
		ulong ulwork = 0;
		int i;

		//  This loop tops out at 18 -- the most digits in a ulong
		for (i = 0; 
				i < str.Length && i < 18 && str[i] >= '0' && str[i] <= '9'; 
				i++) 
		{
			ulwork = (ulwork * 10) + unchecked((uint)(str[i]-'0'));
		}
		double work = (double)ulwork * Pow10(exponent - i + 1);

		//  Bleed off the over-precise numbers
		while (i < str.Length && str[i] >= '0' && str[i] <= '9') i++;

		//  Parse after the exponent
		if (i < str.Length && (str[i] == 'e' || str[i] == 'E') ) 
		{
			exponent = 0;
			bool negExponent = false;
			i++;

			if (str.Substring(i).StartsWith(nfi.PositiveSign)) 
			{
				i++;
			} 
			else if (str.Substring(i).StartsWith(nfi.NegativeSign)) 
			{
				negExponent = true;
				i++;
			} 

		    while (i < str.Length && str[i] >= '0' && str[i] <= '9')
				exponent = (10*exponent) + (int)(str[i++] - '0');

			if (negExponent) exponent *= -1;
			work *= Pow10(exponent);
		}

		if (i < str.Length)
		{
			//  Oops.  Throw a "junk found" exception.
			throw new FormatException();
		}

		if (negative) work *= -1.0d;

		return work;
	}
}; // class NumberParser

}; // namespace System.Private
