/*
 * SDPolygon.h - Polygon header.
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

#ifndef _SD_POLYGON_H_
#define _SD_POLYGON_H_

#include "CrayonsInternal.h"

#ifdef __cplusplus
extern "C" {
#endif

struct _tagSDPolygonX
{
	SDUInt32  capacity;
	SDUInt32  count;
	SDEdgeX  *edges;
	SDPointX  firstPoint;
	SDPointX  currentPoint;
	SDBool    hasCurrentPoint;
};

SDINTERNAL void
SDPolygonX_Initialize(SDPolygonX *_this);
SDINTERNAL void
SDPolygonX_Finalize(SDPolygonX *_this);
SDINTERNAL void
SDPolygonX_Reset(SDPolygonX *_this);
SDINTERNAL SDPointX
SDPolygonX_GetCurrentPoint(SDPolygonX *_this);
SDINTERNAL SDBool
SDPolygonX_HasCurrentPoint(SDPolygonX *_this);
SDINTERNAL SDStatus
SDPolygonX_AddEdge(SDPolygonX *_this,
                   SDPointX   *point1,
                   SDPointX   *point2);
SDINTERNAL SDStatus
SDPolygonX_MoveTo(SDPolygonX *_this,
                  SDPointX   *point);
SDINTERNAL SDStatus
SDPolygonX_LineTo(SDPolygonX *_this,
                  SDPointX   *point);
SDINTERNAL SDStatus
SDPolygonX_Close(SDPolygonX *_this);

#define SDPolygon_Edges(polygon)     ((polygon).edges)
#define SDPolygon_EdgeCount(polygon) ((polygon).count)

#ifdef __cplusplus
};
#endif

#endif /* _SD_POLYGON_H_ */
