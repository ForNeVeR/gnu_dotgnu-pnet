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
 *	Deinitialize the GC.
 */
void ILGCDeinit();

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
int ILGCInvokeFinalizers(int timeout);

/*
 * Temporarily disable finalizers that are called during allocation.
 */
int ILGCDisableFinalizers(int timeout);

/*
 * Re-enable finalizers that are called during allocation.
 */
void ILGCEnableFinalizers(void);

/*
 * Get the current size of the garbage collector's heap.
 */
long ILGCGetHeapSize(void);

/*
 * Register a pointer to a weak reference.
 */
void ILGCRegisterWeak(void *ptr);

/*
 * Unregister a pointer to a weak reference.
 */
void ILGCUnregisterWeak(void *ptr);

/*
 * Register a pointer to a general weak reference.
 */
void ILGCRegisterGeneralWeak(void *ptr, void *obj);

/*
 * Creates and returns a type descriptor for an object.
 * The descriptor will be passed ILGCAllocExplicitlyTyped.
 *
 * The bitmap parameter describes the layout of the object.
 * If the first bit of the first UInt of bitmap is 1 then 
 * the first word of the object is considered a pointer (and so on).
 * The len argument specifies how many bits in the map are
 * to be used.  If len is less than the number of bits in the object
 * then the rest of the words in the object is considered to
 * hold pointers.
 */
ILNativeInt ILGCCreateTypeDescriptor(ILNativeUInt bitmap[], ILNativeUInt len);

/*
 * Allocate a block of memory from the garbage collector.
 * The block may contain pointers to other blocks as described
 * in the descriptor.  The block is guaranteed to be zero'ed.
 */
void *ILGCAllocExplicitlyTyped(unsigned long size, ILNativeInt descriptor);

#ifdef	__cplusplus
};
#endif

#endif	/* _IL_GC_H */
