/*
 * SDTextureBrush.c - Texture brush implementation.
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

#include "CTextureBrush.h"
#include "CImage.h"
#include "CMatrix.h"
#include "CUtils.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Clone this texture brush. */
static SDStatus
SDTextureBrush_Clone(SDBrush  *_this,
                     SDBrush **_clone)
{
	/* declarations */
	SDTextureBrush  *brush;
	SDTextureBrush **clone;

	/* assertions */
	SDASSERT((_this != 0));
	SDASSERT((clone != 0));

	/* get this as a texture brush */
	brush = (SDTextureBrush *)_this;

	/* get the clone as a texture brush */
	clone = (SDTextureBrush **)_clone;

	/* create the clone */
	SDStatus_Check
		(SDTextureBrush_Create
			(clone, brush->image, brush->rectangle, brush->wrapMode));

	/* initialize the transformation of the clone */
	(*clone)->transform = brush->transform;

	/* return successfully */
	return SDStatus_OK;
}

/* Finalize this texture brush. */
static void
SDTextureBrush_Finalize(SDBrush *_this)
{
	/* declarations */
	SDTextureBrush *brush;

	/* assertions */
	SDASSERT((_this != 0));

	/* get this as a texture brush */
	brush = (SDTextureBrush *)_this;

	/* finalize the image */
	SDImage_Destroy(&(brush->image));
}

/* Create a pattern for this brush. */
static SDStatus
SDTextureBrush_CreatePattern(SDBrush   *_this,
                             SDPattern *pattern)
{
	/* assertions */
	SDASSERT((_this   != 0));
	SDASSERT((pattern != 0));

	/* create the pattern */
	{
		/* declarations */
		SDUInt32         x;
		SDUInt32         y;
		SDUInt32         w;
		SDUInt32         h;
		SDStatus         status;
		SDTextureBrush  *brush;
		pixman_format_t *format;

		/* get this as a texture brush */
		brush = (SDTextureBrush *)_this;

		/* set the pattern transformation */
		pattern->transform = &(brush->transform);

		/* get the rectangle dimensions */
		x = (SDUInt32)SDRectangle_X(brush->rectangle);
		y = (SDUInt32)SDRectangle_Y(brush->rectangle);
		w = (SDUInt32)SDRectangle_Width(brush->rectangle);
		h = (SDUInt32)SDRectangle_Height(brush->rectangle);

		/* TODO: handle different wrapping modes */

		/* create the pixman format */
		format = pixman_format_create(PIXMAN_FORMAT_NAME_ARGB32);

		/* ensure we have a format */
		SDStatus_Require((format != 0), SDStatus_OutOfMemory);

		/* create the pixman image */
		pattern->image = pixman_image_create(format, w, h);

		/* ensure we have an image */
		SDStatus_Require((pattern->image != 0), SDStatus_OutOfMemory);

		/* copy the image data */
		status =
			SDUtils_PixmanImageRectangle
				(brush->image->image, pattern->image, x, y, w, h);

		/* handle copying failures */
		if(status != SDStatus_OK)
		{
			pixman_image_destroy(pattern->image);
			pattern->image = 0;
			return status;
		}
	}

	/* return successfully */
	return SDStatus_OK;
}

/* Intialize this texture brush. */
static SDStatus
SDTextureBrush_Initialize(SDTextureBrush *_this,
                          SDImage        *image,
                          SDRectangleF    rectangle,
                          SDWrapMode      wrapMode)
{
	/* assertions */
	SDASSERT((_this != 0));
	SDASSERT((image != 0));

	/* initialize the members */
	{
		/* clone the image */
		SDStatus_Check
			(SDImage_Clone
				(image, &(_this->image)));

		/* initialize the base */
		SDBrush_Initialize((SDBrush *)_this, &SDTextureBrush_Class);

		/* initialize the remaining members */
		_this->rectangle = rectangle;
		_this->wrapMode  = wrapMode;
		_this->transform = SDAffineTransformF_Identity;
	}

	/* return successfully */
	return SDStatus_OK;
}

/* Create a texture brush. */
SDStatus
SDTextureBrush_Create(SDTextureBrush **_this,
                      SDImage         *image,
                      SDRectangleF     rectangle,
                      SDWrapMode       wrapMode)
{
	/* ensure we have a this pointer pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* allocate the brush */
	if(!(*_this = (SDTextureBrush *)SDMalloc(sizeof(SDTextureBrush))))
	{
		return SDStatus_OutOfMemory;
	}

	/* initialize the brush */
	SDStatus_Check
		(SDTextureBrush_Initialize
			(*_this, image, rectangle, wrapMode));

	/* return successfully */
	return SDStatus_OK;
}

/* Get the image of the texture. */
SDStatus
SDTextureBrush_GetImage(SDTextureBrush  *_this,
                        SDImage        **image)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have an image pointer pointer */
	SDStatus_Require((image != 0), SDStatus_ArgumentNull);

	/* get the image */
	return SDImage_Clone(_this->image, image);
}

/* Get the wrap mode of the texture. */
SDStatus
SDTextureBrush_GetWrapMode(SDTextureBrush *_this,
                           SDWrapMode     *wrapMode)
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

/* Set the wrap mode of the texture. */
SDStatus
SDTextureBrush_SetWrapMode(SDTextureBrush *_this,
                           SDWrapMode      wrapMode)
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

/* Get the transformation matrix of the texture. */
SDStatus
SDTextureBrush_GetTransform(SDTextureBrush *_this,
                            SDMatrix       *matrix)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* get the transformation */
	return SDMatrix_SetTransform(matrix, &(_this->transform));
}

/* Multiply the transformation matrix of the texture by another matrix. */
SDStatus
SDTextureBrush_MultiplyTransform(SDTextureBrush *_this,
                                 SDMatrix       *matrix,
                                 SDMatrixOrder   order)
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

/* Reset the transformation matrix of the texture. */
SDStatus
SDTextureBrush_ResetTransform(SDTextureBrush *_this)
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

/* Rotate the transformation matrix of the texture. */
SDStatus
SDTextureBrush_RotateTransform(SDTextureBrush *_this,
                               SDFloat         angle,
                               SDMatrixOrder   order)
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

/* Scale the transformation matrix of the texture. */
SDStatus
SDTextureBrush_ScaleTransform(SDTextureBrush *_this,
                              SDFloat         sx,
                              SDFloat         sy,
                              SDMatrixOrder   order)
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

/* Set the transformation matrix of the texture. */
SDStatus
SDTextureBrush_SetTransform(SDTextureBrush *_this,
                            SDMatrix       *matrix)
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

/* Translate the transformation matrix of the texture. */
SDStatus
SDTextureBrush_TranslateTransform(SDTextureBrush *_this,
                                  SDFloat         dx,
                                  SDFloat         dy,
                                  SDMatrixOrder   order)
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
