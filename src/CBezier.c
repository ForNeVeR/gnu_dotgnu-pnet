/*
 * SDBezier.c - Bezier implementation.
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

#include "CBezier.h"

#ifdef __cplusplus
extern "C" {
#endif

SDINTERNAL SDBool
SDBezierX_Initialize(SDBezierX *_this,
                     SDPointX  *a,
                     SDPointX  *b,
                     SDPointX  *c,
                     SDPointX  *d)
{
	/* assertions */
	SDASSERT((_this != 0));
	SDASSERT((a     != 0));
	SDASSERT((b     != 0));
	SDASSERT((c     != 0));
	SDASSERT((d     != 0));

	/* set the control points of the curve */
    _this->a = *a;
    _this->b = *b;
    _this->c = *c;
    _this->d = *d;

	/* bail out now if the curve is degenerate */
	if(SDPoint_X(*a) == SDPoint_X(*d) && SDPoint_Y(*a) == SDPoint_Y(*d))
	{
		/* return with degenerate flag */
		return 1;
	}

	/* return with non-degenerate flag */
	return 0;
}

SDINTERNAL SDBool
SDBezierF_Initialize(SDBezierF *_this,
                     SDPointF  *a,
                     SDPointF  *b,
                     SDPointF  *c,
                     SDPointF  *d)
{
	/* assertions */
	SDASSERT((_this != 0));
	SDASSERT((a     != 0));
	SDASSERT((b     != 0));
	SDASSERT((c     != 0));
	SDASSERT((d     != 0));

	/* set the control points of the curve */
    _this->a = *a;
    _this->b = *b;
    _this->c = *c;
    _this->d = *d;

	/* bail out now if the curve is degenerate */
	if(SDPoint_X(*a) == SDPoint_X(*d) && SDPoint_Y(*a) == SDPoint_Y(*d))
	{
		/* return with degenerate flag */
		return 1;
	}

	/* return with non-degenerate flag */
	return 0;
}


SDINTERNAL void
SDBezierX_Finalize(SDBezierX *_this)
{
	/* assertions */
	SDASSERT((_this != 0));

	/* reset the members */
	*_this = SDBezierX_Zero;
}

SDINTERNAL void
SDBezierF_Finalize(SDBezierF *_this)
{
	/* assertions */
	SDASSERT((_this != 0));

	/* reset the members */
	*_this = SDBezierF_Zero;
}

static SDMATH SDPointX
SDBezierX_Midpoint(SDPointX start,
                   SDPointX end)
{
	/* initialize the midpoint to the start point */
	SDPointX middle = start;

	/* calculate and set the midpoint */
	SDPoint_X(middle) += ((SDPoint_X(end) - SDPoint_X(start)) >> 1);
	SDPoint_Y(middle) += ((SDPoint_Y(end) - SDPoint_Y(start)) >> 1);

	/* return the midpoint */
	return middle;
}

static SDMATH SDPointF
SDBezierF_Midpoint(SDPointF start,
                   SDPointF end)
{
	/* initialize the midpoint to the start point */
	SDPointF middle = start;

	/* calculate and set the midpoint */
	SDPoint_X(middle) += ((SDPoint_X(end) - SDPoint_X(start)) * 0.5f);
	SDPoint_Y(middle) += ((SDPoint_Y(end) - SDPoint_Y(start)) * 0.5f);

	/* return the midpoint */
	return middle;
}

static void
SDBezierX_DeCasteljau(SDBezierX *_this,
                      SDBezierX *start,
                      SDBezierX *end)
{
	/* assertions */
	SDASSERT((_this != 0));
	SDASSERT((start != 0));
	SDASSERT((end   != 0));

	/* initialize the results */
	*start = SDBezierX_Zero;
	*end   = SDBezierX_Zero;

	/* calculate the results */
	{
		/* get the curve control points */
		const SDPointX a = _this->a;
		const SDPointX b = _this->b;
		const SDPointX c = _this->c;
		const SDPointX d = _this->d;

		/* calculate the midpoints */
		const SDPointX ab    = SDBezierX_Midpoint(a, b);
		const SDPointX bc    = SDBezierX_Midpoint(b, c);
		const SDPointX cd    = SDBezierX_Midpoint(c, d);
		const SDPointX abbc  = SDBezierX_Midpoint(ab, bc);
		const SDPointX bccd  = SDBezierX_Midpoint(bc, cd);
		const SDPointX ab_cd = SDBezierX_Midpoint(abbc, bccd);

		/* set the starting curve control points */
		start->a = a;
		start->b = ab;
		start->c = abbc;
		start->d = ab_cd;

		/* set the ending curve control points */
		end->a = ab_cd;
		end->b = bccd;
		end->c = cd;
		end->d = d;
	}
}

