/*
 * SDUtils.c - Utilities implementation.
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

#include "CUtils.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Convert a value from one unit to another. */
SDINTERNAL SDFloat
SDUtils_ConvertUnits(SDGraphicsUnit fromUnit,
                     SDGraphicsUnit toUnit,
                     SDFloat        value)
{
	/* get the vertical resolution */
	const SDFloat dpiY = SDGraphics_DefaultDpiY;

	/* return the converted value */
	return SDUtils_ConvertUnitsDPI(fromUnit, toUnit, value, dpiY, dpiY);
}
SDINTERNAL SDFloat
SDUtils_ConvertUnitsDPI(SDGraphicsUnit fromUnit,
                        SDGraphicsUnit toUnit,
                        SDFloat        value,
                        SDFloat        fromDpiY,
                        SDFloat        toDpiY)
{
	/* bail out now if there's nothing to do */
	if(fromUnit == toUnit) { return value; }

	/*\
	|*| NOTE: we assume we have inch input, convert to inch scaled target, then
	|*|       fix up to scale to the actual target, to place multiplication
	|*|       before division
	\*/

	/* convert the value from the starting value to inch scale */
	switch(toUnit)
	{
		case SDGraphicsUnit_World:
		case SDGraphicsUnit_Pixel:      { value *= toDpiY;         } break;
		case SDGraphicsUnit_Display:    { value *= 75.0f;          } break;
		case SDGraphicsUnit_Point:      { value *= 72.0f;          } break;
		case SDGraphicsUnit_Inch:       { /* nothing to do here */ } break;
		case SDGraphicsUnit_Document:   { value *= 300.0f;         } break;
		case SDGraphicsUnit_Millimeter: { value *= 25.4f;          } break;
	}

	/* convert the value from inch scale to the target units */
	switch(fromUnit)
	{
		case SDGraphicsUnit_World:
		case SDGraphicsUnit_Pixel:      { value /= fromDpiY;       } break;
		case SDGraphicsUnit_Display:    { value /= 75.0f;          } break;
		case SDGraphicsUnit_Point:      { value /= 72.0f;          } break;
		case SDGraphicsUnit_Inch:       { /* nothing to do here */ } break;
		case SDGraphicsUnit_Document:   { value /= 300.0f;         } break;
		case SDGraphicsUnit_Millimeter: { value /= 25.4;           } break;
	}

	/* return the converted value */
	return value;
}


/* TODO: should we support 0xC080 encoded nulls? */

#define SDUtils_Char8Length(c) \
	(((c) < 0x80) ? 1 : \
     (((c) < 0x800) ? 2 : \
      (((c) < 0x10000) ? 3 : \
       (((c) < 0x200000) ? 4 : \
        (((c) < 0x4000000) ? 5 : 6)))))

#define SDUtils_SurrogatesToChar32(hi, lo) \
	(((((SDChar32)hi) - 0xD800) << 10) + (((SDChar32)lo) & 0x03FF) + 0x10000)

/* Convert a UTF16 string to a UTF8 string. */
SDINTERNAL SDStatus
SDUtils_Char16ToChar8(SDChar16  *string,
                      SDChar8  **result)
{
	/* declarations */
	SDChar8  *end;
	SDChar8  *dst;
	SDChar16 *src;
	SDUInt32  count;

	/* get the character pointer */
	src = string;

	/* set the count to the default */
	count = 0;

	/* calculate the UTF8 length */
	while(*src)
	{
		/* declarations */
		SDChar32 c32;
		SDChar16 c16;

		/* get the current character */
		c16 = *src;

		/* process the current character, based on range */
		if(c16 < 0xD800 || c16 > 0xDBFF)
		{
			/* ensure we don't have a low surrogate */
			SDStatus_Require
				((c16 < 0xDC00 || c16 > 0xDFFF), SDStatus_Argument);

			/* set the full character */
			c32 = (SDChar32)c16;
		}
		else
		{
			/* declarations */
			SDChar16 hi;

			/* get the high surrogate */
			hi = c16;

			/* get the low surrogate and update the input pointer */
			c16 = *(++src);

			/* ensure we have a low surrogate */
			SDStatus_Require
				((c16 > 0xDBFF && c16 < 0xE000), SDStatus_Argument);

			/* combine the surrogates to get the full character */
			c32 = SDUtils_SurrogatesToChar32(hi, c16);
		}

		/* update the count */
		count += SDUtils_Char8Length(c32);

		/* move to the next character position */
		++c16;
	}

	/* allocate the UTF8 string */
	if(!(*result = (SDChar8 *)SDMalloc((count + 1) * sizeof(SDChar8))))
	{
		return SDStatus_OutOfMemory;
	}

	/* get the input character pointer */
	src = string;

	/* get the output character pointer */
	dst = *result;

	/* get the end of output pointer */
	end = (dst + count);

	/* generate the UTF8 output */
	while(dst != end)
	{
		/* declarations */
		SDChar16 c16;
		SDChar32 c32;

		/* get the current character */
		c16 = *src;

		/* get the current input character in UTF32 */
		if(c16 < 0xD800 || c16 > 0xDBFF)
		{
			/* set the full character */
			c32 = (SDChar32)c16;
		}
		else
		{
			/* declarations */
			SDChar16 hi;

			/* get the high surrogate */
			hi = c16;

			/* get the low surrogate and update the input position */
			c16 = *(++src);

			/* get the UTF32 character */
			c32 = SDUtils_SurrogatesToChar32(hi, c16);
		}

		/* encode the full character into the output buffer */
		if(c32 < 0x80)
		{
			*dst++ = (SDChar8)c32;
		}
		else if(c32 < 0x800)
		{
			*dst++ = (SDChar8)(0xC0 | ((c32 >> 6) & 0xFF));
			*dst++ = (SDChar8)(0x80 | ((c32 >> 0) & 0x3F));
		}
		else if(c32 < 0x10000)
		{
			*dst++ = (SDChar8)(0xE0 | ((c32 >> 12) & 0xFF));
			*dst++ = (SDChar8)(0x80 | ((c32 >>  6) & 0x3F));
			*dst++ = (SDChar8)(0x80 | ((c32 >>  0) & 0x3F));
		}
		else if(c32 < 0x200000)
		{
			*dst++ = (SDChar8)(0xF0 | ((c32 >> 18) & 0xFF));
			*dst++ = (SDChar8)(0x80 | ((c32 >> 12) & 0x3F));
			*dst++ = (SDChar8)(0x80 | ((c32 >>  6) & 0x3F));
			*dst++ = (SDChar8)(0x80 | ((c32 >>  0) & 0x3F));
		}
		else if(c32 < 0x4000000)
		{
			*dst++ = (SDChar8)(0xF8 | ((c32 >> 24) & 0xFF));
			*dst++ = (SDChar8)(0x80 | ((c32 >> 18) & 0x3F));
			*dst++ = (SDChar8)(0x80 | ((c32 >> 12) & 0x3F));
			*dst++ = (SDChar8)(0x80 | ((c32 >>  6) & 0x3F));
			*dst++ = (SDChar8)(0x80 | ((c32 >>  0) & 0x3F));
		}
		else
		{
			*dst++ = (SDChar8)(0xFC | ((c32 >> 30) & 0xFF));
			*dst++ = (SDChar8)(0x80 | ((c32 >> 24) & 0x3F));
			*dst++ = (SDChar8)(0x80 | ((c32 >> 18) & 0x3F));
			*dst++ = (SDChar8)(0x80 | ((c32 >> 12) & 0x3F));
			*dst++ = (SDChar8)(0x80 | ((c32 >>  6) & 0x3F));
			*dst++ = (SDChar8)(0x80 | ((c32 >>  0) & 0x3F));
		}

		/* move to the next input position */
		++src;
	}

	/* null terminate the output */
	*dst = '\0';

	/* return successfully */
	return SDStatus_OK;
}

