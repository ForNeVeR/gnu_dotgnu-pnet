/*
 * CUtils.h - Utilities header.
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

#ifndef _C_UTILS_H_
#define _C_UTILS_H_

#include "CrayonsInternal.h"

#ifdef __cplusplus
extern "C" {
#endif

CINTERNAL CFloat
CUtils_ConvertUnits(CGraphicsUnit fromUnit,
                    CGraphicsUnit toUnit,
                    CFloat        value);
CINTERNAL CFloat
CUtils_ConvertUnitsDPI(CGraphicsUnit fromUnit,
                       CGraphicsUnit toUnit,
                       CFloat        value,
                       CFloat        fromDpiY,
                       CFloat        toDpiY);
CINTERNAL CStatus
CUtils_ToPixmanImage(CPixelFormat    format,
                     CByte          *scan0,
                     pixman_image_t *image,
                     CUInt32         x,
                     CUInt32         y,
                     CUInt32         width,
                     CUInt32         height,
                     CUInt32         stride,
                     CColorPalette  *palette);
CINTERNAL CStatus
CUtils_FromPixmanImage(CPixelFormat    format,
                       pixman_image_t *image,
                       CByte          *scan0,
                       CUInt32         x,
                       CUInt32         y,
                       CUInt32         width,
                       CUInt32         height,
                       CUInt32         stride,
                       CColorPalette  *palette);
CINTERNAL CStatus
CUtils_GetPixmanPixelPointer(pixman_image_t  *image,
                             CUInt32          x,
                             CUInt32          y,
                             CColor         **pixel);
CINTERNAL CUInt32
CUtils_FormatToStride(CPixelFormat pixelFormat,
                      CUInt32      width);
CINTERNAL CUInt32
CUtils_BytesPerLine(CPixelFormat pixelFormat,
                    CUInt32      width);
CINTERNAL pixman_transform_t
CUtils_ToPixmanTransform(CAffineTransformF *transform);
CINTERNAL CStatus
CUtils_CreateSolidPattern(pixman_image_t **pattern,
                          CColor           color);
CINTERNAL pixman_color_t
CUtils_ToPixmanColor(CColor color);
CINTERNAL CStatus
CUtils_PixmanImageRectangle(pixman_image_t *src,
                            pixman_image_t *dst,
                            CUInt32         x,
                            CUInt32         y,
                            CUInt32         width,
                            CUInt32         height);
CINTERNAL CBool
CUtils_UseGray(CSmoothingMode   smoothing,
                CPixelOffsetMode pixelOffset);

#ifdef __cplusplus
};
#endif

#endif /* _C_UTILS_H_ */
