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

#ifdef	__cplusplus
};
#endif

#endif	/* _IL_ENGINE_H */
