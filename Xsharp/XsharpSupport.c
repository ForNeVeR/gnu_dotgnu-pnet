/*
 * XsharpSupport.c - C support code for Xsharp.
 *
 * Copyright (C) 2003  Southern Storm Software, Pty Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#if !defined(X_DISPLAY_MISSING) && HAVE_SELECT

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#ifdef WIN32
	#include <X11/Xwinsock.h>
#endif
#ifdef USE_XFT_EXTENSION
	#include <X11/Xft/Xft.h>
#endif
#if TIME_WITH_SYS_TIME
	#include <sys/time.h>
    #include <time.h>
#else
    #if HAVE_SYS_TIME_H
		#include <sys/time.h>
    #else
        #include <time.h>
    #endif
#endif
#if HAVE_SYS_TYPES_H
	#include <sys/types.h>
#endif
#if HAVE_SYS_SELECT_H
	#include <sys/select.h>
#endif
#if HAVE_UNISTD_H
	#include <unistd.h>
#endif
#include <stdlib.h>
#include <stdio.h>

int XNextEventWithTimeout(Display *dpy, XEvent *event, int timeout)
{
	int fd = ConnectionNumber(dpy);
	fd_set readSet;
	struct timeval tv;
	int result;

	/* Initialize the read set */
	FD_ZERO(&readSet);
	FD_SET(fd, &readSet);

	/* Select on the display connection */
	if(timeout >= 0)
	{
		tv.tv_sec = timeout / 1000;
		tv.tv_usec = (timeout % 1000) * 1000;
		result = select(fd + 1, &readSet, (fd_set *)0, (fd_set *)0, &tv);
	}
	else
	{
		result = select(fd + 1, &readSet, (fd_set *)0, (fd_set *)0,
					    (struct timeval *)0);
	}

	/* If there was activity on the connection, then read the event */
	if(result > 0)
	{
		XNextEvent(dpy, event);
	}

	/* Return the final result to the caller */
	return result;
}

/*
 * Font style flags.
 */
#define	FontStyle_Normal		0
#define	FontStyle_Bold			1
#define	FontStyle_Italic		2
#define	FontStyle_Underline		4
#define	FontStyle_StrikeOut		8

#ifndef USE_XFT_EXTENSION

/*
 * Try to create a font with specific parameters.
 */
static XFontSet TryCreateFont(Display *dpy, const char *family,
							  int pointSize, int style)
{
	XFontSet fontSet;
	char *name;
	const char *weight;
	const char *slant;
	char **missingCharsets;
	int missingCharsetCount;
	char *defStringReturn;

	/* Create a buffer big enough to hold the base XLFD */
	name = (char *)malloc((family ? strlen(family) : 1) + 128);
	if(!name)
	{
		return 0;
	}

	/* Determine the weight and slant to use */
	if((style & FontStyle_Bold) != 0)
	{
		weight = "bold";
	}
	else
	{
		weight = "medium";
	}
	if((style & FontStyle_Italic) != 0)
	{
		slant = "i";
	}
	else
	{
		slant = "r";
	}

	/* Create the base XLFD */
	if(pointSize != -1)
	{
		sprintf(name, "-*-%s-%s-%s-*-*-*-%d-*-*-*-*-*-*",
				(family ? family : "*"), weight, slant, pointSize);
	}
	else
	{
		sprintf(name, "-*-%s-%s-%s-*-*-*-*-*-*-*-*-*-*",
				(family ? family : "*"), weight, slant);
	}

	/* Try to create the font set using just the base name */
	missingCharsets = 0;
	missingCharsetCount = 0;
	defStringReturn = 0;
	fontSet = XCreateFontSet(dpy, name, &missingCharsets,
							 &missingCharsetCount, &defStringReturn);
	if(fontSet)
	{
		free(name);
		return fontSet;
	}
	/* TODO: process the missing charsets */

	/* Give up - we couldn't create this variant */
	free(name);
	return 0;
}

#endif /* USE_XFT_EXTENSION */

/*
 * Create a font from a description.
 */
