/*
 * unicode.c - Routines for manipulating Unicode characters and strings.
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

#include "il_system.h"
#include "il_utils.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Include the Unicode category table from "unicat.c".
 * The table is automatically generated from "UnicodeData.txt"
 * using the "mkcategory" program.  The latest version of
 * the file can be found at the following URL:
 *
 *		ftp://ftp.unicode.org/Public/UNIDATA/UnicodeData.txt
 *
 * If the SMALL_UNICODE_TABLE macro is defined, then a smaller
 * table restricted to the Latin-1 subset is used instead of
 * the full table.  This is for memory-limited devices that
 * are likely to only be used within a Latin-1 locale.
 */
#include "unicat.c"

ILUnicodeCategory ILGetUnicodeCategory(unsigned ch)
{
#ifndef SMALL_UNICODE_TABLE
	/* The full Unicode category table is available */
	if(ch < (unsigned)0x10000)
	{
		return (ILUnicodeCategory)
					((charCategories[ch / 6] >> (5 * (ch % 6))) & 0x1F);
	}
	else
	{
		return ILUnicode_OtherNotAssigned;
	}
#else
	/* The restricted Latin-1 subset table is being used */
	if(ch < (unsigned)0x0100)
	{
		return (ILUnicodeCategory)
					((charCategories[ch / 6] >> (5 * (ch % 6))) & 0x1F);
	}

	/* Recognize some extra currency symbols, including the Euro,
	   which may be needed in some Latin-1 locales in Europe */
	if(ch >= (unsigned)0x20A0 && ch <= (unsigned)0x20AF)
	{
		return ILUnicode_CurrencySymbol;
	}

	/* Check for surrogates */
	if(ch >= (unsigned)0xD800 && ch <= (unsigned)0xDFFF)
	{
		return ILUnicode_Surrogate;
	}

	/* We have no idea what this is, so return OtherNotAssigned */
	return ILUnicode_OtherNotAssigned;
#endif
}

/*
 * Include the Unicode numeric value table from "uninum.c".
 * The table is automatically generated from "UnicodeData.txt"
 * using the "mknumber" program.
 */
#include "uninum.c"

double ILGetUnicodeValue(unsigned ch)
{
	int left, right, middle;
	left = 0;
	right = (sizeof(charValues) / sizeof(struct ILUniNumInfo)) - 1;
	while(left <= right)
	{
		middle = (left + right) / 2;
		if(charValues[middle].ch == ch)
		{
			return charValues[middle].value;
		}
		else if(charValues[middle].ch < ch)
		{
			left = middle + 1;
		}
		else
		{
			right = middle - 1;
		}
	}
	return -1.0;
}

#ifdef	__cplusplus
};
#endif
