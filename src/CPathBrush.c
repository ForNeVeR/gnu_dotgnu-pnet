/*
 * SDPathBrush.c - Path gradient brush implementation.
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

#include "CPathBrush.h"
#include "CMatrix.h"
#include "CPath.h"
#include "CUtils.h"

#ifdef __cplusplus
extern "C" {
#endif

/* TODO: implement more than a glorified solid brush */

static const SDPointF SDPointF_Zero = { 0.0f, 0.0f };

/* Initialize this path brush. */
static SDStatus
SDPathBrush_Initialize(SDPathBrush *_this,
                       SDPath      *path)
{
	/* assertions */
	SDASSERT((_this != 0));

	/* initialize the members */
	{
		/* initialize the path */
		SDStatus_Check
			(SDPath_Clone
				(path, &(_this->path)));

		/* find the center point and bounds */
		{
			/* declarations */
			SDFloat   maxX,  maxY;
			SDFloat   minX,  minY;
			SDFloat   sumX,  sumY;
			SDPointF *curr, *end;

			/*\
			|*| NOTE: directly accessing the path internals isn't exactly a
			|*|       clean way to do this, but it avoids making yet another
			|*|       copy of the data... this should probably be made into
			|*|       an internal access path method... this also only
			|*|       takes into account the polygonal bounds of curves,
			|*|       which may not be what we need
			\*/

			/* get the point pointer */
			curr = _this->path->points;

			/* get the end pointer */
			end = (curr + _this->path->count);

			/* ensure we have at least one point (for now) */
			if(curr == end)
			{
				SDPath_Destroy(&(_this->path));
				return SDStatus_Argument;
			}

			/* initialize the bounds and sums */
			sumX = maxX = minX = SDPoint_X(*curr);
			sumY = maxY = minY = SDPoint_Y(*curr);

			/* move to the next point position */
			++curr;

			/* calculate the bounds and sums */
			while(curr != end)
			{
				/* declarations */
				SDFloat x, y;

				/* get the current coordinates */
				x = SDPoint_X(*curr);
				y = SDPoint_Y(*curr);

				/* update the sums */
				sumX += x;
				sumY += y;

				/* update the bounds */
				maxX = ((x > maxX) ? x : maxX);
				maxY = ((y > maxY) ? y : maxY);
				minX = ((x < minX) ? x : minX);
				minY = ((y < minY) ? y : minY);
			}

			/* initialize the center point */
			SDPoint_X(_this->centerPoint) = (sumX / _this->path->count);
			SDPoint_Y(_this->centerPoint) = (sumY / _this->path->count);

			/* initialize the bounding rectangle */
			SDRectangle_X(_this->rectangle)      = (minX);
			SDRectangle_Y(_this->rectangle)      = (minY);
			SDRectangle_Width(_this->rectangle)  = (maxX - minX);
			SDRectangle_Height(_this->rectangle) = (maxY - minY);
		}

		/* allocate the surrounding colors */
		if(!(_this->surroundColors = (SDColor *)SDMalloc(sizeof(SDColor))))
		{
			SDPath_Destroy(&(_this->path));
			return SDStatus_OutOfMemory;
		}

		/* initialize the remaining members */
		_this->transform         = SDAffineTransformF_Identity;
		_this->wrapMode          = SDWrapMode_Clamp;
		_this->blend             = SDBlend_Zero;
		_this->colorBlend        = SDColorBlend_Zero;
		_this->focusPoint        = SDPointF_Zero;
		_this->centerColor       = SDColor_White;
		_this->surroundColors[0] = SDColor_White;
		_this->surroundCount     = 1;
	}

	/* initialize the base */
	SDBrush_Initialize((SDBrush *)_this, &SDPathBrush_Class);

	/* return successfully */
	return SDStatus_OK;
}

