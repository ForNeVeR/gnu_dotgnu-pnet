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

#ifndef _GNU_SOURCE
#define _GNU_SOURCE 1
#endif
#include "ildb_context.h"
#include "ildb_utils.h"
#include "ildb_cmd.h"
#include "il_system.h"
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

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
#if HAVE_GET_CURRENT_DIR_NAME
	char *dir = get_current_dir_name();
#elif HAVE_GETCWD
	char name[8192];
	char *dir = getcwd(name, sizeof(name));
#elif HAVE_GETWD
	char name[8192];
	char *dir = getwd(name);
#else
	char *dir = 0;
#endif
	ILDbInfo(db, "Working directory %s.", (dir ? dir : "."));
#if HAVE_GET_CURRENT_DIR_NAME
	free(dir);
#endif
}

/*
 * Table of system commands.
 */
ILDbCmdInfo ILDbSystemCommands[] = {
	{"quit", 1, 0, 0, Quit, 0,
		"quit help"},
	{"pwd", 2, 0, 0, Pwd, 0,
		"pwd help"},
};
int ILDbNumSystemCommands = (sizeof(ILDbSystemCommands) /
							 sizeof(ILDbCmdInfo));

#ifdef	__cplusplus
};
#endif
