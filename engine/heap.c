/*
 * heap.c - Heap implementation for the runtime engine.
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

/*
 * Note: this is a very early implementation of the heap.
 * A lot of tuning will be required in the future.
 */

#include "engine_private.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Heap lock handling: not yet active as we don't yet have threading.
 */
#define	HEAP_LOCK(heap)
#define	HEAP_UNLOCK(heap)

/*
 * Fudge factors to account for overhead in the system's "malloc" heap.
 */
#define	HEAP_FUDGE_ROUND	16
#define	HEAP_FUDGE_OVERHEAD	16

void _ILHeapCreate(ILHeap *heap)
{
	ILMemZero(heap, sizeof(ILHeap));
	heap->maxHeapSize = 8192 * 1024;
	heap->pageAllocSize = ILPageAllocSize() * IL_HEAP_ARENA_NUM_PAGES;
}

void _ILHeapDestroy(ILHeap *heap)
{
	int posn;
	ILHeapArena *arena;
	unsigned long block;
	ILHeapLarge *large, *next;

	/* Free the small memory block arenas */
	for(posn = 0; posn < IL_HEAP_NUM_ARENAS; ++posn)
	{
		arena = &(heap->smallArenas[posn]);
		for(block = 0; block < arena->numOtherBlocks; ++block)
		{
			ILPageFree(arena->otherBlocks[block], heap->pageAllocSize);
		}
		if(arena->otherBlocks)
		{
			ILFree(arena->otherBlocks);
		}
	}

	/* Free the overflow blocks for large allocations */
	large = heap->overflowBlocks;
	while(large != 0)
	{
		next = large->next;
		ILFree(large);
		large = next;
	}
}

/*
 * Allocate a new set of pages for an arena.
 * Returns zero if out of memory.
 */
static int AllocPages(ILHeap *heap, ILHeapArena *arena)
{
	void **newOther;
	void *block;

	if(!(arena->lastBlock))
	{
		/* Nothing in this arena yet, so initialize it */
		arena->lastBlock = ILPageAlloc(heap->pageAllocSize);
		return (arena->lastBlock != 0);
	}
	else
	{
		/* Expand the size of the "otherBlocks" array */
		newOther = (void **)ILRealloc
				(arena->otherBlocks,
				 sizeof(void *) * (arena->numOtherBlocks + 1));
		if(!newOther)
		{
			return 0;
		}
		arena->otherBlocks = newOther;

		/* Allocate a new set of pages */
		block = ILPageAlloc(heap->pageAllocSize);
		if(!block)
		{
			return 0;
		}

		/* Shift "lastBlock" into the "otherBlocks" array */
		arena->otherBlocks[(arena->numOtherBlocks)++] = arena->lastBlock;

		/* Replace "lastBlock" with the new allocation and exit */
		arena->lastBlock = block;
		return 1;
	}
}

