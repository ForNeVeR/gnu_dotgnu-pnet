/*
 * utf8.c - Utility functions for managing UTF-8 sequences.
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

#include "il_utils.h"

#ifdef	__cplusplus
extern	"C" {
#endif

unsigned long ILUTF8ReadChar(const char *str, int len, int *posn)
{
	char ch = str[*posn];
	unsigned long result;
	if((ch & 0x80) == 0)
	{
		/* Single-byte UTF-8 encoding */
		++(*posn);
		return (unsigned long)ch;
	}
	else if((ch & (char)0xE0) == (char)0xC0 && (*posn + 2) <= len)
	{
		/* Two-byte UTF-8 encoding */
		result = ((((unsigned long)(ch & 0x1F)) << 6) |
		           ((unsigned long)(str[(*posn) + 1] & 0x3F)));
		(*posn) += 2;
		return result;
	}
	else if((ch & (char)0xF0) == (char)0xE0 && (*posn + 3) <= len)
	{
		/* Three-byte UTF-8 encoding */
		result = ((((unsigned long)(ch & 0x0F)) << 12) |
		          (((unsigned long)(str[(*posn) + 1] & 0x3F)) << 6) |
		           ((unsigned long)(str[(*posn) + 2] & 0x3F)));
		(*posn) += 3;
		return result;
	}
	else if((ch & (char)0xF8) == (char)0xF0 && (*posn + 4) <= len)
	{
		/* Four-byte UTF-8 encoding */
		result = ((((unsigned long)(ch & 0x07)) << 18) |
		          (((unsigned long)(str[(*posn) + 1] & 0x3F)) << 12) |
		          (((unsigned long)(str[(*posn) + 2] & 0x3F)) << 6) |
		           ((unsigned long)(str[(*posn) + 3] & 0x3F)));
		(*posn) += 4;
		return result;
	}
	else if((ch & (char)0xFC) == (char)0xF8 && (*posn + 5) <= len)
	{
		/* Five-byte UTF-8 encoding */
		result = ((((unsigned long)(ch & 0x03)) << 24) |
		          (((unsigned long)(str[(*posn) + 1] & 0x3F)) << 18) |
		          (((unsigned long)(str[(*posn) + 2] & 0x3F)) << 12) |
		          (((unsigned long)(str[(*posn) + 3] & 0x3F)) << 6) |
		           ((unsigned long)(str[(*posn) + 4] & 0x3F)));
		(*posn) += 5;
		return result;
	}
	else if((ch & (char)0xFC) == (char)0xFC && (*posn + 6) <= len)
	{
		/* Six-byte UTF-8 encoding */
		result = ((((unsigned long)(ch & 0x03)) << 30) |
		          (((unsigned long)(str[(*posn) + 1] & 0x3F)) << 24) |
		          (((unsigned long)(str[(*posn) + 2] & 0x3F)) << 18) |
		          (((unsigned long)(str[(*posn) + 3] & 0x3F)) << 12) |
		          (((unsigned long)(str[(*posn) + 4] & 0x3F)) << 6) |
		           ((unsigned long)(str[(*posn) + 5] & 0x3F)));
		(*posn) += 6;
		return result;
	}
	else
	{
		/* Invalid UTF-8 encoding: treat as an 8-bit Latin-1 character */
		++(*posn);
		return (((unsigned long)ch) & 0xFF);
	}
}

#ifdef	__cplusplus
};
#endif
