/*
 * ildb_context.h - Main context that holds the debugger's state.
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

#ifndef	_ILDB_CONTEXT_H
#define	_ILDB_CONTEXT_H

#include <stdio.h>
#include "il_engine.h"
#include "il_debug.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Forward declarations.
 */
typedef struct _tagILDbCmdList ILDbCmdList;

/*
 * Structure of the main context object for the debugger.
 */
typedef struct
{
	const char     *progname;			/* argv[0] for the program */
	int				quit;				/* Non-zero to quit the debugger */
	ILDbCmdList	   *commands;			/* Registered commands */
	ILDbCmdList	   *lastCommands;		/* Last registered command list */
	FILE		   *traceStream;		/* Trace message stream */
	char		  **dirSearch;			/* Directory search path */
	int				dirSearchNum;		/* Number of search path entries */
	unsigned long	listSize;			/* Default size for "list" displays */
	ILExecProcess  *process;			/* Process that is being debugged */
	ILDebugContext *dbgContext;			/* Current debug context */

} ILDb;

/*
 * Create the debugger context and parse the command-line options.
 */
ILDb *ILDbCreate(int argc, char **argv);

/*
 * Destroy the debugger context.
 */
void ILDbDestroy(ILDb *db);

/*
 * Get a debug context for a specific image.
 */
ILDebugContext *ILDbGetDebugContext(ILDb *db, ILImage *image);

#ifdef	__cplusplus
};
#endif

#endif	/* _ILDB_CONTEXT_H */
