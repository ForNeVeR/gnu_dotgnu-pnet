/*
 * lib_encoding.c - Internalcall methods for "System.Text.DefaultEncoding".
 *
 * Copyright (C) 2002  Southern Storm Software, Pty Ltd.
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

#include "engine.h"
#include "lib_defs.h"
#include "il_utils.h"
#if HAVE_WCHAR_H
#include <wchar.h>
#endif
#if HAVE_STDLIB_H
#include <stdlib.h>
#endif

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * See if we have sufficient functions to do locale-based conversion.
 * The "IL_USE_LATIN1" macro is defined to set the encoding to Latin1
 * if we have no idea how to convert to and from the system encoding.
 */
#if !(defined(HAVE_WCTOMB) || defined(HAVE_WCRTOMB)) || \
    !(defined(HAVE_MBTOWC) || defined(HAVE_MBRTOWC))
#define	IL_USE_LATIN1	1
#endif

/*
 * Get the byte count for an array of characters.
 */
static ILInt32 GetByteCount(ILUInt16 *chars, ILInt32 count)
{
#ifdef IL_USE_LATIN1
	return count;
#else
#if HAVE_WCRTOMB
	/* Use the re-entrant function to perform the conversion */
	mbstate_t state;
	char buf[MB_CUR_MAX+1];
	size_t chlen;
	ILUInt16 ch;
	ILInt32 len = 0;
	ILMemZero(&state, sizeof(state));
	wcrtomb(buf, 0, &state);
	while(count > 0)
	{
		ch = *chars++;
		if(ch != 0)
		{
			chlen = wcrtomb(buf, (wchar_t)ch, &state);
			if(chlen != (size_t)(-1))
			{
				len += (ILInt32)chlen;
			}
		}
		else
		{
			++len;
		}
		--count;
	}
	return len;
#else
	/* Use the non re-entrant function to perform the conversion
	   and just hope that the underlying libc takes care of the
	   thread-safety issues for us */
	char buf[MB_CUR_MAX];
	int chlen;
	ILInt32 len = 0;
	wctomb((char *)0, 0);
	while(count > 0)
	{
		chlen = wctomb(buf, (wchar_t)(*chars));
		if(chlen > 0)
		{
			len += (ILInt32)chlen;
		}
		++chars;
		--count;
	}
	return len;
#endif
#endif
}

/*
 * Get the bytes that are represented by an array of characters.
 */
static ILInt32 GetBytes(ILUInt16 *chars, ILInt32 charCount,
                        ILUInt8 *bytes, ILInt32 byteCount)
{
#ifdef IL_USE_LATIN1
	ILInt32 len;
	ILUInt32 ch;

	/* Check for enough space in the output buffer */
	if(charCount > byteCount)
	{
		return -1;
	}

	/* Convert the characters */
	len = charCount;
	while(len > 0)
	{
		ch = *chars++;
		if(ch < (ILUInt16)0x0100)
		{
			*bytes++ = (ILUInt8)ch;
		}
		else
		{
			*bytes++ = (ILUInt8)'?';
		}
	}
	return charCount;
#else
#if HAVE_WCRTOMB
	/* Use the re-entrant function to perform the conversion */
	mbstate_t state;
	char buf[MB_CUR_MAX+1];
	size_t chlen;
	ILUInt16 ch;
	ILInt32 len = 0;
	ILMemZero(&state, sizeof(state));
	wcrtomb(buf, 0, &state);
	while(charCount > 0)
	{
		ch = *chars++;
		if(ch != 0)
		{
			chlen = wcrtomb(buf, (wchar_t)ch, &state);
			if(chlen != (size_t)(-1))
			{
				if(((ILInt32)chlen) > byteCount)
				{
					return -1;
				}
				ILMemCpy(bytes, buf, chlen);
				bytes += chlen;
				byteCount -= (ILInt32)chlen;
				len += (ILInt32)chlen;
			}
		}
		else if(byteCount > 0)
		{
			*bytes++ = 0;
			--byteCount;
			++len;
		}
		else
		{
			return -1;
		}
		--charCount;
	}
	return len;
#else
	/* Use the non re-entrant function to perform the conversion
	   and just hope that the underlying libc takes care of the
	   thread-safety issues for us */
	char buf[MB_CUR_MAX];
	int chlen;
	ILInt32 len = 0;
	wctomb((char *)0, 0);
	while(charCount > 0)
	{
		chlen = wctomb(buf, (wchar_t)(*chars));
		if(chlen > 0)
		{
			if(((ILInt32)chlen) > byteCount)
			{
				return -1;
			}
			ILMemCpy(bytes, buf, chlen);
			bytes += chlen;
			byteCount -= (ILInt32)chlen;
			len += (ILInt32)chlen;
		}
		++chars;
		--charCount;
	}
	return len;
#endif
#endif
}

/*
 * Get the characters that are represented by an array of bytes.
 */
