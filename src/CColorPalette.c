/*
 * SDColorPalette.c - Color palette implementation.
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

#include "CColorPalette.h"

#ifdef __cplusplus
extern "C" {
#endif

SDStatus
SDColorPalette_Create(SDColorPalette **_this,
                      SDColor         *colors,
                      SDUInt32         count,
                      SDPaletteFlag    flags)
{
	/* ensure we have a this pointer pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a color table */
	SDStatus_Require((colors != 0), SDStatus_ArgumentNull);

	/* ensure we have a valid count */
	SDStatus_Require
		((count == 256 || count == 128 || count == 2), SDStatus_Argument);

	/* allocate the palette */
	if(!(*_this = (SDColorPalette *)SDMalloc(sizeof(SDColorPalette))))
	{
		return SDStatus_OutOfMemory;
	}

	/* allocate the color table */
	if(!((*_this)->colors = (SDColor *)SDMalloc(count * sizeof(SDColor))))
	{
		SDFree(*_this);
		*_this = 0;
		return SDStatus_OutOfMemory;
	}

	/* copy the color entries */
	SDMemCopy((*_this)->colors, colors, (count * sizeof(SDColor)));

	/* set the count */
	(*_this)->count = count;

	/* set the flags */
	(*_this)->flags = flags;

	/* return successfully */
	return SDStatus_OK;
}

SDStatus
SDColorPalette_Destroy(SDColorPalette **_this)
{
	/* ensure we have a this pointer pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* finalize the members */
	if((*_this)->colors != 0)
	{
		SDFree((*_this)->colors);
	}

	/* dispose of this color palette */
	SDFree(*_this);

	/* null the this pointer */
	*_this = 0;

	/* return successfully */
	return SDStatus_OK;
}

SDINTERNAL SDBool
SDColorPalette_CheckFormat(SDColorPalette *_this,
                           SDPixelFormat   format)
{
	/* assertions */
	SDASSERT((_this != 0));

	/* determine and return if the format is ok */
	switch(format)
	{
		default: return 1;
		case SDPixelFormat_1bppIndexed: { return (_this->count >=   2); }
		case SDPixelFormat_4bppIndexed: { return (_this->count >=  16); }
		case SDPixelFormat_8bppIndexed: { return (_this->count >= 256); }
	}
}

SDINTERNAL SDColor
SDColorPalette_GetColor(SDColorPalette *_this,
                        SDUInt32        index)
{
	/* assertions */
	SDASSERT((_this != 0));
	SDASSERT((index < _this->count));

	/* return the color table entry */
	return _this->colors[index];
}

SDINTERNAL SDUInt32
SDColorPalette_FindBestMatch(SDColorPalette *_this,
                             SDColor         color)
{
	/* declarations */
	SDByte    a;
	SDByte    r;
	SDByte    g;
	SDByte    b;
	SDUInt32  index;
	SDUInt32  best;
	SDUInt32  distance;
	SDColor  *curr;
	SDColor  *end;

	/* assertions */
	SDASSERT((_this != 0));

	/* get the color components */
	a = SDColor_A(color);
	r = SDColor_R(color);
	g = SDColor_G(color);
	b = SDColor_B(color);

	/* set the index to the default */
	index = 0;

	/* set the best match to the default */
	best = -1;

	/* set the distance to the default */
	distance = 200000;

	/* get the color entry pointer */
	curr = _this->colors;

	/* get the end pointer */
	end = (curr + _this->count);

	/* find the best match */
	while(curr != end)
	{
		/* declarations */
		SDInt32 distA, distR, distG, distB, dist;

		/* get the color components for the current entry */
		distA = SDColor_A(*curr) - a;
		distR = SDColor_R(*curr) - r;
		distG = SDColor_G(*curr) - g;
		distB = SDColor_B(*curr) - b;

		/* calculate the distance */
		dist = ((distA * distA) +
		        (distR * distR) +
		        (distG * distG) +
		        (distB * distB));

		/* set the best match to the current entry, as needed */
		if(dist < distance)
		{
			/* reset the distance */
			distance = (SDUInt32)dist;

			/* reset the best match */
			best = index;

			/* bail out now if we have a perfect match */
			if(distance == 0) { break; }
		}

		/* move to the next entry position */
		++index;
		++curr;
	}

	/* return the best matching entry */
	return best;
}


#ifdef __cplusplus
};
#endif
