/*
 * BuiltinBitmaps.cs - Builtin bitmaps for drawing special decorations.
 *
 * Copyright (C) 2002, 2003  Southern Storm Software, Pty Ltd.
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

namespace Xsharp
{

using System;

internal sealed class BuiltinBitmaps
{

	// Size of special bitmaps.
	public const int RadioWidth = 12;
	public const int RadioHeight = 12;

	// Radio button - bottom shadow color.
	private static readonly byte[] radio_b_bits = {
   		0xf0, 0x00, 0x0c, 0x03, 0x02, 0x00, 0x02, 0x02, 0x01, 0x00, 0x01, 0x00,
   		0x01, 0x00, 0x01, 0x00, 0x02, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00
	};

	// Radio button - enhanced bottom shadow color.
	private static readonly byte[] radio_B_bits = {
   		0x00, 0x00, 0xf0, 0x00, 0x0c, 0x03, 0x04, 0x00, 0x02, 0x00, 0x02, 0x00,
   		0x02, 0x00, 0x02, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	};

	// Radio button - top shadow color.
	private static readonly byte[] radio_t_bits = {
   		0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x04, 0x00, 0x08, 0x00, 0x08,
   		0x00, 0x08, 0x00, 0x08, 0x00, 0x04, 0x00, 0x04, 0x0c, 0x03, 0xf0, 0x00
	};

	// Radio button - enhanced top shadow color.
	private static readonly byte[] radio_T_bits = {
   		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x04,
   		0x00, 0x04, 0x00, 0x04, 0x00, 0x02, 0x0c, 0x03, 0xf0, 0x00, 0x00, 0x00
	};

	// Radio button - "white" background.
	private static readonly byte[] radio_w_bits = {
   		0x00, 0x00, 0x00, 0x00, 0xf0, 0x00, 0xf8, 0x01, 0x9c, 0x03, 0x0c, 0x03,
   		0x0c, 0x03, 0x9c, 0x03, 0xf8, 0x01, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00
	};

	// Radio button - "black" foreground.
	private static readonly byte[] radio_f_bits = {
   		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x00, 0xf0, 0x00,
   		0xf0, 0x00, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	};

	// Pre-loaded bitmaps for the current object's display.
	public Xlib.Pixmap RadioBottom;
	public Xlib.Pixmap RadioBottomEnhanced;
	public Xlib.Pixmap RadioTop;
	public Xlib.Pixmap RadioTopEnhanced;
	public Xlib.Pixmap RadioBackground;
	public Xlib.Pixmap RadioForeground;

	// Load builtin bitmaps for a particular display.
	public BuiltinBitmaps(Display display)
			{
				IntPtr dpy = display.dpy;
				Xlib.Drawable drawable = display.DefaultRootWindow.handle;
				RadioBottom = Xlib.XCreateBitmapFromData
					(dpy, drawable, radio_b_bits, (uint)12, (uint)12);
				RadioBottomEnhanced = Xlib.XCreateBitmapFromData
					(dpy, drawable, radio_B_bits, (uint)12, (uint)12);
				RadioTop = Xlib.XCreateBitmapFromData
					(dpy, drawable, radio_t_bits, (uint)12, (uint)12);
				RadioTopEnhanced = Xlib.XCreateBitmapFromData
					(dpy, drawable, radio_T_bits, (uint)12, (uint)12);
				RadioBackground = Xlib.XCreateBitmapFromData
					(dpy, drawable, radio_w_bits, (uint)12, (uint)12);
				RadioForeground = Xlib.XCreateBitmapFromData
					(dpy, drawable, radio_f_bits, (uint)12, (uint)12);
			}

} // class BuiltinBitmaps

} // namespace Xsharp
