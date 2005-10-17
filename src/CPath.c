/*
 * SDPath.c - Path implementation.
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

#include "CPath.h"
#include "CFiller.h"
#include "CFlattener.h"
#include "CMatrix.h"
#include "CStroker.h"
#include "CTrapezoids.h"

#ifdef __cplusplus
extern "C" {
#endif

#define _TYPE_LINE_CLOSE   (SDPathType_Line   | SDPathType_CloseSubpath)
#define _TYPE_BEZIER_CLOSE (SDPathType_Bezier | SDPathType_CloseSubpath)

#define _SetPoint(currP, currT, x, y, type)                                    \
		*(currT) = (type);                                                     \
		SDPoint_X(*(currP)) = (x);                                             \
		SDPoint_Y(*(currP)) = (y)
#define _NextPoint(currP, currT)                                               \
	++currP; ++currT
#define _SetPointAdvance(currP, currT, x, y, type)                             \
	_SetPoint((currP), (currT), (x), (y), (type));                             \
	_NextPoint((currP), (currT))
#define _MoveTo(currP, currT, x, y)                                            \
	do {                                                                       \
		_SetPointAdvance((currP), (currT), (x), (y), SDPathType_Start);        \
	} while(0)
#define _LineTo(currP, currT, x, y)                                            \
	do {                                                                       \
		_SetPointAdvance((currP), (currT), (x), (y), SDPathType_Line);         \
	} while(0)
#define _LineToClose(currP, currT, x, y)                                       \
	do {                                                                       \
		_SetPointAdvance((currP), (currT), (x), (y), _TYPE_LINE_CLOSE);        \
	} while(0)
#define _CurveTo(currP, currT, x1, y1, x2, y2, x3, y3)                         \
	do {                                                                       \
		_SetPointAdvance((currP), (currT), (x1), (y1), SDPathType_Bezier);     \
		_SetPointAdvance((currP), (currT), (x2), (y2), SDPathType_Bezier);     \
		_SetPointAdvance((currP), (currT), (x3), (y3), SDPathType_Bezier);     \
	} while(0)
#define _CurveToClose(currP, currT, x1, y1, x2, y2, x3, y3)                    \
	do {                                                                       \
		_SetPointAdvance((currP), (currT), (x1), (y1), SDPathType_Bezier);     \
		_SetPointAdvance((currP), (currT), (x2), (y2), SDPathType_Bezier);     \
		_SetPointAdvance((currP), (currT), (x3), (y3), _TYPE_BEZIER_CLOSE);    \
	} while(0)
#define _BeginAdd(path, n, plist, tlist, x1, y1)                               \
	do {                                                                       \
		/* get the current count */                                            \
		const SDUInt32 _cc_ = (path)->count;                                   \
		                                                                       \
		/* ensure the capacity of the point and type lists */                  \
		SDStatus_Check(SDPath_EnsureCapacity((path), (_cc_ + (n))));           \
		                                                                       \
		/* update the count */                                                 \
		(path)->count = (_cc_ + (n));                                          \
		                                                                       \
		/* get the point and type list pointers */                             \
		(plist) = ((path)->points + _cc_);                                     \
		(tlist) = ((path)->types  + _cc_);                                     \
		                                                                       \
		/* set the start point */                                              \
		if((path)->newFigure)                                                  \
		{                                                                      \
			/* start the new figure */                                         \
			_MoveTo((plist), (tlist), (x1), (y1));                             \
			                                                                   \
			/* reset the new figure flag */                                    \
			(path)->newFigure = 0;                                             \
		}                                                                      \
		else                                                                   \
		{                                                                      \
			/* set a line to the start point */                                \
			_LineTo((plist), (tlist), (x1), (y1));                             \
		}                                                                      \
	} while(0)
#define _BeginNew(path, n, plist, tlist, x1, y1)                               \
	do {                                                                       \
		/* get the current count */                                            \
		const SDUInt32 _cc_ = (path)->count;                                   \
		                                                                       \
		/* ensure the capacity of the point and type lists */                  \
		SDStatus_Check(SDPath_EnsureCapacity((path), (_cc_ + (n))));           \
		                                                                       \
		/* update the count */                                                 \
		(path)->count = (_cc_ + (n));                                          \
		                                                                       \
		/* get the point and type list pointers */                             \
		(plist) = ((path)->points + _cc_);                                     \
		(tlist) = ((path)->types  + _cc_);                                     \
		                                                                       \
		/* start the new figure */                                             \
		_MoveTo((plist), (tlist), (x1), (y1));                                 \
			                                                                   \
		/* set the new figure flag */                                          \
		(path)->newFigure = 1;                                                 \
	} while(0)

