/*
 * CharacterInfo.cs - Implementation of the
 *        "System.Globalization.CharacterInfo" class.
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

namespace System.Globalization
{

#if !ECMA_COMPAT

using System;
using System.Runtime.InteropServices;
using Platform;

public class CharacterInfo
{
	internal static readonly char HIGH_SURROGATE_START = '\uD800';
	internal static readonly char HIGH_SURROGATE_END   = '\uDBFF';
	internal static readonly char LOW_SURROGATE_START  = '\uDC00';
	internal static readonly char LOW_SURROGATE_END    = '\uDFFF';
	internal static readonly char[] WhitespaceChars =
		{'\t', '\n', '\v', '\f', '\r', ' ',
		 '\u00A0', '\u2000', '\u2001', '\u2002', '\u2003',
		 '\u2004', '\u2005', '\u2006', '\u2007', '\u2008',
		 '\u2009', '\u200A', '\u200B', '\u3000', '\uFEFF'};

	// Get the numeric value associated with a Unicode character.
	public static float GetNumericValue(char ch)
			{ return unchecked((float)ch); /* ??? */ }

	// Public interface to get a Unicode character's category.
	public static UnicodeCategory GetUnicodeCategory(char ch)
			{ return Platform.SysCharInfo.GetUnicodeCategory(ch); }

	// Category testing.
	public static bool IsControl(char ch)
			{
				return (GetUnicodeCategory(ch) ==
							UnicodeCategory.Control);
			}
	public static bool IsDigit(char ch)
			{
				return (GetUnicodeCategory(ch) ==
							UnicodeCategory.DecimalDigitNumber);
			}
	public static bool IsLetter(char ch)
			{
				UnicodeCategory category = GetUnicodeCategory(ch);
				return (category == UnicodeCategory.UppercaseLetter ||
						category == UnicodeCategory.LowercaseLetter ||
						category == UnicodeCategory.TitlecaseLetter ||
						category == UnicodeCategory.ModifierLetter ||
						category == UnicodeCategory.OtherLetter);
			}
	public static bool IsLower(char ch)
			{
				return (GetUnicodeCategory(ch) ==
							UnicodeCategory.LowercaseLetter);
			}
	public static bool IsMark(char ch)
			{
				UnicodeCategory category = GetUnicodeCategory(ch);
				return (category == UnicodeCategory.NonSpacingMark ||
						category == UnicodeCategory.SpacingCombiningMark ||
						category == UnicodeCategory.EnclosingMark);
			}
	public static bool IsNumber(char ch)
			{
				UnicodeCategory category = GetUnicodeCategory(ch);
				return (category == UnicodeCategory.DecimalDigitNumber ||
						category == UnicodeCategory.LetterNumber ||
						category == UnicodeCategory.OtherNumber);
			}
	public static bool IsPrivateUse(char ch)
			{
				return (GetUnicodeCategory(ch) ==
							UnicodeCategory.PrivateUse);
			}
	public static bool IsPunctuation(char ch)
			{
				UnicodeCategory category = GetUnicodeCategory(ch);
				return (category == UnicodeCategory.ConnectorPunctuation ||
						category == UnicodeCategory.DashPunctuation ||
						category == UnicodeCategory.OpenPunctuation ||
						category == UnicodeCategory.ClosePunctuation ||
						category == UnicodeCategory.InitialQuotePunctuation ||
						category == UnicodeCategory.FinalQuotePunctuation ||
						category == UnicodeCategory.OtherPunctuation);
			}
	public static bool IsSeparator(char ch)
			{
				UnicodeCategory category = GetUnicodeCategory(ch);
				return (category == UnicodeCategory.SpaceSeparator ||
						category == UnicodeCategory.LineSeparator ||
						category == UnicodeCategory.ParagraphSeparator);
			}
	public static bool IsSurrogate(char ch)
			{
				return (GetUnicodeCategory(ch) ==
							UnicodeCategory.Surrogate);
			}
	public static bool IsSymbol(char ch)
			{
				UnicodeCategory category = GetUnicodeCategory(ch);
				return (category == UnicodeCategory.MathSymbol ||
						category == UnicodeCategory.CurrencySymbol ||
						category == UnicodeCategory.ModifierSymbol ||
						category == UnicodeCategory.OtherSymbol);
			}
	public static bool IsTitleCase(char ch)
			{
				return (GetUnicodeCategory(ch) ==
							UnicodeCategory.TitlecaseLetter);
			}
	public static bool IsUpper(char ch)
			{
				return (GetUnicodeCategory(ch) ==
							UnicodeCategory.UppercaseLetter);
			}
	public static bool IsWhiteSpace(char ch)
			{
				if(GetUnicodeCategory(ch) ==
							UnicodeCategory.SpaceSeparator)
				{
					return true;
				}
				return (ch == '\t' || ch == '\n' || ch == '\v' ||
				        ch == '\f' || ch == '\r' || ch == '\u0085' ||
						ch == '\u2028' || ch == '\u2029');
			}

}; // class CharacterInfo

#endif // !ECMA_COMPAT

}; // namespace System.Globalization
