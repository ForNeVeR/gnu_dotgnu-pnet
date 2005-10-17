/*
 * SDImage.h - Image header.
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

#ifndef _SD_IMAGE_H_
#define _SD_IMAGE_H_

#include "CrayonsInternal.h"
#include "CMutex.h"

#ifdef __cplusplus
extern "C" {
#endif

struct _tagSDImage
{
	SDImageType      type;
	SDImageFormat    format;
	SDImageFlag      flags;
	SDPixelFormat    pixelFormat;
	SDFloat          dpiX;
	SDFloat          dpiY;
	SDUInt32         width;
	SDUInt32         height;
#if 0
	SDPropertyItem  *propertyItems;
	SDUInt32         propertyItemCount;
	SDFrameInfo     *frameDimensions;
	SDUInt32         frameDimensionCount;
	SDFrame         *active;
	SDFrame         *activeBuffer;
#endif
	SDUInt32         bitmapDataX;
	SDUInt32         bitmapDataY;
	SDBitmapData    *bitmapData;
	SDBool           locked;
	SDColorPalette  *palette;
	pixman_image_t  *image;
	SDMutex         *lock;
	SDUInt32         refCount;
};

SDINTERNAL SDImage *
SDImage_Reference(SDImage *_this);

#ifdef __cplusplus
};
#endif

#endif /* _SD_IMAGE_H_ */