static void
SDPath_Initialize(SDPath *_this)
{
	/* assertions */
	SDASSERT((_this != 0));

	/* initialize the path */
	_this->capacity  = 0;
	_this->count     = 0;
	_this->points    = 0;
	_this->types     = 0;
	_this->winding   = 0;
	_this->newFigure = 1;
	_this->hasCurves = 0;
}

static void
SDPath_Finalize(SDPath *_this)
{
	/* assertions */
	SDASSERT((_this != 0));

	/* finalize the path */
	{
		/* get the point list */
		SDPointF *points = _this->points;

		/* get the type list */
		SDByte *types = _this->types;

		/* finalize, as needed */
		if(points != 0)
		{
			/* reset the members */
			_this->capacity  = 0;
			_this->count     = 0;
			_this->points    = 0;
			_this->types     = 0;
			_this->winding   = 0;
			_this->newFigure = 1;
			_this->hasCurves = 0;

			/* free the point list */
			SDFree(points);

			/* free the type list */
			SDFree(types);
		}
	}
}

/*\
|*| Ensure the capacity of point and type lists of this path.
|*|
|*|   _this - this path
|*|   count - the total minimum capacity required
|*|
|*|  Returns status code.
\*/
static SDStatus
SDPath_EnsureCapacity(SDPath   *_this,
                      SDUInt32  minimum)
{
	/* assertions */
	SDASSERT((_this != 0));

	/* ensure capacity */
	_EnsurePathCapacity(_this, minimum);

	/* return successfully */
	return SDStatus_OK;
}

/* Sanity check the given type list. */
static SDStatus
_SanityCheckTypes(SDByte   *types,
                  SDUInt32  count)
{
	/* TODO: assume all is ok for now */
	return SDStatus_OK;
}


/* Create a path. */
SDStatus
SDPath_Create(SDPath **_this)
{
	/* ensure we have a this pointer pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* allocate the path */
	if(!(*_this = (SDPath *)SDMalloc(sizeof(SDPath))))
	{
		return SDStatus_OutOfMemory;
	}

	/* initialize the path */
	SDPath_Initialize(*_this);

	/* return successfully */
	return SDStatus_OK;
}

/* Destroy a path. */
SDStatus
SDPath_Destroy(SDPath **_this)
{
	/* ensure we have a this pointer pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a this pointer */
	SDStatus_Require(((*_this) != 0), SDStatus_ArgumentNull);

	/* finalize the path */
	SDPath_Finalize(*_this);

	/* free the path */
	SDFree(*_this);

	/* null the this pointer */
	*_this = 0;

	/* return successfully */
	return SDStatus_OK;
}

/* Get the fill mode of this path. */
SDStatus
SDPath_GetFillMode(SDPath     *_this,
                   SDFillMode *fillMode)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a fill mode pointer */
	SDStatus_Require((fillMode != 0), SDStatus_ArgumentNull);

	/* get the fill mode */
	*fillMode = (_this->winding ? SDFillMode_Winding : SDFillMode_Alternate);

	/* return successfully */
	return SDStatus_OK;
}

/* Set the fill mode of this path. */
SDStatus
SDPath_SetFillMode(SDPath     *_this,
                   SDFillMode  fillMode)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* set the fill mode */
	_this->winding = ((fillMode == SDFillMode_Winding) ?  1 : 0);

	/* return successfully */
	return SDStatus_OK;
}

/* Get the points in this path. */
SDStatus
SDPath_GetPoints(SDPath    *_this,
                 SDPointF **points,
                 SDUInt32  *count)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a point list pointer */
	SDStatus_Require((points != 0), SDStatus_ArgumentNull);

	/* ensure we have a count pointer */
	SDStatus_Require((count != 0), SDStatus_ArgumentNull);

	/* get the points */
	{
		/* get the count and size */
		const SDUInt32 cnt  = _this->count;
		const SDUInt32 size = (cnt * sizeof(SDPointF));

		/* allocate the point list */
		if(!(*points = (SDPointF *)SDMalloc(size)))
		{
			*count = 0;
			return SDStatus_OutOfMemory;
		}

		/* get the points */
		SDMemCopy(*points, _this->points, size);

		/* get the count */
		*count = cnt;
	}

	/* return successfully */
	return SDStatus_OK;
}

