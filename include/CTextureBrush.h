/*
 * SDTextureBrush.h - Texture brush header.
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

#ifndef _SD_TEXTUREBRUSH_H_
#define _SD_TEXTUREBRUSH_H_

#include "CBrush.h"

#ifdef __cplusplus
extern "C" {
#endif

struct _tagSDTextureBrush
{
	SDBrush             _base;
	SDAffineTransformF  transform;
	SDRectangleF        rectangle;
	SDWrapMode          wrapMode;
	SDImage            *image;
};

static SDStatus
SDTextureBrush_Clone(SDBrush  *_this,
                     SDBrush **_clone);
static void
SDTextureBrush_Finalize(SDBrush *_this);
static SDStatus
SDTextureBrush_CreatePattern(SDBrush   *_this,
                             SDPattern *pattern);

static const SDBrushClass SDTextureBrush_Class =
{
	SDBrushType_TextureFill,
	SDTextureBrush_Clone,
	SDTextureBrush_Finalize,
	SDTextureBrush_CreatePattern,
	"sentinel"
};

#ifdef __cplusplus
};
#endif

#endif /* _SD_TEXTUREBRUSH_H_ */
