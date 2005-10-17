/*
 * SDStroker.c - Stroker implementation.
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

#include "CStroker.h"
#include "CAffineTransform.h"
#include "CBezier.h"
#include "CFlattener.h"
#include "CMath.h"
#include "CPen.h"
#include "CPath.h"
#include "CPointArray.h"

#ifdef __cplusplus
extern "C" {
#endif

static const SDStrokeJoiner SDStrokeJoiner_Zero;
static const SDStrokeCapper SDStrokeCapper_Zero;

#define SDStrokeJoiner_Join(j, p, cX, cY, pC, pS, cC, cS) \
	((j)->Join((j), (p), (cX), (cY), (pC), (pS), (cC), (cS)))
#define SDStrokeCapper_Cap(c, p, cX, cY, sX, sY) \
	((c)->Cap((c), (p), (cX), (cY), (sX), (sY)))

#define SDStroker_CirclePoints(array, radius, transform, scale)                \
	do {                                                                       \
		/* declarations */                                                     \
		SDBezierF bezier;                                                      \
		SDPointF  points[12];                                                  \
		                                                                       \
		/* calculate the width and radius */                                   \
		const SDDouble r = (radius);                                           \
		const SDDouble w = (r * 2.0f);                                         \
		                                                                       \
		/* calculate the distance along the tangents */                        \
		const SDDouble d = (r * SDMath_Arc90Fraction);                         \
		                                                                       \
		/* calculate the tangential control points */                          \
		const SDFloat p = (r + d);                                             \
		const SDFloat m = (r - d);                                             \
		                                                                       \
		/* initialize the points */                                            \
		SDPoint_X(points[0])  = w; SDPoint_Y(points[0])  = r;                  \
		SDPoint_X(points[1])  = w; SDPoint_Y(points[1])  = m;                  \
		SDPoint_X(points[2])  = p; SDPoint_Y(points[2])  = 0;                  \
		SDPoint_X(points[3])  = r; SDPoint_Y(points[3])  = 0;                  \
		SDPoint_X(points[4])  = m; SDPoint_Y(points[4])  = 0;                  \
		SDPoint_X(points[5])  = 0; SDPoint_Y(points[5])  = m;                  \
		SDPoint_X(points[6])  = 0; SDPoint_Y(points[6])  = r;                  \
		SDPoint_X(points[7])  = 0; SDPoint_Y(points[7])  = p;                  \
		SDPoint_X(points[8])  = m; SDPoint_Y(points[8])  = w;                  \
		SDPoint_X(points[9])  = r; SDPoint_Y(points[9])  = w;                  \
		SDPoint_X(points[10]) = p; SDPoint_Y(points[10]) = w;                  \
		SDPoint_X(points[11]) = w; SDPoint_Y(points[11]) = p;                  \
		                                                                       \
		/* transform or scale the points */                                    \
		if((transform) != 0)                                                   \
		{                                                                      \
			SDAffineTransformF_TransformPoints((transform), points, 12);       \
		}                                                                      \
		else                                                                   \
		{                                                                      \
			SDVectorF_ScalePoints((scale), points, 12);                        \
		}                                                                      \
		                                                                       \
		/* reset the count */                                                  \
		SDPointArray_Count(*(array)) = 0;                                      \
		                                                                       \
		/* initialize the first quadrant */                                    \
		SDBezierF_Initialize                                                   \
			(&bezier, &points[0], &points[1], &points[2], &points[3]);         \
		                                                                       \
		/* flatten the first quadrant */                                       \
		SDStatus_Check                                                         \
			(SDBezierF_Flatten                                                 \
				(&bezier, (array), SDFiller_TOLERANCE));                       \
		                                                                       \
		/* initialize the second quadrant */                                   \
		SDBezierF_Initialize                                                   \
			(&bezier, &points[3], &points[4], &points[5], &points[6]);         \
		                                                                       \
		/* flatten the second quadrant */                                      \
		SDStatus_Check                                                         \
			(SDBezierF_Flatten                                                 \
				(&bezier, (array), SDFiller_TOLERANCE));                       \
		                                                                       \
		/* initialize the third quadrant */                                    \
		SDBezierF_Initialize                                                   \
			(&bezier, &points[6], &points[7], &points[8], &points[9]);         \
		                                                                       \
		/* flatten the third quadrant */                                       \
		SDStatus_Check                                                         \
			(SDBezierF_Flatten                                                 \
				(&bezier, (array), SDFiller_TOLERANCE));                       \
		                                                                       \
		/* initialize the fourth quadrant */                                   \
		SDBezierF_Initialize                                                   \
			(&bezier, &points[9], &points[10], &points[11], &points[0]);       \
		                                                                       \
		/* flatten the fourth quadrant */                                      \
		SDStatus_Check                                                         \
			(SDBezierF_Flatten                                                 \
				(&bezier, (array), SDFiller_TOLERANCE));                       \
	} while(0)

#define SDStroker_TempSpacePoints(array, points, count, size)                  \
	do {                                                                       \
		/* declarations */                                                     \
		SDPointF *tmp;                                                         \
		                                                                       \
		/* get the points */                                                   \
		tmp = SDPointArray_Points(*(array));                                   \
		                                                                       \
		/* get the count */                                                    \
		(count) = SDPointArray_Count(*(array));                                \
		                                                                       \
		/* calculate the size */                                               \
		(size) = ((count) * sizeof(SDPointF));                                 \
		                                                                       \
		/* allocate the points */                                              \
		if(!((points) = (SDPointF *)SDMalloc((size) << 1)))                    \
		{                                                                      \
			return SDStatus_OutOfMemory;                                       \
		}                                                                      \
		                                                                       \
		/* copy the points */                                                  \
		SDMemCopy((points), tmp, (size));                                      \
	} while(0)

