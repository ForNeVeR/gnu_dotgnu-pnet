/*
 * ildb_show.c - Processing for "show" and "info" commands.
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
#include "ildb_cmd.h"
#include "ildb_search.h"
#include "ildb_source.h"
#include "il_system.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Show the directories that are searched for source files.
 */
static void Show_Directories(ILDb *db, char *argv[])
{
	ILDbSearchPrint(db);
}

/*
 * Show the default size of listings.
 */
static void Show_ListSize(ILDb *db, char *argv[])
{
	if(db->listSize != 0 && db->listSize != (unsigned long)(long)(-1))
	{
		ILDbInfo(db, "The number of lines displayed by default is %lu.",
				 db->listSize);
	}
	else
	{
		ILDbInfo(db, "The number of lines displayed by default is unlimited.");
	}
}

/*
 * Print information about the current line in the program.
 */
static void Info_Line(ILDb *db, char *argv[])
{
	if(db->currFile)
	{
		ILDbInfo(db, "Line %ld of \"%s\".",
				 db->currLine, ILDbSourceDisplayName(db->currFile));
	}
	else
	{
		ILDbInfo(db, "No line number information available.");
	}
}

/*
 * Print information about the current file in the program.
 */
static void Info_Source(ILDb *db, char *argv[])
{
	if(db->currFile)
	{
		const char *filename = ILDbSourceDiskFile(db->currFile);
		int len;
		if(filename)
		{
			len = strlen(filename);
			while(len > 0 && filename[len - 1] != '/' &&
				  filename[len - 1] != '\\')
			{
				--len;
			}
			ILDbInfo(db, "Current source file is %s", filename + len);
			ILDbInfo(db, "Located in %s", filename);
			ILDbInfo(db, "Contains %lu lines.", db->currFile->numLines);
			len = strlen(filename);
			while(len > 0 && filename[len - 1] != '.')
			{
				--len;
			}
			if(db->currFile->classInfo || !ILStrICmp(filename + len, "cs"))
			{
				ILDbInfo(db, "Source language is C#.");
			}
		}
		else
		{
			ILDbInfo(db, "No current source file.");
		}
	}
	else
	{
		ILDbInfo(db, "No current source file.");
	}
}

/*
Current source file is ildb_main.c
Compilation directory is /home/rweather/work/pnet/ildb/
Located in /home/rweather/work/pnet/ildb/ildb_main.c
Contains 314 lines.
Source language is c.
Compiled with stabs debugging format.
*/

/*
 * Table of "show" and "info" commands.
 */
ILDbCmdInfo ILDbShowCommands[] = {
	{"show", 3, "directories", 3, Show_Directories, 0,
		"show the directories that are searched for source files",
		0},
	{"show", 3, "listsize", 2, Show_ListSize, 0,
		"show the default list size",
		0},
	{"show", 3, 0, 0, 0, 0,
		"show information about the debugger",
		0},

	{"info", 3, "line", 2, Info_Line, 0,
		"show information on the current source line",
		0},
	{"info", 3, "source", 6, Info_Source, 0,
		"show information on the particular source file",
		0},
	{"info", 3, 0, 0, 0, 0,
		"show information about the program",
		0},
};
int ILDbNumShowCommands = (sizeof(ILDbShowCommands) / sizeof(ILDbCmdInfo));

#ifdef	__cplusplus
};
#endif