static SDStatus
SDUtils_ToPixmanImage64bppPArgb(SDByte         *scan0,
                                pixman_image_t *image,
                                SDUInt32        x,
                                SDUInt32        y,
                                SDUInt32        width,
                                SDUInt32        height,
                                SDUInt32        stride)
{
	/* TODO */
	return SDStatus_NotImplemented;
}

static SDStatus
SDUtils_ToPixmanImage64bppArgb(SDByte         *scan0,
                               pixman_image_t *image,
                               SDUInt32        x,
                               SDUInt32        y,
                               SDUInt32        width,
                               SDUInt32        height,
                               SDUInt32        stride)
{
	/* TODO */
	return SDStatus_NotImplemented;
}

static SDStatus
SDUtils_ToPixmanImage48bppRgb(SDByte         *scan0,
                              pixman_image_t *image,
                              SDUInt32        x,
                              SDUInt32        y,
                              SDUInt32        width,
                              SDUInt32        height,
                              SDUInt32        stride)
{
	/* TODO */
	return SDStatus_NotImplemented;
}

static SDStatus
SDUtils_ToPixmanImage32bppPArgb(SDByte         *scan0,
                                pixman_image_t *image,
                                SDUInt32        x,
                                SDUInt32        y,
                                SDUInt32        width,
                                SDUInt32        height,
                                SDUInt32        stride)
{
	/* declarations */
	SDByte   *data;
	SDUInt32  strideC;

	/* assertions */
	SDASSERT((scan0 != 0));
	SDASSERT((image != 0));

	/* get the data */
	data = (SDByte *)pixman_image_get_data(image);

	/* get the stride */
	strideC = pixman_image_get_stride(image);

	/* copy the data, line by line */
	while(y < height)
	{
		/* declarations */
		SDColor  *currC;
		SDByte   *currP;
		SDUInt32  pos;

		/* get the current color pointer */
		currC = (SDColor *)((data + (y * strideC)) + (x << 2));

		/* get the current pixel pointer */
		currP = ((scan0 + (y * stride)) + (x << 2));

		/* copy the color data, pixel by pixel */
		for(pos = x; pos < width; ++pos)
		{
			/* get the color components */
			const SDByte b = *currP++;
			const SDByte g = *currP++;
			const SDByte r = *currP++;
			const SDByte a = *currP++;

			/* get the pixel color */
			*currC++ = SDPixmanPixel_FromARGB(a, r, g, b);
		}

		/* update the y position */
		++y;
	}

	/* return successfully */
	return SDStatus_OK;
}

static SDStatus
SDUtils_ToPixmanImage32bppArgb(SDByte         *scan0,
                               pixman_image_t *image,
                               SDUInt32        x,
                               SDUInt32        y,
                               SDUInt32        width,
                               SDUInt32        height,
                               SDUInt32        stride)
{
	/* declarations */
	SDByte   *data;
	SDUInt32  strideC;

	/* assertions */
	SDASSERT((scan0 != 0));
	SDASSERT((image != 0));

	/* get the data */
	data = (SDByte *)pixman_image_get_data(image);

	/* get the stride */
	strideC = pixman_image_get_stride(image);

	/* copy the data, line by line */
	while(y < height)
	{
		/* declarations */
		SDColor  *currC;
		SDByte   *currP;
		SDUInt32  pos;

		/* get the current color pointer */
		currC = (SDColor *)((data + (y * strideC)) + (x << 2));

		/* get the current pixel pointer */
		currP = ((scan0 + (y * stride)) + (x << 2));

		/* copy the color data, pixel by pixel */
		for(pos = x; pos < width; ++pos)
		{
			/* get the color components */
			const SDByte b = *currP++;
			const SDByte g = *currP++;
			const SDByte r = *currP++;
			const SDByte a = *currP++;

			/* get the pixel color based on the transparency */
			if(a == 0)
			{
				*currC++ = SDPixmanPixel_FromARGB(0, 0, 0, 0);
			}
			else
			{
				*currC++ =
					SDPixmanPixel_FromARGB
						(a, ((r * a) / 255), ((g * a) / 255), ((b * a) / 255));
			}
		}

		/* update the y position */
		++y;
	}

	/* return successfully */
	return SDStatus_OK;
}

static SDStatus
SDUtils_ToPixmanImage32bppRgb(SDByte         *scan0,
                              pixman_image_t *image,
                              SDUInt32        x,
                              SDUInt32        y,
                              SDUInt32        width,
                              SDUInt32        height,
                              SDUInt32        stride)
{
	/* declarations */
	SDByte   *data;
	SDUInt32  strideC;

	/* assertions */
	SDASSERT((scan0 != 0));
	SDASSERT((image != 0));

	/* get the data */
	data = (SDByte *)pixman_image_get_data(image);

	/* get the stride */
	strideC = pixman_image_get_stride(image);

	/* copy the data, line by line */
	while(y < height)
	{
		/* declarations */
		SDColor  *currC;
		SDByte   *currP;
		SDUInt32  pos;

		/* get the current color pointer */
		currC = (SDColor *)((data + (y * strideC)) + (x << 2));

		/* get the current pixel pointer */
		currP = ((scan0 + (y * stride)) + (x << 2));

		/* copy the color data, pixel by pixel */
		for(pos = x; pos < width; ++pos)
		{
			/* get the color components */
			const SDByte b = *currP++;
			const SDByte g = *currP++;
			const SDByte r = *currP++;

			/* get the pixel color */
			*currC++ = SDPixmanPixel_FromARGB(0xFF, r, g, b);

			/* update the pixel position */
			++currP;
		}

		/* update the y position */
		++y;
	}

	/* return successfully */
	return SDStatus_OK;
}

