/*
 * SDRegionRasterizer.c - Region rasterizer implementation.
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

#include "CRegionRasterizer.h"
#include "CAffineTransform.h"
#include "CTrapezoids.h"
#include "CPointArray.h"

#ifdef __cplusplus
extern "C" {
#endif

static const pixman_operator_t SDRegionType_PixmanOperator[] =
{
	PIXMAN_OPERATOR_SRC,
	PIXMAN_OPERATOR_IN,
	PIXMAN_OPERATOR_OVER,
	PIXMAN_OPERATOR_XOR,
	PIXMAN_OPERATOR_OUT,
	PIXMAN_OPERATOR_OUT_REVERSE
};

static SDStatus
SDRegionRasterizer_CreateMaskSimple(SDRegionRasterizer  *_this,
                                    SDByte               value,
                                    pixman_image_t     **mask)
{
	/* assertions */
	SDASSERT((_this != 0));
	SDASSERT((mask  != 0));

	/* create the mask */
	*mask =
		pixman_image_create
			(_this->format, _this->width, _this->height);

	/* ensure we have a mask */
	SDStatus_Require((*mask != 0), SDStatus_OutOfMemory);

	/* ensure we have the size */
	if(_this->size == -1)
	{
		_this->size = (_this->height * pixman_image_get_stride(*mask));
	}

	/* initialize the mask */
	SDMemSet(pixman_image_get_data(*mask), value, _this->size);

	/* return successfully */
	return SDStatus_OK;
}

static SDStatus
SDRegionRasterizer_CreateMaskPath(SDRegionRasterizer  *_this,
                                  const SDPointF      *points,
                                  const SDByte        *types,
                                  SDUInt32             count,
                                  SDFillMode           fillMode,
                                  pixman_image_t     **mask)
{
	/* declarations */
	SDTrapezoidX *trapezoids;
	SDUInt32      tcount;

	/* assertions */
	SDASSERT((_this  != 0));
	SDASSERT((points != 0));
	SDASSERT((types  != 0));
	SDASSERT((mask   != 0));

	/* reset filler and trapezoids */
	SDTrapezoids_Reset(&(_this->trapezoids));
	SDFiller_Reset(&(_this->filler));

	/* perform path trapezoidation */
	SDStatus_Check
		(SDFiller_ToTrapezoids
			(&(_this->filler), &(_this->trapezoids),
			 points, types, count, fillMode));

	/* create the mask */
	SDStatus_Check
		(SDRegionRasterizer_CreateMaskSimple
			(_this, 0x00, mask));

	/* get the trapezoid list and the count */
	tcount     = SDTrapezoids_Count(_this->trapezoids);
	trapezoids = SDTrapezoids_Trapezoids(_this->trapezoids);

	/* add the trapezoids to the mask */
	pixman_add_trapezoids
		(*mask, 0, 0, ((pixman_trapezoid_t *)trapezoids), tcount);

	/* return successfully */
	return SDStatus_OK;
}

static SDStatus
SDRegionRasterizer_Data(SDRegionInterpreter  *_this,
                        SDRegionNode         *node,
                        void                **data)
{
	/* declarations */
	SDRegionRasterizer *rast;

	/* assertions */
	SDASSERT((_this != 0));
	SDASSERT((node  != 0));
	SDASSERT((data  != 0));

	/* set the data to the default */
	*data = 0;

	/* get this as a rasterizer */
	rast = ((SDRegionRasterizer *)_this);

	/* rasterize based on type */
	if(node->type == SDRegionType_Rectangle)
	{
		/* declarations */
		SDPointF      points[4];
		SDRegionRect *rr;

		/* get the region rectangle */
		rr = ((SDRegionRect *)node);

		/* set the points */
		SDRegionRect_RectToPath(points, rr->rectangle);

		/* transform the points, as needed */
		if(rast->transform != 0)
		{
			SDAffineTransformF_TransformPoints(rast->transform, points, 4);
		}

		/* rasterize the rectangle */
		{
			/* declarations */
			pixman_image_t *mask;

			/* create the mask */
			SDStatus_Check
				(SDRegionRasterizer_CreateMaskPath
					(rast, points, SDRegionRect_PathTypes, 4,
					 SDFillMode_Alternate, &mask));

			/* set the data */
			*data = mask;
		}
	}
	else if(node->type == SDRegionType_Path)
	{
		/* declarations */
		SDPointArrayF *array;
		SDRegionPath  *rp;

		/* get the region path */
		rp = ((SDRegionPath *)node);

		/* get the array */
		array = &(rast->array);

		/* ensure the capacity of the array */
		SDStatus_Check
			(SDPointArrayF_EnsureCapacity
				(array, rp->count));

		/* copy the points */
		SDMemCopy(SDPointArray_Points(*array), rp->points, rp->count);

		/* transform the points, as needed */
		if(rast->transform != 0)
		{
			SDAffineTransformF_TransformPoints
				(rast->transform, SDPointArray_Points(*array), rp->count);
		}

		/* rasterize the path */
		{
			/* declarations */
			pixman_image_t *mask;

			/* create the mask */
			SDStatus_Check
				(SDRegionRasterizer_CreateMaskPath
					(rast, SDPointArray_Points(*array), rp->types, rp->count,
					 rp->fillMode, &mask));

			/* set the data */
			*data = mask;
		}
	}
	else if(node->type == SDRegionType_Infinite)
	{
		/* declarations */
		pixman_image_t *mask;

		/* create the mask */
		SDStatus_Check
			(SDRegionRasterizer_CreateMaskSimple
				(rast, 0xFF, &mask));

		/* set the data */
		*data = mask;
	}
	else
	{
		/* declarations */
		pixman_image_t *mask;

		/* assertions */
		SDASSERT((node->type == SDRegionType_Empty));

		/* create the mask */
		SDStatus_Check
			(SDRegionRasterizer_CreateMaskSimple
				(rast, 0x00, &mask));

		/* set the data */
		*data = mask;
	}

	/* return successfully */
	return SDStatus_OK;
}

