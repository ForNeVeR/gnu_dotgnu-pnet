/*
 * SDX11Surface.c - X11 surface implementation.
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

#include "CX11Surface.h"
#include "CUtils.h"
#include <X11/Xutil.h>

#ifdef __cplusplus
extern "C" {
#endif

/* TODO: use XRender when available */

static SDStatus
SDX11Surface_GetImages(SDX11Surface    *_this,
                       SDUInt32         x,
                       SDUInt32         y,
                       SDUInt32         width,
                       SDUInt32         height,
                       XImage         **imageX,
                       pixman_image_t **imageP)
{
	/* declarations */
	Pixmap           pixmap;
	pixman_format_t *format;

	/* assertions */
	SDASSERT((_this  != 0));
	SDASSERT((imageX != 0));
	SDASSERT((imageP != 0));

	/* create the pixmap */
	pixmap =
		XCreatePixmap
			(_this->dpy, _this->drawable, width, height, _this->depth);

	/* ensure we have a graphics context */
	if(_this->gc == 0)
	{
		/* declarations */
		XGCValues v;

		/* disable expose events */
		v.graphics_exposures = False;

		/* create the graphics context */
		_this->gc =
			XCreateGC
				(_this->dpy, _this->drawable, GCGraphicsExposures, &v);
	}

	/* copy the surface to the pixmap */
	XCopyArea
		(_this->dpy, _this->drawable, pixmap, _this->gc,
		 x, y, width, height, 0, 0);

	/* get the image data */
	*imageX =
		XGetImage
			(_this->dpy, pixmap, 0, 0, width, height, AllPlanes, ZPixmap);

	/* dispose of the pixmap */
	XFreePixmap(_this->dpy, pixmap);

	/* ensure we have the image data */
	SDStatus_Require((*imageX != 0), SDStatus_OutOfMemory);

	/* create the pixman format */
	format =
		pixman_format_create_masks
			((*imageX)->bits_per_pixel, 0, _this->visual->red_mask,
			 _this->visual->green_mask, _this->visual->blue_mask);

	/* ensure we have a format */
	if(format == 0)
	{
		XDestroyImage(*imageX);
		return SDStatus_OutOfMemory;
	}

	/* create the pixman image */
	*imageP =
		pixman_image_create_for_data
			((pixman_bits_t *)(*imageX)->data, format, (*imageX)->width,
			 (*imageX)->height, (*imageX)->bits_per_pixel,
			 (*imageX)->bytes_per_line);

	/* dispose of the format */
	pixman_format_destroy(format);

	/* ensure we have an image */
	if(imageP == 0)
	{
		XDestroyImage(*imageX);
		return SDStatus_OutOfMemory;
	}

	/* return successfully */
	return SDStatus_OK;
}

static SDStatus
SDX11Surface_Composite(SDSurface         *_this,
                       SDUInt32           x,
                       SDUInt32           y,
                       SDUInt32           width,
                       SDUInt32           height,
                       pixman_image_t    *src,
                       pixman_image_t    *mask,
                       pixman_operator_t  op)
{
	/* declarations */
	SDX11Surface *surface;

	/* assertions */
	SDASSERT((_this != 0));
	SDASSERT((src   != 0));

	/* get this as an x surface */
	surface = (SDX11Surface *)_this;

	/* perform the composite */
	{
		/* declarations */
		XImage          *imageX;
		pixman_image_t  *imageP;

		/* create the images */
		SDStatus_Check
			(SDX11Surface_GetImages
				(surface, x, y, width, height, &imageX, &imageP));

		/* perform the composite */
		pixman_composite
			(op, src, mask, imageP, 0, 0, 0, 0, 0, 0, width, height);

		/* dispose of the pixman image */
		pixman_image_destroy(imageP);

		/* push the image data back to the server */
		XPutImage
			(surface->dpy, surface->drawable, surface->gc, imageX, 0, 0,
			 x, y, imageX->width, imageX->height);

		/* dispose of the x image */
		XDestroyImage(imageX);
	}

	/* return successfully */
	return SDStatus_OK;
}