static void
SDBezierF_DeCasteljau(SDBezierF *_this,
                      SDBezierF *start,
                      SDBezierF *end)
{
	/* assertions */
	SDASSERT((_this != 0));
	SDASSERT((start != 0));
	SDASSERT((end   != 0));

	/* initialize the results */
	*start = SDBezierF_Zero;
	*end   = SDBezierF_Zero;

	/* calculate the results */
	{
		/* get the curve control points */
		const SDPointF a = _this->a;
		const SDPointF b = _this->b;
		const SDPointF c = _this->c;
		const SDPointF d = _this->d;

		/* calculate the midpoints */
		const SDPointF ab    = SDBezierF_Midpoint(a, b);
		const SDPointF bc    = SDBezierF_Midpoint(b, c);
		const SDPointF cd    = SDBezierF_Midpoint(c, d);
		const SDPointF abbc  = SDBezierF_Midpoint(ab, bc);
		const SDPointF bccd  = SDBezierF_Midpoint(bc, cd);
		const SDPointF ab_cd = SDBezierF_Midpoint(abbc, bccd);

		/* set the starting curve control points */
		start->a = a;
		start->b = ab;
		start->c = abbc;
		start->d = ab_cd;

		/* set the ending curve control points */
		end->a = ab_cd;
		end->b = bccd;
		end->c = cd;
		end->d = d;
	}
}

static SDMATH SDDouble
SDBezierX_DistanceToLine(SDPointX p,
                         SDPointX start,
                         SDPointX end)
{
	/* declarations */
	SDDouble u;

	/*\
	|*|
	|*| L - the line
	|*| S - the start of the line
	|*| E - the end of the line
	|*| P - the point whose distance from the line is to be calculated
	|*| I - intersection of P and L
	|*| m - magnitude squared
	|*|
	|*| . - dot product operator
	|*| x - x accessor
	|*| y - y accessor
	|*|
	|*|   PS = (P - S)
	|*|   ES = (E - S)
	|*|
	|*|   L  = S + (u * ES)
	|*|   I  = (P - L) . (ES)
	|*|      = (PS - (u * ES)) . (ES)
	|*|   ---------------------------------------------
	|*|   m  = ((x(ES) * x(ES)) + (y(ES) * y(ES)))
	|*|   u  = ((x(PS) * x(ES)) + (y(PS) * y(ES))) / m
	|*|
	\*/

	/* calculate the component vectors */
	const SDDouble esX = SDFixed_ToFloat(SDPoint_X(end) - SDPoint_X(start));
	const SDDouble esY = SDFixed_ToFloat(SDPoint_Y(end) - SDPoint_Y(start));
	const SDDouble psX = SDFixed_ToFloat(SDPoint_X(p)   - SDPoint_X(start));
	const SDDouble psY = SDFixed_ToFloat(SDPoint_Y(p)   - SDPoint_Y(start));

	/* handle degenerate case (distance to start point) */
	if(esX == 0.0f && esY == 0.0f)
	{
		/* calculate and return the distance (squared) */
		return SDMath_DotProduct(psX, psY, psX, psY);
	}

	/* calculate the line coefficient */
	u = (SDMath_DotProduct(psX, psY, esX, esY) /
	     SDMath_DotProduct(esX, esY, esX, esY));

	/* handle degenerate case (distance to start point) */
	if(u <= 0.0f)
	{
		/* calculate and return the distance (squared) */
		return SDMath_DotProduct(psX, psY, psX, psY);
	}

	/* handle degenerate case (distance to end point) */
	if(u >= 1.0f)
	{
		/* calculate the component vectors */
		const SDDouble peX = SDFixed_ToFloat(SDPoint_X(p) - SDPoint_X(end));
		const SDDouble peY = SDFixed_ToFloat(SDPoint_Y(p) - SDPoint_Y(end));

		/* calculate and return the distance (squared) */
		return SDMath_DotProduct(peX, peY, peX, peY);
	}

	/* handle non-degenerate case (distance to line segment) */
	{
		/* calculate the component vectors */
		const SDDouble piX = psX - (u * esX);
		const SDDouble piY = psY - (u * esY);

		/* calculate and return the distance (squared) */
		return SDMath_DotProduct(piX, piY, piX, piY);
	}
}