static SDStatus
SDUtils_ToPixmanImage24bppRgb(SDByte         *scan0,
                              pixman_image_t *image,
                              SDUInt32        x,
                              SDUInt32        y,
                              SDUInt32        width,
                              SDUInt32        height,
                              SDUInt32        stride)
{
	/* declarations */
	SDByte   *data;
	SDUInt32  strideC;

	/* assertions */
	SDASSERT((scan0 != 0));
	SDASSERT((image != 0));

	/* get the data */
	data = (SDByte *)pixman_image_get_data(image);

	/* get the stride */
	strideC = pixman_image_get_stride(image);

	/* copy the data, line by line */
	while(y < height)
	{
		/* declarations */
		SDColor  *currC;
		SDByte   *currP;
		SDUInt32  pos;

		/* get the current color pointer */
		currC = (SDColor *)((data + (y * strideC)) + (x << 2));

		/* get the current pixel pointer */
		currP = ((scan0 + (y * stride)) + ((x << 1) + x));

		/* copy the color data, pixel by pixel */
		for(pos = x; pos < width; ++pos)
		{
			/* get the color components */
			const SDByte b = *currP++;
			const SDByte g = *currP++;
			const SDByte r = *currP++;

			/* get the pixel color */
			*currC++ = SDPixmanPixel_FromARGB(0xFF, r, g, b);
		}

		/* update the y position */
		++y;
	}

	/* return successfully */
	return SDStatus_OK;
}

static SDStatus
SDUtils_ToPixmanImage16bppArgb1555(SDByte         *scan0,
                                   pixman_image_t *image,
                                   SDUInt32        x,
                                   SDUInt32        y,
                                   SDUInt32        width,
                                   SDUInt32        height,
                                   SDUInt32        stride)
{
	/* declarations */
	SDByte   *data;
	SDUInt32  strideC;

	/* assertions */
	SDASSERT((scan0 != 0));
	SDASSERT((image != 0));

	/* get the data */
	data = (SDByte *)pixman_image_get_data(image);

	/* get the stride */
	strideC = pixman_image_get_stride(image);

	/* copy the data, line by line */
	while(y < height)
	{
		/* declarations */
		SDColor  *currC;
		SDByte   *currP;
		SDUInt32  pos;

		/* get the current color pointer */
		currC = (SDColor *)((data + (y * strideC)) + (x << 2));

		/* get the current pixel pointer */
		currP = ((scan0 + (y * stride)) + (x << 1));

		/* copy the color data, pixel by pixel */
		for(pos = x; pos < width; ++pos)
		{
			/* declarations */
			SDByte r, g, b;

			/* get the color components */
			g = *currP++;
			r = *currP++;

			/* get the pixel color based on transparency */
			if((r & 0x80) == 0)
			{
				*currC++ = SDPixmanPixel_FromARGB(0, 0, 0, 0);
			}
			else
			{
				/* extract the color components */
				b = (SDByte)(g & 0x1F);
				g = (SDByte)(((r << 3) & 0x18) | ((g >> 5) & 0x07));
				r = (SDByte)((r >> 2) & 0x1F);

				/* scale the color components */
				b = (SDByte)(((b << 8) - b) / 31);
				g = (SDByte)(((g << 8) - g) / 31);
				r = (SDByte)(((r << 8) - r) / 31);

				/* get the pixel color */
				*currC++ = SDPixmanPixel_FromARGB(0xFF, r, g, b);
			}
		}

		/* update the y position */
		++y;
	}

	/* return successfully */
	return SDStatus_OK;
}

static SDStatus
SDUtils_ToPixmanImage16bppRgb565(SDByte         *scan0,
                                 pixman_image_t *image,
                                 SDUInt32        x,
                                 SDUInt32        y,
                                 SDUInt32        width,
                                 SDUInt32        height,
                                 SDUInt32        stride)
{
	/* declarations */
	SDByte   *data;
	SDUInt32  strideC;

	/* assertions */
	SDASSERT((scan0 != 0));
	SDASSERT((image != 0));

	/* get the data */
	data = (SDByte *)pixman_image_get_data(image);

	/* get the stride */
	strideC = pixman_image_get_stride(image);

	/* copy the data, line by line */
	while(y < height)
	{
		/* declarations */
		SDColor  *currC;
		SDByte   *currP;
		SDUInt32  pos;

		/* get the current color pointer */
		currC = (SDColor *)((data + (y * strideC)) + (x << 2));

		/* get the current pixel pointer */
		currP = ((scan0 + (y * stride)) + (x << 1));

		/* copy the color data, pixel by pixel */
		for(pos = x; pos < width; ++pos)
		{
			/* declarations */
			SDByte r, g, b;

			/* get the color components */
			g = *currP++;
			r = *currP++;

			/* extract the color components */
			b = (SDByte)(g & 0x1F);
			g = (SDByte)(((r << 3) & 0x38) | ((g >> 5) & 0x07));
			r = (SDByte)(r >> 3);

			/* scale the color components */
			b = (SDByte)(((b << 8) - b) / 31);
			g = (SDByte)(((g << 8) - g) / 63);
			r = (SDByte)(((r << 8) - r) / 31);

			/* get the pixel color */
			*currC++ = SDPixmanPixel_FromARGB(0xFF, r, g, b);
		}

		/* update the y position */
		++y;
	}

	/* return successfully */
	return SDStatus_OK;
}

static SDStatus
SDUtils_ToPixmanImage16bppRgb555(SDByte         *scan0,
                                 pixman_image_t *image,
                                 SDUInt32        x,
                                 SDUInt32        y,
                                 SDUInt32        width,
                                 SDUInt32        height,
                                 SDUInt32        stride)
{
	/* declarations */
	SDByte   *data;
	SDUInt32  strideC;

	/* assertions */
	SDASSERT((scan0 != 0));
	SDASSERT((image != 0));

	/* get the data */
	data = (SDByte *)pixman_image_get_data(image);

	/* get the stride */
	strideC = pixman_image_get_stride(image);

	/* copy the data, line by line */
	while(y < height)
	{
		/* declarations */
		SDColor  *currC;
		SDByte   *currP;
		SDUInt32  pos;

		/* get the current color pointer */
		currC = (SDColor *)((data + (y * strideC)) + (x << 2));

		/* get the current pixel pointer */
		currP = ((scan0 + (y * stride)) + (x << 1));

		/* copy the color data, pixel by pixel */
		for(pos = x; pos < width; ++pos)
		{
			/* declarations */
			SDByte r, g, b;

			/* get the color components */
			g = *currP++;
			r = *currP++;

			/* extract the color components */
			b = (SDByte)(g & 0x1F);
			g = (SDByte)(((r << 3) & 0x18) | ((g >> 5) & 0x07));
			r = (SDByte)((r >> 2) & 0x1F);

			/* scale the color components */
			b = (SDByte)(((b << 8) - b) / 31);
			g = (SDByte)(((g << 8) - g) / 31);
			r = (SDByte)(((r << 8) - r) / 31);

			/* get the pixel color */
			*currC++ = SDPixmanPixel_FromARGB(0xFF, r, g, b);
		}

		/* update the y position */
		++y;
	}

	/* return successfully */
	return SDStatus_OK;
}