/* Get the types of the points in this path. */
SDStatus
SDPath_GetTypes(SDPath    *_this,
                SDByte   **types,
                SDUInt32  *count)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a type list pointer */
	SDStatus_Require((types != 0), SDStatus_ArgumentNull);

	/* ensure we have a count pointer */
	SDStatus_Require((count != 0), SDStatus_ArgumentNull);

	/* get the types */
	{
		/* get the count and size */
		const SDUInt32 cnt  = _this->count;
		const SDUInt32 size = (cnt * sizeof(SDByte));

		/* allocate the type list */
		if(!(*types = (SDByte *)SDMalloc(size)))
		{
			*count = 0;
			return SDStatus_OutOfMemory;
		}

		/* get the types */
		SDMemCopy(*types, _this->types, size);

		/* get the count */
		*count = cnt;
	}

	/* return successfully */
	return SDStatus_OK;
}

/* Get the points and types in this path. */
SDStatus
SDPath_GetPathData(SDPath    *_this,
                   SDPointF **points,
                   SDByte   **types,
                   SDUInt32  *count)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a point list pointer */
	SDStatus_Require((points != 0), SDStatus_ArgumentNull);

	/* ensure we have a type list pointer */
	SDStatus_Require((types != 0), SDStatus_ArgumentNull);

	/* ensure we have a count pointer */
	SDStatus_Require((count != 0), SDStatus_ArgumentNull);

	/* get the path data */
	{
		/* get the count and sizes */
		const SDUInt32 cnt   = _this->count;
		const SDUInt32 sizeP = (cnt * sizeof(SDPointF));
		const SDUInt32 sizeT = (cnt * sizeof(SDByte));

		/* allocate the point list */
		if(!(*points = (SDPointF *)SDMalloc(sizeP)))
		{
			*types = 0;
			*count = 0;
			return SDStatus_OutOfMemory;
		}

		/* allocate the type list */
		if(!(*types = (SDByte *)SDMalloc(sizeT)))
		{
			SDFree(*points);
			*points = 0;
			*count = 0;
			return SDStatus_OutOfMemory;
		}

		/* get the points */
		SDMemCopy(*points, _this->points, sizeP);

		/* get the types */
		SDMemCopy(*types, _this->types, sizeT);

		/* get the count */
		*count = cnt;
	}

	/* return successfully */
	return SDStatus_OK;
}

/* Set the points and types in this path. */
SDStatus
SDPath_SetPathData(SDPath   *_this,
                   SDPointF *points,
                   SDByte   *types,
                   SDUInt32  count)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a point list */
	SDStatus_Require((points != 0), SDStatus_ArgumentNull);

	/* ensure we have a type list */
	SDStatus_Require((types != 0), SDStatus_ArgumentNull);

	/* sanity check the types */
	SDStatus_Check(_SanityCheckTypes(types, count));

	/* set the points and types */
	{
		/* ensure the capacity of the point and type lists */
		SDStatus_Check(SDPath_EnsureCapacity(_this, count));

		/* update the count */
		_this->count = count;

		/* copy the points to the point list */
		SDMemCopy(_this->points, points, (count * sizeof(SDPointF)));

		/* copy the types to the type list */
		SDMemCopy(_this->types, types, (count * sizeof(SDByte)));

		/* set the new figure flag */
		_this->newFigure =
			((count == 0) ||
			 ((types[count - 1] & SDPathType_CloseSubpath) != 0));
	}

	/* return successfully */
	return SDStatus_OK;
}

/* Add an arc to the current figure. */
SDStatus
SDPath_AddArc(SDPath  *_this,
              SDFloat  x,
              SDFloat  y,
              SDFloat  width,
              SDFloat  height,
              SDFloat  startAngle,
              SDFloat  sweepAngle)
{
	/* TODO */
	return SDStatus_NotImplemented;
}

/* Add a bezier curve to the current figure. */
SDStatus
SDPath_AddBezier(SDPath  *_this,
                 SDFloat  x1,
                 SDFloat  y1,
                 SDFloat  x2,
                 SDFloat  y2,
                 SDFloat  x3,
                 SDFloat  y3,
                 SDFloat  x4,
                 SDFloat  y4)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* add the bezier curve */
	{
		/* declarations */
		SDPointF *p;
		SDByte   *t;

		/* perform standard startup procedures for path segment additions */
		_BeginAdd(_this, 4, p, t, x1, y1);

		/* complete the curve */
		_CurveTo(p, t, x2, y2, x3, y3, x4, y4);

		/* flag that we have curves */
		_this->hasCurves = 1;
	}

	/* return successfully */
	return SDStatus_OK;
}

