/*
 * SDGraphics.c - Graphics implementation.
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

#include "CGraphics.h"
#include "CBrush.h"
#include "CFiller.h"
#include "CImage.h"
#include "CMatrix.h"
#include "CPath.h"
#include "CPen.h"
#include "CRegion.h"
#include "CStroker.h"
#include "CSurface.h"
#include "CTrapezoids.h"
#include "CUtils.h"

#ifdef __cplusplus
extern "C" {
#endif

/*\
|*| NOTE: The defines/ifdefs here are a hack to get something akin to C# region
|*|       blocks; they serve a purely aesthetic purpose.
\*/

/******************************************************************************/
#define SDGraphics_LIFETIME
#ifdef SDGraphics_LIFETIME
/* Create a graphics context. */
SDStatus
SDGraphics_Create(SDGraphics **_this,
                  SDSurface   *surface)
{
	/* ensure we have a this pointer pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a surface pointer */
	SDStatus_Require((surface != 0), SDStatus_ArgumentNull);

	/* allocate the graphics context */
	if(!(*_this = (SDGraphics *)SDMalloc(sizeof(SDGraphics))))
	{
		return SDStatus_OutOfMemory;
	}

	/* initialize the graphics context */
	{
		/* declarations */
		SDStatus status;

		/* get the graphics context */
		SDGraphics *gc = *_this;

		/* initialize the transformation pipeline */
		SDGraphicsPipeline_ResetWorld(&(gc->pipeline));
		SDGraphicsPipeline_ResetPage(&(gc->pipeline));

		/* initialize the clipping region */
		if((status = SDRegion_Create(&(gc->clip))) != SDStatus_OK)
		{
			SDFree(*_this);
			*_this = 0;
			return status;
		}

		/* initialize the rendering path */
		if((status = SDPath_Create(&(gc->path))) != SDStatus_OK)
		{
			SDRegion_Destroy(&(gc->clip));
			SDFree(*_this);
			*_this = 0;
			return status;
		}

		/* initialize the stroking path */
		if((status = SDPath_Create(&(gc->stroke))) != SDStatus_OK)
		{
			SDPath_Destroy(&(gc->path));
			SDRegion_Destroy(&(gc->clip));
			SDFree(*_this);
			*_this = 0;
			return status;
		}

		/* initialize the remaining members */
		gc->surface                    = surface;
		gc->compositingMode            = SDCompositingMode_SourceOver;
		gc->compositingQuality         = SDCompositingQuality_Default;
		gc->pageUnit                   = SDGraphicsUnit_Display;
		gc->pageScale                  = 1.0;
		gc->interpolationMode          = SDInterpolationMode_Default;
		gc->pixelOffsetMode            = SDPixelOffsetMode_Default;
		SDPoint_X(gc->renderingOrigin) = 0;
		SDPoint_Y(gc->renderingOrigin) = 0;
		gc->smoothingMode              = SDSmoothingMode_Default;
		gc->textContrast               = 0;
		gc->textRenderingHint          = SDTextRenderingHint_SystemDefault;
	}

	/* return successfully */
	return SDStatus_OK;
}

/* Destroy a graphics context. */
SDStatus
SDGraphics_Destroy(SDGraphics **_this)
{
	/* ensure we have a this pointer pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a this pointer */
	SDStatus_Require((*_this != 0), SDStatus_ArgumentNull);

	/* finalize the graphics context */
	{
		/* get the graphics context */
		SDGraphics *gc = *_this;

		/* finalize, as needed */
		if(gc->surface != 0)
		{
			/* destroy the surface */
			SDSurface_Destroy(&(gc->surface));

			/* destroy the clip */
			SDRegion_Destroy(&(gc->clip));

			/* destroy the rendering path */
			SDPath_Destroy(&(gc->path));
		}
	}

	/* dispose of the graphics context */
	SDFree(*_this);

	/* null the graphics context */
	*_this = 0;

	/* return successfully */
	return SDStatus_OK;
}
#endif
/******************************************************************************/











/******************************************************************************/
#define SDGraphics_WORLD
#ifdef SDGraphics_WORLD
/* Get the transformation matrix of this graphics context. */
SDStatus
SDGraphics_GetTransform(SDGraphics *_this,
                        SDMatrix   *matrix)
{
	/* declarations */
	SDAffineTransformF t;

	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a matrix pointer */
	SDStatus_Require((matrix != 0), SDStatus_ArgumentNull);

	/* get the world transformation */
	SDGraphicsPipeline_GetWorld(&(_this->pipeline), &t);

	/* set the matrix transformation */
	SDMatrix_SetTransform(matrix, &t);

	/* return successfully */
	return SDStatus_OK;
}

/* Multiply the transformation matrix by another matrix. */
SDStatus
SDGraphics_MultiplyTransform(SDGraphics    *_this,
                             SDMatrix      *matrix,
                             SDMatrixOrder  order)
{
	/* declarations */
	SDAffineTransformF t;

	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a matrix pointer */
	SDStatus_Require((matrix != 0), SDStatus_ArgumentNull);

	/* get the matrix transformation */
	SDMatrix_GetTransform(matrix, &t);

	/* multiply the world transformation */
	SDStatus_Check
		(SDGraphicsPipeline_MultiplyWorld
			(&(_this->pipeline), &t, order));

	/* return successfully */
	return SDStatus_OK;
}

/* Reset the transformation matrix of this graphics context. */
SDStatus
SDGraphics_ResetTransform(SDGraphics *_this)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* reset the world transformation and inverse */
	SDGraphicsPipeline_ResetWorld(&(_this->pipeline));

	/* return successfully */
	return SDStatus_OK;
}

/* Rotate the transformation matrix of this graphics context. */
SDStatus
SDGraphics_RotateTransform(SDGraphics    *_this,
                           SDFloat        angle,
                           SDMatrixOrder  order)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* rotate the world transformation and inverse */
	SDGraphicsPipeline_RotateWorld(&(_this->pipeline), angle, order);

	/* return successfully */
	return SDStatus_OK;
}

/* Scale the transformation matrix of this graphics context. */
SDStatus
SDGraphics_ScaleTransform(SDGraphics    *_this,
                          SDFloat        sx,
                          SDFloat        sy,
                          SDMatrixOrder  order)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* scale the world transformation and inverse */
	SDGraphicsPipeline_ScaleWorld(&(_this->pipeline), sx, sy, order);

	/* return successfully */
	return SDStatus_OK;
}

/* Set the transformation matrix of this graphics context. */
SDStatus
SDGraphics_SetTransform(SDGraphics *_this,
                        SDMatrix   *matrix)
{
	/* declarations */
	SDAffineTransformF t;

	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a matrix pointer */
	SDStatus_Require((matrix != 0), SDStatus_ArgumentNull);

	/* get the matrix transformation */
	SDMatrix_GetTransform(matrix, &t);

	/* set the world transformation */
	SDStatus_Check
		(SDGraphicsPipeline_SetWorld
			(&(_this->pipeline), &t));

	/* return successfully */
	return SDStatus_OK;
}

/* Translate the transformation matrix of this graphics context. */
SDStatus
SDGraphics_TranslateTransform(SDGraphics    *_this,
                              SDFloat        dx,
                              SDFloat        dy,
                              SDMatrixOrder  order)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* translate the world transformation and inverse */
	SDGraphicsPipeline_TranslateWorld(&(_this->pipeline), dx, dy, order);

	/* return successfully */
	return SDStatus_OK;
}
#endif
/******************************************************************************/












/******************************************************************************/
#define SDGraphics_METAFILE
#ifdef SDGraphics_METAFILE
#if 0
/* Add a metafile comment. */
SDStatus
SDGraphics_AddMetafileComment(SDGraphics *_this,
                              SDByte     *data,
                              SDUInt32    count)
{
	return SDStatus_NotImplemented;
}