static ILInt32 GetChars(ILUInt8 *bytes, ILInt32 byteCount,
						ILUInt16 *chars, ILInt32 charCount)
{
#ifdef IL_USE_LATIN1
	ILInt32 len;

	/* Check for enough space in the output buffer */
	if(byteCount > charCount)
	{
		return -1;
	}

	/* Convert the bytes */
	len = byteCount;
	while(len > 0)
	{
		*chars++ = (ILUInt16)(*bytes++);
	}
	return byteCount;
#else
#if HAVE_MBRTOWC
	/* Use the re-entrant function to perform the conversion */
	mbstate_t state;
	size_t chlen;
	ILInt32 len = 0;
	wchar_t ch;
	int wrlen;
	ILMemZero(&state, sizeof(state));
	mbrtowc((wchar_t *)0, (char *)0, 0, &state);
	while(byteCount > 0)
	{
		chlen = mbrtowc(&ch, (char *)bytes, (size_t)byteCount, &state);
		if(chlen == (size_t)(-1) || chlen == (size_t)(-2))
		{
			/* Invalid character */
			++bytes;
			--byteCount;
		}
		else if(chlen != 0)
		{
			/* Ordinary character */
			wrlen = ILUTF16WriteChar((unsigned short *)0, (unsigned long)ch);
			if(charCount < (ILInt32)wrlen)
			{
				return -1;
			}
			ILUTF16WriteChar(chars, (unsigned long)ch);
			chars += wrlen;
			len += wrlen;
			bytes += chlen;
			byteCount -= (ILInt32)chlen;
		}
		else
		{
			/* Embedded NUL character */
			if(charCount <= 0)
			{
				return -1;
			}
			*chars++ = '\0';
			++len;
			++bytes;
			--byteCount;
		}
	}
	return len;
#else
	/* Use the non re-entrant function to perform the conversion
	   and just hope that the underlying libc takes care of the
	   thread-safety issues for us */
	int chlen;
	ILInt32 len = 0;
	wchar_t ch;
	int wrlen;
	mbtowc((wchar_t *)0, (char *)0, 0);
	while(byteCount > 0)
	{
		chlen = mbtowc(&ch, (char *)bytes, (size_t)byteCount);
		if(chlen > 0)
		{
			/* Ordinary character */
			wrlen = ILUTF16WriteChar((unsigned short *)0, (unsigned long)ch);
			if(charCount < (ILInt32)wrlen)
			{
				return -1;
			}
			ILUTF16WriteChar(chars, (unsigned long)ch);
			chars += wrlen;
			len += wrlen;
			bytes += chlen;
			byteCount -= (ILInt32)chlen;
		}
		else if(!chlen)
		{
			/* Embedded NUL character */
			if(charCount <= 0)
			{
				return -1;
			}
			*chars++ = '\0';
			++len;
			++bytes;
			--byteCount;
		}
		else
		{
			/* Invalid character */
			++bytes;
			--byteCount;
		}
	}
	return len;
#endif
#endif
}

/*
 * private static int InternalGetByteCount(char[] chars, int index, int count);
 */
ILInt32 _IL_DefaultEncoding_InternalGetByteCount_acii
				(ILExecThread *_thread, System_Array *chars,
				 ILInt32 index, ILInt32 count)
{
	return GetByteCount(((ILUInt16 *)ArrayToBuffer(chars)) + index, count);
}

/*
 * private static int InternalGetByteCount(String s, int index, int count);
 */
ILInt32 _IL_DefaultEncoding_InternalGetByteCount_Stringii
				(ILExecThread *_thread, ILString *s,
				 ILInt32 index, ILInt32 count)
{
	return GetByteCount(StringToBuffer(s) + index, count);
}

/*
 * private static int InternalGetBytes(char[] chars, int charIndex,
 *									   int charCount, byte[] bytes,
 *									   int byteIndex);
 */
ILInt32 _IL_DefaultEncoding_InternalGetBytes_aciiaBi
				(ILExecThread *_thread, System_Array *chars,
				 ILInt32 charIndex, ILInt32 charCount,
				 System_Array * bytes, ILInt32 byteIndex)
{
	return GetBytes(((ILUInt16 *)ArrayToBuffer(chars)) + charIndex, charCount,
					((ILUInt8 *)ArrayToBuffer(bytes)) + byteIndex,
					bytes->length - byteIndex);
}

/*
 * private static int InternalGetBytes(String s, int charIndex,
 *									   int charCount, byte[] bytes,
 *									   int byteIndex);
 */
ILInt32 _IL_DefaultEncoding_InternalGetBytes_StringiiaBi
				(ILExecThread *_thread, ILString *s,
				 ILInt32 charIndex, ILInt32 charCount,
				 System_Array *bytes, ILInt32 byteIndex)
{
	return GetBytes(StringToBuffer(s) + charIndex, charCount,
					((ILUInt8 *)ArrayToBuffer(bytes)) + byteIndex,
					bytes->length - byteIndex);
}

