/*
 * SDRegion.c - Region implementation.
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

#include "CRegion.h"
#include "CMatrix.h"
#include "CRegionDisposer.h"
#include "CRegionCloner.h"
#include "CRegionRasterizer.h"
#include "CRegionTransformer.h"
#include "CRegionTranslator.h"

#ifdef __cplusplus
extern "C" {
#endif

static const SDRegionMask SDRegionMask_Zero =
	{ 0, { 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f } };

#define _OperationPath(_this, type, path)                                      \
	do {                                                                       \
		/* declarations */                                                     \
		SDRegionOp   *_op;                                                     \
		SDRegionPath *_data;                                                   \
		                                                                       \
		/* create the path node */                                             \
		SDStatus_Check                                                         \
			(SDRegionPath_Create                                               \
				(&_data, (path)));                                             \
		                                                                       \
		/* create the operation node */                                        \
		if(!(SDRegionOp_Alloc(_op)))                                           \
		{                                                                      \
			return SDStatus_OutOfMemory;                                       \
		}                                                                      \
		                                                                       \
		/* initialize the operation node */                                    \
		_op->_base = SDRegionNode_ ## type;                                    \
		_op->left  = (_this)->head;                                            \
		_op->right = ((SDRegionNode *)_data);                                  \
		                                                                       \
		/* reset the head node */                                              \
		(_this)->head = ((SDRegionNode *)_op);                                 \
		                                                                       \
		/* handle change event */                                              \
		SDRegion_OnChange((_this));                                            \
	} while(0)
#define _OperationRectangle(_this, type, rectangle)                            \
	do {                                                                       \
		/* declarations */                                                     \
		SDRegionOp   *_op;                                                     \
		SDRegionRect *_data;                                                   \
		                                                                       \
		/* create the rectangle node */                                        \
		SDStatus_Check                                                         \
			(SDRegionRect_Create                                               \
				(&_data, (rectangle)));                                        \
		                                                                       \
		/* create the operation node */                                        \
		if(!(SDRegionOp_Alloc(_op)))                                           \
		{                                                                      \
			return SDStatus_OutOfMemory;                                       \
		}                                                                      \
		                                                                       \
		/* initialize the operation node */                                    \
		_op->_base = SDRegionNode_ ## type;                                    \
		_op->left  = (_this)->head;                                            \
		_op->right = ((SDRegionNode *)_data);                                  \
		                                                                       \
		/* reset the head node */                                              \
		(_this)->head = ((SDRegionNode *)_op);                                 \
		                                                                       \
		/* handle change event */                                              \
		SDRegion_OnChange((_this));                                            \
	} while(0)
#define _OperationRegion(_this, type, other)                                   \
	do {                                                                       \
		/* declarations */                                                     \
		SDRegionOp   *_op;                                                     \
		SDRegionNode *_data;                                                   \
		                                                                       \
		/* create the copy nodes */                                            \
		SDStatus_Check                                                         \
			(SDRegionNode_Clone                                                \
				((other)->head, &_data));                                      \
		                                                                       \
		/* create the operation node */                                        \
		if(!(SDRegionOp_Alloc(_op)))                                           \
		{                                                                      \
			return SDStatus_OutOfMemory;                                       \
		}                                                                      \
		                                                                       \
		/* initialize the operation node */                                    \
		_op->_base = SDRegionNode_ ## type;                                    \
		_op->left  = (_this)->head;                                            \
		_op->right = ((SDRegionNode *)_data);                                  \
		                                                                       \
		/* reset the head node */                                              \
		(_this)->head = ((SDRegionNode *)_op);                                 \
		                                                                       \
		/* handle change event */                                              \
		SDRegion_OnChange((_this));                                            \
	} while(0)

SDINTERNAL void
SDRegionData_Free(SDRegionNode *node)
{
	/* assertions */
	SDASSERT((node != 0));

	/* dispose of path data, as needed */
	if(node->type == SDRegionType_Path)
	{
		/* get the path node */
		SDRegionPath *rp = ((SDRegionPath *)node);

		/* free the point list */
		SDFree(rp->points);

		/* free the type list */
		SDFree(rp->types);
	}

	/* free the node */
	SDFree(node);
}