/* Finalize this path brush. */
static void
SDPathBrush_Finalize(SDBrush *_this)
{
	/* assertions */
	SDASSERT((_this != 0));

	/* finalize this brush */
	{
		/* declarations */
		SDPathBrush *brush;

		/* get this as a path brush */
		brush = (SDPathBrush *)_this;

		/* finalize the blend */
		SDBlend_Finalize(&(brush->blend));

		/* finalize the color blend */
		SDColorBlend_Finalize(&(brush->colorBlend));

		/* finalize the surrounding colors, as needed */
		if(brush->surroundColors != 0)
		{
			SDFree(brush->surroundColors);
			brush->surroundColors = 0;
			brush->surroundCount  = 0;
		}

		/* finalize the path, as needed */
		if(brush->path != 0)
		{
			SDPath_Destroy(&(brush->path));
		}
	}
}

/* Clone this path brush. */
static SDStatus
SDPathBrush_Clone(SDBrush  *_this,
                  SDBrush **_clone)
{
	/* assertions */
	SDASSERT((_this  != 0));
	SDASSERT((_clone != 0));

	/* clone this brush */
	{
		/* declarations */
		SDPathBrush *brush;
		SDPathBrush *clone;
		SDStatus     status;

		/* get this as a path brush */
		brush = (SDPathBrush *)_this;

		/* create the clone */
		SDStatus_Check
			(SDPathBrush_Create
				(((SDPathBrush **)_clone), brush->path));

		/* get the clone brush */
		clone = ((SDPathBrush *)(*_clone));

		/* copy the blend */
		status = SDBlend_Copy(&(brush->blend), &(clone->blend));

		/* handle blend copy failures */
		if(status != SDStatus_OK)
		{
			SDPathBrush_Finalize(*_clone);
			SDFree(*_clone);
			*_clone = 0;
			return status;
		}

		/* copy the color blend */
		status = SDColorBlend_Copy(&(brush->colorBlend), &(clone->colorBlend));

		/* handle color blend copy failures */
		if(status != SDStatus_OK)
		{
			SDPathBrush_Finalize(*_clone);
			SDFree(*_clone);
			*_clone = 0;
			return status;
		}

		/* copy the surrounding colors */
		status =
			SDPathBrush_SetSurroundColors
				(clone, brush->surroundColors, brush->surroundCount);

		/* handle surrounding colors copy failures */
		if(status != SDStatus_OK)
		{
			SDPathBrush_Finalize(*_clone);
			SDFree(*_clone);
			*_clone = 0;
			return status;
		}

		/* initialize the remaining clone members */
		clone->transform   = brush->transform;
		clone->wrapMode    = brush->wrapMode;
		clone->focusPoint  = brush->focusPoint;
		clone->centerColor = brush->centerColor;
	}

	/* return successfully */
	return SDStatus_OK;
}

/* Create a pattern for this brush. */
static SDStatus
SDPathBrush_CreatePattern(SDBrush   *_this,
                          SDPattern *pattern)
{
	/* declarations */
	SDPathBrush *brush;

	/* assertions */
	SDASSERT((_this   != 0));
	SDASSERT((pattern != 0));

	/* TODO: implement this the right way */

	/* get this as a path brush */
	brush = (SDPathBrush *)_this;

	/* set the pattern transformation */
	pattern->transform = 0;

	/* create the pattern */
	return SDUtils_CreateSolidPattern(&(pattern->image), brush->centerColor);
}

/* Create a path brush. */
SDStatus
SDPathBrush_Create(SDPathBrush **_this,
                   SDPath       *path)
{
	/* ensure we have a this pointer pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a path */
	SDStatus_Require((path != 0), SDStatus_ArgumentNull);

	/* allocate the brush */
	if(!(*_this = (SDPathBrush *)SDMalloc(sizeof(SDPathBrush))))
	{
		return SDStatus_OutOfMemory;
	}

	/* initialize the brush */
	{
		/* declarations */
		SDStatus status;

		/* initialize the brush */
		if((status = SDPathBrush_Initialize(*_this, path)) != SDStatus_OK)
		{
			SDFree(*_this);
			*_this = 0;
			return status;
		}
	}

	/* return successfully */
	return SDStatus_OK;
}