void *_ILHeapAlloc(ILHeap *heap, ILUInt32 size)
{
	ILHeapArena *arena;
	ILHeapLarge *large;
	void *block;
	ILUInt32 fudgeSize;

	/* Lock the heap while we are manipulating it */
	HEAP_LOCK(heap);

	/* Acquire the block */
	if(size <= IL_HEAP_ARENA_MAX_SIZE)
	{
		/* Allocate from an arena */
		arena = &(heap->smallArenas[(size - 1) / IL_HEAP_ARENA_STEP]);
		if(arena->firstFree)
		{
			/* We already know that this is free */
			block = arena->firstFree;
			arena->firstFree = *((void **)block);
		}
		else if(arena->freeIndex > 0)
		{
			/* We can allocate from the last block in the arena */
			--(arena->freeIndex);
			size = (size + IL_HEAP_ARENA_STEP - 1) & ~(IL_HEAP_ARENA_STEP - 1);
			block = (void *)(((unsigned char *)(arena->lastBlock)) +
									(arena->freeIndex * size));
		}
		else
		{
			/* Check the maximum heap size */
			if((heap->currHeapSize + heap->pageAllocSize) > heap->maxHeapSize)
			{
				HEAP_UNLOCK(heap);
				return 0;
			}

			/* We must allocate a new page block for the arena */
			if(!AllocPages(heap, arena))
			{
				HEAP_UNLOCK(heap);
				return 0;
			}

			/* Allocate from the new page */
			size = (size + IL_HEAP_ARENA_STEP - 1) & ~(IL_HEAP_ARENA_STEP - 1);
			arena->freeIndex = (heap->pageAllocSize / size) - 1;
			block = (void *)(((unsigned char *)(arena->lastBlock)) +
									(arena->freeIndex * size));

			/* Update the current heap size */
			heap->currHeapSize += heap->pageAllocSize;
		}
	}
	else
	{
		/* Check the maximum heap size */
		fudgeSize = ((size + sizeof(ILHeapLarge) + HEAP_FUDGE_ROUND - 1)
							& ~(HEAP_FUDGE_ROUND - 1)) + HEAP_FUDGE_OVERHEAD;
		if((heap->currHeapSize + fudgeSize) > heap->maxHeapSize)
		{
			HEAP_UNLOCK(heap);
			return 0;
		}

		/* Allocate a large memory block */
		large = (ILHeapLarge *)ILMalloc(size + sizeof(ILHeapLarge));
		if(!large)
		{
			HEAP_UNLOCK(heap);
			return 0;
		}

		/* Link the large memory block into the heap's control list */
		large->next = heap->overflowBlocks;
		large->prev = 0;
		if(heap->overflowBlocks)
		{
			heap->overflowBlocks->prev = large;
		}
		heap->overflowBlocks = large;

		/* Get the pointer to the data region of the large memory block */
		block = (void *)(large + 1);

		/* Update the current heap size */
		heap->currHeapSize += fudgeSize;
	}

	/* Unlock the heap */
	HEAP_UNLOCK(heap);

	/* Return the allocated block to the caller */
	return block;
}

void *_ILHeapCalloc(ILHeap *heap, ILUInt32 size)
{
	void *block = _ILHeapAlloc(heap, size);
	if(block)
	{
		ILMemZero(block, size);
	}
	return block;
}

void _ILHeapFree(ILHeap *heap, void *block, ILUInt32 size)
{
	ILHeapArena *arena;
	ILHeapLarge *large;

	/* Lock the heap while we are manipulating it */
	HEAP_LOCK(heap);

	/* Free the block */
	if(block && size)
	{
		if(size <= IL_HEAP_ARENA_MAX_SIZE)
		{
			arena = &(heap->smallArenas[(size - 1) / IL_HEAP_ARENA_STEP]);
			*((void **)block) = arena->firstFree;
			arena->firstFree = block;
		}
		else
		{
			large = ((ILHeapLarge *)block) - 1;
			if(large->next)
			{
				large->next->prev = large->prev;
			}
			if(large->prev)
			{
				large->prev->next = large->next;
			}
			else
			{
				heap->overflowBlocks = large->next;
			}
			ILFree(large);
		}
	}

	/* Unlock the heap */
	HEAP_UNLOCK(heap);
}

/*
 * Initialize a class.
 */
static int InitializeClass(ILExecThread *thread, ILClass *classInfo)
{
	/* Lay out the class's fields */
	if(!_ILLayoutClass(thread, classInfo))
	{
		/* TODO: Throw a "TypeInitializationException" */
		return 0;
	}

	/* The class has been initialized */
	return 1;
}

ILObject *_ILEngineAlloc(ILExecThread *thread, ILClass *classInfo,
						 ILUInt32 size)
{
	void *ptr;

	/* Make sure the class has been initialized before we start */
	if(classInfo != 0 && !InitializeClass(thread, classInfo))
	{
		return 0;
	}

	/* Allocate memory from the heap */
	ptr = _ILHeapAlloc(&(thread->process->heap), size + sizeof(ILClass *));
	if(!ptr)
	{
		/* Throw an "OutOfMemoryException" */
		thread->thrownException = thread->process->outOfMemoryObject;
		return 0;
	}

	/* Set the class into the block */
	*((ILClass **)ptr) = classInfo;

	/* Return a pointer to the data just after the class information */
	return (void *)(((ILClass **)ptr) + 1);
}

ILObject *_ILEngineAllocObject(ILExecThread *thread, ILClass *classInfo)
{
	if(!InitializeClass(thread, classInfo))
	{
		return 0;
	}
	return _ILEngineAlloc(thread, classInfo,
						  ((ILClassPrivate *)(classInfo->userData))->size);
}

#ifdef	__cplusplus
};
#endif
