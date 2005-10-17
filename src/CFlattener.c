/*
 * SDFlattener.c - Flattener implementation.
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

#include "CFlattener.h"
#include "CPath.h"

#ifdef __cplusplus
extern "C" {
#endif

static SDStatus
SDFlattener_EnsureCapacity(SDFlattener *_this,
                           SDUInt32     minimum)
{
	/* assertions */
	SDASSERT((_this != 0));

	/* ensure capacity */
	_EnsurePathCapacity(_this, minimum);

	/* return successfully */
	return SDStatus_OK;
}

SDINTERNAL void
SDFlattener_Initialize(SDFlattener *_this)
{
	/* assertions */
	SDASSERT((_this != 0));

	/* initialize the point array */
	SDPointArrayF_Initialize(&(_this->array));

	/* initialize the remaining members */
	_this->points    = 0;
	_this->types     = 0;
	_this->count     = 0;
	_this->capacity  = 0;
}

SDINTERNAL void
SDFlattener_Finalize(SDFlattener *_this,
                     SDPointF    **points,
                     SDByte      **types,
                     SDUInt32     *count,
                     SDUInt32     *capacity)
{
	/* assertions */
	SDASSERT((_this  != 0));

	/* finalize the point array */
	SDPointArrayF_Finalize(&(_this->array));

	/* get or finalize the path information */
	if(points != 0)
	{
		/* assertions */
		SDASSERT((types    != 0));
		SDASSERT((count    != 0));
		SDASSERT((capacity != 0));

		/* get the path information */
		*points   = _this->points;
		*types    = _this->types;
		*count    = _this->count;
		*capacity = _this->capacity;
	}
	else
	{
		/* assertions */
		SDASSERT((types    == 0));
		SDASSERT((count    == 0));
		SDASSERT((capacity == 0));

		/* finalize the path information */
		SDFree(_this->points);
		SDFree(_this->types);
	}

	/* reset the path information */
	_this->points   = 0;
	_this->types    = 0;
	_this->count    = 0;
	_this->capacity = 0;
}

SDINTERNAL SDStatus
SDFlattener_Flatten(SDFlattener *_this,
                    SDPointF    *points,
                    SDByte      *types,
                    SDUInt32     count,
                    SDFloat      tolerance)
{
	/* declarations */
	SDByte    *srcT;
	SDByte    *dstT;
	SDPointF  *srcP;
	SDPointF  *dstP;
	SDPointF  *end;
	SDUInt32   srcN;
	SDUInt32   dstN;

	/* assertions */
	SDASSERT((_this  != 0));
	SDASSERT((points != 0));
	SDASSERT((types  != 0));

	/* get the type input pointer */
	srcT = types;

	/* get the point input pointer */
	srcP = points;

	/* get the end of input pointer */
	end  = (srcP + count);

	/* set the counts to the default */
	srcN = 0;
	dstN = 0;

	/*\
	|*| NOTE: to avoid excessive method calls, we ensure the capacity
	|*|       assuming all lines, then ensure again only when we hit
	|*|       a curve, so at any point we can add a line cheaply
	\*/

	/* ensure capacity */
	SDStatus_Check(SDFlattener_EnsureCapacity(_this, count));

	/* get the type output pointer */
	dstT = types;

	/* get the point output pointer */
	dstP = points;

	/* flatten the path */
	while(srcP != end)
	{
		/* process point based on type */
		if((*srcT & SDPathType_TypeMask) != SDPathType_Bezier)
		{
			/* line to point */
			*dstP = *srcP;
			*dstT = *srcT;

			/* move to the next destination position */
			++dstP; ++dstT; ++dstN;
		}
		else
		{
			/* declarations */
			SDByte         type;
			SDPointArrayF *array;
			SDPointF      *a;
			SDPointF      *b;
			SDPointF      *c;
			SDPointF      *d;
			SDBezierF      bezier;

			/* assertions */
			SDASSERT((srcP       != points));
			SDASSERT(((srcP + 2) != end));

			/* get the current point */
			a = (srcP - 1);

			/* save the type */
			type = *srcT;

			/* get the first point */
			b = srcP;

			/* advance to the second point */
			++srcP; ++srcT; ++srcN;

			/* assertions */
			SDASSERT(((*srcT & SDPathType_TypeMask) == SDPathType_Bezier));

			/* get the second point */
			c = srcP;

			/* advance to the third point */
			++srcP; ++srcT; ++srcN;

			/* assertions */
			SDASSERT(((*srcT & SDPathType_TypeMask) == SDPathType_Bezier));

			/* get the third point */
			d = srcP;

			/* initialize the bezier and handle degenerates */
			if(SDBezierF_Initialize(&bezier, a, b, c, d))
			{
				/* move to the next source position */
				++srcP; ++srcT; ++srcN;

				/* TODO: copy d's extra type information into a? */

				/* handle next point */
				continue;
			}

			/* get the point array */
			array = &(_this->array);

			/* reset the count of the point array */
			SDPointArray_Count(*array) = 0;

			/* flatten the bezier curve */
			SDStatus_Check
				(SDBezierF_Flatten
					(&bezier, array, tolerance));

			/* add the lines */
			{
				/* declarations */
				SDPointF *arrP;
				SDPointF *arrE;
				SDUInt32  arrN;

				/* get the count */
				arrN = SDPointArray_Count(*array);

				/* set the count */
				_this->count = dstN;

				/* ensure capacity */
				SDStatus_Check
					(SDFlattener_EnsureCapacity
						(_this, (((dstN + arrN) - 1) + (count - srcN))));

				/* get the destination type pointer */
				dstT = (_this->types + dstN);

				/* get the destination point pointer */
				dstP = (_this->points + dstN);

				/* update the destination count */
				dstN += (arrN - 1);

				/* get the array point pointer */
				arrP = SDPointArray_Points(*array);

				/* get the end pointer */
				arrE = (arrP + arrN);

				/* skip the current point */
				++arrP;

				/* add the lines */
				while(arrP != arrE)
				{
					/* set the point */
					*dstP++ = *arrP++;

					/* set the type */
					*dstT++ = SDPathType_Line;
				}

				/* add the extra type information to the last point */
				*(dstT - 1) |= (type & ~SDPathType_TypeMask);
			}
		}

		/* move to the next source position */
		++srcP; ++srcT; ++srcN;
	}

	/* return successfully */
	return SDStatus_OK;
}


#ifdef __cplusplus
};
#endif
