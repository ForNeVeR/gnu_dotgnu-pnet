/*
 * CUtils.c - Utilities implementation.
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
CINTERNAL CFloat
CUtils_ConvertUnits(CGraphicsUnit fromUnit,
                     CGraphicsUnit toUnit,
                     CFloat        value)
{
	/* get the vertical resolution */
	const CFloat dpiY = CGraphics_DefaultDpiY;

	/* return the converted value */
	return CUtils_ConvertUnitsDPI(fromUnit, toUnit, value, dpiY, dpiY);
}
CINTERNAL CFloat
CUtils_ConvertUnitsDPI(CGraphicsUnit fromUnit,
                        CGraphicsUnit toUnit,
                        CFloat        value,
                        CFloat        fromDpiY,
                        CFloat        toDpiY)
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
		case CGraphicsUnit_World:
		case CGraphicsUnit_Pixel:      { value *= toDpiY;         } break;
		case CGraphicsUnit_Display:    { value *= 75.0f;          } break;
		case CGraphicsUnit_Point:      { value *= 72.0f;          } break;
		case CGraphicsUnit_Inch:       { /* nothing to do here */ } break;
		case CGraphicsUnit_Document:   { value *= 300.0f;         } break;
		case CGraphicsUnit_Millimeter: { value *= 25.4f;          } break;
	}

	/* convert the value from inch scale to the target units */
	switch(fromUnit)
	{
		case CGraphicsUnit_World:
		case CGraphicsUnit_Pixel:      { value /= fromDpiY;       } break;
		case CGraphicsUnit_Display:    { value /= 75.0f;          } break;
		case CGraphicsUnit_Point:      { value /= 72.0f;          } break;
		case CGraphicsUnit_Inch:       { /* nothing to do here */ } break;
		case CGraphicsUnit_Document:   { value /= 300.0f;         } break;
		case CGraphicsUnit_Millimeter: { value /= 25.4;           } break;
	}

	/* return the converted value */
	return value;
}


/* TODO: should we support 0xC080 encoded nulls? */

#define CUtils_Char8Length(c) \
	(((c) < 0x80) ? 1 : \
     (((c) < 0x800) ? 2 : \
      (((c) < 0x10000) ? 3 : \
       (((c) < 0x200000) ? 4 : \
        (((c) < 0x4000000) ? 5 : 6)))))

#define CUtils_SurrogatesToChar32(hi, lo) \
	(((((CChar32)hi) - 0xD800) << 10) + (((CChar32)lo) & 0x03FF) + 0x10000)

/* Convert a UTF16 string to a UTF8 string. */
CINTERNAL CStatus
CUtils_Char16ToChar8(CChar16  *string,
                      CChar8  **result)
{
	/* declarations */
	CChar8  *end;
	CChar8  *dst;
	CChar16 *src;
	CUInt32  count;

	/* get the character pointer */
	src = string;

	/* set the count to the default */
	count = 0;

	/* calculate the UTF8 length */
	while(*src)
	{
		/* declarations */
		CChar32 c32;
		CChar16 c16;

		/* get the current character */
		c16 = *src;

		/* process the current character, based on range */
		if(c16 < 0xD800 || c16 > 0xDBFF)
		{
			/* ensure we don't have a low surrogate */
			CStatus_Require
				((c16 < 0xDC00 || c16 > 0xDFFF), CStatus_Argument);

			/* set the full character */
			c32 = (CChar32)c16;
		}
		else
		{
			/* declarations */
			CChar16 hi;

			/* get the high surrogate */
			hi = c16;

			/* get the low surrogate and update the input pointer */
			c16 = *(++src);

			/* ensure we have a low surrogate */
			CStatus_Require
				((c16 > 0xDBFF && c16 < 0xE000), CStatus_Argument);

			/* combine the surrogates to get the full character */
			c32 = CUtils_SurrogatesToChar32(hi, c16);
		}

		/* update the count */
		count += CUtils_Char8Length(c32);

		/* move to the next character position */
		++c16;
	}

	/* allocate the UTF8 string */
	if(!(*result = (CChar8 *)CMalloc((count + 1) * sizeof(CChar8))))
	{
		return CStatus_OutOfMemory;
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
		CChar16 c16;
		CChar32 c32;

		/* get the current character */
		c16 = *src;

		/* get the current input character in UTF32 */
		if(c16 < 0xD800 || c16 > 0xDBFF)
		{
			/* set the full character */
			c32 = (CChar32)c16;
		}
		else
		{
			/* declarations */
			CChar16 hi;

			/* get the high surrogate */
			hi = c16;

			/* get the low surrogate and update the input position */
			c16 = *(++src);

			/* get the UTF32 character */
			c32 = CUtils_SurrogatesToChar32(hi, c16);
		}

		/* encode the full character into the output buffer */
		if(c32 < 0x80)
		{
			*dst++ = (CChar8)c32;
		}
		else if(c32 < 0x800)
		{
			*dst++ = (CChar8)(0xC0 | ((c32 >> 6) & 0xFF));
			*dst++ = (CChar8)(0x80 | ((c32 >> 0) & 0x3F));
		}
		else if(c32 < 0x10000)
		{
			*dst++ = (CChar8)(0xE0 | ((c32 >> 12) & 0xFF));
			*dst++ = (CChar8)(0x80 | ((c32 >>  6) & 0x3F));
			*dst++ = (CChar8)(0x80 | ((c32 >>  0) & 0x3F));
		}
		else if(c32 < 0x200000)
		{
			*dst++ = (CChar8)(0xF0 | ((c32 >> 18) & 0xFF));
			*dst++ = (CChar8)(0x80 | ((c32 >> 12) & 0x3F));
			*dst++ = (CChar8)(0x80 | ((c32 >>  6) & 0x3F));
			*dst++ = (CChar8)(0x80 | ((c32 >>  0) & 0x3F));
		}
		else if(c32 < 0x4000000)
		{
			*dst++ = (CChar8)(0xF8 | ((c32 >> 24) & 0xFF));
			*dst++ = (CChar8)(0x80 | ((c32 >> 18) & 0x3F));
			*dst++ = (CChar8)(0x80 | ((c32 >> 12) & 0x3F));
			*dst++ = (CChar8)(0x80 | ((c32 >>  6) & 0x3F));
			*dst++ = (CChar8)(0x80 | ((c32 >>  0) & 0x3F));
		}
		else
		{
			*dst++ = (CChar8)(0xFC | ((c32 >> 30) & 0xFF));
			*dst++ = (CChar8)(0x80 | ((c32 >> 24) & 0x3F));
			*dst++ = (CChar8)(0x80 | ((c32 >> 18) & 0x3F));
			*dst++ = (CChar8)(0x80 | ((c32 >> 12) & 0x3F));
			*dst++ = (CChar8)(0x80 | ((c32 >>  6) & 0x3F));
			*dst++ = (CChar8)(0x80 | ((c32 >>  0) & 0x3F));
		}

		/* move to the next input position */
		++src;
	}

	/* null terminate the output */
	*dst = '\0';

	/* return successfully */
	return CStatus_OK;
}

