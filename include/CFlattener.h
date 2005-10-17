/*
 * SDFlattener.h - Flattener header.
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

#ifndef _SD_FLATTENER_H_
#define _SD_FLATTENER_H_

#include "CBezier.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _tagSDFlattener SDFlattener;
struct _tagSDFlattener
{
	SDPointArrayF  array;
	SDPointF      *points;
	SDByte        *types;
	SDUInt32       count;
	SDUInt32       capacity;
};

SDINTERNAL void
SDFlattener_Initialize(SDFlattener *_this);
SDINTERNAL void
SDFlattener_Finalize(SDFlattener *_this,
                     SDPointF    **points,
                     SDByte      **types,
                     SDUInt32     *count,
                     SDUInt32     *capacity);
SDINTERNAL SDStatus
SDFlattener_Flatten(SDFlattener *_this,
                    SDPointF    *points,
                    SDByte      *types,
                    SDUInt32     count,
                    SDFloat      tolerance);

#ifdef __cplusplus
};
#endif

#endif /* _SD_FLATTENER_H_ */
