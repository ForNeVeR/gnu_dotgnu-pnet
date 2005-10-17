/*
 * SDFiller.c - Filler implementation.
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

#include "CFiller.h"
#include "CBezier.h"
#include "CTrapezoids.h"

#ifdef __cplusplus
extern "C" {
#endif

static SDStatus
SDFiller_Move(SDPathInterpreter *_this,
              SDFloat            x,
              SDFloat            y,
              SDPathType         type)
{
	/* declarations */
	SDPolygonX *polygon;
	SDPointX    point;

	/* assertions */
	SDASSERT((_this != 0));

	/* get the polygon */
	polygon = &(((SDFiller *)_this)->polygon);

	/* close the polygon */
	SDStatus_Check
		(SDPolygonX_Close
			(polygon));

	/* get the point */
	SDPoint_X(point) = SDFloat_ToFixed(x);
	SDPoint_Y(point) = SDFloat_ToFixed(y);

	/* move to the point */
	SDStatus_Check
		(SDPolygonX_MoveTo
			(polygon, &point));

	/* return successfully */
	return SDStatus_OK;
}

static SDStatus
SDFiller_Line(SDPathInterpreter *_this,
              SDFloat            x,
              SDFloat            y,
              SDPathType         type)
{
	/* declarations */
	SDPolygonX *polygon;
	SDPointX    point;

	/* assertions */
	SDASSERT((_this != 0));

	/* get the polygon */
	polygon = &(((SDFiller *)_this)->polygon);

	/* get the point */
	SDPoint_X(point) = SDFloat_ToFixed(x);
	SDPoint_Y(point) = SDFloat_ToFixed(y);

	/* line to the point */
	SDStatus_Check
		(SDPolygonX_LineTo
			(polygon, &point));

	/* return successfully */
	return SDStatus_OK;
}

static SDStatus
SDFiller_Curve(SDPathInterpreter *_this,
               SDFloat            x1,
               SDFloat            y1,
               SDFloat            x2,
               SDFloat            y2,
               SDFloat            x3,
               SDFloat            y3,
               SDPathType         type)
{
	/* declarations */
	SDPointArrayX *array;
	SDPolygonX    *polygon;
	SDBezierX      bezier;

	/* assertions */
	SDASSERT((_this != 0));

	/* perform setup for curve flattening */
	{
		SDFiller *filler;
		SDPointX  a;
		SDPointX  b;
		SDPointX  c;
		SDPointX  d;

		/* get this as a filler */
		filler = ((SDFiller *)_this);

		/* get the polygon */
		polygon = &(filler->polygon);

		/* get the current point */
		a = SDPolygonX_GetCurrentPoint(polygon);

		/* get the first point */
		SDPoint_X(b) = SDFloat_ToFixed(x1);
		SDPoint_Y(b) = SDFloat_ToFixed(y1);

		/* get the second point */
		SDPoint_X(c) = SDFloat_ToFixed(x2);
		SDPoint_Y(c) = SDFloat_ToFixed(y2);

		/* get the third point */
		SDPoint_X(d) = SDFloat_ToFixed(x3);
		SDPoint_Y(d) = SDFloat_ToFixed(y3);

		/* initialize bezier and bail out now if curve is degenerate */
		if(SDBezierX_Initialize(&bezier, &a, &b, &c, &d))
		{
			return SDStatus_OK;
		}

		/* get the point array */
		array = &(filler->array);

		/* reset the count of the point array */
		SDPointArray_Count(*array) = 0;
	}

	/* flatten the bezier curve */
	SDStatus_Check
		(SDBezierX_Flatten
			(&bezier, array, SDFiller_TOLERANCE));

	/* add the lines to the polygon */
	{
		/* declarations */
		SDPointX *curr;
		SDPointX *end;

		/* get the point pointer */
		curr = SDPointArray_Points(*array);

		/* get the end pointer */
		end = (curr + SDPointArray_Count(*array));

		/* skip the current point */
		++curr;

		/* add the lines */
		while(curr != end)
		{
			/* add the current point */
			SDStatus_Check
				(SDPolygonX_LineTo
					(polygon, curr));

			/* move to the next point */
			++curr;
		}
	}

	/* return successfully */
	return SDStatus_OK;
}

static SDStatus
SDFiller_Close(SDPathInterpreter *_this)
{
	/* declarations */
	SDFiller   *filler;
	SDPolygonX *polygon;

	/* assertions */
	SDASSERT((_this != 0));

	/* get this as a filler */
	filler = ((SDFiller *)_this);

	/* get the polygon */
	polygon = &(filler->polygon);

	/* close the polygon */
	SDStatus_Check(SDPolygonX_Close(polygon));

	/* tessellate the polygon, as needed */
	if(filler->trapezoids != 0)
	{
		/* tessellate the polygon */
		SDStatus_Check
			(SDTrapezoids_TessellatePolygon
				(filler->trapezoids, polygon, filler->fillMode));

		/* reset the polygon */
		SDPolygonX_Reset(polygon);
	}

	/* return successfully */
	return SDStatus_OK;
}

