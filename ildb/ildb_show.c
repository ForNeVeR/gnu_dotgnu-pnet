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
 * Print information about the current line in the program.
 */
static void Info_Line(ILDb *db, char *argv[])
{
	ILDbInfo(db, "No line number information available.");
}

/*
 * Table of "show" and "info" commands.
 */
ILDbCmdInfo ILDbShowCommands[] = {
	{"show", 3, "directories", 3, Show_Directories, 0,
		"show the directories that are searched for source files",
		0},
	{"show", 3, 0, 0, 0, 0,
		"show information about the debugger",
		0},

	{"info", 3, "line", 2, Info_Line, 0,
		"show information on a particular source line",
		0},
	{"info", 3, 0, 0, 0, 0,
		"show information about the program",
		0},
};
int ILDbNumShowCommands = (sizeof(ILDbShowCommands) / sizeof(ILDbCmdInfo));

#ifdef	__cplusplus
};
#endif
