/*
 * il_gc.h - Interface to the garbage collector.
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

#ifndef	_IL_GC_H
#define	_IL_GC_H

#include "il_system.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Initialize the garbage collector with a specific maximum heap size.
 * If "maxSize" is zero, then the collector will use all of memory.
 */
void ILGCInit(unsigned long maxSize);

/*
 * Allocate a block of memory from the garbage collector.
 * The block may contain pointers to other blocks.  The
 * block is guaranteed to be zero'ed.
 */
void *ILGCAlloc(unsigned long size);

/*
 * Allocate a block of memory from the garbage collector
 * that will never contain pointers.  Use this for allocating
 * arrays of atomic numeric types.  The block is zero'ed.
 */
void *ILGCAllocAtomic(unsigned long size);

/*
 * Allocate a block of memory that is persistent.  It will
 * not be collected until explicited free'd, but it will
 * still be scanned for object pointers.
 */
void *ILGCAllocPersistent(unsigned long size);

/*
 * Free a persistent block of memory.
 */
void ILGCFreePersistent(void *block);

/*
 * Finalization callback function.
 */
typedef void (*ILGCFinalizer)(void *block, void *data);

/*
 * Register a finalizer for a block of memory.
 */
void ILGCRegisterFinalizer(void *block, ILGCFinalizer func, void *data);

/*
 * Mark a region of memory as not containing any object pointers.
 */
void ILGCMarkNoPointers(void *start, unsigned long size);

/*
 * Trigger explicit garbage collection.
 */
void ILGCCollect(void);

/*
 * Invoke the pending finalizers and wait for them to complete.
 */
void ILGCInvokeFinalizers(void);

/*
 * Get the current size of the garbage collector's heap.
 */
long ILGCGetHeapSize(void);

#ifdef	__cplusplus
};
#endif

#endif	/* _IL_GC_H */
