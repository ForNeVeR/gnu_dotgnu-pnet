/*
 * ildb_run.c - Processing for run and breakpoint commands.
 *
 * Copyright (C) 2002  Southern Storm Software, Pty Ltd.
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
#include "ildb_input.h"
#include "ildb_source.h"
#include "il_gc.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Debug hook for application execution.
 */
static int DebugHook(ILDb *db, ILExecThread *thread, ILMethod *method,
					 ILInt32 offset, int type)
{
	/* TODO */
	return IL_HOOK_CONTINUE;
}

int ILDbCreateProcess(ILDb *db)
{
	int error;

	/* Bail out if we already have a process */
	if(db->process)
	{
		return 1;
	}

	/* Report an error if there is no program to be loaded */
	if(!(db->debugProgram))
	{
		ILDbError(db, "No executable file specified.  "
					  "Use the \"file\" command.");
		return 0;
	}

	/* Destroy the cached source files prior to loading the application */
	ILDbSourceDestroy(db->sourceFiles);
	db->sourceFiles = 0;
	db->currFile = 0;
	db->currLine = 1;

	/* Create the execution process and install the debug hook */
	db->process = ILExecProcessCreate(0, 0);
	if(!(db->process))
	{
		ILDbOutOfMemory(db);
	}
	if(!ILExecProcessDebugHook
			(db->process, (ILExecDebugHookFunc)DebugHook, db))
	{
		ILExecProcessDestroy(db->process);
		db->process = 0;
		ILDbError(db, "The runtime engine does not support debugging.");
		return 0;
	}

	/* Load the application into the execution process */
	error = ILExecProcessLoadFile(db->process, db->debugProgram);
	if(error < 0)
	{
		perror(db->debugProgram);
		fflush(stderr);
		ILExecProcessDestroy(db->process);
		db->process = 0;
		return 0;
	}
	else if(error > 0)
	{
		fprintf(stderr, "%s: %s\n", db->debugProgram, ILImageLoadError(error));
		fflush(stderr);
		ILExecProcessDestroy(db->process);
		db->process = 0;
		return 0;
	}

	/* Find the entry point and validate it */
	db->entryPoint = ILExecProcessGetEntry(db->process);
	if(!(db->entryPoint))
	{
		fprintf(stderr, "%s: no program entry point\n", db->debugProgram);
		fflush(stderr);
		ILExecProcessDestroy(db->process);
		db->process = 0;
		return 0;
	}
	if(ILExecProcessEntryType(db->entryPoint) == IL_ENTRY_INVALID)
	{
		fprintf(stderr, "%s: invalid entry point\n", db->debugProgram);
		fflush(stderr);
		ILExecProcessDestroy(db->process);
		db->process = 0;
		return 0;
	}

	/* Install the initial breakpoint watches */
	/* TODO */

	/* Ready to go */
	return 1;
}

/*
 * Run a program using the debugger.
 */