static SDStatus
SDStroker_StrokeSubpaths(SDStroker *_this,
                         SDPath    *path,
                         SDPointF  *points,
                         SDByte    *types,
                         SDUInt32   count)
{
	/* declarations */
	SDByte   *type;
	SDPointF *curr;
	SDPointF *end;

	/* assertions */
	SDASSERT((_this     != 0));
	SDASSERT((path      != 0));
	SDASSERT((points    != 0));
	SDASSERT((types     != 0));

	/* get the current type pointer */
	type = types;

	/* get the current point pointer */
	curr = points;

	/* get the end pointer */
	end = (curr + count);

	/* reset the count of the array */
	SDPointArray_Count(_this->array) = 0;

	/* stroke the subpaths */
	while(curr != end)
	{
		/* declarations */
		SDPointF *first;

		/* get the first point */
		first = curr;

		/* get the current subpath */
		while(curr != end && ((*type & SDPathType_Start) == 0))
		{
			/* add the current point */
			SDStatus_Check
				(SDPointArrayF_AppendPointNoRepeat
					(&(_this->array), curr));

			/* close, as needed */
			if((*type & SDPathType_CloseSubpath) != 0)
			{
				SDStatus_Check
					(SDPointArrayF_AppendPointNoRepeat
						(&(_this->array), first));
			}

			/* move to the next position */
			++type; ++curr;
		}

		/* stroke the subpath, as needed */
		if(SDPointArray_Count(_this->array) != 0)
		{
			/* stroke the subpath */
			SDStatus_Check
				(_this->Stroke
					(_this, path, SDPointArray_Points(_this->array),
					 SDPointArray_Count(_this->array)));

			/* reset for next subpath */
			SDPointArray_Count(_this->array) = 0;
		}
	}

	/* apply device transformation */
	SDPath_TransformAffine(path, &(_this->dev));

	/* return successfully */
	return SDStatus_OK;
}

static SDStatus
SDStroker_FullStroke(SDStroker *_this,
                     SDPath    *path,
                     SDPointF  *points,
                     SDUInt32   count)
{
	/* declarations */
	SDAffineTransformF *transform;
	SDPointF           *end;
	SDPointF           *curr;
	SDPointF           *next;
	SDPointF           *last;
	SDBool              needCap;
	SDFloat             prevC;
	SDFloat             prevS;
	SDFloat             radius;

	/* assertions */
	SDASSERT((_this  != 0));
	SDASSERT((path   != 0));
	SDASSERT((points != 0));

	/* get the end of points pointer */
	end  = (points + count);

	/* get the current point pointer */
	curr = points;

	/* get the next point pointer */
	next = (curr + 1);

	/* get the last point pointer */
	last = (end - 1);

	/* flag that we need a start cap */
	needCap = 1;

	/* get the pen transformation */
	transform = &(_this->pen);

	/* get the pen radius */
	radius = _this->radius;

	/* generate the stroke */
	while(next != end)
	{
		/* declarations */
		SDFloat  currX;
		SDFloat  currY;
		SDFloat  nextX;
		SDFloat  nextY;
		SDFloat  slopeX;
		SDFloat  slopeY;
		SDFloat  length;
		SDPointF delta[4];

		/* get the current and next coordinates */
		currX = SDPoint_X(*curr);
		currY = SDPoint_Y(*curr);
		nextX = SDPoint_X(*next);
		nextY = SDPoint_Y(*next);

		/* calculate the line slope */
		slopeX = (nextX - currX);
		slopeY = (nextY - currY);

		/* calculate the line length */
		length =
			(SDFloat)SDMath_Sqrt
				((slopeX * slopeX) + (slopeY * slopeY));

		/* stroke non-degenerate lines */
		if(length != 0.0f)
		{
			/* get the current sine and cosine */
			slopeX = slopeX / length;
			slopeY = slopeY / length;

			/* add a join or start cap, as needed */
			if(needCap)
			{
				/* add the start cap, as needed */
				if((_this->startCapper.Cap) != 0)
				{
					SDStatus_Check
						(SDStrokeCapper_Cap
							(&(_this->startCapper), path, &currX, &currY,
							 slopeX, slopeY));
				}

				/* reset the need start cap flag */
				needCap = 0;
			}
			else
			{
				/* add the join */
				SDStatus_Check
					(SDStrokeJoiner_Join
						(&(_this->joiner), path, currX, currY,
						 prevC, prevS, slopeX, slopeY));
			}

			/* add the end cap, as needed */
			if(next == last && ((_this->endCapper.Cap) != 0))
			{
				SDStatus_Check
					(SDStrokeCapper_Cap
						(&(_this->endCapper), path, &nextX, &nextY,
						 -slopeX, -slopeY));
			}

			/* calculate the stroke bounds */
			SDPoint_X(delta[0]) =  slopeY * radius;
			SDPoint_Y(delta[0]) = -slopeX * radius;
			SDPoint_X(delta[1]) = -slopeY * radius;
			SDPoint_Y(delta[1]) =  slopeX * radius;
			SDPoint_X(delta[2]) =  SDPoint_X(delta[1]);
			SDPoint_Y(delta[2]) =  SDPoint_Y(delta[1]);
			SDPoint_X(delta[3]) =  SDPoint_X(delta[0]);
			SDPoint_Y(delta[3]) =  SDPoint_Y(delta[0]);

			/* transform by the pen transformation */
			SDAffineTransformF_TransformPoints(transform, delta, 4);

			/* translate the stroke into place */
			SDPoint_X(delta[0]) += currX;
			SDPoint_Y(delta[0]) += currY;
			SDPoint_X(delta[1]) += currX;
			SDPoint_Y(delta[1]) += currY;
			SDPoint_X(delta[2]) += nextX;
			SDPoint_Y(delta[2]) += nextY;
			SDPoint_X(delta[3]) += nextX;
			SDPoint_Y(delta[3]) += nextY;

			/* add the stroke to the path */
			SDStatus_Check
				(SDPath_AddPolygon
					(path, delta, 4));

			/* update the points */
			curr = next++;

			/* update the previous slope */
			prevC = slopeX;
			prevS = slopeY;
		}
	}

	/* handle degenerate subpaths */
	if(needCap)
	{
		/* add single point caps */
		{
			/* add a start cap, as needed */
			if((_this->startCapper.Cap) != 0)
			{
				/* get the current x coordinate */
				SDFloat x = SDPoint_X(*curr);

				/* get the current y coordinate */
				SDFloat y = SDPoint_Y(*curr);

				/* add the start cap */
				SDStatus_Check
					(SDStrokeCapper_Cap
						(&(_this->startCapper), path, &x, &y, 1.0f, 0.0f));
			}

			/* add an end cap, as needed */
			if((_this->endCapper.Cap) != 0)
			{
				/* get the current x coordinate */
				SDFloat x = SDPoint_X(*curr);

				/* get the current y coordinate */
				SDFloat y = SDPoint_Y(*curr);

				/* add the end cap */
				SDStatus_Check
					(SDStrokeCapper_Cap
						(&(_this->endCapper), path, &x, &y, -1.0f, 0.0f));
			}
		}
	}

	/* return successfully */
	return SDStatus_OK;
}