/* Enumerate the contents of a metafile. */
SDStatus
SDGraphics_EnumerateMetafile(SDGraphics             *_this,
                             SDMetafile             *metafile,
                             SDPointF                dst,
                             SDImageAttributes      *atts,
                             SDMetafileEnumCallback  callback,
                             void                   *callbackData)
{
	return SDStatus_NotImplemented;
}
SDStatus
SDGraphics_EnumerateMetafile(SDGraphics             *_this,
                             SDMetafile             *metafile,
                             SDPointF               *dst,
                             SDUInt32                count,
                             SDImageAttributes      *atts,
                             SDMetafileEnumCallback  callback,
                             void                   *callbackData)
{
	return SDStatus_NotImplemented;
}
SDStatus
SDGraphics_EnumerateMetafile(SDGraphics             *_this,
                             SDMetafile             *metafile,
                             SDRectangleF            dst,
                             SDImageAttributes      *atts,
                             SDMetafileEnumCallback  callback,
                             void                   *callbackData)
{
	return SDStatus_NotImplemented;
}
SDStatus
SDGraphics_EnumerateMetafile(SDGraphics             *_this,
                             SDMetafile             *metafile,
                             SDPointF                dst,
                             SDRectangleF            src,
                             SDGraphicsUnit          srcUnit,
                             SDImageAttributes      *atts,
                             SDMetafileEnumCallback  callback,
                             void                   *callbackData)
{
	return SDStatus_NotImplemented;
}
SDStatus
SDGraphics_EnumerateMetafile(SDGraphics             *_this,
                             SDMetafile             *metafile,
                             SDPointF               *dst,
                             SDUInt32                count,
                             SDRectangleF            src,
                             SDGraphicsUnit          srcUnit,
                             SDImageAttributes      *atts,
                             SDMetafileEnumCallback  callback,
                             void                   *callbackData)
{
	return SDStatus_NotImplemented;
}
SDStatus
SDGraphics_EnumerateMetafile(SDGraphics             *_this,
                             SDMetafile             *metafile,
                             SDRectangleF            dst,
                             SDRectangleF            src,
                             SDGraphicsUnit          srcUnit,
                             SDImageAttributes      *atts,
                             SDMetafileEnumCallback  callback,
                             void                   *callbackData)
{
	return SDStatus_NotImplemented;
}
#endif
#endif
/******************************************************************************/












/******************************************************************************/
#define SDGraphics_CLIP
#ifdef SDGraphics_CLIP
/* Get the clipping region of this graphics context. */
SDStatus
SDGraphics_GetClip(SDGraphics *_this,
                   SDRegion   *region)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* set the region to the clipping region */
	SDStatus_Check
		(SDRegion_CombineRegion
			(region, _this->clip, SDCombineMode_Replace));

	/* return successfully */
	return SDStatus_OK;
}

/* Get the clipping bounds of this graphics context. */
SDStatus
SDGraphics_GetClipBounds(SDGraphics   *_this,
                         SDRectangleF *clipBounds)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* get the clipping bounds */
	SDStatus_Check
		(SDRegion_GetBounds
			(_this->clip, _this, clipBounds));

	/* return successfully */
	return SDStatus_OK;
}

/* Get the visible clipping bounds of this graphics context. */
SDStatus
SDGraphics_GetVisibleClipBounds(SDGraphics   *_this,
                                SDRectangleF *bounds)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a bounds pointer */
	SDStatus_Require((bounds != 0), SDStatus_ArgumentNull);

	/* get the visible clipping bounds */
	{
		/* declarations */
		SDRectangleF visible;

		/* get the clipping bounds */
		SDStatus_Check
			(SDRegion_GetBounds
				(_this->clip, _this, bounds));

		/* get the surface bounds */
		SDStatus_Check
			(SDSurface_GetBoundsF
				(_this->surface, &visible));

		/* obtain the intersection of the visible area and the clip bounds */
		/* TODO */
	}

	/* return successfully */
	return SDStatus_OK;
}

/* Determine if the clipping region is empty. */
SDStatus
SDGraphics_IsClipEmpty(SDGraphics *_this,
                       SDBool     *empty)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have an empty flag pointer */
	SDStatus_Require((empty != 0), SDStatus_ArgumentNull);

	/* determine if the clipping region is empty */
	SDStatus_Check
		(SDRegion_IsEmpty
			(_this->clip, _this, empty));

	/* return successfully */
	return SDStatus_OK;
}

/* Determine if the visible clipping region is empty. */
SDStatus
SDGraphics_IsVisibleClipEmpty(SDGraphics *_this,
                              SDBool     *empty)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have an empty flag pointer */
	SDStatus_Require((empty != 0), SDStatus_ArgumentNull);

	/* determine if the visible clipping region is empty */
	{
		/* declarations */
		SDRectangleF visible;

		/* get the surface bounds */
		SDStatus_Check
			(SDSurface_GetBoundsF
				(_this->surface, &visible));

		/* determine if the visible area is within the clipping region */
		SDStatus_Check
			(SDRegion_IsVisibleRectangle
				(_this->clip, _this, visible, empty));
	}

	/* return successfully */
	return SDStatus_OK;
}

/* Determine if a point is within the visible clip region. */
SDStatus
SDGraphics_IsVisiblePoint(SDGraphics *_this,
                          SDPointF    point,
                          SDBool     *visible)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a visible flag pointer */
	SDStatus_Require((visible != 0), SDStatus_ArgumentNull);

	/* determine if the visible clipping region is empty */
	{
		/* declarations */
		SDRectangleF v;

		/* get the surface bounds */
		SDStatus_Check
			(SDSurface_GetBoundsF
				(_this->surface, &v));

		/* determine if the point is within the visible bounds */
		if(!(SDRectangle_ContainsPoint(v, point)))
		{
			/* set the visible flag to false */
			*visible = 0;

			/* return successfully */
			return SDStatus_OK;
		}

		/* determine if the point is within the clipping region */
		SDStatus_Check
			(SDRegion_IsVisiblePoint
				(_this->clip, _this, point, visible));
	}

	/* return successfully */
	return SDStatus_OK;
}

/* Determine if any part of a rectangle is within the visible clip region. */
SDStatus
SDGraphics_IsVisibleRectangle(SDGraphics   *_this,
                              SDRectangleF  rect,
                              SDBool       *visible)
{
	/* TODO */
	return SDStatus_NotImplemented;
}

/* Reset the clipping region. */
SDStatus
SDGraphics_ResetClip(SDGraphics *_this)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* reset the clipping region */
	SDStatus_Check
		(SDRegion_MakeInfinite
			(_this->clip));

	/* return successfully */
	return SDStatus_OK;
}

/* Set the clipping region to that of another graphics context. */
SDStatus
SDGraphics_SetClipGraphics(SDGraphics    *_this,
                           SDGraphics    *graphics,
                           SDCombineMode  combineMode)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a graphics context pointer */
	SDStatus_Require((graphics != 0), SDStatus_ArgumentNull);

	/* set the clipping region */
	SDStatus_Check
		(SDRegion_CombineRegion
			(_this->clip, graphics->clip, combineMode));

	/* return successfully */
	return SDStatus_OK;
}

/* Set the clipping region to a given path. */
SDStatus
SDGraphics_SetClipPath(SDGraphics    *_this,
                       SDPath        *path,
                       SDCombineMode  combineMode)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a path pointer */
	SDStatus_Require((path != 0), SDStatus_ArgumentNull);

	/* set the clipping region */
	SDStatus_Check
		(SDRegion_CombinePath
			(_this->clip, path, combineMode));

	/* return successfully */
	return SDStatus_OK;
}

/* Set the clipping region to a given region. */
SDStatus
SDGraphics_SetClipRegion(SDGraphics    *_this,
                         SDRegion      *region,
                         SDCombineMode  combineMode)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a region pointer */
	SDStatus_Require((region != 0), SDStatus_ArgumentNull);

	/* set the clipping region */
	SDStatus_Check
		(SDRegion_CombineRegion
			(_this->clip, region, combineMode));

	/* return successfully */
	return SDStatus_OK;
}

/* Set the clipping region to a given rectangle. */
SDStatus
SDGraphics_SetClipRectangle(SDGraphics    *_this,
                            SDRectangleF   rect,
                            SDCombineMode  combineMode)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* set the clipping region */
	SDStatus_Check
		(SDRegion_CombineRectangle
			(_this->clip, rect, combineMode));

	/* return successfully */
	return SDStatus_OK;
}

/* Translate the clipping region by a specific amount. */
SDStatus
SDGraphics_TranslateClip(SDGraphics *_this,
                         SDFloat     dx,
                         SDFloat     dy)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* translate the clipping region */
	SDStatus_Check
		(SDRegion_Translate
			(_this->clip, dx, dy));

	/* return successfully */
	return SDStatus_OK;
}

/* Get the clipping mask. */
static SDStatus
SDGraphics_GetClipMask(SDGraphics      *_this,
                       pixman_image_t **mask)
{
	/* declarations */
	SDBool gray;

	/* assertions */
	SDASSERT((_this != 0));
	SDASSERT((mask  != 0));

	/* determine if we should use gray values */
	gray = SDUtils_UseGray(_this->smoothingMode, _this->pixelOffsetMode);

	/* get the surface mask */
	SDStatus_Check
		(SDSurface_GetClipMask
			(_this->surface, mask, gray));

	/* get the clipping mask */
	SDStatus_Check
		(SDRegion_GetMask
			(_this->clip, &SDGraphicsPipeline_Device(_this->pipeline), *mask));

	/* return successfully */
	return SDStatus_OK;
}

