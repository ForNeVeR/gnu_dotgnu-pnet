/*
 * DrawingToolkit.cs - Implementation of IToolkit for Win32.
 * Copyright (C) 2003  Neil Cawse.
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

using System;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Drawing.Toolkit;
using System.Drawing.Text;
using System.Drawing.Imaging;
using System.Collections;
using d = System.Diagnostics.Debug;

public class DrawingToolkit : IToolkit
{
	DrawingRootTopLevelWindow drawingRootTopLevelWindow;

	// Process events in the event queue.  If "waitForEvent" is true,
	// then wait for the next event and return "false" if "Quit" was
	// seen.  If "waitForEvent" is false, then process events in the
	// queue and return "true".  If "waitForEvent" is false and there
	// are no events in the queue, then return "false".
	public bool ProcessEvents(bool waitForEvent)
			{
				Win32.Api.MSG message;
				if (!waitForEvent)
				{
					//return false if there is no message or the quit message
					if (!Win32.Api.PeekMessageA(out message, IntPtr.Zero, 0, 0, Win32.Api.PeekMessageType.PM_NOREMOVE)
						|| message.message==Win32.Api.WindowsMessages.WM_QUIT)
						return false;
				}
				//process the message
				if (Win32.Api.GetMessageA(out message, IntPtr.Zero,0,0)==0)
					return false; //occurs on WM_QUIT
				Win32.Api.TranslateMessage(ref message);
				Win32.Api.DispatchMessageA(ref message);	
				return true;
			}

	// Send a quit message to the toolkit, which should cause
	// it to exit from the "Run" method.
	public void Quit()
			{
				Win32.Api.PostQuitMessage(0);
			}

	// Resolve a system color to an RGB value.  Returns -1 if the
	// system does not support the color and a default should be used.
	// TODO
	public int ResolveSystemColor(KnownColor color)
			{
				return -1;
			/*ActiveBorder			= 1,
	ActiveCaption			= 2,
	ActiveCaptionText		= 3,
	AppWorkspace			= 4,
	Control					= 5,
	ControlDark				= 6,
	ControlDarkDark			= 7,
	ControlLight			= 8,
	ControlLightLight		= 9,
	ControlText				= 10,
	Desktop					= 11,
	GrayText				= 12,
	Highlight				= 13,
	HighlightText			= 14,
	HotTrack				= 15,
	InactiveBorder			= 16,
	InactiveCaption			= 17,
	InactiveCaptionText		= 18,
	Info					= 19,
	InfoText				= 20,
	Menu					= 21,
	MenuText				= 22,
	ScrollBar				= 23,
	Window					= 24,
	WindowFrame				= 25,
	WindowText				= 26,*/
			}

	// Create an IToolkitGraphics object from a HDC.
	public IToolkitGraphics CreateFromHdc(IntPtr hdc, IntPtr hdevice)
			{
				// This is tricky - maybe we have to keep track of which hdc's we create?
				return null;
			}

	// Create an IToolkitGraphics object from a HWND.
	public IToolkitGraphics CreateFromHwnd(IntPtr hwnd)
			{
				return null;
			}

	// Create a solid toolkit brush.
	public IToolkitBrush CreateSolidBrush(System.Drawing.Color color)
			{
				return new DrawingSolidBrush(this, color);
			}

	// Create a hatched toolkit brush.
	public IToolkitBrush CreateHatchBrush
					(HatchStyle style, System.Drawing.Color foreColor,
					 System.Drawing.Color backColor)
			{
				return new DrawingHatchBrush(this, style, foreColor, backColor);
			}

	// Create a linear gradient brush.
	public IToolkitBrush CreateLinearGradientBrush
				(RectangleF rect, System.Drawing.Color color1,
				 System.Drawing.Color color2,
				 LinearGradientMode mode)
			{
				//TODO
				return null;
			}

	public IToolkitBrush CreateLinearGradientBrush
				(RectangleF rect, System.Drawing.Color color1,
				 System.Drawing.Color color2, float angle,
				 bool isAngleScaleable)
			{
				//TODO
				return null;
			}

	// Create a texture brush.
	public IToolkitBrush CreateTextureBrush
				(TextureBrush properties, RectangleF dstRect,
				 ImageAttributes imageAttr)
			{
				return new DrawingTextureBrush(this, properties, dstRect, imageAttr);
			}

	// Create a toolkit pen from pen properties.
	// If the toolkit does not support the precise combination of pen
	// properties, it will return the closest matching pen.
	public IToolkitPen CreatePen(Pen pen)
			{
				return new DrawingPen(this, pen);
			}

	// Create a toolkit font from the properties in the specified object.
	public IToolkitFont CreateFont(System.Drawing.Font font)
			{
				return new DrawingFont(this, font);
			}

	// Get the handle for the halftone palette.  IntPtr.Zero if not supported.
	//TODO
	public IntPtr GetHalftonePalette()
			{
				return IntPtr.Zero;
			}

	// Create a form.
	public IToolkitWindow CreateTopLevelWindow(int width, int height)
			{
				//for now..
				if ( drawingRootTopLevelWindow == null)
				{
					return drawingRootTopLevelWindow = new DrawingRootTopLevelWindow(this, string.Empty, width, height);
				}
					
				return new DrawingTopLevelWindow(this, string.Empty, width, height);
			}

	// Create a top-level dialog shell.
	//TODO
	public IToolkitWindow CreateTopLevelDialog
				(int width, int height, bool modal, bool resizable,
				 IToolkitWindow dialogParent)
			{
				DrawingTopLevelWindow window;
				window = new DrawingTopLevelWindow
					(this, String.Empty, width, height);
				/*if(dialogParent is TopLevelWindow)
				{
					window.TransientFor = (TopLevelWindow)dialogParent;
				}
				if(modal)
				{
					window.InputType = MotifInputType.ApplicationModal;
				}
				else
				{
					window.InputType = MotifInputType.Modeless;
				}
				if(!resizable)
				{
					window.Decorations = MotifDecorations.Border |
										 MotifDecorations.Title |
										 MotifDecorations.Menu;
					window.Functions = MotifFunctions.Move |
									   MotifFunctions.Close;
				}*/
				return window;
			}

	// Create a top-level menu shell.
	public IToolkitWindow CreateTopLevelMenu
				(int x, int y, int width, int height)
			{
				// TODO
				return null;
			}

	// Create a child window.  If "parent" is null, then the child
	// does not yet have a "real" parent - it will be reparented later.
	public IToolkitWindow CreateChildWindow
				(IToolkitWindow parent, int x, int y, int width, int height)
			{
				DrawingWindow dparent;
				if(parent is DrawingWindow)
				{
					dparent = ((DrawingWindow)parent);
				}
				else
				{
					dparent = null;
				}
				return new DrawingControlWindow(this, "", dparent, x, y, width, height);
			}

	// Get a list of all font families on this system, or all font
	// families that are compatible with a particular IToolkitGraphics.
	//TODO
	public FontFamily[] GetFontFamilies(IToolkitGraphics graphics)
			{
				// We only support three font families.  Extend later.
				return new FontFamily [] {
					new FontFamily("Arial"),
					new FontFamily("Times New Roman"),
					new FontFamily("Courier New"),
				};
			}

	// Get font family metric information.
	public void GetFontFamilyMetrics(GenericFontFamilies genericFamily,
							  		 String name,
									 System.Drawing.FontStyle style,
							  		 out int ascent, out int descent,
							  		 out int emHeight, out int lineSpacing)
			{
				//TODO
				switch(genericFamily)
				{
					case GenericFontFamilies.SansSerif:
					default:
					{
						// Metrics for "Arial".
						ascent = 1854;
						descent = 434;
						emHeight = 2048;
						lineSpacing = 2355;
					}
					break;

					case GenericFontFamilies.Serif:
					{
						// Metrics for "Times New Roman".
						ascent = 1825;
						descent = 443;
						emHeight = 2048;
						lineSpacing = 2355;
					}
					break;

					case GenericFontFamilies.Monospace:
					{
						// Metrics for "Courier New".
						ascent = 1705;
						descent = 615;
						emHeight = 2048;
						lineSpacing = 2320;
					}
					break;
				}
			}

	// Get the IToolkitFont that corresponds to a hdc's current font.
	// Returns null if there is no way to obtain the information.
	//TODO
	public IToolkitFont GetFontFromHdc(IntPtr hdc)
			{
				return null;
			}

	// Get the IToolkitFont that corresponds to a native font object.
	// Returns null if there is no way to obtain the information.
	//TODO
	public IToolkitFont GetFontFromHfont(IntPtr hfont)
			{
				return null;
			}

	// Get the IToolkitFont that corresponds to LOGFONT information.
	// Returns null if there is no way to obtain the information.
	//TODO
	public IToolkitFont GetFontFromLogFont(Object lf, IntPtr hdc)
			{
				return null;
			}

	// Get the default IToolkitGraphics object to measure screen sizes.
	public IToolkitGraphics GetDefaultGraphics()
			{
				d.WriteLine("DrawingToolkit.GetDefaultGraphics");
				return new DrawingGraphics(this, Win32.Api.GetDC(Win32.Api.GetDesktopWindow()));
			}

	// Get the screen size, in pixels.
	public Size GetScreenSize()
	{
		int x = Win32.Api.GetSystemMetrics(Win32.Api.SystemMetricsType.SM_CXSCREEN);
		int y = Win32.Api.GetSystemMetrics(Win32.Api.SystemMetricsType.SM_CYSCREEN);
		return new Size(x,y);
	}

	// Get the working area of the screen, excluding task bars, etc.
	public System.Drawing.Rectangle GetWorkingArea()
	{
		Win32.Api.RECT r;
		Win32.Api.SystemParametersInfoA(Win32.Api.SystemParametersAction.SPI_GETWORKAREA, 0, out r, 0);
		return new Rectangle(r.left,r.right,r.right-r.left,r.bottom-r.top);
	}

	// Get the adjustment values for a top-level window, to convert
	// between window bounds and client bounds.  Each value should
	// be >= 0 and indicate the number of pixels to subtract from the
	// windows bounds to get the client bounds.
	public void GetWindowAdjust(out int leftAdjust, out int topAdjust,
						        out int rightAdjust, out int bottomAdjust,
								ToolkitWindowFlags flags)
	{
		// TODO
		leftAdjust = 0;
		topAdjust = 0;
		rightAdjust = 0;
		bottomAdjust = 0;
	}

}; // class DrawingToolkit

}; // namespace System.Drawing.Toolkit