/*
 * private static int InternalGetCharCount(byte[] bytes, int index, int count);
 */
ILInt32 _IL_DefaultEncoding_InternalGetCharCount
				(ILExecThread *_thread, System_Array *bytes,
				 ILInt32 index, ILInt32 count)
{
#ifdef IL_USE_LATIN1
	return count;
#else
#if HAVE_MBRTOWC
	/* Use the re-entrant function to perform the conversion */
	mbstate_t state;
	size_t chlen;
	ILInt32 len = 0;
	wchar_t ch;
	char *buf = ((char *)(ArrayToBuffer(bytes))) + index;
	ILMemZero(&state, sizeof(state));
	mbrtowc((wchar_t *)0, (char *)0, 0, &state);
	while(count > 0)
	{
		chlen = mbrtowc(&ch, buf, (size_t)count, &state);
		if(chlen == (size_t)(-1) || chlen == (size_t)(-2))
		{
			/* Invalid character */
			++buf;
			--count;
		}
		else if(chlen != 0)
		{
			/* Ordinary character */
			len += ILUTF16WriteChar((unsigned short *)0, (unsigned long)ch);
			buf += chlen;
			count -= (ILInt32)chlen;
		}
		else
		{
			/* Embedded NUL character */
			++len;
			++buf;
			--count;
		}
	}
	return len;
#else
	/* Use the non re-entrant function to perform the conversion
	   and just hope that the underlying libc takes care of the
	   thread-safety issues for us */
	int chlen;
	ILInt32 len = 0;
	wchar_t ch;
	char *buf = ((char *)(ArrayToBuffer(bytes))) + index;
	mbtowc((wchar_t *)0, (char *)0, 0);
	while(count > 0)
	{
		chlen = mbtowc(&ch, buf, (size_t)count);
		if(chlen > 0)
		{
			/* Ordinary character */
			len += ILUTF16WriteChar((unsigned short *)0, (unsigned long)ch);
			buf += chlen;
			count -= (ILInt32)chlen;
		}
		else if(!chlen)
		{
			/* Embedded NUL character */
			++len;
			++buf;
			--count;
		}
		else
		{
			/* Invalid character */
			++buf;
			--count;
		}
	}
	return len;
#endif
#endif
}

/*
 * private static int InternalGetChars(byte[] bytes, int byteIndex,
 *									   int byteCount, char[] chars,
 *									   int charIndex);
 */
ILInt32 _IL_DefaultEncoding_InternalGetChars
				(ILExecThread *_thread, System_Array *bytes,
				 ILInt32 byteIndex, ILInt32 byteCount,
				 System_Array *chars, ILInt32 charIndex)
{
	return GetChars(((ILUInt8 *)ArrayToBuffer(bytes)) + byteIndex, byteCount,
					((ILUInt16 *)ArrayToBuffer(chars)) + charIndex,
					chars->length - charIndex);
}

/*
 * private static int InternalGetMaxByteCount(int charCount);
 */
ILInt32 _IL_DefaultEncoding_InternalGetMaxByteCount
				(ILExecThread *_thread, ILInt32 charCount)
{
#ifdef IL_USE_LATIN1
	return charCount;
#else
	return charCount * MB_CUR_MAX;
#endif
}

/*
 * private static int InternalGetMaxCharCount(int byteCount);
 */
ILInt32 _IL_DefaultEncoding_InternalGetMaxCharCount
				(ILExecThread *_thread, ILInt32 byteCount)
{
	return byteCount;
}

/*
 * private static String InternalGetString(byte[] bytes, int index, int count);
 */
ILString *_IL_DefaultEncoding_InternalGetString
				(ILExecThread *_thread, System_Array *bytes,
				 ILInt32 index, ILInt32 count)
{
	System_String *str;
	ILInt32 charCount = _IL_DefaultEncoding_InternalGetCharCount
			(_thread, bytes, index, count);
	if(charCount < 0)
	{
		return 0;
	}
	str = _IL_String_NewString(_thread, charCount);
	if(!str)
	{
		return 0;
	}
	GetChars(((ILUInt8 *)ArrayToBuffer(bytes)) + index, count,
			 StringToBuffer(str), str->length);
	return (ILString *)str;
}

/*
 * internal static int InternalCodePage();
 */
ILInt32 _IL_DefaultEncoding_InternalCodePage(ILExecThread *_thread)
{
#ifdef IL_USE_LATIN1
	/* We know that the default encoding is Latin-1, so return
	   the magic Windows code page number for it */
	return 28591;
#else
	/* We have no idea what the code page is, so force the
	   "System.Text.Encoding.Default" property to allocate an
	   instance of the "System.Text.DefaultEncoding" class */
	return 0;
#endif
}

#ifdef	__cplusplus
};
#endif
