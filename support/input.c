/*
 * input.c - Simple line input facilities for stdin.
 *
 * Copyright (C) 2003  Southern Storm Software, Pty Ltd.
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
#include <stdio.h>

#ifdef	__cplusplus
extern	"C" {
#endif

int ILInputGetChar(void)
{
#ifndef REDUCED_STDIO
	return getc(stdin);
#else
	return -1;
#endif
}

void ILInputUngetChar(int ch)
{
#ifndef REDUCED_STDIO
	ungetc(ch, stdin);
#endif
}

int ILInputReadLine(void *buffer, int len)
{
#ifndef REDUCED_STDIO
	return (fgets(buffer, len, stdin) != 0);
#else
	return 0;
#endif
}

int ILInputRead(void *buffer, int len)
{
#ifndef REDUCED_STDIO
	return (int)fread(buffer, 1, len, stdin);
#else
	return -1;
#endif
}

#ifdef	__cplusplus
};
#endif