void *XSharpCreateFont(Display *dpy, const char *family,
					   int pointSize, int style)
{
#ifdef USE_XFT_EXTENSION

	XftPattern *pattern;
	XftPattern *matched;
	XftFont *font;
	XftResult result;

	/* Create the font pattern to be used */
	pattern = XftPatternCreate();
	if(!pattern)
	{
		return 0;
	}
	if(!XftPatternAddString(pattern, XFT_FAMILY, family))
	{
		XftPatternDestroy(pattern);
		return 0;
	}
	if(!XftPatternAddDouble(pattern, XFT_SIZE, ((double)pointSize) / 10.0))
	{
		XftPatternDestroy(pattern);
		return 0;
	}
	if((style & FontStyle_Bold) != 0)
	{
		if(!XftPatternAddInteger(pattern, XFT_WEIGHT, XFT_WEIGHT_BOLD))
		{
			XftPatternDestroy(pattern);
			return 0;
		}
	}
	if((style & FontStyle_Italic) != 0)
	{
		if(!XftPatternAddInteger(pattern, XFT_SLANT, XFT_SLANT_ITALIC))
		{
			XftPatternDestroy(pattern);
			return 0;
		}
	}

	/* Perform font matching to find the closest possible font */
	matched = XftFontMatch(dpy, DefaultScreen(dpy), pattern, &result);
	XftPatternDestroy(pattern);
	if(!matched)
	{
		return 0;
	}

	/* Create an Xft font based on the matched pattern */
	return XftFontOpenPattern(dpy, matched);

#else /* !USE_XFT_EXTENSION */

	XFontSet fontSet;

	/* Try with the actual parameters first */
	fontSet = TryCreateFont(dpy, family, pointSize, style);
	if(fontSet)
	{
		return fontSet;
	}

	/* Remove the style, but keep the family */
	fontSet = TryCreateFont(dpy, family, pointSize, FontStyle_Normal);
	if(fontSet)
	{
		return fontSet;
	}

	/* Remove the family, but keep the style */
	fontSet = TryCreateFont(dpy, 0, pointSize, style);
	if(fontSet)
	{
		return fontSet;
	}

	/* Remove the point size and the style, but keep the family */
	fontSet = TryCreateFont(dpy, family, -1, FontStyle_Normal);
	if(fontSet)
	{
		return fontSet;
	}

	/* Remove everything - this will succeed unless X has no fonts at all! */
	return TryCreateFont(dpy, 0, -1, FontStyle_Normal);

#endif
}

/*
 * Free a font set.
 */
void XSharpFreeFont(Display *dpy, void *fontSet)
{
#ifdef USE_XFT_EXTENSION
	XftFontClose(dpy, (XftFont *)fontSet);
#else
	XFreeFontSet(dpy, (XFontSet)fontSet);
#endif
}

/*
 * Forward declaration.
 */
void XSharpTextExtents(Display *dpy, void *fontSet, const char *str,
					   XRectangle *overall_ink_return,
					   XRectangle *overall_logical_return);

/*
 * Draw a string using a font set.
 */