static SDStatus
SDUtils_ToPixmanImage16bppGrayScale(SDByte         *scan0,
                                    pixman_image_t *image,
                                    SDUInt32        x,
                                    SDUInt32        y,
                                    SDUInt32        width,
                                    SDUInt32        height,
                                    SDUInt32        stride)
{
	/* declarations */
	SDByte   *data;
	SDUInt32  strideC;

	/* assertions */
	SDASSERT((scan0 != 0));
	SDASSERT((image != 0));

	/* get the data */
	data = (SDByte *)pixman_image_get_data(image);

	/* get the stride */
	strideC = pixman_image_get_stride(image);

	/* copy the data, line by line */
	while(y < height)
	{
		/* declarations */
		SDColor  *currC;
		SDByte   *currP;
		SDUInt32  pos;

		/* get the current color pointer */
		currC = (SDColor *)((data + (y * strideC)) + (x << 2));

		/* get the current pixel pointer (offset for the high byte) */
		currP = ((scan0 + (y * stride)) + ((x << 1) + 1));

		/* copy the color data, pixel by pixel */
		for(pos = x; pos < width; ++pos)
		{
			/* get the gray value */
			const SDByte val = *currP++; ++currP;

			/* get the pixel color */
			*currC++ = SDPixmanPixel_FromARGB(0xFF, val, val, val);
		}

		/* update the y position */
		++y;
	}

	/* return successfully */
	return SDStatus_OK;
}

static SDStatus
SDUtils_ToPixmanImage8bppIndexed(SDByte         *scan0,
                                 pixman_image_t *image,
                                 SDUInt32        x,
                                 SDUInt32        y,
                                 SDUInt32        width,
                                 SDUInt32        height,
                                 SDUInt32        stride,
                                 SDColorPalette *palette)
{
	/* TODO */
	return SDStatus_NotImplemented;
}

static SDStatus
SDUtils_ToPixmanImage4bppIndexed(SDByte         *scan0,
                                 pixman_image_t *image,
                                 SDUInt32        x,
                                 SDUInt32        y,
                                 SDUInt32        width,
                                 SDUInt32        height,
                                 SDUInt32        stride,
                                 SDColorPalette *palette)
{
	/* TODO */
	return SDStatus_NotImplemented;
}

static SDStatus
SDUtils_ToPixmanImage1bppIndexed(SDByte         *scan0,
                                 pixman_image_t *image,
                                 SDUInt32        x,
                                 SDUInt32        y,
                                 SDUInt32        width,
                                 SDUInt32        height,
                                 SDUInt32        stride,
                                 SDColorPalette *palette)
{
	/* TODO */
	return SDStatus_NotImplemented;
}

static SDStatus
SDUtils_FromPixmanImage64bppPArgb(pixman_image_t *image,
                                  SDByte         *scan0,
                                  SDUInt32        x,
                                  SDUInt32        y,
                                  SDUInt32        width,
                                  SDUInt32        height,
                                  SDUInt32        stride)
{
	/* TODO */
	return SDStatus_NotImplemented;
}

static SDStatus
SDUtils_FromPixmanImage64bppArgb(pixman_image_t *image,
                                 SDByte         *scan0,
                                 SDUInt32        x,
                                 SDUInt32        y,
                                 SDUInt32        width,
                                 SDUInt32        height,
                                 SDUInt32        stride)
{
	/* TODO */
	return SDStatus_NotImplemented;
}

static SDStatus
SDUtils_FromPixmanImage48bppRgb(pixman_image_t *image,
                                SDByte         *scan0,
                                SDUInt32        x,
                                SDUInt32        y,
                                SDUInt32        width,
                                SDUInt32        height,
                                SDUInt32        stride)
{
	/* TODO */
	return SDStatus_NotImplemented;
}

static SDStatus
SDUtils_FromPixmanImage32bppPArgb(pixman_image_t *image,
                                  SDByte         *scan0,
                                  SDUInt32        x,
                                  SDUInt32        y,
                                  SDUInt32        width,
                                  SDUInt32        height,
                                  SDUInt32        stride)
{
	/* declarations */
	SDByte   *data;
	SDUInt32  strideC;

	/* assertions */
	SDASSERT((scan0 != 0));
	SDASSERT((image != 0));

	/* get the data */
	data = (SDByte *)pixman_image_get_data(image);

	/* get the stride */
	strideC = pixman_image_get_stride(image);

	/* copy the data, line by line */
	while(y < height)
	{
		/* declarations */
		SDColor  *currC;
		SDByte   *currP;
		SDUInt32  pos;

		/* get the current color pointer */
		currC = (SDColor *)((data + (y * strideC)) + (x << 2));

		/* get the current pixel pointer */
		currP = ((scan0 + (y * stride)) + (x << 2));

		/* copy the color data, pixel by pixel */
		for(pos = x; pos < width; ++pos)
		{
			/* declarations */
			SDByte a, r, g, b;

			/* get the color components */
			SDPixmanPixel_ToARGB(*currC, a, r, g, b);

			/* set the pixel color */
			*currP++ = b;
			*currP++ = g;
			*currP++ = r;
			*currP++ = a;

			/* move to the next pixel position */
			++currC;
		}

		/* update the y position */
		++y;
	}

	/* return successfully */
	return SDStatus_OK;
}

static SDStatus
SDUtils_FromPixmanImage32bppArgb(pixman_image_t *image,
                                 SDByte         *scan0,
                                 SDUInt32        x,
                                 SDUInt32        y,
                                 SDUInt32        width,
                                 SDUInt32        height,
                                 SDUInt32        stride)
{
	/* declarations */
	SDByte   *data;
	SDUInt32  strideC;

	/* assertions */
	SDASSERT((scan0 != 0));
	SDASSERT((image != 0));

	/* get the data */
	data = (SDByte *)pixman_image_get_data(image);

	/* get the stride */
	strideC = pixman_image_get_stride(image);

	/* copy the data, line by line */
	while(y < height)
	{
		/* declarations */
		SDColor  *currC;
		SDByte   *currP;
		SDUInt32  pos;

		/* get the current color pointer */
		currC = (SDColor *)((data + (y * strideC)) + (x << 2));

		/* get the current pixel pointer */
		currP = ((scan0 + (y * stride)) + (x << 2));

		/* copy the color data, pixel by pixel */
		for(pos = x; pos < width; ++pos)
		{
			/* declarations */
			SDByte a, r, g, b;

			/* get the color components */
			SDPixmanPixel_ToARGB(*currC, a, r, g, b);

			/* set the pixel color based on the transparency */
			if(a == 0)
			{
				*currP++ = 0;
				*currP++ = 0;
				*currP++ = 0;
				*currP++ = 0;
			}
			else
			{
				*currP++ = ((SDByte)(((b << 8) - b) / a));
				*currP++ = ((SDByte)(((g << 8) - g) / a));
				*currP++ = ((SDByte)(((r << 8) - r) / a));
				*currP++ = a;
			}

			/* move to the next pixel position */
			++currC;
		}

		/* update the y position */
		++y;
	}

	/* return successfully */
	return SDStatus_OK;
}

