/*
 * SDHatchBrush.h - Hatch brush header.
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

#ifndef _SD_HATCHBRUSH_H_
#define _SD_HATCHBRUSH_H_

#include "CBrush.h"

#ifdef __cplusplus
extern "C" {
#endif

struct _tagSDHatchBrush
{
	SDBrush      _base;
	SDHatchStyle style;
	SDColor      foreground;
	SDColor      background;
};

#define SDHatchStyle_IsValid(style) \
	(((style) >= SDHatchStyle_Min) && ((style) <= SDHatchStyle_Max))

#define SDHatchBrush_DataFormat   PIXMAN_FORMAT_NAME_ARGB32
#define SDHatchBrush_StyleWidth   8
#define SDHatchBrush_StyleHeight  8
#define SDHatchBrush_StylesLength (sizeof(SDHatchBrush_HatchInfo) / 8)

#define SDHatchBrush_StyleToData_SetRow(src, dst, fg, bg)                      \
	do {                                                                       \
		/* get the current source row */                                       \
		const SDByte row = *(src);                                             \
		                                                                       \
		/* set the current data row */                                         \
		*(dst)++ = ((row & 0x80) ? (fg) : (bg));                               \
		*(dst)++ = ((row & 0x40) ? (fg) : (bg));                               \
		*(dst)++ = ((row & 0x20) ? (fg) : (bg));                               \
		*(dst)++ = ((row & 0x10) ? (fg) : (bg));                               \
		*(dst)++ = ((row & 0x08) ? (fg) : (bg));                               \
		*(dst)++ = ((row & 0x04) ? (fg) : (bg));                               \
		*(dst)++ = ((row & 0x02) ? (fg) : (bg));                               \
		*(dst)++ = ((row & 0x01) ? (fg) : (bg));                               \
		                                                                       \
		/* move to the next row */                                             \
		++src;                                                                 \
	} while(0)

#define SDHatchBrush_StyleToData(style, data, stride, foreground, background)  \
	do {                                                                       \
		/* declarations */                                                     \
		const SDByte  *src;                                                    \
		SDColor       *dst;                                                    \
		SDColor        fg;                                                     \
		SDColor        bg;                                                     \
		                                                                       \
		/* get the source pixel row pointer */                                 \
		src = SDHatchBrush_Styles[(style)];                                    \
		                                                                       \
		/* NOTE: pixman's format is native endian */                           \
		                                                                       \
		/* get the destination pixel pointer */                                \
		dst = ((SDColor *)(data));                                             \
		                                                                       \
		/* get the foreground color */                                         \
		fg = (foreground);                                                     \
		                                                                       \
		/* get the background color */                                         \
		bg = (background);                                                     \
		                                                                       \
		/* set the data, row by row */                                         \
		SDHatchBrush_StyleToData_SetRow(src, dst, fg, bg);                     \
		(data) += (stride);                                                    \
		dst = ((SDColor *)(data));                                             \
		SDHatchBrush_StyleToData_SetRow(src, dst, fg, bg);                     \
		(data) += (stride);                                                    \
		dst = ((SDColor *)(data));                                             \
		SDHatchBrush_StyleToData_SetRow(src, dst, fg, bg);                     \
		(data) += (stride);                                                    \
		dst = ((SDColor *)(data));                                             \
		SDHatchBrush_StyleToData_SetRow(src, dst, fg, bg);                     \
		(data) += (stride);                                                    \
		dst = ((SDColor *)(data));                                             \
		SDHatchBrush_StyleToData_SetRow(src, dst, fg, bg);                     \
		(data) += (stride);                                                    \
		dst = ((SDColor *)(data));                                             \
		SDHatchBrush_StyleToData_SetRow(src, dst, fg, bg);                     \
		(data) += (stride);                                                    \
		dst = ((SDColor *)(data));                                             \
		SDHatchBrush_StyleToData_SetRow(src, dst, fg, bg);                     \
		(data) += (stride);                                                    \
		dst = ((SDColor *)(data));                                             \
		SDHatchBrush_StyleToData_SetRow(src, dst, fg, bg);                     \
	} while(0)

