/*
 * SDGraphicsPipeline.c - Graphics pipeline implementation.
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

#include "CGraphicsPipeline.h"
#include "CAffineTransform.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Handle changes to this graphics pipeline. */
static void
SDGraphicsPipeline_OnChange(SDGraphicsPipeline *_this)
{
	/* declarations */
	SDAffineTransformF dt;
	SDAffineTransformF dti;

	/* assertions */
	SDASSERT((_this != 0));

	/* get the transformations and inverses */
	dt  = _this->worldTransform;
	dti = _this->pageTransformInverse;

	/* calculate the device transformation */
	SDAffineTransformF_Multiply
		(&dt, &(_this->pageTransform), SDMatrixOrder_Append);

	/* calculate the inverse device transformation */
	SDAffineTransformF_Multiply
		(&dti, &(_this->worldTransformInverse), SDMatrixOrder_Append);

	/* set the device transformation and inverse */
	_this->deviceTransform        = dt;
	_this->deviceTransformInverse = dti;
}

/* Get the device transformation of this graphics pipeline */
SDINTERNAL void
SDGraphicsPipeline_GetDevice(SDGraphicsPipeline *_this,
                             SDAffineTransformF *transform)
{
	/* assertions */
	SDASSERT((_this     != 0));
	SDASSERT((transform != 0));

	/* get the device transformation */
	*transform = _this->deviceTransform;
}

/* Get the inverse device transformation of this graphics pipeline */
SDINTERNAL void
SDGraphicsPipeline_GetDeviceInverse(SDGraphicsPipeline *_this,
                                    SDAffineTransformF *transform)
{
	/* assertions */
	SDASSERT((_this     != 0));
	SDASSERT((transform != 0));

	/* get the inverse device transformation */
	*transform = _this->deviceTransformInverse;
}

/* Get the page transformation of this graphics pipeline */
SDINTERNAL void
SDGraphicsPipeline_GetPage(SDGraphicsPipeline *_this,
                           SDAffineTransformF *transform)
{
	/* assertions */
	SDASSERT((_this     != 0));
	SDASSERT((transform != 0));

	/* get the page transformation */
	*transform = _this->pageTransform;
}

/* Get the inverse page transformation of this graphics pipeline */
SDINTERNAL void
SDGraphicsPipeline_GetPageInverse(SDGraphicsPipeline *_this,
                                  SDAffineTransformF *transform)
{
	/* assertions */
	SDASSERT((_this     != 0));
	SDASSERT((transform != 0));

	/* get the inverse page transformation */
	*transform = _this->pageTransformInverse;
}

/* Get the world transformation of this graphics pipeline */
SDINTERNAL void
SDGraphicsPipeline_GetWorld(SDGraphicsPipeline *_this,
                            SDAffineTransformF *transform)
{
	/* assertions */
	SDASSERT((_this     != 0));
	SDASSERT((transform != 0));

	/* get the world transformation */
	*transform = _this->worldTransform;
}

/* Get the inverse world transformation of this graphics pipeline */
SDINTERNAL void
SDGraphicsPipeline_GetWorldInverse(SDGraphicsPipeline *_this,
                                   SDAffineTransformF *transform)
{
	/* assertions */
	SDASSERT((_this     != 0));
	SDASSERT((transform != 0));

	/* get the inverse world transformation */
	*transform = _this->worldTransformInverse;
}

/* Get the transformation from one coordinate space to another. */
SDINTERNAL void
SDGraphicsPipeline_GetSpaceTransform(SDGraphicsPipeline *_this,
                                     SDCoordinateSpace   dst,
                                     SDCoordinateSpace   src,
                                     SDAffineTransformF *transform)
{
	/* assertions */
	SDASSERT((_this     != 0));
	SDASSERT((transform != 0));
	SDASSERT((src       != dst));

	/* get the transformation from source space to destination space */
	switch(src)
	{
		case SDCoordinateSpace_World:
		{
			/* get the transformation from world space to destination space */
			if(dst == SDCoordinateSpace_Device)
			{
				*transform = _this->deviceTransform;
			}
			else /* dst == SDCoordinateSpace_Page */
			{
				*transform = _this->worldTransform;
			}
		}
		break;

		case SDCoordinateSpace_Page:
		{
			/* get the transformation from page space to destination space */
			if(dst == SDCoordinateSpace_World)
			{
				*transform = _this->worldTransformInverse;
			}
			else /* dst == SDCoordinateSpace_Device */
			{
				*transform = _this->pageTransform;
			}
		}
		break;

		case SDCoordinateSpace_Device:
		{
			/* get the transformation from device space to destination space */
			if(dst == SDCoordinateSpace_Page)
			{
				*transform = _this->pageTransformInverse;
			}
			else /* dst == SDCoordinateSpace_World */
			{
				*transform = _this->deviceTransformInverse;
			}
		}
		break;
	}
}

