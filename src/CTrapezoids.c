/*
 * SDTrapezoids.c - Trapezoids implementation.
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

#include "CTrapezoids.h"
#include "CFiller.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef SDInt64 SDFixedL;
typedef SDInt64 SDFixedL48;

typedef struct _tagSDIntersectionInfo SDIntersectionInfo;
struct _tagSDIntersectionInfo
{
	SDFixed intersection;
	SDBool  ok;
};

/* Initialize these trapezoids. */
SDINTERNAL void
SDTrapezoids_Initialize(SDTrapezoids *_this)
{
	/* assertions */
	SDASSERT((_this != 0));

	/* initialize the trapezoids */
	_this->count      = 0;
	_this->capacity   = 0;
	_this->trapezoids = 0;
}

/* Finalize these trapezoids. */
SDINTERNAL void
SDTrapezoids_Finalize(SDTrapezoids *_this)
{
	/* assertions */
	SDASSERT((_this != 0));

	/* finalize the trapezoids */
	{
		/* get the trapezoid list */
		SDTrapezoidX *trapezoids = _this->trapezoids;

		/* finalize, as needed */
		if(trapezoids)
		{
			/* reset the members */
			_this->count      = 0;
			_this->capacity   = 0;
			_this->trapezoids = 0;

			/* free the trapezoid list */
			SDFree(trapezoids);
		}
	}
}

/* Reset these trapezoids. */
SDINTERNAL void
SDTrapezoids_Reset(SDTrapezoids *_this)
{
	/* assertions */
	SDASSERT((_this != 0));

	/* reset the trapezoids */
	_this->count = 0;
}

/* Add a trapezoid. */
static SDStatus
SDTrapezoids_AddTrapezoid(SDTrapezoids  *_this,
                          SDFixed        top,
                          SDFixed        bottom,
                          const SDLineX *left,
                          const SDLineX *right)
{
	/* declarations */
	SDUInt32 count;

	/* assertions */
	SDASSERT((_this != 0));
	SDASSERT((left  != 0));
	SDASSERT((right != 0));

	/* bail out now if the trapezoid is degenerate */
	SDStatus_Require((top != bottom), SDStatus_OK);

	/* get the trapezoid count */
	count = _this->count;

	/* ensure the capacity of trapezoid list */
	if(count >= _this->capacity)
	{
		/* declarations */
		SDTrapezoidX *tmp;
		SDUInt32  newSize;
		SDUInt32  newCapacity;

		/* get the capacity */
		const SDUInt32 capacity = _this->capacity;

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
		newSize = (newCapacity * sizeof(SDTrapezoidX));

		/* create the new trapezoid list */
		if(!(tmp = (SDTrapezoidX *)SDRealloc(_this->trapezoids, newSize)))
		{
			return SDStatus_OutOfMemory;
		}

		/* update the capacity */
		_this->capacity = newCapacity;

		/* set the trapezoid list */
		_this->trapezoids = tmp;
	}

	/* add the new trapezoid */
	{
		SDTrapezoidX *trapezoid        = (_this->trapezoids + count);
		SDTrapezoid_Top(*trapezoid)    =  top;
		SDTrapezoid_Bottom(*trapezoid) =  bottom;
		SDTrapezoid_Left(*trapezoid)   = *left;
		SDTrapezoid_Right(*trapezoid)  = *right;
	}

	/* update the trapezoid count */
	_this->count = (count + 1);

	/* return successfully */
	return SDStatus_OK;
}

/* Compare the tops of two edges. */
static int
SDEdgeX_CompareTop(const void *_a,
                   const void *_b)
{
	/* get the edges */
	const SDEdgeX *a = (const SDEdgeX *)_a;
	const SDEdgeX *b = (const SDEdgeX *)_b;

	/* return comparison */
	return (SDEdge_Y1(*a) - SDEdge_Y1(*b));
}

/* Compare the current x positions of two edges. */
static int
SDEdgeX_CompareCurrentX(const void *_a,
                        const void *_b)
{
	/* declarations */
	int cmp;

	/* get the edges */
	const SDEdgeX *a = _a;
	const SDEdgeX *b = _b;

	/* compare on current x position */
	cmp = (int)(SDEdge_CurrentX(*a) - SDEdge_CurrentX(*b));

	/* compare on slope, if current x positions are equal */
	if(cmp == 0)
	{
		/* calculate the vectors */
		const SDFixedL48 aVectorX = (SDEdge_X2(*a) - SDEdge_X1(*a));
		const SDFixedL48 aVectorY = (SDEdge_Y2(*a) - SDEdge_Y1(*a));
		const SDFixedL48 bVectorX = (SDEdge_X2(*b) - SDEdge_X1(*b));
		const SDFixedL48 bVectorY = (SDEdge_Y2(*b) - SDEdge_Y1(*b));

		/* return the slope comparison (positive is clockwise) */
		return (int)SDMath_CrossProduct(bVectorX, bVectorY, aVectorX, aVectorY);
	}

	/* return comparison */
	return cmp;
}

