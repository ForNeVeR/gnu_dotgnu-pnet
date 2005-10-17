/*
 * SDSurface.c - Surface implementation.
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

#include "CSurface.h"

#ifdef __cplusplus
extern "C" {
#endif

SDStatus
SDSurface_Lock(SDSurface *_this)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* lock this surface */
	SDMutex_Lock(_this->lock);

	/* return successfully */
	return SDStatus_OK;
}

SDStatus
SDSurface_Unlock(SDSurface *_this)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* unlock this surface */
	SDMutex_Unlock(_this->lock);

	/* return successfully */
	return SDStatus_OK;
}

SDStatus
SDSurface_GetBoundsF(SDSurface    *_this,
                     SDRectangleF *bounds)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a bounds pointer */
	SDStatus_Require((bounds != 0), SDStatus_ArgumentNull);

	/* set the width and height */
	SDRectangle_X(*bounds)      = _this->x;
	SDRectangle_Y(*bounds)      = _this->y;
	SDRectangle_Width(*bounds)  = _this->width;
	SDRectangle_Height(*bounds) = _this->height;

	/* return successfully */
	return SDStatus_OK;
}

SDStatus
SDSurface_GetBounds(SDSurface *_this,
                    SDUInt32  *x,
                    SDUInt32  *y,
                    SDUInt32  *width,
                    SDUInt32  *height)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have bounds pointers */
	SDStatus_Require((x      != 0), SDStatus_ArgumentNull);
	SDStatus_Require((y      != 0), SDStatus_ArgumentNull);
	SDStatus_Require((width  != 0), SDStatus_ArgumentNull);
	SDStatus_Require((height != 0), SDStatus_ArgumentNull);

	/* set the width and height */
	*x      = _this->x;
	*y      = _this->y;
	*width  = _this->width;
	*height = _this->height;

	/* return successfully */
	return SDStatus_OK;
}

SDStatus
SDSurface_SetBounds(SDSurface *_this,
                    SDUInt32   x,
                    SDUInt32   y,
                    SDUInt32   width,
                    SDUInt32   height)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* bail out now if nothing has changed */
	if(x     == _this->x     && y      == _this->y &&
	   width == _this->width && height == _this->height)
	{
		return SDStatus_OK;
	}

	/* ensure the width is within bounds */
	SDStatus_Require
		((width > 0 && (x + width) < 32768), SDStatus_ArgumentOutOfRange);

	/* ensure the height is within bounds */
	SDStatus_Require
		((height > 0 && (y + height) < 32768), SDStatus_ArgumentOutOfRange);

	/* set the width and height */
	_this->x      = x;
	_this->y      = y;
	_this->width  = width;
	_this->height = height;

	/* finalize the clip mask, as needed */
	if(_this->clip != 0)
	{
		pixman_image_destroy(_this->clip);
		_this->clip = 0;
	}

	/* finalize the compositing mask, as needed */
	if(_this->comp != 0)
	{
		pixman_image_destroy(_this->comp);
		_this->comp = 0;
	}

	/* return successfully */
	return SDStatus_OK;
}

SDINTERNAL SDStatus
SDSurface_GetClipMask(SDSurface        *_this,
                      pixman_image_t  **mask,
                      SDBool            gray)
{
	/* declarations */
	pixman_format_t *format;

	/* assertions */
	SDASSERT((_this != 0));
	SDASSERT((mask  != 0));

	/* bail out now if there's nothing to do */
	if(_this->clip != 0 && !(_this->maskFlags & SDSurface_ClipMask8) == !gray)
	{
		*mask = _this->clip;
		return SDStatus_OK;
	}

	/* dispose of the clip mask, as needed */
	if(_this->clip != 0)
	{
		pixman_image_destroy(_this->clip);
		_this->clip = 0;
	}

	/* create the pixman format */
	if(gray)
	{
		format = pixman_format_create(PIXMAN_FORMAT_NAME_A8);
		_this->maskFlags |= SDSurface_ClipMask8;
	}
	else
	{
		format = pixman_format_create(PIXMAN_FORMAT_NAME_A1);
		_this->maskFlags &= ~SDSurface_ClipMask8;
	}

	/* ensure we have a format */
	SDStatus_Require((format != 0), SDStatus_OutOfMemory);

	/* create the pixman image */
	*mask = pixman_image_create(format, _this->width, _this->height);

	/* dispose of the format */
	pixman_format_destroy(format);

	/* ensure we have an image */
	SDStatus_Require((*mask != 0), SDStatus_OutOfMemory);

	/* set the clip mask */
	_this->clip = *mask;

	/* return successfully */
	return SDStatus_OK;
}