static SDStatus
SDStroker_FastStroke(SDStroker *_this,
                     SDPath    *path,
                     SDPointF  *points,
                     SDUInt32   count)
{
	/* declarations */
	SDPointF           *end;
	SDPointF           *curr;
	SDPointF           *next;

	/* assertions */
	SDASSERT((_this  != 0));
	SDASSERT((path   != 0));
	SDASSERT((points != 0));

	/* get the end of points pointer */
	end  = (points + count);

	/* get the current point pointer */
	curr = points;

	/* get the next point pointer */
	next = (curr + 1);

	/* generate the stroke */
	while(next != end)
	{
		/* declarations */
		SDFloat  currX;
		SDFloat  currY;
		SDFloat  nextX;
		SDFloat  nextY;
		SDFloat  slopeX;
		SDFloat  slopeY;
		SDFloat  length;
		SDPointF delta[4];

		/* get the current and next coordinates */
		currX = SDPoint_X(*curr);
		currY = SDPoint_Y(*curr);
		nextX = SDPoint_X(*next);
		nextY = SDPoint_Y(*next);

		/* calculate the line slope */
		slopeX = (nextX - currX);
		slopeY = (nextY - currY);

		/* calculate the line length */
		length =
			(SDFloat)SDMath_Sqrt
				((slopeX * slopeX) + (slopeY * slopeY));

		/* stroke non-degenerate lines */
		if(length != 0.0f)
		{
			/* get the current sine and cosine */
			slopeX = slopeX / length;
			slopeY = slopeY / length;

			/* calculate the stroke bounds */
			SDPoint_X(delta[0]) =  slopeY * 0.5f;
			SDPoint_Y(delta[0]) = -slopeX * 0.5f;
			SDPoint_X(delta[1]) = -slopeY * 0.5f;
			SDPoint_Y(delta[1]) =  slopeX * 0.5f;
			SDPoint_X(delta[2]) =  SDPoint_X(delta[1]);
			SDPoint_Y(delta[2]) =  SDPoint_Y(delta[1]);
			SDPoint_X(delta[3]) =  SDPoint_X(delta[0]);
			SDPoint_Y(delta[3]) =  SDPoint_Y(delta[0]);

			/* translate the stroke into place */
			SDPoint_X(delta[0]) += currX;
			SDPoint_Y(delta[0]) += currY;
			SDPoint_X(delta[1]) += currX;
			SDPoint_Y(delta[1]) += currY;
			SDPoint_X(delta[2]) += nextX;
			SDPoint_Y(delta[2]) += nextY;
			SDPoint_X(delta[3]) += nextX;
			SDPoint_Y(delta[3]) += nextY;

			/* add the stroke to the path */
			SDStatus_Check
				(SDPath_AddPolygon
					(path, delta, 4));

			/* update the points */
			curr = next++;
		}
	}

	/* return successfully */
	return SDStatus_OK;
}

static SDMATH SDPointF
SDStrokeJoiner_MiterIntersect(SDPointF a,
                              SDPointF b,
                              SDFloat  prevC,
                              SDFloat  prevS,
                              SDFloat  currC,
                              SDFloat  currS,
                              SDDouble cross)
{
	/* declarations */
	SDPointF intersect;
	SDDouble  iX;
	SDDouble  iY;

	/* get the point components */
	const SDFloat aX = SDPoint_X(a);
	const SDFloat aY = SDPoint_Y(a);
	const SDFloat bX = SDPoint_X(b);
	const SDFloat bY = SDPoint_Y(b);

	/* calculate the product of the previous sine and current cosine */
	const SDDouble pScC = (prevS * currC);

	/* calculate the product of the previous cosine and current sine */
	const SDDouble pCcS = (prevC * currS);

	/* calculate the product of the previous sine and current sine */
	const SDDouble pScS = (prevS * currS);

	/* calculate the vertical component of the intersection vector */
	iY  = ((((bX - aX) * pScS) + (aY * pCcS) - (bY * pScC)) / cross);

	/* calculate the horizontal component of the intersection vector */
	iX = (SDMath_Abs(prevS) >= SDMath_Abs(currS)) ?
	     ((((iY - aY) * prevC) / prevS) + aX) :
	     ((((iY - bY) * currC) / currS) + bX);

	/* set the components of the intersection vector */
	SDPoint_X(intersect) = (SDFloat)iX;
	SDPoint_Y(intersect) = (SDFloat)iY;

	/* return the intersection vector */
	return intersect;
}

