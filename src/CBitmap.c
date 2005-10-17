/*
 * SDBitmap.c - Bitmap implementation.
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

#include "CBitmap.h"
#include "CUtils.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Initialize this bitmap. */
static SDStatus
SDBitmap_Initialize(SDBitmap      *_this,
                    SDUInt32       width,
                    SDUInt32       height,
                    SDPixelFormat  format)
{
	/* declarations */
	pixman_format_t *fmt;

	/* assertions */
	SDASSERT((_this != 0));

	/* initialize the members */
	_this->type        = SDImageType_Bitmap;
	_this->format      = SDImageFormat_MemoryBMP;
	_this->flags       = SDImageFlag_None;
	_this->pixelFormat = format;
	_this->dpiX        = SDGraphics_DefaultDpiX;
	_this->dpiY        = SDGraphics_DefaultDpiY;
	_this->width       = width;
	_this->height      = height;
	_this->bitmapDataX = 0;
	_this->bitmapDataY = 0;
	_this->bitmapData  = 0;
	_this->locked      = 0;
	_this->palette     = 0;
	_this->refCount    = 1;

	/* create the mutex */
	SDStatus_Check
		(SDMutex_Create
			(&(_this->lock)));

	/* create the pixman format */
	if(!(fmt = pixman_format_create(PIXMAN_FORMAT_NAME_ARGB32)))
	{
		SDMutex_Destroy(&(_this->lock));
		return SDStatus_OutOfMemory;
	}

	/* create the pixman image */
	_this->image = pixman_image_create(fmt, width, height);

	/* free the format */
	pixman_format_destroy(fmt);

	/* handle image creation failures */
	if(_this->image == 0)
	{
		SDMutex_Destroy(&(_this->lock));
		return SDStatus_OutOfMemory;
	}

	/* initialize the pixman image data */
	{
		/* declarations */
		SDByte   *d;
		SDUInt32  h;
		SDUInt32  s;

		/* get the pixman image information */
		d = (SDByte *)pixman_image_get_data(_this->image);
		h = (SDUInt32)pixman_image_get_height(_this->image);
		s = (SDUInt32)pixman_image_get_stride(_this->image);

		/* initialize the data */
		SDMemSet(d, 0x00, (h * s));
	}

	/* return successfully */
	return SDStatus_OK;
}

/* Create a bitmap. */
#if 0
SDStatus
SDBitmap_CreateStream(SDBitmap **_this,
                      SDStream  *stream,
                      SDBool     useICM)
{
	/* TODO */
	return SDStatus_NotImplemented;
}
SDStatus
SDBitmap_CreateFile(SDBitmap **_this,
                    SDChar16  *filename,
                    SDBool     useICM)
{
	/* TODO */
	return SDStatus_NotImplemented;
}
#endif

SDStatus
SDBitmap_Create(SDBitmap      **_this,
                SDUInt32        width,
                SDUInt32        height,
                SDPixelFormat   format)
{
	/* declarations */
	SDStatus status;

	/* ensure we have a this pointer pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* allocate the bitmap */
	if(!(*_this = (SDBitmap *)SDMalloc(sizeof(SDBitmap))))
	{
		return SDStatus_OutOfMemory;
	}

	/* initialize the members */
	status = SDBitmap_Initialize(*_this, width, height, format);

	/* handle initialization failures */
	if(status != SDStatus_OK)
	{
		SDFree(*_this);
		*_this = 0;
		return status;
	}

	/* return successfully */
	return SDStatus_OK;
}

SDStatus
SDBitmap_CreateData(SDBitmap      **_this,
                    SDByte         *data,
                    SDUInt32        width,
                    SDUInt32        height,
                    SDUInt32        stride,
                    SDPixelFormat   format)
{
	/* declarations */
	SDStatus status;

	/* ensure we have a data pointer */
	SDStatus_Require((data != 0), SDStatus_ArgumentNull);

	/* create the bitmap */
	SDStatus_Check
		(SDBitmap_Create
			(_this, width, height, format));

	/* initialize the pixman image data */
	status =
		SDUtils_ToPixmanImage
			(format, data, (*_this)->image, 0, 0, width, height, stride,
			 (*_this)->palette);

	/* handle image data initialization failures */
	if(status != SDStatus_OK)
	{
		SDBitmap_Destroy(_this);
		return status;
	}

	/* return successfully */
	return SDStatus_OK;
}

