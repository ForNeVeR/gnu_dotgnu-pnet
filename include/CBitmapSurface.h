/*
 * SDBitmapSurface.h - Bitmap surface header.
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

#ifndef _SD_BITMAPSURFACE_H_
#define _SD_BITMAPSURFACE_H_

#include "CBitmap.h"
#include "CSurface.h"

#ifdef __cplusplus
extern "C" {
#endif

struct _tagSDBitmapSurface
{
	SDBitmap *image;
};

static SDStatus
SDBitmapSurface_Composite(SDSurface         *_this,
                          SDUInt32           x,
                          SDUInt32           y,
                          SDUInt32           width,
                          SDUInt32           height,
                          pixman_image_t    *src,
                          pixman_image_t    *mask,
                          pixman_operator_t  op);
static SDStatus
SDBitmapSurface_Clear(SDSurface *_this,
                      SDColor    color);
static SDStatus
SDBitmapSurface_Flush(SDSurface        *_this,
                      SDFlushIntention  intention);
static void
SDBitmapSurface_Finalize(SDSurface *_this);


static const SDSurfaceClass SDBitmapSurface_Class =
{
	SDBitmapSurface_Composite,
	SDBitmapSurface_Clear,
	SDBitmapSurface_Flush,
	SDBitmapSurface_Finalize,
	"sentinel"
};

#ifdef __cplusplus
};
#endif

#endif /* _SD_BITMAPSURFACE_H_ */