void XSharpDrawString(Display *dpy, Drawable drawable, GC gc,
					  void *fontSet, int x, int y,
					  const char *str, int style, Region clipRegion,
					  unsigned long colorValue)
{
	XRectangle overall_ink_return;
	XRectangle overall_logical_return;
#ifdef USE_XFT_EXTENSION
	XftDraw *draw;
	XftColor color;
	XGCValues values;
#else
	XFontSetExtents *extents;
#endif
	int line1, line2;

#ifdef USE_XFT_EXTENSION

	/* TODO: 16-bit fonts */

	/* Set up the Xft color value to draw with */
	XGetGCValues(dpy, gc, GCForeground, &values);
	color.pixel = values.foreground;
	color.color.red = (unsigned short)(((colorValue >> 16) & 0xFF) << 8);
	color.color.green = (unsigned short)(((colorValue >> 8) & 0xFF) << 8);
	color.color.blue = (unsigned short)((colorValue & 0xFF) << 8);
	color.color.alpha = (unsigned short)0xFFFF;

	/* Draw the string */
	draw = XftDrawCreate(dpy, drawable,
						 DefaultVisual(dpy, DefaultScreen(dpy)),
						 DefaultColormap(dpy, DefaultScreen(dpy)));
	if(draw)
	{
		if(clipRegion)
		{
			XftDrawSetClip(draw, clipRegion);
		}
		XftDrawString8(draw, &color, (XftFont *)fontSet,
					   x, y, (XftChar8 *)str, strlen(str));
		XftDrawDestroy(draw);
	}

#else

	/* Draw the string using the core API */
	XmbDrawString(dpy, drawable, (XFontSet)fontSet, gc, x, y,
				  str, strlen(str));

#endif

	/* Calculate the positions of the underline and strike-out */
	if((style & FontStyle_Underline) != 0)
	{
		line1 = y + 2;
	}
	else
	{
		line1 = y;
	}
	if((style & FontStyle_StrikeOut) != 0)
	{
	#ifdef USE_XFT_EXTENSION
		line2 = y + (((XftFont *)fontSet)->height / 2);
	#else
		extents = XExtentsOfFontSet(fontSet);
		if(extents)
		{
			line2 = y + (extents->max_logical_extent.y / 2);
		}
		else
		{
			line2 = y;
		}
	#endif
	}
	else
	{
		line2 = y;
	}

	/* Draw the underline and strike-out */
	if(line1 != y || line2 != y)
	{
		XSharpTextExtents(dpy, fontSet, str,
				 	      &overall_ink_return, &overall_logical_return);
		XSetLineAttributes(dpy, gc, 1, LineSolid, CapNotLast, JoinMiter);
		if(line1 != y)
		{
			XDrawLine(dpy, drawable, gc, x, line1,
					  x + overall_logical_return.width, line1);
		}
		if(line2 != y)
		{
			XDrawLine(dpy, drawable, gc, x, line2,
					  x + overall_logical_return.width, line2);
		}
	}
}

/*
 * Calculate the extent information for a string.
 */
void XSharpTextExtents(Display *dpy, void *fontSet, const char *str,
					   XRectangle *overall_ink_return,
					   XRectangle *overall_logical_return)
{
#ifdef USE_XFT_EXTENSION

	/* TODO: 16-bit fonts */

	XGlyphInfo extents;
	XftTextExtents8(dpy, fontSet, (XftChar8 *)str, strlen(str), &extents);

	overall_ink_return->x = -(extents.x);
	overall_ink_return->y = -(extents.y);
	overall_ink_return->width = extents.width;
	overall_ink_return->height = extents.height;

	overall_logical_return->x = -(extents.x);
	overall_logical_return->y = -(extents.y);
	overall_logical_return->width = extents.x + extents.xOff;
	overall_logical_return->height = extents.y + extents.yOff;

#else
	XmbTextExtents((XFontSet)fontSet, str, strlen(str),
			 	   overall_ink_return, overall_logical_return);
#endif
}

/*
 * Calculate the extent information for a font.
 */
void XSharpFontExtents(void *fontSet,
					   XRectangle *max_ink_return,
					   XRectangle *max_logical_return)
{
#ifdef USE_XFT_EXTENSION

	/* Synthesize enough information to keep "Xsharp.FontExtents" happy */
	max_logical_return->x = 0;
	max_logical_return->y = -(((XftFont *)fontSet)->ascent);
	max_logical_return->width = ((XftFont *)fontSet)->max_advance_width;
	max_logical_return->height = ((XftFont *)fontSet)->ascent +
								 ((XftFont *)fontSet)->descent;
	*max_ink_return = *max_logical_return;

#else

	XFontSetExtents *extents;
	extents = XExtentsOfFontSet((XFontSet)fontSet);
	if(extents)
	{
		*max_ink_return = extents->max_ink_extent;
		*max_logical_return = extents->max_logical_extent;
	}

#endif
}

/*
 * Get the contents of the RESOURCE_MANAGER property on the root window.
 */
