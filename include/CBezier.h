/*
 * SDBezier.h - Bezier header.
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

#ifndef _SD_BEZIER_H_
#define _SD_BEZIER_H_

#include "CPointArray.h"

#ifdef __cplusplus
extern "C" {
#endif

static const SDBezierX SDBezierX_Zero =
{
	{ SDFixed_Zero, SDFixed_Zero },
	{ SDFixed_Zero, SDFixed_Zero },
	{ SDFixed_Zero, SDFixed_Zero },
	{ SDFixed_Zero, SDFixed_Zero }
};

static const SDBezierF SDBezierF_Zero =
{
	{ 0.0f, 0.0f },
	{ 0.0f, 0.0f },
	{ 0.0f, 0.0f },
	{ 0.0f, 0.0f }
};

SDINTERNAL SDBool
SDBezierX_Initialize(SDBezierX *_this,
                     SDPointX  *a,
                     SDPointX  *b,
                     SDPointX  *c,
                     SDPointX  *d);
SDINTERNAL SDBool
SDBezierF_Initialize(SDBezierF *_this,
                     SDPointF  *a,
                     SDPointF  *b,
                     SDPointF  *c,
                     SDPointF  *d);
SDINTERNAL void
SDBezierX_Finalize(SDBezierX *_this);
SDINTERNAL void
SDBezierF_Finalize(SDBezierF *_this);

SDINTERNAL SDStatus
SDBezierX_Flatten(SDBezierX     *_this,
                  SDPointArrayX *array,
                  SDDouble       tolerance);
SDINTERNAL SDStatus
SDBezierF_Flatten(SDBezierF     *_this,
                  SDPointArrayF *array,
                  SDDouble       tolerance);

#ifdef __cplusplus
};
#endif

#endif /* _SD_BEZIER_H_ */
