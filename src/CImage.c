/*
 * SDImage.c - Image implementation.
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

#include "CImage.h"
#include "CUtils.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* Define the frame dimensions. */
const SDGuid SDFrameDimension_Page =
{
	0x7462dc86, 0x6180, 0x4c7e, 0x8e, 0x3f, 0xee, 0x73, 0x33, 0xa7, 0xa4, 0x83
};
const SDGuid SDFrameDimension_Resolution =
{
	0x84236f7b, 0x3bd3, 0x428f, 0x8d, 0xab, 0x4e, 0xa1, 0x43, 0x9c, 0xa3, 0x15
};
const SDGuid SDFrameDimension_Time =
{
	0x6aedbd6d, 0x3fb5, 0x418a, 0x83, 0xa6, 0x7f, 0x45, 0x22, 0x9d, 0xc8, 0x72
};



/* Define the image formats. */
const SDGuid SDImageFormat_MemoryBMP =
{
	0xb96b3caa, 0x0728, 0x11d3, 0x9d, 0x7b, 0x00, 0x00, 0xf8, 0x1e, 0xf3, 0x2e
};
const SDGuid SDImageFormat_BMP =
{
	0xb96b3cab, 0x0728, 0x11d3, 0x9d, 0x7b, 0x00, 0x00, 0xf8, 0x1e, 0xf3, 0x2e
};
const SDGuid SDImageFormat_EMF =
{
	0xb96b3cac, 0x0728, 0x11d3, 0x9d, 0x7b, 0x00, 0x00, 0xf8, 0x1e, 0xf3, 0x2e
};
const SDGuid SDImageFormat_WMF =
{
	0xb96b3cad, 0x0728, 0x11d3, 0x9d, 0x7b, 0x00, 0x00, 0xf8, 0x1e, 0xf3, 0x2e
};
const SDGuid SDImageFormat_JPG =
{
	0xb96b3cae, 0x0728, 0x11d3, 0x9d, 0x7b, 0x00, 0x00, 0xf8, 0x1e, 0xf3, 0x2e
};
const SDGuid SDImageFormat_PNG =
{
	0xb96b3caf, 0x0728, 0x11d3, 0x9d, 0x7b, 0x00, 0x00, 0xf8, 0x1e, 0xf3, 0x2e
};
const SDGuid SDImageFormat_GIF =
{
	0xb96b3cb0, 0x0728, 0x11d3, 0x9d, 0x7b, 0x00, 0x00, 0xf8, 0x1e, 0xf3, 0x2e
};
const SDGuid SDImageFormat_TIFF =
{
	0xb96b3cb1, 0x0728, 0x11d3, 0x9d, 0x7b, 0x00, 0x00, 0xf8, 0x1e, 0xf3, 0x2e
};
const SDGuid SDImageFormat_EXIF =
{
	0xb96b3cb2, 0x0728, 0x11d3, 0x9d, 0x7b, 0x00, 0x00, 0xf8, 0x1e, 0xf3, 0x2e
};
/******************************************************************************/





















/******************************************************************************/
/* Create an image. */
#if 0
SDStatus
SDImage_CreateData(SDImage  **_this,
                   SDByte    *data,
                   SDUInt32  count)
{
	/* TODO */
	return SDStatus_NotImplemented;
}
SDStatus
SDImage_CreateFile(SDImage  **_this,
                   SDChar16  *filename,
                   SDBool     useICM)
{
	/* TODO */
	return SDStatus_NotImplemented;
}
SDStatus
SDImage_CreateStream(SDImage  **_this,
                     SDStream  *stream,
                     SDBool     useICM)
{
	/* TODO */
	return SDStatus_NotImplemented;
}
#endif

/* Reference this image. */
SDINTERNAL SDImage *
SDImage_Reference(SDImage *_this)
{
	/* assertions */
	SDASSERT((_this != 0));

	/* reference this image synchronously */
	SDMutex_Lock(_this->lock);
	{
		++(_this->refCount);
	}
	SDMutex_Unlock(_this->lock);

	/* return this image */
	return _this;
}