/* Add a sequence of connected bezier curves to the current figure. */
SDStatus
SDPath_AddBeziers(SDPath   *_this,
                  SDPointF *points,
                  SDUInt32  count)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a point list pointer */
	SDStatus_Require((points != 0), SDStatus_ArgumentNull);

	/* ensure we have the required minimum number of points */
	SDStatus_Require((count >= 4), SDStatus_Argument_NeedAtLeast4Points);

	/* ensure we have a valid number of additional points */
	SDStatus_Require
		((((count - 4) % 3) == 0), SDStatus_Argument_InvalidPointCount);

	/* add the bezier curves */
	{
		/* declarations */
		SDPointF *p;
		SDByte   *t;

		/* get the end of input pointer */
		const SDPointF *const end = (points + count);

		/* perform standard startup procedures for path segment additions */
		_BeginAdd
			(_this,              count,
			 p,                  t,
			 SDPoint_X(*points), SDPoint_Y(*points++));

		/* add the curve segments */
		while(points != end)
		{
			_CurveTo
				(p,                  t,
				 SDPoint_X(*points), SDPoint_Y(*points++),
				 SDPoint_X(*points), SDPoint_Y(*points++),
				 SDPoint_X(*points), SDPoint_Y(*points++));
		}

		/* flag that we have curves */
		_this->hasCurves = 1;
	}

	/* return successfully */
	return SDStatus_OK;
}

/* Add a closed curve to this path. */
SDStatus
SDPath_AddClosedCardinalCurve(SDPath   *_this,
                              SDPointF *points,
                              SDUInt32  count,
                              SDFloat   tension)
{
	/* TODO */
	return SDStatus_NotImplemented;
}

/* Add a curve to the current figure. */
SDStatus
SDPath_AddCardinalCurve(SDPath   *_this,
                        SDPointF *points,
                        SDUInt32  count,
                        SDUInt32  offset,
                        SDUInt32  numberOfSegments,
                        SDFloat   tension)
{
	/* TODO */
	return SDStatus_NotImplemented;
}

/* Add an ellipse to this path. */
SDStatus
SDPath_AddEllipse(SDPath  *_this,
                  SDFloat  x,
                  SDFloat  y,
                  SDFloat  width,
                  SDFloat  height)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* add the ellipse */
	{
		/* declarations */
		SDPointF *p;
		SDByte   *t;

		/* calculate the radii */
		const SDDouble rX = width / 2;
		const SDDouble rY = height / 2;

		/* calculate the distances along the tangents */
		const SDDouble dX = rX * SDMath_Arc90Fraction;
		const SDDouble dY = rY * SDMath_Arc90Fraction;

		/* calculate the center point */
		const SDFloat cX = x + rX;
		const SDFloat cY = y + rY;

		/* calculate the tangential control points */
		const SDFloat pX = cX + dX;
		const SDFloat pY = cY + dY;
		const SDFloat mX = cX - dX;
		const SDFloat mY = cY - dY;

		/* calculate the edge points */
		const SDFloat right  = x + width;
		const SDFloat bottom = y + height;

		/* perform standard startup procedures for new figure additions */
		_BeginNew(_this, 13, p, t, right, cY);

		/* curve counter-clockwise, starting from the top-right quadrant */
		_CurveTo     (p, t, right, mY,     pX,    y,      cX,    y);
		_CurveTo     (p, t, mX,    y,      x,     mY,     x,     cY);
		_CurveTo     (p, t, x,     pY,     mX,    bottom, cX,    bottom);
		_CurveToClose(p, t, pX,    bottom, right, pY,     right, cY);

		/* flag that we have curves */
		_this->hasCurves = 1;
	}

	/* return successfully */
	return SDStatus_OK;
}

/* Add a line to the current figure. */
SDStatus
SDPath_AddLine(SDPath  *_this,
               SDFloat  x1,
               SDFloat  y1,
               SDFloat  x2,
               SDFloat  y2)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* add the line */
	{
		/* declarations */
		SDPointF *p;
		SDByte   *t;

		/* perform standard startup procedures for path segment additions */
		_BeginAdd(_this, 2, p, t, x1, y1);

		/* complete the line */
		_LineTo(p,  t, x2, y2);
	}

	/* return successfully */
	return SDStatus_OK;
}

