/*
 * SDSolidBrush.h - Solid brush header.
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

#ifndef _SD_SOLIDBRUSH_H_
#define _SD_SOLIDBRUSH_H_

#include "CBrush.h"

#ifdef __cplusplus
extern "C" {
#endif

struct _tagSDSolidBrush
{
	SDBrush _base;
	SDColor color;
};

static SDStatus
SDSolidBrush_Clone(SDBrush  *_this,
                   SDBrush **_clone);
static void
SDSolidBrush_Finalize(SDBrush *_this);
static SDStatus
SDSolidBrush_CreatePattern(SDBrush   *_this,
                           SDPattern *pattern);

static const SDBrushClass SDSolidBrush_Class =
{
	SDBrushType_SolidFill,
	SDSolidBrush_Clone,
	SDSolidBrush_Finalize,
	SDSolidBrush_CreatePattern,
	"sentinel"
};

#ifdef __cplusplus
};
#endif

#endif /* _SD_SOLIDBRUSH_H_ */
