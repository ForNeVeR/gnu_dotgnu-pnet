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
#include "il_align.h"
#include "il_gc.h"
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
#define	IL_ENGINE_STACK_SIZE		8192
#endif
#ifndef	IL_ENGINE_FRAME_STACK_SIZE
#define	IL_ENGINE_FRAME_STACK_SIZE	512
#endif

/*
 * Structure that keeps track of a loaded external module.
 */
typedef struct _tagILLoadedModule ILLoadedModule;
struct _tagILLoadedModule
{
	ILLoadedModule *next;
	void		   *handle;
	char			name[1];

};

/*
 * Execution control context for a process.
 */
struct _tagILExecProcess
{
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

	/* The coder in use by this process */
	ILCoder		   *coder;

	/* Useful builtin classes */
	ILClass        *stringClass;
	ILClass        *exceptionClass;
	ILClass        *clrTypeClass;

	/* The object to throw when the system runs out of memory */
	ILObject	   *outOfMemoryObject;

	/* The time when the engine was started */
	ILCurrTime		startTime;

	/* Hash table that contains all intern'ed strings within the system */
	void		   *internHash;

	/* Hash table that maps program items to reflection objects */
	void		   *reflectionHash;

	/* List of loaded modules for PInvoke methods */
	ILLoadedModule *loadedModules;

	/* The image that contains the program entry point */
	ILImage		   *entryImage;

};

/*
 * Information that is stored in a stack call frame.
 * Offsets are used to refer to the stack instead of
 * pointers.  This allows the stack to be realloc'ed,
 * without having to rearrange the saved frame data.
 */
typedef struct _tagILCallFrame
{
	ILMethod       *method;			/* Method being executed in the frame */
	unsigned char  *pc;				/* PC to return to in the parent method */
	ILUInt32	   	frame;			/* Base of the local variable frame */
	ILUInt32		exceptHeight;	/* Height of the frame for exceptions */

} ILCallFrame;
#define	IL_INVALID_PC		((unsigned char *)(ILNativeInt)(-1))

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
	unsigned char  *pc;				/* Current program position */
	ILUInt32		exceptHeight;	/* Height of the frame for exceptions */
	ILUInt32		frame;			/* Base of the local variable frame */
	CVMWord        *stackTop;		/* Current stack top */
	ILMethod       *method;			/* Current method being executed */

	/* Last exception that was thrown */
	ILObject       *thrownException;

	/* Security manager in use by this thread */
	ILObject	   *securityManager;

	/* Stack of call frames in use */
	ILCallFrame	   *frameStack;
	ILUInt32		numFrames;
	ILUInt32		maxFrames;

};

/*
 * Details that are stored for an interface implementation record.
 */
typedef struct _tagILImplPrivate ILImplPrivate;
struct _tagILImplPrivate
{
	ILClass		   *interface;			/* Name of the interface */
	ILImplPrivate  *next;				/* Next interface for the class */

};
#define	ILImplPrivate_Table(priv)	((ILUInt16 *)((priv) + 1))

/*
 * Private information that is associated with a class.
 */
typedef struct _tagILClassPrivate ILClassPrivate;
struct _tagILClassPrivate
{
	ILUInt32		size;				/* Full instance size */
	ILUInt32		staticSize;			/* Size of static data */
	ILUInt32		inLayout : 1;		/* Non-zero if in layout algorithm */
	ILUInt32		hasFinalizer : 1;	/* Non-zero if non-trivial finalizer */
	ILUInt32		alignment : 14;		/* Preferred instance alignment */
	ILUInt32		vtableSize : 16;	/* Size of the vtable */
	ILMethod      **vtable;				/* Methods within the vtable */
	ILObject       *clrType;			/* Associated CLR type object */
	ILObject       *staticData;			/* Static data area object */
	ILImplPrivate  *implements;			/* Interface implementation records */

};

/*
 * Structure of an "internalcall" method table entry.
 */
typedef struct
{
	const char	   *methodName;
	const char	   *signature;
	void           *func;

} ILMethodTableEntry;

/*
 * Helper macros for defining "internalcall" method tables.
 */
#define	IL_METHOD_BEGIN(name)	\
			static ILMethodTableEntry const name[] = {
#define	IL_METHOD(name,sig,func)	\
			{(name), (sig), (void *)(func)},
#define	IL_CONSTRUCTOR(name,sig,func,allocFunc)	\
			{(name), (sig), (void *)(func)}, \
			{(name), 0, (void *)(allocFunc)},
#define	IL_METHOD_END			\
			{0, 0, 0}};

