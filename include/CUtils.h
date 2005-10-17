/*
 * SDUtils.h - Utilities header.
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

#ifndef _SD_UTILS_H_
#define _SD_UTILS_H_

#include "CrayonsInternal.h"

#ifdef __cplusplus
extern "C" {
#endif

SDINTERNAL SDFloat
SDUtils_ConvertUnits(SDGraphicsUnit fromUnit,
                     SDGraphicsUnit toUnit,
                     SDFloat        value);
SDINTERNAL SDFloat
SDUtils_ConvertUnitsDPI(SDGraphicsUnit fromUnit,
                        SDGraphicsUnit toUnit,
                        SDFloat        value,
                        SDFloat        fromDpiY,
                        SDFloat        toDpiY);
SDINTERNAL SDStatus
SDUtils_ToPixmanImage(SDPixelFormat   format,
                      SDByte         *scan0,
                      pixman_image_t *image,
                      SDUInt32        x,
                      SDUInt32        y,
                      SDUInt32        width,
                      SDUInt32        height,
                      SDUInt32        stride,
                      SDColorPalette *palette);
SDINTERNAL SDStatus
SDUtils_FromPixmanImage(SDPixelFormat   format,
                        pixman_image_t *image,
                        SDByte         *scan0,
                        SDUInt32        x,
                        SDUInt32        y,
                        SDUInt32        width,
                        SDUInt32        height,
                        SDUInt32        stride,
                        SDColorPalette *palette);
SDINTERNAL SDStatus
SDUtils_GetPixmanPixelPointer(pixman_image_t  *image,
                              SDUInt32         x,
                              SDUInt32         y,
                              SDColor        **pixel);
SDINTERNAL SDUInt32
SDUtils_FormatToStride(SDPixelFormat pixelFormat,
                       SDUInt32      width);
SDINTERNAL SDUInt32
SDUtils_BytesPerLine(SDPixelFormat pixelFormat,
                     SDUInt32      width);
SDINTERNAL pixman_transform_t
SDUtils_ToPixmanTransform(SDAffineTransformF *transform);
SDINTERNAL SDStatus
SDUtils_CreateSolidPattern(pixman_image_t **pattern,
                           SDColor          color);
SDINTERNAL pixman_color_t
SDUtils_ToPixmanColor(SDColor color);
SDINTERNAL SDStatus
SDUtils_PixmanImageRectangle(pixman_image_t *src,
                             pixman_image_t *dst,
                             SDUInt32        x,
                             SDUInt32        y,
                             SDUInt32        width,
                             SDUInt32        height);
SDINTERNAL SDBool
SDUtils_UseGray(SDSmoothingMode   smoothing,
                SDPixelOffsetMode pixelOffset);

#ifdef __cplusplus
};
#endif

#endif /* _SD_UTILS_H_ */
