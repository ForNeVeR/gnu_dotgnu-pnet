/*
 * engine.h - Core definitions for the runtime engine.
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

#ifndef	_ENGINE_ENGINE_H
#define	_ENGINE_ENGINE_H

#include "il_engine.h"
#include "il_system.h"
#include "il_program.h"
#include "heap.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Default values.
 */
#ifndef	IL_ENGINE_STACK_SIZE
#define	IL_ENGINE_STACK_SIZE	1024
#endif

/*
 * Execution control context for a process.
 */
struct _tagILExecProcess
{
	/* The heap that is shared between all threads in this process */
	ILHeap			heap;

	/* List of threads that are active within this process */
	ILExecThread   *firstThread;

	/* The "main" thread for the process */
	ILExecThread   *mainThread;

	/* Default stack size for new threads */
	ILUInt32	  	stackSize;

	/* Context that holds all images that have been loaded by this process */
	ILContext 	   *context;

	/* Exit status if the process executes something like "exit(N)" */
	int 			exitStatus;

};

/*
 * Information that is stored in a stack call frame.
 * Offsets are used to refer to the stack instead of
 * pointers.  This allows the stack to be realloc'ed
 * without having to rearrange the saved frame data.
 */
typedef struct _tagILCallFrame
{
	unsigned char  *pc;			/* PC to return to */
	ILUInt32	   	stackTop;	/* Position to unwind the stack to */
	ILUInt32		args;		/* Argument position for caller */
	ILUInt32		locals;		/* Local variables for caller */
	ILMethod       *method;		/* Method being executed in the frame */
	ILUInt32		parent;		/* Position of the parent frame */

} ILCallFrame;

/*
 * Execution control context for a single thread.
 */
struct _tagILExecThread
{
	/* Back-pointer to the process this thread belongs to */
	ILExecProcess  *process;

	/* Sibling threads */
	ILExecThread   *nextThread;
	ILExecThread   *prevThread;

	/* Extent of the execution stack */
	ILValue		   *stackBase;
	ILValue		   *stackLimit;

	/* Current thread state */
	unsigned char  *pc;
	ILValue		   *stackTop;
	ILValue		   *args;
	ILValue		   *locals;
	ILMethod       *method;
	ILCallFrame	   *frame;

};

/*
 * Private information that is associated with a class.
 */
typedef struct _tagILClassPrivate ILClassPrivate;
struct _tagILClassPrivate
{
	ILUInt32		size;				/* Full instance size */
	ILUInt32		inLayout : 1;		/* Non-zero if in layout algorithm */
	ILUInt32		alignment : 15;		/* Preferred instance alignment */
	ILUInt32		vtableSize : 16;	/* Size of the vtable */
	ILMethod      **vtable;				/* Methods within the vtable */

};

/*
 * Execute the interpreter on a thread.
 */
void _ILInterpreter(ILExecThread *thread);

/*
 * Push a new call frame for a method call.
 * Returns zero if out of memory.
 */
int _ILExecThreadFramePush(ILExecThread *thread, ILMethod *method);

/*
 * Pop the current call frame and return to the
 * previous call level.  If the method is returning
 * a value, it will be copied to the top of the
 * stack in the previous frame.
 */
void _ILExecThreadFramePop(ILExecThread *thread);

/*
 * Lay out a class's fields, virtual methods, and interfaces.
 * Returns zero if there is something wrong with the definition.
 */
int _ILLayoutClass(ILExecThread *thread, ILClass *info);

/*
 * Security check types.
 */
#define	IL_SECURITY_LAYOUT			1

/*
 * Perform a security check on a program item.
 */
int _ILSecurityCheck(ILExecThread *thread, ILProgramItem *info, int type);

#ifdef	__cplusplus
};
#endif

#endif	/* _ENGINE_ENGINE_H */