/* Destroy this bitmap. */
SDStatus
SDBitmap_Destroy(SDBitmap **_this)
{
	return SDImage_Destroy(_this);
}

/* Clone this bitmap and transform it into a new pixel format. */
SDStatus
SDBitmap_Clone(SDBitmap       *_this,
               SDBitmap      **clone,
               SDUInt32        x,
               SDUInt32        y,
               SDUInt32        width,
               SDUInt32        height,
               SDPixelFormat   format)
{
	/* declarations */
	SDStatus     status;
	SDBitmapData bd;

	/* lock the image data */
	SDStatus_Check
		(SDBitmap_LockBits
			(_this, x, y, width, height,
			 SDImageLockMode_ReadOnly, format, &bd));

	/* create the bitmap */
	status =
		SDBitmap_CreateData
			(clone, bd.scan0, width, height, bd.stride, format);

	/* unlock the image data */
	SDBitmap_UnlockBits(_this, &bd);

	/* return status */
	return status;
}

/* Get the color of a specific pixel. */
SDStatus
SDBitmap_GetPixel(SDBitmap *_this,
                  SDUInt32  x,
                  SDUInt32  y,
                  SDColor  *color)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a color pointer */
	SDStatus_Require((color != 0), SDStatus_ArgumentNull);

	/* get the pixel synchronously */
	SDMutex_Lock(_this->lock);
	{
		/* declarations */
		SDStatus  status;
		SDColor  *pixel;

		/* ensure the image data isn't locked */
		if(_this->locked)
		{
			SDMutex_Unlock(_this->lock);
			return SDStatus_InvalidOperation_ImageLocked;
		}

		/* get the pixel pointer */
		status =
			SDUtils_GetPixmanPixelPointer
				(_this->image, x, y, &pixel);

		/* ensure we have a pixel pointer */
		if(status != SDStatus_OK)
		{
			SDMutex_Unlock(_this->lock);
			return status;
		}

		/* get the pixel color */
		{
			/* declarations */
			SDByte a, r, g, b;

			/* get the pixel color components */
			SDPixmanPixel_ToARGB(*pixel, a, r, g, b);

			/* get the pixel color based on transparency */
			if(a == 0)
			{
				*color = SDColor_FromARGB(0, 0, 0, 0);
			}
			else
			{
				/* scale the components */
				b = (((b << 8) - b) / a);
				g = (((g << 8) - g) / a);
				r = (((r << 8) - r) / a);

				/* get the pixel color */
				*color = SDColor_FromARGB(a, r, g, b);
			}
		}
	}
	SDMutex_Unlock(_this->lock);

	/* return successfully */
	return SDStatus_OK;
}

/* Set a pixel within this bitmap. */
SDStatus
SDBitmap_SetPixel(SDBitmap *_this,
                  SDUInt32  x,
                  SDUInt32  y,
                  SDColor   color)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* set the pixel synchronously */
	SDMutex_Lock(_this->lock);
	{
		/* declarations */
		SDStatus  status;
		SDColor  *pixel;

		/* ensure the image data isn't locked */
		if(_this->locked)
		{
			SDMutex_Unlock(_this->lock);
			return SDStatus_InvalidOperation_ImageLocked;
		}

		/* get the pixel pointer */
		status =
			SDUtils_GetPixmanPixelPointer
				(_this->image, x, y, &pixel);

		/* ensure we have a pixel pointer */
		if(status != SDStatus_OK)
		{
			SDMutex_Unlock(_this->lock);
			return status;
		}

		/* get the pixel value */
		{
			/* declarations */
			SDByte a, r, g, b;

			/* get the color components */
			a = SDColor_A(color);
			r = SDColor_R(color);
			g = SDColor_G(color);
			b = SDColor_B(color);

			/* set the pixel color based on the transparency */
			if(a == 0)
			{
				*pixel = SDPixmanPixel_FromARGB(0, 0, 0, 0);
			}
			else
			{
				*pixel =
					SDPixmanPixel_FromARGB
						(a, ((r * a) / 255), ((g * a) / 255), ((b * a) / 255));
			}
		}
	}
	SDMutex_Unlock(_this->lock);

	/* return successfully */
	return SDStatus_OK;
}

