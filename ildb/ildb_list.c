/*
 * ildb_list.c - Processing for "list" commands.
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
 * List the contents of a file, function, etc.
 */
static void List(ILDb *db, char *args)
{
}

/*
 * Table of "list" commands.
 */
ILDbCmdInfo ILDbListCommands[] = {
	{"l", 1, 0, 0, 0, List,
		"list help"},
	{"list", 2, 0, 0, 0, List,
		"list help"},
};
int ILDbNumListCommands = (sizeof(ILDbListCommands) / sizeof(ILDbCmdInfo));

#ifdef	__cplusplus
};
#endif