char *XSharpGetResources(Display *dpy, Window root)
{
	Atom resourceManager;
	Atom actualTypeReturn;
	int actualFormatReturn;
	unsigned long nitemsReturn;
	unsigned long bytesAfterReturn;
	unsigned char *propReturn;

	/* Register the RESOURCE_MANAGER atom with the X server */
	resourceManager = XInternAtom(dpy, "RESOURCE_MANAGER", False);

	/* Get the property, stopping at 1k characters */
	propReturn = 0;
	nitemsReturn = 0;
	bytesAfterReturn = 0;
	XGetWindowProperty(dpy, root, resourceManager, 0, 1024, False,
					   XA_STRING, &actualTypeReturn, &actualFormatReturn,
					   &nitemsReturn, &bytesAfterReturn, &propReturn);
	if(bytesAfterReturn > 0)
	{
		/* We now know the real size, so fetch it properly this time */
		if(propReturn)
		{
			XFree((void *)propReturn);
		}
		propReturn = 0;
		XGetWindowProperty(dpy, root, resourceManager, 0,
						   (long)(1024 + bytesAfterReturn), False,
						   XA_STRING, &actualTypeReturn, &actualFormatReturn,
						   &nitemsReturn, &bytesAfterReturn, &propReturn);
	}
	return (char *)propReturn;
}

/*
 * Free a return value from "XSharpGetResources".
 */
void XSharpFreeResources(char *value)
{
	if(value)
	{
		XFree((void *)value);
	}
}

/*
 * Pixel formats, which must match "DotGNU.Images.PixelFormat".
 */
#define	PF_Undefined				0x00000000
#define	PF_DontCare					0x00000000
#define	PF_Max						0x0000000F
#define	PF_Indexed					0x00010000
#define	PF_Gdi						0x00020000
#define	PF_Format16bppRgb555		0x00021005
#define	PF_Format16bppRgb565		0x00021006
#define	PF_Format24bppRgb			0x00021808
#define	PF_Format32bppRgb			0x00022009
#define	PF_Format1bppIndexed		0x00030101
#define	PF_Format4bppIndexed		0x00030402
#define PF_Format8bppIndexed		0x00030803
#define PF_Alpha					0x00040000
#define PF_Format16bppArgb1555		0x00061007
#define PF_PAlpha					0x00080000
#define PF_Format32bppPArgb			0x000E200B
#define PF_Extended					0x00100000
#define PF_Format16bppGrayScale		0x00101004
#define PF_Format48bppRgb			0x0010300C
#define PF_Format64bppPArgb			0x001C400E
#define PF_Canonical				0x00200000
#define PF_Format32bppArgb			0x0026200A
#define PF_Format64bppArgb			0x0034400D

/*
 * Read the contents of a scan line as ARGB values.  The alpha component
 * can be omitted if the format does not include "PF_Alpha".
 */