static SDStatus
SDRegionPath_Create(SDRegionPath **_this,
                    SDPath        *path)
{
	/* declarations */
	SDPointF     *points;
	SDByte       *types;
	SDUInt32      count;
	SDFillMode    fillMode;

	/* assertions */
	SDASSERT((_this != 0));

	/* get the fill mode */
	SDStatus_Check
		(SDPath_GetFillMode
			(path, &fillMode));

	/* get the path data */
	SDStatus_Check
		(SDPath_GetPathData
			(path, &points, &types, &count));

	/* create the node */
	if(!(SDRegionPath_Alloc(*_this)))
	{
		SDFree(points);
		SDFree(types);
		return SDStatus_OutOfMemory;
	}

	/* initialize the node */
	(*_this)->_base    = SDRegionNode_Path;
	(*_this)->points   = points;
	(*_this)->types    = types;
	(*_this)->count    = count;
	(*_this)->fillMode = fillMode;

	/* return successfully */
	return SDStatus_OK;
}

static SDStatus
SDRegionRect_Create(SDRegionRect **_this,
                    SDRectangleF   rectangle)
{
	/* assertions */
	SDASSERT((_this     != 0));

	/* create the node */
	if(!(SDRegionRect_Alloc(*_this)))
	{
		return SDStatus_OutOfMemory;
	}

	/* initialize the node */
	(*_this)->_base     = SDRegionNode_Rectangle;
	(*_this)->rectangle = rectangle;

	/* return successfully */
	return SDStatus_OK;
}


static SDStatus
SDRegionNode_Clone(SDRegionNode  *_this,
                   SDRegionNode **clone)
{
	/* declarations */
	SDRegionCloner cloner;
	SDStatus       status;

	/* assertions */
	SDASSERT((_this != 0));
	SDASSERT((clone != 0));

	/* initialize the cloner */
	SDRegionCloner_Initialize(&cloner);

	/* clone the node */
	status =
		SDRegionInterpreter_Interpret
			(((SDRegionInterpreter *)(&cloner)), _this, (void **)clone);

	/* finalize the cloner */
	SDRegionCloner_Finalize(&cloner);

	/* return status */
	return status;
}

/* Handle change events. */
static void
SDRegion_OnChange(SDRegion *_this)
{
	/* assertions */
	SDASSERT((_this != 0));

	/* clear the mask */
	if(_this->mask.image != 0)
	{
		pixman_image_destroy(_this->mask.image);
		_this->mask = SDRegionMask_Zero;
	}
}

/* Generate the mask for this region. */
static SDStatus
SDRegion_GenerateMask(SDRegion           *_this,
                      SDAffineTransformF *transform,
                      SDUInt32            width,
                      SDUInt32            height,
                      SDBool              gray)
{
	/* assertions */
	SDASSERT((_this     != 0));
	SDASSERT((transform != 0));

	/* clear the mask, as needed */
	if(_this->mask.image != 0)
	{
		/* declarations */
		SDUInt32 w;
		SDUInt32 h;
		SDBool   g;

		/* get the mask image information */
		w = pixman_image_get_width(_this->mask.image);
		h = pixman_image_get_height(_this->mask.image);
		g = (pixman_image_get_depth(_this->mask.image) != 1);

		/* bail out now if the mask is okay as is */
		if(w == width && h == height && g == gray)
		{
			/* declarations */
			SDBool eq;

			/* determine if the transformations are equal */
			SDAffineTransformF_Equals(transform, &(_this->mask.transform), &eq);

			/* bail out now if everything matches */
			SDStatus_Require((!eq), SDStatus_OK);
		}

		/* destroy the current mask image */
		pixman_image_destroy(_this->mask.image);

		/* reset the mask */
		_this->mask = SDRegionMask_Zero;
	}

	/* generate a new mask */
	{
		/* declarations */
		SDRegionRasterizer rast;
		SDStatus           status;

		/* initialize the rasterizer */
		SDStatus_Check
			(SDRegionRasterizer_Initialize
				(&rast, transform, width, height, gray));

		/* generate the mask image */
		status =
			SDRegionInterpreter_Interpret
				((SDRegionInterpreter *)&rast,
				 _this->head, (void **)&(_this->mask.image));

		/* finalize the rasterizer */
		SDRegionRasterizer_Finalize(&rast);

		/* handle mask generation failures */
		SDStatus_Check(status);

		/* set the mask transform */
		_this->mask.transform = *transform;
	}

	/* return successfully */
	return SDStatus_OK;
}