/* Get the gradient blend. */
SDStatus
SDPathBrush_GetBlend(SDPathBrush *_this,
                     SDBlend     *blend)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a blend pointer */
	SDStatus_Require((blend != 0), SDStatus_ArgumentNull);

	/* ensure we have a blend */
	SDStatus_Require((_this->blend.count > 0), SDStatus_InvalidOperation);

	/* get the blend */
	return SDBlend_Copy(&(_this->blend), blend);
}

/* Set the gradient blend. */
SDStatus
SDPathBrush_SetBlend(SDPathBrush *_this,
                     SDBlend      blend)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have factors */
	SDStatus_Require((blend.factors != 0), SDStatus_ArgumentNull);

	/* ensure we have positions */
	SDStatus_Require((blend.positions != 0), SDStatus_ArgumentNull);

	/* ensure the count is in range */
	SDStatus_Require((blend.count >= 2), SDStatus_ArgumentOutOfRange);

	/* set the blend */
	{
		/* declarations */
		SDBlend tmp;

		/* copy the blend */
		SDStatus_Check
			(SDBlend_Copy
				(&(blend), &(tmp)));

		/* dispose of the current blend as needed */
		if(_this->blend.count != 0)
		{
			SDBlend_Finalize(&(_this->blend));
		}
		else
		{
			SDColorBlend_Finalize(&(_this->colorBlend));
		}

		/* set the blend */
		_this->blend = tmp;
	}

	/* send change signal to base */
	SDBrush_OnChange((SDBrush *)_this);

	/* return successfully */
	return SDStatus_OK;
}

/* Get the center color of the gradient. */
SDStatus
SDPathBrush_GetCenterColor(SDPathBrush *_this,
                           SDColor     *centerColor)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a center color pointer */
	SDStatus_Require((centerColor != 0), SDStatus_ArgumentNull);

	/* get the center color */
	*centerColor = _this->centerColor;

	/* return successfully */
	return SDStatus_OK;
}

/* Set the center color of the gradient. */
SDStatus
SDPathBrush_SetCenterColor(SDPathBrush *_this,
                           SDColor      centerColor)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* set the center color */
	_this->centerColor = centerColor;

	/* send change signal to base */
	SDBrush_OnChange((SDBrush *)_this);

	/* return successfully */
	return SDStatus_OK;
}

/* Get the center point of the gradient. */
SDStatus
SDPathBrush_GetCenterPoint(SDPathBrush *_this,
                           SDPointF    *centerPoint)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a center point pointer */
	SDStatus_Require((centerPoint != 0), SDStatus_ArgumentNull);

	/* get the center point */
	*centerPoint = _this->centerPoint;

	/* return successfully */
	return SDStatus_OK;
}

/* Set the center point of the gradient. */
SDStatus
SDPathBrush_SetCenterPoint(SDPathBrush *_this,
                           SDPointF     centerPoint)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* set the center point */
	_this->centerPoint = centerPoint;

	/* send change signal to base */
	SDBrush_OnChange((SDBrush *)_this);

	/* return successfully */
	return SDStatus_OK;
}

/* Get the color blend of the gradient. */
SDStatus
SDPathBrush_GetColorBlend(SDPathBrush  *_this,
                          SDColorBlend *colorBlend)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a color blend pointer */
	SDStatus_Require((colorBlend != 0), SDStatus_ArgumentNull);

	/* ensure we have a color blend */
	SDStatus_Require((_this->colorBlend.count > 0), SDStatus_InvalidOperation);

	/* get the color blend */
	return SDColorBlend_Copy(&(_this->colorBlend), colorBlend);
}

