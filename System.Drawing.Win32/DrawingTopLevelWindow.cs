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
using d=System.Diagnostics.Debug;

internal class DrawingTopLevelWindow : DrawingWindow, IToolkitWindow
{

	protected static uint createCount;
	public DrawingTopLevelWindow(IToolkit toolkit, String name,
						 		 int width, int height) : base ( toolkit )
			{
				System.Diagnostics.Debug.WriteLine("DrawingTopLevelWindow");
				dimensions = new Rectangle(0, 0, width, height);

				//At the moment we create a unique class name for EVERY window. SWF does it for each unique window class
				className = "DrawingTopLevelWindow" + createCount;

				//Register the windows class
				windowsClass = new Win32.Api.WNDCLASS();
				windowsClass.style = Win32.Api.WindowClassStyle.CS_DBLCLKS;
				windowsClass.lpfnWndProc = new Win32.Api.WNDPROC(WindowsLoop);
				 //We will draw
				windowsClass.hbrBackground = IntPtr.Zero;
				windowsClass.lpszClassName = className ;
				if (Win32.Api.RegisterClassA( ref windowsClass)==0) 
				{
					throw new Exception("Failed to register Windows class " + className);
				}
				createCount++;
				
				//Set default window characteristics
				style = Win32.Api.WindowStyle.WS_THICKFRAME |
			Win32.Api.WindowStyle.WS_SYSMENU | Win32.Api.WindowStyle.WS_MAXIMIZEBOX | Win32.Api.WindowStyle.WS_MINIMIZEBOX | Win32.Api.WindowStyle.WS_CAPTION | Win32.Api.WindowStyle.WS_CLIPCHILDREN;
				menu = false;
				extendedStyle = 0;
				CreateWindow();
			}

	// Change the set of supported window decorations and functions.
	void IToolkitWindow.SetWindowFlags(ToolkitWindowFlags flags)
			{
				if (hwnd == IntPtr.Zero)
					throw new ApplicationException("DrawingTopLevelWindow.SetWindowsFlags ERROR: Cant SetWindowsFlags. Hwnd not created yet.");
		
				style = Win32.Api.WindowStyle.WS_POPUP | Win32.Api.WindowStyle.WS_VISIBLE;
		
				//to remove the popup style
				Win32.Api.WindowStyle overlapped = ~Win32.Api.WindowStyle.WS_POPUP;
				
				if((flags & ToolkitWindowFlags.Close) > 0)
					style |= Win32.Api.WindowStyle.WS_SYSMENU;

				if((flags & ToolkitWindowFlags.Minimize) > 0)
					style = style & overlapped | Win32.Api.WindowStyle.WS_MINIMIZEBOX;

				if((flags & ToolkitWindowFlags.Maximize) > 0)
					style = style & overlapped | Win32.Api.WindowStyle.WS_MAXIMIZEBOX;

				if((flags & ToolkitWindowFlags.Caption) > 0)
					style |= Win32.Api.WindowStyle.WS_CAPTION;

				if((flags & ToolkitWindowFlags.Border) > 0)
					style |= Win32.Api.WindowStyle.WS_BORDER;

				if((flags & ToolkitWindowFlags.ResizeHandles) > 0)
					style |= Win32.Api.WindowStyle.WS_THICKFRAME;

				if((flags & ToolkitWindowFlags.Menu) > 0)
					style = style | Win32.Api.WindowStyle.WS_SYSMENU;

				if((flags & ToolkitWindowFlags.Resize) > 0)
					style |= Win32.Api.WindowStyle.WS_THICKFRAME;

				//We dont handle the Move flag in Win32

				//TODO: NOT SURE HERE
				if((flags & ToolkitWindowFlags.Modal) > 0)
					style |= Win32.Api.WindowStyle.WS_POPUP | Win32.Api.WindowStyle.WS_DLGFRAME;

				//TODO: Need a hidden window
				if((flags & ToolkitWindowFlags.ShowInTaskBar)>0)
					;
				else
					;			

				if((flags & ToolkitWindowFlags.TopMost)>0)
					extendedStyle |= Win32.Api.WindowsExtendedStyle.WS_EX_TOPMOST;

				if((flags & ToolkitWindowFlags.ToolWindow) > 0)
					extendedStyle |= Win32.Api.WindowsExtendedStyle.WS_EX_TOOLWINDOW;		

				//Now set the style
				if (Win32.Api.SetWindowLong(hwnd, Win32.Api.SetWindowLongType.GWL_STYLE,style) == 0)
					throw new InvalidOperationException("Unable to change the window style.");
				if (Win32.Api.SetWindowLong(hwnd, Win32.Api.SetWindowLongType.GWL_EXSTYLE,extendedStyle) == 0)
					throw new InvalidOperationException("Unable to change the extended window style.");

				//Redraw the entire window including the non client portion
				Win32.Api.RedrawWindow( hwnd, IntPtr.Zero, IntPtr.Zero, Win32.Api.RedrawWindowFlags.RDW_INVALIDATE | Win32.Api.RedrawWindowFlags.RDW_FRAME );
				d.WriteLine( "DrawingTopLevelWindow.SetWindowFlags, hwnd="+hwnd );

			}

	//Use the style and extended style and menu thats currenly in use to get the actual size of the window with all the extra trimmings added
	protected override Rectangle OutsideFromClientSize(Rectangle client)
	{
		Win32.Api.RECT rect;
		rect.left = client.Left;
		rect.top = client.Top;
		rect.right = client.Right;
		rect.bottom = client.Bottom;
		Win32.Api.AdjustWindowRectEx( ref rect, style, menu, extendedStyle );
		return new Rectangle( rect.left, rect.top, rect.right-rect.left, rect.bottom-rect.top );
	}

	//Create the invisible control
	protected void CreateWindow() 
	{
		Size outside = OutsideFromClientSize( dimensions.Size );

		hwnd = Win32.Api.CreateWindowExA( extendedStyle, className, "", style, Win32.Api.CW_USEDEFAULT, 0, outside.Width, outside.Height, IntPtr.Zero, IntPtr.Zero, Win32.Api.GetModuleHandleA(null), IntPtr.Zero );
		if (hwnd==IntPtr.Zero) 
		{
			throw new Exception( "Failed to create new Window" );
		}
		setVisible();
		Win32.Api.InvalidateRect( hwnd, IntPtr.Zero,true );
		d.WriteLine( "DrawingTopLevelWindow.CreateWindow, hwnd="+hwnd+", [" + dimensions.Size.ToString() + "]" );
	}
}; // class DrawingTopLevelWindow

}; // namespace System.Drawing.Toolkit