/* Destroy this image. */
SDStatus
SDImage_Destroy(SDImage **_this)
{
	/* declarations */
	SDMutex *lock;
	SDBool   lockOwner;

	/* ensure we have a this pointer pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a this pointer */
	SDStatus_Require((*_this != 0), SDStatus_ArgumentNull);

	/* get the lock */
	lock = (*_this)->lock;

	/* finalize and dispose of the image */
	SDMutex_Lock(lock);
	{
		/* update the reference count */
		--((*_this)->refCount);

		/* finalize, as needed */
		if((*_this)->refCount != 0)
		{
			lockOwner = 0;
		}
		else
		{
			/* finalize the bitmap data */
			if((*_this)->locked)
			{
				SDFree((*_this)->bitmapData);
				(*_this)->bitmapData = 0;
				(*_this)->locked = 0;
			}

			/* set the lock ownership flag */
			lockOwner = 1;

			/* finalize the palette */
			if((*_this)->palette != 0)
			{
				SDColorPalette_Destroy(&((*_this)->palette));
			}

			/* finalize the pixman image */
			pixman_image_destroy((*_this)->image);

			/* dispose of the image */
			SDFree(*_this);
		}
	}
	SDMutex_Unlock(lock);

	/* destroy the lock, as needed */
	if(lockOwner) { SDMutex_Destroy(&lock); }

	/* null the image pointer */
	*_this = 0;

	/* return successfully */
	return SDStatus_OK;
}

/* Get the flags of this image. */
SDStatus
SDImage_GetFlags(SDImage     *_this,
                 SDImageFlag *flags)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a flags pointer */
	SDStatus_Require((flags != 0), SDStatus_ArgumentNull);

	/* get the flags synchronously */
	SDMutex_Lock(_this->lock);
	{
		*flags = _this->flags;
	}
	SDMutex_Unlock(_this->lock);

	/* return successfully */
	return SDStatus_OK;
}

/* Get the height of this image. */
SDStatus
SDImage_GetHeight(SDImage  *_this,
                  SDUInt32 *height)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a height pointer */
	SDStatus_Require((height != 0), SDStatus_ArgumentNull);

	/* get the height synchronously */
	SDMutex_Lock(_this->lock);
	{
		*height = _this->height;
	}
	SDMutex_Unlock(_this->lock);

	/* return successfully */
	return SDStatus_OK;
}

/* Get the horizontal resolution of this image. */
SDStatus
SDImage_GetHorizontalResolution(SDImage *_this,
                                SDFloat *dpiX)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a horizontal resolution pointer */
	SDStatus_Require((dpiX != 0), SDStatus_ArgumentNull);

	/* get the horizontal resolution synchronously */
	SDMutex_Lock(_this->lock);
	{
		*dpiX = _this->dpiX;
	}
	SDMutex_Unlock(_this->lock);

	/* return successfully */
	return SDStatus_OK;
}

/* Get the type of this image. */
SDStatus
SDImage_GetImageType(SDImage     *_this,
                     SDImageType *type)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a type pointer */
	SDStatus_Require((type != 0), SDStatus_ArgumentNull);

	/* get the type */
	*type = _this->type;

	/* return successfully */
	return SDStatus_OK;
}

/* Get the size of this image. */
SDStatus
SDImage_GetPhysicalDimension(SDImage *_this,
                             SDSizeF *size)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a size pointer */
	SDStatus_Require((size != 0), SDStatus_ArgumentNull);

	/* get the size synchronously */
	SDMutex_Lock(_this->lock);
	{
		SDSize_Width(*size)  = _this->width;
		SDSize_Height(*size) = _this->height;
	}
	SDMutex_Unlock(_this->lock);

	/* return successfully */
	return SDStatus_OK;
}

/* Get the pixel format of this image. */
SDStatus
SDImage_GetPixelFormat(SDImage       *_this,
                       SDPixelFormat *pixelFormat)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a pixel format pointer */
	SDStatus_Require((pixelFormat != 0), SDStatus_ArgumentNull);

	/* get the pixel format synchronously */
	SDMutex_Lock(_this->lock);
	{
		*pixelFormat = _this->pixelFormat;
	}
	SDMutex_Unlock(_this->lock);

	/* return successfully */
	return SDStatus_OK;
}

/* Get the raw format of this image. */
SDStatus
SDImage_GetRawFormat(SDImage *_this,
                     SDGuid  *format)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a raw format pointer */
	SDStatus_Require((format != 0), SDStatus_ArgumentNull);

	/* get the raw format */
	*format = _this->format;

	/* return successfully */
	return SDStatus_OK;
}

/* Get the vertical resolution of this image. */
SDStatus
SDImage_GetVerticalResolution(SDImage *_this,
                              SDFloat *dpiY)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a vertical resolution pointer */
	SDStatus_Require((dpiY != 0), SDStatus_ArgumentNull);

	/* get the vertical resolution synchronously */
	SDMutex_Lock(_this->lock);
	{
		*dpiY = _this->dpiY;
	}
	SDMutex_Unlock(_this->lock);

	/* return successfully */
	return SDStatus_OK;
}

