/*
 * SDFiller.h - Filler header.
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

#ifndef _SD_FILLER_H_
#define _SD_FILLER_H_

#include "CPathInterpreter.h"
#include "CPolygon.h"

#ifdef __cplusplus
extern "C" {
#endif

struct _tagSDFiller
{
	SDPathInterpreter  _base;
	SDPointArrayX      array;
	SDPolygonX         polygon;
	SDTrapezoids      *trapezoids;
	SDFillMode         fillMode;
};

SDINTERNAL void
SDFiller_Initialize(SDFiller *_this);
SDINTERNAL void
SDFiller_Finalize(SDFiller *_this);
SDINTERNAL void
SDFiller_Reset(SDFiller *_this);
SDINTERNAL SDStatus
SDFiller_ToPolygon(SDFiller       *_this,
                   SDPolygonX     *polygon,
                   const SDPointF *points,
                   const SDByte   *types,
                   SDUInt32        count);
SDINTERNAL SDStatus
SDFiller_ToTrapezoids(SDFiller       *_this,
                      SDTrapezoids   *trapezoids,
                      const SDPointF *points,
                      const SDByte   *types,
                      SDUInt32        count,
                      SDFillMode      fillMode);

#ifdef __cplusplus
};
#endif

#endif /* _SD_FILLER_H_ */
