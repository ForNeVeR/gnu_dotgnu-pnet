/*
 * SDPolygon.c - Polygon implementation.
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

#include "CPolygon.h"

#ifdef __cplusplus
extern "C" {
#endif

SDINTERNAL void
SDPolygonX_Initialize(SDPolygonX *_this)
{
	/* assertions */
	SDASSERT((_this != 0));

	/* initialize the path */
	_this->capacity        = 0;
	_this->count           = 0;
	_this->edges           = 0;
	_this->hasCurrentPoint = 0;
}

SDINTERNAL void
SDPolygonX_Finalize(SDPolygonX *_this)
{
	/* assertions */
	SDASSERT((_this  != 0));

	/* finalize the polygon */
	{
		/* get the edge list */
		SDEdgeX *edges = _this->edges;

		/* finalize, as needed */
		if(edges != 0)
		{
			/* reset the members */
			_this->capacity  = 0;
			_this->count     = 0;
			_this->edges     = 0;

			/* free the edge list */
			SDFree(edges);
		}

		/* reset the current point flag */
		_this->hasCurrentPoint = 0;
	}
}

SDINTERNAL void
SDPolygonX_Reset(SDPolygonX *_this)
{
	/* assertions */
	SDASSERT((_this  != 0));

	/* reset the members */
	_this->count           = 0;
	_this->hasCurrentPoint = 0;
}

SDINTERNAL SDPointX
SDPolygonX_GetCurrentPoint(SDPolygonX *_this)
{
	/* assertions */
	SDASSERT((_this  != 0));

	/* return the current point */
	return _this->currentPoint;
}

SDINTERNAL SDBool
SDPolygonX_HasCurrentPoint(SDPolygonX *_this)
{
	/* assertions */
	SDASSERT((_this  != 0));

	/* return the has current point flag */
	return _this->hasCurrentPoint;
}

SDINTERNAL SDStatus
SDPolygonX_AddEdge(SDPolygonX *_this,
                   SDPointX   *point1,
                   SDPointX   *point2)
{
	/* assertions */
	SDASSERT((_this  != 0));
	SDASSERT((point1 != 0));
	SDASSERT((point2 != 0));

	/* add the edge, if it isn't horizontal */
	if(SDPoint_Y(*point1) != SDPoint_Y(*point2))
	{
		/* get the edge count */
		const SDUInt32 count = _this->count;

		/* get the capacity */
		const SDUInt32 capacity = _this->capacity;

		/* ensure the capacity of edge list */
		if(count >= capacity)
		{
			/* declarations */
			SDEdgeX  *tmp;
			SDUInt32  newSize;
			SDUInt32  newCapacity;

			/* calculate the new capacity */
			newCapacity = ((capacity + 32) & ~31);

			/* calculate the optimal capacity, as needed */
			if(capacity != 0)
			{
				/* calculate a new capacity candidate */
				const SDUInt32 newCapacity2 = (capacity << 1);

				/* use the larger candidate capacity */
				if(newCapacity < newCapacity2)
				{
					newCapacity = newCapacity2;
				}
			}

			/* calculate the new points size */
			newSize = (newCapacity  * sizeof(SDEdgeX));

			/* create the new edge list */
			if(!(tmp = (SDEdgeX *)SDRealloc(_this->edges, newSize)))
			{
				return SDStatus_OutOfMemory;
			}

			/* update the capacity */
			_this->capacity = newCapacity;

			/* set the edge list */
			_this->edges = tmp;
		}

		/* add the new edge */
		{
			/* get the current edge */
			SDEdgeX *edge = (_this->edges + count);

			/* set the edge properties based on the point order */
			if(SDPoint_Y(*point1) < SDPoint_Y(*point2))
			{
				SDEdge_Point1(*edge)    = *point1;
				SDEdge_Point2(*edge)    = *point2;
				SDEdge_Clockwise(*edge) =  1;
			}
			else
			{
				SDEdge_Point1(*edge)    = *point2;
				SDEdge_Point2(*edge)    = *point1;
				SDEdge_Clockwise(*edge) =  0;
			}
		}

		/* update the edge count */
		_this->count = (count + 1);
	}

	/* update the current point */
	SDPolygonX_MoveTo(_this, point2);

	/* return successfully */
	return SDStatus_OK;
}

SDINTERNAL SDStatus
SDPolygonX_MoveTo(SDPolygonX *_this,
                  SDPointX   *point)
{
	/* assertions */
	SDASSERT((_this != 0));
	SDASSERT((point != 0));

	/* set the first point, as needed */
	if(!(_this->hasCurrentPoint))
	{
		_this->firstPoint = *point;
	}

	/* set the current point */
	_this->currentPoint    = *point;
	_this->hasCurrentPoint = 1;

	/* return successfully */
	return SDStatus_OK;
}

SDINTERNAL SDStatus
SDPolygonX_LineTo(SDPolygonX *_this,
                  SDPointX   *point)
{
	/* assertions */
	SDASSERT((_this != 0));
	SDASSERT((point != 0));

	/* add an edge, if possible */
	if(_this->hasCurrentPoint)
	{
		return SDPolygonX_AddEdge(_this, &(_this->currentPoint), point);
	}

	/* move to the point */
	SDPolygonX_MoveTo(_this, point);

	/* return successfully */
	return SDStatus_OK;
}

SDINTERNAL SDStatus
SDPolygonX_Close(SDPolygonX *_this)
{
	/* assertions */
	SDASSERT((_this != 0));

	/* close the polygon, as needed */
	if(_this->hasCurrentPoint)
	{
		/* add an edge from the last point to the first */
		SDStatus_Check
			(SDPolygonX_AddEdge
				(_this, &(_this->currentPoint), &(_this->firstPoint)));

		/* reset current point flag */
		_this->hasCurrentPoint = 0;
	}

	/* return successfully */
	return SDStatus_OK;
}


#ifdef __cplusplus
};
#endif