/* Get the compositing mask. */
static SDStatus
SDGraphics_GetCompositingMask(SDGraphics      *_this,
                              pixman_image_t **mask)
{
	/* declarations */
	SDBool gray;

	/* assertions */
	SDASSERT((_this != 0));
	SDASSERT((mask  != 0));

	/* determine if we should use gray values */
	gray = SDUtils_UseGray(_this->smoothingMode, _this->pixelOffsetMode);

	/* get the surface mask */
	SDStatus_Check
		(SDSurface_GetCompositingMask
			(_this->surface, mask, gray));

	/* reset the compositing mask */
	{
		/* declarations */
		SDByte   *data;
		SDUInt32  height;
		SDUInt32  stride;

		/* TODO: is this needed? */

		/* get the mask information */
		data   = (SDByte *)pixman_image_get_data(*mask);
		height = (SDUInt32)pixman_image_get_height(*mask);
		stride = (SDUInt32)pixman_image_get_stride(*mask);

		/* reset the mask */
		SDMemSet(data, 0x00, (height * stride));
	}

	/* return successfully */
	return SDStatus_OK;
}
#endif
/******************************************************************************/












/******************************************************************************/
#define SDGraphics_PROPERTIES
#ifdef SDGraphics_PROPERTIES
/* Get the compositing mode of this graphics context. */
SDStatus
SDGraphics_GetCompositingMode(SDGraphics        *_this,
                              SDCompositingMode *compositingMode)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a compositing mode pointer */
	SDStatus_Require((compositingMode != 0), SDStatus_ArgumentNull);

	/* get the compositing mode */
	*compositingMode = _this->compositingMode;

	/* return successfully */
	return SDStatus_OK;
}

/* Set the compositing mode of this graphics context. */
SDStatus
SDGraphics_SetCompositingMode(SDGraphics        *_this,
                              SDCompositingMode  compositingMode)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* set the compositing mode */
	_this->compositingMode = compositingMode;

	/* return successfully */
	return SDStatus_OK;
}

/* Get the compositing quality of this graphics context. */
SDStatus
SDGraphics_GetCompositingQuality(SDGraphics           *_this,
                                 SDCompositingQuality *compositingQuality)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a compositing quality pointer */
	SDStatus_Require((compositingQuality != 0), SDStatus_ArgumentNull);

	/* get the compositing quality */
	*compositingQuality = _this->compositingQuality;

	/* return successfully */
	return SDStatus_OK;
}

/* Set the compositing quality of this graphics context. */
SDStatus
SDGraphics_SetCompositingQuality(SDGraphics           *_this,
                                 SDCompositingQuality  compositingQuality)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* set the compositing quality */
	_this->compositingQuality = compositingQuality;

	/* return successfully */
	return SDStatus_OK;
}

/* Get the horizontal resolution of this graphics context. */
SDStatus
SDGraphics_GetDpiX(SDGraphics *_this,
                   SDFloat    *dpiX)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a horizontal resolution pointer */
	SDStatus_Require((dpiX != 0), SDStatus_ArgumentNull);

	/* get the horizontal resolution */
#if 0
	SDStatus_Check
		(SDSurface_GetDpiX
			(_this->surface, dpiX));
#else
	*dpiX = SDGraphics_DefaultDpiX;
#endif

	/* return successfully */
	return SDStatus_OK;
}

/* Get the vertical resolution of this graphics context. */
SDStatus
SDGraphics_GetDpiY(SDGraphics *_this,
                   SDFloat    *dpiY)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a vertical resolution pointer */
	SDStatus_Require((dpiY != 0), SDStatus_ArgumentNull);

	/* get the vertical resolution */
#if 0
	SDStatus_Check
		(SDSurface_GetDpiY
			(_this->surface, dpiY));
#else
	*dpiY = SDGraphics_DefaultDpiY;
#endif

	/* return successfully */
	return SDStatus_OK;
}

/* Get the interpolation mode of this graphics context. */
SDStatus
SDGraphics_GetInterpolationMode(SDGraphics          *_this,
                                SDInterpolationMode *interpolationMode)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have an interpolation mode pointer */
	SDStatus_Require((interpolationMode != 0), SDStatus_ArgumentNull);

	/* get the interpolation mode */
	*interpolationMode = _this->interpolationMode;

	/* return successfully */
	return SDStatus_OK;
}

/* Set the interpolation mode of this graphics context. */
SDStatus
SDGraphics_SetInterpolationMode(SDGraphics          *_this,
                                SDInterpolationMode  interpolationMode)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* set the interpolation mode */
	_this->interpolationMode = interpolationMode;

	/* return successfully */
	return SDStatus_OK;
}

/* Get the page scaling factor of this graphics context. */
SDStatus
SDGraphics_GetPageScale(SDGraphics *_this,
                        SDFloat    *pageScale)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a page scaling factor pointer */
	SDStatus_Require((pageScale != 0), SDStatus_ArgumentNull);

	/* get the page scaling factor */
	*pageScale = _this->pageScale;

	/* return successfully */
	return SDStatus_OK;
}

/* Set the page scaling factor of this graphics context. */
SDStatus
SDGraphics_SetPageScale(SDGraphics *_this,
                        SDFloat     pageScale)
{
	/* declarations */
	SDFloat dpiX;
	SDFloat dpiY;

	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* set the page scaling factor */
	_this->pageScale = pageScale;

	/* get the horizontal resolution */
	SDStatus_Check
		(SDGraphics_GetDpiX
			(_this, &dpiX));

	/* get the vertical resolution */
	SDStatus_Check
		(SDGraphics_GetDpiY
			(_this, &dpiY));

	/* update the pipeline */
	SDGraphicsPipeline_SetPage
		(&(_this->pipeline), _this->pageUnit, pageScale, dpiX, dpiY);

	/* return successfully */
	return SDStatus_OK;
}

/* Get the page unit of this graphics context. */
SDStatus
SDGraphics_GetPageUnit(SDGraphics     *_this,
                       SDGraphicsUnit *pageUnit)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a page unit pointer */
	SDStatus_Require((pageUnit != 0), SDStatus_ArgumentNull);

	/* get the page unit */
	*pageUnit = _this->pageUnit;

	/* return successfully */
	return SDStatus_OK;
}

/* Set the page unit of this graphics context. */
SDStatus
SDGraphics_SetPageUnit(SDGraphics     *_this,
                       SDGraphicsUnit  pageUnit)
{
	/* declarations */
	SDFloat dpiX;
	SDFloat dpiY;

	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* set the page unit */
	_this->pageUnit = pageUnit;

	/* get the horizontal resolution */
	SDStatus_Check
		(SDGraphics_GetDpiX
			(_this, &dpiX));

	/* get the vertical resolution */
	SDStatus_Check
		(SDGraphics_GetDpiY
			(_this, &dpiY));

	/* update the pipeline */
	SDGraphicsPipeline_SetPage
		(&(_this->pipeline), pageUnit, _this->pageScale, dpiX, dpiY);

	/* return successfully */
	return SDStatus_OK;
}

/* Get the pixel offset mode of this graphics context. */
SDStatus
SDGraphics_GetPixelOffsetMode(SDGraphics        *_this,
                              SDPixelOffsetMode *pixelOffsetMode)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a pixel offset mode pointer */
	SDStatus_Require((pixelOffsetMode != 0), SDStatus_ArgumentNull);

	/* get the pixel offset mode */
	*pixelOffsetMode = _this->pixelOffsetMode;

	/* return successfully */
	return SDStatus_OK;
}

/* Set the pixel offset mode of this graphics context. */
SDStatus
SDGraphics_SetPixelOffsetMode(SDGraphics        *_this,
                              SDPixelOffsetMode  pixelOffsetMode)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* set the pixel offset mode */
	_this->pixelOffsetMode = pixelOffsetMode;

	/* return successfully */
	return SDStatus_OK;
}

/* Get the rendering origin of this graphics context. */
SDStatus
SDGraphics_GetRenderingOrigin(SDGraphics *_this,
                              SDPointI   *renderingOrigin)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a rendering origin pointer */
	SDStatus_Require((renderingOrigin != 0), SDStatus_ArgumentNull);

	/* get the rendering origin */
	*renderingOrigin = _this->renderingOrigin;

	/* return successfully */
	return SDStatus_OK;
}

/* Set the rendering origin of this graphics context. */
SDStatus
SDGraphics_SetRenderingOrigin(SDGraphics *_this,
                              SDPointI    renderingOrigin)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* set the rendering origin */
	_this->renderingOrigin = renderingOrigin;

	/* return successfully */
	return SDStatus_OK;
}

/* Get the smoothing mode of this graphics context. */
SDStatus
SDGraphics_GetSmoothingMode(SDGraphics      *_this,
                            SDSmoothingMode *smoothingMode)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a smoothing mode pointer */
	SDStatus_Require((smoothingMode != 0), SDStatus_ArgumentNull);

	/* get the smoothing mode */
	*smoothingMode = _this->smoothingMode;

	/* return successfully */
	return SDStatus_OK;
}

