/*
 * ildb_context.c - Main context that holds the debugger's state.
 *
 * Copyright (C) 2001, 2002  Southern Storm Software, Pty Ltd.
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
#include "ildb_search.h"
#include "ildb_source.h"
#include "il_system.h"

#ifdef	__cplusplus
extern	"C" {
#endif

ILDb *ILDbCreate(int argc, char **argv)
{
	ILDb *db;
	char *env;

	/* Initialize the runtime engine's global data */
	ILExecInit(0);

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
	db->listSize = 10;
	db->process = 0;
	db->dbgContext = 0;
	db->debugProgram = 0;
	db->args = 0;
	db->entryPoint = 0;
	db->running = 0;
	db->printFullNames = 0;
	db->sourceFiles = 0;
	db->currFile = 0;
	db->currLine = 1;
	db->tabStops = 8;

	/* Set the initial source directory search path */
	db->dirSearch = 0;
	db->dirSearchNum = 0;
	ILDbSearchReset(db);

	/* Register the standard command lists */
	ILDbRegisterCommands(db, ILDbRunCommands, ILDbNumRunCommands);
	ILDbRegisterCommands(db, ILDbListCommands, ILDbNumListCommands);
	ILDbRegisterCommands(db, ILDbDisplayCommands, ILDbNumDisplayCommands);
	ILDbRegisterCommands(db, ILDbShowCommands, ILDbNumShowCommands);
	ILDbRegisterCommands(db, ILDbSystemCommands, ILDbNumSystemCommands);
	ILDbRegisterCommands(db, ILDbSetCommands, ILDbNumSetCommands);
	ILDbRegisterCommands(db, ILDbHelpCommands, ILDbNumHelpCommands);

	/* Change the tab stop setting if EXINIT has "ts=4" in it */
	env = getenv("EXINIT");
	while(env != 0 && *env != '\0')
	{
		if(!strncmp(env, "ts=4", 4) && (env[4] == '\0' || env[4] == ' '))
		{
			db->tabStops = 4;
			break;
		}
		++env;
	}

	/* Return the context to the caller */
	return db;
}

void ILDbDestroy(ILDb *db)
{
	ILDbSourceDestroy(db->sourceFiles);
	if(db->debugProgram)
	{
		ILFree(db->debugProgram);
	}
	ILDbFreeStringArray(db->args, -1);
	if(db->args)
	{
	}
	if(db->dbgContext)
	{
		ILDebugDestroy(db->dbgContext);
	}
	if(db->process)
	{
		ILExecProcessDestroy(db->process);
	}
	ILDbFreeCommands(db);
	ILDbSearchDestroy(db);
	ILDbTraceClose(db);
	ILFree(db);
}

ILDebugContext *ILDbGetDebugContext(ILDb *db, ILImage *image)
{
	if(db->dbgContext)
	{
		/* We may already have a cached context for this image */
		if(ILDebugToImage(db->dbgContext) == image)
		{
			return db->dbgContext;
		}

		/* Destroy the current context */
		ILDebugDestroy(db->dbgContext);
		db->dbgContext = 0;
	}
	if(!ILDebugPresent(image))
	{
		return 0;
	}
	db->dbgContext = ILDebugCreate(image);
	if(!(db->dbgContext))
	{
		ILDbOutOfMemory(db);
	}
	return db->dbgContext;
}

#ifdef	__cplusplus
};
#endif
