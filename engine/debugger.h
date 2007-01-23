/*
 * debugger.h - debugger definitions.
 *
 * Copyright (C) 2005  Southern Storm Software, Pty Ltd.
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

#ifndef	_DEBUGGER_H
#define	_DEBUGGER_H

#include "il_debugger.h"
#ifdef IL_USE_JIT
#include "jit/jit-except.h"
#endif

#ifdef IL_CONFIG_DEBUGGER

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Run types.
 */
#define	IL_DEBUGGER_RUN_TYPE_STOPPED		0
#define	IL_DEBUGGER_RUN_TYPE_CONTINUE		1
#define	IL_DEBUGGER_RUN_TYPE_STEP			2
#define	IL_DEBUGGER_RUN_TYPE_NEXT			3
#define	IL_DEBUGGER_RUN_TYPE_FINISH			4
#define	IL_DEBUGGER_RUN_TYPE_DETACHED		5
#define	IL_DEBUGGER_RUN_TYPE_UNTIL			6

#define IL_DEBUGGER_COMMAND_MAX_ARG_COUNT	3


/*
 * Check whether debugger is watching method's assembly.
 */
int ILDebuggerIsAssemblyWatched(ILDebugger *debugger, ILMethod *method);

/*
 * Information about a thread that is under the control of the debugger.
 */
struct _tagILDebuggerThreadInfo
{
	/* User id used to identify this thread */
	int id;

	/* Info from hook function */
	ILExecProcess *process;
	ILExecThread *execThread;
	void *userData;
	ILMethod *method;
	ILInt32 offset;
	int type;

	/* Current location in source file */
	const char *sourceFile;
	ILUInt32 line;
	ILUInt32 col;

	/* Stack trace height */
	ILInt32 numFrames;

#ifdef IL_USE_JIT
	/* JIT stack trace */
	jit_stack_trace_t jitStackTrace;
#endif

	int volatile runType;
	ILDebuggerThreadInfo *next;
};

/*
 * Debugger breakpoint placed by user.
 */
struct _tagILBreakpoint
{
	int id;
	ILMethod *method;
	ILUInt32 offset;
	const char *sourceFile;
	ILUInt32 line;
	ILUInt32 col;
	ILBreakpoint *next;
};

/*
 * Information about watched assembly (image).
 */
struct _tagILAssemblyWatch
{
	ILImage *image;

	/* Watch count - 0 if watch is disabled */
	int count;

	ILAssemblyWatch *next;
};

/*
 * Structure of a debugger instance.
 */
struct _tagILDebugger
{
	/* Lock to serialize access to this object */
	ILMutex *lock;

	/* Refernce to current exec process */
	ILExecProcess *process;

	/* Debugger client stream */
	ILDebuggerIO *io;

	/* Thread with command loop or 0 if debugging runs in single thread */
	ILThread *commandThread;

	/* Currently debugged thread followed by other stopped threads */
	ILDebuggerThreadInfo * volatile dbthread;

	/* Linked list of watched assemblies or NULL to watch all */
	ILAssemblyWatch *assemblyWatches;
	
	/* Linked list of user breakpoints */
	ILBreakpoint *breakpoint;

	/* Flag used to stop all threads after break command */
	int breakAll;

	/* Return IL_HOOK_ABORT from hook function when this flag is set */ 
	int volatile abort;

	/* Arguments for current command */
	char *args[IL_DEBUGGER_COMMAND_MAX_ARG_COUNT];
	int argCount;

};


#ifdef	__cplusplus
};
#endif

#endif	/* IL_CONFIG_DEBUGGER */

#endif	/* _DEBUGGER_H */