/* Initialize this region. */
static void
SDRegion_Initialize(SDRegion     *_this,
                    SDRegionNode *head)
{
	/* assertions */
	SDASSERT((_this != 0));
	SDASSERT((head  != 0));

	/* initialize the region */
	_this->head = head;

	/* initialize the mask */
	_this->mask = SDRegionMask_Zero;
}

/* Finalize this region. */
static void
SDRegion_Finalize(SDRegion *_this)
{
	/* assertions */
	SDASSERT((_this != 0));

	/* finalize the region */
	{
		/* declarations */
		SDRegionNode *node;

		/* get the head node */
		node = _this->head;

		/* dispose of nodes, as needed */
		if(node != 0)
		{
			if(SDRegionNode_IsData(node))
			{
				SDRegionData_Free(node);
			}
			else
			{
				/* declarations */
				SDRegionDisposer  disposer;
				void             *data;

				/* initialize the disposer */
				SDRegionDisposer_Initialize(&disposer);

				/* dispose of the node */
				SDRegionInterpreter_Interpret
					(((SDRegionInterpreter *)(&disposer)), _this->head, data);

				/* finalize the disposer */
				SDRegionDisposer_Finalize(&disposer);
			}
		}

		/* reset the head pointer */
		_this->head = 0;

		/* finalize the mask */
		if(_this->mask.image != 0)
		{
			pixman_image_destroy(_this->mask.image);
			_this->mask = SDRegionMask_Zero;
		}
	}
}

/* Create an infinite region. */
SDStatus
SDRegion_Create(SDRegion **_this)
{
	/* declarations */
	SDRegionNode *node;

	/* ensure we have a this pointer pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* allocate the region */
	if(!(*_this = (SDRegion *)SDMalloc(sizeof(SDRegion))))
	{
		return SDStatus_OutOfMemory;
	}

	/* create the node */
	if(!(SDRegionNode_Alloc(node)))
	{
		SDFree(*_this);
		*_this = 0;
		return SDStatus_OutOfMemory;
	}

	/* initialize the node */
	*node = SDRegionNode_Infinite;

	/* initialize the region */
	SDRegion_Initialize(*_this, node);

	/* return successfully */
	return SDStatus_OK;
}

/* Create a path region. */
SDStatus
SDRegion_CreatePath(SDRegion **_this,
                    SDPath    *path)
{
	/* declarations */
	SDRegionPath *node;

	/* ensure we have a this pointer pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* create the path node */
	SDStatus_Check
		(SDRegionPath_Create
			(&node, path));

	/* allocate the region */
	if(!(*_this = (SDRegion *)SDMalloc(sizeof(SDRegion))))
	{
		SDRegionData_Free((SDRegionNode *)node);
		return SDStatus_OutOfMemory;
	}

	/* initialize the region */
	SDRegion_Initialize(*_this, (SDRegionNode *)node);

	/* return successfully */
	return SDStatus_OK;
}

/* Create a rectangular region. */
SDStatus
SDRegion_CreateRectangle(SDRegion     **_this,
                         SDRectangleF   rectangle)
{
	/* declarations */
	SDRegionRect *node;

	/* ensure we have a this pointer pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* create the rectangle node */
	SDStatus_Check
		(SDRegionRect_Create
			(&node, rectangle));

	/* allocate the region */
	if(!(*_this = (SDRegion *)SDMalloc(sizeof(SDRegion))))
	{
		SDRegionData_Free((SDRegionNode *)node);
		return SDStatus_OutOfMemory;
	}

	/* initialize the region */
	SDRegion_Initialize(*_this, (SDRegionNode *)node);

	/* return successfully */
	return SDStatus_OK;
}

/* Create a region from serialized region data. */
SDStatus
SDRegion_CreateData(SDRegion **_this,
                    SDByte    *data,
                    SDUInt32   count)
{
#if 0
	/* ensure we have a this pointer pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* allocate the region */
	if(!(*_this = (SDRegion *)SDMalloc(sizeof(SDRegion))))
	{
		return SDStatus_OutOfMemory;
	}

	/* TODO */

	/* return successfully */
	return SDStatus_OK;
#else
	return SDStatus_NotImplemented;
#endif
}