/* Set the smoothing mode of this graphics context. */
SDStatus
SDGraphics_SetSmoothingMode(SDGraphics      *_this,
                            SDSmoothingMode  smoothingMode)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* set the smoothing mode */
	_this->smoothingMode = smoothingMode;

	/* return successfully */
	return SDStatus_OK;
}

/* Get the text contrast of this graphics context. */
SDStatus
SDGraphics_GetTextContrast(SDGraphics *_this,
                           SDUInt32   *textContrast)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a text contrast pointer */
	SDStatus_Require((textContrast != 0), SDStatus_ArgumentNull);

	/* get the text contrast */
	*textContrast = _this->textContrast;

	/* return successfully */
	return SDStatus_OK;
}

/* Set the text contrast of this graphics context. */
SDStatus
SDGraphics_SetTextContrast(SDGraphics *_this,
                           SDUInt32    textContrast)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* set the text contrast */
	_this->textContrast = textContrast;

	/* return successfully */
	return SDStatus_OK;
}

/* Get the text rendering hint of this graphics context. */
SDStatus
SDGraphics_GetTextRenderingHint(SDGraphics          *_this,
                                SDTextRenderingHint *textRenderingHint)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a text rendering hint pointer */
	SDStatus_Require((textRenderingHint != 0), SDStatus_ArgumentNull);

	/* get the text rendering hint */
	*textRenderingHint = _this->textRenderingHint;

	/* return successfully */
	return SDStatus_OK;
}

/* Set the text rendering hint of this graphics context. */
SDStatus
SDGraphics_SetTextRenderingHint(SDGraphics          *_this,
                                SDTextRenderingHint  textRenderingHint)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* set the text rendering hint */
	_this->textRenderingHint = textRenderingHint;

	/* return successfully */
	return SDStatus_OK;
}
#endif
/******************************************************************************/












/******************************************************************************/
#define SDGraphics_IMAGING
#ifdef SDGraphics_IMAGING

static const pixman_transform_t SDPixmanTransform_Identity =
{{
	{ SDFixed_One,  SDFixed_Zero, SDFixed_Zero },
	{ SDFixed_Zero, SDFixed_One,  SDFixed_Zero },
	{ SDFixed_Zero, SDFixed_Zero, SDFixed_One  }
}};

/* Draw an xbm glyph. */
SDStatus
SDGraphics_DrawXBM(SDGraphics   *_this,
                   SDByte       *bits,
                   SDUInt32      count,
                   SDRectangleF  dst,
                   SDColor       color)
{
	/* TODO */
	return SDStatus_NotImplemented;
}

/* Draw an image. */
SDStatus
SDGraphics_DrawImage(SDGraphics *_this,
                     SDImage    *image,
                     SDFloat     x,
                     SDFloat     y)
{
	/* declarations */
	SDSizeF size;

	/* get the size of the image */
	SDStatus_Check
		(SDImage_GetPhysicalDimension
			(image, &size));

	/* draw the image */
	return
		SDGraphics_DrawImageRect
			(_this, image, x, y, SDSize_Width(size), SDSize_Height(size));
}

/* Draw an image. */
SDStatus
SDGraphics_DrawImageRect(SDGraphics *_this,
                         SDImage    *image,
                         SDFloat     x,
                         SDFloat     y,
                         SDFloat     width,
                         SDFloat     height)
{
	/* declarations */
	SDStatus status;

	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have an image pointer */
	SDStatus_Require((image != 0), SDStatus_ArgumentNull);

	/* paint to the surface synchronously */
	SDSurface_Lock(_this->surface);
	{
		/* declarations */
		pixman_image_t *mask;

		/* get the clip mask */
		status =
			SDGraphics_GetClipMask
				(_this, &mask);

		/* handle masking failures */
		if(status != SDStatus_OK)
		{
			SDSurface_Unlock(_this->surface);
			return status;
		}

		/* draw the image synchronously */
		SDMutex_Lock(image->lock);
		{
			/* declarations */
			pixman_transform_t transform;

			/* get the transformation */
			transform =
				SDUtils_ToPixmanTransform
					(&(SDGraphicsPipeline_DeviceInverse(_this->pipeline)));

			/* set the image transformation */
			if(pixman_image_set_transform(image->image, &transform))
			{
				SDMutex_Unlock(image->lock);
				SDSurface_Unlock(_this->surface);
				return SDStatus_OutOfMemory;
			}

			/* composite the image */
			status =
				SDSurface_Composite
					(_this->surface, x, y, width, height, image->image, mask,
					 _this->interpolationMode, _this->compositingMode);

			/* reset the image transformation */
			pixman_image_set_transform
				(image->image,
				 ((pixman_transform_t *)&SDPixmanTransform_Identity));
		}
		SDMutex_Unlock(image->lock);
	}
	SDSurface_Unlock(_this->surface);

	/* return status */
	return status;
}


/* Draw an image. */
SDStatus
SDGraphics_DrawImagePoints(SDGraphics *_this,
                           SDImage    *image,
                           SDPointF   *dst,
                           SDUInt32    count)
{
	/* TODO */
	return SDStatus_NotImplemented;
}

/* Draw an image. */
SDStatus
SDGraphics_DrawImageRectPoints(SDGraphics     *_this,
                               SDImage        *image,
                               SDPointF        dst,
                               SDRectangleF    src,
                               SDGraphicsUnit  srcUnit)
{
	/* TODO */
	return SDStatus_NotImplemented;
}

/* Draw an image. */
SDStatus
SDGraphics_DrawImageRectPointsAttributes(SDGraphics           *_this,
                                         SDImage              *image,
                                         SDPointF             *dst,
                                         SDUInt32              count,
                                         SDRectangleF          src,
                                         SDGraphicsUnit        srcUnit,
                                         SDImageAttributes    *atts)
{
	/* TODO */
	return SDStatus_NotImplemented;
}

/* Draw an image. */
SDStatus
SDGraphics_DrawImageRectRectAttributes(SDGraphics           *_this,
                                       SDImage              *image,
                                       SDRectangleF          dst,
                                       SDRectangleF          src,
                                       SDGraphicsUnit        srcUnit,
                                       SDImageAttributes    *atts)
{
	/* TODO */
	return SDStatus_NotImplemented;
}
#endif
/******************************************************************************/












/******************************************************************************/
#define SDGraphics_DRAWING
#ifdef SDGraphics_DRAWING

/* Composite the image. */
static SDStatus
SDGraphics_Composite(SDGraphics     *_this,
                     SDPattern      *pattern,
                     pixman_image_t *mask)
{
	/* declarations */
	SDStatus  status;
	SDUInt32  width;
	SDUInt32  height;

	/* assertions */
	SDASSERT((_this   != 0));
	SDASSERT((pattern != 0));
	SDASSERT((mask    != 0));

	/* apply transformation, as needed */
	if(pattern->transform != 0)
	{
		/* declarations */
		SDAffineTransformF affine;
		pixman_transform_t transform;

		/* get the pattern transformation */
		affine = *(pattern->transform);

		/* apply the device transformation */
		SDAffineTransformF_Multiply
			(&affine, &SDGraphicsPipeline_DeviceInverse(_this->pipeline),
			 SDMatrixOrder_Append);

		/* get the pixman transformation */
		transform = SDUtils_ToPixmanTransform(&affine);

		/* set the image transformation */
		pixman_image_set_transform(pattern->image, &transform);
	}

	/* get the source dimensions */
	width = pixman_image_get_width(pattern->image);
	height = pixman_image_get_height(pattern->image);

	/* composite the image */
	status =
		SDSurface_Composite
			(_this->surface, 0, 0, width, height, pattern->image, mask,
			 _this->interpolationMode, _this->compositingMode);

	/* reset transformation, as needed */
	if(pattern->transform != 0)
	{
		pixman_image_set_transform
			(pattern->image,
			 ((pixman_transform_t *)&SDPixmanTransform_Identity));
	}

	/* return status */
	return status;
}

/* Fill the given path. */
static SDStatus
SDGraphics_Fill2(SDGraphics *_this,
                 SDPath     *path,
                 SDPattern  *pattern)
{
	/* declarations */
	SDStatus status;

	/* assertions */
	SDASSERT((_this   != 0));
	SDASSERT((path    != 0));
	SDASSERT((pattern != 0));

	/* fill the given path */
	{
		/* declarations */
		SDTrapezoids    trapezoids;
		pixman_image_t *clip;
		pixman_image_t *mask;

		/* get the clipping mask */
		SDStatus_Check
			(SDGraphics_GetClipMask
				(_this, &clip));

		/* get the compositing mask */
		SDStatus_Check
			(SDGraphics_GetCompositingMask
				(_this, &mask));

		/* initialize the trapezoids */
		SDTrapezoids_Initialize(&trapezoids);

		/* fill the path */
		status =
			SDPath_Fill
				(path, &trapezoids);

		/* handle fill failures */
		if(status != SDStatus_OK)
		{
			SDTrapezoids_Finalize(&trapezoids);
			return status;
		}

		/* composite the trapezoids */
		pixman_composite_trapezoids
			(PIXMAN_OPERATOR_IN, clip, mask, 0, 0,
			 ((pixman_trapezoid_t *)SDTrapezoids_Trapezoids(trapezoids)),
			 SDTrapezoids_Count(trapezoids));

		/* finalize the trapezoids */
		SDTrapezoids_Finalize(&trapezoids);

		/* composite the image */
		status = SDGraphics_Composite(_this, pattern, mask);
	}

	/* return status */
	return status;
}

