/*
 * SDAffineTransform.h - Affine transformation header.
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

#ifndef _SD_AFFINETRANFSORM_H_
#define _SD_AFFINETRANFSORM_H_

#include "CrayonsInternal.h"

#ifdef __cplusplus
extern "C" {
#endif

static const SDAffineTransformF SDAffineTransformF_Identity =
{
	1.0f, 0.0f,
	0.0f, 1.0f,
	0.0f, 0.0f
};
static const SDVectorF SDVectorF_Zero = { 0.0f, 0.0f };


SDINTERNAL void
SDAffineTransformF_Equals(const SDAffineTransformF *_this,
                          const SDAffineTransformF *other,
                          SDBool                   *eq);
SDINTERNAL void
SDAffineTransformF_NotEquals(const SDAffineTransformF *_this,
                             const SDAffineTransformF *other,
                             SDBool                   *ne);
SDINTERNAL void
SDAffineTransformF_SetIdentity(SDAffineTransformF *_this);
SDINTERNAL SDStatus
SDAffineTransformF_SetParallelogram(SDAffineTransformF *_this,
                                    SDRectangleF        rect,
                                    SDPointF            tl,
                                    SDPointF            tr,
                                    SDPointF            bl);
SDINTERNAL void
SDAffineTransformF_SetElements(SDAffineTransformF *_this,
                               SDFloat             m11,
                               SDFloat             m12,
                               SDFloat             m21,
                               SDFloat             m22,
                               SDFloat             dx,
                               SDFloat             dy);
SDINTERNAL void
SDAffineTransformF_GetDeterminant(const SDAffineTransformF *_this,
                                  SDFloat                  *determinant);
SDINTERNAL SDStatus
SDAffineTransformF_GetInverse(const SDAffineTransformF *_this,
                              SDAffineTransformF       *inverse);
SDINTERNAL void
SDAffineTransformF_Multiply(SDAffineTransformF       *_this,
                            const SDAffineTransformF *other,
                            SDMatrixOrder             order);
SDINTERNAL SDStatus
SDAffineTransformF_MultiplyInverse(SDAffineTransformF       *_this,
                                   const SDAffineTransformF *other,
                                   SDMatrixOrder             order);
SDINTERNAL void
SDAffineTransformF_Rotate(SDAffineTransformF *_this,
                          SDFloat             angle,
                          SDMatrixOrder       order);
SDINTERNAL void
SDAffineTransformF_RotateInverse(SDAffineTransformF *_this,
                                 SDFloat             angle,
                                 SDMatrixOrder       order);
SDINTERNAL void
SDAffineTransformF_Scale(SDAffineTransformF *_this,
                         SDFloat             scaleX,
                         SDFloat             scaleY,
                         SDMatrixOrder       order);
SDINTERNAL void
SDAffineTransformF_ScaleInverse(SDAffineTransformF *_this,
                                SDFloat             scaleX,
                                SDFloat             scaleY,
                                SDMatrixOrder       order);
SDINTERNAL void
SDAffineTransformF_ExtractScale(SDAffineTransformF *_this,
                                SDFloat            *scaleX,
                                SDFloat            *scaleY,
                                SDMatrixOrder       order);
SDINTERNAL void
SDAffineTransformF_Shear(SDAffineTransformF *_this,
                         SDFloat             shearX,
                         SDFloat             shearY,
                         SDMatrixOrder       order);
SDINTERNAL void
SDAffineTransformF_Translate(SDAffineTransformF *_this,
                             SDFloat             offsetX,
                             SDFloat             offsetY,
                             SDMatrixOrder       order);
SDINTERNAL void
SDAffineTransformF_TranslateInverse(SDAffineTransformF *_this,
                                    SDFloat             offsetX,
                                    SDFloat             offsetY,
                                    SDMatrixOrder       order);
SDINTERNAL void
SDAffineTransformF_TransformPoints(const SDAffineTransformF *_this,
                                   SDPointF                 *points,
                                   SDUInt32                  count);
SDINTERNAL void
SDAffineTransformF_TransformVectors(const SDAffineTransformF *_this,
                                    SDPointF                 *points,
                                    SDUInt32                  count);

SDINTERNAL void
SDVectorF_ScalePoints(const SDVectorF *_this,
                      SDPointF        *points,
                      SDUInt32         count);
SDINTERNAL void
SDVectorF_TranslatePoints(const SDVectorF *_this,
                          SDPointF        *points,
                          SDUInt32         count);

#ifdef __cplusplus
};
#endif

#endif /* _SD_AFFINETRANFSORM_H_ */