static SDStatus
SDStrokeJoiner_AddMiter(SDStrokeJoiner *_this,
                        SDPath         *path,
                        SDFloat         centerX,
                        SDFloat         centerY,
                        SDFloat         prevC,
                        SDFloat         prevS,
                        SDFloat         currC,
                        SDFloat         currS)
{
	/* assertions */
	SDASSERT((_this != 0));
	SDASSERT((path  != 0));

	/* bail out now if there's nothing to do */
	SDStatus_Require(((currC != prevC) || (currS != prevS)), SDStatus_OK);

	/* add the miter */
	{
		/* declarations */
		SDVectorF center;

		/* calculate the dot and cross products */
		const SDDouble dot   = SDMath_DotProduct(prevC, prevS, currC, currS);
		const SDDouble cross = SDMath_CrossProduct(prevC, prevS, currC, currS);

		/* get the center point */
		SDVector_X(center) = centerX;
		SDVector_Y(center) = centerY;

		/* add the join based on the limit and angle */
		if((_this->u.other.limitSquared * (1 + dot)) >= 2)
		{
			/* declarations */
			SDPointF delta[4];

			/* set the first point */
			SDPoint_X(delta[0]) = 0.0f;
			SDPoint_Y(delta[0]) = 0.0f;

			/* set the second and fourth points based on angle */
			if(cross < 0.0f)
			{
				SDPoint_X(delta[1]) =  prevS;
				SDPoint_Y(delta[1]) = -prevC;
				SDPoint_X(delta[3]) =  currS;
				SDPoint_Y(delta[3]) = -currC;
			}
			else
			{
				SDPoint_X(delta[1]) = -prevS;
				SDPoint_Y(delta[1]) =  prevC;
				SDPoint_X(delta[3]) = -currS;
				SDPoint_Y(delta[3]) =  currC;
			}

			/* calculate the intersection */
			delta[2] =
				SDStrokeJoiner_MiterIntersect
					(delta[1], delta[3], prevC, prevS, currC, currS, cross);

			/* scale the join to the stroke size */
			SDVectorF_ScalePoints(&(_this->u.other.scale), delta, 4);

			/* transform the join */
			SDAffineTransformF_TransformPoints
				(_this->u.other.transform, delta, 4);

			/* translate the join into place */
			SDVectorF_TranslatePoints(&center, delta, 4);

			/* add the join to the path */
			SDStatus_Check
				(SDPath_AddPolygon
					(path, delta, 4));
		}
		else
		{
			/* declarations */
			SDPointF delta[3];

			/* set the first point */
			SDPoint_X(delta[0]) = 0.0f;
			SDPoint_Y(delta[0]) = 0.0f;

			/* set the second and third points based on angle */
			if(cross < 0.0f)
			{
				SDPoint_X(delta[1]) =  prevS;
				SDPoint_Y(delta[1]) = -prevC;
				SDPoint_X(delta[2]) =  currS;
				SDPoint_Y(delta[2]) = -currC;
			}
			else
			{
				SDPoint_X(delta[1]) = -prevS;
				SDPoint_Y(delta[1]) =  prevC;
				SDPoint_X(delta[2]) = -currS;
				SDPoint_Y(delta[2]) =  currC;
			}

			/* scale the join to the stroke size */
			SDVectorF_ScalePoints(&(_this->u.other.scale), delta, 3);

			/* transform the join */
			SDAffineTransformF_TransformPoints
				(_this->u.other.transform, delta, 3);

			/* translate the join into place */
			SDVectorF_TranslatePoints(&center, delta, 3);

			/* add the join to the path */
			SDStatus_Check
				(SDPath_AddPolygon
					(path, delta, 3));
		}
	}

	/* return successfully */
	return SDStatus_OK;
}

static SDStatus
SDStrokeJoiner_AddRound(SDStrokeJoiner *_this,
                        SDPath         *path,
                        SDFloat         centerX,
                        SDFloat         centerY,
                        SDFloat         prevC,
                        SDFloat         prevS,
                        SDFloat         currC,
                        SDFloat         currS)
{
	/* assertions */
	SDASSERT((_this != 0));
	SDASSERT((path  != 0));

	/* bail out now if there's nothing to do */
	SDStatus_Require(((currC != prevC) || (currS != prevS)), SDStatus_OK);

	/* add the circle */
	{
		/* declarations */
		SDPointF  *points;
		SDPointF  *tmp;
		SDVectorF  center;

		/* get the center point */
		SDVector_X(center) = centerX;
		SDVector_X(center) = centerY;

		/* get the points */
		points = _this->u.round.points;

		/* get the temporary space */
		tmp = (points + _this->u.round.count);

		/* copy the points into temporary space */
		SDMemCopy(tmp, points, _this->u.round.size);

		/* translate the cap into place */
		SDVectorF_TranslatePoints(&center, tmp, _this->u.round.count);

		/* add the cap to the path */
		SDStatus_Check
			(SDPath_AddPolygon
				(path, tmp, _this->u.round.count));
	}

	/* return successfully */
	return SDStatus_OK;
}