static CStatus
CUtils_ToPixmanImage64bppPArgb(CByte         *scan0,
                                pixman_image_t *image,
                                CUInt32        x,
                                CUInt32        y,
                                CUInt32        width,
                                CUInt32        height,
                                CUInt32        stride)
{
	/* TODO */
	return CStatus_NotImplemented;
}

static CStatus
CUtils_ToPixmanImage64bppArgb(CByte         *scan0,
                               pixman_image_t *image,
                               CUInt32        x,
                               CUInt32        y,
                               CUInt32        width,
                               CUInt32        height,
                               CUInt32        stride)
{
	/* TODO */
	return CStatus_NotImplemented;
}

static CStatus
CUtils_ToPixmanImage48bppRgb(CByte         *scan0,
                              pixman_image_t *image,
                              CUInt32        x,
                              CUInt32        y,
                              CUInt32        width,
                              CUInt32        height,
                              CUInt32        stride)
{
	/* TODO */
	return CStatus_NotImplemented;
}

static CStatus
CUtils_ToPixmanImage32bppPArgb(CByte         *scan0,
                                pixman_image_t *image,
                                CUInt32        x,
                                CUInt32        y,
                                CUInt32        width,
                                CUInt32        height,
                                CUInt32        stride)
{
	/* declarations */
	CByte   *data;
	CUInt32  strideC;

	/* assertions */
	CASSERT((scan0 != 0));
	CASSERT((image != 0));

	/* get the data */
	data = (CByte *)pixman_image_get_data(image);

	/* get the stride */
	strideC = pixman_image_get_stride(image);

	/* copy the data, line by line */
	while(y < height)
	{
		/* declarations */
		CColor  *currC;
		CByte   *currP;
		CUInt32  pos;

		/* get the current color pointer */
		currC = (CColor *)((data + (y * strideC)) + (x << 2));

		/* get the current pixel pointer */
		currP = ((scan0 + (y * stride)) + (x << 2));

		/* copy the color data, pixel by pixel */
		for(pos = x; pos < width; ++pos)
		{
			/* get the color components */
			const CByte b = *currP++;
			const CByte g = *currP++;
			const CByte r = *currP++;
			const CByte a = *currP++;

			/* get the pixel color */
			*currC++ = CPixmanPixel_FromARGB(a, r, g, b);
		}

		/* update the y position */
		++y;
	}

	/* return successfully */
	return CStatus_OK;
}

static CStatus
CUtils_ToPixmanImage32bppArgb(CByte         *scan0,
                               pixman_image_t *image,
                               CUInt32        x,
                               CUInt32        y,
                               CUInt32        width,
                               CUInt32        height,
                               CUInt32        stride)
{
	/* declarations */
	CByte   *data;
	CUInt32  strideC;

	/* assertions */
	CASSERT((scan0 != 0));
	CASSERT((image != 0));

	/* get the data */
	data = (CByte *)pixman_image_get_data(image);

	/* get the stride */
	strideC = pixman_image_get_stride(image);

	/* copy the data, line by line */
	while(y < height)
	{
		/* declarations */
		CColor  *currC;
		CByte   *currP;
		CUInt32  pos;

		/* get the current color pointer */
		currC = (CColor *)((data + (y * strideC)) + (x << 2));

		/* get the current pixel pointer */
		currP = ((scan0 + (y * stride)) + (x << 2));

		/* copy the color data, pixel by pixel */
		for(pos = x; pos < width; ++pos)
		{
			/* get the color components */
			const CByte b = *currP++;
			const CByte g = *currP++;
			const CByte r = *currP++;
			const CByte a = *currP++;

			/* get the pixel color based on the transparency */
			if(a == 0)
			{
				*currC++ = CPixmanPixel_FromARGB(0, 0, 0, 0);
			}
			else
			{
				*currC++ =
					CPixmanPixel_FromARGB
						(a, ((r * a) / 255), ((g * a) / 255), ((b * a) / 255));
			}
		}

		/* update the y position */
		++y;
	}

	/* return successfully */
	return CStatus_OK;
}

static CStatus
CUtils_ToPixmanImage32bppRgb(CByte         *scan0,
                              pixman_image_t *image,
                              CUInt32        x,
                              CUInt32        y,
                              CUInt32        width,
                              CUInt32        height,
                              CUInt32        stride)
{
	/* declarations */
	CByte   *data;
	CUInt32  strideC;

	/* assertions */
	CASSERT((scan0 != 0));
	CASSERT((image != 0));

	/* get the data */
	data = (CByte *)pixman_image_get_data(image);

	/* get the stride */
	strideC = pixman_image_get_stride(image);

	/* copy the data, line by line */
	while(y < height)
	{
		/* declarations */
		CColor  *currC;
		CByte   *currP;
		CUInt32  pos;

		/* get the current color pointer */
		currC = (CColor *)((data + (y * strideC)) + (x << 2));

		/* get the current pixel pointer */
		currP = ((scan0 + (y * stride)) + (x << 2));

		/* copy the color data, pixel by pixel */
		for(pos = x; pos < width; ++pos)
		{
			/* get the color components */
			const CByte b = *currP++;
			const CByte g = *currP++;
			const CByte r = *currP++;

			/* get the pixel color */
			*currC++ = CPixmanPixel_FromARGB(0xFF, r, g, b);

			/* update the pixel position */
			++currP;
		}

		/* update the y position */
		++y;
	}

	/* return successfully */
	return CStatus_OK;
}

