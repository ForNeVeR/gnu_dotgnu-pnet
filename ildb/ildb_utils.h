/*
 * ildb_utils.h - Debugger utility functions.
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

#ifndef	_ILDB_UTILS_H
#define	_ILDB_UTILS_H

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Bail out if the system is out of memory.
 */
void ILDbOutOfMemory(ILDb *db);

/*
 * Report an error message.
 */
void ILDbError(ILDb *db, const char *format, ...);

/*
 * Report an informational message.
 */
void ILDbInfo(ILDb *db, const char *format, ...);

/*
 * Report a trace message, which allows tracking
 * of internal problems with the debugger.
 */
void ILDbTrace(ILDb *db, const char *format, ...);

/*
 * Open the trace stream, to write to a specific file.
 */
void ILDbTraceOpen(ILDb *db, const char *filename);

/*
 * Close the trace stream.  It is safe to call this
 * even if the stream is not open.
 */
void ILDbTraceClose(ILDb *db);

/*
 * Helper macros.
 */
#define	ILDbIsSpace(ch)	((ch) == ' ' || (ch) == '\t' || \
						 (ch) == '\r' || (ch) == '\n' || \
						 (ch) == '\f' || (ch) == '\v')

/*
 * Parse a numeric quantity.
 */
long ILDbParseNumber(ILDb *db, char *value, int len,
					 const char *name, long defValue);

/*
 * Print the value in a memory location.  The "ptr" value points to
 * the memory that contains the value.
 */
void ILDbPrintValue(ILExecThread *thread, void *ptr, ILType *type,
				    int expand, int hexFlag);

/*
 * Print the static fields within a class.
 */
void ILDbPrintStatic(ILExecThread *thread, ILClass *classInfo,
					 int expand, int hexFlag);

#ifdef	__cplusplus
};
#endif

#endif	/* _ILDB_UTILS_H */