/* Calculate the intersection of the given line segments. */
static SDMATH SDIntersectionInfo
SDLineX_CalculateIntersection(SDLineX a, SDLineX b)
{
	/* declarations */
	SDIntersectionInfo info;

	/* calculate the inverse slopes */
	const SDDouble sA =
		(SDFixed_ToDouble(SDLine_X2(a) - SDLine_X1(a)) /
		 SDFixed_ToDouble(SDLine_Y2(a) - SDLine_Y1(a)));
	const SDDouble sB =
		(SDFixed_ToDouble(SDLine_X2(b) - SDLine_X1(b)) /
		 SDFixed_ToDouble(SDLine_Y2(b) - SDLine_Y1(b)));

	/* calculate the intersection, if possible */
	if(sA == sB)
	{
		info.ok = 0;
	}
	else
	{
		/* calculate the intercepts */
		const SDDouble iA =
			(SDFixed_ToDouble(SDLine_X1(a)) -
			 (sA * SDFixed_ToDouble(SDLine_Y1(a))));
		const SDDouble iB =
			(SDFixed_ToDouble(SDLine_X1(b)) -
			 (sB * SDFixed_ToDouble(SDLine_Y1(b))));

		/* calculate the intersection */
		info.intersection = SDDouble_ToFixed((iB - iA) / (sA - sB));

		/* flag that we have an intersection */
		info.ok = 1;
	}

	/* return the intersection information */
	return info;
}

/* Calculate the x position of the line at the given y position. */
static SDMATH SDFixed
SDLineX_CalculateCurrentX(SDLineX line, SDFixed y)
{
	/* declarations */
	SDFixed dx;

	/* calculate the x vector */
	const SDFixed vectorX = (SDLine_X2(line) - SDLine_X1(line));

	/* calculate the y vector */
	const SDFixed vectorY = (SDLine_Y2(line) - SDLine_Y1(line));

	/* calculate the y delta */
	y -= SDLine_Y1(line);

	/* calculate the x delta */
	dx = (((SDFixedL48)vectorX * (SDFixedL48)(y)) / vectorY);

	/* calculate and return the current x position */
	return (SDLine_X1(line) + dx);
}

