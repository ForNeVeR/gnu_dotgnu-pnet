/*
 * SDTrapezoids.h - Trapezoids header.
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

#ifndef _SD_TRAPEZOIDS_H_
#define _SD_TRAPEZOIDS_H_

#include "CrayonsInternal.h"

#ifdef __cplusplus
extern "C" {
#endif

SDINTERNAL void
SDTrapezoids_Initialize(SDTrapezoids *_this);
SDINTERNAL void
SDTrapezoids_Finalize(SDTrapezoids *_this);
SDINTERNAL void
SDTrapezoids_Reset(SDTrapezoids *_this);
SDINTERNAL SDStatus
SDTrapezoids_TessellatePolygon(SDTrapezoids *_this,
                               SDPolygonX   *polygon,
                               SDFillMode    fillMode);
SDINTERNAL SDStatus
SDTrapezoids_Fill(SDTrapezoids *_this,
                  SDPointF     *points,
                  SDByte       *types,
                  SDUInt32      count,
                  SDFillMode    fillMode);

#ifdef __cplusplus
};
#endif

#endif /* _SD_TRAPEZOIDS_H_ */