/* Get the width of this image. */
SDStatus
SDImage_GetWidth(SDImage  *_this,
                 SDUInt32 *width)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a width pointer */
	SDStatus_Require((width != 0), SDStatus_ArgumentNull);

	/* get the width synchronously */
	SDMutex_Lock(_this->lock);
	{
		*width = _this->width;
	}
	SDMutex_Unlock(_this->lock);

	/* return successfully */
	return SDStatus_OK;
}
/******************************************************************************/










/******************************************************************************/
/* Clone this image. */
SDStatus
SDImage_Clone(SDImage  *_this,
              SDImage **clone)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* create the clone synchronously */
	SDMutex_Lock(_this->lock);
	{
		/* declarations */
		SDStatus status;

		/* create the image clone */
		status =
			SDBitmap_Create
				((SDBitmap **)clone, _this->width, _this->height,
				 _this->pixelFormat);

		/* handle clone creation failures */
		if(status != SDStatus_OK)
		{
			SDMutex_Unlock(_this->lock);
			return status;
		}

		/* initialize the pixman image data */
		{
			/* declarations */
			SDByte   *src;
			SDByte   *dst;
			SDUInt32  height;
			SDUInt32  stride;

			/* get the pixman image data */
			src = (SDByte *)pixman_image_get_data(_this->image);
			dst = (SDByte *)pixman_image_get_data((*clone)->image);

			/* get the height */
			height = pixman_image_get_height(_this->image);

			/* get the stride */
			stride = pixman_image_get_stride(_this->image);

			/* copy the image data */
			SDMemCopy(dst, src, (height * stride));
		}
	}
	SDMutex_Unlock(_this->lock);

	/* return successfully */
	return SDStatus_OK;
}

/******************************************************************************/
















/******************************************************************************/
/* Get a bounding rectangle for this image. */
SDStatus
SDImage_GetBounds(SDImage        *_this,
                  SDGraphicsUnit  pageUnit,
                  SDRectangleF   *bounds)
{
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a bounds pointer */
	SDStatus_Require((bounds != 0), SDStatus_ArgumentNull);

	/* get the bounds synchronously */
	SDMutex_Lock(_this->lock);
	{
		/* set the offsets */
		SDRectangle_X(*bounds) = 0;
		SDRectangle_Y(*bounds) = 0;

		/* get the width */
		SDRectangle_Width(*bounds) =
			SDUtils_ConvertUnitsDPI
				(SDGraphicsUnit_Pixel, pageUnit,
				 _this->width, _this->dpiY, _this->dpiY);

		/* get the height */
		SDRectangle_Height(*bounds) =
			SDUtils_ConvertUnitsDPI
				(SDGraphicsUnit_Pixel, pageUnit,
				 _this->height, _this->dpiX, _this->dpiX);
	}
	SDMutex_Unlock(_this->lock);

	/* return successfully */
	return SDStatus_OK;
}

#if 0
/* Get parameter information for a specific encoder. */
SDStatus
SDImage_GetEncoderParameters(SDImage             *_this,
                             SDGuid               encoder,
                             SDEncoderParameter **parameters,
                             SDUInt32            *count)
{
	/* TODO */
	return SDStatus_NotImplemented;
}
#endif

/* Get the raw image data of this image. */
SDStatus
SDImage_GetData(SDImage   *_this,
                SDByte   **data,
                SDUInt32  *count)
{
	/* TODO */
	return SDStatus_NotImplemented;
}

/******************************************************************************/






/******************************************************************************/
/* Get the palette of this image. */
SDStatus
SDImage_GetPalette(SDImage         *_this,
                   SDColorPalette **palette)
{
#if 0
	/* declarations */
	SDColorPalette *tmp;

	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a palette pointer pointer */
	SDStatus_Require((palette != 0), SDStatus_ArgumentNull);

	/* TODO: locking */

	/* get the palette */
	tmp = _this->palette;

	/* clone the palette, as needed */
	if(tmp == 0)
	{
		*palette = 0;
	}
	else
	{
		SDStatus_Check
			(SDColorPalette_Clone
				(tmp, palette));
	}

	/* return successfully */
	return SDStatus_OK;
#else
	return SDStatus_NotImplemented;
#endif
}

