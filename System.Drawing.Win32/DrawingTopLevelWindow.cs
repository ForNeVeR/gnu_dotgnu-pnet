/*
 * DrawingTopLevelWindow.cs - This is a windows form
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

internal class DrawingTopLevelWindow : DrawingWindow, IToolkitWindow
{

	protected static uint createCount;
	public DrawingTopLevelWindow(DrawingToolkit toolkit, String name,
						 		 int width, int height, IToolkitEventSink sink) : base ( toolkit )
			{
				this.sink = sink;
				//Console.WriteLine("DrawingTopLevelWindow");
				dimensions = new Rectangle(0, 0, width, height);

				// At the moment we create a unique class name for EVERY window. SWF does it for each unique window class
				className = "DrawingTopLevelWindow" + createCount++;

				// Register the windows class
				windowsClass = new Win32.Api.WNDCLASS();
				windowsClass.style = Win32.Api.WindowClassStyle.CS_DBLCLKS;
				windowsClass.lpfnWndProc = new Win32.Api.WNDPROC(toolkit.WindowsLoop);
				// We will draw
				windowsClass.hbrBackground = IntPtr.Zero;
				windowsClass.lpszClassName = className ;
				if (Win32.Api.RegisterClassA( ref windowsClass)==0) 
				{
					throw new Exception("Failed to register Windows class " + className);
				}
				
				// Set default window characteristics
				flags = ToolkitWindowFlags.Menu | ToolkitWindowFlags.Close | ToolkitWindowFlags.Minimize | ToolkitWindowFlags.Maximize | ToolkitWindowFlags.Caption | ToolkitWindowFlags.ResizeHandles;
				toolkit.GetWin32StylesFromFlags( flags, out style, out extendedStyle);
				menu = false;
				extendedStyle = 0;
				topOfHeirarchy = this;
			}

	// Change the set of supported window decorations and functions.
	void IToolkitWindow.SetWindowFlags(ToolkitWindowFlags flags)
			{
				if (hwnd == IntPtr.Zero)
					throw new ApplicationException("DrawingTopLevelWindow.SetWindowsFlags ERROR: Cant SetWindowsFlags. Hwnd not created yet");
		
				Win32.Api.WindowStyle style;
				Win32.Api.WindowsExtendedStyle extendedStyle;
				
				toolkit.GetWin32StylesFromFlags( flags, out style, out extendedStyle);
				
				//Now set the style
				if (Win32.Api.SetWindowLong(hwnd, Win32.Api.SetWindowLongType.GWL_STYLE,style) == 0)
					throw new InvalidOperationException("Unable to change the window style");
				if (Win32.Api.SetWindowLong(hwnd, Win32.Api.SetWindowLongType.GWL_EXSTYLE,extendedStyle) == 0)
					throw new InvalidOperationException("Unable to change the extended window style");

				// Redraw the entire window including the non client portion
				Win32.Api.RedrawWindow( hwnd, IntPtr.Zero, IntPtr.Zero, Win32.Api.RedrawWindowFlags.RDW_INVALIDATE | Win32.Api.RedrawWindowFlags.RDW_FRAME );
				// Console.WriteLine( "DrawingTopLevelWindow.SetWindowFlags, " + sink );

			}

	internal override void SetFocus()
	{
		if (sink != null)
			sink.ToolkitPrimaryFocusEnter();
		//Console.WriteLine( "DrawingTopLevelWindow.GotFocus "+sink );
	}

	internal override void KillFocus()
	{
		if (sink != null)
			sink.ToolkitPrimaryFocusLeave();
		//Console.WriteLine( "DrawingTopLevelWindow.LostFocus "+sink ) ;
	}

	internal override void Close()
	{
		if(sink != null)
		{
			sink.ToolkitClose();
		}
	}


	//Create the invisible control
	internal override void CreateWindow() 
	{
		int leftAdjust, topAdjust, rightAdjust, bottomAdjust;
		Toolkit.GetWindowAdjust( out leftAdjust, out topAdjust, out rightAdjust, out bottomAdjust, flags);

		Size outside = new Size(dimensions.Width + leftAdjust + rightAdjust, dimensions.Height + topAdjust + bottomAdjust);

		hwnd = Win32.Api.CreateWindowExA( extendedStyle, className, "", style, Win32.Api.CW_USEDEFAULT, 0, outside.Width, outside.Height, IntPtr.Zero, IntPtr.Zero, Win32.Api.GetModuleHandleA(null), IntPtr.Zero );
		if (hwnd==IntPtr.Zero) 
		{
			throw new Exception("OS reported failure to create new Window");
		}
		dimensions = (this as IToolkitWindow).Dimensions;
		outsideDimensions = new Rectangle(dimensions.X - leftAdjust, dimensions.Y - topAdjust, dimensions.Width + leftAdjust + rightAdjust, dimensions.Height + topAdjust + bottomAdjust);
		sink.ToolkitExternalMove( dimensions.X, dimensions.Y );
		sink.ToolkitExternalResize( dimensions.Width, dimensions.Height );
		//Console.WriteLine( "DrawingTopLevelWindow.CreateWindow, "+sink+", [" + dimensions.Size + "]" );
	}
}; // class DrawingTopLevelWindow

}; // namespace System.Drawing.Toolkit
