/*
 * SDLineBrush.h - Linear gradient brush header.
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

#ifndef _SD_LINEBRUSH_H_
#define _SD_LINEBRUSH_H_

#include "CBlend.h"

#ifdef __cplusplus
extern "C" {
#endif

struct _tagSDLineBrush
{
	SDBrush            _base;
	SDAffineTransformF transform;
	SDRectangleF       rectangle;
	SDWrapMode         wrapMode;
	SDBlend            blend;
	SDColorBlend       colorBlend;
	SDColor            startColor;
	SDColor            endColor;
	SDFloat            angle;
	SDBitField         isAngleScalable : 1;
	SDBitField         gammaCorrection : 1;
};

static SDStatus
SDLineBrush_Clone(SDBrush  *_this,
                  SDBrush **_clone);
static void
SDLineBrush_Finalize(SDBrush *_this);
static SDStatus
SDLineBrush_CreatePattern(SDBrush   *_this,
                          SDPattern *pattern);

static const SDBrushClass SDLineBrush_Class =
{
	SDBrushType_LinearGradient,
	SDLineBrush_Clone,
	SDLineBrush_Finalize,
	SDLineBrush_CreatePattern,
	"sentinel"
};

#ifdef __cplusplus
};
#endif

#endif /* _SD_LINEBRUSH_H_ */