/* Set the color blend of the gradient. */
SDStatus
SDPathBrush_SetColorBlend(SDPathBrush  *_this,
                          SDColorBlend  colorBlend)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have colors */
	SDStatus_Require((colorBlend.colors != 0), SDStatus_ArgumentNull);

	/* ensure we have positions */
	SDStatus_Require((colorBlend.positions != 0), SDStatus_ArgumentNull);

	/* ensure the count is in range */
	SDStatus_Require((colorBlend.count >= 2), SDStatus_ArgumentOutOfRange);

	/* set the color blend */
	{
		/* declarations */
		SDColorBlend tmp;

		/* copy the color blend */
		SDStatus_Check
			(SDColorBlend_Copy
				(&(colorBlend), &(tmp)));

		/* dispose of the current blend as needed */
		if(_this->colorBlend.count != 0)
		{
			SDColorBlend_Finalize(&(_this->colorBlend));
		}
		else
		{
			SDBlend_Finalize(&(_this->blend));
		}

		/* set the color blend */
		_this->colorBlend = tmp;
	}

	/* send change signal to base */
	SDBrush_OnChange((SDBrush *)_this);

	/* return successfully */
	return SDStatus_OK;
}

/* Get the focus point of the gradient. */
SDStatus
SDPathBrush_GetFocusPoint(SDPathBrush *_this,
                          SDPointF    *focusPoint)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a focus point pointer */
	SDStatus_Require((focusPoint != 0), SDStatus_ArgumentNull);

	/* get the focus point */
	*focusPoint = _this->focusPoint;

	/* return successfully */
	return SDStatus_OK;
}

/* Set the focus point of the gradient. */
SDStatus
SDPathBrush_SetFocusPoint(SDPathBrush *_this,
                          SDPointF     focusPoint)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* set the focus point */
	_this->focusPoint = focusPoint;

	/* send change signal to base */
	SDBrush_OnChange((SDBrush *)_this);

	/* return successfully */
	return SDStatus_OK;
}

/* Get the bounding rectangle of the gradient. */
SDStatus
SDPathBrush_GetRectangle(SDPathBrush  *_this,
                         SDRectangleF *rectangle)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a bounding rectangle pointer */
	SDStatus_Require((rectangle != 0), SDStatus_ArgumentNull);

	/* get the bounding rectangle */
	*rectangle = _this->rectangle;

	/* return successfully */
	return SDStatus_OK;
}

/* Get the surrounding colors of the gradient. */
SDStatus
SDPathBrush_GetSurroundColors(SDPathBrush  *_this,
                              SDColor     **surroundColors,
                              SDUInt32     *count)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a surrounding colors pointer pointer */
	SDStatus_Require((surroundColors != 0), SDStatus_ArgumentNull);

	/* ensure we have a count pointer */
	SDStatus_Require((count != 0), SDStatus_ArgumentNull);

	/* get the surrounding colors */
	{
		/* get the count */
		*count = _this->surroundCount;

		/* allocate the surrounding colors */
		*surroundColors = (SDColor *)SDMalloc(*count * sizeof(SDColor));

		/* ensure we have surrounding colors */
		SDStatus_Require((*surroundColors != 0), SDStatus_OutOfMemory);

		/* get the surrounding colors */
		SDMemCopy(*surroundColors, _this->surroundColors, *count);
	}

	/* return successfully */
	return SDStatus_OK;
}