static SDStatus
SDUtils_FromPixmanImage32bppRgb(pixman_image_t *image,
                                SDByte         *scan0,
                                SDUInt32        x,
                                SDUInt32        y,
                                SDUInt32        width,
                                SDUInt32        height,
                                SDUInt32        stride)
{
	/* declarations */
	SDByte   *data;
	SDUInt32  strideC;

	/* assertions */
	SDASSERT((scan0 != 0));
	SDASSERT((image != 0));

	/* get the data */
	data = (SDByte *)pixman_image_get_data(image);

	/* get the stride */
	strideC = pixman_image_get_stride(image);

	/* copy the data, line by line */
	while(y < height)
	{
		/* declarations */
		SDColor  *currC;
		SDByte   *currP;
		SDUInt32  pos;

		/* get the current color pointer */
		currC = (SDColor *)((data + (y * strideC)) + (x << 2));

		/* get the current pixel pointer */
		currP = ((scan0 + (y * stride)) + (x << 2));

		/* copy the color data, pixel by pixel */
		for(pos = x; pos < width; ++pos)
		{
			/* declarations */
			SDByte a, r, g, b;

			/* get the color components */
			SDPixmanPixel_ToARGB(*currC, a, r, g, b);

			/* set the pixel color */
			*currP++ = b;
			*currP++ = g;
			*currP++ = r;
			*currP++ = 0;

			/* move to the next pixel position */
			++currC;
		}

		/* update the y position */
		++y;
	}

	/* return successfully */
	return SDStatus_OK;
}

static SDStatus
SDUtils_FromPixmanImage24bppRgb(pixman_image_t *image,
                                SDByte         *scan0,
                                SDUInt32        x,
                                SDUInt32        y,
                                SDUInt32        width,
                                SDUInt32        height,
                                SDUInt32        stride)
{
	/* declarations */
	SDByte   *data;
	SDUInt32  strideC;

	/* assertions */
	SDASSERT((scan0 != 0));
	SDASSERT((image != 0));

	/* get the data */
	data = (SDByte *)pixman_image_get_data(image);

	/* get the stride */
	strideC = pixman_image_get_stride(image);

	/* copy the data, line by line */
	while(y < height)
	{
		/* declarations */
		SDColor  *currC;
		SDByte   *currP;
		SDUInt32  pos;

		/* get the current color pointer */
		currC = (SDColor *)((data + (y * strideC)) + (x << 2));

		/* get the current pixel pointer */
		currP = ((scan0 + (y * stride)) + ((x << 1) + x));

		/* copy the color data, pixel by pixel */
		for(pos = x; pos < width; ++pos)
		{
			/* declarations */
			SDByte a, r, g, b;

			/* get the color components */
			SDPixmanPixel_ToARGB(*currC, a, r, g, b);

			/* set the pixel color */
			*currP++ = b;
			*currP++ = g;
			*currP++ = r;

			/* move to the next pixel position */
			++currC;
		}

		/* update the y position */
		++y;
	}

	/* return successfully */
	return SDStatus_OK;
}

static SDStatus
SDUtils_FromPixmanImage16bppArgb1555(pixman_image_t *image,
                                     SDByte         *scan0,
                                     SDUInt32        x,
                                     SDUInt32        y,
                                     SDUInt32        width,
                                     SDUInt32        height,
                                     SDUInt32        stride)
{
	/* declarations */
	SDByte   *data;
	SDUInt32  strideC;

	/* assertions */
	SDASSERT((scan0 != 0));
	SDASSERT((image != 0));

	/* get the data */
	data = (SDByte *)pixman_image_get_data(image);

	/* get the stride */
	strideC = pixman_image_get_stride(image);

	/* copy the data, line by line */
	while(y < height)
	{
		/* declarations */
		SDColor  *currC;
		SDByte   *currP;
		SDUInt32  pos;

		/* get the current color pointer */
		currC = (SDColor *)((data + (y * strideC)) + (x << 2));

		/* get the current pixel pointer */
		currP = ((scan0 + (y * stride)) + (x << 1));

		/* copy the color data, pixel by pixel */
		for(pos = x; pos < width; ++pos)
		{
			/* declarations */
			SDByte a, r, g, b;

			/* get the color components */
			SDPixmanPixel_ToARGB(*currC, a, r, g, b);

			/* set the pixel color based on transparency */
			if(a < 128)
			{
				*currP++ = 0;
				*currP++ = 0;
			}
			else
			{
				/* set the pixel color */
				*currP++ = ((SDByte)(((g << 2) & 0xE0) | ((b >> 3) & 0x1F)));
				*currP++ =
					((SDByte)(((r >> 1) & 0x7C) | ((g >> 6) & 0x03) | 0x80));
			}

			/* move to the next pixel position */
			++currC;
		}

		/* update the y position */
		++y;
	}

	/* return successfully */
	return SDStatus_OK;
}

static SDStatus
SDUtils_FromPixmanImage16bppRgb565(pixman_image_t *image,
                                   SDByte         *scan0,
                                   SDUInt32        x,
                                   SDUInt32        y,
                                   SDUInt32        width,
                                   SDUInt32        height,
                                   SDUInt32        stride)
{
	/* declarations */
	SDByte   *data;
	SDUInt32  strideC;

	/* assertions */
	SDASSERT((scan0 != 0));
	SDASSERT((image != 0));

	/* get the data */
	data = (SDByte *)pixman_image_get_data(image);

	/* get the stride */
	strideC = pixman_image_get_stride(image);

	/* copy the data, line by line */
	while(y < height)
	{
		/* declarations */
		SDColor  *currC;
		SDByte   *currP;
		SDUInt32  pos;

		/* get the current color pointer */
		currC = (SDColor *)((data + (y * strideC)) + (x << 2));

		/* get the current pixel pointer */
		currP = ((scan0 + (y * stride)) + (x << 1));

		/* copy the color data, pixel by pixel */
		for(pos = x; pos < width; ++pos)
		{
			/* declarations */
			SDByte a, r, g, b;

			/* get the color components */
			SDPixmanPixel_ToARGB(*currC, a, r, g, b);

			/* set the pixel color based on transparency */
			if(a < 128)
			{
				*currP++ = 0;
				*currP++ = 0;
			}
			else
			{
				/* set the pixel color */
				*currP++ = ((SDByte)(((g << 3) & 0xE0) | ((b >> 3) & 0x1F)));
				*currP++ = ((SDByte)(((r << 0) & 0xF8) | ((g >> 5) & 0x07)));
			}

			/* move to the next pixel position */
			++currC;
		}

		/* update the y position */
		++y;
	}

	/* return successfully */
	return SDStatus_OK;
}