/* Stroke the current path. */
static SDStatus
SDGraphics_Stroke(SDGraphics *_this,
                  SDPen      *pen)
{
	/* assertions */
	SDASSERT((_this != 0));
	SDASSERT((pen   != 0));

	/* perform the stroke */
	{
		/* declarations */
		SDStatus  status;
		SDStroker stroker;
		SDPattern pattern;

		/* get the source pattern */
		SDStatus_Check
			(SDPen_GetPattern
				(pen, &pattern));

		/* reset the stroke path */
		SDStatus_Check
			(SDPath_Reset
				(_this->stroke));

		/* initialize the stroker */
		SDStatus_Check
			(SDStroker_Initialize
				(&stroker, pen, &SDGraphicsPipeline_Device(_this->pipeline)));

		/* stroke the path */
		status =
			SDPath_Stroke
				(_this->path, _this->stroke, &stroker);

		/* finalize the stroker */
		SDStroker_Finalize(&stroker);

		/* handle stroke failures */
		SDStatus_Check(status);

		/* fill the path */
		SDStatus_Check
			(SDGraphics_Fill2
				(_this, _this->stroke, &pattern));
	}

	/* return successfully */
	return SDStatus_OK;
}

/* Draw an arc. */
SDStatus
SDGraphics_DrawArc(SDGraphics   *_this,
                   SDPen        *pen,
                   SDRectangleF  rect,
                   SDFloat       startAngle,
                   SDFloat       sweepAngle)
{
	/* declarations */
	SDStatus status;

	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a pen pointer */
	SDStatus_Require((pen != 0), SDStatus_ArgumentNull);

	/* perform the stroke synchronously */
	SDSurface_Lock(_this->surface);
	{
		/* reset the path */
		status = SDPath_Reset(_this->path);

		/* handle reset failures */
		if(status != SDStatus_OK)
		{
			SDSurface_Unlock(_this->surface);
			return status;
		}

		/* add the arc to the path */
		status =
			SDPath_AddArc
				(_this->path,
				 SDRectangle_X(rect),
				 SDRectangle_Y(rect),
				 SDRectangle_Width(rect),
				 SDRectangle_Height(rect),
				 startAngle, sweepAngle);

		/* handle pathing failures */
		if(status != SDStatus_OK)
		{
			SDSurface_Unlock(_this->surface);
			return status;
		}

		/* stroke the path */
		status = SDGraphics_Stroke(_this, pen);
	}
	SDSurface_Unlock(_this->surface);

	/* return status */
	return status;
}

/* Draw a Bezier spline. */
SDStatus
SDGraphics_DrawBezier(SDGraphics *_this,
                      SDPen      *pen,
                      SDPointF    a,
                      SDPointF    b,
                      SDPointF    c,
                      SDPointF    d)
{
	/* declarations */
	SDStatus status;

	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a pen pointer */
	SDStatus_Require((pen != 0), SDStatus_ArgumentNull);

	/* perform the stroke synchronously */
	SDSurface_Lock(_this->surface);
	{
		/* reset the path */
		status = SDPath_Reset(_this->path);

		/* handle reset failures */
		if(status != SDStatus_OK)
		{
			SDSurface_Unlock(_this->surface);
			return status;
		}

		/* add the bezier to the path */
		status =
			SDPath_AddBezier
				(_this->path,
				 SDPoint_X(a), SDPoint_Y(a),
				 SDPoint_X(b), SDPoint_Y(b),
				 SDPoint_X(c), SDPoint_Y(c),
				 SDPoint_X(d), SDPoint_Y(d));

		/* handle pathing failures */
		if(status != SDStatus_OK)
		{
			SDSurface_Unlock(_this->surface);
			return status;
		}

		/* stroke the path */
		status = SDGraphics_Stroke(_this, pen);
	}
	SDSurface_Unlock(_this->surface);

	/* return status */
	return status;
}

/* Draw a series of Bezier splines. */
SDStatus
SDGraphics_DrawBeziers(SDGraphics *_this,
                       SDPen      *pen,
                       SDPointF   *points,
                       SDUInt32    count)
{
	/* declarations */
	SDStatus status;

	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a pen pointer */
	SDStatus_Require((pen != 0), SDStatus_ArgumentNull);

	/* ensure we have a point list */
	SDStatus_Require((points != 0), SDStatus_ArgumentNull);

	/* perform the stroke synchronously */
	SDSurface_Lock(_this->surface);
	{
		/* reset the path */
		status = SDPath_Reset(_this->path);

		/* handle reset failures */
		if(status != SDStatus_OK)
		{
			SDSurface_Unlock(_this->surface);
			return status;
		}

		/* add the beziers to the path */
		status =
			SDPath_AddBeziers
				(_this->path, points, count);

		/* handle pathing failures */
		if(status != SDStatus_OK)
		{
			SDSurface_Unlock(_this->surface);
			return status;
		}

		/* stroke the path */
		status = SDGraphics_Stroke(_this, pen);
	}
	SDSurface_Unlock(_this->surface);

	/* return status */
	return status;
}

/* Draw a closed cardinal spline. */
SDStatus
SDGraphics_DrawClosedCurve(SDGraphics *_this,
                           SDPen      *pen,
                           SDPointF   *points,
                           SDUInt32    count,
                           SDFloat     tension)
{
	/* declarations */
	SDStatus status;

	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a pen pointer */
	SDStatus_Require((pen != 0), SDStatus_ArgumentNull);

	/* ensure we have a point list */
	SDStatus_Require((points != 0), SDStatus_ArgumentNull);

	/* perform the stroke synchronously */
	SDSurface_Lock(_this->surface);
	{
		/* reset the path */
		status = SDPath_Reset(_this->path);

		/* handle reset failures */
		if(status != SDStatus_OK)
		{
			SDSurface_Unlock(_this->surface);
			return status;
		}

		/* add the cardinal curve to the path */
		status =
			SDPath_AddClosedCardinalCurve
				(_this->path, points, count, tension);

		/* handle pathing failures */
		if(status != SDStatus_OK)
		{
			SDSurface_Unlock(_this->surface);
			return status;
		}

		/* stroke the path */
		status = SDGraphics_Stroke(_this, pen);
	}
	SDSurface_Unlock(_this->surface);

	/* return status */
	return status;
}

/* Draw a cardinal spline. */
SDStatus
SDGraphics_DrawCurve(SDGraphics *_this,
                     SDPen      *pen,
                     SDPointF   *points,
                     SDUInt32    count,
                     SDUInt32    offset,
                     SDUInt32    numberOfSegments,
                     SDFloat     tension)
{
	/* declarations */
	SDStatus status;

	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a pen pointer */
	SDStatus_Require((pen != 0), SDStatus_ArgumentNull);

	/* ensure we have a point list */
	SDStatus_Require((points != 0), SDStatus_ArgumentNull);

	/* perform the stroke synchronously */
	SDSurface_Lock(_this->surface);
	{
		/* reset the path */
		status = SDPath_Reset(_this->path);

		/* handle reset failures */
		if(status != SDStatus_OK)
		{
			SDSurface_Unlock(_this->surface);
			return status;
		}

		/* add the cardinal curve to the path */
		status =
			SDPath_AddCardinalCurve
				(_this->path, points, count, offset, numberOfSegments, tension);

		/* handle pathing failures */
		if(status != SDStatus_OK)
		{
			SDSurface_Unlock(_this->surface);
			return status;
		}

		/* stroke the path */
		status = SDGraphics_Stroke(_this, pen);
	}
	SDSurface_Unlock(_this->surface);

	/* return status */
	return status;
}

