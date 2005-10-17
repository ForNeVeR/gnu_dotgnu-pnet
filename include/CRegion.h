/*
 * SDRegion.h - Region header.
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

#ifndef _SD_REGION_H_
#define _SD_REGION_H_

#include "CrayonsInternal.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef SDUInt32 SDRegionType;
#define SDRegionType_Intersect  0x00000001
#define SDRegionType_Union      0x00000002
#define SDRegionType_Xor        0x00000003
#define SDRegionType_Exclude    0x00000004
#define SDRegionType_Complement 0x00000005
#define SDRegionType_Rectangle  0x10000000
#define SDRegionType_Path       0x10000001
#define SDRegionType_Empty      0x10000002
#define SDRegionType_Infinite   0x10000003

typedef struct _tagSDRegionNode SDRegionNode;
typedef struct _tagSDRegionOp   SDRegionOp;
typedef struct _tagSDRegionRect SDRegionRect;
typedef struct _tagSDRegionPath SDRegionPath;
typedef struct _tagSDRegionMask SDRegionMask;

struct _tagSDRegionNode
{
	SDRegionType  type;
};
struct _tagSDRegionOp
{
	SDRegionNode  _base;
	SDRegionNode *left;
	SDRegionNode *right;
};
struct _tagSDRegionRect
{
	SDRegionNode _base;
	SDRectangleF rectangle;
};
struct _tagSDRegionPath
{
	SDRegionNode  _base;
	SDPointF     *points;
	SDByte       *types;
	SDUInt32      count;
	SDFillMode    fillMode;
};
struct _tagSDRegionMask
{
	pixman_image_t     *image;
	SDAffineTransformF  transform;
};
struct _tagSDRegion
{
	SDRegionNode *head;
	SDRegionMask  mask;
};

#define SDRegionNode_Default(type) \
	static const SDRegionNode SDRegionNode_ ## type = \
		{ SDRegionType_ ## type }

SDRegionNode_Default(Intersect);
SDRegionNode_Default(Union);
SDRegionNode_Default(Xor);
SDRegionNode_Default(Exclude);
SDRegionNode_Default(Complement);
SDRegionNode_Default(Rectangle);
SDRegionNode_Default(Path);
SDRegionNode_Default(Empty);
SDRegionNode_Default(Infinite);

#define SDRegionOp_Alloc(node) \
	((node) = ((SDRegionOp *)SDMalloc(sizeof(SDRegionOp))))
#define SDRegionNode_Alloc(node) \
	((node) = ((SDRegionNode *)SDMalloc(sizeof(SDRegionNode))))
#define SDRegionRect_Alloc(node) \
	((node) = ((SDRegionRect *)SDMalloc(sizeof(SDRegionRect))))
#define SDRegionPath_Alloc(node) \
	((node) = ((SDRegionPath *)SDMalloc(sizeof(SDRegionPath))))

#define SDRegionNode_Type(node)   (((SDRegionNode *)(node))->type)
#define SDRegionNode_IsData(node) ((SDRegionNode_Type(node)) & 0x10000000)
#define SDRegionNode_IsOp(node)   (!(SDRegionNode_IsData((node))))

static const SDByte SDRegionRect_PathTypes[] =
{
	SDPathType_Start,
	SDPathType_Line,
	SDPathType_Line,
	(SDPathType_Line | SDPathType_CloseSubpath)
};

#define SDRegionRect_RectToPath(points, rectangle)                             \
	do {                                                                       \
		/* declarations */                                                     \
		SDPointF *curr;                                                        \
		                                                                       \
		/* get the edges of the rectangle */                                   \
		const SDFloat top = SDRectangle_Y((rectangle));                        \
		const SDFloat left = SDRectangle_X((rectangle));                       \
		const SDFloat right = SDRectangle_Width((rectangle)) + left;           \
		const SDFloat bottom = SDRectangle_Height((rectangle)) + top;          \
		                                                                       \
		/* get the current point pointer */                                    \
		curr = (points);                                                       \
		                                                                       \
		/* set the first point */                                              \
		SDPoint_X(*curr) = left;                                               \
		SDPoint_Y(*curr) = top;                                                \
		                                                                       \
		/* move to the next position */                                        \
		++curr;                                                                \
		                                                                       \
		/* set the second point */                                             \
		SDPoint_X(*curr) = right;                                              \
		SDPoint_Y(*curr) = top;                                                \
		                                                                       \
		/* move to the next position */                                        \
		++curr;                                                                \
		                                                                       \
		/* set the third point */                                              \
		SDPoint_X(*curr) = right;                                              \
		SDPoint_Y(*curr) = bottom;                                             \
		                                                                       \
		/* move to the next position */                                        \
		++curr;                                                                \
		                                                                       \
		/* set the fourth point */                                             \
		SDPoint_X(*curr) = left;                                               \
		SDPoint_Y(*curr) = bottom;                                             \
	} while(0)

SDINTERNAL void
SDRegionData_Free(SDRegionNode *node);
SDINTERNAL SDStatus
SDRegion_GetMask(SDRegion           *_this,
                 SDAffineTransformF *transform,
                 pixman_image_t     *mask);

#ifdef __cplusplus
};
#endif

#endif /* _SD_REGION_H_ */