static CStatus
CUtils_ToPixmanImage24bppRgb(CByte         *scan0,
                              pixman_image_t *image,
                              CUInt32        x,
                              CUInt32        y,
                              CUInt32        width,
                              CUInt32        height,
                              CUInt32        stride)
{
	/* declarations */
	CByte   *data;
	CUInt32  strideC;

	/* assertions */
	CASSERT((scan0 != 0));
	CASSERT((image != 0));

	/* get the data */
	data = (CByte *)pixman_image_get_data(image);

	/* get the stride */
	strideC = pixman_image_get_stride(image);

	/* copy the data, line by line */
	while(y < height)
	{
		/* declarations */
		CColor  *currC;
		CByte   *currP;
		CUInt32  pos;

		/* get the current color pointer */
		currC = (CColor *)((data + (y * strideC)) + (x << 2));

		/* get the current pixel pointer */
		currP = ((scan0 + (y * stride)) + ((x << 1) + x));

		/* copy the color data, pixel by pixel */
		for(pos = x; pos < width; ++pos)
		{
			/* get the color components */
			const CByte b = *currP++;
			const CByte g = *currP++;
			const CByte r = *currP++;

			/* get the pixel color */
			*currC++ = CPixmanPixel_FromARGB(0xFF, r, g, b);
		}

		/* update the y position */
		++y;
	}

	/* return successfully */
	return CStatus_OK;
}

static CStatus
CUtils_ToPixmanImage16bppArgb1555(CByte         *scan0,
                                   pixman_image_t *image,
                                   CUInt32        x,
                                   CUInt32        y,
                                   CUInt32        width,
                                   CUInt32        height,
                                   CUInt32        stride)
{
	/* declarations */
	CByte   *data;
	CUInt32  strideC;

	/* assertions */
	CASSERT((scan0 != 0));
	CASSERT((image != 0));

	/* get the data */
	data = (CByte *)pixman_image_get_data(image);

	/* get the stride */
	strideC = pixman_image_get_stride(image);

	/* copy the data, line by line */
	while(y < height)
	{
		/* declarations */
		CColor  *currC;
		CByte   *currP;
		CUInt32  pos;

		/* get the current color pointer */
		currC = (CColor *)((data + (y * strideC)) + (x << 2));

		/* get the current pixel pointer */
		currP = ((scan0 + (y * stride)) + (x << 1));

		/* copy the color data, pixel by pixel */
		for(pos = x; pos < width; ++pos)
		{
			/* declarations */
			CByte r, g, b;

			/* get the color components */
			g = *currP++;
			r = *currP++;

			/* get the pixel color based on transparency */
			if((r & 0x80) == 0)
			{
				*currC++ = CPixmanPixel_FromARGB(0, 0, 0, 0);
			}
			else
			{
				/* extract the color components */
				b = (CByte)(g & 0x1F);
				g = (CByte)(((r << 3) & 0x18) | ((g >> 5) & 0x07));
				r = (CByte)((r >> 2) & 0x1F);

				/* scale the color components */
				b = (CByte)(((b << 8) - b) / 31);
				g = (CByte)(((g << 8) - g) / 31);
				r = (CByte)(((r << 8) - r) / 31);

				/* get the pixel color */
				*currC++ = CPixmanPixel_FromARGB(0xFF, r, g, b);
			}
		}

		/* update the y position */
		++y;
	}

	/* return successfully */
	return CStatus_OK;
}

static CStatus
CUtils_ToPixmanImage16bppRgb565(CByte         *scan0,
                                 pixman_image_t *image,
                                 CUInt32        x,
                                 CUInt32        y,
                                 CUInt32        width,
                                 CUInt32        height,
                                 CUInt32        stride)
{
	/* declarations */
	CByte   *data;
	CUInt32  strideC;

	/* assertions */
	CASSERT((scan0 != 0));
	CASSERT((image != 0));

	/* get the data */
	data = (CByte *)pixman_image_get_data(image);

	/* get the stride */
	strideC = pixman_image_get_stride(image);

	/* copy the data, line by line */
	while(y < height)
	{
		/* declarations */
		CColor  *currC;
		CByte   *currP;
		CUInt32  pos;

		/* get the current color pointer */
		currC = (CColor *)((data + (y * strideC)) + (x << 2));

		/* get the current pixel pointer */
		currP = ((scan0 + (y * stride)) + (x << 1));

		/* copy the color data, pixel by pixel */
		for(pos = x; pos < width; ++pos)
		{
			/* declarations */
			CByte r, g, b;

			/* get the color components */
			g = *currP++;
			r = *currP++;

			/* extract the color components */
			b = (CByte)(g & 0x1F);
			g = (CByte)(((r << 3) & 0x38) | ((g >> 5) & 0x07));
			r = (CByte)(r >> 3);

			/* scale the color components */
			b = (CByte)(((b << 8) - b) / 31);
			g = (CByte)(((g << 8) - g) / 63);
			r = (CByte)(((r << 8) - r) / 31);

			/* get the pixel color */
			*currC++ = CPixmanPixel_FromARGB(0xFF, r, g, b);
		}

		/* update the y position */
		++y;
	}

	/* return successfully */
	return CStatus_OK;
}

static CStatus
CUtils_ToPixmanImage16bppRgb555(CByte         *scan0,
                                 pixman_image_t *image,
                                 CUInt32        x,
                                 CUInt32        y,
                                 CUInt32        width,
                                 CUInt32        height,
                                 CUInt32        stride)
{
	/* declarations */
	CByte   *data;
	CUInt32  strideC;

	/* assertions */
	CASSERT((scan0 != 0));
	CASSERT((image != 0));

	/* get the data */
	data = (CByte *)pixman_image_get_data(image);

	/* get the stride */
	strideC = pixman_image_get_stride(image);

	/* copy the data, line by line */
	while(y < height)
	{
		/* declarations */
		CColor  *currC;
		CByte   *currP;
		CUInt32  pos;

		/* get the current color pointer */
		currC = (CColor *)((data + (y * strideC)) + (x << 2));

		/* get the current pixel pointer */
		currP = ((scan0 + (y * stride)) + (x << 1));

		/* copy the color data, pixel by pixel */
		for(pos = x; pos < width; ++pos)
		{
			/* declarations */
			CByte r, g, b;

			/* get the color components */
			g = *currP++;
			r = *currP++;

			/* extract the color components */
			b = (CByte)(g & 0x1F);
			g = (CByte)(((r << 3) & 0x18) | ((g >> 5) & 0x07));
			r = (CByte)((r >> 2) & 0x1F);

			/* scale the color components */
			b = (CByte)(((b << 8) - b) / 31);
			g = (CByte)(((g << 8) - g) / 31);
			r = (CByte)(((r << 8) - r) / 31);

			/* get the pixel color */
			*currC++ = CPixmanPixel_FromARGB(0xFF, r, g, b);
		}

		/* update the y position */
		++y;
	}

	/* return successfully */
	return CStatus_OK;
}