/* Create a region from a GDI region. */
SDStatus
SDRegion_CreateHRGN(SDRegion **_this,
                    void      *hrgn)
{
#if 0
	/* ensure we have a this pointer pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* allocate the region */
	if(!(*_this = (SDRegion *)SDMalloc(sizeof(SDRegion))))
	{
		return SDStatus_OutOfMemory;
	}

	/* TODO: add support for wine and native win32 */

	/* return successfully */
	return SDStatus_OK;
#else
	return SDStatus_NotImplemented;
#endif
}

/* Destroy a region. */
SDStatus
SDRegion_Destroy(SDRegion **_this)
{
	/* ensure we have a this pointer pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a this pointer */
	SDStatus_Require(((*_this) != 0), SDStatus_ArgumentNull);

	/* finalize the region */
	SDRegion_Finalize(*_this);

	/* free the region */
	SDFree(*_this);

	/* null the this pointer */
	*_this = 0;

	/* return successfully */
	return SDStatus_OK;
}

/* Clone this region. */
SDStatus
SDRegion_Clone(SDRegion  *_this,
               SDRegion **clone)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a clone pointer pointer */
	SDStatus_Require((clone != 0), SDStatus_ArgumentNull);

	/* create the clone region */
	SDStatus_Check(SDRegion_Create(clone));

	/* clone the members */
	{
		/* declarations */
		SDStatus status;

		/* copy the nodes */
		status = SDRegion_CombineRegion(*clone, _this, SDCombineMode_Replace);

		/* handle status */
		if(status != SDStatus_OK)
		{
			/* destroy the clone */
			SDRegion_Destroy(clone);

			/* return status */
			return status;
		}
	}

	/* return successfully */
	return SDStatus_OK;
}

/* Form the combination of this region with a path. */
SDStatus
SDRegion_CombinePath(SDRegion      *_this,
                     SDPath        *path,
                     SDCombineMode  combineMode)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a path pointer */
	SDStatus_Require((path != 0), SDStatus_ArgumentNull);

	/* ensure the combination mode is in range */
	SDCombineMode_Default(combineMode);

	/* form the combination */
	{
		/* declarations */
		SDRegionType t;

		/* get the head node type */
		t = SDRegionNode_Type(_this->head);

		/* form the combination */
		switch(combineMode)
		{
			case SDCombineMode_Intersect:
			{
				/* bail out now if there's nothing to do */
				SDStatus_Require((t != SDRegionType_Empty), SDStatus_OK);

				/* replace, as needed */
				if(t == SDRegionType_Infinite) { goto _Replace; }

				/* form the intersection */
				_OperationPath(_this, Intersect, path);
			}
			break;
			case SDCombineMode_Union:
			{
				/* bail out now if there's nothing to do */
				SDStatus_Require((t != SDRegionType_Infinite), SDStatus_OK);

				/* replace, as needed */
				if(t == SDRegionType_Empty) { goto _Replace; }

				/* form the union */
				_OperationPath(_this, Union, path);
			}
			break;
			case SDCombineMode_Xor:
			{
				/* replace, as needed */
				if(t == SDRegionType_Empty) { goto _Replace; }

				/* form the xor */
				_OperationPath(_this, Xor, path);
			}
			break;
			case SDCombineMode_Exclude:
			{
				/* bail out now if there's nothing to do */
				SDStatus_Require((t != SDRegionType_Empty), SDStatus_OK);

				/* form the exclusion */
				_OperationPath(_this, Exclude, path);
			}
			break;
			case SDCombineMode_Complement:
			{
				/* replace, as needed */
				if(t == SDRegionType_Empty) { goto _Replace; }

				/* form the empty region, as needed */
				if(t == SDRegionType_Infinite)
				{
					return SDRegion_MakeEmpty(_this);
				}

				/* form the complement */
				_OperationPath(_this, Complement, path);
			}
			break;
			case SDCombineMode_Replace:
			default:
			{
				/* provide local jump target */
				_Replace:

				/* add new path */
				{
					/* declarations */
					SDRegionPath *data;

					/* create the path node */
					SDStatus_Check
						(SDRegionPath_Create
							(&data, path));

					/* dispose of current region data */
					SDRegion_Finalize(_this);

					/* reset the head node */
					_this->head = (SDRegionNode *)data;
				}
			}
			break;
		}
	}

	/* return successfully */
	return SDStatus_OK;
}