/* Tessellate the polygon into trapezoids (corrupts polygon). */
SDINTERNAL SDStatus
SDTrapezoids_TessellatePolygon(SDTrapezoids *_this,
                               SDPolygonX   *polygon,
                               SDFillMode    fillMode)
{
	/* declarations */
	SDFixed   top;
	SDUInt32  edgeCount;
	SDEdgeX  *edges;
	SDEdgeX  *active;
	SDEdgeX  *inactive;
	SDEdgeX  *end;

	/* assertions */
	SDASSERT((_this   != 0));
	SDASSERT((polygon != 0));

	/* get the edge count */
	edgeCount = SDPolygon_EdgeCount(*polygon);

	/* bail out now if there's nothing to do */
	SDStatus_Require((edgeCount != 0), SDStatus_OK);

	/* get the edges */
	edges = SDPolygon_Edges(*polygon);

	/* sort the edges by top */
	qsort(edges, edgeCount, sizeof(SDEdgeX), SDEdgeX_CompareTop);

	/* get the active edge pointer */
	active = edges;

	/* get the inactive edge pointer */
	inactive = (edges + 1);

	/* get the end of input pointer */
	end = (edges + edgeCount);

	/* get the starting y position */
	top = SDEdge_Y1(*edges);

	/* process all the edges */
	while(active != end)
	{
		/* declarations */
		SDEdgeX *curr;
		SDFixed  bottom;

		/* find inactive edges */
		while(inactive != end && SDEdge_Y1(*inactive) <= top) { ++inactive; }

		/* calculate the current x positions of the active edges */
		for(curr = active; curr != inactive; ++curr)
		{
			SDEdge_CurrentX(*curr) =
				SDLineX_CalculateCurrentX
					(SDEdge_Line(*curr), top);
		}

		/* sort the edges by their current x position */
		qsort
			(active, (inactive - active), sizeof(SDEdgeX),
			 SDEdgeX_CompareCurrentX);

		/* set the bottom position to the default */
		bottom = SDEdge_Y2(*active);

		/* search for the bottom position among active edges */
		{
			/* get the last active edge */
			const SDEdgeX *last = (inactive - 1);

			/* check the last active edge for the bottom position */
			if(SDEdge_Y2(*last) < bottom) { bottom = SDEdge_Y2(*last); }

			/* search the active edges */
			for(curr = active; curr != last; ++curr)
			{
				/* get the next active edge */
				const SDEdgeX *next = (curr + 1);

				/* set the bottom position, based on vertex */
				if(SDEdge_Y2(*curr) < bottom) { bottom = SDEdge_Y2(*curr); }

				/* set the bottom position, based on intersection */
				if(SDEdge_CurrentX(*curr) != SDEdge_CurrentX(*next))
				{
					/* calculate the intersection ceiling */
					const SDIntersectionInfo info =
						SDLineX_CalculateIntersection
							(SDEdge_Line(*curr), SDEdge_Line(*next));

					/* set the bottom position, as needed */
					if(info.ok &&
					   info.intersection > top &&
					   info.intersection < bottom)
					{
						bottom = info.intersection;
					}
				}
			}
		}

		/* set the bottom position, based on next inactive vertex */
		if(inactive != end && SDEdge_Y1(*inactive) < bottom)
		{
			bottom = SDEdge_Y1(*inactive);
		}

		/* generate trapezoids from the active edges */
		if(fillMode == SDFillMode_Alternate)
		{
			/* declarations */
			SDInt32 inside;

			/* get the last active edge */
			const SDEdgeX *last = (inactive - 1);

			/* set the insideness to the default */
			inside = 0;

			/* generate trapezoids */
			for(curr = active; curr != last; ++curr)
			{
				/* get the next active edge */
				const SDEdgeX *next = (curr + 1);

				/* calculate the insideness of the current edge */
				inside = ((inside + 1) & 1);

				/* add the trapezoid, if it's inside */
				if(inside != 0)
				{
					/* add the trapezoid */
					SDStatus_Check
						(SDTrapezoids_AddTrapezoid
							(_this,
							 top, bottom,
							 &SDEdge_Line(*curr),
							 &SDEdge_Line(*next)));
				}
			}
		}
		else
		{
			/* declarations */
			SDInt32 inside;

			/* get the last active edge */
			const SDEdgeX *last = (inactive - 1);

			/* set the insideness to the default */
			inside = 0;

			/* generate trapezoids */
			for(curr = active; curr != last; ++curr)
			{
				/* get the next active edge */
				const SDEdgeX *next = (curr + 1);

				/* calculate the insideness of the current edge */
				inside = (inside + (SDEdge_Clockwise(*curr) ? 1 : -1));

				/* add the trapezoid, if it's inside */
				if(inside != 0)
				{
					/* add the trapezoid */
					SDStatus_Check
						(SDTrapezoids_AddTrapezoid
							(_this,
							 top, bottom,
							 &SDEdge_Line(*curr),
							 &SDEdge_Line(*next)));
				}
			}
		}

		/* remove passed edges */
		for(curr = active; curr != inactive; ++curr)
		{
			/* remove the current edge, as needed */
			if(SDEdge_Y2(*curr) <= bottom)
			{
				/* calculate the number of bytes to move */
				const SDUInt32 n = ((curr - active) * sizeof(SDEdgeX));

				/* remove the current edge from the edge list */
				SDMemMove((active + 1), active, n);

				/* update active edge pointer */
				++active;
			}
		}

		/* move to next y position */
		top = bottom;
	}

	/* return successfully */
	return SDStatus_OK;
}

/* Fill the path to these trapezoids. */
SDINTERNAL SDStatus
SDTrapezoids_Fill(SDTrapezoids *_this,
                  SDPointF     *points,
                  SDByte       *types,
                  SDUInt32      count,
                  SDFillMode    fillMode)
{
	/* declarations */
	SDStatus status;
	SDFiller filler;

	/* bail out now if there's nothing to do */
	SDStatus_Require((count != 0), SDStatus_OK);

	/* assertions */
	SDASSERT((_this  != 0));
	SDASSERT((points != 0));
	SDASSERT((types  != 0));

	/* initialize the filler */
	SDFiller_Initialize(&filler);

	/* fill to the trapezoids */
	status =
		SDFiller_ToTrapezoids
			(&filler, _this, points, types, count, fillMode);

	/* finalize the filler */
	SDFiller_Finalize(&filler);

	/* return status */
	return status;
}


#ifdef __cplusplus
};
#endif