/* Reset the page transformation of this graphics pipeline. */
SDINTERNAL void
SDGraphicsPipeline_ResetPage(SDGraphicsPipeline *_this)
{
	/* assertions */
	SDASSERT((_this != 0));

	/* set the page transformation and inverse */
	_this->pageTransform        = SDAffineTransformF_Identity;
	_this->pageTransformInverse = SDAffineTransformF_Identity;

	/* update the device transformation and inverse */
	_this->deviceTransform        = _this->worldTransform;
	_this->deviceTransformInverse = _this->worldTransformInverse;
}

/* Set the page transformation of this graphics pipeline. */
SDINTERNAL void
SDGraphicsPipeline_SetPage(SDGraphicsPipeline *_this,
                           SDGraphicsUnit      pageUnit,
                           SDFloat             pageScale,
                           SDFloat             dpiX,
                           SDFloat             dpiY)
{
	/* declarations */
	SDFloat sx;
	SDFloat sy;
	SDFloat sxi;
	SDFloat syi;

	/* assertions */
	SDASSERT((_this != 0));

	/* factor in the page scaling factor */
	if(pageScale != 1.0f)
	{
		dpiX *= pageScale;
		dpiY *= pageScale;
	}

	/* calculate the scaling factors */
	switch(pageUnit)
	{
		case SDGraphicsUnit_Display:
		{
			sx  = (dpiX  / 75.0f);
			sy  = (dpiY  / 75.0f);
			sxi = (75.0f / dpiX);
			syi = (75.0f / dpiY);
		}
		break;

		case SDGraphicsUnit_Point:
		{
			sx  = (dpiX  / 72.0f);
			sy  = (dpiY  / 72.0f);
			sxi = (72.0f / dpiX);
			syi = (72.0f / dpiY);
		}
		break;

		case SDGraphicsUnit_Inch:
		{
			sx  = (dpiX / 1.0f);
			sy  = (dpiY / 1.0f);
			sxi = (1.0f / dpiX);
			syi = (1.0f / dpiY);
		}
		break;

		case SDGraphicsUnit_Document:
		{
			sx  = (dpiX   / 300.0f);
			sy  = (dpiY   / 300.0f);
			sxi = (300.0f / dpiX);
			syi = (300.0f / dpiY);
		}
		break;

		case SDGraphicsUnit_Millimeter:
		{
			sx  = (dpiX  / 25.4f);
			sy  = (dpiY  / 25.4f);
			sxi = (25.4f / dpiX);
			syi = (25.4f / dpiY);
		}
		break;

		case SDGraphicsUnit_World:
		case SDGraphicsUnit_Pixel:
		default:
		{
			sx  = (pageScale / 1.0f);
			sy  = (pageScale / 1.0f);
			sxi = (1.0f      / pageScale);
			syi = (1.0f      / pageScale);
		}
		break;
	}

	/* set the page transformation */
	SDAffineTransformF_SetElements
		(&(_this->pageTransform), sx, 0.0f, 0.0f, sy, 0.0f, 0.0f);

	/* set the inverse page transformation */
	SDAffineTransformF_SetElements
		(&(_this->pageTransformInverse), sxi, 0.0f, 0.0f, syi, 0.0f, 0.0f);

	/* update the device transformation and inverse */
	SDGraphicsPipeline_OnChange(_this);
}