/* Add a sequence of connected line segments to the current figure. */
SDStatus
SDPath_AddLines(SDPath   *_this,
                SDPointF *points,
                SDUInt32  count)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a point list pointer */
	SDStatus_Require((points != 0), SDStatus_ArgumentNull);

	/* ensure we have the required minimum number of points */
	SDStatus_Require((count >= 2), SDStatus_Argument_NeedAtLeast2Points);

	/* add the lines */
	{
		/* declarations */
		SDPointF *p;
		SDByte   *t;

		/* get the end of input pointer */
		const SDPointF *const end = (points + count);

		/* perform standard startup procedures for path segment additions */
		_BeginAdd
			(_this,              count,
			 p,                  t,
			 SDPoint_X(*points), SDPoint_Y(*points++));

		/* add the line segments */
		while(points != end)
		{
			_LineTo
				(p,                  t,
				 SDPoint_X(*points), SDPoint_Y(*points++));
		}
	}

	/* return successfully */
	return SDStatus_OK;
}

/* Add the contents of another path to this path. */
SDStatus
SDPath_AddPath(SDPath *_this,
               SDPath *path,
               SDBool  connect)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a path pointer */
	SDStatus_Require((path != 0), SDStatus_ArgumentNull);

	/* bail out now if there's nothing to do */
	SDStatus_Require((path->count != 0), SDStatus_OK);

	/* add the path */
	{
		/* ensure the path capacity */
		SDStatus_Check
			(SDPath_EnsureCapacity
				(_this, (_this->count + path->count)));

		/* copy the points */
		SDMemCopy
			((_this->points + _this->count),
			 path->points, (path->count * sizeof(SDPointF)));

		/* copy the types */
		SDMemCopy
			((_this->types + _this->count),
			 path->types, (path->count * sizeof(SDByte)));

		/* connect the path as needed */
		if(connect && !(_this->newFigure))
		{
			_this->types[_this->count] = SDPathType_Line;
		}

		/* update the flags */
		_this->hasCurves |= path->hasCurves;
		_this->newFigure  = path->newFigure;

		/* update the count */
		_this->count += path->count;
	}

	/* return successfully */
	return SDStatus_OK;
}

/* Add a pie section to this path. */
SDStatus
SDPath_AddPie(SDPath  *_this,
              SDFloat  x,
              SDFloat  y,
              SDFloat  width,
              SDFloat  height,
              SDFloat  startAngle,
              SDFloat  sweepAngle)
{
	/* TODO */
	return SDStatus_NotImplemented;
}

/* Add a polygon to this path. */
SDStatus
SDPath_AddPolygon(SDPath   *_this,
                  SDPointF *points,
                  SDUInt32  count)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a point list pointer */
	SDStatus_Require((points != 0), SDStatus_ArgumentNull);

	/* ensure we have the required minimum number of points */
	SDStatus_Require((count >= 3), SDStatus_Argument_NeedAtLeast3Points);

	/* add the polygon */
	{
		/* declarations */
		SDPointF *p;
		SDByte   *t;

		/* get the last point pointer */
		const SDPointF *last = (points + count - 1);

		/* ignore redundant last points */
		if(SDPoint_X(*points) == SDPoint_X(*last) &&
		   SDPoint_Y(*points) == SDPoint_Y(*last))
		{
			--last;
			--count;
		}

		/* perform standard startup procedures for new figure additions */
		_BeginNew
			(_this,            count,
			 p,                t,
			 SDPoint_X(*points), SDPoint_Y(*points++));

		/* add the polygon edges */
		while(points != last)
		{
			_LineTo
				(p,                  t,
				 SDPoint_X(*points), SDPoint_Y(*points++));
		}

		/* complete the polygon */
		_LineToClose
			(p,                  t,
			 SDPoint_X(*points), SDPoint_Y(*points));
	}

	/* return successfully */
	return SDStatus_OK;
}

/* Add a rectangle to this path. */
SDStatus
SDPath_AddRectangle(SDPath  *_this,
                    SDFloat  x,
                    SDFloat  y,
                    SDFloat  width,
                    SDFloat  height)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* add the rectangle */
	{
		/* declarations */
		SDPointF *p;
		SDByte   *t;

		/* calculate edges */
		const SDFloat right  = (x + width);
		const SDFloat bottom = (y + height);

		/* perform standard startup procedures for new figure additions */
		_BeginNew(_this, 4, p, t, x, y);

		/* add the remaining rectangle sides */
		_LineTo(p, t, right, y);
		_LineTo(p, t, right, bottom);
		_LineToClose(p, t, x, bottom);
	}

	/* return successfully */
	return SDStatus_OK;
}

