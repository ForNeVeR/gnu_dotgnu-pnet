/*
 * SDStroker.h - Stroker header.
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

#ifndef _SD_STROKER_H_
#define _SD_STROKER_H_

#include "CrayonsInternal.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _tagSDStrokeJoiner SDStrokeJoiner;
typedef struct _tagSDStrokeCapper SDStrokeCapper;

struct _tagSDStrokeJoiner
{
	SDLineJoin type;
	union
	{
		struct
		{
			SDPointF *points;
			SDUInt32  count;
			SDUInt32  size;
		} round;
		struct
		{
			SDAffineTransformF  *transform;
			SDVectorF            scale;
			SDDouble             limitSquared;
		} other;
	} u;

	SDStatus (*Join)(SDStrokeJoiner *_this,
	                 SDPath         *path,
	                 SDFloat         centerX,
	                 SDFloat         centerY,
	                 SDFloat         prevC,
	                 SDFloat         prevS,
	                 SDFloat         currC,
	                 SDFloat         currS);
};

struct _tagSDStrokeCapper
{
	SDLineCap type;
	union
	{
		struct
		{
			SDPointF *points;
			SDUInt32  count;
			SDUInt32  size;
		} round;
		struct
		{
			SDFloat radius;
			union
			{
				SDAffineTransformF  *transform;
				SDVectorF           *scale;
			} u;
		} other;
	} u;

	SDStatus (*Cap)(SDStrokeCapper *_this,
	                SDPath         *path,
	                SDFloat        *centerX,
	                SDFloat        *centerY,
	                SDFloat         slopeX,
	                SDFloat         slopeY);
};

struct _tagSDStroker
{
	SDPointArrayF       array;
	SDStrokeCapper      startCapper;
	SDStrokeCapper      endCapper;
	SDStrokeJoiner      joiner;
	SDAffineTransformF  dev;
	SDAffineTransformF  pen;
	SDVectorF           scale;
	SDFloat             radius;

	SDStatus (*Stroke)(SDStroker *_this,
	                   SDPath    *path,
	                   SDPointF  *points,
	                   SDUInt32   count);
};

SDINTERNAL SDStatus
SDStroker_Initialize(SDStroker          *_this,
                     SDPen              *pen,
                     SDAffineTransformF *deviceTransform);
SDINTERNAL void
SDStroker_Finalize(SDStroker *_this);
SDINTERNAL SDStatus
SDStroker_Stroke(SDStroker *_this,
                 SDPath    *path,
                 SDPointF  *points,
                 SDByte    *types,
                 SDUInt32   count);

#ifdef __cplusplus
};
#endif

#endif /* _SD_STROKER_H_ */
