/*
 * SDRegionStack.h - Region stack header.
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

#ifndef _SD_REGIONSTACK_H_
#define _SD_REGIONSTACK_H_

#include "CRegion.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _tagSDRegionStackNode SDRegionStackNode;
struct _tagSDRegionStackNode
{
	SDUInt32    visited;
	SDRegionOp *op;
	void       *left;
	void       *right;
};
static const SDRegionStackNode SDRegionStackNode_Zero;

#define SDRegionStack_Top(stack) ((stack).elements[((stack).count - 1)])
#define SDRegionStack_Size 32

typedef struct _tagSDRegionStack SDRegionStack;
struct _tagSDRegionStack
{
	SDRegionStackNode  elements[SDRegionStack_Size];
	SDUInt32           count;
	SDRegionStack     *prev;
	SDRegionStack     *next;
};
static const SDRegionStack SDRegionStack_Zero;

#define SDRegionStack_Alloc(stack) \
	((stack) = ((SDRegionStack *)SDMalloc(sizeof(SDRegionStack))))

SDINTERNAL void
SDRegionStack_Initialize(SDRegionStack *_this);
SDINTERNAL void
SDRegionStack_Finalize(SDRegionStack *_this);
SDINTERNAL void
SDRegionStack_Pop(SDRegionStack **_this);
SDINTERNAL SDStatus
SDRegionStack_Push(SDRegionStack **_this,
                   SDRegionOp     *op);
SDINTERNAL void
SDRegionStack_Reset(SDRegionStack **_this);

#ifdef __cplusplus
};
#endif

#endif /* _SD_REGIONSTACK_H_ */