/* Add a sequence of rectangles to this path. */
SDStatus
SDPath_AddRectangles(SDPath       *_this,
                     SDRectangleF *rects,
                     SDUInt32      count)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a rectangle list pointer */
	SDStatus_Require((rects != 0), SDStatus_ArgumentNull);

	/* bail out now if there's nothing to add */
	SDStatus_Require((count != 0), SDStatus_OK);

	/* add the rectangles */
	{
		/* declarations */
		SDPointF *p;
		SDByte   *t;
		SDFloat   left;
		SDFloat   top;
		SDFloat   right;
		SDFloat   bottom;

		/* get the end of input pointer */
		const SDRectangleF *const end = (rects + count);

		/* get the rectangle edges */
		left   = SDRectangle_X(*rects);
		top    = SDRectangle_Y(*rects);
		right  = left + SDRectangle_Width(*rects);
		bottom = top + SDRectangle_Height(*rects++);

		/* perform standard startup procedures for new figure additions */
		_BeginNew
			(_this, (count * 4),
			 p,     t,
			 left,  top);

		/* complete the first rectangle */
		_LineTo(p, t, right, top);
		_LineTo(p, t, right, bottom);
		_LineToClose(p, t, left, bottom);

		/* add the remaining rectangles */
		while(rects != end)
		{
			/* get the rectangle edges */
			left   = SDRectangle_X(*rects);
			top    = SDRectangle_Y(*rects);
			right  = left + SDRectangle_Width(*rects);
			bottom = top + SDRectangle_Height(*rects++);

			/* add the rectangle */
			_MoveTo(p, t, left, top);
			_LineTo(p, t, right, top);
			_LineTo(p, t, right, bottom);
			_LineToClose(p, t, left, bottom);
		}
	}

	/* return successfully */
	return SDStatus_OK;
}

/* Add a string to this path. */
SDStatus
SDPath_AddString(SDPath         *_this,
                 SDChar16       *s,
                 SDUInt32        length,
                 SDFontFamily   *family,
                 SDFontStyle     style,
                 SDFloat         emSize,
                 SDRectangleF    layoutRect,
                 SDStringFormat *format)
{
	/* TODO */
	return SDStatus_NotImplemented;
}

/* Clear all markers from this path. */
SDStatus
SDPath_ClearMarkers(SDPath *_this)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* clear markers */
	{
		/* declarations */
		SDByte *curr;

		/* get the end pointer */
		const SDByte *const end = (_this->types + _this->count);

		/* clear the markers */
		for(curr = _this->types; curr != end; ++curr)
		{
			*curr &= ~SDPathType_PathMarker;
		}
	}

	/* return successfully */
	return SDStatus_OK;
}

/* Clone this path. */
SDStatus
SDPath_Clone(SDPath  *_this,
             SDPath **clone)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a clone pointer pointer */
	SDStatus_Require((clone != 0), SDStatus_ArgumentNull);

	/* create the clone path */
	SDStatus_Check(SDPath_Create(clone));

	/* clone the members */
	{
		/* declarations */
		SDPointF *tmpP;
		SDByte   *tmpT;

		/* get the count and capacity */
		const SDUInt32 count    = _this->count;
		const SDUInt32 capacity = ((count + 31) & ~31);

		/* allocate the clone points list */
		if(!(tmpP = (SDPointF *)SDMalloc(capacity * sizeof(SDPointF))))
		{
			SDPath_Destroy(clone);
			return SDStatus_OutOfMemory;
		}

		/* allocate the clone types list */
		if(!(tmpT = (SDByte *)SDMalloc(capacity * sizeof(SDByte))))
		{
			SDFree(tmpP);
			SDPath_Destroy(clone);
			return SDStatus_OutOfMemory;
		}

		/* deep copy the points into the clone */
		SDMemCopy(tmpP, _this->points, (count * sizeof(SDPointF)));

		/* deep copy the types into the clone */
		SDMemCopy(tmpT, _this->types, (count * sizeof(SDByte)));

		/* set the clone members */
		(*clone)->capacity  = capacity;
		(*clone)->count     = count;
		(*clone)->points    = tmpP;
		(*clone)->types     = tmpT;
		(*clone)->winding   = _this->winding;
		(*clone)->newFigure = _this->newFigure;
	}

	/* return successfully */
	return SDStatus_OK;
}