/*
 * Class information for the CVM coder.
 */
extern ILCoderClass const _ILCVMCoderClass;

/*
 * Execute the CVM interpreter on a thread.  Returns zero for
 * a regular return, or non-zero if an exception was thrown.
 */
int _ILCVMInterpreter(ILExecThread *thread);

/*
 * Lay out a class's fields, virtual methods, and interfaces.
 * Returns zero if there is something wrong with the definition.
 */
int _ILLayoutClass(ILClass *info);

/*
 * Security check types.
 */
#define	IL_SECURITY_LAYOUT			1

/*
 * Perform a security check on a program item.
 */
int _ILSecurityCheck(ILProgramItem *info, int type);

/*
 * Verify the contents of a method.
 */
int _ILVerify(ILCoder *coder, unsigned char **start, ILMethod *method,
			  ILMethodCode *code, int unsafeAllowed);

/*
 * Construct the "ffi_cif" structure that is needed to
 * call a PInvoke or "internalcall" method.  Returns NULL
 * if insufficient memory for the structure.
 */
void *_ILMakeCifForMethod(ILCoder *coder, ILMethod *method, int isInternal);

/*
 * Construct the "ffi_cif" structure that is needed to
 * call a PInvoke or "internalcall" constructor in
 * allocation mode.  Returns NULL if insufficient memory
 * for the structure.
 */
void *_ILMakeCifForConstructor(ILCoder *coder, ILMethod *method,
							   int isInternal);

/*
 * Convert a method into executable code.  Returns a pointer
 * to the method entry point or NULL if something is wrong.
 */
unsigned char *_ILConvertMethod(ILExecThread *thread, ILMethod *method);

/*
 * Allocate a block of memory and associate it with a specific class.
 * This will throw an exception if out of memory, and return zero.
 */
ILObject *_ILEngineAlloc(ILExecThread *thread, ILClass *classInfo,
						 ILUInt32 size);

/*
 * Allocate a block of memory that is guaranteed never to contain
 * pointers to other objects, and to never need finalization.
 */
ILObject *_ILEngineAllocAtomic(ILExecThread *thread, ILClass *classInfo,
							   ILUInt32 size);

/*
 * Allocate a block of memory for a specific class.  Get the size
 * from the class information block.
 */
ILObject *_ILEngineAllocObject(ILExecThread *thread, ILClass *classInfo);

/*
 * Find the function for an "internalcall" method.
 */
void *_ILFindInternalCall(ILMethod *method, int ctorAlloc);

/*
 * Look up an interface method.  Returns NULL if not found.
 */
ILMethod *_ILLookupInterfaceMethod(ILClass *objectClass,
								   ILClass *interfaceClass,
								   ILUInt32 index);

/*
 * Match a type against a lookup signature value.
 */
int _ILLookupTypeMatch(ILType *type, const char *signature);

/*
 * Intern a string from a constant within an image.
 * Returns NULL if an exception was thrown.
 */
ILString *_ILStringInternFromImage(ILExecThread *thread, ILImage *image,
								   ILToken token);

/*
 * Convert a string into a buffer of characters for direct access.
 * This is faster than calling "ToCharArray()", but should only
 * be used inside the engine.  Returns the length.
 */
ILInt32 _ILStringToBuffer(ILExecThread *thread, ILString *str, ILUInt16 **buf);

/*
 * Dump a CVM instruction starting at a particular address.
 * Returns the size of the instruction.
 */
int _ILDumpCVMInsn(FILE *stream, ILMethod *currMethod, unsigned char *pc);

/*
 * Get a call frame which is a specific number of frames up the stack,
 * not counting the active frame.  Returns NULL if "n" is invalid.
 */
ILCallFrame *_ILGetCallFrame(ILExecThread *thread, ILInt32 n);

/*
 * Get the next higher call frame after "frame".  Returns NULL
 * if "frame" is the top-most frame on the stack.
 */
ILCallFrame *_ILGetNextCallFrame(ILExecThread *thread, ILCallFrame *frame);

/*
 * Reallocate the call frames for a given thread in order
 * to provide more space.  Returns NULL if out of memory.
 */
ILCallFrame *_ILAllocCallFrame(ILExecThread *thread);

/*
 * Create a system exception object of a particular class.
 * We do this very carefully, to avoid re-entering the engine.
 * We cannot call the exception's constructor, so we do the
 * equivalent work here instead.
 */
void *_ILSystemException(ILExecThread *thread, const char *className);

#ifdef	__cplusplus
};
#endif

#endif	/* _ENGINE_ENGINE_H */