static SDStatus
SDStrokeJoiner_AddBevel(SDStrokeJoiner *_this,
                        SDPath         *path,
                        SDFloat         centerX,
                        SDFloat         centerY,
                        SDFloat         prevC,
                        SDFloat         prevS,
                        SDFloat         currC,
                        SDFloat         currS)
{
	/* assertions */
	SDASSERT((_this != 0));
	SDASSERT((path  != 0));

	/* bail out now if there's nothing to do */
	SDStatus_Require(((currC != prevC) || (currS != prevS)), SDStatus_OK);

	/* add the bevel */
	{
		/* declarations */
		SDVectorF center;
		SDPointF  delta[3];

		/* calculate the cross product */
		const SDDouble cross = SDMath_CrossProduct(prevC, prevS, currC, currS);

		/* get the center point */
		SDVector_X(center) = centerX;
		SDVector_Y(center) = centerY;

		/* set the first point */
		SDPoint_X(delta[0]) = 0.0f;
		SDPoint_Y(delta[0]) = 0.0f;

		/* set the second and third points based on angle */
		if(cross < 0.0f)
		{
			SDPoint_X(delta[1]) =  prevS;
			SDPoint_Y(delta[1]) = -prevC;
			SDPoint_X(delta[2]) =  currS;
			SDPoint_Y(delta[2]) = -currC;
		}
		else
		{
			SDPoint_X(delta[1]) = -prevS;
			SDPoint_Y(delta[1]) =  prevC;
			SDPoint_X(delta[2]) = -currS;
			SDPoint_Y(delta[2]) =  currC;
		}

		/* scale the join to the stroke size */
		SDVectorF_ScalePoints(&(_this->u.other.scale), delta, 3);

		/* transform the join */
		SDAffineTransformF_TransformPoints
			(_this->u.other.transform, delta, 3);

		/* translate the join into place */
		SDVectorF_TranslatePoints(&center, delta, 3);

		/* add the join to the path */
		SDStatus_Check
			(SDPath_AddPolygon
				(path, delta, 3));
	}

	/* return successfully */
	return SDStatus_OK;
}

static SDStatus
SDStrokeCapper_AddSquare(SDStrokeCapper *_this,
                         SDPath         *path,
                         SDFloat        *centerX,
                         SDFloat        *centerY,
                         SDFloat         slopeX,
                         SDFloat         slopeY)
{
	/* assertions */
	SDASSERT((_this   != 0));
	SDASSERT((path    != 0));
	SDASSERT((centerX != 0));
	SDASSERT((centerY != 0));

	/* add the square */
	{
		/* declarations */
		SDPointF  delta[4];
		SDVectorF center;
		SDFloat   dX;
		SDFloat   dY;

		/* get the center point */
		SDVector_X(center) = *centerX;
		SDVector_Y(center) = *centerY;

		/* calculate the bounds components */
		dX = (slopeX * _this->u.other.radius);
		dY = (slopeY * _this->u.other.radius);

		/* calculate the cap bounds */
		SDPoint_X(delta[0]) =  (dY + dX);
		SDPoint_Y(delta[0]) =  (dY - dX);
		SDPoint_X(delta[1]) =  (dY);
		SDPoint_Y(delta[1]) = -(dX);
		SDPoint_X(delta[2]) = -(dY);
		SDPoint_Y(delta[2]) =  (dX);
		SDPoint_X(delta[3]) = -(SDPoint_Y(delta[0]));
		SDPoint_Y(delta[3]) =  (SDPoint_X(delta[0]));

		/* transform the cap */
		SDAffineTransformF_TransformPoints
			(_this->u.other.u.transform, delta, 4);

		/* translate the cap into place */
		SDVectorF_TranslatePoints(&center, delta, 4);

		/* add the stroke to the path */
		SDStatus_Check
			(SDPath_AddPolygon
				(path, delta, 4));
	}

	/* return successfully */
	return SDStatus_OK;
}

static SDStatus
SDStrokeCapper_AddSquareAnchor(SDStrokeCapper *_this,
                               SDPath         *path,
                               SDFloat        *centerX,
                               SDFloat        *centerY,
                               SDFloat         slopeX,
                               SDFloat         slopeY)
{
	/* assertions */
	SDASSERT((_this   != 0));
	SDASSERT((path    != 0));
	SDASSERT((centerX != 0));
	SDASSERT((centerY != 0));

	/* add the square */
	{
		/* declarations */
		SDPointF  delta[4];
		SDVectorF center;
		SDFloat   dX;
		SDFloat   dY;

		/* get the center point */
		SDVector_X(center) = *centerX;
		SDVector_Y(center) = *centerY;

		/* calculate the bounds components */
		dX = (slopeX * _this->u.other.radius);
		dY = (slopeY * _this->u.other.radius);

		/* calculate the cap bounds */
		SDPoint_X(delta[0]) =  (dY + dX);
		SDPoint_Y(delta[0]) =  (dY - dX);
		SDPoint_X(delta[1]) =  SDPoint_Y(delta[0]);
		SDPoint_Y(delta[1]) = -SDPoint_X(delta[0]);
		SDPoint_X(delta[2]) = -SDPoint_X(delta[0]);
		SDPoint_Y(delta[2]) = -SDPoint_Y(delta[0]);
		SDPoint_X(delta[3]) = -SDPoint_Y(delta[0]);
		SDPoint_Y(delta[3]) =  SDPoint_X(delta[0]);

		/* transform the cap */
		SDVectorF_ScalePoints(_this->u.other.u.scale, delta, 4);

		/* translate the cap into place */
		SDVectorF_TranslatePoints(&center, delta, 4);

		/* add the stroke to the path */
		SDStatus_Check
			(SDPath_AddPolygon
				(path, delta, 4));
	}

	/* return successfully */
	return SDStatus_OK;
}

