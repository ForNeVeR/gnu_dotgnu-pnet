/*
 * DrawingControlWindow.cs - Implementation of the control, inherits from drawing window.
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
using d = System.Diagnostics.Debug;

internal class DrawingControlWindow : DrawingWindow, IToolkitWindow
{
	//for a unique class name
	protected static uint createCount;

	public DrawingControlWindow(IToolkit toolkit, string name, DrawingWindow parent, int x,
		int y, int width, int height) : base (toolkit, parent)
	{
		d.WriteLine("DrawingControlWindow");

		//At the moment we create a unique class name for EVERY window. SWF does it for each window type
		className = "DrawingControlWindow" + createCount;
		//Register the windows class
		windowsClass = new Win32.Api.WNDCLASS();
		windowsClass.style = Win32.Api.WindowClassStyle.CS_DBLCLKS;
		windowsClass.lpfnWndProc = new Win32.Api.WNDPROC(WindowsLoop);
		windowsClass.hbrBackground = IntPtr.Zero; //(IntPtr)(Win32.Api.COLOR_WINDOW + 1);
		windowsClass.lpszClassName = className ;
		if (Win32.Api.RegisterClassA( ref windowsClass)==0) 
		{
			throw new Exception("Failed to register Windows class " + className);
		}
		createCount++;
		//Set default windows settings
		style = Win32.Api.WindowStyle.WS_CHILD;
		extendedStyle = 0;
		if (parent != null)
		{
			CreateWindow( new Rectangle( x, y, width, height ) );
		}
	}

	// Reparent this window to underneath a new parent.
	// If there is no parent then we destroy the window.
	// If the parent changes, we destroy and recreate
	void IToolkitWindow.Reparent(IToolkitWindow parent, int x, int y)
	{
		if (parent != this.parent)
		{
			if(this.parent != null || parent == null )
			{
				(this as IToolkitWindow).Destroy();
			}
			if (parent != null)
			{
				Rectangle dimension = (this as IToolkitWindow).Dimensions;
				CreateWindow( new Rectangle( x, y, dimension.Width, dimension.Height )) ;
			}
		}
		d.WriteLine("DrawingWindow.Reparent, hwnd="+hwnd);
	}

	
	protected void CreateWindow(Rectangle bounds)
	{
		hwnd = Win32.Api.CreateWindowExA( extendedStyle, className, string.Empty, style, bounds.X, bounds.Y, bounds.Width, bounds.Height, parent.hwnd, IntPtr.Zero,Win32.Api.GetModuleHandleA(null),IntPtr.Zero );
		d.WriteLine("DrawingControlWindow.CreateWindow hwnd="+hwnd + ",parent hwnd="+parent.hwnd+",["+bounds.ToString()+"]");
		if (hwnd==IntPtr.Zero) 
		{
			throw new Exception("Failed to create new Window");
		}
	}


}; // class DrawingControlWindow

}; // namespace System.Drawing.Toolkit