static void Read_16bppRgb555(unsigned char *input,
							 unsigned long *output, int num)
{
	unsigned short value;
	while(num > 0)
	{
		value = *((unsigned short *)input);
		*output++ = (((unsigned long)(value & 0x7C00)) << 9) |
					(((unsigned long)(value & 0x03E0)) << 6) |
					(((unsigned long)(value & 0x001F)) << 3);
		input += 2;
		--num;
	}
}
static void Read_16bppRgb565(unsigned char *input,
							 unsigned long *output, int num)
{
	unsigned short value;
	while(num > 0)
	{
		value = *((unsigned short *)input);
		*output++ = (((unsigned long)(value & 0xF800)) << 8) |
					(((unsigned long)(value & 0x07E0)) << 5) |
					(((unsigned long)(value & 0x001F)) << 3);
		input += 2;
		--num;
	}
}
static void Read_16bppArgb1555(unsigned char *input,
							   unsigned long *output, int num)
{
	unsigned short value;
	while(num > 0)
	{
		value = *((unsigned short *)input);
		*output = (((unsigned long)(value & 0x7C00)) << 9) |
				  (((unsigned long)(value & 0x03E0)) << 6) |
				  (((unsigned long)(value & 0x001F)) << 3);
		if((value & 0x8000) != 0)
		{
			*output |= (unsigned long)0xFF000000;
		}
		input += 2;
		--num;
	}
}
static void Read_16bppGrayScale(unsigned char *input,
							    unsigned long *output, int num)
{
	unsigned short value;
	while(num > 0)
	{
		value = (*((unsigned short *)input) >> 8);
		*output++ = (((unsigned long)value) << 16) |
					(((unsigned long)value) << 8) |
					 ((unsigned long)value);
		input += 2;
		--num;
	}
}
static void Read_24bppRgb(unsigned char *input,
					      unsigned long *output, int num)
{
	while(num > 0)
	{
		*output++ = ((unsigned long)(input[0])) |
					(((unsigned long)(input[1])) << 8) |
					(((unsigned long)(input[2])) << 16);
		input += 3;
		--num;
	}
}
static void Read_32bppRgb(unsigned char *input,
					      unsigned long *output, int num)
{
	while(num > 0)
	{
		*output++ = ((unsigned long)(input[0])) |
					(((unsigned long)(input[1])) << 8) |
					(((unsigned long)(input[2])) << 16);
		input += 4;
		--num;
	}
}
static void Read_32bppArgb(unsigned char *input,
					       unsigned long *output, int num)
{
	while(num > 0)
	{
		*output++ = ((unsigned long)(input[0])) |
					(((unsigned long)(input[1])) << 8) |
					(((unsigned long)(input[2])) << 16) |
					(((unsigned long)(input[3])) << 24);
		input += 4;
		--num;
	}
}
static void Read_48bppRgb(unsigned char *input,
					      unsigned long *output, int num)
{
	while(num > 0)
	{
		*output++ = ((unsigned long)(input[1])) |
					(((unsigned long)(input[3])) << 8) |
					(((unsigned long)(input[5])) << 16);
		input += 6;
		--num;
	}
}
static void Read_64bppArgb(unsigned char *input,
					       unsigned long *output, int num)
{
	while(num > 0)
	{
		*output++ = ((unsigned long)(input[1])) |
					(((unsigned long)(input[3])) << 8) |
					(((unsigned long)(input[5])) << 16) |
					(((unsigned long)(input[7])) << 24);
		input += 8;
		--num;
	}
}

/*
 * Get the read function for a format.
 */
typedef void (*ReadFunc)(unsigned char *input, unsigned long *output, int num);
static ReadFunc GetReadFunc(int pixelFormat)
{
	switch(pixelFormat)
	{
		case PF_Format16bppRgb555:		return Read_16bppRgb555;
		case PF_Format16bppRgb565:		return Read_16bppRgb565;
		case PF_Format24bppRgb:			return Read_24bppRgb;
		case PF_Format32bppRgb:			return Read_32bppRgb;
		case PF_Format16bppArgb1555:	return Read_16bppArgb1555;
		case PF_Format32bppPArgb:		return Read_32bppArgb;
		case PF_Format16bppGrayScale:	return Read_16bppGrayScale;
		case PF_Format48bppRgb:			return Read_48bppRgb;
		case PF_Format64bppPArgb:		return Read_64bppArgb;
		case PF_Format32bppArgb:		return Read_32bppArgb;
		case PF_Format64bppArgb:		return Read_64bppArgb;
		default:						return 0;
	}
}

/*
 * Write RGB data to an indexed XImage structure.
 */
static void Write_Indexed(XImage *image, int line, unsigned long *input,
						  int num, unsigned long *palette)
{
	int column;
	unsigned long pixel;
	for(column = 0; column < num; ++column)
	{
		pixel = *input++;
		pixel = (((pixel & 0x00FF0000) >> 16) * 5 / 255) * 36 +
		        (((pixel & 0x0000FF00) >> 8) * 5 / 255) * 6 +
		        ((pixel & 0x000000FF) * 5 / 255);
		XPutPixel(image, column, line, palette[pixel]);
	}
}

/*
 * Get the write function for an image.
 */
typedef void (*WriteFunc)(unsigned char *output, unsigned long *input, int num);
static WriteFunc GetWriteFunc(XImage *image)
{
	/* TODO */
	return 0;
}

/*
 * Forward declaration.
 */
void XSharpDestroyImage(XImage *image);