static SDMATH SDDouble
SDBezierF_DistanceToLine(SDPointF p,
                         SDPointF start,
                         SDPointF end)
{
	/* declarations */
	SDDouble u;

	/*\
	|*|
	|*| L - the line
	|*| S - the start of the line
	|*| E - the end of the line
	|*| P - the point whose distance from the line is to be calculated
	|*| I - intersection of P and L
	|*| m - magnitude squared
	|*|
	|*| . - dot product operator
	|*| x - x accessor
	|*| y - y accessor
	|*|
	|*|   PS = (P - S)
	|*|   ES = (E - S)
	|*|
	|*|   L  = S + (u * ES)
	|*|   I  = (P - L) . (ES)
	|*|      = (PS - (u * ES)) . (ES)
	|*|   ---------------------------------------------
	|*|   m  = ((x(ES) * x(ES)) + (y(ES) * y(ES)))
	|*|   u  = ((x(PS) * x(ES)) + (y(PS) * y(ES))) / m
	|*|
	\*/

	/* calculate the component vectors */
	const SDDouble esX = (SDPoint_X(end) - SDPoint_X(start));
	const SDDouble esY = (SDPoint_Y(end) - SDPoint_Y(start));
	const SDDouble psX = (SDPoint_X(p)   - SDPoint_X(start));
	const SDDouble psY = (SDPoint_Y(p)   - SDPoint_Y(start));

	/* handle degenerate case (distance to start point) */
	if(esX == 0.0f && esY == 0.0f)
	{
		/* calculate and return the distance (squared) */
		return SDMath_DotProduct(psX, psY, psX, psY);
	}

	/* calculate the line coefficient */
	u = (SDMath_DotProduct(psX, psY, esX, esY) /
	     SDMath_DotProduct(esX, esY, esX, esY));

	/* handle degenerate case (distance to start point) */
	if(u <= 0.0f)
	{
		/* calculate and return the distance (squared) */
		return SDMath_DotProduct(psX, psY, psX, psY);
	}

	/* handle degenerate case (distance to end point) */
	if(u >= 1.0f)
	{
		/* calculate the component vectors */
		const SDDouble peX = (SDPoint_X(p) - SDPoint_X(end));
		const SDDouble peY = (SDPoint_Y(p) - SDPoint_Y(end));

		/* calculate and return the distance (squared) */
		return SDMath_DotProduct(peX, peY, peX, peY);
	}

	/* handle non-degenerate case (distance to line segment) */
	{
		/* calculate the component vectors */
		const SDDouble piX = psX - (u * esX);
		const SDDouble piY = psY - (u * esY);

		/* calculate and return the distance (squared) */
		return SDMath_DotProduct(piX, piY, piX, piY);
	}
}

static SDBool
SDBezierX_IsInTolerance(SDBezierX *_this,
                        SDDouble   tSquared)
{
	/* assertions */
	SDASSERT((_this != 0));

	/* perform a tolerance check on the first control point */
	if(SDBezierX_DistanceToLine(_this->b, _this->a, _this->d) >= tSquared)
	{
		/* return the 'is not inside tolerance' flag */
		return 0;
	}

	/* perform a tolerance check on the second control point */
	if(SDBezierX_DistanceToLine(_this->c, _this->a, _this->d) >= tSquared)
	{
		/* return the 'is not inside tolerance' flag */
		return 0;
	}

	/* return the 'is inside tolerance' flag */
	return 1;
}

