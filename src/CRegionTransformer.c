/*
 * SDRegionTransformer.c - Region transformer implementation.
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

#include "CRegionTransformer.h"
#include "CAffineTransform.h"

#ifdef __cplusplus
extern "C" {
#endif

static SDStatus
SDRegionTransformer_Op(SDRegionInterpreter  *_this,
                       SDRegionOp           *op,
                       void                 *left,
                       void                 *right,
                       void                **data)
{
	/* assertions */
	SDASSERT((_this != 0));
	SDASSERT((op    != 0));
	SDASSERT((data  != 0));

	/* set data to the default */
	*data = 0;

	/* handle the left operand */
	if(left != 0)
	{
		/* get the left node */
		SDRegionNode *node = ((SDRegionNode *)left);

		/* assertions */
		SDASSERT((op->left->type == SDRegionType_Rectangle));
		SDASSERT((node->type     == SDRegionType_Path));

		/* free the current left node */
		SDRegionData_Free(op->left);

		/* set the left node */
		op->left = node;
	}

	/* handle the right operand */
	if(right != 0)
	{
		/* get the right node */
		SDRegionNode *node = ((SDRegionNode *)right);

		/* assertions */
		SDASSERT((op->right->type == SDRegionType_Rectangle));
		SDASSERT((node->type      == SDRegionType_Path));

		/* free the current right node */
		SDRegionData_Free(op->right);

		/* set the right node */
		op->right = node;
	}

	/* return successfully */
	return SDStatus_OK;
}

static SDStatus
SDRegionTransformer_Data(SDRegionInterpreter  *_this,
                         SDRegionNode         *node,
                         void                **data)
{
	/* declarations */
	SDRegionTransformer *t;

	/* assertions */
	SDASSERT((_this != 0));
	SDASSERT((node  != 0));
	SDASSERT((data  != 0));

	/* set the data to the default */
	*data = 0;

	/* get this as a region transformer */
	t = ((SDRegionTransformer *)_this);

	/* transform the node */
	if(node->type == SDRegionType_Rectangle)
	{
		/* declarations */
		SDRegionPath *rp;
		SDRegionRect *rr;

		/* get the rectangle node */
		rr = ((SDRegionRect *)node);

		/* allocate the path node */
		if(!SDRegionPath_Alloc(rp))
		{
			return SDStatus_OutOfMemory;
		}

		/* allocate the point list */
		if(!(rp->points = (SDPointF *)SDMalloc(4 * sizeof(SDPointF))))
		{
			SDFree(rp);
			return SDStatus_OutOfMemory;
		}

		/* allocate the type list */
		if(!(rp->types = (SDByte *)SDMalloc(4 * sizeof(SDByte))))
		{
			SDFree(rp->points);
			SDFree(rp);
			return SDStatus_OutOfMemory;
		}

		/* set the base */
		rp->_base = SDRegionNode_Path;

		/* set the count */
		rp->count = 4;

		/* set the fill mode */
		rp->fillMode = SDFillMode_Alternate;

		/* set the points */
		SDRegionRect_RectToPath(rp->points, rr->rectangle);

		/* set the types */
		SDMemCopy(rp->types, SDRegionRect_PathTypes, 4);

		/* transform the points */
		SDAffineTransformF_TransformPoints(t->transform, rp->points, 4);

		/* set the data */
		*data = (void *)rp;
	}
	else if(node->type == SDRegionType_Path)
	{
		/* get the path node */
		SDRegionPath *rp = ((SDRegionPath *)node);

		/* transform the points */
		SDAffineTransformF_TransformPoints
			(t->transform, rp->points, rp->count);
	}

	/* return successfully */
	return SDStatus_OK;
}