/* Draw an ellipse. */
SDStatus
SDGraphics_DrawEllipse(SDGraphics   *_this,
                       SDPen        *pen,
                       SDRectangleF  rect)
{
	/* declarations */
	SDStatus status;

	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a pen pointer */
	SDStatus_Require((pen != 0), SDStatus_ArgumentNull);

	/* perform the stroke synchronously */
	SDSurface_Lock(_this->surface);
	{
		/* reset the path */
		status = SDPath_Reset(_this->path);

		/* handle reset failures */
		if(status != SDStatus_OK)
		{
			SDSurface_Unlock(_this->surface);
			return status;
		}

		/* add the ellipse to the path */
		status =
			SDPath_AddEllipse
				(_this->path,
				 SDRectangle_X(rect),
				 SDRectangle_Y(rect),
				 SDRectangle_Width(rect),
				 SDRectangle_Height(rect));

		/* handle pathing failures */
		if(status != SDStatus_OK)
		{
			SDSurface_Unlock(_this->surface);
			return status;
		}

		/* stroke the path */
		status = SDGraphics_Stroke(_this, pen);
	}
	SDSurface_Unlock(_this->surface);

	/* return status */
	return status;
}

/* Draw a line between two points. */
SDStatus
SDGraphics_DrawLine(SDGraphics *_this,
                    SDPen      *pen,
                    SDPointF    start,
                    SDPointF    end)
{
	/* declarations */
	SDStatus status;

	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a pen pointer */
	SDStatus_Require((pen != 0), SDStatus_ArgumentNull);

	/* perform the stroke synchronously */
	SDSurface_Lock(_this->surface);
	{
		/* reset the path */
		status = SDPath_Reset(_this->path);

		/* handle reset failures */
		if(status != SDStatus_OK)
		{
			SDSurface_Unlock(_this->surface);
			return status;
		}

		/* add the line to the path */
		status =
			SDPath_AddLine
				(_this->path,
				 SDPoint_X(start), SDPoint_Y(start),
				 SDPoint_X(end),   SDPoint_Y(end));

		/* handle pathing failures */
		if(status != SDStatus_OK)
		{
			SDSurface_Unlock(_this->surface);
			return status;
		}

		/* stroke the path */
		status = SDGraphics_Stroke(_this, pen);
	}
	SDSurface_Unlock(_this->surface);

	/* return status */
	return status;
}

/* Draw a series of connected line segments. */
SDStatus
SDGraphics_DrawLines(SDGraphics *_this,
                     SDPen      *pen,
                     SDPointF   *points,
                     SDUInt32    count)
{
	/* declarations */
	SDStatus status;

	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a pen pointer */
	SDStatus_Require((pen != 0), SDStatus_ArgumentNull);

	/* ensure we have a point list */
	SDStatus_Require((points != 0), SDStatus_ArgumentNull);

	/* perform the stroke synchronously */
	SDSurface_Lock(_this->surface);
	{
		/* reset the path */
		status = SDPath_Reset(_this->path);

		/* handle reset failures */
		if(status != SDStatus_OK)
		{
			SDSurface_Unlock(_this->surface);
			return status;
		}

		/* add the lines to the path */
		status =
			SDPath_AddLines
				(_this->path, points, count);

		/* handle pathing failures */
		if(status != SDStatus_OK)
		{
			SDSurface_Unlock(_this->surface);
			return status;
		}

		/* stroke the path */
		status = SDGraphics_Stroke(_this, pen);
	}
	SDSurface_Unlock(_this->surface);

	/* return status */
	return status;
}

/* Draw a path object. */
SDStatus
SDGraphics_DrawPath(SDGraphics *_this,
                    SDPen      *pen,
                    SDPath     *path)
{
	/* declarations */
	SDStatus status;

	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a pen pointer */
	SDStatus_Require((pen != 0), SDStatus_ArgumentNull);

	/* ensure we have a path pointer */
	SDStatus_Require((path != 0), SDStatus_ArgumentNull);

	/* perform the stroke synchronously */
	SDSurface_Lock(_this->surface);
	{
		/* reset the path */
		status = SDPath_Reset(_this->path);

		/* handle reset failures */
		if(status != SDStatus_OK)
		{
			SDSurface_Unlock(_this->surface);
			return status;
		}

		/* add the path to the path */
		status =
			SDPath_AddPath
				(_this->path, path, 0);

		/* handle pathing failures */
		if(status != SDStatus_OK)
		{
			SDSurface_Unlock(_this->surface);
			return status;
		}

		/* stroke the path */
		status = SDGraphics_Stroke(_this, pen);
	}
	SDSurface_Unlock(_this->surface);

	/* return status */
	return status;
}

/* Draw a pie shape. */
SDStatus
SDGraphics_DrawPie(SDGraphics   *_this,
                   SDPen        *pen,
                   SDRectangleF  rect,
                   SDFloat       startAngle,
                   SDFloat       sweepAngle)
{
	/* declarations */
	SDStatus status;

	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a pen pointer */
	SDStatus_Require((pen != 0), SDStatus_ArgumentNull);

	/* perform the stroke synchronously */
	SDSurface_Lock(_this->surface);
	{
		/* reset the path */
		status = SDPath_Reset(_this->path);

		/* handle reset failures */
		if(status != SDStatus_OK)
		{
			SDSurface_Unlock(_this->surface);
			return status;
		}

		/* add the pie to the path */
		status =
			SDPath_AddPie
				(_this->path,
				 SDRectangle_X(rect),
				 SDRectangle_Y(rect),
				 SDRectangle_Width(rect),
				 SDRectangle_Height(rect),
				 startAngle, sweepAngle);

		/* handle pathing failures */
		if(status != SDStatus_OK)
		{
			SDSurface_Unlock(_this->surface);
			return status;
		}

		/* stroke the path */
		status = SDGraphics_Stroke(_this, pen);
	}
	SDSurface_Unlock(_this->surface);

	/* return status */
	return status;
}

/* Draw a polygon. */
SDStatus
SDGraphics_DrawPolygon(SDGraphics *_this,
                       SDPen      *pen,
                       SDPointF   *points,
                       SDUInt32    count)
{
	/* declarations */
	SDStatus status;

	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a pen pointer */
	SDStatus_Require((pen != 0), SDStatus_ArgumentNull);

	/* ensure we have a point list */
	SDStatus_Require((points != 0), SDStatus_ArgumentNull);

	/* perform the stroke synchronously */
	SDSurface_Lock(_this->surface);
	{
		/* reset the path */
		status = SDPath_Reset(_this->path);

		/* handle reset failures */
		if(status != SDStatus_OK)
		{
			SDSurface_Unlock(_this->surface);
			return status;
		}

		/* add the polygon to the path */
		status =
			SDPath_AddPolygon
				(_this->path, points, count);

		/* handle pathing failures */
		if(status != SDStatus_OK)
		{
			SDSurface_Unlock(_this->surface);
			return status;
		}

		/* stroke the path */
		status = SDGraphics_Stroke(_this, pen);
	}
	SDSurface_Unlock(_this->surface);

	/* return status */
	return status;
}

/* Draw a rectangle. */
SDStatus
SDGraphics_DrawRectangle(SDGraphics   *_this,
                         SDPen        *pen,
                         SDRectangleF  rect)
{
	/* declarations */
	SDStatus status;

	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a pen pointer */
	SDStatus_Require((pen != 0), SDStatus_ArgumentNull);

	/* perform the stroke synchronously */
	SDSurface_Lock(_this->surface);
	{
		/* reset the path */
		status = SDPath_Reset(_this->path);

		/* handle reset failures */
		if(status != SDStatus_OK)
		{
			SDSurface_Unlock(_this->surface);
			return status;
		}

		/* add the rectangle to the path */
		status =
			SDPath_AddRectangle
				(_this->path, 
				 SDRectangle_X(rect),
				 SDRectangle_Y(rect),
				 SDRectangle_Width(rect),
				 SDRectangle_Height(rect));

		/* handle pathing failures */
		if(status != SDStatus_OK)
		{
			SDSurface_Unlock(_this->surface);
			return status;
		}

		/* stroke the path */
		status = SDGraphics_Stroke(_this, pen);
	}
	SDSurface_Unlock(_this->surface);

	/* return status */
	return status;
}

/* Draw a series of rectangles. */
SDStatus
SDGraphics_DrawRectangles(SDGraphics   *_this,
                          SDPen        *pen,
                          SDRectangleF *rects,
                          SDUInt32      count)
{
	/* declarations */
	SDStatus status;

	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a pen pointer */
	SDStatus_Require((pen != 0), SDStatus_ArgumentNull);

	/* ensure we have a rectangle list */
	SDStatus_Require((rects != 0), SDStatus_ArgumentNull);

	/* perform the stroke synchronously */
	SDSurface_Lock(_this->surface);
	{
		/* reset the path */
		status = SDPath_Reset(_this->path);

		/* handle reset failures */
		if(status != SDStatus_OK)
		{
			SDSurface_Unlock(_this->surface);
			return status;
		}

		/* add the rectangles to the path */
		status =
			SDPath_AddRectangles
				(_this->path, rects, count);

		/* handle pathing failures */
		if(status != SDStatus_OK)
		{
			SDSurface_Unlock(_this->surface);
			return status;
		}

		/* stroke the path */
		status = SDGraphics_Stroke(_this, pen);
	}
	SDSurface_Unlock(_this->surface);

	/* return status */
	return status;
}
#endif
/******************************************************************************/












