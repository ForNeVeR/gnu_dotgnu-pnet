/*
 * lib_stdio.c - Internalcall methods for "Platform.Stdio".
 *
 * Copyright (C) 2001, 2002  Southern Storm Software, Pty Ltd.
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

/*

Note: it is possible to access stdin/stdout/stderr through regular
file stream operations using descriptors 0, 1, or 2.  However, those
versions don't go through the regular C stdio routines, and won't
work on platforms that don't have Unix-style file descriptors.

This also provides a convenient place where "System.Console" can be
redirected to somewhere other than the C stdio routines, which may
be useful for debugging in embedded environments.

*/

#include "engine.h"
#include "lib_defs.h"
#include "il_utils.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * public static void StdClose(int fd);
 */
void _IL_Stdio_StdClose(ILExecThread *thread, ILInt32 fd)
{
	/* We don't currently do anything as allowing programs
	   to close stdin/stdout/stderr is fraught with danger */
}

/*
 * public static void StdFlush(int fd);
 */
void _IL_Stdio_StdFlush(ILExecThread *thread, ILInt32 fd)
{
#ifndef REDUCED_STDIO
	switch(fd)
	{
		case 0:		fflush(stdin); break;
		case 1:		fflush(stdout); break;
		case 2:		fflush(stderr); break;
	}
#endif
}

/*
 * public static void StdWrite(int fd, char value);
 */
void _IL_Stdio_StdWrite_ic(ILExecThread *thread, ILInt32 fd, ILUInt16 value)
{
#ifndef REDUCED_STDIO
	switch(fd)
	{
		case 1:		putc((value & 0xFF), stdout); break;
		case 2:		putc((value & 0xFF), stderr); break;
	}
#else
	putchar(value);
#endif
}

/*
 * Write a 16-bit character buffer to a file descriptor.
 */
static void StdWrite(ILInt32 fd, ILUInt16 *buf, ILInt32 length)
{
#ifndef REDUCED_STDIO
	if(fd == 1)
	{
		while(length > 0)
		{
			putc((*buf & 0xFF), stdout);
			++buf;
			--length;
		}
	}
	else if(fd == 2)
	{
		while(length > 0)
		{
			putc((*buf & 0xFF), stderr);
			++buf;
			--length;
		}
	}
#else
	while(length > 0)
	{
		putchar((*buf & 0xFF));
		++buf;
		--length;
	}
#endif
}

/*
 * public static void StdWrite(int fd, char[] value, int index, int count);
 */
void _IL_Stdio_StdWrite_iacii(ILExecThread *thread, ILInt32 fd,
							  System_Array *value, ILInt32 index,
							  ILInt32 count)
{
	StdWrite(fd, ((ILUInt16 *)ArrayToBuffer(value)) + index, count);
}

/*
 * public static void StdWrite(int fd, byte[] value, int index, int count);
 */
void _IL_Stdio_StdWrite_iaBii(ILExecThread *thread, ILInt32 fd,
							  System_Array *value, ILInt32 index,
							  ILInt32 count)
{
#ifndef REDUCED_STDIO
	if(fd == 1)
	{
		fwrite(((ILUInt8 *)ArrayToBuffer(value)) + index, 1, count, stdout);
	}
	else if(fd == 2)
	{
		fwrite(((ILUInt8 *)ArrayToBuffer(value)) + index, 1, count, stderr);
	}
#else
	unsigned char *buf;
	buf = ((unsigned char *)(ArrayToBuffer(value))) + index;
	while(count > 0)
	{
		putchar((*buf & 0xFF));
		++buf;
		--count;
	}
#endif
}

/*
 * public static void StdWrite(int fd, String value)
 */
void _IL_Stdio_StdWrite_iString(ILExecThread *thread, ILInt32 fd,
								ILString *value)
{
	if(value != 0)
	{
		ILUInt16 *buf;
		ILInt32 len = _ILStringToBuffer(thread, value, &buf);
		StdWrite(fd, buf, len);
	}
}

/*
 * public static int StdRead(int fd);
 */
ILInt32 _IL_Stdio_StdRead_i(ILExecThread *thread, ILInt32 fd)
{
	if(fd == 0)
	{
		return (ILInt32)ILInputGetChar();
	}
	else
	{
		return -1;
	}
}

/*
 * public static int StdRead(int fd, char[] value, int index, int count);
 */
ILInt32 _IL_Stdio_StdRead_iacii(ILExecThread *thread, ILInt32 fd,
								System_Array *value, ILInt32 index,
								ILInt32 count)
{
	ILUInt16 *buf = ((ILUInt16 *)(ArrayToBuffer(value))) + index;
	ILInt32 result = 0;
	int ch;
	if(fd != 0)
	{
		return -1;
	}
	while(count > 0)
	{
		ch = ILInputGetChar();
		if(ch == -1)
		{
			break;
		}
		*buf++ = (ILUInt16)(ch & 0xFF);
		++result;
		--count;
	}
	return result;
}

/*
 * public static int StdRead(int fd, byte[] value, int index, int count);
 */
ILInt32 _IL_Stdio_StdRead_iaBii(ILExecThread *thread, ILInt32 fd,
								System_Array *value, ILInt32 index,
								ILInt32 count)
{
	if(fd != 0)
	{
		return -1;
	}
	return (ILInt32)ILInputRead
		(((ILUInt8 *)ArrayToBuffer(value)) + index, (int)count);
}

/*
 * public static int StdPeek(int fd);
 */
ILInt32 _IL_Stdio_StdPeek(ILExecThread *thread, ILInt32 fd)
{
	if(fd == 0)
	{
		int ch = ILInputGetChar();
		if(ch != -1)
		{
			ILInputUngetChar(ch);
		}
		return (ILInt32)ch;
	}
	else
	{
		return -1;
	}
}

#ifdef	__cplusplus
};
#endif