static SDStatus
SDUtils_FromPixmanImage16bppRgb555(pixman_image_t *image,
                                   SDByte         *scan0,
                                   SDUInt32        x,
                                   SDUInt32        y,
                                   SDUInt32        width,
                                   SDUInt32        height,
                                   SDUInt32        stride)
{
	/* declarations */
	SDByte   *data;
	SDUInt32  strideC;

	/* assertions */
	SDASSERT((scan0 != 0));
	SDASSERT((image != 0));

	/* get the data */
	data = (SDByte *)pixman_image_get_data(image);

	/* get the stride */
	strideC = pixman_image_get_stride(image);

	/* copy the data, line by line */
	while(y < height)
	{
		/* declarations */
		SDColor  *currC;
		SDByte   *currP;
		SDUInt32  pos;

		/* get the current color pointer */
		currC = (SDColor *)((data + (y * strideC)) + (x << 2));

		/* get the current pixel pointer */
		currP = ((scan0 + (y * stride)) + (x << 1));

		/* copy the color data, pixel by pixel */
		for(pos = x; pos < width; ++pos)
		{
			/* declarations */
			SDByte a, r, g, b;

			/* get the color components */
			SDPixmanPixel_ToARGB(*currC, a, r, g, b);

			/* set the pixel color based on transparency */
			if(a < 128)
			{
				*currP++ = 0;
				*currP++ = 0;
			}
			else
			{
				/* set the pixel color */
				*currP++ = ((SDByte)(((g << 2) & 0xE0) | ((b >> 3) & 0x1F)));
				*currP++ = ((SDByte)(((r >> 1) & 0x7C) | ((g >> 6) & 0x03)));
			}

			/* move to the next pixel position */
			++currC;
		}

		/* update the y position */
		++y;
	}

	/* return successfully */
	return SDStatus_OK;
}

static SDStatus
SDUtils_FromPixmanImage16bppGrayScale(pixman_image_t *image,
                                      SDByte         *scan0,
                                      SDUInt32        x,
                                      SDUInt32        y,
                                      SDUInt32        width,
                                      SDUInt32        height,
                                      SDUInt32        stride)
{
	/* declarations */
	SDByte   *data;
	SDUInt32  strideC;

	/* assertions */
	SDASSERT((scan0 != 0));
	SDASSERT((image != 0));

	/* get the data */
	data = (SDByte *)pixman_image_get_data(image);

	/* get the stride */
	strideC = pixman_image_get_stride(image);

	/* copy the data, line by line */
	while(y < height)
	{
		/* declarations */
		SDColor  *currC;
		SDByte   *currP;
		SDUInt32  pos;

		/* get the current color pointer */
		currC = (SDColor *)((data + (y * strideC)) + (x << 2));

		/* get the current pixel pointer */
		currP = ((scan0 + (y * stride)) + (x << 1));

		/* copy the color data, pixel by pixel */
		for(pos = x; pos < width; ++pos)
		{
			/* declarations */
			SDByte a, r, g, b;

			/* get the color components */
			SDPixmanPixel_ToARGB(*currC, a, r, g, b);

			/* set the pixel color based on transparency */
			if(a < 128)
			{
				*currP++ = 0;
				*currP++ = 0;
			}
			else
			{
				/* declarations */
				SDUInt16 value;

				/* get the gray value */
				value = (SDUInt16)(255 * SDColor_IntensityRGB(r, g, b));

				/* set the pixel color */
				*currP++ = ((SDByte)(value & 0xFF));
				*currP++ = ((SDByte)(value >> 8));
			}

			/* move to the next pixel position */
			++currC;
		}

		/* update the y position */
		++y;
	}

	/* return successfully */
	return SDStatus_OK;
}

static SDStatus
SDUtils_FromPixmanImage8bppIndexed(pixman_image_t *image,
                                   SDByte         *scan0,
                                   SDUInt32        x,
                                   SDUInt32        y,
                                   SDUInt32        width,
                                   SDUInt32        height,
                                   SDUInt32        stride,
                                   SDColorPalette *palette)
{
	/* TODO */
	return SDStatus_NotImplemented;
}

static SDStatus
SDUtils_FromPixmanImage4bppIndexed(pixman_image_t *image,
                                   SDByte         *scan0,
                                   SDUInt32        x,
                                   SDUInt32        y,
                                   SDUInt32        width,
                                   SDUInt32        height,
                                   SDUInt32        stride,
                                   SDColorPalette *palette)
{
	/* TODO */
	return SDStatus_NotImplemented;
}

static SDStatus
SDUtils_FromPixmanImage1bppIndexed(pixman_image_t *image,
                                   SDByte         *scan0,
                                   SDUInt32        x,
                                   SDUInt32        y,
                                   SDUInt32        width,
                                   SDUInt32        height,
                                   SDUInt32        stride,
                                   SDColorPalette *palette)
{
	/* TODO */
	return SDStatus_NotImplemented;
}