/* Reset the world transformation of this graphics pipeline. */
SDINTERNAL void
SDGraphicsPipeline_ResetWorld(SDGraphicsPipeline *_this)
{
	/* assertions */
	SDASSERT((_this != 0));

	/* set the world transformation and inverse */
	_this->worldTransform        = SDAffineTransformF_Identity;
	_this->worldTransformInverse = SDAffineTransformF_Identity;

	/* update the device transformation and inverse */
	_this->deviceTransform        = _this->pageTransform;
	_this->deviceTransformInverse = _this->pageTransformInverse;
}

/* Set the world transformation of this graphics pipeline. */
SDINTERNAL SDStatus
SDGraphicsPipeline_SetWorld(SDGraphicsPipeline *_this,
                            SDAffineTransformF *transform)
{
	/* declarations */
	SDAffineTransformF inverse;

	/* assertions */
	SDASSERT((_this     != 0));
	SDASSERT((transform != 0));

	/* get the inverse transformation */
	SDStatus_Check
		(SDAffineTransformF_GetInverse
			(transform, &inverse));

	/* set the world transformation and inverse */
	_this->worldTransform        = *transform;
	_this->worldTransformInverse =  inverse;

	/* update the device transformation and inverse */
	SDGraphicsPipeline_OnChange(_this);

	/* return successfully */
	return SDStatus_OK;
}

/* Multiply the world transformation by another transformation. */
SDINTERNAL SDStatus
SDGraphicsPipeline_MultiplyWorld(SDGraphicsPipeline *_this,
                                 SDAffineTransformF *other,
                                 SDMatrixOrder       order)
{
	/* assertions */
	SDASSERT((_this != 0));
	SDASSERT((other != 0));

	/* multiply the inverse transformation */
	SDStatus_Check
		(SDAffineTransformF_MultiplyInverse
			(&(_this->worldTransformInverse), other, order));

	/* multiply the transformation */
	SDAffineTransformF_Multiply
		(&(_this->worldTransform), other, order);

	/* update the device transformation and inverse */
	SDGraphicsPipeline_OnChange(_this);

	/* return successfully */
	return SDStatus_OK;
}

/* Rotate the world transformation of this graphics pipeline. */
SDINTERNAL void
SDGraphicsPipeline_RotateWorld(SDGraphicsPipeline *_this,
                               SDFloat             angle,
                               SDMatrixOrder       order)
{
	/* assertions */
	SDASSERT((_this != 0));

	/* rotate the transformation */
	SDAffineTransformF_Rotate
		(&(_this->worldTransform), angle, order);

	/* rotate the inverse transformation */
	SDAffineTransformF_RotateInverse
		(&(_this->worldTransformInverse), angle, order);

	/* update the device transformation and inverse */
	SDGraphicsPipeline_OnChange(_this);
}

/* Scale the world transformation of this graphics pipeline. */
SDINTERNAL void
SDGraphicsPipeline_ScaleWorld(SDGraphicsPipeline *_this,
                              SDFloat             sx,
                              SDFloat             sy,
                              SDMatrixOrder       order)
{
	/* assertions */
	SDASSERT((_this != 0));

	/* scale the transformation */
	SDAffineTransformF_Scale
		(&(_this->worldTransform), sx, sy, order);

	/* scale the inverse transformation */
	SDAffineTransformF_ScaleInverse
		(&(_this->worldTransformInverse), sx, sy, order);

	/* update the device transformation and inverse */
	SDGraphicsPipeline_OnChange(_this);
}

/* Translate the world transformation of this graphics pipeline. */
SDINTERNAL void
SDGraphicsPipeline_TranslateWorld(SDGraphicsPipeline *_this,
                                  SDFloat             dx,
                                  SDFloat             dy,
                                  SDMatrixOrder       order)
{
	/* assertions */
	SDASSERT((_this != 0));

	/* translate the transformation */
	SDAffineTransformF_Translate
		(&(_this->worldTransform), dx, dy, order);

	/* translate the inverse transformation */
	SDAffineTransformF_TranslateInverse
		(&(_this->worldTransformInverse), dx, dy, order);

	/* update the device transformation and inverse */
	SDGraphicsPipeline_OnChange(_this);
}


#ifdef __cplusplus
};
#endif
