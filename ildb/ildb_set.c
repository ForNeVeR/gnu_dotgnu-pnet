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
#include "ildb_search.h"
#include "il_system.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Set the directories that are searched for source files.
 */
static void Set_Directory(ILDb *db, char *argv[])
{
	int posn;
	char *dir;
	int len;

	/* Reset the path or add new entries to it */
	if(!(argv[0]))
	{
		/* TODO: query the user before obliterating the path */
		ILDbSearchReset(db);
	}
	else
	{
		/* Process the entries in reverse order, so that they
		   are moved to the front of the path correctly */
		posn = 0;
		while(argv[posn + 1] != 0)
		{
			++posn;
		}
		while(posn >= 0)
		{
			/* Split this argument into sections separated by ':' */
			dir = argv[posn];
			len = strlen(dir);
			while(len > 0)
			{
				while(len > 0 && dir[len - 1] != ':')
				{
					--len;
				}
				ILDbSearchAdd(db, dir + len);
				if(len > 0)
				{
					--len;
					dir[len] = '\0';
				}
			}
			--posn;
		}
	}

	/* Print the new state of the directory search path */
	ILDbSearchPrint(db);
}

/*
 * Set the number of source lines to show by default with "list".
 */
static void Set_ListSize(ILDb *db, char *argv[])
{
	db->listSize = (unsigned long)ILDbParseNumber(db, argv[0], -1, "list size",
											      (long)(db->listSize));
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
	{"directory", 3, 0, 0, Set_Directory, 0,
		"set the directories to search for source files",
		0},

	{"set", 3, "listsize", 2, Set_ListSize, 0,
		"set number of source lines displayed by default",
		0},
	{"set", 3, 0, 0, Set_Default, 0,
		"set debugger options and variables",
		0},
};
int ILDbNumSetCommands = (sizeof(ILDbSetCommands) / sizeof(ILDbCmdInfo));

#ifdef	__cplusplus
};
#endif