static CStatus
CUtils_ToPixmanImage16bppGrayScale(CByte         *scan0,
                                    pixman_image_t *image,
                                    CUInt32        x,
                                    CUInt32        y,
                                    CUInt32        width,
                                    CUInt32        height,
                                    CUInt32        stride)
{
	/* declarations */
	CByte   *data;
	CUInt32  strideC;

	/* assertions */
	CASSERT((scan0 != 0));
	CASSERT((image != 0));

	/* get the data */
	data = (CByte *)pixman_image_get_data(image);

	/* get the stride */
	strideC = pixman_image_get_stride(image);

	/* copy the data, line by line */
	while(y < height)
	{
		/* declarations */
		CColor  *currC;
		CByte   *currP;
		CUInt32  pos;

		/* get the current color pointer */
		currC = (CColor *)((data + (y * strideC)) + (x << 2));

		/* get the current pixel pointer (offset for the high byte) */
		currP = ((scan0 + (y * stride)) + ((x << 1) + 1));

		/* copy the color data, pixel by pixel */
		for(pos = x; pos < width; ++pos)
		{
			/* get the gray value */
			const CByte val = *currP++; ++currP;

			/* get the pixel color */
			*currC++ = CPixmanPixel_FromARGB(0xFF, val, val, val);
		}

		/* update the y position */
		++y;
	}

	/* return successfully */
	return CStatus_OK;
}

static CStatus
CUtils_ToPixmanImage8bppIndexed(CByte         *scan0,
                                 pixman_image_t *image,
                                 CUInt32        x,
                                 CUInt32        y,
                                 CUInt32        width,
                                 CUInt32        height,
                                 CUInt32        stride,
                                 CColorPalette *palette)
{
	/* TODO */
	return CStatus_NotImplemented;
}

static CStatus
CUtils_ToPixmanImage4bppIndexed(CByte         *scan0,
                                 pixman_image_t *image,
                                 CUInt32        x,
                                 CUInt32        y,
                                 CUInt32        width,
                                 CUInt32        height,
                                 CUInt32        stride,
                                 CColorPalette *palette)
{
	/* TODO */
	return CStatus_NotImplemented;
}

static CStatus
CUtils_ToPixmanImage1bppIndexed(CByte         *scan0,
                                 pixman_image_t *image,
                                 CUInt32        x,
                                 CUInt32        y,
                                 CUInt32        width,
                                 CUInt32        height,
                                 CUInt32        stride,
                                 CColorPalette *palette)
{
	/* TODO */
	return CStatus_NotImplemented;
}

static CStatus
CUtils_FromPixmanImage64bppPArgb(pixman_image_t *image,
                                  CByte         *scan0,
                                  CUInt32        x,
                                  CUInt32        y,
                                  CUInt32        width,
                                  CUInt32        height,
                                  CUInt32        stride)
{
	/* TODO */
	return CStatus_NotImplemented;
}

static CStatus
CUtils_FromPixmanImage64bppArgb(pixman_image_t *image,
                                 CByte         *scan0,
                                 CUInt32        x,
                                 CUInt32        y,
                                 CUInt32        width,
                                 CUInt32        height,
                                 CUInt32        stride)
{
	/* TODO */
	return CStatus_NotImplemented;
}

static CStatus
CUtils_FromPixmanImage48bppRgb(pixman_image_t *image,
                                CByte         *scan0,
                                CUInt32        x,
                                CUInt32        y,
                                CUInt32        width,
                                CUInt32        height,
                                CUInt32        stride)
{
	/* TODO */
	return CStatus_NotImplemented;
}