/* Set the palette of this image. */
SDStatus
SDImage_SetPalette(SDImage        *_this,
                   SDColorPalette *palette)
{
#if 0
	/* declarations */
	SDPixelFormat pixelFormat;

	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a palette pointer */
	SDStatus_Require((palette != 0), SDStatus_ArgumentNull);

	/* TODO: locking */

	/* get the pixel format */
	pixelFormat = _this->pixelFormat;

	/* ensure we have an indexed frame */
	SDStatus_Require
		((SDPixelFormat_IsIndexed(pixelFormat)), SDStatus_InvalidOperation);

	/* ensure we have a correctly sized palette */
	SDStatus_Require
		((SDColorPalette_CheckFormat(pixelFormat)), SDStatus_Argument);

	/* set the palette */
	{
		/* declarations */
		SDColorPalette *tmp, *old;

		/* get the old palette */
		old = _this->palette;

		/* clone the new palette */
		SDStatus_Check
			(SDColorPalette_Clone
				(palette, &tmp));

		/* dispose of the old palette, as needed */
		if(old != 0)
		{
			SDColorPalette_Destroy(&(old));
		}

		/* set the palette */
		_this->palette = tmp;
	}

	/* return successfully */
	return SDStatus_OK;
#else
	return SDStatus_NotImplemented;
#endif
}

/******************************************************************************/









/******************************************************************************/
/* Get a specific property item. */
SDStatus
SDImage_GetPropertyItem(SDImage        *_this,
                        SDPropertyID    id,
                        SDPropertyItem *item)
{
#if 0
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a property item pointer */
	SDStatus_Require((item != 0), SDStatus_ArgumentNull);

	/* get the property item */
	{
		/* declarations */
		SDPropertyItem *curr;
		SDPropertyItem *end;

		/* get the count */
		const SDUInt32 count = _this->propertyItemCount;

		/* get the property item pointer */
		curr = _this->propertyItems;

		/* get the end pointer */
		end = (curr + count);

		/* get the property item */
		while(curr != end)
		{
			/* get the item if it matches */
			if(curr->id == id)
			{
				/* initialize the item */
				SDStatus_Check
					(SDPropertyItem_Initialize
						(item, id, curr->length, curr->type, curr->value));

				/* return successfully */
				return SDStatus_OK;
			}

			/* move to the next property item */
			++curr;
		}
	}

	/* TODO: test GDI+ to see what it does here */

	/* return unsuccessfully */
	return SDStatus_Argument;
#else
	return SDStatus_NotImplemented;
#endif
}

/* Set a property on this image. */
SDStatus
SDImage_SetPropertyItem(SDImage        *_this,
                        SDPropertyItem *item)
{
	/* TODO */
	return SDStatus_NotImplemented;
}

/* Remove a specific property item. */
SDStatus
SDImage_RemovePropertyItem(SDImage      *_this,
                           SDPropertyID  id)
{
	/* TODO */
	return SDStatus_NotImplemented;
}

/* Get a list of the property ids of this image. */
SDStatus
SDImage_GetPropertyIDs(SDImage       *_this,
                       SDPropertyID **ids,
                       SDUInt32      *count)
{
	/* TODO */
	return SDStatus_NotImplemented;
}

/* Get a list of the property items of this image. */
SDStatus
SDImage_GetPropertyItems(SDImage         *_this,
                         SDPropertyItem **propertyItems,
                         SDUInt32        *count)
{
	/* TODO */
	return SDStatus_NotImplemented;
}
/******************************************************************************/








/******************************************************************************/
/* Get a thumbnail version of this image. */
SDStatus
SDImage_GetThumbnailImage(SDImage               *_this,
                          SDUInt32               width,
                          SDUInt32               height,
                          SDImage              **thumbnail)
{
	/* TODO */
	return SDStatus_NotImplemented;
}

/* Rotate and/or flip this image. */
SDStatus
SDImage_RotateFlip(SDImage          *_this,
                   SDRotateFlipType  rotateFlipType)
{
	/* TODO */
	return SDStatus_NotImplemented;
}
/******************************************************************************/







/******************************************************************************/
#if 0
/* Save this image. */
SDStatus
SDImage_Save(SDImage            *_this,
             SDStream           *stream,
             SDGuid              encoder,
             SDEncoderParameter *parameters,
             SDUInt32            count)
{
	/* TODO */
	return SDStatus_NotImplemented;
}

/* Add a frame to the previously saved image file. */
SDStatus
SDImage_SaveAdd(SDImage            *_this,
                SDEncoderParameter *parameters,
                SDUInt32            count)
{
	/* TODO */
	return SDStatus_NotImplemented;
}
SDStatus
SDImage_SaveAddImage(SDImage            *_this,
                     SDImage            *image,
                     SDEncoderParameter *parameters,
                     SDUInt32            count)
{
	/* TODO */
	return SDStatus_NotImplemented;
}
#endif
/******************************************************************************/










