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
#include "il_thread.h"
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
#ifndef	IL_CONFIG_STACK_SIZE
#define	IL_CONFIG_STACK_SIZE		8192
#endif
#ifndef	IL_CONFIG_FRAME_STACK_SIZE
#define	IL_CONFIG_FRAME_STACK_SIZE	512
#endif

/*
 * Determine if we should use interface method tables.
 */
#ifndef IL_CONFIG_REDUCE_CODE
	#define	IL_USE_IMTS	1
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
 * Structure of a breakpoint watch registration.
 */
typedef struct _tagILExecDebugWatch ILExecDebugWatch;
struct _tagILExecDebugWatch
{
	ILMethod		   *method;
	ILUInt32			count;
	ILExecDebugWatch   *next;

};

/*
 *	Passed to objects when they are finalized.  Objects being finalized should
 * verify the information in the context valid before continuuing finalization
 * because it is possible for objects to be finalized *long* after their owner
 * process has been destroyed.
 */
typedef struct __tagILFinalizationContext ILFinalizationContext;
struct __tagILFinalizationContext
{
	/* The process that created the object and registered the finalizer or 0
	    if that process has been destroyed. */
	ILExecProcess *volatile process;
};

/*
 * Execution control context for a process.
 */
struct _tagILExecProcess
{
	/* Lock to serialize access to this object */
	ILMutex        *lock;

	/* List of threads that are active within this process */
	ILExecThread   *firstThread;

	/* The "main" thread for the process */
	ILExecThread   *mainThread;

	/* The finalizer thread for the process */
	ILExecThread   *finalizerThread;

	/* Default stack size for new threads */
	ILUInt32	  	stackSize;
	ILUInt32	  	frameStackSize;

	/* Context that holds all images that have been loaded by this process */
	ILContext 	   *context;

	/* Read/write lock for the metadata in "context" */
	ILRWLock       *metadataLock;

	/* Exit status if the process executes something like "exit(N)" */
	int 			exitStatus;

	/* The coder in use by this process */
	ILCoder		   *coder;

	/* Useful builtin classes */
	ILClass        *objectClass;
	ILClass        *stringClass;
	ILClass        *exceptionClass;
	ILClass        *clrTypeClass;
	ILClass		 *threadAbortClass;

	/* The object to throw when the system runs out of memory */
	ILObject	   *outOfMemoryObject;

	/* The command-line argument values */
	ILObject       *commandLineObject;

	/* The time when the engine was started */
	ILCurrTime		startTime;

	/* Hash table that contains all intern'ed strings within the system */
	void		   *internHash;

#ifdef USE_HASHING_MONITORS
	/* Hash table that contains all monitors */
	void			*monitorHash;
	ILMutex		*monitorSystemLock;
#endif

	/* Finalization context used by this process */
	ILFinalizationContext *finalizationContext;

	/* Hash table that maps program items to reflection objects */
	void		   *reflectionHash;

	/* List of loaded modules for PInvoke methods */
	ILLoadedModule *loadedModules;

	/* List of GC handles */
	struct _tagILGCHandleTable *gcHandles;

	/* The image that contains the program entry point */
	ILImage		   *entryImage;

	/* The custom internal call table which is runtime settable */
	ILEngineInternalClassList* internalClassTable;

#ifdef IL_CONFIG_DEBUG_LINES

	/* Breakpoint debug information */
	ILExecDebugHookFunc debugHookFunc;
	void               *debugHookData;
	ILExecDebugWatch   *debugWatchList;
	int					debugWatchAll;

#endif /* IL_CONFIG_DEBUG_LINES */

	/* Cryptographic seed material */
	ILMutex        	   *randomLock;
	int					randomBytesDelivered;
	ILInt64				randomLastTime;
	unsigned char		randomPool[20];
	int					randomCount;

	/* Size of the global thread-static allocation */
	ILUInt32			numThreadStaticSlots;

#ifdef IL_USE_IMTS

	/* Last-allocated base identifier for interface method tables */
	ILUInt32			imtBase;

#endif

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
	CVMWord		   *frame;			/* Base of the local variable frame */
	CVMWord		   *exceptHeight;	/* Height of the frame for exceptions */
	void           *permissions;	/* Permissions for this stack frame */

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

