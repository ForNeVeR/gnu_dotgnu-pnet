/*
 * SDLineBrush.c - Linear gradient brush implementation.
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

#include "CLineBrush.h"
#include "CMatrix.h"
#include "CUtils.h"

#ifdef __cplusplus
extern "C" {
#endif

/* TODO: implement more than a glorified solid brush */

static const SDRectangleF SDRectangleF_Zero = { 0.0f, 0.0f, 0.0f, 0.0f };

static void
SDLineBrush_Initialize(SDLineBrush  *_this,
                       SDRectangleF  rectangle,
                       SDColor       startColor,
                       SDColor       endColor,
                       SDFloat       angle,
                       SDBool        isAngleScalable,
                       SDWrapMode    wrapMode)
{
	/* assertions */
	SDASSERT((_this != 0));

	/* initialize the members */
	{
		_this->transform       = SDAffineTransformF_Identity;
		_this->rectangle       = SDRectangleF_Zero;
		_this->wrapMode        = wrapMode;
		_this->blend           = SDBlend_Zero;
		_this->colorBlend      = SDColorBlend_Zero;
		_this->startColor      = startColor;
		_this->endColor        = endColor;
		_this->angle           = angle;
		_this->isAngleScalable = (isAngleScalable != 0);
		_this->gammaCorrection = 0;
	}

	/* initialize the base */
	SDBrush_Initialize((SDBrush *)_this, &SDLineBrush_Class);
}

/* Finalize this line brush. */
static void
SDLineBrush_Finalize(SDBrush *_this)
{
	/* assertions */
	SDASSERT((_this != 0));

	/* finalize this brush */
	{
		/* declarations */
		SDLineBrush *brush;

		/* get this as a line brush */
		brush = (SDLineBrush *)_this;

		/* finalize the blend */
		SDBlend_Finalize(&(brush->blend));

		/* finalize the color blend */
		SDColorBlend_Finalize(&(brush->colorBlend));
	}
}

/* Clone this line brush. */
static SDStatus
SDLineBrush_Clone(SDBrush  *_this,
                  SDBrush **_clone)
{
	/* assertions */
	SDASSERT((_this  != 0));
	SDASSERT((_clone != 0));

	/* clone this brush */
	{
		/* declarations */
		SDLineBrush *brush;
		SDLineBrush *clone;
		SDStatus     status;

		/* allocate the clone */
		if(!(*_clone = (SDBrush *)SDMalloc(sizeof(SDLineBrush))))
		{
			return SDStatus_OutOfMemory;
		}

		/* get this as a line brush */
		brush = (SDLineBrush *)_this;

		/* get the clone brush */
		clone = ((SDLineBrush *)(*_clone));

		/* copy the blend */
		status = SDBlend_Copy(&(brush->blend), &(clone->blend));

		/* handle blend copy failures */
		if(status != SDStatus_OK)
		{
			SDFree(*_clone);
			*_clone = 0;
			return status;
		}

		/* copy the color blend */
		status = SDColorBlend_Copy(&(brush->colorBlend), &(clone->colorBlend));

		/* handle color blend copy failures */
		if(status != SDStatus_OK)
		{
			SDBlend_Finalize(&(clone->blend));
			SDFree(*_clone);
			*_clone = 0;
			return status;
		}

		/* initialize the remaining clone members */
		clone->transform       = brush->transform;
		clone->rectangle       = brush->rectangle;
		clone->wrapMode        = brush->wrapMode;
		clone->startColor      = brush->startColor;
		clone->endColor        = brush->endColor;
		clone->angle           = brush->angle;
		clone->isAngleScalable = brush->isAngleScalable;
		clone->gammaCorrection = brush->gammaCorrection;
		
	}

	/* return successfully */
	return SDStatus_OK;
}

/* Create a pattern for this brush. */
static SDStatus
SDLineBrush_CreatePattern(SDBrush   *_this,
                          SDPattern *pattern)
{
	/* declarations */
	SDLineBrush *brush;

	/* assertions */
	SDASSERT((_this   != 0));
	SDASSERT((pattern != 0));

	/* TODO: implement this the right way */

	/* get this as a line brush */
	brush = (SDLineBrush *)_this;

	/* set the pattern transformation */
	pattern->transform = 0;

	/* create the pattern */
	return SDUtils_CreateSolidPattern(&(pattern->image), brush->startColor);
}

