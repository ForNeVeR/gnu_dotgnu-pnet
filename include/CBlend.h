/*
 * SDBlend.h - Gradient blending header.
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

#ifndef _SD_BLEND_H_
#define _SD_BLEND_H_

#include "CBrush.h"

#ifdef __cplusplus
extern "C" {
#endif

static const SDBlend      SDBlend_Zero      = { 0, 0, 0 };
static const SDColorBlend SDColorBlend_Zero = { 0, 0, 0 };

#define SDBlend_TriangularHalfCount 2
#define SDBlend_TriangularFullCount 3
#define SDBlend_SigmaBellHalfCount  256
#define SDBlend_SigmaBellFullCount  511

SDINTERNAL SDStatus
SDBlend_Initialize(SDBlend  *_this,
                   SDUInt32  count);
SDINTERNAL SDStatus
SDBlend_Copy(SDBlend *_this,
             SDBlend *copy);
SDINTERNAL void
SDBlend_SetTriangularShape(SDBlend *_this,
                           SDFloat  focus,
                           SDFloat  scale);
SDINTERNAL void
SDBlend_SetSigmaBellShape(SDBlend *_this,
                          SDFloat  focus,
                          SDFloat  scale);
SDINTERNAL void
SDBlend_Finalize(SDBlend *_this);
SDINTERNAL SDStatus
SDColorBlend_Copy(SDColorBlend *_this,
                  SDColorBlend *copy);
SDINTERNAL void
SDColorBlend_Finalize(SDColorBlend *_this);

#ifdef __cplusplus
};
#endif

#endif /* _SD_BLEND_H_ */