static void RunCmd(ILDb *db, char *argv[])
{
	ILExecThread *thread;
	ILObject *args;
	int argc, argNum;
	ILString *argString;
	int sawException;
	ILInt32 retval;
	ILObject *exception;
	char *exString;

	/* Clear the input repeat buffer */
	ILDbInputClearRepeat();

	/* Record the arguments to the "run" command for later */
	if(argv[0] != 0)
	{
		ILDbFreeStringArray(db->args, -1);
		db->args = ILDbDupStringArray(db, argv, -1);
	}

	/* Do we already have a running application? */
	if(db->running)
	{
		/* Prompt the user to determine if we can restart */
		if(!ILDbInputConfirm
			("The program being debugged has been started already.\n"
			 "Start it from the beginning? (y or n) ", 0))
		{
			return;
		}

		/* Abort the running application */
		longjmp(db->jumpBack, 1);
	}

	/* Set a jump-back point for aborted applications */
	if(setjmp(db->jumpBack))
	{
		/* Destroy the process */
		db->running = 0;
		ILExecProcessDestroy(db->process);
		db->process = 0;
		ILGCCollect();
	}

	/* If we don't have an execution process, then create one */
	if(!ILDbCreateProcess(db))
	{
		return;
	}

	/* Tell the user that we are attempting to start the program */
	fputs("Starting program: ", stdout);
	fputs(db->debugProgram, stdout);
	if(db->args)
	{
		argNum = 0;
		while(db->args[argNum] != 0)
		{
			putc(' ', stdout);
			fputs(db->args[argNum], stdout);
			++argNum;
		}
	}
	putc('\n', stdout);
	fflush(stdout);

	/* Turn off the readline input handling */
	ILDbInputDisable();

	/* The process is now running */
	db->running = 1;

	/* Create the argument buffer to pass to "Main" */
	if(db->args)
	{
		argc = 0;
		while(db->args[argc] != 0)
		{
			++argc;
		}
	}
	else
	{
		argc = 0;
	}
	thread = ILExecProcessGetMain(db->process);
	args = ILExecProcessSetCommandLine
				(db->process, db->debugProgram, db->args);

	/* Call the "Main" method.  This will return back through the
	   debug hook when it reaches a breakpoint */
	sawException = 0;
	if(args != 0 && !ILExecThreadHasException(thread))
	{
		retval = 0;
		if(ILExecThreadCall(thread, db->entryPoint, &retval, args))
		{
			/* An exception was thrown while executing the program */
			sawException = 1;
			retval = 1;
		}
	}
	else
	{
		/* An exception was thrown while building the argument array */
		sawException = 1;
		retval = 1;
	}

	/* Report the exit status or the exception that was thrown */
	if(!sawException)
	{
		if(!retval)
		{
			ILDbInfo(db, "\nProgram exited normally.");
		}
		else
		{
			ILDbInfo(db, "\nProgram exited with code %02ld.", (long)retval);
		}
	}
	else
	{
		exception = ILExecThreadGetException(thread);
		ILExecThreadClearException(thread);
		argString = ILObjectToString(thread, exception);
		if(argString != 0 &&
		   (exString = ILStringToAnsi(thread, argString)) != 0)
		{
			ILDbError(db, "\nProgram exited with uncaught exception: %s",
					  exString);
		}
		else
		{
			ILDbOutOfMemory(db);
		}
	}

	/* Destroy the execution process and then reload it in non-run mode */
	db->running = 0;
	ILExecProcessDestroy(db->process);
	db->process = 0;
	ILGCCollect();
	ILDbCreateProcess(db);

	/* Turn readline input handling back on again */
	ILDbInputEnable();
}

/*
 * Print or specify the program to be debugged.
 */
static void FileCmd(ILDb *db, char *argv[])
{
	char *file;

	/* If there are no arguments, then print the current executable */
	if(!(argv[0]))
	{
		if(db->debugProgram)
		{
			ILDbInfo(db, "Executable file is %s.", db->debugProgram);
		}
		else
		{
			ILDbInfo(db, "No executable file now.");
		}
		return;
	}

	/* Cannot do this if a program is running */
	if(db->running)
	{
		ILDbError(db, "Cannot change executables while a program is running.");
		return;
	}

	/* Expand the filename */
	file = ILExpandFilename(argv[0], (char *)0);
	if(!file)
	{
		ILDbOutOfMemory(db);
	}

	/* Destroy the current process and reload */
	if(db->process)
	{
		ILExecProcessDestroy(db->process);
		db->process = 0;
	}
	if(db->debugProgram)
	{
		ILFree(db->debugProgram);
	}
	ILDbFreeStringArray(db->args, -1);
	db->args = 0;
	db->debugProgram = file;
	ILDbCreateProcess(db);
}

/*
 * Table of "run" commands.
 */
ILDbCmdInfo ILDbRunCommands[] = {
	{"run", 1, 0, 0, RunCmd, 0,
		"run the application that is to be debugged",
		0},
	{"file", 3, 0, 0, FileCmd, 0,
		"specific the application that is to be debugged",
		0},
};
int ILDbNumRunCommands = (sizeof(ILDbRunCommands) / sizeof(ILDbCmdInfo));

#ifdef	__cplusplus
};
#endif