/* Convert pixel data into pixman image data. */
SDINTERNAL SDStatus
SDUtils_ToPixmanImage(SDPixelFormat   format,
                      SDByte         *scan0,
                      pixman_image_t *image,
                      SDUInt32        x,
                      SDUInt32        y,
                      SDUInt32        width,
                      SDUInt32        height,
                      SDUInt32        stride,
                      SDColorPalette *palette)
{
	/* convert the image according to the format */
	switch(format)
	{
		default: { return SDStatus_NotSupported; }

		case SDPixelFormat_64bppPArgb:
		{
			return
				SDUtils_ToPixmanImage64bppPArgb
					(scan0, image, x, y, width, height, stride);
		}
		case SDPixelFormat_64bppArgb:
		{
			return
				SDUtils_ToPixmanImage64bppArgb
					(scan0, image, x, y, width, height, stride);
		}
		case SDPixelFormat_48bppRgb:
		{
			return
				SDUtils_ToPixmanImage48bppRgb
					(scan0, image, x, y, width, height, stride);
		}
		case SDPixelFormat_32bppPArgb:
		{
			return
				SDUtils_ToPixmanImage32bppPArgb
					(scan0, image, x, y, width, height, stride);
		}
		case SDPixelFormat_32bppArgb:
		{
			return
				SDUtils_ToPixmanImage32bppArgb
					(scan0, image, x, y, width, height, stride);
		}
		case SDPixelFormat_32bppRgb:
		{
			return
				SDUtils_ToPixmanImage32bppRgb
					(scan0, image, x, y, width, height, stride);
		}
		case SDPixelFormat_24bppRgb:
		{
			return
				SDUtils_ToPixmanImage24bppRgb
					(scan0, image, x, y, width, height, stride);
		}
		case SDPixelFormat_16bppArgb1555:
		{
			return
				SDUtils_ToPixmanImage16bppArgb1555
					(scan0, image, x, y, width, height, stride);
		}
		case SDPixelFormat_16bppRgb565:
		{
			return
				SDUtils_ToPixmanImage16bppRgb565
					(scan0, image, x, y, width, height, stride);
		}
		case SDPixelFormat_16bppRgb555:
		{
			return
				SDUtils_ToPixmanImage16bppRgb555
					(scan0, image, x, y, width, height, stride);
		}
		case SDPixelFormat_16bppGrayScale:
		{
			return
				SDUtils_ToPixmanImage16bppGrayScale
					(scan0, image, x, y, width, height, stride);
		}
		case SDPixelFormat_8bppIndexed:
		{
			return
				SDUtils_ToPixmanImage8bppIndexed
					(scan0, image, x, y, width, height, stride, palette);
		}
		case SDPixelFormat_4bppIndexed:
		{
			return
				SDUtils_ToPixmanImage4bppIndexed
					(scan0, image, x, y, width, height, stride, palette);
		}
		case SDPixelFormat_1bppIndexed:
		{
			return
				SDUtils_ToPixmanImage1bppIndexed
					(scan0, image, x, y, width, height, stride, palette);
		}
	}
}

/* Convert pixman image data into pixel data. */
SDINTERNAL SDStatus
SDUtils_FromPixmanImage(SDPixelFormat   format,
                        pixman_image_t *image,
                        SDByte         *scan0,
                        SDUInt32        x,
                        SDUInt32        y,
                        SDUInt32        width,
                        SDUInt32        height,
                        SDUInt32        stride,
                        SDColorPalette *palette)
{
	/* convert the image according to the format */
	switch(format)
	{
		default: { return SDStatus_NotSupported; }

		case SDPixelFormat_64bppPArgb:
		{
			return
				SDUtils_FromPixmanImage64bppPArgb
					(image, scan0, x, y, width, height, stride);
		}
		case SDPixelFormat_64bppArgb:
		{
			return
				SDUtils_FromPixmanImage64bppArgb
					(image, scan0, x, y, width, height, stride);
		}
		case SDPixelFormat_48bppRgb:
		{
			return
				SDUtils_FromPixmanImage48bppRgb
					(image, scan0, x, y, width, height, stride);
		}
		case SDPixelFormat_32bppPArgb:
		{
			return
				SDUtils_FromPixmanImage32bppPArgb
					(image, scan0, x, y, width, height, stride);
		}
		case SDPixelFormat_32bppArgb:
		{
			return
				SDUtils_FromPixmanImage32bppArgb
					(image, scan0, x, y, width, height, stride);
		}
		case SDPixelFormat_32bppRgb:
		{
			return
				SDUtils_FromPixmanImage32bppRgb
					(image, scan0, x, y, width, height, stride);
		}
		case SDPixelFormat_24bppRgb:
		{
			return
				SDUtils_FromPixmanImage24bppRgb
					(image, scan0, x, y, width, height, stride);
		}
		case SDPixelFormat_16bppArgb1555:
		{
			return
				SDUtils_FromPixmanImage16bppArgb1555
					(image, scan0, x, y, width, height, stride);
		}
		case SDPixelFormat_16bppRgb565:
		{
			return
				SDUtils_FromPixmanImage16bppRgb565
					(image, scan0, x, y, width, height, stride);
		}
		case SDPixelFormat_16bppRgb555:
		{
			return
				SDUtils_FromPixmanImage16bppRgb555
					(image, scan0, x, y, width, height, stride);
		}
		case SDPixelFormat_16bppGrayScale:
		{
			return
				SDUtils_FromPixmanImage16bppGrayScale
					(image, scan0, x, y, width, height, stride);
		}
		case SDPixelFormat_8bppIndexed:
		{
			return
				SDUtils_FromPixmanImage8bppIndexed
					(image, scan0, x, y, width, height, stride, palette);
		}
		case SDPixelFormat_4bppIndexed:
		{
			return
				SDUtils_FromPixmanImage4bppIndexed
					(image, scan0, x, y, width, height, stride, palette);
		}
		case SDPixelFormat_1bppIndexed:
		{
			return
				SDUtils_FromPixmanImage1bppIndexed
					(image, scan0, x, y, width, height, stride, palette);
		}
	}
}

SDINTERNAL SDStatus
SDUtils_GetPixmanPixelPointer(pixman_image_t  *image,
                              SDUInt32         x,
                              SDUInt32         y,
                              SDColor        **pixel)
{
	/* declarations */
	SDByte   *data;
	SDUInt32  width;
	SDUInt32  height;
	SDUInt32  stride;

	/* assertions */
	SDASSERT((image != 0));
	SDASSERT((pixel != 0));

	/* get the pixman image information */
	data   = (SDByte *)pixman_image_get_data(image);
	width  = (SDUInt32)pixman_image_get_width(image);
	height = (SDUInt32)pixman_image_get_height(image);
	stride = (SDUInt32)pixman_image_get_stride(image);

	/* ensure the pixel is within bounds */
	SDStatus_Require((x < width && y < height), SDStatus_ArgumentOutOfRange);

	/* get the pixel pointer */
	*pixel = ((SDColor *)(data + (y * stride) + (x << 2)));

	/* return successfully */
	return SDStatus_OK;
}

SDINTERNAL SDUInt32
SDUtils_FormatToStride(SDPixelFormat pixelFormat,
                       SDUInt32      width)
{
	return ((SDUtils_BytesPerLine(pixelFormat, width) + 3) & ~3);
}

