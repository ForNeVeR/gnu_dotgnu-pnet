/*
 * IToolkit.cs - Implementation of the "System.Drawing.Toolkit.IToolkit" class.
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

namespace System.Drawing.Toolkit
{

using System.Drawing.Drawing2D;
using System.Drawing.Imaging;
using System.Drawing.Text;

public interface IToolkit
{
	// Process events in the event queue.  If "waitForEvent" is true,
	// then wait for the next event and return "false" if "Quit" was
	// seen.  If "waitForEvent" is false, then process events in the
	// queue and return "true".  If "waitForEvent" is false and there
	// are no events in the queue, then return "false".
	bool ProcessEvents(bool waitForEvent);

	// Send a quit message to the toolkit, which should cause
	// it to exit from the "Run" method.
	void Quit();

	// Resolve a system color to an RGB value.  Returns -1 if the
	// system does not support the color and a default should be used.
	int ResolveSystemColor(KnownColor color);

	// Create an IToolkitGraphics object from a HDC.
	IToolkitGraphics CreateFromHdc(IntPtr hdc, IntPtr hdevice);

	// Create an IToolkitGraphics object from a HWND.
	IToolkitGraphics CreateFromHwnd(IntPtr hwnd);

	// Create a solid toolkit brush.
	IToolkitBrush CreateSolidBrush(Color color);

	// Create a hatched toolkit brush.
	IToolkitBrush CreateHatchBrush(HatchStyle style, Color foreColor,
								   Color backColor);

	// Create a linear gradient brush.  Returns null if the
	// toolkit does not support linear gradient brushes.
	IToolkitBrush CreateLinearGradientBrush
			(RectangleF rect, Color color1, Color color2,
			 LinearGradientMode mode);
	IToolkitBrush CreateLinearGradientBrush
			(RectangleF rect, Color color1, Color color2,
			 float angle, bool isAngleScaleable);

	// Create a texture brush.
	IToolkitBrush CreateTextureBrush
				(TextureBrush properties, RectangleF dstRect,
				 ImageAttributes imageAttr);

	// Create a toolkit pen from the properties in the specified object.
	// If the toolkit does not support the precise combination of pen
	// properties, it will return the closest matching pen.
	IToolkitPen CreatePen(Pen pen);

	// Create a toolkit font from the properties in the specified object.
	IToolkitFont CreateFont(Font font);

	// Get the handle for the halftone palette.  IntPtr.Zero if not supported.
	IntPtr GetHalftonePalette();

	// Create a top-level application window.
	IToolkitWindow CreateTopLevelWindow(int width, int height);

	// Create a top-level dialog shell.
	IToolkitWindow CreateTopLevelDialog
			(int width, int height, bool modal, bool resizable,
			 IToolkitWindow dialogParent);

	// Create a top-level menu shell.
	IToolkitWindow CreateTopLevelMenu(int x, int y, int width, int height);

	// Create a child window.  If "parent" is null, then the child
	// does not yet have a "real" parent - it will be reparented later.
	IToolkitWindow CreateChildWindow(IToolkitWindow parent,
									 int x, int y, int width, int height);

	// Get a list of all font families on this system, or all font
	// families that are compatible with a particular IToolkitGraphics.
	FontFamily[] GetFontFamilies(IToolkitGraphics graphics);

	// Get font family metric information.
	void GetFontFamilyMetrics(GenericFontFamilies genericFamily,
							  String name, FontStyle style,
							  out int ascent, out int descent,
							  out int emHeight, out int lineSpacing);

	// Get the IToolkitFont that corresponds to a hdc's current font.
	// Returns null if there is no way to obtain the information.
	IToolkitFont GetFontFromHdc(IntPtr hdc);

	// Get the IToolkitFont that corresponds to a native font object.
	// Returns null if there is no way to obtain the information.
	IToolkitFont GetFontFromHfont(IntPtr hfont);

	// Get the IToolkitFont that corresponds to LOGFONT information.
	// Returns null if there is no way to obtain the information.
	IToolkitFont GetFontFromLogFont(Object lf, IntPtr hdc);

	// Get the default IToolkitGraphics object to measure screen sizes.
	IToolkitGraphics GetDefaultGraphics();

	// Get the screen size, in pixels.
	Size GetScreenSize();

	// Get the working area of the screen, excluding task bars, etc.
	Rectangle GetWorkingArea();

	// Get the adjustment values for a top-level window, to convert
	// between window bounds and client bounds.  Each value should
	// be >= 0 and indicate the number of pixels to subtract from the
	// windows bounds to get the client bounds.
	void GetWindowAdjust(out int leftAdjust, out int topAdjust,
						 out int rightAdjust, out int bottomAdjust,
						 ToolkitWindowFlags flags);

	// Register a timer that should fire every "interval" milliseconds.
	// Returns a cookie that can be used to identify the timer.
	Object RegisterTimer(Object owner, int interval, EventHandler expire);

	// Unregister a timer.
	void UnregisterTimer(Object cookie);

}; // interface IToolkit

}; // namespace System.Drawing.Toolkit
