/*
 * SDPathBrush.h - Path gradient brush header.
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

#ifndef _SD_PATHBRUSH_H_
#define _SD_PATHBRUSH_H_

#include "CBlend.h"

#ifdef __cplusplus
extern "C" {
#endif

struct _tagSDPathBrush
{
	SDBrush             _base;
	SDAffineTransformF  transform;
	SDRectangleF        rectangle;
	SDWrapMode          wrapMode;
	SDBlend             blend;
	SDColorBlend        colorBlend;
	SDPointF            focusPoint;
	SDPointF            centerPoint;
	SDColor             centerColor;
	SDPath             *path;
	SDColor            *surroundColors;
	SDUInt32            surroundCount;
};

static SDStatus
SDPathBrush_Clone(SDBrush  *_this,
                  SDBrush **_clone);
static void
SDPathBrush_Finalize(SDBrush *_this);
static SDStatus
SDPathBrush_CreatePattern(SDBrush   *_this,
                          SDPattern *pattern);

static const SDBrushClass SDPathBrush_Class =
{
	SDBrushType_PathGradient,
	SDPathBrush_Clone,
	SDPathBrush_Finalize,
	SDPathBrush_CreatePattern,
	"sentinel"
};

#ifdef __cplusplus
};
#endif

#endif /* _SD_PATHBRUSH_H_ */