SDINTERNAL SDStatus
SDSurface_GetCompositingMask(SDSurface       *_this,
                             pixman_image_t **mask,
                             SDBool           gray)
{
	/* declarations */
	pixman_format_t *format;

	/* assertions */
	SDASSERT((_this != 0));
	SDASSERT((mask  != 0));

	/* bail out now if there's nothing to do */
	if(_this->comp != 0 && !(_this->maskFlags & SDSurface_CompMask8) == !gray)
	{
		*mask = _this->comp;
		return SDStatus_OK;
	}

	/* dispose of the compositing mask, as needed */
	if(_this->comp != 0)
	{
		pixman_image_destroy(_this->comp);
		_this->comp = 0;
	}

	/* create the pixman format */
	if(gray)
	{
		format = pixman_format_create(PIXMAN_FORMAT_NAME_A8);
		_this->maskFlags |= SDSurface_CompMask8;
	}
	else
	{
		format = pixman_format_create(PIXMAN_FORMAT_NAME_A1);
		_this->maskFlags &= ~SDSurface_CompMask8;
	}

	/* ensure we have a format */
	SDStatus_Require((format != 0), SDStatus_OutOfMemory);

	/* create the pixman image */
	*mask = pixman_image_create(format, _this->width, _this->height);

	/* dispose of the format */
	pixman_format_destroy(format);

	/* ensure we have an image */
	SDStatus_Require((*mask != 0), SDStatus_OutOfMemory);

	/* set the compositing mask */
	_this->comp = *mask;

	/* return successfully */
	return SDStatus_OK;
}

SDStatus
SDSurface_Reference(SDSurface *_this)
{
	/* assertions */
	SDASSERT((_this != 0));

	/* update the reference count synchronously */
	SDSurface_Lock(_this);
	{
		++(_this->refCount);
	}
	SDSurface_Unlock(_this);

	/* return successfully */
	return SDStatus_OK;
}

SDStatus
SDSurface_Destroy(SDSurface **_this)
{
	/* declarations */
	SDMutex *lock;
	SDBool   lockOwner;

	/* ensure we have a this pointer pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a this pointer */
	SDStatus_Require((*_this != 0), SDStatus_ArgumentNull);

	/* get the lock */
	lock = (*_this)->lock;

	/* finalize this surface synchronously */
	SDMutex_Lock(lock);
	{
		/* update the reference count */
		--((*_this)->refCount);

		/* finalize, as needed */
		if((*_this)->refCount != 0)
		{
			lockOwner = 0;
		}
		else
		{
			/* set the lock ownership flag */
			lockOwner = 1;

			/* finalize the clip mask, as needed */
			if((*_this)->clip != 0)
			{
				pixman_image_destroy((*_this)->clip);
			}

			/* finalize the compositing mask, as needed */
			if((*_this)->comp != 0)
			{
				pixman_image_destroy((*_this)->comp);
			}

			/* finalize the surface */
			(*_this)->_class->Finalize(*_this);

			/* dispose of the surface */
			SDFree(*_this);
		}
	}
	SDMutex_Unlock(lock);

	/* destroy the lock, as needed */
	if(lockOwner) { SDMutex_Destroy(&lock); }

	/* null the surface pointer */
	*_this = 0;

	/* return successfully */
	return SDStatus_OK;
}

