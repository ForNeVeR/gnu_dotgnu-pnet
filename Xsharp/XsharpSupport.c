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

#else /* X_DISPLAY_MISSING || !HAVE_SELECT */

int XNextEventWithTimeout(void *dpy, void *event, int timeout)
{
	return -1;
}

#endif /* X_DISPLAY_MISSING || !HAVE_SELECT */