/* Form the combination of this region with a rectangle. */
SDStatus
SDRegion_CombineRectangle(SDRegion      *_this,
                          SDRectangleF   rectangle,
                          SDCombineMode  combineMode)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure the combination mode is in range */
	SDCombineMode_Default(combineMode);

	/* form the combination */
	{
		/* declarations */
		SDRegionType t;

		/* get the head node type */
		t = SDRegionNode_Type(_this->head);

		/* form the combination */
		switch(combineMode)
		{
			case SDCombineMode_Intersect:
			{
				/* bail out now if there's nothing to do */
				SDStatus_Require((t != SDRegionType_Empty), SDStatus_OK);

				/* replace, as needed */
				if(t == SDRegionType_Infinite) { goto _Replace; }

				/* form the intersection */
				_OperationRectangle(_this, Intersect, rectangle);
			}
			break;
			case SDCombineMode_Union:
			{
				/* bail out now if there's nothing to do */
				SDStatus_Require((t != SDRegionType_Infinite), SDStatus_OK);

				/* replace, as needed */
				if(t == SDRegionType_Empty) { goto _Replace; }

				/* form the union */
				_OperationRectangle(_this, Union, rectangle);
			}
			break;
			case SDCombineMode_Xor:
			{
				/* replace, as needed */
				if(t == SDRegionType_Empty) { goto _Replace; }

				/* form the xor */
				_OperationRectangle(_this, Xor, rectangle);
			}
			break;
			case SDCombineMode_Exclude:
			{
				/* bail out now if there's nothing to do */
				SDStatus_Require((t != SDRegionType_Empty), SDStatus_OK);

				/* form the exclusion */
				_OperationRectangle(_this, Exclude, rectangle);
			}
			break;
			case SDCombineMode_Complement:
			{
				/* replace, as needed */
				if(t == SDRegionType_Empty) { goto _Replace; }

				/* form the empty region, as needed */
				if(t == SDRegionType_Infinite)
				{
					return SDRegion_MakeEmpty(_this);
				}

				/* form the complement */
				_OperationRectangle(_this, Complement, rectangle);
			}
			break;
			case SDCombineMode_Replace:
			default:
			{
				/* provide local jump target */
				_Replace:

				/* add new rectangle */
				{
					/* declarations */
					SDRegionRect *data;

					/* create the rectangle node */
					SDStatus_Check
						(SDRegionRect_Create
							(&data, rectangle));

					/* dispose of current region data */
					SDRegion_Finalize(_this);

					/* reset the head node */
					_this->head = (SDRegionNode *)data;
				}
			}
			break;
		}
	}

	/* return successfully */
	return SDStatus_OK;
}