/*
 * Create an XImage structure from a device-independent bitmap.
 *
 * If the input image is indexed, then "palette" indicates the
 * converted pixel values of the image's palette.
 *
 * If the input image is RGB, and the screen is indexed, then
 * "palette" indicates a 216-color palette to convert the image to.
 */
XImage *XSharpCreateImageFromDIB(Screen *screen, int width, int height,
								 int stride, int pixelFormat,
								 unsigned char *data, int isMask,
								 unsigned long *palette)
{
	Display *dpy = DisplayOfScreen(screen);
	Visual *visual = DefaultVisualOfScreen(screen);
	unsigned int depth;
	int format;
	int bitmap_pad;
	XImage *image;
	unsigned char *imageData;
	unsigned char *temp1;
	unsigned char *temp2;
	int line, column;
	unsigned long *tempLine;
	ReadFunc readFunc;
	WriteFunc writeFunc;

	/* Get the depth, format, and pad values */
	if(isMask)
	{
		depth = 1;
		format = XYBitmap;
		bitmap_pad = 8;
	}
	else
	{
		depth = DefaultDepthOfScreen(screen);
		format = ZPixmap;
		if(depth <= 8)
		{
			bitmap_pad = 8;
		}
		else if(depth <= 16)
		{
			bitmap_pad = 16;
		}
		else
		{
			bitmap_pad = 32;
		}
	}

	/* Create the main image structure */
	image = XCreateImage(dpy, visual, depth, format, 0, 0,
						 (unsigned int)width, (unsigned int)height,
						 bitmap_pad, 0);
	if(!image)
	{
		return 0;
	}

	/* Allocate space for the image.  Conventional wisdom says that
	   we should use shared memory for this.  However, on today's
	   hardware, most users will never notice the difference unless
	   the system is displaying full-motion video.  Which we aren't */
	imageData = (unsigned char *)malloc(image->bytes_per_line *
										image->height);
	if(!imageData)
	{
		XDestroyImage(image);
		return 0;
	}
	image->data = (char *)imageData;

	/* Convert the input image into raw X pixels within the XImage.
	   There are probably a lot of things that can be done to speed
	   this up, although for most small icon-sized images, it usually
	   doesn't matter too much */
	if(isMask)
	{
		for(line = 0; line < height; ++line)
		{
			temp1 = imageData + line * image->bytes_per_line;
			temp2 = data + line * stride;
			for(column = (int)(image->bytes_per_line); column > 0; --column)
			{
				*temp1++ = *temp2++;
			}
		}
	}
	else if((pixelFormat & PF_Indexed) != 0)
	{
		for(line = 0; line < height; ++line)
		{
			temp1 = data + line * stride;
			if(pixelFormat == PF_Format1bppIndexed)
			{
				/* Source image has one bit per pixel */
				for(column = 0; column < width; column += 2)
				{
					if((temp1[column / 8] & (0x80 >> (column % 8))) != 0)
					{
						XPutPixel(image, column, line, palette[1]);
					}
					else
					{
						XPutPixel(image, column, line, palette[0]);
					}
				}
			}
			else if(pixelFormat == PF_Format4bppIndexed)
			{
				/* Source image has four bits per pixel */
				for(column = 0; column < width; column += 2)
				{
					XPutPixel(image, column, line,
							  palette[(*temp1 >> 4) & 0x0F]);
					if((column + 1) < width)
					{
						XPutPixel(image, column, line,
								  palette[*temp1 & 0x0F]);
					}
					++temp1;
				}
			}
			else
			{
				/* Source image has eight bits per pixel */
				for(column = 0; column < width; ++column)
				{
					XPutPixel(image, column, line, palette[*temp1]);
					++temp1;
				}
			}
		}
	}
	else if(visual->class == TrueColor || visual->class == DirectColor)
	{
		/* Map an RGB image to an RGB-capable display */
		tempLine = (unsigned long *)(malloc(sizeof(unsigned long) * width));
		if(!tempLine)
		{
			XSharpDestroyImage(image);
			return 0;
		}
		readFunc = GetReadFunc(pixelFormat);
		writeFunc = GetWriteFunc(image);
		if(!readFunc || !writeFunc)
		{
			free(tempLine);
			XSharpDestroyImage(image);
			return 0;
		}
		for(line = 0; line < height; ++line)
		{
			(*readFunc)(data + line * stride, tempLine, width);
			(*writeFunc)(imageData + line * image->bytes_per_line,
						 tempLine, width);
		}
		free(tempLine);
	}
	else
	{
		/* Map an RGB image to an indexed display (usually 8-bit) */
		tempLine = (unsigned long *)(malloc(sizeof(unsigned long) * width));
		if(!tempLine)
		{
			XSharpDestroyImage(image);
			return 0;
		}
		readFunc = GetReadFunc(pixelFormat);
		if(!readFunc)
		{
			free(tempLine);
			XSharpDestroyImage(image);
			return 0;
		}
		for(line = 0; line < height; ++line)
		{
			(*readFunc)(data + line * stride, tempLine, width);
			Write_Indexed(image, line, tempLine, width, palette);
		}
		free(tempLine);
	}

	/* Return the final image structure to the caller */
	return image;
}