static SDStatus
SDRegionRasterizer_Op(SDRegionInterpreter  *_this,
                      SDRegionOp           *op,
                      void                 *left,
                      void                 *right,
                      void                **data)
{
	/* declarations */
	SDRegionRasterizer *rast;

	/* assertions */
	SDASSERT((_this != 0));
	SDASSERT((op    != 0));
	SDASSERT((left  != 0));
	SDASSERT((right != 0));
	SDASSERT((data  != 0));

	/* set the data to the default */
	*data = 0;

	/* get this as a rasterizer */
	rast = ((SDRegionRasterizer *)_this);

	/* perform the operation */
	{
		/* declarations */
		pixman_image_t *li;
		pixman_image_t *ri;

		/* get the images */
		li = ((pixman_image_t *)left);
		ri = ((pixman_image_t *)right);

		/* composite the images */
		pixman_composite
			(SDRegionType_PixmanOperator[op->_base.type], li, 0, ri,
			 0, 0, 0, 0, 0, 0, rast->width, rast->height);

		/* free the left operand */
		pixman_image_destroy(li);

		/* set the data */
		*data = ri;
	}

	/* return successfully */
	return SDStatus_OK;
}

static void
SDRegionRasterizer_FreeData(void *data)
{
	/* assertions */
	SDASSERT((data != 0));

	/* free the data */
	pixman_image_destroy((pixman_image_t *)data);
}

static const SDRegionInterpreterClass SDRegionRasterizer_Class =
{
	SDRegionRasterizer_Data,
	SDRegionRasterizer_Op,
	SDRegionRasterizer_FreeData
};

SDINTERNAL SDStatus
SDRegionRasterizer_Initialize(SDRegionRasterizer *_this,
                              SDAffineTransformF *transform,
                              SDFloat             width,
                              SDFloat             height,
                              SDBool              gray)
{
	/* assertions */
	SDASSERT((_this != 0));

	/* initialize the members */
	{
		/* initialize the format */
		if(gray)
		{
			_this->format = pixman_format_create(PIXMAN_FORMAT_NAME_A8);
		}
		else
		{
			_this->format = pixman_format_create(PIXMAN_FORMAT_NAME_A1);
		}

		/* ensure we have a format */
		SDStatus_Require((_this->format != 0), SDStatus_OutOfMemory);

		/* initialize the trapezoids */
		SDTrapezoids_Initialize(&(_this->trapezoids));

		/* initialize the filler */
		SDFiller_Initialize(&(_this->filler));

		/* initialize the array */
		SDPointArrayF_Initialize(&(_this->array));

		/* initialize the remaining members */
		_this->transform = transform;
		_this->size      = -1;
		_this->width     = width;
		_this->height    = height;
	}

	/* initialize the base */
	SDRegionInterpreter_Initialize
		((SDRegionInterpreter *)_this, &SDRegionRasterizer_Class);

	/* return successfully */
	return SDStatus_OK;
}

SDINTERNAL void
SDRegionRasterizer_Finalize(SDRegionRasterizer *_this)
{
	/* assertions */
	SDASSERT((_this != 0));

	/* finalize the members */
	{
		/* finalize the base */
		SDRegionInterpreter_Finalize((SDRegionInterpreter *)_this);

		/* finalize the trapezoids */
		SDTrapezoids_Finalize(&(_this->trapezoids));

		/* finalize the filler */
		SDFiller_Finalize(&(_this->filler));

		/* finalize the array */
		SDPointArrayF_Finalize(&(_this->array));

		/* free the format */
		pixman_format_destroy(_this->format);

		/* finalize the transform */
		_this->transform = 0;
	}
}


#ifdef __cplusplus
};
#endif
