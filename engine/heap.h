/*
 * heap.h - Heap implementation for the runtime engine.
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

#ifndef	_ENGINE_HEAP_H
#define	_ENGINE_HEAP_H

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Structure of a heap arena for small memory blocks.
 */
typedef struct _tagILHeapArena ILHeapArena;
struct _tagILHeapArena
{
	/* Last block within the arena */
	void *lastBlock;

	/* Array of other blocks within the arena */
	void **otherBlocks;
	unsigned long numOtherBlocks;

	/* Allocation information for getting objects */
	void *firstFree;
	unsigned long freeIndex;

};

/*
 * Tuning parameters for small memory block arenas.
 */
#ifndef	IL_HEAP_ARENA_STEP
#define	IL_HEAP_ARENA_STEP		16		/* Must be a power of 2 */
#endif
#ifndef	IL_HEAP_ARENA_MAX_SIZE
#define	IL_HEAP_ARENA_MAX_SIZE	256		/* Must be a multiple of the step */
#endif
#ifndef	IL_HEAP_ARENA_NUM_PAGES
#define	IL_HEAP_ARENA_NUM_PAGES	4
#endif
#define	IL_HEAP_NUM_ARENAS		(IL_HEAP_ARENA_MAX_SIZE / IL_HEAP_ARENA_STEP)

/*
 * Structure of a block header for large memory blocks.
 */
typedef struct _tagILHeapLarge ILHeapLarge;
struct _tagILHeapLarge
{
	ILHeapLarge *next;		/* Next large overflow block */
	ILHeapLarge *prev;		/* Previous large overflow block */

};

/*
 * Structure of the heap.
 */
typedef struct _tagILHeap ILHeap;
struct _tagILHeap
{
	/* Arenas for small memory blocks.  We round object sizes
	   up to a multiple of IL_HEAP_ARENA_STEP and then allocate
	   in the respective arena */
	ILHeapArena	smallArenas[IL_HEAP_NUM_ARENAS];

	/* Overflow area for larger memory blocks */
	ILHeapLarge *overflowBlocks;

	/* Current and maximum size for the heap */
	unsigned long currHeapSize;
	unsigned long maxHeapSize;

	/* Size of page allocations that are used in the arenas */
	unsigned long pageAllocSize;

};

/*
 * Create the heap.
 */
void _ILHeapCreate(ILHeap *heap);

/*
 * Destroy the heap.
 */
void _ILHeapDestroy(ILHeap *heap);

/*
 * Allocate a block from the heap.  Returns NULL if out of memory.
 */
void *_ILHeapAlloc(ILHeap *heap, ILUInt32 size);

/*
 * Allocate a block from the heap and clear it to all-zeroes.
 * Returns NULL if out of memory.
 */
void *_ILHeapCalloc(ILHeap *heap, ILUInt32 size);

/*
 * Free a block back to the heap.
 */
void _ILHeapFree(ILHeap *heap, void *block, ILUInt32 size);

#ifdef	__cplusplus
};
#endif

#endif	/* _ENGINE_HEAP_H */
