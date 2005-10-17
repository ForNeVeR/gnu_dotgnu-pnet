/*
 * SDSurface.h - Surface header.
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

#ifndef _SD_SURFACE_H_
#define _SD_SURFACE_H_

#include "CrayonsInternal.h"
#include "CMutex.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _tagSDSurfaceClass SDSurfaceClass;

struct _tagSDSurface
{
	const SDSurfaceClass *_class;
	SDMutex              *lock;
	SDUInt32              refCount;
	SDUInt32              x;
	SDUInt32              y;
	SDUInt32              width;
	SDUInt32              height;
	pixman_image_t       *clip;
	pixman_image_t       *comp;
	SDUInt32              maskFlags;
};

struct _tagSDSurfaceClass
{
	/*\
	|*| Composite the image onto the surface.
	|*|
	|*|   _this  - this surface
	|*|   x      - destination x coordinate
	|*|   y      - destination y coordinate
	|*|   width  - width of composition
	|*|   height - height of composition
	|*|   src    - source image
	|*|   mask   - mask image
	|*|   op     - compositing operator
	|*|
	|*|  Returns status code.
	\*/
	SDStatus (*Composite)(SDSurface         *_this,
	                      SDUInt32           x,
	                      SDUInt32           y,
	                      SDUInt32           width,
	                      SDUInt32           height,
	                      pixman_image_t    *src,
	                      pixman_image_t    *mask,
	                      pixman_operator_t  op);

	/*\
	|*| Clear the surface.
	|*|
	|*|   _this - this surface
	|*|   color - background color
	|*|
	|*|  Returns status code.
	\*/
	SDStatus (*Clear)(SDSurface *_this,
	                  SDColor    color);

	/*\
	|*| Flush the surface.
	|*|
	|*|   _this     - this surface
	|*|   intention - flush operation
	|*|
	|*|  Returns status code.
	\*/
	SDStatus (*Flush)(SDSurface        *_this,
	                  SDFlushIntention  intention);

	/*\
	|*| Finalize the surface.
	|*|
	|*|   _this - this surface
	\*/
	void (*Finalize)(SDSurface *_this);

	/*\
	|*| Sentinel string to catch missing methods in class tables.
	\*/
	const char *sentinel;
};


SDINTERNAL SDStatus
SDSurface_GetClipMask(SDSurface        *_this,
                      pixman_image_t  **mask,
                      SDBool            gray);
SDINTERNAL SDStatus
SDSurface_GetCompositingMask(SDSurface       *_this,
                             pixman_image_t **mask,
                             SDBool           gray);
SDINTERNAL SDStatus
SDSurface_Initialize(SDSurface            *_this,
                     const SDSurfaceClass *_class,
                     SDUInt32              x,
                     SDUInt32              y,
                     SDUInt32              width,
                     SDUInt32              height);

#define SDSurface_ClipMask8 1
#define SDSurface_CompMask8 2

#ifdef __cplusplus
};
#endif

#endif /* _SD_SURFACE_H_ */
