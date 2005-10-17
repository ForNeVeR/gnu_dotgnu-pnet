/*
 * SDHatchBrush.c - Hatch brush implementation.
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

#include "CHatchBrush.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Clone this hatch brush. */
static SDStatus
SDHatchBrush_Clone(SDBrush  *_this,
                   SDBrush **_clone)
{
	/* declarations */
	SDHatchBrush  *brush;
	SDHatchBrush **clone;

	/* assertions */
	SDASSERT((_this != 0));
	SDASSERT((clone != 0));

	/* get this as a hatch brush */
	brush = (SDHatchBrush *)_this;

	/* get the clone as a hatch brush */
	clone = (SDHatchBrush **)_clone;

	/* create the clone */
	return
		SDHatchBrush_Create
			(clone, brush->style, brush->foreground, brush->background);
}

/* Finalize this hatch brush. */
static void
SDHatchBrush_Finalize(SDBrush *_this)
{
	/* assertions */
	SDASSERT((_this != 0));

	/* nothing to do here */
}

/* Create a pattern for this brush. */
static SDStatus
SDHatchBrush_CreatePattern(SDBrush   *_this,
                           SDPattern *pattern)
{
	/* assertions */
	SDASSERT((_this   != 0));
	SDASSERT((pattern != 0));

	/* create the pattern */
	{
		/* declarations */
		pixman_format_t *format;
		SDHatchBrush    *brush;
		SDByte          *data;
		SDColor          fg;
		SDColor          bg;
		SDUInt32         stride;

		/* get this as a hatch brush */
		brush = (SDHatchBrush *)_this;

		/* set the pattern transformation */
		pattern->transform = 0;

		/* calculate the pre-multiplied foreground */
		{
			/* declarations */
			SDByte a, r, g, b;

			/* get the foreground */
			fg = brush->foreground;

			/* get the components and pre-multiply */
			a = ((SDColor_A(fg)));
			r = ((SDColor_R(fg) * a) / 255);
			g = ((SDColor_G(fg) * a) / 255);
			b = ((SDColor_B(fg) * a) / 255);

			/* set the foreground */
			fg = SDPixmanPixel_FromARGB(a, r, g, b);
		}

		/* calculate the pre-multiplied background */
		{
			/* declarations */
			SDByte a, r, g, b;

			/* get the background */
			bg = brush->background;

			/* get the components and pre-multiply */
			a = ((SDColor_A(bg)));
			r = ((SDColor_R(bg) * a) / 255);
			g = ((SDColor_G(bg) * a) / 255);
			b = ((SDColor_B(bg) * a) / 255);

			/* set the background */
			bg = SDPixmanPixel_FromARGB(a, r, g, b);
		}

		/* create the pixman format */
		format = pixman_format_create(SDHatchBrush_DataFormat);

		/* ensure we have a format */
		SDStatus_Require((format != 0), SDStatus_OutOfMemory);

		/* create the pixman image */
		pattern->image =
			pixman_image_create
				(format, SDHatchBrush_StyleWidth, SDHatchBrush_StyleHeight);

		/* destroy the pixman format */
		pixman_format_destroy(format);

		/* ensure we have an image */
		SDStatus_Require((pattern->image != 0), SDStatus_OutOfMemory);

		/* get the data and stride */
		data = (SDByte *)pixman_image_get_data(pattern->image);
		stride = (SDUInt32)pixman_image_get_stride(pattern->image);

		/* get the image data for the style */
		SDHatchBrush_StyleToData(brush->style, data, stride, fg, bg);

		/* set the repeat flag */
		pixman_image_set_repeat(pattern->image, 1);
	}

	/* return successfully */
	return SDStatus_OK;
}

/* Create a hatch brush. */
SDStatus
SDHatchBrush_Create(SDHatchBrush **_this,
                    SDHatchStyle   style,
                    SDColor        foreground,
                    SDColor        background)
{
	/* ensure we have a this pointer pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a valid hatch style */
	SDStatus_Require((SDHatchStyle_IsValid(style)), SDStatus_Argument);

	/* allocate the brush */
	if(!(*_this = (SDHatchBrush *)SDMalloc(sizeof(SDHatchBrush))))
	{
		return SDStatus_OutOfMemory;
	}

	/* initialize the members */
	(*_this)->style      = style;
	(*_this)->foreground = foreground;
	(*_this)->background = background;

	/* initialize the base */
	SDBrush_Initialize((SDBrush *)(*_this), &SDHatchBrush_Class);

	/* return successfully */
	return SDStatus_OK;
}

/* Get the background color of the hatch. */
SDStatus
SDHatchBrush_GetBackgroundColor(SDHatchBrush *_this,
                                SDColor      *background)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a background color pointer */
	SDStatus_Require((background != 0), SDStatus_ArgumentNull);

	/* get the background color */
	*background = _this->background;

	/* return successfully */
	return SDStatus_OK;
}

/* Get the foreground color of the hatch. */
SDStatus
SDHatchBrush_GetForegroundColor(SDHatchBrush *_this,
                                SDColor      *foreground)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a foreground color pointer */
	SDStatus_Require((foreground != 0), SDStatus_ArgumentNull);

	/* get the foreground color */
	*foreground = _this->foreground;

	/* return successfully */
	return SDStatus_OK;
}

/* Get the style of the hatch. */
SDStatus
SDHatchBrush_GetHatchStyle(SDHatchBrush *_this,
                           SDHatchStyle *style)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a style pointer */
	SDStatus_Require((style != 0), SDStatus_ArgumentNull);

	/* get the style */
	*style = _this->style;

	/* return successfully */
	return SDStatus_OK;
}


#ifdef __cplusplus
};
#endif
