/*
 * SDRegionTranslator.c - Region translator implementation.
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

#include "CRegionTranslator.h"
#include "CAffineTransform.h"

#ifdef __cplusplus
extern "C" {
#endif

static SDStatus
SDRegionTranslator_Data(SDRegionInterpreter  *_this,
                        SDRegionNode         *node,
                        void                **data)
{
	/* declarations */
	SDRegionTranslator *rt;

	/* assertions */
	SDASSERT((_this != 0));
	SDASSERT((node  != 0));
	SDASSERT((data  != 0));

	/* set the data to the default */
	*data = 0;

	/* get this as a region translator */
	rt = ((SDRegionTranslator *)_this);

	/* translate the node */
	if(node->type == SDRegionType_Rectangle)
	{
		/* get the rectangle node */
		SDRegionRect *rr = ((SDRegionRect *)node);

		/* apply the translation */
		SDRectangle_X(rr->rectangle) += SDVector_X(rt->offset);
		SDRectangle_Y(rr->rectangle) += SDVector_Y(rt->offset);
	}
	else if(node->type == SDRegionType_Path)
	{
		/* get the path node */
		SDRegionPath *rp = ((SDRegionPath *)node);

		/* apply the translation */
		SDVectorF_TranslatePoints
			(&(rt->offset), rp->points, rp->count);
	}

	/* return successfully */
	return SDStatus_OK;
}

static SDStatus
SDRegionTranslator_Op(SDRegionInterpreter  *_this,
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

	/* set the data to the default */
	*data = 0;

	/* return successfully */
	return SDStatus_OK;
}

static const SDRegionInterpreterClass SDRegionTranslator_Class =
{
	SDRegionTranslator_Data,
	SDRegionTranslator_Op,
	0
};

SDINTERNAL void
SDRegionTranslator_Initialize(SDRegionTranslator *_this,
                              SDFloat             dx,
                              SDFloat             dy)
{
	/* assertions */
	SDASSERT((_this != 0));

	/* initialize the offset */
	SDVector_X(_this->offset) = dx;
	SDVector_Y(_this->offset) = dy;

	/* initialize the base */
	SDRegionInterpreter_Initialize
		((SDRegionInterpreter *)_this, &SDRegionTranslator_Class);
}

SDINTERNAL void
SDRegionTranslator_Finalize(SDRegionTranslator *_this)
{
	/* assertions */
	SDASSERT((_this != 0));

	/* finalize the members */
	{
		/* finalize the base */
		SDRegionInterpreter_Finalize((SDRegionInterpreter *)_this);

		/* finalize the offset */
		_this->offset = SDVectorF_Zero;
	}
}

#ifdef __cplusplus
};
#endif