/* Make the bitmap data from the current image data. */
static SDStatus
SDBitmap_MakeBitmapData(SDBitmap        *_this,
                        SDUInt32         x,
                        SDUInt32         y,
                        SDUInt32         width,
                        SDUInt32         height,
                        SDImageLockMode  lockMode,
                        SDPixelFormat    format)
{
	/* declarations */
	SDUInt32 stride;

	/* assertions */
	SDASSERT((_this             != 0));
	SDASSERT((_this->bitmapData == 0));

	/* ensure we have a valid lock mode */
	if(lockMode < SDImageLockMode_ReadOnly ||
	   lockMode > SDImageLockMode_ReadWrite)
	{
		return SDStatus_Argument;
	}

	/* ensure the rectangle is in bounds */
	if(((width) == 0) ||
	   ((height) == 0) ||
	   ((x + width) > _this->width) ||
	   ((y + height) > _this->height))
	{
		return SDStatus_ArgumentOutOfRange;
	}

	/* calculate the stride based on the pixel format */
	switch(format)
	{
		case SDPixelFormat_1bppIndexed:
		case SDPixelFormat_4bppIndexed:
		case SDPixelFormat_8bppIndexed:
		case SDPixelFormat_48bppRgb:
		case SDPixelFormat_64bppArgb:
		case SDPixelFormat_64bppPArgb:
			{ return SDStatus_NotImplemented; }

		case SDPixelFormat_16bppGrayScale:
		case SDPixelFormat_16bppRgb555:
		case SDPixelFormat_16bppRgb565:
		case SDPixelFormat_16bppArgb1555:
		{
			stride = (((width << 1) + 3) & ~3);
		}
		break;

		case SDPixelFormat_24bppRgb:
		{
			stride = ((((width << 1) + width) + 3) & ~3);
		}
		break;

		case SDPixelFormat_32bppRgb:
		case SDPixelFormat_32bppArgb:
		case SDPixelFormat_32bppPArgb:
		{
			stride = (width << 2);
		}
		break;

		default: { return SDStatus_NotSupported; }
	}

	/* create the bitmap data */
	{
		/* calculate the size */
		const SDUInt32 size = (sizeof(SDBitmapData) + (height * stride));

		/* allocate the bitmap data */
		if(!(_this->bitmapData = (SDBitmapData *)SDMalloc(size)))
		{
			return SDStatus_OutOfMemory;
		}

		/* set the bitmap data scans */
		_this->bitmapData->scan0 =
			(((SDByte *)_this->bitmapData) + sizeof(SDBitmapData));

		/* set the remaining bitmap data members */
		_this->bitmapDataX             = x;
		_this->bitmapDataY             = y;
		_this->bitmapData->width       = width;
		_this->bitmapData->height      = height;
		_this->bitmapData->stride      = stride;
		_this->bitmapData->pixelFormat = format;
		_this->bitmapData->reserved    = lockMode;
	}

	/* copy the image data, as needed */
	if(lockMode & SDImageLockMode_ReadOnly)
	{
		/* declarations */
		SDStatus status;

		/* copy the image data */
		status =
			SDUtils_FromPixmanImage
				(_this->bitmapData->pixelFormat, _this->image,
				 _this->bitmapData->scan0,       _this->bitmapDataX,
        		 _this->bitmapDataY,             _this->bitmapData->width,
				 _this->bitmapData->height,      _this->bitmapData->stride,
				 _this->palette);

		/* handle copying failures */
		if(status != SDStatus_OK)
		{
			SDFree(_this->bitmapData);
			_this->bitmapData = 0;
			return status;
		}
	}
	else
	{
		SDMemSet(_this->bitmapData->scan0, 0x00, (height * stride));
	}

	/* return successfully */
	return SDStatus_OK;
}

