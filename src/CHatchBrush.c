/*
 * CHatchBrush.c - Hatch brush implementation.
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
static CStatus
CHatchBrush_Clone(CBrush  *_this,
                   CBrush **_clone)
{
	/* declarations */
	CHatchBrush  *brush;
	CHatchBrush **clone;

	/* assertions */
	CASSERT((_this != 0));
	CASSERT((clone != 0));

	/* get this as a hatch brush */
	brush = (CHatchBrush *)_this;

	/* get the clone as a hatch brush */
	clone = (CHatchBrush **)_clone;

	/* create the clone */
	return
		CHatchBrush_Create
			(clone, brush->style, brush->foreground, brush->background);
}

/* Finalize this hatch brush. */
static void
CHatchBrush_Finalize(CBrush *_this)
{
	/* assertions */
	CASSERT((_this != 0));

	/* nothing to do here */
}

/* Create a pattern for this brush. */
static CStatus
CHatchBrush_CreatePattern(CBrush   *_this,
                           CPattern *pattern)
{
	/* assertions */
	CASSERT((_this   != 0));
	CASSERT((pattern != 0));

	/* create the pattern */
	{
		/* declarations */
		pixman_format_t *format;
		CHatchBrush    *brush;
		CByte          *data;
		CColor          fg;
		CColor          bg;
		CUInt32         stride;

		/* get this as a hatch brush */
		brush = (CHatchBrush *)_this;

		/* set the pattern transformation */
		pattern->transform = 0;

		/* calculate the pre-multiplied foreground */
		{
			/* declarations */
			CByte a, r, g, b;

			/* get the foreground */
			fg = brush->foreground;

			/* get the components and pre-multiply */
			a = ((CColor_A(fg)));
			r = ((CColor_R(fg) * a) / 255);
			g = ((CColor_G(fg) * a) / 255);
			b = ((CColor_B(fg) * a) / 255);

			/* set the foreground */
			fg = CPixmanPixel_FromARGB(a, r, g, b);
		}

		/* calculate the pre-multiplied background */
		{
			/* declarations */
			CByte a, r, g, b;

			/* get the background */
			bg = brush->background;

			/* get the components and pre-multiply */
			a = ((CColor_A(bg)));
			r = ((CColor_R(bg) * a) / 255);
			g = ((CColor_G(bg) * a) / 255);
			b = ((CColor_B(bg) * a) / 255);

			/* set the background */
			bg = CPixmanPixel_FromARGB(a, r, g, b);
		}

		/* create the pixman format */
		format = pixman_format_create(CHatchBrush_DataFormat);

		/* ensure we have a format */
		CStatus_Require((format != 0), CStatus_OutOfMemory);

		/* create the pixman image */
		pattern->image =
			pixman_image_create
				(format, CHatchBrush_StyleWidth, CHatchBrush_StyleHeight);

		/* destroy the pixman format */
		pixman_format_destroy(format);

		/* ensure we have an image */
		CStatus_Require((pattern->image != 0), CStatus_OutOfMemory);

		/* get the data and stride */
		data = (CByte *)pixman_image_get_data(pattern->image);
		stride = (CUInt32)pixman_image_get_stride(pattern->image);

		/* get the image data for the style */
		CHatchBrush_StyleToData(brush->style, data, stride, fg, bg);

		/* set the repeat flag */
		pixman_image_set_repeat(pattern->image, 1);
	}

	/* return successfully */
	return CStatus_OK;
}

/* Create a hatch brush. */
CStatus
CHatchBrush_Create(CHatchBrush **_this,
                    CHatchStyle   style,
                    CColor        foreground,
                    CColor        background)
{
	/* ensure we have a this pointer pointer */
	CStatus_Require((_this != 0), CStatus_ArgumentNull);

	/* ensure we have a valid hatch style */
	CStatus_Require((CHatchStyle_IsValid(style)), CStatus_Argument);

	/* allocate the brush */
	if(!(*_this = (CHatchBrush *)CMalloc(sizeof(CHatchBrush))))
	{
		return CStatus_OutOfMemory;
	}

	/* initialize the members */
	(*_this)->style      = style;
	(*_this)->foreground = foreground;
	(*_this)->background = background;

	/* initialize the base */
	CBrush_Initialize((CBrush *)(*_this), &CHatchBrush_Class);

	/* return successfully */
	return CStatus_OK;
}

/* Get the background color of the hatch. */
CStatus
CHatchBrush_GetBackgroundColor(CHatchBrush *_this,
                                CColor      *background)
{
	/* ensure we have a this pointer */
	CStatus_Require((_this != 0), CStatus_ArgumentNull);

	/* ensure we have a background color pointer */
	CStatus_Require((background != 0), CStatus_ArgumentNull);

	/* get the background color */
	*background = _this->background;

	/* return successfully */
	return CStatus_OK;
}

/* Get the foreground color of the hatch. */
CStatus
CHatchBrush_GetForegroundColor(CHatchBrush *_this,
                                CColor      *foreground)
{
	/* ensure we have a this pointer */
	CStatus_Require((_this != 0), CStatus_ArgumentNull);

	/* ensure we have a foreground color pointer */
	CStatus_Require((foreground != 0), CStatus_ArgumentNull);

	/* get the foreground color */
	*foreground = _this->foreground;

	/* return successfully */
	return CStatus_OK;
}

/* Get the style of the hatch. */
CStatus
CHatchBrush_GetHatchStyle(CHatchBrush *_this,
                           CHatchStyle *style)
{
	/* ensure we have a this pointer */
	CStatus_Require((_this != 0), CStatus_ArgumentNull);

	/* ensure we have a style pointer */
	CStatus_Require((style != 0), CStatus_ArgumentNull);

	/* get the style */
	*style = _this->style;

	/* return successfully */
	return CStatus_OK;
}


#ifdef __cplusplus
};
#endif