static CStatus
CUtils_FromPixmanImage32bppPArgb(pixman_image_t *image,
                                  CByte         *scan0,
                                  CUInt32        x,
                                  CUInt32        y,
                                  CUInt32        width,
                                  CUInt32        height,
                                  CUInt32        stride)
{
	/* declarations */
	CByte   *data;
	CUInt32  strideC;

	/* assertions */
	CASSERT((scan0 != 0));
	CASSERT((image != 0));

	/* get the data */
	data = (CByte *)pixman_image_get_data(image);

	/* get the stride */
	strideC = pixman_image_get_stride(image);

	/* copy the data, line by line */
	while(y < height)
	{
		/* declarations */
		CColor  *currC;
		CByte   *currP;
		CUInt32  pos;

		/* get the current color pointer */
		currC = (CColor *)((data + (y * strideC)) + (x << 2));

		/* get the current pixel pointer */
		currP = ((scan0 + (y * stride)) + (x << 2));

		/* copy the color data, pixel by pixel */
		for(pos = x; pos < width; ++pos)
		{
			/* declarations */
			CByte a, r, g, b;

			/* get the color components */
			CPixmanPixel_ToARGB(*currC, a, r, g, b);

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
	return CStatus_OK;
}

static CStatus
CUtils_FromPixmanImage32bppArgb(pixman_image_t *image,
                                 CByte         *scan0,
                                 CUInt32        x,
                                 CUInt32        y,
                                 CUInt32        width,
                                 CUInt32        height,
                                 CUInt32        stride)
{
	/* declarations */
	CByte   *data;
	CUInt32  strideC;

	/* assertions */
	CASSERT((scan0 != 0));
	CASSERT((image != 0));

	/* get the data */
	data = (CByte *)pixman_image_get_data(image);

	/* get the stride */
	strideC = pixman_image_get_stride(image);

	/* copy the data, line by line */
	while(y < height)
	{
		/* declarations */
		CColor  *currC;
		CByte   *currP;
		CUInt32  pos;

		/* get the current color pointer */
		currC = (CColor *)((data + (y * strideC)) + (x << 2));

		/* get the current pixel pointer */
		currP = ((scan0 + (y * stride)) + (x << 2));

		/* copy the color data, pixel by pixel */
		for(pos = x; pos < width; ++pos)
		{
			/* declarations */
			CByte a, r, g, b;

			/* get the color components */
			CPixmanPixel_ToARGB(*currC, a, r, g, b);

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
				*currP++ = ((CByte)(((b << 8) - b) / a));
				*currP++ = ((CByte)(((g << 8) - g) / a));
				*currP++ = ((CByte)(((r << 8) - r) / a));
				*currP++ = a;
			}

			/* move to the next pixel position */
			++currC;
		}

		/* update the y position */
		++y;
	}

	/* return successfully */
	return CStatus_OK;
}

static CStatus
CUtils_FromPixmanImage32bppRgb(pixman_image_t *image,
                                CByte         *scan0,
                                CUInt32        x,
                                CUInt32        y,
                                CUInt32        width,
                                CUInt32        height,
                                CUInt32        stride)
{
	/* declarations */
	CByte   *data;
	CUInt32  strideC;

	/* assertions */
	CASSERT((scan0 != 0));
	CASSERT((image != 0));

	/* get the data */
	data = (CByte *)pixman_image_get_data(image);

	/* get the stride */
	strideC = pixman_image_get_stride(image);

	/* copy the data, line by line */
	while(y < height)
	{
		/* declarations */
		CColor  *currC;
		CByte   *currP;
		CUInt32  pos;

		/* get the current color pointer */
		currC = (CColor *)((data + (y * strideC)) + (x << 2));

		/* get the current pixel pointer */
		currP = ((scan0 + (y * stride)) + (x << 2));

		/* copy the color data, pixel by pixel */
		for(pos = x; pos < width; ++pos)
		{
			/* declarations */
			CByte a, r, g, b;

			/* get the color components */
			CPixmanPixel_ToARGB(*currC, a, r, g, b);

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
	return CStatus_OK;
}

static CStatus
CUtils_FromPixmanImage24bppRgb(pixman_image_t *image,
                                CByte         *scan0,
                                CUInt32        x,
                                CUInt32        y,
                                CUInt32        width,
                                CUInt32        height,
                                CUInt32        stride)
{
	/* declarations */
	CByte   *data;
	CUInt32  strideC;

	/* assertions */
	CASSERT((scan0 != 0));
	CASSERT((image != 0));

	/* get the data */
	data = (CByte *)pixman_image_get_data(image);

	/* get the stride */
	strideC = pixman_image_get_stride(image);

	/* copy the data, line by line */
	while(y < height)
	{
		/* declarations */
		CColor  *currC;
		CByte   *currP;
		CUInt32  pos;

		/* get the current color pointer */
		currC = (CColor *)((data + (y * strideC)) + (x << 2));

		/* get the current pixel pointer */
		currP = ((scan0 + (y * stride)) + ((x << 1) + x));

		/* copy the color data, pixel by pixel */
		for(pos = x; pos < width; ++pos)
		{
			/* declarations */
			CByte a, r, g, b;

			/* get the color components */
			CPixmanPixel_ToARGB(*currC, a, r, g, b);

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
	return CStatus_OK;
}

static CStatus
CUtils_FromPixmanImage16bppArgb1555(pixman_image_t *image,
                                     CByte         *scan0,
                                     CUInt32        x,
                                     CUInt32        y,
                                     CUInt32        width,
                                     CUInt32        height,
                                     CUInt32        stride)
{
	/* declarations */
	CByte   *data;
	CUInt32  strideC;

	/* assertions */
	CASSERT((scan0 != 0));
	CASSERT((image != 0));

	/* get the data */
	data = (CByte *)pixman_image_get_data(image);

	/* get the stride */
	strideC = pixman_image_get_stride(image);

	/* copy the data, line by line */
	while(y < height)
	{
		/* declarations */
		CColor  *currC;
		CByte   *currP;
		CUInt32  pos;

		/* get the current color pointer */
		currC = (CColor *)((data + (y * strideC)) + (x << 2));

		/* get the current pixel pointer */
		currP = ((scan0 + (y * stride)) + (x << 1));

		/* copy the color data, pixel by pixel */
		for(pos = x; pos < width; ++pos)
		{
			/* declarations */
			CByte a, r, g, b;

			/* get the color components */
			CPixmanPixel_ToARGB(*currC, a, r, g, b);

			/* set the pixel color based on transparency */
			if(a < 128)
			{
				*currP++ = 0;
				*currP++ = 0;
			}
			else
			{
				/* set the pixel color */
				*currP++ = ((CByte)(((g << 2) & 0xE0) | ((b >> 3) & 0x1F)));
				*currP++ =
					((CByte)(((r >> 1) & 0x7C) | ((g >> 6) & 0x03) | 0x80));
			}

			/* move to the next pixel position */
			++currC;
		}

		/* update the y position */
		++y;
	}

	/* return successfully */
	return CStatus_OK;
}

static CStatus
CUtils_FromPixmanImage16bppRgb565(pixman_image_t *image,
                                   CByte         *scan0,
                                   CUInt32        x,
                                   CUInt32        y,
                                   CUInt32        width,
                                   CUInt32        height,
                                   CUInt32        stride)
{
	/* declarations */
	CByte   *data;
	CUInt32  strideC;

	/* assertions */
	CASSERT((scan0 != 0));
	CASSERT((image != 0));

	/* get the data */
	data = (CByte *)pixman_image_get_data(image);

	/* get the stride */
	strideC = pixman_image_get_stride(image);

	/* copy the data, line by line */
	while(y < height)
	{
		/* declarations */
		CColor  *currC;
		CByte   *currP;
		CUInt32  pos;

		/* get the current color pointer */
		currC = (CColor *)((data + (y * strideC)) + (x << 2));

		/* get the current pixel pointer */
		currP = ((scan0 + (y * stride)) + (x << 1));

		/* copy the color data, pixel by pixel */
		for(pos = x; pos < width; ++pos)
		{
			/* declarations */
			CByte a, r, g, b;

			/* get the color components */
			CPixmanPixel_ToARGB(*currC, a, r, g, b);

			/* set the pixel color based on transparency */
			if(a < 128)
			{
				*currP++ = 0;
				*currP++ = 0;
			}
			else
			{
				/* set the pixel color */
				*currP++ = ((CByte)(((g << 3) & 0xE0) | ((b >> 3) & 0x1F)));
				*currP++ = ((CByte)(((r << 0) & 0xF8) | ((g >> 5) & 0x07)));
			}

			/* move to the next pixel position */
			++currC;
		}

		/* update the y position */
		++y;
	}

	/* return successfully */
	return CStatus_OK;
}

static CStatus
CUtils_FromPixmanImage16bppRgb555(pixman_image_t *image,
                                   CByte         *scan0,
                                   CUInt32        x,
                                   CUInt32        y,
                                   CUInt32        width,
                                   CUInt32        height,
                                   CUInt32        stride)
{
	/* declarations */
	CByte   *data;
	CUInt32  strideC;

	/* assertions */
	CASSERT((scan0 != 0));
	CASSERT((image != 0));

	/* get the data */
	data = (CByte *)pixman_image_get_data(image);

	/* get the stride */
	strideC = pixman_image_get_stride(image);

	/* copy the data, line by line */
	while(y < height)
	{
		/* declarations */
		CColor  *currC;
		CByte   *currP;
		CUInt32  pos;

		/* get the current color pointer */
		currC = (CColor *)((data + (y * strideC)) + (x << 2));

		/* get the current pixel pointer */
		currP = ((scan0 + (y * stride)) + (x << 1));

		/* copy the color data, pixel by pixel */
		for(pos = x; pos < width; ++pos)
		{
			/* declarations */
			CByte a, r, g, b;

			/* get the color components */
			CPixmanPixel_ToARGB(*currC, a, r, g, b);

			/* set the pixel color based on transparency */
			if(a < 128)
			{
				*currP++ = 0;
				*currP++ = 0;
			}
			else
			{
				/* set the pixel color */
				*currP++ = ((CByte)(((g << 2) & 0xE0) | ((b >> 3) & 0x1F)));
				*currP++ = ((CByte)(((r >> 1) & 0x7C) | ((g >> 6) & 0x03)));
			}

			/* move to the next pixel position */
			++currC;
		}

		/* update the y position */
		++y;
	}

	/* return successfully */
	return CStatus_OK;
}

static CStatus
CUtils_FromPixmanImage16bppGrayScale(pixman_image_t *image,
                                      CByte         *scan0,
                                      CUInt32        x,
                                      CUInt32        y,
                                      CUInt32        width,
                                      CUInt32        height,
                                      CUInt32        stride)
{
	/* declarations */
	CByte   *data;
	CUInt32  strideC;

	/* assertions */
	CASSERT((scan0 != 0));
	CASSERT((image != 0));

	/* get the data */
	data = (CByte *)pixman_image_get_data(image);

	/* get the stride */
	strideC = pixman_image_get_stride(image);

	/* copy the data, line by line */
	while(y < height)
	{
		/* declarations */
		CColor  *currC;
		CByte   *currP;
		CUInt32  pos;

		/* get the current color pointer */
		currC = (CColor *)((data + (y * strideC)) + (x << 2));

		/* get the current pixel pointer */
		currP = ((scan0 + (y * stride)) + (x << 1));

		/* copy the color data, pixel by pixel */
		for(pos = x; pos < width; ++pos)
		{
			/* declarations */
			CByte a, r, g, b;

			/* get the color components */
			CPixmanPixel_ToARGB(*currC, a, r, g, b);

			/* set the pixel color based on transparency */
			if(a < 128)
			{
				*currP++ = 0;
				*currP++ = 0;
			}
			else
			{
				/* declarations */
				CUInt16 value;

				/* get the gray value */
				value = (CUInt16)(255 * CColor_IntensityRGB(r, g, b));

				/* set the pixel color */
				*currP++ = ((CByte)(value & 0xFF));
				*currP++ = ((CByte)(value >> 8));
			}

			/* move to the next pixel position */
			++currC;
		}

		/* update the y position */
		++y;
	}

	/* return successfully */
	return CStatus_OK;
}

static CStatus
CUtils_FromPixmanImage8bppIndexed(pixman_image_t *image,
                                   CByte         *scan0,
                                   CUInt32        x,
                                   CUInt32        y,
                                   CUInt32        width,
                                   CUInt32        height,
                                   CUInt32        stride,
                                   CColorPalette *palette)
{
	/* TODO */
	return CStatus_NotImplemented;
}

static CStatus
CUtils_FromPixmanImage4bppIndexed(pixman_image_t *image,
                                   CByte         *scan0,
                                   CUInt32        x,
                                   CUInt32        y,
                                   CUInt32        width,
                                   CUInt32        height,
                                   CUInt32        stride,
                                   CColorPalette *palette)
{
	/* TODO */
	return CStatus_NotImplemented;
}

static CStatus
CUtils_FromPixmanImage1bppIndexed(pixman_image_t *image,
                                   CByte         *scan0,
                                   CUInt32        x,
                                   CUInt32        y,
                                   CUInt32        width,
                                   CUInt32        height,
                                   CUInt32        stride,
                                   CColorPalette *palette)
{
	/* TODO */
	return CStatus_NotImplemented;
}

/* Convert pixel data into pixman image data. */
CINTERNAL CStatus
CUtils_ToPixmanImage(CPixelFormat   format,
                      CByte         *scan0,
                      pixman_image_t *image,
                      CUInt32        x,
                      CUInt32        y,
                      CUInt32        width,
                      CUInt32        height,
                      CUInt32        stride,
                      CColorPalette *palette)
{
	/* convert the image according to the format */
	switch(format)
	{
		default: { return CStatus_NotSupported; }

		case CPixelFormat_64bppPArgb:
		{
			return
				CUtils_ToPixmanImage64bppPArgb
					(scan0, image, x, y, width, height, stride);
		}
		case CPixelFormat_64bppArgb:
		{
			return
				CUtils_ToPixmanImage64bppArgb
					(scan0, image, x, y, width, height, stride);
		}
		case CPixelFormat_48bppRgb:
		{
			return
				CUtils_ToPixmanImage48bppRgb
					(scan0, image, x, y, width, height, stride);
		}
		case CPixelFormat_32bppPArgb:
		{
			return
				CUtils_ToPixmanImage32bppPArgb
					(scan0, image, x, y, width, height, stride);
		}
		case CPixelFormat_32bppArgb:
		{
			return
				CUtils_ToPixmanImage32bppArgb
					(scan0, image, x, y, width, height, stride);
		}
		case CPixelFormat_32bppRgb:
		{
			return
				CUtils_ToPixmanImage32bppRgb
					(scan0, image, x, y, width, height, stride);
		}
		case CPixelFormat_24bppRgb:
		{
			return
				CUtils_ToPixmanImage24bppRgb
					(scan0, image, x, y, width, height, stride);
		}
		case CPixelFormat_16bppArgb1555:
		{
			return
				CUtils_ToPixmanImage16bppArgb1555
					(scan0, image, x, y, width, height, stride);
		}
		case CPixelFormat_16bppRgb565:
		{
			return
				CUtils_ToPixmanImage16bppRgb565
					(scan0, image, x, y, width, height, stride);
		}
		case CPixelFormat_16bppRgb555:
		{
			return
				CUtils_ToPixmanImage16bppRgb555
					(scan0, image, x, y, width, height, stride);
		}
		case CPixelFormat_16bppGrayScale:
		{
			return
				CUtils_ToPixmanImage16bppGrayScale
					(scan0, image, x, y, width, height, stride);
		}
		case CPixelFormat_8bppIndexed:
		{
			return
				CUtils_ToPixmanImage8bppIndexed
					(scan0, image, x, y, width, height, stride, palette);
		}
		case CPixelFormat_4bppIndexed:
		{
			return
				CUtils_ToPixmanImage4bppIndexed
					(scan0, image, x, y, width, height, stride, palette);
		}
		case CPixelFormat_1bppIndexed:
		{
			return
				CUtils_ToPixmanImage1bppIndexed
					(scan0, image, x, y, width, height, stride, palette);
		}
	}
}

/* Convert pixman image data into pixel data. */
CINTERNAL CStatus
CUtils_FromPixmanImage(CPixelFormat   format,
                        pixman_image_t *image,
                        CByte         *scan0,
                        CUInt32        x,
                        CUInt32        y,
                        CUInt32        width,
                        CUInt32        height,
                        CUInt32        stride,
                        CColorPalette *palette)
{
	/* convert the image according to the format */
	switch(format)
	{
		default: { return CStatus_NotSupported; }

		case CPixelFormat_64bppPArgb:
		{
			return
				CUtils_FromPixmanImage64bppPArgb
					(image, scan0, x, y, width, height, stride);
		}
		case CPixelFormat_64bppArgb:
		{
			return
				CUtils_FromPixmanImage64bppArgb
					(image, scan0, x, y, width, height, stride);
		}
		case CPixelFormat_48bppRgb:
		{
			return
				CUtils_FromPixmanImage48bppRgb
					(image, scan0, x, y, width, height, stride);
		}
		case CPixelFormat_32bppPArgb:
		{
			return
				CUtils_FromPixmanImage32bppPArgb
					(image, scan0, x, y, width, height, stride);
		}
		case CPixelFormat_32bppArgb:
		{
			return
				CUtils_FromPixmanImage32bppArgb
					(image, scan0, x, y, width, height, stride);
		}
		case CPixelFormat_32bppRgb:
		{
			return
				CUtils_FromPixmanImage32bppRgb
					(image, scan0, x, y, width, height, stride);
		}
		case CPixelFormat_24bppRgb:
		{
			return
				CUtils_FromPixmanImage24bppRgb
					(image, scan0, x, y, width, height, stride);
		}
		case CPixelFormat_16bppArgb1555:
		{
			return
				CUtils_FromPixmanImage16bppArgb1555
					(image, scan0, x, y, width, height, stride);
		}
		case CPixelFormat_16bppRgb565:
		{
			return
				CUtils_FromPixmanImage16bppRgb565
					(image, scan0, x, y, width, height, stride);
		}
		case CPixelFormat_16bppRgb555:
		{
			return
				CUtils_FromPixmanImage16bppRgb555
					(image, scan0, x, y, width, height, stride);
		}
		case CPixelFormat_16bppGrayScale:
		{
			return
				CUtils_FromPixmanImage16bppGrayScale
					(image, scan0, x, y, width, height, stride);
		}
		case CPixelFormat_8bppIndexed:
		{
			return
				CUtils_FromPixmanImage8bppIndexed
					(image, scan0, x, y, width, height, stride, palette);
		}
		case CPixelFormat_4bppIndexed:
		{
			return
				CUtils_FromPixmanImage4bppIndexed
					(image, scan0, x, y, width, height, stride, palette);
		}
		case CPixelFormat_1bppIndexed:
		{
			return
				CUtils_FromPixmanImage1bppIndexed
					(image, scan0, x, y, width, height, stride, palette);
		}
	}
}

CINTERNAL CStatus
CUtils_GetPixmanPixelPointer(pixman_image_t  *image,
                              CUInt32         x,
                              CUInt32         y,
                              CColor        **pixel)
{
	/* declarations */
	CByte   *data;
	CUInt32  width;
	CUInt32  height;
	CUInt32  stride;

	/* assertions */
	CASSERT((image != 0));
	CASSERT((pixel != 0));

	/* get the pixman image information */
	data   = (CByte *)pixman_image_get_data(image);
	width  = (CUInt32)pixman_image_get_width(image);
	height = (CUInt32)pixman_image_get_height(image);
	stride = (CUInt32)pixman_image_get_stride(image);

	/* ensure the pixel is within bounds */
	CStatus_Require((x < width && y < height), CStatus_ArgumentOutOfRange);

	/* get the pixel pointer */
	*pixel = ((CColor *)(data + (y * stride) + (x << 2)));

	/* return successfully */
	return CStatus_OK;
}

CINTERNAL CUInt32
CUtils_FormatToStride(CPixelFormat pixelFormat,
                       CUInt32      width)
{
	return ((CUtils_BytesPerLine(pixelFormat, width) + 3) & ~3);
}

CINTERNAL CUInt32
CUtils_BytesPerLine(CPixelFormat pixelFormat,
                     CUInt32      width)
{
	switch(pixelFormat)
	{
		case CPixelFormat_1bppIndexed:
			{ return ((width + 7) / 8); }

		case CPixelFormat_4bppIndexed:
			{ return ((width + 1) / 2); }

		case CPixelFormat_8bppIndexed:
			{ return (width * 1); }

		case CPixelFormat_16bppRgb555:
		case CPixelFormat_16bppRgb565:
		case CPixelFormat_16bppArgb1555:
		case CPixelFormat_16bppGrayScale:
			{ return (width * 2); }

		case CPixelFormat_24bppRgb:
			{ return (width * 3); }

		case CPixelFormat_32bppRgb:
		case CPixelFormat_32bppPArgb:
		case CPixelFormat_32bppArgb:
			{ return (width * 4); }

		case CPixelFormat_48bppRgb:
			{ return (width * 6); }

		case CPixelFormat_64bppPArgb:
		case CPixelFormat_64bppArgb:
			{ return (width * 8); }

		default:
			{ return (width * 4); }
	}
}

CINTERNAL pixman_transform_t
CUtils_ToPixmanTransform(CAffineTransformF *transform)
{
	/* declarations */
	pixman_transform_t pt;
	CFloat            xx;
	CFloat            xy;
	CFloat            yx;
	CFloat            yy;
	CFloat            dx;
	CFloat            dy;

	/* assertions */
	CASSERT((transform != 0));

	/* get the transformation components */
	{
		xx = CAffineTransform_XX(*transform);
		xy = CAffineTransform_XY(*transform);
		yx = CAffineTransform_YX(*transform);
		yy = CAffineTransform_YY(*transform);
		dx = CAffineTransform_DX(*transform);
		dy = CAffineTransform_DX(*transform);
	}

	/* initialize the pixman transformation */
	{
		pt.matrix[0][0] = CFloat_ToFixed(xx);
		pt.matrix[0][1] = CFloat_ToFixed(xy);
		pt.matrix[0][2] = CFloat_ToFixed(dx);
		pt.matrix[1][0] = CFloat_ToFixed(yx);
		pt.matrix[1][1] = CFloat_ToFixed(yy);
		pt.matrix[1][2] = CFloat_ToFixed(dy);
		pt.matrix[2][0] = CFixed_Zero;
		pt.matrix[2][1] = CFixed_Zero;
		pt.matrix[2][2] = CFixed_One;
	}

	/* return the pixman transformation */
	return pt;
}

CINTERNAL CStatus
CUtils_CreateSolidPattern(pixman_image_t **pattern,
                           CColor          color)
{
	/* assertions */
	CASSERT((pattern != 0));

	/* create the pattern */
	{
		/* declarations */
		pixman_format_t *format;

		/* create the pixman format */
		format = pixman_format_create(PIXMAN_FORMAT_NAME_ARGB32);

		/* ensure we have a format */
		CStatus_Require((format != 0), CStatus_OutOfMemory);

		/* create the pixman image */
		*pattern = pixman_image_create(format, 1, 1);

		/* dispose of the format */
		pixman_format_destroy(format);

		/* ensure we have an image */
		CStatus_Require((*pattern != 0), CStatus_OutOfMemory);

		/* calculate the pre-multiplied color */
		{
			/* declarations */
			CByte a, r, g, b;

			/* get the components and pre-multiply */
			a = ((CColor_A(color)));
			r = ((CColor_R(color) * a) / 255);
			g = ((CColor_G(color) * a) / 255);
			b = ((CColor_B(color) * a) / 255);

			/* set the color */
			color = CPixmanPixel_FromARGB(a, r, g, b);
		}

		/* generate the pattern */
		{
			/* declarations */
			CColor *data;

			/* get the data */
			data = (CColor *)pixman_image_get_data(*pattern);

			/* set the pixel color */
			*data = color;

			/* set the repeat flag */
			pixman_image_set_repeat(*pattern, 1);
		}
	}

	/* return successfully */
	return CStatus_OK;
}

CINTERNAL pixman_color_t
CUtils_ToPixmanColor(CColor color)
{
	/* declarations */
	pixman_color_t pc;

	/* get the components and pre-multiply */
	pc.alpha = (CColor_A(color));
	pc.red   = ((CColor_R(color) * pc.alpha) / 255);
	pc.green = ((CColor_G(color) * pc.alpha) / 255);
	pc.blue  = ((CColor_B(color) * pc.alpha) / 255);
	pc.alpha = (((CByte)(pc.alpha << 8)) | ((CByte)pc.alpha));
	pc.red   = (((CByte)(pc.red   << 8)) | ((CByte)pc.red));
	pc.green = (((CByte)(pc.green << 8)) | ((CByte)pc.green));
	pc.blue  = (((CByte)(pc.blue  << 8)) | ((CByte)pc.blue));

	/* return the pixman color */
	return pc;
}

CINTERNAL CStatus
CUtils_PixmanImageRectangle(pixman_image_t *src,
                             pixman_image_t *dst,
                             CUInt32        x,
                             CUInt32        y,
                             CUInt32        width,
                             CUInt32        height)
{
	/* declarations */
	CByte   *dataS;
	CByte   *dataD;
	CUInt32  strideS;
	CUInt32  strideD;

	/* assertions */
	CASSERT((src != 0));
	CASSERT((dst != 0));

	/* get the data */
	dataS = (CByte *)pixman_image_get_data(src);
	dataD = (CByte *)pixman_image_get_data(dst);

	/* get the stride */
	strideS = pixman_image_get_stride(src);
	strideD = pixman_image_get_stride(dst);

	/* copy the data, line by line */
	while(y < height)
	{
		/* declarations */
		CByte *currS;
		CByte *currD;

		/* get the current pointers */
		currS = ((dataS + (y * strideS)) + (x << 2));
		currD = ((dataD + (y * strideD)) + (x << 2));

		/* copy the data */
		CMemCopy(currD, currS, (width << 2));

		/* update the y position */
		++y;
	}

	/* return successfully */
	return CStatus_OK;
}

CINTERNAL CBool
CUtils_UseGray(CSmoothingMode   smoothing,
                CPixelOffsetMode pixelOffset)
{
	switch(smoothing)
	{
		case CSmoothingMode_AntiAlias:
		case CSmoothingMode_HighQuality:
			{ return 1; }

		case CSmoothingMode_None:
		case CSmoothingMode_HighSpeed:
		case CSmoothingMode_Default:
		default:
		{
			switch(pixelOffset)
			{
				case CPixelOffsetMode_HighQuality:
				case CPixelOffsetMode_Half:
					{ return 1; }

				case CPixelOffsetMode_None:
				case CPixelOffsetMode_HighSpeed:
				case CPixelOffsetMode_Default:
				default:
					{ return 0; }
			}
		}
	}
}


#ifdef __cplusplus
};
#endif
