/*
 * SDRegionStack.c - Region stack implementation.
 *
 * Copyright (C) 2005  Free Software Foundation, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "CRegionStack.h"

#ifdef __cplusplus
extern "C" {
#endif

SDINTERNAL void
SDRegionStack_Initialize(SDRegionStack *_this)
{
	/* assertions */
	SDASSERT(_this != 0);

	/* initialize the stack */
	*_this = SDRegionStack_Zero;
}

SDINTERNAL void
SDRegionStack_Finalize(SDRegionStack *_this)
{
	/* assertions */
	SDASSERT(_this != 0);

	/* get the current stack */
	while(_this->prev != 0) { _this = _this->prev; }

	/* move to the next stack */
	_this = _this->next;

	/* finalize the stack */
	while(_this != 0)
	{
		/* get the next stack */
		SDRegionStack *next = _this->next;

		/* free the current stack */
		SDFree(_this);

		/* move to the next stack */
		_this = next;
	}
}

SDINTERNAL void
SDRegionStack_Pop(SDRegionStack **_this)
{
	/* declarations */
	SDRegionStack *stack;

	/* assertions */
	SDASSERT((_this  != 0));
	SDASSERT((*_this != 0));

	/* get the current stack */
	stack = *_this;

	/* update the count */
	--(stack->count);

	/* update the stack */
	if(stack->count == 0)
	{
		*_this = stack->prev;
	}
}

SDINTERNAL SDStatus
SDRegionStack_Push(SDRegionStack **_this,
                   SDRegionOp     *op)
{
	/* declarations */
	SDRegionStack *stack;

	/* assertions */
	SDASSERT((_this  != 0));
	SDASSERT((*_this != 0));
	SDASSERT((op     != 0));

	/* get the current stack */
	stack = *_this;

	/* ensure the capacity of the stack */
	if(stack->count == SDRegionStack_Size)
	{
		/* get or create the next stack */
		if(stack->next != 0)
		{
			stack = stack->next;
			stack->count = 0;
		}
		else
		{
			/* declarations */
			SDRegionStack *next;

			/* allocate the next stack */
			if(!SDRegionStack_Alloc(stack))
			{
				return SDStatus_OutOfMemory;
			}

			/* initialize the next stack */
			*next = SDRegionStack_Zero;

			/* update the stack list */
			stack->next = next;
			next->prev  = stack;

			/* update the current stack */
			stack  = next;
			*_this = stack;
		}
	}

	/* update the count */
	++(stack->count);

	/* push the operation node */
	SDRegionStack_Top(*stack)    = SDRegionStackNode_Zero;
	SDRegionStack_Top(*stack).op = op;

	/* return successfully */
	return SDStatus_OK;
}

SDINTERNAL void
SDRegionStack_Reset(SDRegionStack **_this)
{
	/* declarations */
	SDRegionStack *stack;
	SDRegionStack *prev;

	/* assertions */
	SDASSERT((_this  != 0));
	SDASSERT((*_this != 0));

	/* get the current stack */
	stack = *_this;

	/* get the previous stack */
	prev = stack->prev;

	/* reset the stack */
	while(prev != 0)
	{
		/* update the count */
		stack->count = 0;

		/* move to the previous stack */
		stack = prev;
		prev = stack->prev;
	}

	/* update the count */
	stack->count = 0;

	/* set the current stack */
	*_this = stack;
}
#ifdef __cplusplus
};
#endif
