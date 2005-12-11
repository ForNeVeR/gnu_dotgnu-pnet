/*
 * sample1.c - Simple sample case for libCrayons.
 *
 * Copyright (C) 2005  Deryk Robosson
 *
 * This program is free software, you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY, without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program, if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
 
 #include <Crayons.h>	/* includes X11/Xlib.h if HAVE_X11_XLIB_H is defined */
 
 #include <stdio.h>
 #include <stdlib.h>
 #include <unistd.h>
 
 int main()
 {
 	/* TODO */
	Display *display;
	int screen_number;
	Window win;
	unsigned int disp_width, disp_height;
	unsigned int width, height;
	unsigned int win_x, win_y;
	unsigned int win_border_width;
	char *display_name = getenv("DISPLAY");

	display = XOpenDisplay(display_name);
	if(display == NULL)
	{
		fprintf(stderr, "cannot connect to Xserver %s\n",
			display_name);
		exit(1);
	}

	screen_number = DefaultScreen(display);
	disp_width = DisplayWidth(display, screen_number);
	disp_height = DisplayHeight(display, screen_number);
	width = disp_width / 3;
	height = disp_height / 3;
	win_x = 0;
	win_y = 0;
	win_border_width = 2;

	win = XCreateSimpleWindow(display,
		RootWindow(display, screen_number),
		win_x, win_y,
		width, height,
		win_border_width,
		BlackPixel(display, screen_number),
		WhitePixel(display, screen_number));

	XMapWindow(display, win);
	XSync(display, 0);

	sleep(4);

	XCloseDisplay(display);
 }
 
