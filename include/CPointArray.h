/*
 * SDPointArray.h - Point array header.
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

#ifndef _SD_POINTARRAY_H_
#define _SD_POINTARRAY_H_

#include "CrayonsInternal.h"

#ifdef __cplusplus
extern "C" {
#endif

static const SDPointArrayX SDPointArrayX_Zero;
static const SDPointArrayF SDPointArrayF_Zero;

SDINTERNAL void
SDPointArrayX_Initialize(SDPointArrayX *_this);
SDINTERNAL void
SDPointArrayF_Initialize(SDPointArrayF *_this);
SDINTERNAL void
SDPointArrayX_Finalize(SDPointArrayX *_this);
SDINTERNAL void
SDPointArrayF_Finalize(SDPointArrayF *_this);
SDINTERNAL SDStatus
SDPointArrayX_AppendPointNoRepeat(SDPointArrayX *_this,
                                  SDPointX      *point);
SDINTERNAL SDStatus
SDPointArrayF_AppendPointNoRepeat(SDPointArrayF *_this,
                                  SDPointF      *point);
SDINTERNAL SDStatus
SDPointArrayX_AppendPoint(SDPointArrayX *_this,
                          SDPointX      *point);
SDINTERNAL SDStatus
SDPointArrayF_AppendPoint(SDPointArrayF *_this,
                          SDPointF      *point);
SDINTERNAL SDStatus
SDPointArrayX_EnsureCapacity(SDPointArrayX *_this,
                             SDUInt32       minimum);
SDINTERNAL SDStatus
SDPointArrayF_EnsureCapacity(SDPointArrayF *_this,
                             SDUInt32       minimum);

#ifdef __cplusplus
};
#endif

#endif /* _SD_POINTARRAY_H_ */