static SDStatus
SDStrokeCapper_AddTriangle(SDStrokeCapper *_this,
                           SDPath         *path,
                           SDFloat        *centerX,
                           SDFloat        *centerY,
                           SDFloat         slopeX,
                           SDFloat         slopeY)
{
	/* assertions */
	SDASSERT((_this   != 0));
	SDASSERT((path    != 0));
	SDASSERT((centerX != 0));
	SDASSERT((centerY != 0));

	/* add the triangle */
	{
		/* declarations */
		SDPointF  delta[3];
		SDVectorF center;
		SDFloat   dX;
		SDFloat   dY;

		/* get the center point */
		SDVector_X(center) = *centerX;
		SDVector_Y(center) = *centerY;

		/* calculate the bounds components */
		dX = (slopeX * _this->u.other.radius);
		dY = (slopeY * _this->u.other.radius);

		/* calculate the cap bounds */
		SDPoint_X(delta[0]) =  dY;
		SDPoint_Y(delta[0]) = -dX;
		SDPoint_X(delta[1]) =  dX;
		SDPoint_Y(delta[1]) =  dY;
		SDPoint_X(delta[2]) = -dY;
		SDPoint_Y(delta[2]) =  dX;

		/* transform the cap */
		SDAffineTransformF_TransformPoints
			(_this->u.other.u.transform, delta, 3);

		/* translate the cap into place */
		SDVectorF_TranslatePoints(&center, delta, 3);

		/* add the cap to the path */
		SDStatus_Check
			(SDPath_AddPolygon
				(path, delta, 3));
	}

	/* return successfully */
	return SDStatus_OK;
}

static SDStatus
SDStrokeCapper_AddDiamondAnchor(SDStrokeCapper *_this,
                                SDPath         *path,
                                SDFloat        *centerX,
                                SDFloat        *centerY,
                                SDFloat         slopeX,
                                SDFloat         slopeY)
{
	/* assertions */
	SDASSERT((_this   != 0));
	SDASSERT((path    != 0));
	SDASSERT((centerX != 0));
	SDASSERT((centerY != 0));

	/* add the diamond */
	{
		/* declarations */
		SDPointF  delta[4];
		SDVectorF center;
		SDFloat   dX;
		SDFloat   dY;

		/* get the center point */
		SDVector_X(center) = *centerX;
		SDVector_Y(center) = *centerY;

		/* calculate the bounds components */
		dX = (slopeX * _this->u.other.radius);
		dY = (slopeY * _this->u.other.radius);

		/* calculate the cap bounds */
		SDPoint_X(delta[0]) =  dY;
		SDPoint_Y(delta[0]) = -dX;
		SDPoint_X(delta[1]) =  dX;
		SDPoint_Y(delta[1]) =  dY;
		SDPoint_X(delta[2]) = -dY;
		SDPoint_Y(delta[2]) =  dX;
		SDPoint_X(delta[3]) = -dX;
		SDPoint_Y(delta[3]) = -dY;

		/* scale the cap */
		SDVectorF_ScalePoints(_this->u.other.u.scale, delta, 4);

		/* translate the cap into place */
		SDVectorF_TranslatePoints(&center, delta, 4);

		/* add the cap to the path */
		SDStatus_Check
			(SDPath_AddPolygon
				(path, delta, 4));
	}

	/* return successfully */
	return SDStatus_OK;
}

static SDStatus
SDStrokeCapper_AddRound(SDStrokeCapper *_this,
                        SDPath         *path,
                        SDFloat        *centerX,
                        SDFloat        *centerY,
                        SDFloat         slopeX,
                        SDFloat         slopeY)
{
	/* assertions */
	SDASSERT((_this   != 0));
	SDASSERT((path    != 0));
	SDASSERT((centerX != 0));
	SDASSERT((centerY != 0));

	/* add the circle */
	{
		/* declarations */
		SDPointF  *points;
		SDPointF  *tmp;
		SDVectorF  center;

		/* get the center point */
		SDVector_X(center) = *centerX;
		SDVector_X(center) = *centerY;

		/* get the points */
		points = _this->u.round.points;

		/* get the temporary space */
		tmp = (points + _this->u.round.count);

		/* copy the points into temporary space */
		SDMemCopy(tmp, points, _this->u.round.size);

		/* translate the cap into place */
		SDVectorF_TranslatePoints(&center, tmp, _this->u.round.count);

		/* add the cap to the path */
		SDStatus_Check
			(SDPath_AddPolygon
				(path, tmp, _this->u.round.count));
	}

	/* return successfully */
	return SDStatus_OK;
}

static SDStatus
SDStrokeCapper_AddArrowAnchor(SDStrokeCapper *_this,
                              SDPath         *path,
                              SDFloat        *centerX,
                              SDFloat        *centerY,
                              SDFloat         slopeX,
                              SDFloat         slopeY)
{
	/* assertions */
	SDASSERT((_this   != 0));
	SDASSERT((path    != 0));
	SDASSERT((centerX != 0));
	SDASSERT((centerY != 0));

	/* add the arrow */
	{
		/* declarations */
		SDPointF  delta[3];
		SDVectorF center;
		SDFloat   dX;
		SDFloat   dY;
		SDFloat   oX;
		SDFloat   oY;

		/*\
		|*| NOTE: the arrow cap is essentially two 30-60 right triangles, with
		|*|       the visible outer angled edges being at 30/210 degrees from
		|*|       the center line, so, if 'offset' is the length from the
		|*|       given center point to the back of the arrow head:
		|*|
		|*|         given that:
		|*|           sin(30)    = 0.5
		|*|           sin(t)     = opposite / hypotenuse
		|*|           cos(t)     = adjacent / hypotenuse
		|*|
		|*|         and substituting:
		|*|           opposite   = radius
		|*|
		|*|         we get:
		|*|           hypotenuse = 2 * radius
		|*|           adjacent   = cos(30) * (2 * radius)
		|*|
		|*|         and therefore:
		|*|           offset     = -(slope * radius * cos(30) * 2)
		|*|
		\*/

		/* get the center point */
		SDVector_X(center) = *centerX;
		SDVector_Y(center) = *centerY;

		/* calculate the bounds components */
		dX = (slopeX * _this->u.other.radius);
		dY = (slopeY * _this->u.other.radius);

		/* define the constant for offset calculation */
		#undef _COS30NEG2
		#define _COS30NEG2 (-1.732050807568877f)

		/* calculate the offset */
		oX = (dX * _COS30NEG2);
		oY = (dY * _COS30NEG2);

		/* undefine the constant for offset calculation */
		#undef _COS30NEG2

		/* calculate the cap bounds */
		SDPoint_X(delta[0]) = 0.0f;
		SDPoint_Y(delta[0]) = 0.0f;
		SDPoint_X(delta[1]) = (oX + dY);
		SDPoint_Y(delta[1]) = (oY - dX);
		SDPoint_X(delta[2]) = (oX - dY);
		SDPoint_Y(delta[2]) = (oY + dX);

		/* transform the cap */
		SDVectorF_ScalePoints(_this->u.other.u.scale, delta, 3);

		/* translate the cap into place */
		SDVectorF_TranslatePoints(&center, delta, 3);

		/* add the cap to the path */
		SDStatus_Check
			(SDPath_AddPolygon
				(path, delta, 3));

		/* update the center point */
		*centerX = (oX + SDVector_X(center));
		*centerY = (oY + SDVector_Y(center));
	}

	/* return successfully */
	return SDStatus_OK;
}