/* Form the combination of this region with another region. */
SDStatus
SDRegion_CombineRegion(SDRegion      *_this,
                       SDRegion      *other,
                       SDCombineMode  combineMode)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have an other pointer */
	SDStatus_Require((other != 0), SDStatus_ArgumentNull);

	/* ensure the combination mode is in range */
	SDCombineMode_Default(combineMode);

	/* form the combination */
	{
		/* declarations */
		SDRegionType tt;
		SDRegionType ot;

		/* get the head node types */
		tt = SDRegionNode_Type(_this->head);
		ot = SDRegionNode_Type(other->head);

		/* form the combination */
		switch(combineMode)
		{
			case SDCombineMode_Intersect:
			{
				/* bail out now if there's nothing to do */
				SDStatus_Require((tt != SDRegionType_Empty), SDStatus_OK);
				SDStatus_Require((ot != SDRegionType_Infinite), SDStatus_OK);

				/* replace, as needed */
				if(tt == SDRegionType_Infinite) { goto _Replace; }

				/* form the empty region, as needed */
				if(ot == SDRegionType_Empty)
				{
					return SDRegion_MakeEmpty(_this);
				}

				/* form the intersection */
				_OperationRegion(_this, Intersect, other);
			}
			break;
			case SDCombineMode_Union:
			{
				/* bail out now if there's nothing to do */
				SDStatus_Require((ot != SDRegionType_Empty), SDStatus_OK);
				SDStatus_Require((tt != SDRegionType_Infinite), SDStatus_OK);

				/* replace, as needed */
				if(tt == SDRegionType_Empty) { goto _Replace; }

				/* form the infinite region, as needed */
				if(ot == SDRegionType_Infinite)
				{
					return SDRegion_MakeInfinite(_this);
				}

				/* form the union */
				_OperationRegion(_this, Union, other);
			}
			break;
			case SDCombineMode_Xor:
			{
				/* bail out now if there's nothing to do */
				SDStatus_Require((ot != SDRegionType_Empty), SDStatus_OK);

				/* replace, as needed */
				if(tt == SDRegionType_Empty) { goto _Replace; }

				/* form the xor */
				_OperationRegion(_this, Xor, other);
			}
			break;
			case SDCombineMode_Exclude:
			{
				/* bail out now if there's nothing to do */
				SDStatus_Require((tt != SDRegionType_Empty), SDStatus_OK);
				SDStatus_Require((ot != SDRegionType_Empty), SDStatus_OK);

				/* form the empty region, as needed */
				if(ot == SDRegionType_Infinite)
				{
					return SDRegion_MakeEmpty(_this);
				}

				/* form the exclusion */
				_OperationRegion(_this, Exclude, other);
			}
			break;
			case SDCombineMode_Complement:
			{
				/* replace, as needed */
				if(tt == SDRegionType_Empty) { goto _Replace; }

				/* form the empty region, as needed */
				if(tt == SDRegionType_Infinite || ot == SDRegionType_Empty)
				{
					return SDRegion_MakeEmpty(_this);
				}

				/* form the complement */
				_OperationRegion(_this, Complement, other);
			}
			break;
			case SDCombineMode_Replace:
			default:
			{
				/* provide local jump target */
				_Replace:

				/* add new region */
				{
					/* declarations */
					SDRegionNode *data;

					/* copy the other tree */
					SDStatus_Check
						(SDRegionNode_Clone
							(other->head, &data));

					/* dispose of current region data */
					SDRegion_Finalize(_this);

					/* reset the head node */
					_this->head = (SDRegionNode *)data;
				}
			}
			break;
		}
	}

	/* return successfully */
	return SDStatus_OK;
}

/* Determine if two regions are equal after applying a transformation. */
SDStatus
SDRegion_Equals(SDRegion   *_this,
                SDRegion   *other,
                SDGraphics *graphics,
                SDBool     *eq)
{
	/* TODO */
	return SDStatus_NotImplemented;
}

/* Get the bounds of this region on a particular graphics context. */
SDStatus
SDRegion_GetBounds(SDRegion     *_this,
                   SDGraphics   *graphics,
                   SDRectangleF *bounds)
{
	/* TODO */
	return SDStatus_NotImplemented;
}

/* Get the raw region data for this region. */
SDStatus
SDRegion_GetData(SDRegion  *_this,
                 SDByte   **data,
                 SDUInt32  *count)
{
	/* TODO */
	return SDStatus_NotImplemented;
}

/* Get an array of rectangles which represents this region. */
SDStatus
SDRegion_GetRegionScans(SDRegion      *_this,
                        SDMatrix      *matrix,
                        SDRectangleF **scans,
                        SDUInt32      *count)
{
	/* TODO */
	return SDStatus_NotImplemented;
}

/* Determine if this region is empty on a particular graphics context. */
SDStatus
SDRegion_IsEmpty(SDRegion   *_this,
                 SDGraphics *graphics,
                 SDBool     *empty)
{
	/* TODO */
	return SDStatus_NotImplemented;
}

/* Determine if this region is infinite on a particular graphics context. */
SDStatus
SDRegion_IsInfinite(SDRegion   *_this,
                    SDGraphics *graphics,
                    SDBool     *infinite)
{
	/* TODO */
	return SDStatus_NotImplemented;
}

/* Determine if a point is contained within this region. */
SDStatus
SDRegion_IsVisiblePoint(SDRegion   *_this,
                        SDGraphics *graphics,
                        SDPointF    point,
                        SDBool     *visible)
{
	/* TODO */
	return SDStatus_NotImplemented;
}

/* Determine if any part of a rectangle is contained within this region. */
SDStatus
SDRegion_IsVisibleRectangle(SDRegion     *_this,
                            SDGraphics   *graphics,
                            SDRectangleF  rectangle,
                            SDBool       *visible)
{
	/* TODO */
	return SDStatus_NotImplemented;
}

