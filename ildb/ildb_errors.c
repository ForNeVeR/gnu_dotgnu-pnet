/*
 * ildb_errors.c - Error message reporting functions.
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

#include "ildb_context.h"
#include "ildb_utils.h"
#include "il_system.h"
#ifdef HAVE_STDARG_H
#include <stdarg.h>
#define	VA_LIST				va_list
#define	VA_START			va_list va; va_start(va, format)
#define	VA_END				va_end(va)
#define	VA_GET_LIST			va
#else
#ifdef HAVE_VARARGS_H
#include <varargs.h>
#define	VA_LIST				va_list
#define	VA_START			va_list va; va_start(va)
#define	VA_END				va_end(va)
#define	VA_GET_LIST			va
#else
#define	VA_LIST				int
#define	VA_START
#define	VA_END
#define	VA_GET_LIST			0
#endif
#endif

#ifdef	__cplusplus
extern	"C" {
#endif

void ILDbOutOfMemory(ILDb *db)
{
	if(db)
	{
		fputs(db->progname, stderr);
		fputs(": ", stderr);
	}
	fputs("virtual memory exhausted\n", stderr);
	exit(1);
}

/*
 * Print a message to a particular stream.
 */
static void PrintMessage(FILE *stream, const char *format, VA_LIST va)
{
	/* Print the message */
#ifdef HAVE_VFPRINTF
	vfprintf(stream, format, va);
#else
	/* Shouldn't happen, but at least print the format */
	fputs(format, stream);
#endif

	/* Terminate the line */
	putc('\n', stream);

	/* Flush the stream */
	fflush(stream);
}

void ILDbError(ILDb *db, const char *format, ...)
{
	VA_START;
	PrintMessage(stdout, format, VA_GET_LIST);
	VA_END;
}

void ILDbInfo(ILDb *db, const char *format, ...)
{
	VA_START;
	PrintMessage(stdout, format, VA_GET_LIST);
	VA_END;
}

void ILDbTrace(ILDb *db, const char *format, ...)
{
	if(db->traceStream)
	{
		VA_START;
		PrintMessage(db->traceStream, format, VA_GET_LIST);
		VA_END;
	}
}

void ILDbTraceOpen(ILDb *db, const char *filename)
{
	db->traceStream = fopen(filename, "w");
	if(!(db->traceStream))
	{
		perror(filename);
	}
}

void ILDbTraceClose(ILDb *db)
{
	if(db->traceStream)
	{
		fclose(db->traceStream);
		db->traceStream = 0;
	}
}

#ifdef	__cplusplus
};
#endif