	/* Support thread object */
	ILThread       *supportThread;

	/* Extent of the execution stack */
	CVMWord		   *stackBase;
	CVMWord		   *stackLimit;

	/* Current thread state */
	unsigned char  *pc;				/* Current program position */
	CVMWord		   *exceptHeight;	/* Height of the frame for exceptions */
	CVMWord		   *frame;			/* Base of the local variable frame */
	CVMWord        *stackTop;		/* Current stack top */
	ILMethod       *method;			/* Current method being executed */
	
	/* Last exception that was thrown */
	ILObject       *thrownException;

	/* Indicates if an abort is in progress */
	int				aborting;

	/* Security manager in use by this thread */
	ILObject	   *securityManager;

	/* System.Threading.Thread object */
	ILObject *clrThread;

	/* Marks this thread as a finalizer thread */
	/* Finalizer threads are destroyed last */
	int isFinalizerThread;

	/* Free monitors list */
	ILExecMonitor *freeMonitor;

	/* Stack of call frames in use */
	ILCallFrame	   *frameStack;
	ILUInt32		numFrames;
	ILUInt32		maxFrames;

	/* Thread-static values for this thread */
	void		  **threadStaticSlots;
	ILUInt32		threadStaticSlotsUsed;

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
 * Define the size of the interface method table.
 */
#ifdef	IL_USE_IMTS
#define	IL_IMT_SIZE		32
#endif

/*
 * Private information that is associated with a class.
 */
typedef struct _tagILClassPrivate ILClassPrivate;
struct _tagILClassPrivate
{
	ILClass		   *classInfo;			/* Back-pointer to the class */
	ILUInt32		size;				/* Full instance size */
	ILUInt32		nativeSize;			/* Full native instance size */
	ILUInt32		staticSize;			/* Size of static data */
	ILUInt32		inLayout : 1;		/* Non-zero if in layout algorithm */
	ILUInt32		hasFinalizer : 1;	/* Non-zero if non-trivial finalizer */
	ILUInt32		managedInstance : 1;/* Non-zero if managed instance field */
	ILUInt32		managedStatic : 1;	/* Non-zero if managed static field */
	ILUInt32		alignment : 6;		/* Preferred instance alignment */
	ILUInt32		nativeAlignment : 6;/* Preferred native alignment */
	ILUInt32		vtableSize : 16;	/* Size of the vtable */
	ILMethod      **vtable;				/* Methods within the vtable */
	ILObject       *clrType;			/* Associated CLR type object */
	ILObject       *staticData;			/* Static data area object */
	ILImplPrivate  *implements;			/* Interface implementation records */
#ifdef IL_USE_IMTS
	ILUInt32		imtBase;			/* Base for IMT identifiers */
	ILMethod	   *imt[IL_IMT_SIZE];	/* Interface method table */
#endif

};

typedef void * ILLockWord;

struct _tagILExecMonitor
{
	ILWaitHandle *supportMonitor;
	volatile ILInt32 waiters;
	ILExecMonitor *next;
};

/*
*	Header of an object.
*/
typedef struct _tagObjectHeader ILObjectHeader;

struct _tagObjectHeader
{
	ILClassPrivate *classPrivate;
#ifdef USE_HASHING_MONITORS
	/* NOTHING */
#else
	ILLockWord lockWord;
#endif
};

/*
 * Class information for the CVM coder.
 */
extern ILCoderClass const _ILCVMCoderClass;

/*
 * Pack parameters onto the CVM stack for a call, using a "va_list"
 * as the source of the values.
 */
int _ILCallPackVaParams(ILExecThread *thread, ILMethod *method,
					    int isCtor, void *_this, void *userData);

/*
 * Pack parameters onto the CVM stack for a call, using an array
 * of ILExecValue values to supply the parameters.
 */
int _ILCallPackVParams(ILExecThread *thread, ILMethod *method,
					   int isCtor, void *_this, void *userData);

/*
 * Unpack a method result from the CVM stack and store it into
 * a direct result buffer.
 */
void _ILCallUnpackDirectResult(ILExecThread *thread, ILMethod *method,
					           int isCtor, void *result, void *userData);

/*
 * Unpack a method result from the CVM stack and store it into
 * an ILExecValue result buffer.
 */
void _ILCallUnpackVResult(ILExecThread *thread, ILMethod *method,
				          int isCtor, void *result, void *userData);

/*
 * Prototype for a parameter packing function.
 */
typedef int (*ILCallPackFunc)(ILExecThread *thread, ILMethod *method,
					          int isCtor, void *_this, void *userData);

/*
 * Prototype for a return value unpacking function.
 */
typedef void (*ILCallUnpackFunc)(ILExecThread *thread, ILMethod *method,
					             int isCtor, void *result, void *userData);

/*
 * Call a method using the supplied packing and unpacking rules.
 */
int _ILCallMethod(ILExecThread *thread, ILMethod *method,
				  ILCallUnpackFunc unpack, void *result,
				  int isCtor, void *_this,
				  ILCallPackFunc pack, void *userData);

/*
 * Execute the CVM interpreter on a thread.  Returns zero for
 * a regular return, or non-zero if an exception was thrown.
 */
int _ILCVMInterpreter(ILExecThread *thread);

/*
 * Inner version of "ILExecThreadCreate".  The caller is
 * responsible for creating the OS-level thread.
 */
ILExecThread *_ILExecThreadCreate(ILExecProcess *process);

/*
 *	Destroy an engine level thread.
 */
void _ILExecThreadDestroy(ILExecThread *thread);

/*
 * Lay out a class's fields, virtual methods, and interfaces.
 * Returns zero if there is something wrong with the definition.
 */
int _ILLayoutClass(ILClass *info);

/*
 * Lay out a class and return its size and alignment.
 */
ILUInt32 _ILLayoutClassReturn(ILClass *info, ILUInt32 *alignment);

/*
 * Determine if layout of a class has already been done.
 */
int _ILLayoutAlreadyDone(ILClass *info);

/*
 * Verify the contents of a method.
 */
int _ILVerify(ILCoder *coder, unsigned char **start, ILMethod *method,
			  ILMethodCode *code, int unsafeAllowed, ILExecThread *thread);

/*
 * Construct the "ffi_cif" structure that is needed to
 * call a PInvoke or "internalcall" method.  Returns NULL
 * if insufficient memory for the structure.
 */
void *_ILMakeCifForMethod(ILMethod *method, int isInternal);

/*
 * Construct the "ffi_cif" structure that is needed to
 * call a PInvoke or "internalcall" constructor in
 * allocation mode.  Returns NULL if insufficient memory
 * for the structure.
 */
void *_ILMakeCifForConstructor(ILMethod *method, int isInternal);

/*
 * Make a native closure for a particular delegate.  "method"
 * is the method within the delegate object.
 */
void *_ILMakeClosureForDelegate(ILObject *delegate, ILMethod *method);

/*
 * Convert a method into executable code.  Returns a pointer
 * to the method entry point or NULL if something is wrong.
 */
unsigned char *_ILConvertMethod(ILExecThread *thread, ILMethod *method);

/*
 * Finalization callback that is invoked by the garbage collector.
 */
void _ILFinalizeObject(void *block, void *data);

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
 * Information that is returned for an internalcall method.
 * The "marshal" value is ignored for libffi-capable systems.
 */
typedef struct
{
	void *func;
	void *marshal;

} ILInternalInfo;

/*
 * Find the function for an "internalcall" method.
 * Returns zero if there is no function information.
 */
int _ILFindInternalCall(ILExecProcess* process, ILMethod *method, 
						int ctorAlloc, ILInternalInfo *info);

/*
 * Find internalcall information for an array method.
 */
int _ILGetInternalArray(ILMethod *method, int *isCtor, ILInternalInfo *info);

/*
 * Find internalcall information for a delegate method.
 */
int _ILGetInternalDelegate(ILMethod *method, int *isCtor,  ILInternalInfo *info);

/*
 * Look up an interface method.  Returns NULL if not found.
 */
ILMethod *_ILLookupInterfaceMethod(ILClassPrivate *objectClassPrivate,
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
 * Intern a string from a field constant within an image.
 * Returns NULL if an exception was thrown.
 */
ILString *_ILStringInternFromConstant(ILExecThread *thread, void *data,
									  unsigned long numChars);

/*
 * Convert a string into a buffer of characters for direct access.
 * This is faster than calling "ToCharArray()", but should only
 * be used inside the engine.  Returns the length.
 */
ILInt32 _ILStringToBuffer(ILExecThread *thread, ILString *str, ILUInt16 **buf);

#ifndef REDUCED_STDIO

/*
 * Dump a CVM instruction starting at a particular address.
 * Returns the size of the instruction.
 */
int _ILDumpCVMInsn(FILE *stream, ILMethod *currMethod, unsigned char *pc);

#endif

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

/*
 * Initialize the CVM native code unrolling library.  Returns
 * zero if insufficient memory available.
 */
int _ILCVMUnrollInit(void);

/*
 * Determine if CVM native code unrolling is possible.
 */
int _ILCVMUnrollPossible(void);

/*
 * Unroll a CVM method to native code.
 */
int _ILCVMUnrollMethod(ILCoder *coder, unsigned char *pc, ILMethod *method);

/*
 * Unroll a CVM method to native code - locked version.
 */
int _ILUnrollMethod(ILExecThread *thread, ILCoder *coder,
					unsigned char *pc, ILMethod *method);

/*
 * Determine the size of a type's values in bytes.  This assumes
 * that the caller has obtained the metadata write lock.
 */
ILUInt32 _ILSizeOfTypeLocked(ILType *type);

/*
 * Get the native closure associated with a delegate.  Returns NULL
 * if the closure could not be created for some reason.
 */
void *_ILDelegateGetClosure(ILObject *delegate);

/*
 * Get the number of parameter words for a method.
 */
ILUInt32 _ILGetMethodParamCount(ILExecThread *thread, ILMethod *method,
								int suppressThis);

/*
 * Lock metadata for reading or writing from the current thread.
 */
#define	IL_METADATA_WRLOCK(thread)	\
			ILRWLockWriteLock((thread)->process->metadataLock)
#define	IL_METADATA_RDLOCK(thread)	\
			ILRWLockReadLock((thread)->process->metadataLock)

/*
 * Unlock metadata from the current thread.
 */
#define	IL_METADATA_UNLOCK(thread)	\
			ILRWLockUnlock((thread)->process->metadataLock)

#ifdef IL_CONFIG_DEBUG_LINES

/*
 * Determine if the current method is on the breakpoint watch list.
 */
int _ILIsBreak(ILExecThread *thread, ILMethod *method);

/*
 * Break out to the debugger for a specific type of breakpoint.
 */
void _ILBreak(ILExecThread *thread, int type);

#endif /* IL_CONFIG_DEBUG_LINES */

/*
 * Perform custom marshalling to convert an object reference
 * into a native pointer.
 */
void *_ILObjectToCustom(ILExecThread *thread, ILObject *obj,
						const char *customName, int customNameLen,
						const char *customCookie, int customCookieLen);

/*
 * Perform custom marshalling to convert a native pointer
 * into an object reference.
 */
ILObject *_ILCustomToObject(ILExecThread *thread, void *ptr,
							const char *customName, int customNameLen,
							const char *customCookie, int customCookieLen);

/*
 *	Gets the current managed thread object from an engine thread.
 */
ILObject *_ILGetCurrentClrThread(ILExecThread *thread);

/*
 *	Makes the given support thread execute in the context of the given engine thread.
 */
void _ILThreadExecuteOn(ILThread *thread, ILExecThread *execThread);

/*
 *	Throws a thread abort exception on the given thread.
 */
void _ILExecThreadThrowThreadAbortException(ILExecThread *thread, ILObject *stateInfo);

/*
 *	Aborts the current thread.
 */
void _ILAbortThread(ILExecThread *thread);

/*
 *	Handles thread aborts & interruption.
 */
void _ILHandleWaitResult(ILExecThread *thread, int result);

/*
 *	Creates a monitor used by the execution engine.
 */
ILExecMonitor *ILExecMonitorCreate(void);

/*
 *	Destroys a monitor used by the execution engine.
 */
void ILExecMonitorDestroy(ILExecMonitor *monitor);

#ifdef	__cplusplus
};
#endif

#endif	/* _ENGINE_ENGINE_H */
