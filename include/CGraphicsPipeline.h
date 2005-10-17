/*
 * SDGraphicsPipeline.h - Graphics pipeline header.
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

#ifndef _SD_GRAPHICSPIPELINE_H_
#define _SD_GRAPHICSPIPELINE_H_

#include "SDLibInternal.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _tagSDGraphicsPipeline SDGraphicsPipeline;
struct _tagSDGraphicsPipeline
{
	/*\
	|*| w == world
	|*| p == page
	|*| d == device
	|*| t == transformation
	|*| i == inverse
	|*|
	|*| wt  : w -> p
	|*| wti : p -> w
	|*|
	|*| pt  : p -> d
	|*| pti : d -> p
	|*|
	|*| dt  : w -> d
	|*| dti : d -> w
	\*/
	SDAffineTransformF worldTransform;
	SDAffineTransformF worldTransformInverse;
	SDAffineTransformF pageTransform;
	SDAffineTransformF pageTransformInverse;
	SDAffineTransformF deviceTransform;
	SDAffineTransformF deviceTransformInverse;
};

#define SDGraphicsPipeline_World(pipeline) \
			((pipeline).worldTransform)
#define SDGraphicsPipeline_WorldInverse(pipeline) \
			((pipeline).worldTransformInverse)
#define SDGraphicsPipeline_Page(pipeline) \
			((pipeline).pageTransform)
#define SDGraphicsPipeline_PageInverse(pipeline) \
			((pipeline).pageTransformInverse)
#define SDGraphicsPipeline_Device(pipeline) \
			((pipeline).deviceTransform)
#define SDGraphicsPipeline_DeviceInverse(pipeline) \
			((pipeline).deviceTransformInverse)

SDINTERNAL void
SDGraphicsPipeline_GetDevice(SDGraphicsPipeline *_this,
                             SDAffineTransformF *transform);
SDINTERNAL void
SDGraphicsPipeline_GetDeviceInverse(SDGraphicsPipeline *_this,
                                    SDAffineTransformF *transform);
SDINTERNAL void
SDGraphicsPipeline_GetPage(SDGraphicsPipeline *_this,
                           SDAffineTransformF *transform);
SDINTERNAL void
SDGraphicsPipeline_GetPageInverse(SDGraphicsPipeline *_this,
                                  SDAffineTransformF *transform);
SDINTERNAL void
SDGraphicsPipeline_GetWorld(SDGraphicsPipeline *_this,
                            SDAffineTransformF *transform);
SDINTERNAL void
SDGraphicsPipeline_GetWorldInverse(SDGraphicsPipeline *_this,
                                   SDAffineTransformF *transform);
SDINTERNAL void
SDGraphicsPipeline_GetSpaceTransform(SDGraphicsPipeline *_this,
                                     SDCoordinateSpace   dst,
                                     SDCoordinateSpace   src,
                                     SDAffineTransformF *transform);
SDINTERNAL void
SDGraphicsPipeline_ResetPage(SDGraphicsPipeline *_this);
SDINTERNAL void
SDGraphicsPipeline_SetPage(SDGraphicsPipeline *_this,
                           SDGraphicsUnit      pageUnit,
                           SDFloat             pageScale,
                           SDFloat             dpiX,
                           SDFloat             dpiY);
SDINTERNAL void
SDGraphicsPipeline_ResetWorld(SDGraphicsPipeline *_this);
SDINTERNAL SDStatus
SDGraphicsPipeline_SetWorld(SDGraphicsPipeline *_this,
                            SDAffineTransformF *transform);
SDINTERNAL SDStatus
SDGraphicsPipeline_MultiplyWorld(SDGraphicsPipeline *_this,
                                 SDAffineTransformF *other,
                                 SDMatrixOrder       order);
SDINTERNAL void
SDGraphicsPipeline_RotateWorld(SDGraphicsPipeline *_this,
                               SDFloat             angle,
                               SDMatrixOrder       order);
SDINTERNAL void
SDGraphicsPipeline_ScaleWorld(SDGraphicsPipeline *_this,
                              SDFloat             sx,
                              SDFloat             sy,
                              SDMatrixOrder       order);
SDINTERNAL void
SDGraphicsPipeline_TranslateWorld(SDGraphicsPipeline *_this,
                                  SDFloat             dx,
                                  SDFloat             dy,
                                  SDMatrixOrder       order);

#ifdef __cplusplus
};
#endif

#endif /* _SD_GRAPHICSPIPELINE_H_ */