/******************************************************************************/
#define SDGraphics_FILLING
#ifdef SDGraphics_FILLING
/* Fill the current path. */
static SDStatus
SDGraphics_Fill(SDGraphics *_this,
                SDBrush    *brush,
                SDFillMode  fillMode)
{
	/* assertions */
	SDASSERT((_this != 0));
	SDASSERT((brush != 0));

	/* perform the stroke */
	{
		/* declarations */
		SDPattern pattern;

		/* get the source pattern */
		SDStatus_Check
			(SDBrush_GetPattern
				(brush, &pattern));

		/* set the fill mode */
		SDStatus_Check
			(SDPath_SetFillMode
				(_this->path, fillMode));

		/* fill the path */
		SDStatus_Check
			(SDGraphics_Fill2
				(_this, _this->path, &pattern));
	}

	/* return successfully */
	return SDStatus_OK;
}

/* Clear the entire drawing surface. */
SDStatus
SDGraphics_Clear(SDGraphics *_this,
                 SDColor     color)
{
	/* declarations */
	SDStatus status;

	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* perform the clear synchronously */
	SDSurface_Lock(_this->surface);
	{
		/* clear the surface */
		status =
			SDSurface_Clear
				(_this->surface, color);
	}
	SDSurface_Unlock(_this->surface);

	/* return status */
	return status;
}

/* Fill a closed cardinal spline. */
SDStatus
SDGraphics_FillClosedCurve(SDGraphics *_this,
                           SDBrush    *brush,
                           SDPointF   *points,
                           SDUInt32    count,
                           SDFloat     tension,
                           SDFillMode  fillMode)
{
	/* declarations */
	SDStatus status;

	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a brush pointer */
	SDStatus_Require((brush != 0), SDStatus_ArgumentNull);

	/* ensure we have a point list */
	SDStatus_Require((points != 0), SDStatus_ArgumentNull);

	/* perform the fill synchronously */
	SDSurface_Lock(_this->surface);
	{
		/* reset the path */
		status = SDPath_Reset(_this->path);

		/* handle reset failures */
		if(status != SDStatus_OK)
		{
			SDSurface_Unlock(_this->surface);
			return status;
		}

		/* add the cardinal curve to the path */
		status =
			SDPath_AddClosedCardinalCurve
				(_this->path, points, count, tension);

		/* handle pathing failures */
		if(status != SDStatus_OK)
		{
			SDSurface_Unlock(_this->surface);
			return status;
		}

		/* fill the path */
		status =
			SDGraphics_Fill
				(_this, brush, fillMode);
	}
	SDSurface_Unlock(_this->surface);

	/* return status */
	return status;
}

/* Fill an ellipse. */
SDStatus
SDGraphics_FillEllipse(SDGraphics   *_this,
                       SDBrush      *brush,
                       SDRectangleF  rect)
{
	/* declarations */
	SDStatus status;

	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a brush pointer */
	SDStatus_Require((brush != 0), SDStatus_ArgumentNull);

	/* perform the fill synchronously */
	SDSurface_Lock(_this->surface);
	{
		/* reset the path */
		status = SDPath_Reset(_this->path);

		/* handle reset failures */
		if(status != SDStatus_OK)
		{
			SDSurface_Unlock(_this->surface);
			return status;
		}

		/* add the ellipse to the path */
		status =
			SDPath_AddEllipse
				(_this->path,
				 SDRectangle_X(rect),
				 SDRectangle_Y(rect),
				 SDRectangle_Width(rect),
				 SDRectangle_Height(rect));

		/* handle pathing failures */
		if(status != SDStatus_OK)
		{
			SDSurface_Unlock(_this->surface);
			return status;
		}

		/* fill the path */
		status =
			SDGraphics_Fill
				(_this, brush, SDFillMode_Alternate);
	}
	SDSurface_Unlock(_this->surface);

	/* return status */
	return status;
}

/* Fill the interior of a path. */
SDStatus
SDGraphics_FillPath(SDGraphics *_this,
                    SDBrush    *brush,
                    SDPath     *path)
{
	/* declarations */
	SDStatus status;

	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a brush pointer */
	SDStatus_Require((brush != 0), SDStatus_ArgumentNull);

	/* ensure we have a path pointer */
	SDStatus_Require((path != 0), SDStatus_ArgumentNull);

	/* perform the fill synchronously */
	SDSurface_Lock(_this->surface);
	{
		/* reset the path */
		status = SDPath_Reset(_this->path);

		/* handle reset failures */
		if(status != SDStatus_OK)
		{
			SDSurface_Unlock(_this->surface);
			return status;
		}

		/* add the path to the path */
		status =
			SDPath_AddPath
				(_this->path, path, 0);

		/* handle pathing failures */
		if(status != SDStatus_OK)
		{
			SDSurface_Unlock(_this->surface);
			return status;
		}

		/* fill the path */
		status =
			SDGraphics_Fill
				(_this, brush, SDFillMode_Alternate);
	}
	SDSurface_Unlock(_this->surface);

	/* return status */
	return status;
}

/* Fill a pie shape. */
SDStatus
SDGraphics_FillPie(SDGraphics   *_this,
                   SDBrush      *brush,
                   SDRectangleF  rect,
                   SDFloat       startAngle,
                   SDFloat       sweepAngle)
{
	/* declarations */
	SDStatus status;

	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a brush pointer */
	SDStatus_Require((brush != 0), SDStatus_ArgumentNull);

	/* perform the fill synchronously */
	SDSurface_Lock(_this->surface);
	{
		/* reset the path */
		status = SDPath_Reset(_this->path);

		/* handle reset failures */
		if(status != SDStatus_OK)
		{
			SDSurface_Unlock(_this->surface);
			return status;
		}

		/* add the pie to the path */
		status =
			SDPath_AddPie
				(_this->path,
				 SDRectangle_X(rect),
				 SDRectangle_Y(rect),
				 SDRectangle_Width(rect),
				 SDRectangle_Height(rect),
				 startAngle, sweepAngle);

		/* handle pathing failures */
		if(status != SDStatus_OK)
		{
			SDSurface_Unlock(_this->surface);
			return status;
		}

		/* fill the path */
		status =
			SDGraphics_Fill
				(_this, brush, SDFillMode_Alternate);
	}
	SDSurface_Unlock(_this->surface);

	/* return status */
	return status;
}

/* Fill a polygon. */
SDStatus
SDGraphics_FillPolygon(SDGraphics *_this,
                       SDBrush    *brush,
                       SDPointF   *points,
                       SDUInt32    count,
                       SDFillMode  fillMode)
{
	/* declarations */
	SDStatus status;

	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a brush pointer */
	SDStatus_Require((brush != 0), SDStatus_ArgumentNull);

	/* ensure we have a point list */
	SDStatus_Require((points != 0), SDStatus_ArgumentNull);

	/* perform the fill synchronously */
	SDSurface_Lock(_this->surface);
	{
		/* reset the path */
		status = SDPath_Reset(_this->path);

		/* handle reset failures */
		if(status != SDStatus_OK)
		{
			SDSurface_Unlock(_this->surface);
			return status;
		}

		/* add the polygon to the path */
		status =
			SDPath_AddPolygon
				(_this->path, points, count);

		/* handle pathing failures */
		if(status != SDStatus_OK)
		{
			SDSurface_Unlock(_this->surface);
			return status;
		}

		/* fill the path */
		status =
			SDGraphics_Fill
				(_this, brush, fillMode);
	}
	SDSurface_Unlock(_this->surface);

	/* return status */
	return status;
}

/* Fill a rectangle. */
SDStatus
SDGraphics_FillRectangle(SDGraphics   *_this,
                         SDBrush      *brush,
                         SDRectangleF  rect)
{
	/* declarations */
	SDStatus status;

	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a brush pointer */
	SDStatus_Require((brush != 0), SDStatus_ArgumentNull);

	/* perform the fill synchronously */
	SDSurface_Lock(_this->surface);
	{
		/* reset the path */
		status = SDPath_Reset(_this->path);

		/* handle reset failures */
		if(status != SDStatus_OK)
		{
			SDSurface_Unlock(_this->surface);
			return status;
		}

		/* add the rectangle to the path */
		status =
			SDPath_AddRectangle
				(_this->path, 
				 SDRectangle_X(rect),
				 SDRectangle_Y(rect),
				 SDRectangle_Width(rect),
				 SDRectangle_Height(rect));

		/* handle pathing failures */
		if(status != SDStatus_OK)
		{
			SDSurface_Unlock(_this->surface);
			return status;
		}

		/* fill the path */
		status =
			SDGraphics_Fill
				(_this, brush, SDFillMode_Alternate);
	}
	SDSurface_Unlock(_this->surface);

	/* return status */
	return status;
}