/*
 * Destroy an image that was created by "XSharpCreateImageFromDIB".
 */
void XSharpDestroyImage(XImage *image)
{
	if(image)
	{
		if(image->data)
		{
			free((void *)(image->data));
			image->data = 0;
		}
		XDestroyImage(image);
	}
}

/*
 * Internal structure of X11 region objects.  We have to define
 * this here because there is no public header that contains it.
 */
typedef struct
{
    short x1, x2, y1, y2;

} BOX;
typedef struct
{
    long size;
    long numRects;
    BOX *rects;
    BOX extents;

} REGION;

/*
 * Get the number of rectangles contained in a region structure.
 */
int XSharpGetRegionSize(Region region)
{
	return (int)(((REGION *)region)->numRects);
}

/*
 * Get the bounding area of a particular rectangle within a region structure.
 */
void XSharpGetRegionRect(Region region, int index, XRectangle *rect)
{
	BOX *box = &(((REGION *)region)->rects[index]);
	rect->x = box->x1;
	rect->y = box->y1;
	rect->width = box->x2 - box->x1;
	rect->height = box->y2 - box->y1;
}

#else /* X_DISPLAY_MISSING || !HAVE_SELECT */

int XNextEventWithTimeout(void *dpy, void *event, int timeout)
{
	/* Nothing to do here */
	return -1;
}

void *XSharpCreateFont(void *dpy, const char *family, int pointSize, int style)
{
	/* Nothing to do here */
	return 0;
}

void XSharpFreeFont(void *dpy, void *fontSet)
{
	/* Nothing to do here */
}

void XSharpDrawString(void *dpy, unsigned long drawable, void *gc,
					  void *fontSet, int x, int y,
					  const char *str, int style, void *clipRegion,
					  unsigned long colorValue)
{
	/* Nothing to do here */
}

void XSharpTextExtents(void *dpy, void *fontSet, const char *str,
					   void *overall_ink_return,
					   void *overall_logical_return)
{
	/* Nothing to do here */
}

void XSharpFontExtents(void *fontSet,
					   void *max_ink_return,
					   void *max_logical_return)
{
	/* Nothing to do here */
}

char *XSharpGetResources(void *dpy, unsigned long root)
{
	/* Nothing to do here */
	return 0;
}

void XSharpFreeResources(char *value)
{
	/* Nothing to do here */
}

void *XSharpCreateImageFromDIB(void *screen, int width, int height,
							   int stride, int pixelFormat,
							   unsigned char *data, int isMask,
							   unsigned long *palette)
{
	/* Nothing to do here */
	return 0;
}

void XSharpDestroyImage(void *image)
{
	/* Nothing to do here */
}

int XSharpGetRegionSize(void *region)
{
	/* Nothing to do here */
	return 0;
}

void XSharpGetRegionRect(void *region, int index, void *rect)
{
	/* Nothing to do here */
}

#endif /* X_DISPLAY_MISSING || !HAVE_SELECT */
