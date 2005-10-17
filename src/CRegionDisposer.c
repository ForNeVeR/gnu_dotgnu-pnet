/*
 * SDRegionDisposer.c - Region disposer implementation.
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

#include "CRegionDisposer.h"

#ifdef __cplusplus
extern "C" {
#endif

static SDStatus
SDRegionDisposer_Op(SDRegionInterpreter  *_this,
                    SDRegionOp           *op,
                    void                 *left,
                    void                 *right,
                    void                **data)
{
	/* assertions */
	SDASSERT((_this != 0));
	SDASSERT((op    != 0));
	SDASSERT((left  == 0));
	SDASSERT((right == 0));
	SDASSERT((data  != 0));

	/* dispose of the operation node */
	SDFree(op);

	/* return successfully */
	return SDStatus_OK;
}

static SDStatus
SDRegionDisposer_Data(SDRegionInterpreter  *_this,
                      SDRegionNode         *node,
                      void                **data)
{
	/* assertions */
	SDASSERT((_this != 0));
	SDASSERT((node  != 0));
	SDASSERT((data  != 0));

	/* dispose of the data node */
	SDRegionData_Free(node);

	/* return successfully */
	return SDStatus_OK;
}

static void
SDRegionDisposer_FreeData(void *data)
{
	/* assertions */
	SDASSERT((data != 0));

	/* free the node members, as needed */
	if(SDRegionNode_IsOp(((SDRegionNode *)data)))
	{
		/* get the operation node */
		SDRegionOp *op = ((SDRegionOp *)data);

		/* free the left operand node, as needed */
		if(op->left) { SDRegionDisposer_FreeData(op->left); }

		/* free the right operand node, as needed */
		if(op->right) { SDRegionDisposer_FreeData(op->right); }
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

static const SDRegionInterpreterClass SDRegionDisposer_Class =
{
	SDRegionDisposer_Data,
	SDRegionDisposer_Op,
	SDRegionDisposer_FreeData
};

SDINTERNAL void
SDRegionDisposer_Initialize(SDRegionDisposer *_this)
{
	/* assertions */
	SDASSERT((_this != 0));

	/* initialize the base */
	SDRegionInterpreter_Initialize
		((SDRegionInterpreter *)_this, &SDRegionDisposer_Class);
}

SDINTERNAL void
SDRegionDisposer_Finalize(SDRegionDisposer *_this)
{
	/* assertions */
	SDASSERT((_this != 0));

	/* finalize the base */
	SDRegionInterpreter_Finalize((SDRegionInterpreter *)_this);
}


#ifdef __cplusplus
};
#endif
