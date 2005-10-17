/*
 * SDX11Surface.h - X11 surface header.
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

#ifdef HAVE_X11_XLIB_H

#ifndef _SD_X11SURFACE_H_
#define _SD_X11SURFACE_H_

#include "CSurface.h"

#ifdef __cplusplus
extern "C" {
#endif

struct _tagSDX11Surface
{
	SDSurface  _base;
	Display   *dpy;
	Screen    *screen;
	Visual    *visual;
	GC         gc;
	Drawable   drawable;
	SDFloat    dpi;
	int        depth;
};

static SDStatus
SDX11Surface_Composite(SDSurface         *_this,
                       SDUInt32           x,
                       SDUInt32           y,
                       SDUInt32           width,
                       SDUInt32           height,
                       pixman_image_t    *src,
                       pixman_image_t    *mask,
                       pixman_operator_t  op);
static SDStatus
SDX11Surface_Clear(SDSurface *_this,
                   SDColor    color);
static SDStatus
SDX11Surface_Flush(SDSurface        *_this,
                   SDFlushIntention  intention);
static void
SDX11Surface_Finalize(SDSurface *_this);


static const SDSurfaceClass SDX11Surface_Class =
{
	SDX11Surface_Composite,
	SDX11Surface_Clear,
	SDX11Surface_Flush,
	SDX11Surface_Finalize,
	"sentinel"
};

#ifdef __cplusplus
};
#endif

#endif /* _SD_X11SURFACE_H_ */

#endif /* HAVE_X11_XLIB_H */
