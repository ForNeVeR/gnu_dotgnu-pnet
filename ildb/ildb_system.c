/*
 * ildb_system.c - Processing for system commands.
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
 * Quit from the program.
 */
static void Quit(ILDb *db, char *argv[])
{
	db->quit = 1;
}

/*
 * Print the current working directory.
 */
static void Pwd(ILDb *db, char *argv[])
{
	char *dir = ILGetCwd();
	ILDbInfo(db, "Working directory %s.", (dir ? dir : "."));
	if(dir)
	{
		ILFree(dir);
	}
}

/*
 * Table of system commands.
 */
ILDbCmdInfo ILDbSystemCommands[] = {
	{"quit", 1, 0, 0, Quit, 0,
		"quit the debugger",
		0},
	{"pwd", 2, 0, 0, Pwd, 0,
		"print the current working directory",
		0},
};
int ILDbNumSystemCommands = (sizeof(ILDbSystemCommands) /
							 sizeof(ILDbCmdInfo));

#ifdef	__cplusplus
};
#endif