static SDStatus
SDRegionTransformer_DataSimple(SDRegionInterpreter  *_this,
                               SDRegionNode         *node,
                               void                **data)
{
	/* declarations */
	SDRegionTransformer *t;

	/* assertions */
	SDASSERT((_this != 0));
	SDASSERT((node  != 0));
	SDASSERT((data  != 0));

	/* set the data to the default */
	*data = 0;

	/* get this as a region transformer */
	t = ((SDRegionTransformer *)_this);

	/* transform the node */
	if(node->type == SDRegionType_Rectangle)
	{
		/* declarations */
		SDRegionRect *rr;

		/* get the transformation properties */
		const SDFloat xx = SDAffineTransform_XX(*(t->transform));
		const SDFloat yy = SDAffineTransform_YY(*(t->transform));
		const SDFloat dx = SDAffineTransform_DX(*(t->transform));
		const SDFloat dy = SDAffineTransform_DY(*(t->transform));

		/* get the rectangle node */
		rr = ((SDRegionRect *)node);

		/* perform the transformation */
		{
			/* get the rectangle properties */
			const SDFloat x = SDRectangle_X(rr->rectangle);
			const SDFloat y = SDRectangle_Y(rr->rectangle);
			const SDFloat w = SDRectangle_Width(rr->rectangle);
			const SDFloat h = SDRectangle_Height(rr->rectangle);

			/* transform x and width */
			if(xx < 0.0f)
			{
				SDRectangle_X(rr->rectangle)     = (xx * (x + w)) + dx;
				SDRectangle_Width(rr->rectangle) = (-xx * w);
			}
			else
			{
				SDRectangle_X(rr->rectangle)     = (xx * x) + dx;
				SDRectangle_Width(rr->rectangle) = (xx * w);
			}

			/* transform y and height */
			if(yy < 0.0f)
			{
				SDRectangle_Y(rr->rectangle)      = (yy * (y - h)) + dy;
				SDRectangle_Height(rr->rectangle) = (-yy * h);
			}
			else
			{
				SDRectangle_Y(rr->rectangle)      = (yy * y) + dy;
				SDRectangle_Height(rr->rectangle) = (yy * h);
			}
		}
	}
	else if(node->type == SDRegionType_Path)
	{
		/* get the path node */
		SDRegionPath *rp = ((SDRegionPath *)node);

		/* transform the points */
		SDAffineTransformF_TransformPoints
			(t->transform, rp->points, rp->count);
	}

	/* return successfully */
	return SDStatus_OK;
}

static void
SDRegionTransformer_FreeData(void *data)
{
	/* declarations */
	SDRegionPath *rp;

	/* assertions */
	SDASSERT((data != 0));

	/* get the path node */
	rp = ((SDRegionPath *)data);

	/* assertions */
	SDASSERT((rp->_base.type == SDRegionType_Path));

	/* free the point list */
	SDFree(rp->points);

	/* free the type list */
	SDFree(rp->types);

	/* free the node */
	SDFree(rp);
}

static const SDRegionInterpreterClass SDRegionTransformer_Class =
{
	SDRegionTransformer_Data,
	SDRegionTransformer_Op,
	SDRegionTransformer_FreeData
};
static const SDRegionInterpreterClass SDRegionTransformer_ClassSimple =
{
	SDRegionTransformer_DataSimple,
	SDRegionTransformer_Op,
	0
};

SDINTERNAL void
SDRegionTransformer_Initialize(SDRegionTransformer *_this,
                               SDAffineTransformF  *transform)
{
	/* assertions */
	SDASSERT((_this     != 0));
	SDASSERT((transform != 0));

	/* initialize the transformation */
	_this->transform = transform;

	/* initialize the base */
	if((SDAffineTransform_XY(*transform) == 0.0f) &&
	   (SDAffineTransform_YX(*transform) == 0.0f))
	{
		SDRegionInterpreter_Initialize
			((SDRegionInterpreter *)_this, &SDRegionTransformer_ClassSimple);
	}
	else
	{
		SDRegionInterpreter_Initialize
			((SDRegionInterpreter *)_this, &SDRegionTransformer_Class);
	}
}

SDINTERNAL void
SDRegionTransformer_Finalize(SDRegionTransformer *_this)
{
	/* assertions */
	SDASSERT((_this != 0));

	/* finalize the members */
	{
		/* finalize the base */
		SDRegionInterpreter_Finalize((SDRegionInterpreter *)_this);

		/* finalize the transform */
		_this->transform = 0;
	}
}


#ifdef __cplusplus
};
#endif