/* Lock a region of this bitmap. */
SDStatus
SDBitmap_LockBits(SDBitmap        *_this,
                  SDUInt32         x,
                  SDUInt32         y,
                  SDUInt32         width,
                  SDUInt32         height,
                  SDImageLockMode  lockMode,
                  SDPixelFormat    format,
                  SDBitmapData    *bitmapData)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a bitmap data pointer */
	SDStatus_Require((bitmapData != 0), SDStatus_ArgumentNull);

	/* lock the bits synchronously */
	SDMutex_Lock(_this->lock);
	{
		/* declarations */
		SDStatus status;

		/* ensure the image data isn't locked */
		if(_this->locked)
		{
			SDMutex_Unlock(_this->lock);
			return SDStatus_InvalidOperation_ImageLocked;
		}

		/* create the bitmap data */
		status =
			SDBitmap_MakeBitmapData
				(_this, x, y, width, height, lockMode, format);

		/* handle bitmap data creation failures */
		if(status != SDStatus_OK)
		{
			SDMutex_Unlock(_this->lock);
			return status;
		}

		/* set the locked flag */
		_this->locked = 1;
	}
	SDMutex_Unlock(_this->lock);

	/* return successfully */
	return SDStatus_OK;
}

/* Set the resolution for this bitmap. */
SDStatus
SDBitmap_SetResolution(SDBitmap *_this,
                       SDFloat   dpiX,
                       SDFloat   dpiY)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* set the resolution synchronously */
	SDMutex_Lock(_this->lock);
	{
		/* ensure the image data isn't locked */
		if(_this->locked)
		{
			SDMutex_Unlock(_this->lock);
			return SDStatus_InvalidOperation_ImageLocked;
		}

		/* set the resolution */
		_this->dpiX = dpiX;
		_this->dpiY = dpiY;
	}
	SDMutex_Unlock(_this->lock);

	/* return successfully */
	return SDStatus_OK;
}

/* Unlock the bits within this bitmap. */
SDStatus
SDBitmap_UnlockBits(SDBitmap     *_this,
                    SDBitmapData *bitmapData)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a bitmap data pointer */
	SDStatus_Require((bitmapData != 0), SDStatus_ArgumentNull);

	/* lock the bits synchronously */
	SDMutex_Lock(_this->lock);
	{
		/* declarations */
		SDStatus status;

		/* ensure the image data is locked */
		if(!(_this->locked))
		{
			SDMutex_Unlock(_this->lock);
			return SDStatus_InvalidOperation;
		}

		/* ensure the bitmap data is for this image */
		if(_this->bitmapData->scan0 != bitmapData->scan0)
		{
			SDMutex_Unlock(_this->lock);
			return SDStatus_Argument;
		}

		/* set the status to the default */
		status = SDStatus_OK;

		/* set the image data, as needed */
		if(_this->bitmapData->reserved & SDImageLockMode_WriteOnly)
		{
			/* copy the bitmap data to the image */
			status =
				SDUtils_ToPixmanImage
					(_this->bitmapData->pixelFormat, _this->bitmapData->scan0,
					 _this->image,                   _this->bitmapDataX,
					 _this->bitmapDataY,             _this->bitmapData->width,
					 _this->bitmapData->height,      _this->bitmapData->stride,
					 _this->palette);
		}

		/* free the bitmap data */
		SDFree(_this->bitmapData);

		/* reset the bitmap data */
		_this->bitmapData = 0;
		_this->locked     = 0;
		bitmapData->scan0 = 0;

		/* handle failures */
		if(status != SDStatus_OK)
		{
			SDMutex_Unlock(_this->lock);
			return status;
		}
	}
	SDMutex_Unlock(_this->lock);

	/* return successfully */
	return SDStatus_OK;
}


#ifdef __cplusplus
};
#endif
