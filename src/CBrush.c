/*
 * SDBrush.c - Brush implementation.
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

#include "CBrush.h"

#ifdef __cplusplus
extern "C" {
#endif

/* TODO: brush pattern caching should be made thread-safe */

/* Initialize this brush. */
SDINTERNAL void
SDBrush_Initialize(SDBrush            *_this,
                   const SDBrushClass *_class)
{
	/* assertions */
	SDASSERT((_this  != 0));
	SDASSERT((_class != 0));

	/* initialize the members */
	_this->_class        = _class;
	_this->pattern.image = 0;
}

/* Get the type of this brush. */
SDStatus
SDBrush_GetBrushType(SDBrush     *_this,
                     SDBrushType *type)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a type pointer */
	SDStatus_Require((type != 0), SDStatus_ArgumentNull);

	/* get the type */
	*type = _this->_class->type;

	/* return successfully */
	return SDStatus_OK;
}

/* Clone this brush. */
SDStatus
SDBrush_Clone(SDBrush  *_this,
              SDBrush **clone)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a clone pointer */
	SDStatus_Require((clone != 0), SDStatus_ArgumentNull);

	/* create the clone */
	return _this->_class->Clone(_this, clone);
}

/* Destroy this brush. */
SDStatus
SDBrush_Destroy(SDBrush **_this)
{
	/* ensure we have a this pointer pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a this pointer */
	SDStatus_Require(((*_this) != 0), SDStatus_ArgumentNull);

	/* finalize the brush */
	(*_this)->_class->Finalize(*_this);

	/* destroy the pattern, as needed */
	if((*_this)->pattern.image != 0)
	{
		pixman_image_destroy((*_this)->pattern.image);
		(*_this)->pattern.image = 0;
	}

	/* null the this pointer */
	*_this = 0;

	/* return successfully */
	return SDStatus_OK;
}

/* Handle a change signal. */
SDINTERNAL void
SDBrush_OnChange(SDBrush *_this)
{
	/* assertions */
	SDASSERT((_this != 0));

	/* destroy the current pattern, as needed */
	if(_this->pattern.image != 0)
	{
		pixman_image_destroy(_this->pattern.image);
		_this->pattern.image = 0;
	}
}

/* Get a pattern for this brush. */
SDINTERNAL SDStatus
SDBrush_GetPattern(SDBrush   *_this,
                   SDPattern *pattern)
{
	/* assertions */
	SDASSERT((_this   != 0));
	SDASSERT((pattern != 0));

	/* create a pattern, as needed */
	if(_this->pattern.image == 0)
	{
		SDStatus_Check
			(_this->_class->CreatePattern
				(_this, &(_this->pattern)));
	}

	/* get the pattern */
	*pattern = _this->pattern;

	/* return successfully */
	return SDStatus_OK;
}


#ifdef __cplusplus
};
#endif