/* Create a line brush. */
SDStatus
SDLineBrush_Create(SDLineBrush  **_this,
                   SDRectangleF   rectangle,
                   SDColor        startColor,
                   SDColor        endColor,
                   SDFloat        angle,
                   SDBool         isAngleScalable,
                   SDWrapMode     wrapMode)
{
	/* ensure we have a this pointer pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* allocate the brush */
	if(!(*_this = (SDLineBrush *)SDMalloc(sizeof(SDLineBrush))))
	{
		return SDStatus_OutOfMemory;
	}

	/* initialize the brush */
	SDLineBrush_Initialize
		(*_this,
		 rectangle,
		 startColor,
		 endColor,
		 angle,
		 isAngleScalable,
		 wrapMode);

	/* return successfully */
	return SDStatus_OK;
}

/* Get the gradient blend. */
SDStatus
SDLineBrush_GetBlend(SDLineBrush *_this,
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
SDLineBrush_SetBlend(SDLineBrush *_this,
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

/* Get the start and end colors of the gradient. */
SDStatus
SDLineBrush_GetColors(SDLineBrush *_this,
                      SDColor     *startColor,
                      SDColor     *endColor)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a start color pointer */
	SDStatus_Require((startColor != 0), SDStatus_ArgumentNull);

	/* ensure we have an end color pointer */
	SDStatus_Require((endColor != 0), SDStatus_ArgumentNull);

	/* get the color */
	*startColor = _this->startColor;
	*endColor   = _this->endColor;

	/* return successfully */
	return SDStatus_OK;
}

/* Set the start and end colors of the gradient. */
SDStatus
SDLineBrush_SetColor(SDLineBrush *_this,
                     SDColor      startColor,
                     SDColor      endColor)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* set the colors */
	_this->startColor = startColor;
	_this->endColor   = endColor;

	/* send change signal to base */
	SDBrush_OnChange((SDBrush *)_this);

	/* return successfully */
	return SDStatus_OK;
}

/* Get the color blend of the gradient. */
SDStatus
SDLineBrush_GetColorBlend(SDLineBrush  *_this,
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
SDLineBrush_SetColorBlend(SDLineBrush  *_this,
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

/* Get the gamma correction flag of the gradient. */
SDStatus
SDLineBrush_GetGammaCorrection(SDLineBrush *_this,
                               SDBool      *gammaCorrection)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a gamma correction flag pointer */
	SDStatus_Require((gammaCorrection != 0), SDStatus_ArgumentNull);

	/* get the gamma correction flag */
	*gammaCorrection = _this->gammaCorrection;

	/* return successfully */
	return SDStatus_OK;
}

/* Set the gamma correction flag of the gradient. */
SDStatus
SDLineBrush_SetGammaCorrection(SDLineBrush *_this,
                               SDBool       gammaCorrection)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* set the gamma correction flag */
	_this->gammaCorrection = gammaCorrection;

	/* send change signal to base */
	SDBrush_OnChange((SDBrush *)_this);

	/* return successfully */
	return SDStatus_OK;
}

/* Get the bounding rectangle of the gradient. */
SDStatus
SDLineBrush_GetRectangle(SDLineBrush  *_this,
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

/* Get the wrap mode of the gradient. */
SDStatus
SDLineBrush_GetWrapMode(SDLineBrush *_this,
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
SDLineBrush_SetWrapMode(SDLineBrush *_this,
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
SDLineBrush_GetTransform(SDLineBrush *_this,
                         SDMatrix    *matrix)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* get the transformation */
	return SDMatrix_SetTransform(matrix, &(_this->transform));
}

/* Multiply the transformation matrix of the gradient by another matrix. */
SDStatus
SDLineBrush_MultiplyTransform(SDLineBrush   *_this,
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
SDLineBrush_ResetTransform(SDLineBrush *_this)
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
SDLineBrush_RotateTransform(SDLineBrush   *_this,
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
SDLineBrush_ScaleTransform(SDLineBrush   *_this,
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
SDLineBrush_SetTriangularShape(SDLineBrush *_this,
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
SDLineBrush_SetSigmaBellShape(SDLineBrush *_this,
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
SDLineBrush_SetTransform(SDLineBrush *_this,
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
SDLineBrush_TranslateTransform(SDLineBrush   *_this,
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