/* Set the colors of the gradient path points. */
SDStatus
SDPathBrush_SetSurroundColors(SDPathBrush *_this,
                              SDColor     *surroundColors,
                              SDUInt32     count)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a surrounding colors pointer */
	SDStatus_Require((surroundColors != 0), SDStatus_ArgumentNull);

	/* set the surrounding colors */
	{
		/* declarations */
		SDUInt32  pc;
		SDColor  *tmp;

		/* get the path count */
		SDStatus_Check
			(SDPath_GetCount
				(_this->path, &pc));

		/* ensure the count is in range */
		SDStatus_Require
			((count != 0 && count < pc), SDStatus_ArgumentOutOfRange);

		/* allocate the surrounding colors */
		tmp = (SDColor *)SDMalloc(count * sizeof(SDColor));

		/* ensure we have surrounding colors */
		SDStatus_Require((tmp != 0), SDStatus_OutOfMemory);

		/* copy the surrounding colors */
		SDMemCopy(tmp, surroundColors, count);

		/* dispose of the current surrounding colors, as needed */
		if(_this->surroundCount != 0)
		{
			SDFree(_this->surroundColors);
		}

		/* set the surrounding colors */
		_this->surroundColors = tmp;

		/* set the surrounding colors count */
		_this->surroundCount = count;
	}

	/* send change signal to base */
	SDBrush_OnChange((SDBrush *)_this);

	/* return successfully */
	return SDStatus_OK;
}

/* Get the wrap mode of the gradient. */
SDStatus
SDPathBrush_GetWrapMode(SDPathBrush *_this,
                        SDWrapMode  *wrapMode)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a wrap mode pointer */
	SDStatus_Require((wrapMode != 0), SDStatus_ArgumentNull);

	/* get the wrap mode */
	*wrapMode = _this->wrapMode;

	/* return successfully */
	return SDStatus_OK;
}

/* Set the wrap mode of the gradient. */
SDStatus
SDPathBrush_SetWrapMode(SDPathBrush *_this,
                        SDWrapMode   wrapMode)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* set the wrap mode */
	_this->wrapMode = wrapMode;

	/* send change signal to base */
	SDBrush_OnChange((SDBrush *)_this);

	/* return successfully */
	return SDStatus_OK;
}

/* Get the transformation matrix of the gradient. */
SDStatus
SDPathBrush_GetTransform(SDPathBrush *_this,
                         SDMatrix    *matrix)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* get the transformation */
	return SDMatrix_SetTransform(matrix, &(_this->transform));
}

/* Multiply the transformation matrix of the gradient by another matrix. */
SDStatus
SDPathBrush_MultiplyTransform(SDPathBrush   *_this,
                              SDMatrix      *matrix,
                              SDMatrixOrder  order)
{
	/* declarations */
	SDAffineTransformF t;

	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* get the matrix transformation */
	SDStatus_Check
		(SDMatrix_GetTransform
			(matrix, &t));

	/* multiply the transformation */
	SDAffineTransformF_Multiply(&(_this->transform), &t, order);

	/* send change signal to base */
	SDBrush_OnChange((SDBrush *)_this);

	/* return successfully */
	return SDStatus_OK;
}

/* Reset the transformation matrix of the gradient. */
SDStatus
SDPathBrush_ResetTransform(SDPathBrush *_this)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* reset the transformation */
	_this->transform = SDAffineTransformF_Identity;

	/* send change signal to base */
	SDBrush_OnChange((SDBrush *)_this);

	/* return successfully */
	return SDStatus_OK;
}

/* Rotate the transformation matrix of the gradient. */
SDStatus
SDPathBrush_RotateTransform(SDPathBrush   *_this,
                            SDFloat        angle,
                            SDMatrixOrder  order)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* rotate the transformation */
	SDAffineTransformF_Rotate(&(_this->transform), angle, order);

	/* send change signal to base */
	SDBrush_OnChange((SDBrush *)_this);

	/* return successfully */
	return SDStatus_OK;
}

/* Scale the transformation matrix of the gradient. */
SDStatus
SDPathBrush_ScaleTransform(SDPathBrush   *_this,
                           SDFloat        sx,
                           SDFloat        sy,
                           SDMatrixOrder  order)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* scale the transformation */
	SDAffineTransformF_Scale(&(_this->transform), sx, sy, order);

	/* send change signal to base */
	SDBrush_OnChange((SDBrush *)_this);

	/* return successfully */
	return SDStatus_OK;
}

