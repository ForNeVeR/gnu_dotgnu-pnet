/*
 * SDPointArray.c - Point array implementation.
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

#include "CPointArray.h"

#ifdef __cplusplus
extern "C" {
#endif

SDINTERNAL void
SDPointArrayX_Initialize(SDPointArrayX *_this)
{
	/* assertions */
	SDASSERT((_this != 0));

	/* initialize the members */
	*_this = SDPointArrayX_Zero;
}

SDINTERNAL void
SDPointArrayF_Initialize(SDPointArrayF *_this)
{
	/* assertions */
	SDASSERT((_this != 0));

	/* initialize the members */
	*_this = SDPointArrayF_Zero;
}

SDINTERNAL void
SDPointArrayX_Finalize(SDPointArrayX *_this)
{
	/* assertions */
	SDASSERT((_this != 0));

	/* finalize the members */
	if(_this->points != 0)
	{
		SDFree(_this->points);
		*_this = SDPointArrayX_Zero;
	}
}

SDINTERNAL void
SDPointArrayF_Finalize(SDPointArrayF *_this)
{
	/* assertions */
	SDASSERT((_this != 0));

	/* finalize the members */
	if(_this->points != 0)
	{
		SDFree(_this->points);
		*_this = SDPointArrayF_Zero;
	}
}

SDINTERNAL SDStatus
SDPointArrayX_AppendPointNoRepeat(SDPointArrayX *_this,
                                  SDPointX      *point)
{
	/* assertions */
	SDASSERT((_this != 0));
	SDASSERT((point != 0));

	/* bail out now if there's nothing to append */
	if(SDPointArray_Count(*_this) != 0)
	{
		/* get the last point */
		const SDPointX last =
			SDPointArray_Point(*_this, SDPointArray_Count(*_this) - 1);

		/* bail out if the last point and the current point match */
		if(SDPoint_X(last) == SDPoint_X(*point) &&
		   SDPoint_Y(last) == SDPoint_Y(*point))
		{
			return SDStatus_OK;
		}
	}

	/* ensure capacity of point list */
	SDStatus_Check
		(SDPointArrayX_EnsureCapacity
			(_this, (SDPointArray_Count(*_this) + 1)));

	/* append the point */
	SDPointArray_Point(*_this, SDPointArray_Count(*_this)++) = *point;

	/* return successfully */
	return SDStatus_OK;
}

SDINTERNAL SDStatus
SDPointArrayF_AppendPointNoRepeat(SDPointArrayF *_this,
                                  SDPointF      *point)
{
	/* assertions */
	SDASSERT((_this != 0));
	SDASSERT((point != 0));

	/* bail out now if there's nothing to append */
	if(SDPointArray_Count(*_this) != 0)
	{
		/* get the last point */
		const SDPointF last =
			SDPointArray_Point(*_this, SDPointArray_Count(*_this) - 1);

		/* bail out if the last point and the current point match */
		if(SDPoint_X(last) == SDPoint_X(*point) &&
		   SDPoint_Y(last) == SDPoint_Y(*point))
		{
			return SDStatus_OK;
		}
	}

	/* ensure capacity of point list */
	SDStatus_Check
		(SDPointArrayF_EnsureCapacity
			(_this, (SDPointArray_Count(*_this) + 1)));

	/* append the point */
	SDPointArray_Point(*_this, SDPointArray_Count(*_this)++) = *point;

	/* return successfully */
	return SDStatus_OK;
}

SDINTERNAL SDStatus
SDPointArrayX_AppendPoint(SDPointArrayX *_this,
                          SDPointX      *point)
{
	/* assertions */
	SDASSERT((_this != 0));
	SDASSERT((point != 0));

	/* ensure capacity of point list */
	SDStatus_Check
		(SDPointArrayX_EnsureCapacity
			(_this, (SDPointArray_Count(*_this) + 1)));

	/* append the point */
	SDPointArray_Point(*_this, SDPointArray_Count(*_this)++) = *point;

	/* return successfully */
	return SDStatus_OK;
}

SDINTERNAL SDStatus
SDPointArrayF_AppendPoint(SDPointArrayF *_this,
                          SDPointF      *point)
{
	/* assertions */
	SDASSERT((_this != 0));
	SDASSERT((point != 0));

	/* ensure capacity of point list */
	SDStatus_Check
		(SDPointArrayF_EnsureCapacity
			(_this, (SDPointArray_Count(*_this) + 1)));

	/* append the point */
	SDPointArray_Point(*_this, SDPointArray_Count(*_this)++) = *point;

	/* return successfully */
	return SDStatus_OK;
}

SDINTERNAL SDStatus
SDPointArrayX_EnsureCapacity(SDPointArrayX *_this,
                             SDUInt32       minimum)
{
	/* assertions */
	SDASSERT((_this != 0));

	/* bail out now if there's nothing to do */
	SDStatus_Require((minimum != 0), SDStatus_OK);

	/* reallocate the list, as needed */
	if(minimum > _this->capacity)
	{
		/* declarations */
		SDPointX *tmp;
		SDUInt32  capacity;

		/* calculate the new capacity */
		capacity = (_this->capacity << 1);

		/* calculate the optimal capacity, as needed */
		if(capacity < minimum || capacity == 0)
		{
			/* calculate the new capacity */
			capacity = (((_this->capacity + minimum) + 31) & ~31);
		}

		/* create the new point list */
		if(!(tmp = (SDPointX *)SDMalloc(capacity * sizeof(SDPointX))))
		{
			return SDStatus_OutOfMemory;
		}

		/* copy existing data, as needed */
		if(_this->count != 0)
		{
			/* copy the points */
			SDMemCopy(tmp, _this->points, (_this->count * sizeof(SDPointX)));
		}

		/* free existing list, as needed */
		if(_this->capacity != 0)
		{
			/* free the point list */
			SDFree(_this->points);
		}

		/* update the capacity */
		_this->capacity = capacity;

		/* set the point list */
		_this->points = tmp;
	}

	/* return successfully */
	return SDStatus_OK;
}

SDINTERNAL SDStatus
SDPointArrayF_EnsureCapacity(SDPointArrayF *_this,
                             SDUInt32       minimum)
{
	/* assertions */
	SDASSERT((_this != 0));

	/* bail out now if there's nothing to do */
	SDStatus_Require((minimum != 0), SDStatus_OK);

	/* reallocate the list, as needed */
	if(minimum > _this->capacity)
	{
		/* declarations */
		SDPointF *tmp;
		SDUInt32  capacity;

		/* calculate the new capacity */
		capacity = (_this->capacity << 1);

		/* calculate the optimal capacity, as needed */
		if(capacity < minimum || capacity == 0)
		{
			/* calculate the new capacity */
			capacity = (((_this->capacity + minimum) + 31) & ~31);
		}

		/* create the new point list */
		if(!(tmp = (SDPointF *)SDMalloc(capacity * sizeof(SDPointF))))
		{
			return SDStatus_OutOfMemory;
		}

		/* copy existing data, as needed */
		if(_this->count != 0)
		{
			/* copy the points */
			SDMemCopy(tmp, _this->points, (_this->count * sizeof(SDPointF)));
		}

		/* free existing list, as needed */
		if(_this->capacity != 0)
		{
			/* free the point list */
			SDFree(_this->points);
		}

		/* update the capacity */
		_this->capacity = capacity;

		/* set the point list */
		_this->points = tmp;
	}

	/* return successfully */
	return SDStatus_OK;
}


#ifdef __cplusplus
};
#endif
