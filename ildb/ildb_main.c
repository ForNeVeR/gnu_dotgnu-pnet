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
#include "ildb_search.h"
#include "il_system.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Copyright and version header which is printed at startup.
 */
static char const header[] =
"ildb " VERSION "\n"
"Copyright 2001, 2002  Southern Storm Software, Pty Ltd.\n"
"ILDB comes with ABSOLUTELY NO WARRANTY.  This is free software,\n"
"and you are welcome to redistribute it under the terms of the\n"
"GNU General Public License.  See the file COPYING for further details.\n";

/*
 * Forward declarations.
 */
static void InputLoop(ILDb *db);
static void PrintHelp(char *progname);

int main(int argc, char *argv[])
{
	ILDb *db;
	int arg;
	char *debugProgram = 0;
	char *startDir = 0;
	int quiet = 0;
	int trace = 0;

	/* Create the debugger context and parse the command-line */
	db = ILDbCreate(argc, argv);

	/* Process the command-line options.  We try to be compatible with
	   as many of gdb's options as possible so that GUI debuggers that
	   wrap up gdb can be fooled into using ildb as-is */
	for(arg = 1; arg < argc; ++arg)
	{
		if(argv[arg][0] == '-')
		{
			/* If the option starts with two "-"'s, then strip the first so
			   that we can support both old-style and new-style gdb options */
			if(argv[arg][1] == '-')
			{
				argv[arg] = argv[arg] + 1;
			}

			/* Parse this option */
			if(!strcmp(argv[arg], "-help") || !strcmp(argv[arg], "-h"))
			{
				/* Print help information and exit */
				PrintHelp(argv[0]);
				return 0;
			}
			else if(!strcmp(argv[arg], "-version") ||
			        !strcmp(argv[arg], "-v"))
			{
				/* Print version information and exit */
				fputs(header, stdout);
				return 0;
			}
			else if(!strncmp(argv[arg], "-exec=", 6))
			{
				/* Name of the program to be debugged: form 1 */
				debugProgram = argv[arg] + 6;
			}
			else if(!strcmp(argv[arg], "-e") ||
			        !strcmp(argv[arg], "-exec") ||
			        !strcmp(argv[arg], "-se"))
			{
				/* Name of the program to be debugged: form 2 */
				if(arg < (argc - 1))
				{
					++arg;
					debugProgram = argv[arg];
				}
			}
			else if(!strncmp(argv[arg], "-se=", 4))
			{
				/* Name of the program to be debugged: form 3 */
				debugProgram = argv[arg] + 4;
			}
			else if(!strncmp(argv[arg], "-directory=", 11))
			{
				/* Directory to add to search path: form 1 */
				ILDbSearchAdd(db, argv[arg] + 11);
			}
			else if(!strcmp(argv[arg], "-d") ||
			        !strcmp(argv[arg], "-directory"))
			{
				/* Directory to add to search path: form 2 */
				if(arg < (argc - 1))
				{
					++arg;
					ILDbSearchAdd(db, argv[arg]);
				}
			}
			else if(!strcmp(argv[arg], "-quiet") ||
			        !strcmp(argv[arg], "-silent") ||
					!strcmp(argv[arg], "-q"))
			{
				/* Suppress the introductory messages */
				quiet = 1;
			}
			else if(!strcmp(argv[arg], "-batch"))
			{
				/* We don't support batch mode */
				fprintf(stderr, "%s: batch mode is not supported\n", argv[0]);
				return 1;
			}
			else if(!strcmp(argv[arg], "-cd"))
			{
				/* Change to a new directory on entry: form 1 */
				if(arg < (argc - 1))
				{
					++arg;
					startDir = argv[arg];
				}
			}
			else if(!strncmp(argv[arg], "-cd=", 4))
			{
				/* Change to a new directory on entry: form 2 */
				startDir = argv[arg] + 4;
			}
			else if(!strcmp(argv[arg], "-fullname") ||
					!strcmp(argv[arg], "-f"))
			{
				/* Print the full name information when stopped */
				db->printFullNames = 1;
			}
			else if(!strcmp(argv[arg], "-trace"))
			{
				/* Enable ildb tracing to help debug the
				   interaction with GUI debuggers */
				trace = 1;
			}
			else if(!strncmp(argv[arg], "-tty=", 5) ||
			        !strcmp(argv[arg], "-tty") ||
			        !strcmp(argv[arg], "-t"))
			{
				/* We don't support tty device debugging */
				fprintf(stderr, "%s: tty debugging is not supported\n",
						argv[0]);
				return 1;
			}
			else if(!strncmp(argv[arg], "-symbols=", 9) ||
			        !strncmp(argv[arg], "-core=", 6) ||
			        !strncmp(argv[arg], "-command=", 9) ||
			        !strncmp(argv[arg], "-annotate=", 10) ||
			        !strncmp(argv[arg], "-interpreter=", 13) ||
			        !strcmp(argv[arg], "-write") ||
			        !strcmp(argv[arg], "-statistics") ||
			        !strcmp(argv[arg], "-nx") ||
			        !strcmp(argv[arg], "-n") ||
			        !strcmp(argv[arg], "-mapped") ||
			        !strcmp(argv[arg], "-m") ||
			        !strcmp(argv[arg], "-readnow") ||
			        !strcmp(argv[arg], "-r") ||
			        !strcmp(argv[arg], "-nowindows") ||
			        !strcmp(argv[arg], "-nw") ||
			        !strcmp(argv[arg], "-windows") ||
			        !strcmp(argv[arg], "-w") ||
			        !strcmp(argv[arg], "-epoch") ||
			        !strcmp(argv[arg], "-async") ||
			        !strcmp(argv[arg], "-noasync"))
			{
				/* Ignore this option as we don't use it */
			}
			else if(!strcmp(argv[arg], "-s") ||
			        !strcmp(argv[arg], "-c") ||
			        !strcmp(argv[arg], "-symbols") ||
			        !strcmp(argv[arg], "-core") ||
			        !strcmp(argv[arg], "-command") ||
			        !strcmp(argv[arg], "-annotate") ||
			        !strcmp(argv[arg], "-interpreter") ||
			        !strcmp(argv[arg], "-x") ||
			        !strcmp(argv[arg], "-b") ||
			        !strcmp(argv[arg], "-baud") ||
			        !strcmp(argv[arg], "-symbols"))
			{
				/* Ignore this option as we don't use it */
				++arg;
			}
			else
			{
				/* This option is not recognized */
				fprintf(stderr, "%s: unrecognized option `%s'\n",
						argv[0], argv[arg]);
				fprintf(stderr,
						"Use `%s --help' for a complete list of options.\n",
						argv[0]);
				return 1;
			}
		}
		else if(!debugProgram)
		{
			/* The first non "-" argument is assumed to be the
			   program that we are trying to debug.  We ignore
			   other non "-" arguments because they are usually
			   core files or process ID's which don't make any
			   sense for us */
			debugProgram = argv[arg];
		}
	}

	/* Open the trace file */
	if(trace)
	{
		ILDbTraceOpen(db, "ildb.out");
	}

	/* Write the command-line options to the trace stream */
	for(arg = 0; arg < argc; ++arg)
	{
		ILDbTrace(db, "argv[%d] = %s", arg, argv[arg]);
	}

	/* Initialize the non-context debugger subsystems */
	ILDbInputInitialize();

	/* Print the initial Copyright and version header */
	if(!quiet)
	{
		fputs(header, stdout);
		fflush(stdout);
	}

	/* Load the program to be debugged */
	if(debugProgram)
	{
		debugProgram = ILExpandFilename(debugProgram, (char *)0);
		if(!debugProgram)
		{
			ILDbOutOfMemory(db);
		}
		db->debugProgram = debugProgram;
		ILDbCreateProcess(db);
	}

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

/*
 * Print the program help information.
 */
static void PrintHelp(char *progname)
{
	fputs(header, stdout);
	printf("\n");
	printf("Usage: %s [options] [program]\n\n", progname);
	printf("    --cd=DIR\n");
	printf("        Change to `DIR' on entry to the debugger.\n");
	printf("    --directory=DIR  or -d DIR\n");
	printf("        Add a directory to the source search path.\n");
	printf("    --quiet          or -q\n");
	printf("        Suppress introductory messages.\n");
	printf("    --version        or -v\n");
	printf("        Print the version of the program.\n");
	printf("    --help           or -h\n");
	printf("        Print this help message.\n\n");
	printf("This program also supports many common gdb(1) options.\n");
}

#ifdef	__cplusplus
};
#endif
