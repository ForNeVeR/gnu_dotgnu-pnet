/*
 * SDPath.h - Path header.
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

#ifndef _SD_PATH_H_
#define _SD_PATH_H_

#include "CrayonsInternal.h"

#ifdef __cplusplus
extern "C" {
#endif

struct _tagSDPath
{
	SDUInt32    capacity;
	SDUInt32    count;
	SDPointF   *points;
	SDUInt8    *types;
	SDBitField  winding   : 1;
	SDBitField  newFigure : 1;
	SDBitField  hasCurves : 1;
};

#if 0
struct _tagSDPathIterator
{
	SDPath   *path;
	/* TODO */
};
#endif

/*\
|*| Ensure the capacity of point and type lists of a path.
|*|
|*|   path  - the path
|*|   count - the total minimum capacity required
|*|
|*|  Returns status code on error.
\*/
#define _EnsurePathCapacity(path, minimum)                                     \
	do {                                                                       \
		/* reallocate the lists, as needed */                                  \
		if((minimum) > (path)->capacity)                                       \
		{                                                                      \
			/* declarations */                                                 \
			SDPointF *tmpP;                                                    \
			SDByte   *tmpT;                                                    \
			SDUInt32  newSize;                                                 \
			SDUInt32  newCapacity;                                             \
			                                                                   \
			/* get the capacity */                                             \
			const SDUInt32 capacity = (path)->capacity;                        \
			                                                                   \
			/* calculate the new capacity */                                   \
			newCapacity = (((capacity + (minimum)) + 31) & ~31);               \
			                                                                   \
			/* calculate the optimal capacity, as needed */                    \
			if(capacity != 0)                                                  \
			{                                                                  \
				/* calculate a new capacity candidate */                       \
				const SDUInt32 newCapacity2 = (capacity << 1);                 \
				                                                               \
				/* use the larger candidate capacity */                        \
				if(newCapacity < newCapacity2)                                 \
				{                                                              \
					newCapacity = newCapacity2;                                \
				}                                                              \
			}                                                                  \
			                                                                   \
			/* calculate the new points size */                                \
			newSize = (newCapacity * sizeof(SDPointF));                        \
			                                                                   \
			/* create the new points list */                                   \
			if(!(tmpP = (SDPointF *)SDMalloc(newSize)))                        \
			{                                                                  \
				return SDStatus_OutOfMemory;                                   \
			}                                                                  \
			                                                                   \
			/* calculate the new types size */                                 \
			newSize = (newCapacity * sizeof(SDByte));                          \
			                                                                   \
			/* create the new types list */                                    \
			if(!(tmpT = (SDByte *)SDMalloc(newSize)))                          \
			{                                                                  \
				SDFree(tmpP);                                                  \
				return SDStatus_OutOfMemory;                                   \
			}                                                                  \
			                                                                   \
			/* copy existing data, as needed */                                \
			if((path)->count != 0)                                             \
			{                                                                  \
				/* copy the points and types */                                \
				SDMemCopy                                                      \
					(tmpP, (path)->points,                                     \
					 ((path)->count * sizeof(SDPointF)));                      \
				SDMemCopy                                                      \
					(tmpT, (path)->types,                                      \
					 ((path)->count  * sizeof(SDByte)));                       \
			}                                                                  \
			                                                                   \
			/* free existing lists, as needed */                               \
			if(capacity != 0)                                                  \
			{                                                                  \
				/* free the point and type lists */                            \
				SDFree((path)->points);                                        \
				SDFree((path)->types);                                         \
			}                                                                  \
			                                                                   \
			/* update the capacity */                                          \
			(path)->capacity = newCapacity;                                    \
			                                                                   \
			/* set the point and type lists */                                 \
			(path)->points = tmpP;                                             \
			(path)->types  = tmpT;                                             \
		}                                                                      \
	} while(0)

SDINTERNAL void
SDPath_TransformAffine(SDPath             *_this,
                       SDAffineTransformF *transform);
SDINTERNAL SDStatus
SDPath_Stroke(SDPath    *_this,
              SDPath    *stroke,
              SDStroker *stroker);
SDINTERNAL SDStatus
SDPath_Fill(SDPath       *_this,
            SDTrapezoids *trapezoids);

#ifdef __cplusplus
};
#endif

#endif /* _SD_PATH_H_ */