/* Close all open figures in this path and start a new one. */
SDStatus
SDPath_CloseAllFigures(SDPath *_this)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* close the figures */
	{
		/* declarations */
		SDByte *curr, *prev;

		/* get the end pointer */
		const SDByte *const end = (_this->types + _this->count);

		/* bail out now if there's nothing to close */
		SDStatus_Require((_this->count <= 1), SDStatus_OK);

		/* get the current pointer */
		curr = (_this->types + 2);

		/* get the previous pointer */
		prev = (_this->types + 1);

		/* close the figures */
		while(curr != end)
		{
			/* close the previous figure if we're on a new figure */
			if(*curr == SDPathType_Start)
			{
				*prev |= SDPathType_CloseSubpath;
			}

			/* advance to next position */
			prev = curr++;
		}

		/* close the current figure */
		*prev |= SDPathType_CloseSubpath;

		/* start a new figure */
		_this->newFigure = 1;
	}

	/* return successfully */
	return SDStatus_OK;
}

/* Close the current figure in this path and start a new one. */
SDStatus
SDPath_CloseFigure(SDPath *_this)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* close the current figure */
	{
		/* bail out now if there's nothing to close */
		SDStatus_Require((_this->count <= 1), SDStatus_OK);

		/* close the current figure */
		_this->types[_this->count - 1] |= SDPathType_CloseSubpath;

		/* start a new figure */
		_this->newFigure = 1;
	}

	/* return successfully */
	return SDStatus_OK;
}

/* Flatten curves in this path into sequences of connected line segments. */
SDStatus
SDPath_Flatten(SDPath   *_this,
               SDMatrix *matrix,
               SDFloat   flatness)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* bail out now if there's nothing to flatten */
	SDStatus_Require((_this->count == 0), SDStatus_OK);

	/* transform the path, as needed */
	if(matrix != 0)
	{
		/* transform the points */
		SDStatus_Check
			(SDMatrix_TransformPoints
				(matrix, _this->points, _this->count));
	}

	/* bail out now if there's nothing more to do */
	SDStatus_Require((_this->hasCurves != 0), SDStatus_OK);

	/* flatten the path */
	{
		/* declarations */
		SDFlattener  flattener;
		SDPointF    *points;
		SDByte      *types;
		SDUInt32     count;
		SDUInt32     capacity;
		SDStatus     status;

		/* initialize the flattener */
		SDFlattener_Initialize(&flattener);

		/* flatten the path */
		status =
			SDFlattener_Flatten
				(&flattener, _this->points, _this->types, _this->count,
				 flatness);

		/* handle flattening failures */
		if(status != SDStatus_OK)
		{
			/* finalize the flattener */
			SDFlattener_Finalize(&flattener, 0, 0, 0, 0);

			/* return status */
			return status;
		}

		/* finalize the flattener */
		SDFlattener_Finalize(&flattener, &points, &types, &count, &capacity);

		/* finalize the current path members */
		SDFree(_this->points);
		SDFree(_this->types);

		/* initialize the members */
		_this->points   = points;
		_this->types    = types;
		_this->count    = count;
		_this->capacity = capacity;
	}

	/* return successfully */
	return SDStatus_OK;
}

/* Get the point count of this path. */
SDStatus
SDPath_GetCount(SDPath   *_this,
                SDUInt32 *count)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a count pointer */
	SDStatus_Require((count != 0), SDStatus_ArgumentNull);

	/* get the count */
	*count = _this->count;

	/* return successfully */
	return SDStatus_OK;
}

/* Get the bounds of this path. */
SDStatus
SDPath_GetBounds(SDPath       *_this,
                 SDMatrix     *matrix,
                 SDPen        *pen,
                 SDRectangleF *bounds)
{
	/* TODO */
	return SDStatus_NotImplemented;
}

/* Get the last point in this path. */
SDStatus
SDPath_GetLastPoint(SDPath   *_this,
                    SDPointF *point)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* get the last point */
	{
		/* ensure we have a point */
		SDStatus_Require((_this->count > 0), SDStatus_Argument);

		/* get the last point */
		*point = _this->points[_this->count - 1];
	}

	/* return successfully */
	return SDStatus_OK;
}

/* Determine if a point is visible within an outline of this path. */
SDStatus
SDPath_IsOutlineVisible(SDPath     *_this,
                        SDFloat     x,
                        SDFloat     y,
                        SDPen      *pen,
                        SDGraphics *graphics,
                        SDBool     *visible)
{
	/* TODO */
	return SDStatus_NotImplemented;
}