SDStatus
SDSurface_Composite(SDSurface           *_this,
                    SDUInt32             x,
                    SDUInt32             y,
                    SDUInt32             width,
                    SDUInt32             height,
                    pixman_image_t      *src,
                    pixman_image_t      *mask,
                    SDInterpolationMode  interpolationMode,
                    SDCompositingMode    compositingMode)
{
	/* declarations */
	pixman_operator_t op;

	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a source pointer */
	SDStatus_Require((src != 0), SDStatus_ArgumentNull);

	/* ensure we have a mask pointer */
	SDStatus_Require((mask != 0), SDStatus_ArgumentNull);

	/* adjust bounds of composite */
	{
		/* get the bounds */
		SDUInt32 x1 = _this->x;
		SDUInt32 y1 = _this->y;
		SDUInt32 x2 = _this->width  + x1;
		SDUInt32 y2 = _this->height + x2;

		/* calculate the new bounds */
		if(x            > x1) { x1 = x; }
		if(y            > y1) { y1 = y; }
		if((x + width)  < x2) { x2 = (x + width); }
		if((y + height) < y2) { y2 = (y + height); }

		/* bail out now if there's nothing to do */
		SDStatus_Require((x1 < x2 && y1 < y2), SDStatus_OK);

		/* set the bounds */
		x      = x1;
		y      = y1;
		width  = (x2 - x1);
		height = (y2 - y1);
	}

	/* set the filter */
	switch(interpolationMode)
	{
		case SDInterpolationMode_Bicubic:
		case SDInterpolationMode_HighQuality:
		case SDInterpolationMode_HighQualityBilinear:
		case SDInterpolationMode_HighQualityBicubic:
			{ pixman_image_set_filter(src, PIXMAN_FILTER_BEST); } break;

		case SDInterpolationMode_NearestNeighbor:
			{ pixman_image_set_filter(src, PIXMAN_FILTER_NEAREST); } break;

		case SDInterpolationMode_Bilinear:
			{ pixman_image_set_filter(src, PIXMAN_FILTER_BILINEAR); } break;

		case SDInterpolationMode_LowQuality:
			{ pixman_image_set_filter(src, PIXMAN_FILTER_FAST); } break;

		case SDInterpolationMode_Default:
		default:
			{ pixman_image_set_filter(src, PIXMAN_FILTER_GOOD); } break;
	}

	/* set the operator */
	switch(compositingMode)
	{
		case SDCompositingMode_SourceCopy:
			{ op = PIXMAN_OPERATOR_SRC; } break;

		case SDCompositingMode_Xor:
			{ op = PIXMAN_OPERATOR_XOR; } break;

		case SDCompositingMode_SourceOver:
		default:
			{ op = PIXMAN_OPERATOR_OVER; } break;
	}

	/* perform the compositing operation */
	SDStatus_Check
		(_this->_class->Composite
			(_this, x, y, width, height, src, mask, op));

	/* return successfully */
	return SDStatus_OK;
}

SDStatus
SDSurface_Clear(SDSurface *_this,
                SDColor    color)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* perform the compositing operation */
	SDStatus_Check
		(_this->_class->Clear
			(_this, color));

	/* return successfully */
	return SDStatus_OK;
}

SDStatus
SDSurface_Flush(SDSurface        *_this,
                SDFlushIntention  intention)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* perform the compositing operation */
	SDStatus_Check
		(_this->_class->Flush
			(_this, intention));

	/* return successfully */
	return SDStatus_OK;
}

SDINTERNAL SDStatus
SDSurface_Initialize(SDSurface            *_this,
                     const SDSurfaceClass *_class,
                     SDUInt32              x,
                     SDUInt32              y,
                     SDUInt32              width,
                     SDUInt32              height)
{
	/* assertions */
	SDASSERT((_this != 0));

	/* set the bounds */
	_this->x      = x;
	_this->y      = y;
	_this->width  = width;
	_this->height = height;

	/* set the class */
	_this->_class = _class;

	/* set the reference count */
	_this->refCount = 1;

	/* set the masks and mask flags */
	_this->clip      = 0;
	_this->comp      = 0;
	_this->maskFlags = 0;

	/* create the lock */
	return SDMutex_Create(&(_this->lock));
}

#ifdef __cplusplus
};
#endif