static SDStatus
SDStrokeJoiner_Initialize(SDStrokeJoiner     *_this,
                          SDPointArrayF      *array,
                          SDAffineTransformF *transform,
                          SDLineJoin          join,
                          SDFloat             radius,
                          SDFloat             limit)
{
	/* assertions */
	SDASSERT((_this     != 0));
	SDASSERT((array     != 0));
	SDASSERT((transform != 0));

	/* set the type to the default */
	_this->type = 0;

	/* initialize the joiner */
	switch(join)
	{
		case SDLineJoin_Miter:
		case SDLineJoin_MiterClipped:
		{
			/*\
			|*| TODO: the fallback for Miter should be a partial miter, out to
			|*|       a distance of the limit times the width, instead of a
			|*|       bevel fallback, as is the case for MiterClipped
			\*/
			_this->u.other.transform         = transform;
			SDVector_X(_this->u.other.scale) = radius;
			SDVector_Y(_this->u.other.scale) = radius;
			_this->u.other.limitSquared      = (limit * limit);
			_this->Join                      = SDStrokeJoiner_AddMiter;
		}
		break;
		case SDLineJoin_Round:
		{
			/* get the circle points */
			SDStroker_CirclePoints(array, radius, transform, 0);

			/* set the points with temporary spacing */
			SDStroker_TempSpacePoints
				(array,
				 _this->u.round.points,
				 _this->u.round.count,
				 _this->u.round.size);

			/* set the join method */
			_this->Join = SDStrokeJoiner_AddRound;
		}
		break;
		case SDLineJoin_Bevel:
		default:
		{
			_this->u.other.transform         = transform;
			SDVector_X(_this->u.other.scale) = radius;
			SDVector_Y(_this->u.other.scale) = radius;
			_this->Join                      = SDStrokeJoiner_AddBevel;
		}
		break;
	}

	/* set the type */
	_this->type = join;

	/* return successfully */
	return SDStatus_OK;
}

static void
SDStrokeJoiner_Finalize(SDStrokeJoiner *_this)
{
	/* assertions */
	SDASSERT((_this != 0));

	/* finalize the capper */
	if(_this->type == SDLineJoin_Round)
	{
		SDFree(_this->u.round.points);
		_this->type = 0;
	}
}

static SDStatus
SDStrokeCapper_Initialize(SDStrokeCapper     *_this,
                          SDPointArrayF      *array,
                          SDAffineTransformF *transform,
                          SDVectorF          *scale,
                          SDLineCap           cap,
                          SDFloat             radius)
{
	/* assertions */
	SDASSERT((_this     != 0));
	SDASSERT((array     != 0));
	SDASSERT((transform != 0));
	SDASSERT((scale     != 0));

	/* set the type to the default */
	_this->type = 0;

	/* initialize the capper */
	switch(cap)
	{
		case SDLineCap_Square:
		{
			_this->u.other.radius      = radius;
			_this->u.other.u.transform = transform;
			_this->Cap                 = SDStrokeCapper_AddSquare;
		}
		break;
		case SDLineCap_Round:
		case SDLineCap_RoundAnchor:
		{
			/* get the circle points */
			if(cap == SDLineCap_Round)
			{
				SDStroker_CirclePoints(array, radius, transform, 0);
			}
			else
			{
				SDStroker_CirclePoints(array, (radius * 2.0f), 0, scale);
			}

			/* set the points with temporary spacing */
			SDStroker_TempSpacePoints
				(array,
				 _this->u.round.points,
				 _this->u.round.count,
				 _this->u.round.size);

			/* set the cap method */
			_this->Cap = SDStrokeCapper_AddRound;
		}
		break;
		case SDLineCap_Triangle:
		{
			_this->u.other.radius      = radius;
			_this->u.other.u.transform = transform;
			_this->Cap                 = SDStrokeCapper_AddTriangle;
		}
		break;
		case SDLineCap_SquareAnchor:
		{
			_this->u.other.radius  = (radius * 1.5f);
			_this->u.other.u.scale = scale;
			_this->Cap             = SDStrokeCapper_AddSquareAnchor;
		}
		break;
		case SDLineCap_DiamondAnchor:
		{
			_this->u.other.radius  = (radius * 2.0f);
			_this->u.other.u.scale = scale;
			_this->Cap             = SDStrokeCapper_AddDiamondAnchor;
		}
		break;
		case SDLineCap_ArrowAnchor:
		{
			_this->u.other.radius  = (radius * 2.0f);
			_this->u.other.u.scale = scale;
			_this->Cap             = SDStrokeCapper_AddArrowAnchor;
		}
		break;
		case SDLineCap_Flat:
		case SDLineCap_NoAnchor:
		case SDLineCap_Custom: /* TODO: add custom cap */
		default:
		{
			_this->Cap  = 0;
		}
		break;
	}

	/* set the type */
	_this->type = cap;

	/* return successfully */
	return SDStatus_OK;
}

