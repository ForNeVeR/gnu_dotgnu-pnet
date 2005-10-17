/*
 * SDRegionCloner.c - Region cloner implementation.
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

#include "CRegionCloner.h"

#ifdef __cplusplus
extern "C" {
#endif

static SDStatus
SDRegionCloner_Op(SDRegionInterpreter  *_this,
                  SDRegionOp           *op,
                  void                 *left,
                  void                 *right,
                  void                **data)
{
	/* declarations */
	SDRegionOp *ro;

	/* assertions */
	SDASSERT((_this != 0));
	SDASSERT((op    != 0));
	SDASSERT((left  != 0));
	SDASSERT((right != 0));
	SDASSERT((data  != 0));

	/* create the operation node */
	if(!(SDRegionOp_Alloc(ro)))
	{
		return SDStatus_OutOfMemory;
	}

	/* initialize the operation node */
	ro->_base = op->_base;
	ro->left  = (SDRegionNode *)left;
	ro->right = (SDRegionNode *)right;

	/* set the clone */
	*data = (void *)ro;

	/* return successfully */
	return SDStatus_OK;
}

static SDStatus
SDRegionCloner_Data(SDRegionInterpreter  *_this,
                    SDRegionNode         *node,
                    void                **data)
{
	/* assertions */
	SDASSERT((_this != 0));
	SDASSERT((node  != 0));
	SDASSERT((data  != 0));

	/* set the data to the default */
	*data = 0;

	/* clone the data node */
	if(node->type == SDRegionType_Path)
	{
		/* declarations */
		SDRegionPath *rp;

		/* get the count and sizes */
		const SDUInt32 count = ((SDRegionPath *)node)->count;
		const SDUInt32 sizeP = (count * sizeof(SDPointF));
		const SDUInt32 sizeT = (count * sizeof(SDByte));

		/* create the path node */
		if(!(SDRegionPath_Alloc(rp)))
		{
			return SDStatus_OutOfMemory;
		}

		/* initialize the path node */
		rp->_base    = *node;
		rp->count    = count;
		rp->fillMode = ((SDRegionPath *)node)->fillMode;

		/* allocate the point list */
		if(!(rp->points = (SDPointF *)SDMalloc(sizeP)))
		{
			SDFree(rp);
			return SDStatus_OutOfMemory;
		}

		/* allocate the type list */
		if(!(rp->types = (SDByte *)SDMalloc(sizeT)))
		{
			SDFree(rp->points);
			SDFree(rp);
			return SDStatus_OutOfMemory;
		}

		/* copy the points */
		SDMemCopy(rp->points, ((SDRegionPath *)node)->points, sizeP);

		/* copy the types */
		SDMemCopy(rp->types, ((SDRegionPath *)node)->types, sizeT);

		/* set the clone */
		*data = (void *)rp;
	}
	else if(node->type == SDRegionType_Rectangle)
	{
		/* declarations */
		SDRegionRect *rr;

		/* create the rectangle node */
		if(!(SDRegionRect_Alloc(rr)))
		{
			return SDStatus_OutOfMemory;
		}

		/* initialize the rectangle node */
		rr->_base     = *node;
		rr->rectangle = ((SDRegionRect *)node)->rectangle;

		/* set the clone */
		*data = (void *)rr;
	}
	else
	{
		/* declarations */
		SDRegionNode *rn;

		/* create the node */
		if(!(SDRegionNode_Alloc(rn)))
		{
			return SDStatus_OutOfMemory;
		}

		/* initialize the node */
		*rn = *node;

		/* set the clone */
		*data = (void *)rn;
	}

	/* return successfully */
	return SDStatus_OK;
}

static void
SDRegionCloner_FreeData(void *data)
{
	/* assertions */
	SDASSERT((data != 0));

	/* free the node members, as needed */
	if(SDRegionNode_IsOp(((SDRegionNode *)data)))
	{
		/* get the operation node */
		SDRegionOp *op = ((SDRegionOp *)data);

		/* free the left operand node, as needed */
		if(op->left) { SDRegionCloner_FreeData(op->left); }

		/* free the right operand node, as needed */
		if(op->right) { SDRegionCloner_FreeData(op->right); }
	}
	else if(((SDRegionNode *)data)->type == SDRegionType_Path)
	{
		/* get the path node */
		SDRegionPath *rp = ((SDRegionPath *)data);

		/* free the point list */
		SDFree(rp->points);

		/* free the type list */
		SDFree(rp->types);
	}

	/* free the node */
	SDFree(data);
}

static const SDRegionInterpreterClass SDRegionCloner_Class =
{
	SDRegionCloner_Data,
	SDRegionCloner_Op,
	SDRegionCloner_FreeData
};

SDINTERNAL void
SDRegionCloner_Initialize(SDRegionCloner *_this)
{
	/* assertions */
	SDASSERT((_this != 0));

	/* initialize the base */
	SDRegionInterpreter_Initialize
		((SDRegionInterpreter *)_this, &SDRegionCloner_Class);
}

SDINTERNAL void
SDRegionCloner_Finalize(SDRegionCloner *_this)
{
	/* assertions */
	SDASSERT((_this != 0));

	/* finalize the base */
	SDRegionInterpreter_Finalize((SDRegionInterpreter *)_this);
}


#ifdef __cplusplus
};
#endif