static SDBool
SDBezierF_IsInTolerance(SDBezierF *_this,
                        SDDouble   tSquared)
{
	/* assertions */
	SDASSERT((_this != 0));

	/* perform a tolerance check on the first control point */
	if(SDBezierF_DistanceToLine(_this->b, _this->a, _this->d) >= tSquared)
	{
		/* return the 'is not inside tolerance' flag */
		return 0;
	}

	/* perform a tolerance check on the second control point */
	if(SDBezierF_DistanceToLine(_this->c, _this->a, _this->d) >= tSquared)
	{
		/* return the 'is not inside tolerance' flag */
		return 0;
	}

	/* return the 'is inside tolerance' flag */
	return 1;
}

static SDStatus
SDBezierX_FlattenR(SDBezierX     *_this,
                   SDPointArrayX *array,
                   SDDouble       tSquared)
{
	/* declarations */
	SDBezierX w;
	SDBezierX z;

	/* assertions */
	SDASSERT((_this != 0));
	SDASSERT((array != 0));

	/* handle the base case */
	if(SDBezierX_IsInTolerance(_this, tSquared))
	{
		return SDPointArrayX_AppendPoint(array, &(_this->a));
	}

	/* split the curve */
	SDBezierX_DeCasteljau(_this, &w, &z);

	/* recurse on the first half of the curve */
	SDStatus_Check
		(SDBezierX_FlattenR
			(&w, array, tSquared));

	/* recurse on the second half of the curve */
	SDStatus_Check
		(SDBezierX_FlattenR
			(&z, array, tSquared));

	/* return successfully */
	return SDStatus_OK;
}

static SDStatus
SDBezierF_FlattenR(SDBezierF     *_this,
                   SDPointArrayF *array,
                   SDDouble       tSquared)
{
	/* declarations */
	SDBezierF w;
	SDBezierF z;

	/* assertions */
	SDASSERT((_this != 0));
	SDASSERT((array != 0));

	/* handle the base case */
	if(SDBezierF_IsInTolerance(_this, tSquared))
	{
		return SDPointArrayF_AppendPoint(array, &(_this->a));
	}

	/* split the curve */
	SDBezierF_DeCasteljau(_this, &w, &z);

	/* recurse on the first half of the curve */
	SDStatus_Check
		(SDBezierF_FlattenR
			(&w, array, tSquared));

	/* recurse on the second half of the curve */
	SDStatus_Check
		(SDBezierF_FlattenR
			(&z, array, tSquared));

	/* return successfully */
	return SDStatus_OK;
}

SDINTERNAL SDStatus
SDBezierX_Flatten(SDBezierX     *_this,
                  SDPointArrayX *array,
                  SDDouble       tolerance)
{
	/* assertions */
	SDASSERT((_this != 0));
	SDASSERT((array != 0));

	/* calculate the tolerance squared */
	tolerance = (tolerance * tolerance);

	/* ensure the tolerance is within bounds */
	if(tolerance < 0.000001f) { tolerance = 0.000001f; }

	/* recursively flatten the curve */
	SDStatus_Check
		(SDBezierX_FlattenR
			(_this, array, tolerance));

	/* add the final point */
	SDStatus_Check
		(SDPointArrayX_AppendPoint
			(array, &(_this->d)));

	/* return successfully */
	return SDStatus_OK;
}

SDINTERNAL SDStatus
SDBezierF_Flatten(SDBezierF     *_this,
                  SDPointArrayF *array,
                  SDDouble       tolerance)
{
	/* assertions */
	SDASSERT((_this != 0));
	SDASSERT((array != 0));

	/* calculate the tolerance squared */
	tolerance = (tolerance * tolerance);

	/* ensure the tolerance is within bounds */
	if(tolerance < 0.000001f) { tolerance = 0.000001f; }

	/* recursively flatten the curve */
	SDStatus_Check
		(SDBezierF_FlattenR
			(_this, array, tolerance));

	/* add the final point */
	SDStatus_Check
		(SDPointArrayF_AppendPoint
			(array, &(_this->d)));

	/* return successfully */
	return SDStatus_OK;
}

#ifdef __cplusplus
};
#endif