SDINTERNAL SDUInt32
SDUtils_BytesPerLine(SDPixelFormat pixelFormat,
                     SDUInt32      width)
{
	switch(pixelFormat)
	{
		case SDPixelFormat_1bppIndexed:
			{ return ((width + 7) / 8); }

		case SDPixelFormat_4bppIndexed:
			{ return ((width + 1) / 2); }

		case SDPixelFormat_8bppIndexed:
			{ return (width * 1); }

		case SDPixelFormat_16bppRgb555:
		case SDPixelFormat_16bppRgb565:
		case SDPixelFormat_16bppArgb1555:
		case SDPixelFormat_16bppGrayScale:
			{ return (width * 2); }

		case SDPixelFormat_24bppRgb:
			{ return (width * 3); }

		case SDPixelFormat_32bppRgb:
		case SDPixelFormat_32bppPArgb:
		case SDPixelFormat_32bppArgb:
			{ return (width * 4); }

		case SDPixelFormat_48bppRgb:
			{ return (width * 6); }

		case SDPixelFormat_64bppPArgb:
		case SDPixelFormat_64bppArgb:
			{ return (width * 8); }

		default:
			{ return (width * 4); }
	}
}

SDINTERNAL pixman_transform_t
SDUtils_ToPixmanTransform(SDAffineTransformF *transform)
{
	/* declarations */
	pixman_transform_t pt;
	SDFloat            xx;
	SDFloat            xy;
	SDFloat            yx;
	SDFloat            yy;
	SDFloat            dx;
	SDFloat            dy;

	/* assertions */
	SDASSERT((transform != 0));

	/* get the transformation components */
	{
		xx = SDAffineTransform_XX(*transform);
		xy = SDAffineTransform_XY(*transform);
		yx = SDAffineTransform_YX(*transform);
		yy = SDAffineTransform_YY(*transform);
		dx = SDAffineTransform_DX(*transform);
		dy = SDAffineTransform_DX(*transform);
	}

	/* initialize the pixman transformation */
	{
		pt.matrix[0][0] = SDFloat_ToFixed(xx);
		pt.matrix[0][1] = SDFloat_ToFixed(xy);
		pt.matrix[0][2] = SDFloat_ToFixed(dx);
		pt.matrix[1][0] = SDFloat_ToFixed(yx);
		pt.matrix[1][1] = SDFloat_ToFixed(yy);
		pt.matrix[1][2] = SDFloat_ToFixed(dy);
		pt.matrix[2][0] = SDFixed_Zero;
		pt.matrix[2][1] = SDFixed_Zero;
		pt.matrix[2][2] = SDFixed_One;
	}

	/* return the pixman transformation */
	return pt;
}

SDINTERNAL SDStatus
SDUtils_CreateSolidPattern(pixman_image_t **pattern,
                           SDColor          color)
{
	/* assertions */
	SDASSERT((pattern != 0));

	/* create the pattern */
	{
		/* declarations */
		pixman_format_t *format;

		/* create the pixman format */
		format = pixman_format_create(PIXMAN_FORMAT_NAME_ARGB32);

		/* ensure we have a format */
		SDStatus_Require((format != 0), SDStatus_OutOfMemory);

		/* create the pixman image */
		*pattern = pixman_image_create(format, 1, 1);

		/* dispose of the format */
		pixman_format_destroy(format);

		/* ensure we have an image */
		SDStatus_Require((*pattern != 0), SDStatus_OutOfMemory);

		/* calculate the pre-multiplied color */
		{
			/* declarations */
			SDByte a, r, g, b;

			/* get the components and pre-multiply */
			a = ((SDColor_A(color)));
			r = ((SDColor_R(color) * a) / 255);
			g = ((SDColor_G(color) * a) / 255);
			b = ((SDColor_B(color) * a) / 255);

			/* set the color */
			color = SDPixmanPixel_FromARGB(a, r, g, b);
		}

		/* generate the pattern */
		{
			/* declarations */
			SDColor *data;

			/* get the data */
			data = (SDColor *)pixman_image_get_data(*pattern);

			/* set the pixel color */
			*data = color;

			/* set the repeat flag */
			pixman_image_set_repeat(*pattern, 1);
		}
	}

	/* return successfully */
	return SDStatus_OK;
}

SDINTERNAL pixman_color_t
SDUtils_ToPixmanColor(SDColor color)
{
	/* declarations */
	pixman_color_t pc;

	/* get the components and pre-multiply */
	pc.alpha = (SDColor_A(color));
	pc.red   = ((SDColor_R(color) * pc.alpha) / 255);
	pc.green = ((SDColor_G(color) * pc.alpha) / 255);
	pc.blue  = ((SDColor_B(color) * pc.alpha) / 255);
	pc.alpha = (((SDByte)(pc.alpha << 8)) | ((SDByte)pc.alpha));
	pc.red   = (((SDByte)(pc.red   << 8)) | ((SDByte)pc.red));
	pc.green = (((SDByte)(pc.green << 8)) | ((SDByte)pc.green));
	pc.blue  = (((SDByte)(pc.blue  << 8)) | ((SDByte)pc.blue));

	/* return the pixman color */
	return pc;
}

SDINTERNAL SDStatus
SDUtils_PixmanImageRectangle(pixman_image_t *src,
                             pixman_image_t *dst,
                             SDUInt32        x,
                             SDUInt32        y,
                             SDUInt32        width,
                             SDUInt32        height)
{
	/* declarations */
	SDByte   *dataS;
	SDByte   *dataD;
	SDUInt32  strideS;
	SDUInt32  strideD;

	/* assertions */
	SDASSERT((src != 0));
	SDASSERT((dst != 0));

	/* get the data */
	dataS = (SDByte *)pixman_image_get_data(src);
	dataD = (SDByte *)pixman_image_get_data(dst);

	/* get the stride */
	strideS = pixman_image_get_stride(src);
	strideD = pixman_image_get_stride(dst);

	/* copy the data, line by line */
	while(y < height)
	{
		/* declarations */
		SDByte *currS;
		SDByte *currD;

		/* get the current pointers */
		currS = ((dataS + (y * strideS)) + (x << 2));
		currD = ((dataD + (y * strideD)) + (x << 2));

		/* copy the data */
		SDMemCopy(currD, currS, (width << 2));

		/* update the y position */
		++y;
	}

	/* return successfully */
	return SDStatus_OK;
}

SDINTERNAL SDBool
SDUtils_UseGray(SDSmoothingMode   smoothing,
                SDPixelOffsetMode pixelOffset)
{
	switch(smoothing)
	{
		case SDSmoothingMode_AntiAlias:
		case SDSmoothingMode_HighQuality:
			{ return 1; }

		case SDSmoothingMode_None:
		case SDSmoothingMode_HighSpeed:
		case SDSmoothingMode_Default:
		default:
		{
			switch(pixelOffset)
			{
				case SDPixelOffsetMode_HighQuality:
				case SDPixelOffsetMode_Half:
					{ return 1; }

				case SDPixelOffsetMode_None:
				case SDPixelOffsetMode_HighSpeed:
				case SDPixelOffsetMode_Default:
				default:
					{ return 0; }
			}
		}
	}
}


#ifdef __cplusplus
};
#endif