/******************************************************************************/
/* Get the number of frames in a specific dimension. */
SDStatus
SDImage_GetFrameCount(SDImage  *_this,
                      SDGuid    dimension,
                      SDUInt32 *frameCount)
{
#if 0
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a frame count pointer */
	SDStatus_Require((frameCount != 0), SDStatus_ArgumentNull);

	/* get the frame count */
	{
		/* declarations */
		SDFrameInfo *curr;
		SDFrameInfo *end;

		/* get the frame information pointer */
		curr = _this->frameDimensions;

		/* get the end pointer */
		end = (curr + (_this->frameDimensionCount));

		/* get the frame count */
		while(curr != end)
		{
			/* get the frame count, if the dimension matches */
			if(SDMemCmp(&(curr->guid), &dimension, sizeof(SDGuid)) == 0)
			{
				/* set the frame count */
				*frameCount = curr->count;

				/* return successfully */
				return SDStatus_OK;
			}

			/* move to the next frame information position */
			++curr;
		}
	}

	/* set the frame count */
	*frameCount = 0;

	/* return successfully */
	return SDStatus_OK;
#else
	return SDStatus_NotImplemented;
#endif
}

/* Get a list of the frame dimensions of this image. */
SDStatus
SDImage_GetFrameDimensions(SDImage   *_this,
                           SDGuid   **dimensions,
                           SDUInt32  *count)
{
#if 0
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we have a frame dimension list pointer */
	SDStatus_Require((dimensions != 0), SDStatus_ArgumentNull);

	/* ensure we have a count pointer */
	SDStatus_Require((count != 0), SDStatus_ArgumentNull);

	/* get the frame dimensions */
	{
		/* declarations */
		SDFrameInfo *curr;
		SDFrameInfo *end;
		SDGuid      *dim;

		/* get the count */
		*count = _this->frameDimensionCount;

		/* allocate the frame dimension list */
		if(!(*dimensions = (SDGuid *)SDMalloc((*count) * sizeof(SDGuid))))
		{
			return SDStatus_OutOfMemory;
		}

		/* get the input pointer */
		curr = _this->frameDimensions;

		/* get the end of input pointer */
		end = (curr + (*count));

		/* get the output pointer */
		dim = *dimensions;

		/* get the frame dimensions */
		while(curr != end)
		{
			/* get the current guid */
			*dim = curr->guid;

			/* move to the next input position */
			++curr;

			/* move to the next output position */
			++dim;
		}
	}

	/* return successfully */
	return SDStatus_OK;
#else
	return SDStatus_NotImplemented;
#endif
}

/* Select a new frame and make it the active one. */
SDStatus
SDImage_SelectActiveFrame(SDImage  *_this,
                          SDGuid    dimension,
                          SDUInt32  frameIndex)
{
#if 0
	/* ensure we have a this pointer */
	SDStatus_Require((_this != 0), SDStatus_ArgumentNull);

	/* ensure we aren't locked (SDBitmap_LockBits) */
	SDStatus_Require
		((_this->lockMode == SDImageLockMode_None), SDStatus_InvalidOperation);

	/* select the frame */
	{
		/* declarations */
		SDFrameInfo *curr;
		SDFrameInfo *end;

		/* get the input pointer */
		curr = _this->frameDimensions;

		/* get the end of input pointer */
		end = (curr + (_this->frameDimensionCount));

		/* select the frame */
		while(curr != end)
		{
			/* select the frame from the matching dimension */
			if(SDMemCmp(&(curr->guid), &dimension, sizeof(SDGuid)) == 0)
			{
				/* declarations */
				SDFrame *tmp;

				/* ensure we have a valid frame index */
				SDStatus_Require
					((index < (curr->count)), SDStatus_ArgumentOutOfRange);

				/* get the frame */
				SDStatus_Check
					(SDFrame_Clone
						(curr->frames[index], &tmp, SDPixelFormat_32bppPArgb));

				/* dispose of the current active buffer frame */
				SDFrame_Destroy(&(_this->activeBuffer));

				/* set the active frame */
				_this->active = &(curr->frames[index]);

				/* set the active buffer frame */
				_this->activeBuffer = tmp;

				/* return successfully */
				return SDStatus_OK;
			}

			/* move to the next frame information position */
			++curr;
		}
	}

	/* return unsuccessfully */
	return SDStatus_Argument;
#else
	return SDStatus_NotImplemented;
#endif
}
/******************************************************************************/

#ifdef __cplusplus
};
#endif
