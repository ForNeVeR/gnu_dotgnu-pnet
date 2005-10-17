/*
 * SDRegionRasterizer.h - Region rasterizer header.
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

#ifndef _SD_REGIONRASTERIZER_H_
#define _SD_REGIONRASTERIZER_H_

#include "CRegionInterpreter.h"
#include "CFiller.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _tagSDRegionRasterizer SDRegionRasterizer;
struct _tagSDRegionRasterizer
{
	SDRegionInterpreter  _base;
	pixman_format_t     *format;
	SDAffineTransformF  *transform;
	SDTrapezoids         trapezoids;
	SDFiller             filler;
	SDPointArrayF        array;
	SDUInt32             width;
	SDUInt32             height;
	SDUInt32             size;
};

SDINTERNAL SDStatus
SDRegionRasterizer_Initialize(SDRegionRasterizer *_this,
                              SDAffineTransformF *transform,
                              SDFloat             width,
                              SDFloat             height,
                              SDBool              gray);
SDINTERNAL void
SDRegionRasterizer_Finalize(SDRegionRasterizer *_this);

#ifdef __cplusplus
};
#endif

#endif /* _SD_REGIONRASTERIZER_H_ */
