/*
 * ildb_context.c - Main context that holds the debugger's state.
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
#include "ildb_cmd.h"
#include "ildb_utils.h"
#include "il_system.h"

#ifdef	__cplusplus
extern	"C" {
#endif

ILDb *ILDbCreate(int argc, char **argv)
{
	ILDb *db;

	/* Allocate and initialize the context structure */
	if((db = (ILDb *)ILMalloc(sizeof(ILDb))) == 0)
	{
		ILDbOutOfMemory(0);
	}
	db->progname = argv[0];
	db->quit = 0;
	db->commands = 0;
	db->lastCommands = 0;
	db->traceStream = 0;

	/* Register the standard command lists */
	ILDbRegisterCommands(db, ILDbListCommands, ILDbNumListCommands);
	ILDbRegisterCommands(db, ILDbDisplayCommands, ILDbNumDisplayCommands);
	ILDbRegisterCommands(db, ILDbShowCommands, ILDbNumShowCommands);
	ILDbRegisterCommands(db, ILDbSystemCommands, ILDbNumSystemCommands);
	ILDbRegisterCommands(db, ILDbSetCommands, ILDbNumSetCommands);

	/* Return the context to the caller */
	return db;
}

void ILDbDestroy(ILDb *db)
{
	ILDbFreeCommands(db);
	ILDbTraceClose(db);
	ILFree(db);
}

#ifdef	__cplusplus
};
#endif