static const SDByte SDHatchBrush_Styles[][8] =
{
	/* SDHatchStyle_Horizontal */
	{ 0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00 },

	/* SDHatchStyle_Vertical */
	{ 0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01 },

	/* SDHatchStyle_ForwardDiagonal */
	{ 0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80 },

	/* SDHatchStyle_BackwardDiagonal */
	{ 0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01 },

	/* SDHatchStyle_Cross */
	{ 0xFF,0x01,0x01,0x01,0x01,0x01,0x01,0x01 },

	/* SDHatchStyle_DiagonalCross */
	{ 0x81,0x42,0x24,0x18,0x18,0x24,0x42,0x81 },

	/* SDHatchStyle_Percent05 */
	{ 0x01,0x00,0x00,0x00,0x10,0x00,0x00,0x00 },

	/* SDHatchStyle_Percent10 */
	{ 0x01,0x00,0x10,0x00,0x01,0x00,0x10,0x00 },

	/* SDHatchStyle_Percent20 */
	{ 0x11,0x00,0x44,0x00,0x11,0x00,0x44,0x00 },

	/* SDHatchStyle_Percent25 */
	{ 0x11,0x44,0x11,0x44,0x11,0x44,0x11,0x44 },

	/* SDHatchStyle_Percent30 */
	{ 0x55,0x22,0x55,0x88,0x55,0x22,0x55,0x88 },

	/* SDHatchStyle_Percent40 */
	{ 0x55,0xAA,0x55,0x8A,0x55,0xAA,0x55,0xA8 },

	/* SDHatchStyle_Percent50 */
	{ 0x55,0xAA,0x55,0xAA,0x55,0xAA,0x55,0xAA },

	/* SDHatchStyle_Percent60 */
	{ 0x77,0xAA,0xDD,0xAA,0x77,0xAA,0xDD,0xAA },

	/* SDHatchStyle_Percent70 */
	{ 0xEE,0xBB,0xEE,0xBB,0xEE,0xBB,0xEE,0xBB },

	/* SDHatchStyle_Percent75 */
	{ 0xEE,0xFF,0xBB,0xFF,0xEE,0xFF,0xBB,0xFF },

	/* SDHatchStyle_Percent80 */
	{ 0xF7,0xFF,0x7F,0xFF,0xF7,0xFF,0x7F,0xFF },

	/* SDHatchStyle_Percent90 */
	{ 0xFF,0xFF,0xFF,0xEF,0xFF,0xFF,0xFF,0xFE },

	/* SDHatchStyle_LightDownwardDiagonal */
	{ 0x11,0x22,0x44,0x88,0x11,0x22,0x44,0x88 },

	/* SDHatchStyle_LightUpwardDiagonal */
	{ 0x88,0x44,0x22,0x11,0x88,0x44,0x22,0x11 },

	/* SDHatchStyle_DarkDownwardDiagonal */
	{ 0x33,0x66,0xCC,0x99,0x33,0x66,0xCC,0x99 },

	/* SDHatchStyle_DarkUpwardDiagonal */
	{ 0xCC,0x66,0x33,0x99,0xCC,0x66,0x33,0x99 },

	/* SDHatchStyle_WideDownwardDiagonal */
	{ 0x83,0x07,0x0E,0x1C,0x38,0x70,0xE0,0xC1 },

	/* SDHatchStyle_WideUpwardDiagonal */
	{ 0xC1,0xE0,0x70,0x38,0x1C,0x0E,0x07,0x83 },

	/* SDHatchStyle_LightVertical */
	{ 0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11 },

	/* SDHatchStyle_LightHorizontal */
	{ 0xFF,0x00,0x00,0x00,0xFF,0x00,0x00,0x00 },

	/* SDHatchStyle_NarrowVertical */
	{ 0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA },

	/* SDHatchStyle_NarrowHorizontal */
	{ 0xFF,0x00,0xFF,0x00,0xFF,0x00,0xFF,0x00 },

	/* SDHatchStyle_DarkVertical */
	{ 0x33,0x33,0x33,0x33,0x33,0x33,0x33,0x33 },

	/* SDHatchStyle_DarkHorizontal */
	{ 0xFF,0xFF,0x00,0x00,0xFF,0xFF,0x00,0x00 },

	/* SDHatchStyle_DashedDownwardDiagonal */
	{ 0x00,0x00,0x11,0x22,0x44,0x88,0x00,0x00 },

	/* SDHatchStyle_DashedUpwardDiagonal */
	{ 0x00,0x00,0x88,0x44,0x22,0x11,0x00,0x00 },

	/* SDHatchStyle_DashedHorizontal */
	{ 0x0F,0x00,0x00,0x00,0xF0,0x00,0x00,0x00 },

	/* SDHatchStyle_DashedVertical */
	{ 0x01,0x01,0x01,0x01,0x10,0x10,0x10,0x10 },

	/* SDHatchStyle_SmallConfetti */
	{ 0x01,0x10,0x02,0x40,0x08,0x80,0x04,0x20 },

	/* SDHatchStyle_LargeConfetti */
	{ 0x8D,0x0C,0xC0,0xD8,0x1B,0x03,0x30,0xB1 },

	/* SDHatchStyle_ZigZag */
	{ 0x81,0x42,0x24,0x18,0x81,0x42,0x24,0x18 },

	/* SDHatchStyle_Wave */
	{ 0x00,0x18,0xA4,0x03,0x00,0x18,0xA4,0x03 },

	/* SDHatchStyle_DiagonalBrick */
	{ 0x80,0x40,0x20,0x10,0x18,0x24,0x42,0x81 },

	/* SDHatchStyle_HorizontalBrick */
	{ 0xFF,0x01,0x01,0x01,0xFF,0x10,0x10,0x10 },

	/* SDHatchStyle_Weave */
	{ 0x11,0x2A,0x44,0xA2,0x11,0x28,0x44,0x8A },

	/* SDHatchStyle_Plaid */
	{ 0x55,0xAA,0x55,0xAA,0x0F,0x0F,0x0F,0x0F },

	/* SDHatchStyle_Divot */
	{ 0x00,0x08,0x10,0x08,0x00,0x01,0x80,0x01 },

	/* SDHatchStyle_DottedGrid */
	{ 0x55,0x00,0x01,0x00,0x01,0x00,0x01,0x00 },

	/* SDHatchStyle_DottedDiamond */
	{ 0x01,0x00,0x44,0x00,0x10,0x00,0x44,0x00 },

	/* SDHatchStyle_Shingle */
	{ 0xC0,0x21,0x12,0x0C,0x30,0x40,0x80,0x80 },

	/* SDHatchStyle_Trellis */
	{ 0xFF,0x66,0xFF,0x99,0xFF,0x66,0xFF,0x99 },

	/* SDHatchStyle_Sphere */
	{ 0xEE,0x91,0xF1,0xF1,0xEE,0x19,0x1F,0x1F },

	/* SDHatchStyle_SmallGrid */
	{ 0xFF,0x11,0x11,0x11,0xFF,0x11,0x11,0x11 },

	/* SDHatchStyle_SmallCheckerBoard */
	{ 0x99,0x66,0x66,0x99,0x99,0x66,0x66,0x99 },

	/* SDHatchStyle_LargeCheckerBoard */
	{ 0x0F,0x0F,0x0F,0x0F,0xF0,0xF0,0xF0,0xF0 },

	/* SDHatchStyle_OutlinedDiamond */
	{ 0x41,0x22,0x14,0x08,0x14,0x22,0x41,0x80 },

	/* SDHatchStyle_SolidDiamond */
	{ 0x08,0x1C,0x3E,0x7F,0x3E,0x1C,0x08,0x00 }
};

static SDStatus
SDHatchBrush_Clone(SDBrush  *_this,
                   SDBrush **_clone);
static SDStatus
SDHatchBrush_CreatePattern(SDBrush   *_this,
                           SDPattern *pattern);
static void
SDHatchBrush_Finalize(SDBrush *_this);

static const SDBrushClass SDHatchBrush_Class =
{
	SDBrushType_HatchFill,
	SDHatchBrush_Clone,
	SDHatchBrush_Finalize,
	SDHatchBrush_CreatePattern,
	"sentinel"
};

#ifdef __cplusplus
};
#endif

#endif /* _SD_HATCHBRUSH_H_ */
