/*
 * ildb_source.h - Manipulate source files in memory.
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

#ifndef	_ILDB_SOURCE_H
#define	_ILDB_SOURCE_H

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Structure of a source file's contents in memory.
 */
typedef struct _tagILDbSourceFile ILDbSourceFile;
struct _tagILDbSourceFile
{
	ILDb		   *owner;
	ILDbSourceFile *next;
	char		   *filename;
	char		   *className;
	ILClass		   *classInfo;
	char		   *text;
	unsigned long	textLen;
	unsigned long	textMax;
	unsigned long  *lines;
	unsigned long	numLines;
	unsigned long	maxLines;
	int				prevIsEOL;
	int				fileIsTemp;

};

/*
 * Create or get a pseudo source file for a class that does not
 * have debug information associated with it.
 */
ILDbSourceFile *ILDbSourceCreatePseudo(ILDb *db, ILClass *classInfo);

/*
 * Create or get a regular source file.  Returns NULL if the
 * file could not be opened.
 */
ILDbSourceFile *ILDbSourceCreate(ILDb *db, const char *filename);

/*
 * Get the on-disk version of of source file's name.  If the
 * file contains pseudo source, it will be written to disk first.
 * Returns NULL if it is not possible to get the on-disk name.
 */
const char *ILDbSourceDiskFile(ILDbSourceFile *file);

/*
 * Get the "display name" for a source file.
 */
const char *ILDbSourceDisplayName(ILDbSourceFile *file);

/*
 * Print a string to an in-memory source file.
 */
void ILDbSourcePrint(ILDbSourceFile *file, const char *str);
void ILDbSourcePrintIndent(ILDbSourceFile *file, int indent, const char *str);

/*
 * Register line information about a program item for the current line.
 */
void ILDbSourceRegisterItem(ILDbSourceFile *file, ILProgramItem *item,
							ILInt32 offset);

/*
 * Get the source file that contains a particular class or member.
 * Returns NULL if it was not possible to locate an approprate file.
 */
ILDbSourceFile *ILDbSourceGet(ILDb *db, ILClass *classInfo, ILMember *member);

/*
 * Destroy a list of cached source files.
 */
void ILDbSourceDestroy(ILDbSourceFile *list);

/*
 * Get a particular line from a source file.  Returns the length
 * of the line, or -1 if the line number is out of range.
 */
long ILDbSourceGetLine(ILDbSourceFile *file, long num, char **line);

#ifdef	__cplusplus
};
#endif

#endif	/* _ILDB_SOURCE_H */