static SDStatus
SDX11Surface_Clear(SDSurface *_this,
                   SDColor    color)
{
	/* declarations */
	SDX11Surface *surface;

	/* assertions */
	SDASSERT((_this != 0));

	/* get this as an x surface */
	surface = (SDX11Surface *)_this;

	/* clear the surface */
	{
		/* declarations */
		XImage          *imageX;
		pixman_image_t  *imageP;
		pixman_color_t   pixel;

		/* create the pixel */
		pixel = SDUtils_ToPixmanColor(color);

		/* create the images */
		SDStatus_Check
			(SDX11Surface_GetImages
				(surface, 0, 0, _this->width, _this->height, &imageX, &imageP));

		/* perform the clear */
		pixman_fill_rectangle
			(PIXMAN_OPERATOR_SRC, imageP, &pixel, _this->x, _this->y,
			 _this->width, _this->height);

		/* dispose of the pixman image */
		pixman_image_destroy(imageP);

		/* push the image data back to the server */
		XPutImage
			(surface->dpy, surface->drawable, surface->gc, imageX, 0, 0,
			 _this->x, _this->y, imageX->width, imageX->height);

		/* dispose of the x image */
		XDestroyImage(imageX);
	}

	/* return successfully */
	return SDStatus_OK;
}

static SDStatus
SDX11Surface_Flush(SDSurface        *_this,
                   SDFlushIntention  intention)
{
	/* declarations */
	SDX11Surface *surface;

	/* assertions */
	SDASSERT((_this != 0));

	/* get this as an x surface */
	surface = (SDX11Surface *)_this;

	/* flush the surface */
	{
		if(intention == SDFlushIntention_Flush)
		{
			XFlush(surface->dpy);
		}
		else
		{
			XSync(surface->dpy, False);
		}
	}

	/* return successfully */
	return SDStatus_OK;
}

static void
SDX11Surface_Finalize(SDSurface *_this)
{
	/* declarations */
	SDX11Surface *surface;

	/* assertions */
	SDASSERT((_this != 0));

	/* get this as an x surface */
	surface = (SDX11Surface *)_this;

	/* finalize the graphics context, as needed */
	if(surface->gc != 0)
	{
		XFreeGC(surface->dpy, surface->gc);
	}
}

SDStatus
SDX11Surface_Create(SDX11Surface **_this,
                    Display       *dpy,
                    Drawable       drawable,
                    Screen        *screen,
                    Visual        *visual,
                    SDUInt32       width,
                    SDUInt32       height)
{
	/* declarations */
	SDX11Surface *surface;

	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a display pointer */
	SDStatus_Require((dpy != 0), SDStatus_ArgumentNull);

	/* ensure we have a screen pointer */
	SDStatus_Require((screen != 0), SDStatus_ArgumentNull);

	/* ensure we have a visual pointer */
	SDStatus_Require((visual != 0), SDStatus_ArgumentNull);

	/* allocate the surface */
	if(!(*_this = (SDX11Surface *)SDMalloc(sizeof(SDX11Surface))))
	{
		return SDStatus_OutOfMemory;
	}

	/* get this as an x surface */
	surface = ((SDX11Surface *)(*_this));

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
				(surface, &SDX11Surface_Class, 0, 0, width, height);

		/* handle base initialization failures */
		if(status != SDStatus_OK)
		{
			SDFree(*_this);
			*_this = 0;
			return status;
		}
	}

	/* initialize the members */
	surface->dpy         = dpy;
	surface->screen      = screen;
	surface->visual      = visual;
	surface->gc          = 0;
	surface->drawable    = drawable;
	/* surface->dpi         = ; */

	/* get the depth */
	{
		/* declarations */
		Depth *currD;
		Depth *endD;

		/* get the depth pointer */
		currD = screen->depths;

		/* get the end depth pointer */
		endD = (currD + screen->ndepths);

		/* search for the matching depth */
		while(currD != endD)
		{
			/* declarations */
			Visual *currV;
			Visual *endV;

			/* get the visual pointer */
			currV = currD->visuals;

			/* get the end visual pointer */
			endV = (currV + currD->nvisuals);

			/* search for the matching visual */
			while(currV != endV)
			{
				/* set the depth, if the visual matches */
				if(currV == visual)
				{
					/* get the depth */
					surface->depth = currD->depth;

					/* break out of loops */
					currD = (endD - 1);
					currV = (endV - 1);
				}

				/* move to next visual position */
				++currV;
			}

			/* move to next depth position */
			++currD;
		}
	}

	/* return successfully */
	return SDStatus_OK;
}


#ifdef __cplusplus
};
#endif

#endif
