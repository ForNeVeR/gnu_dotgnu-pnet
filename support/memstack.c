/*
 * memstack.c - Memory stack implementation.
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

#include "il_utils.h"
#include "il_system.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Tuning parameters.
 */
#ifndef	IL_MEMSTACK_BLOCK_SIZE
#define	IL_MEMSTACK_BLOCK_SIZE	16384
#endif

/*
 * Alignment size for this system.
 */
#define	ALIGNMENT_SIZE	((sizeof(long) > sizeof(void *))	\
								? sizeof(long) : sizeof(void *))

void ILMemStackInit(ILMemStack *stack, unsigned long maxSize)
{
	stack->size = IL_MEMSTACK_BLOCK_SIZE & ~(ILPageAllocSize() - 1);
	stack->posn = stack->size;
	stack->blocks = 0;
	stack->currSize = 0;
	stack->maxSize = maxSize;
}

void ILMemStackDestroy(ILMemStack *stack)
{
	void *block, *next;
	block = stack->blocks;
	while(block != 0)
	{
		next = *((void **)block);
		ILPageFree(block, stack->size);
		block = next;
	}
	stack->posn = stack->size;
	stack->blocks = 0;
	stack->currSize = 0;
}

void *ILMemStackAllocItem(ILMemStack *stack, unsigned size)
{
	void *ptr;

	/* Round up the size to the nearest alignment boundary */
	size = (size + ALIGNMENT_SIZE - 1) & ~(ALIGNMENT_SIZE - 1);

	/* Do we need to allocate a new block? */
	if((stack->posn + size) > stack->size)
	{
		if(stack->maxSize && stack->currSize >= stack->maxSize)
		{
			/* We've reached the built-in limit for the stack */
			return 0;
		}
		ptr = ILPageAlloc(stack->size);
		if(!ptr)
		{
			/* The system itself is out of memory */
			return 0;
		}
		*((void **)ptr) = stack->blocks;
		stack->blocks = ptr;
		stack->posn = ALIGNMENT_SIZE;
		stack->currSize += stack->size;
	}

	/* Allocate the item */
	ptr = (void *)(((unsigned char *)(stack->blocks)) + stack->posn);
	stack->posn += size;
	return ptr;
}

#ifdef	__cplusplus
};
#endif
