/*
 * ildb_set.c - Processing for "set" commands.
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
#include "il_system.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Set the directories that are searched for source files.
 */
static void Set_Directories(ILDb *db, char *argv[])
{
	ILDbInfo(db, "Source directories searched: $cdir:$cwd");
}

/*
 * Default set command handler, which ignores its arguments.
 * Useful as a catch-all for programs that think that they
 * are talking to gdb, when they really aren't.
 */
static void Set_Default(ILDb *db, char *argv[])
{
	/* Nothing to do here */
}

/*
 * Table of "set" commands.
 */
ILDbCmdInfo ILDbSetCommands[] = {
	{"directories", 2, 0, 0, Set_Directories, 0,
		"set directories help"},
	{"set", 3, 0, 0, Set_Default, 0,
		"set help"},
};
int ILDbNumSetCommands = (sizeof(ILDbSetCommands) / sizeof(ILDbCmdInfo));

#ifdef	__cplusplus
};
#endif
