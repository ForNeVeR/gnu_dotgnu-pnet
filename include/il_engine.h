/*
 * il_engine.h - Interface to the runtime engine.
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

#ifndef	_IL_ENGINE_H
#define	_IL_ENGINE_H

#include "il_program.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Execution control context for a process.
 */
typedef struct _tagILExecProcess ILExecProcess;

/*
 * Execution control context for a single thread.
 */
typedef struct _tagILExecThread ILExecThread;

/*
 * Object handle.
 */
typedef struct _tagILObject ILObject;

/*
 * Types that are useful for passing values through va lists.
 */
#if SIZEOF_INT > 4
typedef int				ILVaInt;
typedef unsigned int	ILVaUInt;
#else
typedef ILInt32			ILVaInt;
typedef ILUInt32		ILVaUInt;
#endif
typedef double			ILVaDouble;

/*
 * The structure that is used to pass typed references around.
 */
typedef struct
{
	void *type;
	void *value;

} ILTypedRef;

/*
 * Create a new process, including the "main" thread.
 */
ILExecProcess *ILExecProcessCreate(void);

/*
 * Destroy a process and all threads associated with it.
 */
void ILExecProcessDestroy(ILExecProcess *process);

/*
 * Get the "main" thread for a process.
 */
ILExecThread *ILExecProcessGetMain(ILExecProcess *process);

/*
 * Load an image into a process.  Returns 0 if OK, or
 * an image load error code otherwise (see "il_image.h").
 */
int ILExecProcessLoadImage(ILExecProcess *process, FILE *file);

/*
 * Load the contents of a file as an image into a process.
 * Returns 0 if OK, -1 if the file could not be opened,
 * or an image load error code otherwise.
 */
int ILExecProcessLoadFile(ILExecProcess *process, const char *filename);

/*
 * Get the exit status for a process.
 */
int ILExecProcessGetStatus(ILExecProcess *process);

/*
 * Get the entry point method for a process.
 */
ILMethod *ILExecProcessGetEntry(ILExecProcess *process);

/*
 * Create a new thread and attach it to a process.
 * Returns NULL if out of memory.
 */
ILExecThread *ILExecThreadCreate(ILExecProcess *process);

/*
 * Destroy a thread and detach it from its process.
 */
void ILExecThreadDestroy(ILExecThread *thread);

/*
 * Get the process that corresponds to a thread.
 */
ILExecProcess *ILExecThreadGetProcess(ILExecThread *thread);

/*
 * Get the method that is executing "num" steps up
 * the thread stack.  Zero indicates the currently
 * executing method.  Returns NULL if "num" is invalid.
 */
ILMethod *ILExecThreadStackMethod(ILExecThread *thread, unsigned long num);

/*
 * Determine the size of a type's values in bytes.
 * Returns zero if there is something wrong with the type.
 */
ILUInt32 ILSizeOfType(ILExecThread *thread, ILType *type);

/*
 * Look up a type name within a particular thread's context.
 * Returns NULL if the name could not be found.
 */
ILClass *ILExecThreadLookupType(ILExecThread *thread, const char *typeName);

/*
 * Look up a method by type name, method name, and signature.
 * Returns NULL if the method could not be found.  This function
 * will search ancestor classes if the method is not found in
 * a child class.
 */
ILMethod *ILExecThreadLookupMethod(ILExecThread *thread,
								   const char *typeName,
								   const char *methodName,
								   const char *signature);

/*
 * Look up a field by type name, field name, and signature.
 * Returns NULL if the field could not be found.  This function
 * will search ancestor classes if the field is not found in
 * a child class.
 */
ILField *ILExecThreadLookupField(ILExecThread *thread,
								 const char *typeName,
								 const char *fieldName,
								 const char *signature);

/*
 * Call a particular method within a thread's context.
 * Returns non-zero if an exception was thrown during
 * the processing of the method.  Zero otherwise.
 * The return value, if any, is placed in "*result".
 */
int ILExecThreadCall(ILExecThread *thread, ILMethod *method,
					 void *result, ...);

/*
 * Call a particular virtual method within a thread's context.
 * If the method isn't marked as virtual, then an ordinary
 * method call will be used instead.
 */
int ILExecThreadCallVirtual(ILExecThread *thread, ILMethod *method,
							void *result, ...);

/*
 * Look up a method by name and then call it.  If the
 * method is not found, an exception will be thrown.
 */
int ILExecThreadCallNamed(ILExecThread *thread, const char *typeName,
						  const char *methodName, const char *signature,
						  void *result, ...);

/*
 * Look up a method by name and then call it using a
 * virtual cal.  If the method is not found, an exception
 * will be thrown.
 */
int ILExecThreadCallNamedVirtual(ILExecThread *thread, const char *typeName,
						         const char *methodName, const char *signature,
						         void *result, ...);

/*
 * Determine if there is a last-occuring exception
 * for a thread.  Returns non-zero if so.
 */
int ILExecThreadHasException(ILExecThread *thread);

/*
 * Get the last-occurring exception for a thread.
 * Returns NULL if there is no exception object.
 */
ILObject *ILExecThreadGetException(ILExecThread *thread);

/*
 * Set the last-occuring exception for a thread.
 */
void ILExecThreadSetException(ILExecThread *thread, ILObject *obj);

/*
 * Clear the last-occuring exception for a thread.  If an
 * exception is not cleared, it will be re-thrown from the
 * current method.
 */
void ILExecThreadClearException(ILExecThread *thread);

#ifdef	__cplusplus
};
#endif

#endif	/* _IL_ENGINE_H */