SDINTERNAL SDStatus
SDFiller_ToPolygon(SDFiller       *_this,
                   SDPolygonX     *polygon,
                   const SDPointF *points,
                   const SDByte   *types,
                   SDUInt32        count)
{
	/* declarations */
	SDPathInterpreter *interpreter;
	SDPolygonX         original;
	SDStatus           status;

	/* assertions */
	SDASSERT((_this    != 0));
	SDASSERT((points  != 0));
	SDASSERT((types   != 0));
	SDASSERT((polygon != 0));

	/* get this as a path interpreter */
	interpreter = ((SDPathInterpreter *)_this);

	/* swap the polygons */
	original       = _this->polygon;
	_this->polygon = *polygon;

	/* interpret the path */
	status = SDPathInterpreter_Interpret(interpreter, points, types, count);

	/* handle path interpretation failures */
	if(status != SDStatus_OK)
	{
		/* swap the polygons */
		*polygon       = _this->polygon;
		_this->polygon = original;

		/* return status */
		return status;
	}

	/* ensure the polygon is closed */
	status = SDPolygonX_Close(&(_this->polygon));

	/* swap the polygons */
	*polygon       = _this->polygon;
	_this->polygon = original;

	/* return status */
	return status;
}

SDINTERNAL SDStatus
SDFiller_ToTrapezoids(SDFiller       *_this,
                      SDTrapezoids   *trapezoids,
                      const SDPointF *points,
                      const SDByte   *types,
                      SDUInt32        count,
                      SDFillMode      fillMode)
{
	/* declarations */
	SDPathInterpreter *interpreter;
	SDStatus           status;

	/* assertions */
	SDASSERT((_this       != 0));
	SDASSERT((points     != 0));
	SDASSERT((types      != 0));
	SDASSERT((trapezoids != 0));

	/* get this as a path interpreter */
	interpreter = ((SDPathInterpreter *)_this);

	/* set the trapezoids */
	_this->trapezoids = trapezoids;

	/* set the fill mode */
	_this->fillMode = fillMode;

	/* interpret the path */
	status = SDPathInterpreter_Interpret(interpreter, points, types, count);

	/* reset the trapezoids */
	_this->trapezoids = 0;

	/* handle path interpretation failures */
	SDStatus_Check(status);

	/* finish trapezoidation, as needed */
	if(SDPolygonX_HasCurrentPoint(&(_this->polygon)))
	{
		/* ensure the polygon is closed */
		SDStatus_Check
			(SDPolygonX_Close
				(&(_this->polygon)));

		/* tessellate the polygon */
		return
			SDTrapezoids_TessellatePolygon
				(trapezoids, &(_this->polygon), fillMode);
	}

	/* return successfully */
	return SDStatus_OK;
}

static const SDPathInterpreterClass SDFiller_Class =
{
	SDFiller_Move,
	SDFiller_Line,
	SDFiller_Curve,
	SDFiller_Close,
	"sentinel"
};

SDINTERNAL void
SDFiller_Initialize(SDFiller *_this)
{
	/* assertions */
	SDASSERT((_this != 0));

	/* initialize the base */
	_this->_base._class = &SDFiller_Class;

	/* initialize the point array */
	SDPointArrayX_Initialize(&(_this->array));

	/* initialize the polygon */
	SDPolygonX_Initialize(&(_this->polygon));

	/* set the trapezoids to the default */
	_this->trapezoids = 0;
}

SDINTERNAL void
SDFiller_Finalize(SDFiller *_this)
{
	/* assertions */
	SDASSERT((_this != 0));

	/* reset the trapezoids */
	_this->trapezoids = 0;

	/* finalize the point array */
	SDPointArrayX_Finalize(&(_this->array));

	/* finalize the polygon */
	SDPolygonX_Finalize(&(_this->polygon));

	/* finalize the base */
	_this->_base._class = 0;
}

SDINTERNAL void
SDFiller_Reset(SDFiller *_this)
{
	/* assertions */
	SDASSERT((_this != 0));

	/* reset the trapezoids */
	_this->trapezoids = 0;

	/* reset the polygon */
	SDPolygonX_Reset(&(_this->polygon));

	/* reset the point array */
	SDPointArray_Count(_this->array) = 0;
}

#ifdef __cplusplus
};
#endif
