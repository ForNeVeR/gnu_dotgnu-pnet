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
#include <X11/Xatom.h>
#ifdef WIN32
	#include <X11/Xwinsock.h>
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

/*
 * Create a font from a description.
 */
XFontSet XSharpCreateFont(Display *dpy, const char *family,
						  int pointSize, int style)
{
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
}

/*
 * Draw a string using a font set.
 */
void XSharpDrawString(Display *dpy, Drawable drawable, GC gc,
					  XFontSet fontSet, int x, int y,
					  const char *str, int style)
{
	XRectangle overall_ink_return;
	XRectangle overall_logical_return;
	XFontSetExtents *extents;
	int line1, line2;

	/* Draw the string itself */
	XmbDrawString(dpy, drawable, fontSet, gc, x, y,
				  str, strlen(str));

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
		extents = XExtentsOfFontSet(fontSet);
		if(extents)
		{
			line2 = y + (extents->max_logical_extent.y / 2);
		}
		else
		{
			line2 = y;
		}
	}
	else
	{
		line2 = y;
	}

	/* Draw the underline and strike-out */
	if(line1 != y || line2 != y)
	{
		XmbTextExtents(fontSet, str, strlen(str),
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
void XSharpTextExtents(XFontSet fontSet, const char *str,
					   XRectangle *overall_ink_return,
					   XRectangle *overall_logical_return)
{
	XmbTextExtents(fontSet, str, strlen(str),
			 	   overall_ink_return, overall_logical_return);
}

/*
 * Calculate the extent information for a font.
 */
void XSharpFontExtents(XFontSet fontSet,
					   XRectangle *max_ink_return,
					   XRectangle *max_logical_return)
{
	XFontSetExtents *extents;
	extents = XExtentsOfFontSet(fontSet);
	if(extents)
	{
		*max_ink_return = extents->max_ink_extent;
		*max_logical_return = extents->max_logical_extent;
	}
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

void XSharpDrawString(void *dpy, unsigned long drawable, void *gc,
					  void *fontSet, int x, int y,
					  const char *str, int style)
{
	/* Nothing to do here */
}

void XSharpTextExtents(void *fontSet, const char *str,
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

#endif /* X_DISPLAY_MISSING || !HAVE_SELECT */
