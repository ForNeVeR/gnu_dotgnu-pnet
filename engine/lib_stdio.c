/*
 * lib_stdin.c - Internalcall methods for "Platform.Stdio".
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

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * public static void StdClose(int fd);
 */
static void Platform_Stdio_StdClose(ILExecThread *thread,
									ILInt32 fd)
{
	/* We don't currently do anything as allowing programs
	   to close stdin/stdout/stderr is fraught with danger */
}

/*
 * public static void StdFlush(int fd);
 */
static void Platform_Stdio_StdFlush(ILExecThread *thread,
									ILInt32 fd)
{
	switch(fd)
	{
		case 0:		fflush(stdin); break;
		case 1:		fflush(stdout); break;
		case 2:		fflush(stderr); break;
	}
}

/*
 * public static void StdWrite(int fd, char value);
 */
static void Platform_Stdio_StdWrite_1(ILExecThread *thread,
									  ILInt32 fd, ILUInt16 value)
{
	switch(fd)
	{
		case 1:		putc((value & 0xFF), stdout); break;
		case 2:		putc((value & 0xFF), stderr); break;
	}
}

/*
 * Write a 16-bit character buffer to a file descriptor.
 */
static void StdWrite(ILInt32 fd, ILUInt16 *buf, ILInt32 length)
{
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
}

/*
 * public static void StdWrite(int fd, char[] value, int index, int count);
 */
static void Platform_Stdio_StdWrite_2(ILExecThread *thread,
									  ILInt32 fd,
									  System_Array *value,
									  ILInt32 index,
									  ILInt32 count)
{
	StdWrite(fd, ((ILUInt16 *)ArrayToBuffer(value)) + index, count);
}

/*
 * public static void StdWrite(int fd, String value)
 */
static void Platform_Stdio_StdWrite_3(ILExecThread *thread,
									  ILInt32 fd,
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
static ILInt32 Platform_Stdio_StdRead_1(ILExecThread *thread,
									    ILInt32 fd)
{
	if(fd == 0)
	{
		return (ILInt32)(getc(stdin));
	}
	else
	{
		return -1;
	}
}

/*
 * public static int StdRead(int fd, char[] value, int index, int count);
 */
static ILInt32 Platform_Stdio_StdRead_2(ILExecThread *thread,
									    ILInt32 fd,
										System_Array *value,
										ILInt32 index,
										ILInt32 count)
{
	return 0;
}

/*
 * public static int StdPeek(int fd);
 */
static ILInt32 Platform_Stdio_StdPeek(ILExecThread *thread,
									  ILInt32 fd)
{
	if(fd == 0)
	{
		int ch = getc(stdin);
		if(ch != -1)
		{
			ungetc(ch, stdin);
		}
		return (ILInt32)ch;
	}
	else
	{
		return -1;
	}
}

/*
 * Method table for the "Platform.Stdio" class.
 */
IL_METHOD_BEGIN(_ILPlatformStdioMethods)
	IL_METHOD("StdClose",	 "(i)V",		Platform_Stdio_StdClose)
	IL_METHOD("StdFlush",	 "(i)V",		Platform_Stdio_StdFlush)
	IL_METHOD("StdWrite",	 "(ic)V",		Platform_Stdio_StdWrite_1)
	IL_METHOD("StdWrite",	 "(i[cii)V",	Platform_Stdio_StdWrite_2)
	IL_METHOD("StdWrite",	 "(ioSystem.String;)V",	Platform_Stdio_StdWrite_3)
	IL_METHOD("StdRead",	 "(i)i",		Platform_Stdio_StdRead_1)
	IL_METHOD("StdRead",	 "(i[cii)i",	Platform_Stdio_StdRead_2)
	IL_METHOD("StdPeek",	 "(i)i",		Platform_Stdio_StdPeek)
IL_METHOD_END

#ifdef	__cplusplus
};
#endif
