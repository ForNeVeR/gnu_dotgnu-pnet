/*
 * ildb_main.c - Main entry point for the IL debugger.
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
#include "ildb_input.h"
#include "ildb_cmd.h"
#include "ildb_utils.h"
#include "il_system.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Copyright and version header which is printed at startup.
 */
static char const header[] =
"ildb " VERSION "\n"
"Copyright 2001  Southern Storm Software, Pty Ltd.\n"
"ILDB comes with ABSOLUTELY NO WARRANTY.  This is free software,\n"
"and you are welcome to redistribute it under the terms of the\n"
"GNU General Public License.  See the file COPYING for further details.\n";

/*
 * Forward declarations.
 */
static void InputLoop(ILDb *db);

int main(int argc, char *argv[])
{
	ILDb *db;
	int arg;

	/* Create the debugger context and parse the command-line */
	db = ILDbCreate(argc, argv);

	/* Open the trace file */
	/*ILDbTraceOpen(db, "ildb.out");*/

	/* Write the command-line options to the trace stream */
	for(arg = 0; arg < argc; ++arg)
	{
		ILDbTrace(db, "argv[%d] = %s", arg, argv[arg]);
	}

	/* Initialize the non-context debugger subsystems */
	ILDbInputInitialize();

	/* Print the initial Copyright and version header */
	fputs(header, stdout);
	fflush(stdout);

	/* Enter the main input loop */
	InputLoop(db);

	/* Clean up and exit */
	ILDbInputTerminate();
	ILDbDestroy(db);
	return 0;
}

/*
 * Main input loop for the debugger.
 */
static void InputLoop(ILDb *db)
{
	char *line;
	while(!(db->quit) && (line = ILDbInputGetLine("(ildb) ")) != 0)
	{
		/* Print the command to the trace stream */
		ILDbTrace(db, "%s", line);
		
		/* Dispatch the command */
		ILDbDispatchCommand(db, line);
	}
}

#ifdef	__cplusplus
};
#endif
