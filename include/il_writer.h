/*
 * il_writer.h - Routines for writing IL executable images.
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

#ifndef	_IL_WRITER_H
#define	_IL_WRITER_H

#include "il_program.h"
#include "il_utils.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Opaque definition of an IL image writer control structure.
 */
typedef struct _tagILWriter ILWriter;

/*
 * Section flag bits of interest to IL images.
 */
#define	IL_IMAGESECT_CODE			0x00000020
#define	IL_IMAGESECT_INIT_DATA		0x00000040
#define	IL_IMAGESECT_DISCARDABLE	0x02000000
#define	IL_IMAGESECT_EXECUTE		0x20000000
#define	IL_IMAGESECT_READ			0x40000000

/*
 * Predefined flag combinations for interesting sections.
 */
#define	IL_IMAGESECT_TEXT			(IL_IMAGESECT_CODE | \
									 IL_IMAGESECT_EXECUTE | \
									 IL_IMAGESECT_READ)
#define	IL_IMAGESECT_RSRC			(IL_IMAGESECT_INIT_DATA | \
									 IL_IMAGESECT_READ)
#define	IL_IMAGESECT_RELOC			(IL_IMAGESECT_INIT_DATA | \
									 IL_IMAGESECT_DISCARDABLE | \
									 IL_IMAGESECT_READ)
#define	IL_IMAGESECT_DEBUG			(IL_IMAGESECT_INIT_DATA | \
									 IL_IMAGESECT_DISCARDABLE | \
									 IL_IMAGESECT_READ)

/*
 * Offset of entries within the IL runtime header.
 */
#define	IL_IMAGEENTRY_METADATA		8
#define	IL_IMAGEENTRY_RESOURCES		24
#define	IL_IMAGEENTRY_STRONG_NAMES	32
#define	IL_IMAGEENTRY_CODE_MANAGER	40
#define	IL_IMAGEENTRY_VTABLE_FIXUPS	48
#define	IL_IMAGEENTRY_EXPORT_ADDRS	56

/*
 * Flags that modify the behaviour of the writer.
 */
#define	IL_WRITEFLAG_32BIT_ONLY		1	/* Execute on 32-bit systems only */
#define	IL_WRITEFLAG_SUBSYS_CUI		0	/* Command-line subsystem */
#define	IL_WRITEFLAG_SUBSYS_GUI		2	/* GUI subsystem */
#define	IL_WRITEFLAG_OLD_META		4	/* Use Beta 1 and earlier metadata */

/*
 * Debug information types.
 */
#define	IL_DEBUGTYPE_LINE			1	/* Line numbers */
#define	IL_DEBUGTYPE_LINE_OFFSETS	2	/* Line numbers and offsets */
#define	IL_DEBUGTYPE_VARS			3	/* Local variables */
#define	IL_DEBUGTYPE_VARS_OFFSETS	4	/* Local variables and offsets */

/*
 * Create an image writer and attach it to a file stream.
 * If "seekable" is non-zero, then the stream is seekable.
 */
ILWriter *ILWriterCreate(FILE *stream, int seekable, int type, int flags);

/*
 * Output the metadata from an image structure into an
 * image writer's output stream.  This is typically called
 * just before "ILWriterDestroy".
 */
void ILWriterOutputMetadata(ILWriter *writer, ILImage *image);

/*
 * Flush the remainder of an image and destroy an image writer.
 * Returns 1 if OK, 0 if a write error occurred, or -1 if out
 * of memory.
 */
int ILWriterDestroy(ILWriter *writer);

/*
 * Get the current RVA of the text section.
 */
unsigned long ILWriterGetTextRVA(ILWriter *writer);

/*
 * Write a buffer of bytes to the text section.
 */
void ILWriterTextWrite(ILWriter *writer, const void *buffer,
					   unsigned long size);

/*
 * Align the text section on a 4-byte boundary.
 */
void ILWriterTextAlign(ILWriter *writer);

/*
 * Write a buffer of bytes to another section.
 */
void ILWriterOtherWrite(ILWriter *writer, const char *name,
						unsigned long flags, const void *buffer,
						unsigned size);

/*
 * Update an RVA/size directory entry within the IL runtime header.
 */
void ILWriterUpdateHeader(ILWriter *writer, unsigned long entry,
						  unsigned long rva, unsigned long size);

/*
 * Set the entry point token within the IL runtime header.
 */
void ILWriterSetEntryPoint(ILWriter *writer, ILMethod *method);

/*
 * Specify the RVA of a token reference that needs to be fixed
 * up when the image is finally written out.  The contents
 * of code section at "rva" are replaced with the fixed-up
 * token code that corresponds to "item".
 */
void ILWriterSetFixup(ILWriter *writer, unsigned long rva,
					  ILProgramItem *item);

/*
 * Add a string to the debug section's string table.
 * Returns the string offset.
 */
unsigned long ILWriterDebugString(ILWriter *writer, const char *str);

/*
 * Add debug information for a token to the debug section.
 */
void ILWriterDebugAdd(ILWriter *writer, ILProgramItem *item, int type,
					  const void *info, unsigned long len);

#ifdef	__cplusplus
};
#endif

#endif	/* _IL_WRITER_H */
