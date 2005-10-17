/*
 * SDSolidBrush.c - Solid brush implementation.
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

#include "CSolidBrush.h"
#include "CUtils.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Initialize this solid brush. */
static void
SDSolidBrush_Initialize(SDSolidBrush *_this,
                        SDColor       color)
{
	/* assertions */
	SDASSERT((_this != 0));

	/* intialize the members */
	_this->color = color;

	/* initialize the base */
	SDBrush_Initialize((SDBrush *)_this, &SDSolidBrush_Class);
}

/* Finalize this solid brush. */
static void
SDSolidBrush_Finalize(SDBrush *_this)
{
	/* assertions */
	SDASSERT((_this != 0));

	/* nothing to do here */
}

/* Clone this solid brush. */
static SDStatus
SDSolidBrush_Clone(SDBrush  *_this,
                   SDBrush **_clone)
{
	/* declarations */
	SDSolidBrush  *brush;
	SDSolidBrush **clone;

	/* assertions */
	SDASSERT((_this != 0));
	SDASSERT((clone != 0));

	/* get this as a solid brush */
	brush = (SDSolidBrush  *)_this;

	/* get the clone as a solid brush */
	clone = (SDSolidBrush **)_clone;

	/* clone this brush */
	return SDSolidBrush_Create(clone, brush->color);
}

/* Create a pattern for this brush. */
static SDStatus
SDSolidBrush_CreatePattern(SDBrush   *_this,
                           SDPattern *pattern)
{
	/* declarations */
	SDSolidBrush *brush;

	/* assertions */
	SDASSERT((_this   != 0));
	SDASSERT((pattern != 0));

	/* get this as a solid brush */
	brush = (SDSolidBrush *)_this;

	/* set the pattern transformation */
	pattern->transform = 0;

	/* create the pattern */
	return SDUtils_CreateSolidPattern(&(pattern->image), brush->color);
}

/* Create a solid brush. */
SDStatus
SDSolidBrush_Create(SDSolidBrush **_this,
                    SDColor        color)
{
	/* ensure we have a this pointer pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* allocate the brush */
	if(!(*_this = (SDSolidBrush *)SDMalloc(sizeof(SDSolidBrush))))
	{
		return SDStatus_OutOfMemory;
	}

	/* initialize the brush */
	SDSolidBrush_Initialize(*_this, color);

	/* return successfully */
	return SDStatus_OK;
}

/* Get the color of this brush. */
SDStatus
SDSolidBrush_GetColor(SDSolidBrush *_this,
                      SDColor      *color)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a color pointer */
	SDStatus_Require((color != 0), SDStatus_ArgumentNull);

	/* get the color */
	*color = _this->color;

	/* return successfully */
	return SDStatus_OK;
}

/* Set the color of this brush. */
SDStatus
SDSolidBrush_SetColor(SDSolidBrush *_this,
                      SDColor       color)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* set the color */
	_this->color = color;

	/* send change signal to base */
	SDBrush_OnChange((SDBrush *)_this);

	/* return successfully */
	return SDStatus_OK;
}


#ifdef __cplusplus
};
#endif