static void
SDStrokeCapper_Finalize(SDStrokeCapper *_this)
{
	/* assertions */
	SDASSERT((_this != 0));

	/* finalize the capper */
	if((_this->type & ~SDLineCap_AnchorMask) == SDLineCap_Round)
	{
		SDFree(_this->u.round.points);
		_this->type = 0;
	}
}

SDINTERNAL SDStatus
SDStroker_Initialize(SDStroker          *_this,
                     SDPen              *pen,
                     SDAffineTransformF *deviceTransform)
{
	/* assertions */
	SDASSERT((_this           != 0));
	SDASSERT((pen             != 0));
	SDASSERT((deviceTransform != 0));

	/* initialize the stroker */
	{
		/* declarations */
		SDLineCap  startCap;
		SDLineCap  endCap;
		SDLineJoin lineJoin;
		SDFloat    scaleX;
		SDFloat    scaleY;
		SDFloat    width;
		SDFloat    miterLimit;

		/* get the cap and join information from the pen */
		startCap = pen->startCap;
		endCap   = pen->endCap;
		lineJoin = pen->lineJoin;

		/* get the pen width and miter limit */
		width      = pen->width;
		miterLimit = pen->miterLimit;

		/* calculate the pen radius */
		_this->radius = (width * 0.5f);

		/* copy the transformations */
		_this->dev = *deviceTransform;
		_this->pen =  pen->transform;

		/* extract the scale from the device transformation */
		SDAffineTransformF_ExtractScale
			(&(_this->dev), &scaleX, &scaleY, SDMatrixOrder_Prepend);

		/* set the scale */
		SDVector_X(_this->scale) = scaleX;
		SDVector_Y(_this->scale) = scaleY;

		/* scale the pen transformation */
		SDAffineTransformF_Scale
			(&(_this->pen), scaleX, scaleY, SDMatrixOrder_Append);

		/* initialize the temporary array */
		SDPointArrayF_Initialize(&(_this->array));

		/* TODO: compound and dashed strokes */

		/* prepare to stroke the path */
		if(width <= 0 || ((width * scaleX) == 1  && (width * scaleY) == 1))
		{
			/* set up for fast stroke */
			_this->startCapper = SDStrokeCapper_Zero;
			_this->endCapper   = SDStrokeCapper_Zero;
			_this->joiner      = SDStrokeJoiner_Zero;
			_this->Stroke      = SDStroker_FastStroke;
			_this->dev         = *deviceTransform;
			_this->pen         = SDAffineTransformF_Identity;
		}
		else
		{
			/* initialize the start capper */
			SDStatus_Check
				(SDStrokeCapper_Initialize
					(&(_this->startCapper), &(_this->array), &(_this->pen),
					 &(_this->scale), startCap, _this->radius));

			/* initialize the end capper */
			SDStatus_Check
				(SDStrokeCapper_Initialize
					(&(_this->endCapper), &(_this->array), &(_this->pen),
					 &(_this->scale), endCap, _this->radius));

			/* initialize the joiner */
			SDStatus_Check
				(SDStrokeJoiner_Initialize
					(&(_this->joiner), &(_this->array), &(_this->pen),
					 lineJoin, _this->radius, miterLimit));

			/* set the stroke method */
			_this->Stroke = SDStroker_FullStroke;
		}
	}

	/* return successfully */
	return SDStatus_OK;
}

SDINTERNAL void
SDStroker_Finalize(SDStroker *_this)
{
	/* assertions */
	SDASSERT((_this != 0));

	/* finalize the stroker */
	SDPointArrayF_Finalize(&(_this->array));
	SDStrokeCapper_Finalize(&(_this->startCapper));
	SDStrokeCapper_Finalize(&(_this->endCapper));
	SDStrokeJoiner_Finalize(&(_this->joiner));
	_this->Stroke = 0;
}

SDINTERNAL SDStatus
SDStroker_Stroke(SDStroker *_this,
                 SDPath    *path,
                 SDPointF  *points,
                 SDByte    *types,
                 SDUInt32   count)
{
	/* declarations */
	SDStatus  status;

	/* assertions */
	SDASSERT((_this  != 0));
	SDASSERT((path   != 0));
	SDASSERT((points != 0));
	SDASSERT((types  != 0));

	/* bail out now if there's nothing to do */
	SDStatus_Require((count != 0), SDStatus_OK);

	/* stroke path */
	{
		/* declarations */
		SDPointF *newPoints;
		SDByte   *newTypes;
		SDUInt32  newCount;

		/* assertions */
		SDASSERT((points != 0));
		SDASSERT((types  != 0));

		/* scale the path */
		SDVectorF_ScalePoints(&(_this->scale), points, count);

		/* flatten the path */
		{
			/* declarations */
			SDFlattener flattener;
			SDUInt32    capacity;

			/* initialize the flattener */
			SDFlattener_Initialize(&flattener);

			/* flatten the path */
			status =
				SDFlattener_Flatten
					(&flattener, points, types, count, SDFiller_TOLERANCE);

			/* handle flattening failures */
			if(status != SDStatus_OK)
			{
				/* finalize the flattener */
				SDFlattener_Finalize(&flattener, 0, 0, 0, 0);

				/* return status */
				return status;
			}

			/* finalize the flattener */
			SDFlattener_Finalize
				(&flattener, &newPoints, &newTypes, &newCount, &capacity);

		}

		/* stroke the subpaths */
		status =
			SDStroker_StrokeSubpaths
				(_this, path, newPoints, newTypes, newCount);

		/* free the flattened path */
		SDFree(newPoints);
		SDFree(newTypes);
	}

	/* return status */
	return status;
}


#ifdef __cplusplus
};
#endif