/* Make this region empty. */
SDStatus
SDRegion_MakeEmpty(SDRegion *_this)
{
	/* declarations */
	SDRegionNode *data;

	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* create the empty node */
	if(!(SDRegionNode_Alloc(data)))
	{
		return SDStatus_OutOfMemory;
	}

	/* initialize the empty node */
	*data = SDRegionNode_Empty;

	/* dispose of current region data */
	SDRegion_Finalize(_this);

	/* reset the head node */
	_this->head = data;

	/* return successfully */
	return SDStatus_OK;
}

/* Make this region infinite. */
SDStatus
SDRegion_MakeInfinite(SDRegion *_this)
{
	/* declarations */
	SDRegionNode *data;

	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* create the infinite node */
	if(!(SDRegionNode_Alloc(data)))
	{
		return SDStatus_OutOfMemory;
	}

	/* initialize the infinite node */
	*data = SDRegionNode_Infinite;

	/* dispose of current region data */
	SDRegion_Finalize(_this);

	/* reset the head node */
	_this->head = data;

	/* return successfully */
	return SDStatus_OK;
}

/* Transform this region by a matrix. */
SDStatus
SDRegion_Transform(SDRegion *_this,
                   SDMatrix *matrix)
{
	/* declarations */
	SDRegionTransformer  transformer;
	SDAffineTransformF   t;
	SDBool               eq;
	SDStatus             status;
	void                *data;

	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* get the matrix transformation */
	SDStatus_Check
		(SDMatrix_GetTransform
			(matrix, &t));

	/* determine if we have the identity transformation */
	SDAffineTransformF_Equals(&t, &SDAffineTransformF_Identity, &eq);

	/* bail out now if there's nothing to do */
	SDStatus_Require((!eq), SDStatus_OK);

	/* initialize the transformer */
	SDRegionTransformer_Initialize(&transformer, &t);

	/* transform the region */
	status =
		SDRegionInterpreter_Interpret
			(((SDRegionInterpreter *)(&transformer)), _this->head, &data);

	/* finalize the transformer */
	SDRegionTransformer_Finalize(&transformer);

	/* handle change event */
	SDRegion_OnChange((_this));

	/* return status */
	return status;
}

/* Translate this region by a specific amount. */
SDStatus
SDRegion_Translate(SDRegion *_this,
                   SDFloat   dx,
                   SDFloat   dy)
{
	/* declarations */
	SDRegionTranslator  translator;
	SDStatus            status;
	void               *data;

	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* bail out now if there's nothing to do */
	SDStatus_Require(((dx != 0.0f) || (dy != 0.0f)), SDStatus_OK);

	/* initialize the translator */
	SDRegionTranslator_Initialize(&translator, dx, dy);

	/* translate the region */
	status =
		SDRegionInterpreter_Interpret
			(((SDRegionInterpreter *)(&translator)), _this->head, &data);

	/* finalize the translator */
	SDRegionTranslator_Finalize(&translator);

	/* handle change event */
	SDRegion_OnChange((_this));

	/* return status */
	return status;
}

/* Get the mask for this region. */
SDINTERNAL SDStatus
SDRegion_GetMask(SDRegion           *_this,
                 SDAffineTransformF *transform,
                 pixman_image_t     *mask)
{
	/* declarations */
	SDUInt32 w;
	SDUInt32 h;
	SDBool   g;

	/* assertions */
	SDASSERT((_this     != 0));
	SDASSERT((transform != 0));
	SDASSERT((mask      != 0));

	/* get the mask image information */
	w = pixman_image_get_width(mask);
	h = pixman_image_get_height(mask);
	g = (pixman_image_get_depth(mask) != 1);

	/* ensure we have a mask for the given settings */
	SDStatus_Check
		(SDRegion_GenerateMask
			(_this, transform, w, h, g));

	/* copy the mask data */
	{
		/* declarations */
		SDByte   *dst;
		SDByte   *src;
		SDUInt32  s;

		/* get the mask data */
		dst = (SDByte *)pixman_image_get_data(mask);
		src = (SDByte *)pixman_image_get_data(_this->mask.image);

		/* get the stride */
		s = pixman_image_get_stride(mask);

		/* copy the data */
		SDMemCopy(dst, src, (h * s));
	}

	/* return successfully */
	return SDStatus_OK;
}


#ifdef __cplusplus
};
#endif