/* Determine if a point is visible within this path. */
SDStatus
SDPath_IsVisible(SDPath     *_this,
                 SDFloat     x,
                 SDFloat     y,
                 SDGraphics *graphics,
                 SDBool     *visible)
{
	/* TODO */
	return SDStatus_NotImplemented;
}

/* Reset this path. */
SDStatus
SDPath_Reset(SDPath *_this)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* reset this path */
	{
		/* reset the members */
		_this->count     = 0;
		_this->winding   = 0;
		_this->newFigure = 1;
		_this->hasCurves = 0;
	}

	/* return successfully */
	return SDStatus_OK;
}

/* Reverse the order of the points in this path. */
SDStatus
SDPath_Reverse(SDPath *_this)
{
	/* TODO */
	return SDStatus_NotImplemented;
}

/* Set a marker at the current position in this path. */
SDStatus
SDPath_SetMarker(SDPath *_this)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* set the marker */
	{
		/* ensure we have a point at which to set the marker */
		SDStatus_Require((_this->count > 0), SDStatus_Argument);

		/* set the marker */
		_this->types[_this->count - 1] |= SDPathType_PathMarker;
	}

	/* return successfully */
	return SDStatus_OK;
}

/* Start a new figure in this path without closing the current one. */
SDStatus
SDPath_StartFigure(SDPath *_this)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* start the new figure */
	_this->newFigure = 1;

	/* return successfully */
	return SDStatus_OK;
}

/* Transform the points of this path by a matrix. */
SDStatus
SDPath_Transform(SDPath   *_this,
                 SDMatrix *matrix)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a matrix pointer */
	SDStatus_Require((matrix != 0), SDStatus_ArgumentNull);

	/* transform the points */
	{
		/* bail out now if there's nothing to transform */
		SDStatus_Require((_this->count == 0), SDStatus_OK);

		/* transform the points */
		SDStatus_Check
			(SDMatrix_TransformPoints
				(matrix, _this->points, _this->count));
	}

	/* return successfully */
	return SDStatus_OK;
}

/* Warp the points of this path. */
SDStatus
SDPath_Warp(SDPath     *_this,
            SDMatrix   *matrix,
            SDPointF   *dstPoints,
            SDUInt32    dstLength,
            SDFloat     srcX,
            SDFloat     srcY,
            SDFloat     srcWidth,
            SDFloat     srcHeight,
            SDWarpMode  warpMode,
            SDFloat     flatness)
{
	/* TODO */
	return SDStatus_NotImplemented;
}

/* Widen this path. */
SDStatus
SDPath_Widen(SDPath   *_this,
             SDPen    *pen,
             SDMatrix *matrix,
             SDFloat   flatness)
{
	/* TODO */
	return SDStatus_NotImplemented;
}

/* Transform the points of this path by an affine transformation. */
SDINTERNAL void
SDPath_TransformAffine(SDPath             *_this,
                       SDAffineTransformF *transform)
{
	/* assertions */
	SDASSERT((_this     != 0));
	SDASSERT((transform != 0));

	/* transform the points, as needed */
	if(_this->count != 0)
	{
		SDAffineTransformF_TransformPoints
			(transform, _this->points, _this->count);
	}
}

/* Stroke this path to another path. */
SDINTERNAL SDStatus
SDPath_Stroke(SDPath    *_this,
              SDPath    *stroke,
              SDStroker *stroker)
{
	/* assertions */
	SDASSERT((_this   != 0));
	SDASSERT((stroke  != 0));
	SDASSERT((stroker != 0));

	/* stroke the path */
	SDStatus_Check
		(SDStroker_Stroke
			(stroker, stroke, _this->points, _this->types, _this->count));

	/* return successfully */
	return SDStatus_OK;
}

/* Fill this path to trapezoids. */
SDINTERNAL SDStatus
SDPath_Fill(SDPath       *_this,
            SDTrapezoids *trapezoids)
{
	/* declarations */
	SDFillMode fillMode;

	/* assertions */
	SDASSERT((_this      != 0));
	SDASSERT((trapezoids != 0));

	/* get the fill mode */
	fillMode = (_this->winding ? SDFillMode_Winding : SDFillMode_Alternate);

	/* fill the path */
	SDStatus_Check
		(SDTrapezoids_Fill
			(trapezoids, _this->points, _this->types, _this->count, fillMode));

	/* return successfully */
	return SDStatus_OK;
}


#ifdef __cplusplus
};
#endif
