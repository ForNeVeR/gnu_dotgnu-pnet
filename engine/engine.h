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
#include "il_coder.h"
#include "heap.h"
#include "cvm.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Determine if this compiler supports inline functions.
 */
#ifdef __GNUC__
	#define	IL_INLINE	__inline__
#else
	#define	IL_INLINE
#endif

/*
 * Default values.
 */
#ifndef	IL_ENGINE_STACK_SIZE
#define	IL_ENGINE_STACK_SIZE		1024
#endif
#ifndef	IL_ENGINE_FRAME_STACK_SIZE
#define	IL_ENGINE_FRAME_STACK_SIZE	64
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
	ILUInt32	  	frameStackSize;

	/* Context that holds all images that have been loaded by this process */
	ILContext 	   *context;

	/* Exit status if the process executes something like "exit(N)" */
	int 			exitStatus;

};

/*
 * Information that is stored in a stack call frame.
 * Offsets are used to refer to the stack and program
 * instead of pointers.  This allows the stack to be
 * realloc'ed, or the coder cache re-generated, without
 * having to rearrange the saved frame data.
 */
typedef struct _tagILCallFrame
{
	ILMethod       *method;		/* Method being executed in the frame */
	ILUInt32		pc;			/* PC to return to in the parent method */
	ILUInt32	   	frame;		/* Base of the local variable frame */
	ILUInt32		except;		/* PC to jump to on an exception */

} ILCallFrame;

/*
 * Information that is attached to a method to hold
 * runtime engine data.
 */
typedef struct _tagILCallInfo
{
	ILUInt32		generation;	/* Code conversion generation count */
	unsigned char  *pcstart;	/* Start of the method's code */

} ILCallInfo;

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
	CVMWord		   *stackBase;
	CVMWord		   *stackLimit;

	/* Current thread state */
	unsigned char  *pcstart;	/* Start of the CVM code for the method */
	ILUInt32		pc;			/* Offset to the current position */
	ILUInt32		frame;		/* Base of the local variable frame */
	CVMWord        *stackTop;	/* Current stack top */
	ILMethod       *method;		/* Current method being executed */

	/* Last exception that was thrown */
	ILObject       *thrownException;

	/* Stack of call frames in use */
	ILCallFrame	   *frameStack;
	ILUInt32		numFrames;
	ILUInt32		maxFrames;

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
 * Execute the CVM interpreter on a thread.  Returns zero for
 * a regular return, or non-zero if an exception was thrown.
 */
int _ILCVMInterpreter(ILExecThread *thread);

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

/*
 * Verify the contents of a method.
 */
int _ILVerify(ILCoder *coder, ILMethod *method,
			  ILMethodCode *code, int unsafeAllowed);

#ifdef	__cplusplus
};
#endif

#endif	/* _ENGINE_ENGINE_H */
