/*
 * SDBrush.h - Brush header.
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

#ifndef _SD_BRUSH_H_
#define _SD_BRUSH_H_

#include "CrayonsInternal.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _tagSDBrushClass SDBrushClass;

struct _tagSDBrush
{
	const SDBrushClass *_class;
	SDPattern           pattern;
};

struct _tagSDBrushClass
{
	/*\
	|*| The type of the brush.
	\*/
	SDBrushType type;

	/*\
	|*| Clone this brush.
	|*|
	|*|   _this - this brush
	|*|   clone - pointer to clone
	|*|
	|*|  Returns status code.
	\*/
	SDStatus (*Clone)(SDBrush  *_this,
	                  SDBrush **clone);

	/*\
	|*| Finalize this brush.
	|*|
	|*|   _this - this brush
	\*/
	void (*Finalize)(SDBrush *_this);

	/*\
	|*| Create a pattern for this brush.
	|*|
	|*|   _this   - this brush
	|*|   pattern - pointer to pattern
	|*|
	|*|  Returns status code.
	\*/
	SDStatus (*CreatePattern)(SDBrush   *_this,
	                          SDPattern *pattern);

	/*\
	|*| Sentinel string to catch missing methods in class tables.
	\*/
	const char *sentinel;
};


SDINTERNAL void
SDBrush_Initialize(SDBrush            *_this,
                   const SDBrushClass *_class);
SDINTERNAL void
SDBrush_OnChange(SDBrush *_this);
SDINTERNAL SDStatus
SDBrush_GetPattern(SDBrush   *_this,
                   SDPattern *pattern);

#ifdef __cplusplus
};
#endif

#endif /* _SD_BRUSH_H_ */