/* Fill a series of rectangles. */
SDStatus
SDGraphics_FillRectangles(SDGraphics   *_this,
                          SDBrush      *brush,
                          SDRectangleF *rects,
                          SDUInt32      count)
{
	/* declarations */
	SDStatus status;

	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a brush pointer */
	SDStatus_Require((brush != 0), SDStatus_ArgumentNull);

	/* ensure we have a rectangle list */
	SDStatus_Require((rects != 0), SDStatus_ArgumentNull);

	/* perform the fill synchronously */
	SDSurface_Lock(_this->surface);
	{
		/* reset the path */
		status = SDPath_Reset(_this->path);

		/* handle reset failures */
		if(status != SDStatus_OK)
		{
			SDSurface_Unlock(_this->surface);
			return status;
		}

		/* add the rectangles to the path */
		status =
			SDPath_AddRectangles
				(_this->path, rects, count);

		/* handle pathing failures */
		if(status != SDStatus_OK)
		{
			SDSurface_Unlock(_this->surface);
			return status;
		}

		/* fill the path */
		status =
			SDGraphics_Fill
				(_this, brush, SDFillMode_Alternate);
	}
	SDSurface_Unlock(_this->surface);

	/* return status */
	return status;
}

/* Fill a region. */
SDStatus
SDGraphics_FillRegion(SDGraphics *_this,
                      SDBrush    *brush,
                      SDRegion   *region)
{
	/* declarations */
	SDStatus status;

	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a brush pointer */
	SDStatus_Require((brush != 0), SDStatus_ArgumentNull);

	/* ensure we have a region pointer */
	SDStatus_Require((region != 0), SDStatus_ArgumentNull);

	/* perform the fill synchronously */
	SDSurface_Lock(_this->surface);
	{
		/* declarations */
		pixman_image_t *clip;
		pixman_image_t *mask;
		SDPattern       pattern;
		SDUInt32        w;
		SDUInt32        h;
		SDBool          gray;

		/* determine if we should use gray values */
		gray = SDUtils_UseGray(_this->smoothingMode, _this->pixelOffsetMode);

		/* get the clip mask */
		status =
			SDSurface_GetClipMask
				(_this->surface, &clip, gray);

		/* handle clip masking failures */
		if(status != SDStatus_OK)
		{
			SDSurface_Unlock(_this->surface);
			return status;
		}

		/* get the compositing mask */
		status =
			SDSurface_GetCompositingMask
				(_this->surface, &mask, gray);

		/* handle composite masking failures */
		if(status != SDStatus_OK)
		{
			SDSurface_Unlock(_this->surface);
			return status;
		}

		/* get the region mask */
		status =
			SDRegion_GetMask
				(region, &SDGraphicsPipeline_Device(_this->pipeline), mask);

		/* handle region masking failures */
		if(status != SDStatus_OK)
		{
			SDSurface_Unlock(_this->surface);
			return status;
		}

		/* get the width and height */
		w = (SDUInt32)pixman_image_get_width(mask);
		h = (SDUInt32)pixman_image_get_width(mask);

		/* clip the region mask */
		pixman_composite
			(PIXMAN_OPERATOR_IN_REVERSE, clip, 0, mask, 0, 0, 0, 0, 0, 0, w, h);

		/* get the pattern */
		status = SDBrush_GetPattern(brush, &pattern);

		/* handle pattern failures */
		if(status != SDStatus_OK)
		{
			SDSurface_Unlock(_this->surface);
			return status;
		}

		/* composite the region */
		status = SDGraphics_Composite(_this, &pattern, mask);
	}
	SDSurface_Unlock(_this->surface);

	/* return status */
	return status;
}
#endif
/******************************************************************************/











/******************************************************************************/
#define SDGraphics_STACK
#ifdef SDGraphics_STACK
/* Save the current contents of the graphics context in a container. */
SDStatus
SDGraphics_BeginContainer(SDGraphics          *_this,
                          SDGraphicsContainer *container)
{
	/* TODO */
	return SDStatus_NotImplemented;
}
SDStatus
SDGraphics_BeginContainer2(SDGraphics          *_this,
                           SDRectangleF         dst,
                           SDRectangleF         src,
                           SDGraphicsUnit       unit,
                           SDGraphicsContainer *container)
{
	/* TODO */
	return SDStatus_NotImplemented;
}

/* Reset the graphics state back to a previous container level. */
SDStatus
SDGraphics_EndContainer(SDGraphics          *_this,
                        SDGraphicsContainer  container)
{
	/* TODO */
	return SDStatus_NotImplemented;
}

/* Restore to a previous save point. */
SDStatus
SDGraphics_Restore(SDGraphics *_this,
                   SDUInt32    state)
{
	/* TODO */
	return SDStatus_NotImplemented;
}

/* Save the current graphics state. */
SDStatus
SDGraphics_Save(SDGraphics *_this,
                SDUInt32   *state)
{
	/* TODO */
	return SDStatus_NotImplemented;
}
#endif
/******************************************************************************/












/******************************************************************************/
#define SDGraphics_TEXT
#ifdef SDGraphics_TEXT
/* Draw a string. */
SDStatus
SDGraphics_DrawString(SDGraphics     *_this,
                      SDBrush        *brush,
                      SDChar16       *s,
                      SDUInt32        length,
                      SDFont         *font,
                      SDRectangleF    layoutRect,
                      SDStringFormat *format)
{
	/* TODO */
	return SDStatus_NotImplemented;
}

/* Measure the character ranges for a string. */
SDStatus
SDGraphics_MeasureCharacterRanges(SDGraphics      *_this,
                                  SDChar16        *s,
                                  SDUInt32         length,
                                  SDFont          *font,
                                  SDRectangleF     layoutRect,
                                  SDStringFormat  *format,
                                  SDRegion       **regions,
                                  SDUInt32        *count)
{
	/* TODO */
	return SDStatus_NotImplemented;
}

/* Measure the size of a string. */
SDStatus
SDGraphics_MeasureString(SDGraphics     *_this,
                         SDChar16       *s,
                         SDUInt32        length,
                         SDFont         *font,
                         SDRectangleF    layoutRect,
                         SDStringFormat *format,
                         SDUInt32       *charactersFitted,
                         SDUInt32       *linesFilled,
                         SDSizeF        *size)
{
	/* TODO */
	return SDStatus_NotImplemented;
}
#endif
/******************************************************************************/












/******************************************************************************/
#define SDGraphics_UTILITIES
#ifdef SDGraphics_UTILITIES
/* Flush graphics operations to the display device. */
SDStatus
SDGraphics_Flush(SDGraphics       *_this,
                 SDFlushIntention  intention)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* flush the surface */
	SDStatus_Check
		(SDSurface_Flush
			(_this->surface, intention));

	/* return successfully */
	return SDStatus_OK;
}

/* Get the HDC associated with this graphics context. */
SDStatus
SDGraphics_GetHdc(SDGraphics  *_this,
                  void       **hdc)
{
	/* TODO */
	return SDStatus_NotImplemented;
}

/* Get the nearest color that is supported by this graphics context. */
SDStatus
SDGraphics_GetNearestColor(SDGraphics *_this,
                           SDColor     color,
                           SDColor    *nearest)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a nearest color pointer */
	SDStatus_Require((nearest != 0), SDStatus_ArgumentNull);

	/* TODO: is there anything to do here? */

	/* get the nearest color */
	*nearest = color;

	/* return successfully */
	return SDStatus_OK;
}

/* Release a HDC that was obtained via a previous call to "GetHdc". */
SDStatus
SDGraphics_ReleaseHdc(SDGraphics *_this,
                      void       *hdc)
{
	/* TODO */
	return SDStatus_NotImplemented;
}

/* Transform points from one coordinate space to another. */
SDStatus
SDGraphics_TransformPoints(SDGraphics        *_this,
                           SDCoordinateSpace  dst,
                           SDCoordinateSpace  src,
                           SDPointF          *points,
                           SDUInt32           count)
{
	/* declarations */
	SDAffineTransformF t;

	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a point list pointer */
	SDStatus_Require((points != 0), SDStatus_ArgumentNull);

	/* bail out now if there's nothing to transform */
	SDStatus_Require((count != 0), SDStatus_OK);

	/* bail out now if there's nothing to do */
	SDStatus_Require((src != dst), SDStatus_OK);

	/* get the transformation from source space to destination space */
	SDGraphicsPipeline_GetSpaceTransform(&(_this->pipeline), dst, src, &t);

	/* apply the transformation to the point list */
	SDAffineTransformF_TransformPoints(&t, points, count);

	/* return successfully */
	return SDStatus_OK;
}
#endif
/******************************************************************************/


#ifdef __cplusplus
};
#endif