/* Set the shape of the gradient to a triangle. */
SDStatus
SDPathBrush_SetTriangularShape(SDPathBrush *_this,
                               SDFloat      focus,
                               SDFloat      scale)
{
	/* declarations */
	SDUInt32 count;

	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* clamp the focus and scale */
	SDCLAMP(focus, 0.0f, 1.0f);
	SDCLAMP(scale, 0.0f, 1.0f);

	/* set the count */
	if(focus == 0.0f || focus == 1.0f)
	{
		count = SDBlend_TriangularHalfCount;
	}
	else
	{
		count = SDBlend_TriangularFullCount;
	}

	/* allocate and dispose, as needed */
	if(_this->blend.count != count)
	{
		/* declarations */
		SDBlend tmp;

		/* initialize the blend */
		SDBlend_Initialize(&(tmp), count);

		/* dispose of the current blend as needed */
		if(_this->blend.count != 0)
		{
			SDBlend_Finalize(&(_this->blend));
		}
		else
		{
			SDColorBlend_Finalize(&(_this->colorBlend));
		}

		/* set the blend */
		_this->blend = tmp;
	}

	/* set the blend to the triangular shape */
	SDBlend_SetTriangularShape(&(_this->blend), focus, scale);

	/* send change signal to base */
	SDBrush_OnChange((SDBrush *)_this);

	/* return successfully */
	return SDStatus_OK;
}

/* Set the shape of the gradient to a sigma bell. */
SDStatus
SDPathBrush_SetSigmaBellShape(SDPathBrush *_this,
                              SDFloat      focus,
                              SDFloat      scale)
{
	/* declarations */
	SDUInt32 count;

	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* clamp the focus and scale */
	SDCLAMP(focus, 0.0f, 1.0f);
	SDCLAMP(scale, 0.0f, 1.0f);

	/* set the count */
	if(focus == 0.0f || focus == 1.0f)
	{
		count = SDBlend_SigmaBellHalfCount;
	}
	else
	{
		count = SDBlend_SigmaBellFullCount;
	}

	/* allocate and dispose, as needed */
	if(_this->blend.count != count)
	{
		/* declarations */
		SDBlend tmp;

		/* initialize the blend */
		SDBlend_Initialize(&(tmp), count);

		/* dispose of the current blend as needed */
		if(_this->blend.count != 0)
		{
			SDBlend_Finalize(&(_this->blend));
		}
		else
		{
			SDColorBlend_Finalize(&(_this->colorBlend));
		}

		/* set the blend */
		_this->blend = tmp;
	}

	/* set the blend to the sigma bell shape */
	SDBlend_SetSigmaBellShape(&(_this->blend), focus, scale);

	/* send change signal to base */
	SDBrush_OnChange((SDBrush *)_this);

	/* return successfully */
	return SDStatus_OK;
}

/* Set the transformation matrix of the gradient. */
SDStatus
SDPathBrush_SetTransform(SDPathBrush *_this,
                         SDMatrix    *matrix)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* set the transformation */
	SDStatus_Check
		(SDMatrix_GetTransform
			(matrix, &(_this->transform)));

	/* send change signal to base */
	SDBrush_OnChange((SDBrush *)_this);

	/* return successfully */
	return SDStatus_OK;
}

/* Translate the transformation matrix of the gradient. */
SDStatus
SDPathBrush_TranslateTransform(SDPathBrush   *_this,
                               SDFloat        dx,
                               SDFloat        dy,
                               SDMatrixOrder  order)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* translate the transformation */
	SDAffineTransformF_Translate(&(_this->transform), dx, dy, order);

	/* send change signal to base */
	SDBrush_OnChange((SDBrush *)_this);

	/* return successfully */
	return SDStatus_OK;
}


#ifdef __cplusplus
};
#endif
