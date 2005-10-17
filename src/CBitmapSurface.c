/*
 * SDBitmapSurface.c - Bitmap surface implementation.
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

#include "CBitmapSurface.h"
#include "CUtils.h"

#ifdef __cplusplus
extern "C" {
#endif

SDStatus
SDBitmapSurface_Create(SDBitmapSurface **_this,
                       SDBitmap         *image)
{
	/* ensure we have a this pointer pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have an image pointer */
	SDStatus_Require((image != 0), SDStatus_ArgumentNull);

	/* allocate the bitmap surface */
	if(!(*_this = (SDBitmapSurface *)SDMalloc(sizeof(SDBitmapSurface))))
	{
		return SDStatus_OutOfMemory;
	}

	/* initialize the bitmap surface */
	if(!((*_this)->image = (SDBitmap *)SDImage_Reference((SDImage *)image)))
	{
		SDFree(*_this);
		*_this = 0;
		return SDStatus_OutOfMemory;
	}

	/* initialize the base */
	{
		/* declarations */
		SDStatus   status;
		SDSurface *surface;

		/* get this as a surface */
		surface = (SDSurface *)*_this;

		/* initialize the base */
		status =
			SDSurface_Initialize
				(surface, &SDBitmapSurface_Class, 0, 0, image->width,
				 image->height);

		/* handle base initialization failures */
		if(status != SDStatus_OK)
		{
			SDBitmapSurface_Finalize(surface);
			SDFree(*_this);
			*_this = 0;
			return status;
		}
	}

	/* return successfully */
	return SDStatus_OK;
}

static SDStatus
SDBitmapSurface_Composite(SDSurface         *_this,
                          SDUInt32           x,
                          SDUInt32           y,
                          SDUInt32           width,
                          SDUInt32           height,
                          pixman_image_t    *src,
                          pixman_image_t    *mask,
                          pixman_operator_t  op)
{
	/* declarations */
	SDBitmap *image;

	/* assertions */
	SDASSERT((_this != 0));
	SDASSERT((src   != 0));
	SDASSERT((mask  != 0));

	/* get the image */
	image = ((SDBitmapSurface *)_this)->image;

	/* perform the composite synchronously */
	SDMutex_Lock(image->lock);
	{
		/* ensure the image data isn't locked */
		if(image->locked)
		{
			SDMutex_Unlock(image->lock);
			return SDStatus_InvalidOperation_ImageLocked;
		}

		/* perform the composite */
		pixman_composite
			(op, src, mask, image->image, 0, 0, 0, 0, x, y, width, height);
	}
	SDMutex_Unlock(image->lock);

	/* return successfully */
	return SDStatus_OK;
}

static SDStatus
SDBitmapSurface_Clear(SDSurface *_this,
                      SDColor    color)
{
	/* declarations */
	SDBitmap       *image;
	pixman_color_t  pixel;

	/* assertions */
	SDASSERT((_this != 0));

	/* create the pixel */
	pixel = SDUtils_ToPixmanColor(color);

	/* get the image */
	image = ((SDBitmapSurface *)_this)->image;

	/* perform the clear synchronously */
	SDMutex_Lock(image->lock);
	{
		/* ensure the image data isn't locked */
		if(image->locked)
		{
			SDMutex_Unlock(image->lock);
			return SDStatus_InvalidOperation_ImageLocked;
		}

		/* clear the image */
		pixman_fill_rectangle
			(PIXMAN_OPERATOR_SRC, image->image, &pixel, _this->x, _this->y,
			 _this->width, _this->height);
	}
	SDMutex_Unlock(image->lock);

	/* return successfully */
	return SDStatus_OK;
}

static SDStatus
SDBitmapSurface_Flush(SDSurface        *_this,
                      SDFlushIntention  intention)
{
	/* assertions */
	SDASSERT((_this != 0));

	/* nothing to do here */

	/* return successfully */
	return SDStatus_OK;
}

static void
SDBitmapSurface_Finalize(SDSurface *_this)
{
	/* declarations */
	SDBitmapSurface *surface;

	/* assertions */
	SDASSERT((_this != 0));

	/* get this as a bitmap surface */
	surface = (SDBitmapSurface *)_this;

	/* finalize the bitmap surface */
	SDImage_Destroy((SDImage **)&(surface->image));
}


#ifdef __cplusplus
};
#endif
