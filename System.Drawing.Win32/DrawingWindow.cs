/*
 * DrawingWindow.cs - Abstract DrawingWindow class. This is each win32 window, controls and forms.
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
using System.Runtime.InteropServices;
using System.Collections;

internal abstract class DrawingWindow : IToolkitWindow
{
	protected DrawingToolkit toolkit;
	protected IToolkitEventSink sink;
	protected internal IntPtr hwnd;
	protected IntPtr hdc;
	protected internal DrawingWindow parent;
	// The window at the top of the heirarchy
	protected internal DrawingWindow topOfHeirarchy;
	private Color background;
	//cached when we create the window
	private IntPtr backgroundBrush = IntPtr.Zero;

	//Windows Class name
	protected string className;
	//This structure needs to be kept in memory
	protected Win32.Api.WNDCLASS windowsClass;
	//Used so we can tell the size of the non client window
	protected Win32.Api.WindowStyle style;
	protected Win32.Api.WindowsExtendedStyle extendedStyle;
	//Does the window have a menu
	protected bool menu = false;
	protected ToolkitWindowFlags flags = 0;

	// The dimensions of the control relative to parent
	protected Rectangle dimensions;
	// The outside (include OS decorations) dimensions
	protected Rectangle outsideDimensions;
	// Whether the control should be visible once its created
	protected bool visible;

	protected bool suspendExternalMoveResizeNotify;
	protected DrawingWindow( DrawingToolkit toolkit )
	{
		this.toolkit = toolkit;
	}

	// Set the window title (top-level windows only).
	void IToolkitWindow.SetTitle(String title)
	{
		if (hwnd == IntPtr.Zero)
			throw new ApplicationException("DrawingWindow.SetTitle ERROR:Cant set title. Hwnd not created yet");
		if(title == null)
		{
			title = String.Empty;
		}
		Win32.Api.SetWindowTextA(hwnd, title);
		//Console.WriteLine("DrawingWindow.SetTitle, " + sink);
	}

	void IToolkitWindow.Lower()
	{
		//if (hwnd == IntPtr.Zero)
		//	//Console.WriteLine("DrawingWindow.Lower ERROR: Cant lower window. Hwnd not created yet");
		{
			Win32.Api.SetWindowPos(hwnd, Win32.Api.SetWindowsPosPosition.HWND_BOTTOM, 0, 0, 0, 0, Win32.Api.SetWindowsPosFlags.SWP_NOMOVE | Win32.Api.SetWindowsPosFlags.SWP_NOSIZE);
			//Console.WriteLine("DrawingWindow.Lower, " + sink);
		}
	}

	void IToolkitWindow.Raise()
	{
		if (hwnd == IntPtr.Zero)
			throw new ApplicationException("DrawingWindow.Raise ERROR: Cant raise window. Hwnd not created yet");
		
		Win32.Api.SetWindowPos(hwnd, Win32.Api.SetWindowsPosPosition.HWND_TOP, 0, 0, 0, 0, Win32.Api.SetWindowsPosFlags.SWP_NOMOVE | Win32.Api.SetWindowsPosFlags.SWP_NOSIZE);
		//Console.WriteLine("DrawingWindow.Raise, " + sink);
	}

	// Move this window to below one of its siblings.
	// Is this really useful?
	void IToolkitWindow.MoveToBelow(IToolkitWindow sibling)
	{
	}

	// Move this window to above one of its siblings.
	void IToolkitWindow.MoveToAbove(IToolkitWindow sibling)
	{
		if (hwnd == IntPtr.Zero)
			throw new ApplicationException("DrawingWindow.MoveToAbove ERROR: Cant MoveToAbove. Hwnd not created yet");
		Win32.Api.SetWindowPos(hwnd, (sibling as DrawingWindow).hwnd, 0, 0, 0, 0, Win32.Api.SetWindowsPosFlags.SWP_NOMOVE | Win32.Api.SetWindowsPosFlags.SWP_NOSIZE);
		//Console.WriteLine("DrawingWindow.MoveToAbove, "+sink);
	}


	void IToolkitWindow.Reparent(IToolkitWindow parent, int x, int y)
	{
		throw new NotSupportedException("TopLevelWindow can't have a parent");
	}

	public IToolkit Toolkit
	{
		get
		{
			return toolkit;
		}
	}

	// Actual operating system dimensions relative to parent.
	Rectangle IToolkitWindow.Dimensions
	{
		get
		{
			if (hwnd==IntPtr.Zero)
				return dimensions;
			else
			{
				Win32.Api.RECT rect;
				Win32.Api.GetWindowRect(hwnd, out rect);
				System.Drawing.Rectangle r = System.Drawing.Rectangle.FromLTRB(rect.left, rect.top, rect.right, rect.bottom);
				if (parent != null)
				{
					Rectangle parentDimensions = (parent as IToolkitWindow).Dimensions;
					r.X -= parentDimensions.X;
					r.Y -= parentDimensions.Y;
				}
				else if (this is DrawingTopLevelWindow)
				{
					// Get dimensions excluding the window decorations
					int leftAdjust, topAdjust, rightAdjust, bottomAdjust;
					Toolkit.GetWindowAdjust(out leftAdjust, out topAdjust, out rightAdjust, out bottomAdjust, flags);
					r.X += leftAdjust;
					r.Y += topAdjust;
					r.Width -= leftAdjust + rightAdjust;
					r.Height -= topAdjust + bottomAdjust;
				}
				return r;
			}
		}
	}

	// Does the window have focus
	bool IToolkitWindow.Focused
	{
		get
		{
			return Win32.Api.GetFocus() == hwnd;	
		}
	}

	// Get or set the mouse capture on this window.
	bool IToolkitWindow.Capture
	{
		get
		{
			return toolkit.capturedWindow == this;
		}
		set
		{
			//Console.WriteLine("DrawingWindow.Capture, "+ value +", "+ sink);
			if (value)
				toolkit.capturedWindow  = this;
			else
				toolkit.capturedWindow  = null;
		}
	}

	//Set the focus to this window
	void IToolkitWindow.Focus()
	{
		//Console.WriteLine("DrawingWindow.Focus, "+ sink);
		Win32.Api.SetFocus(hwnd);
	}

	//Destroy window
	public virtual void Destroy()
	{
		// Call will ignore if hwnd = IntPtr.Zero
		Win32.Api.DestroyWindow(hwnd);
		//Console.WriteLine("DrawingWindow.Destroy, "+sink);
	}

	void IToolkitWindow.SetWindowFlags(System.Drawing.Toolkit.ToolkitWindowFlags flags)
	{
	}

	// Set the background of the window to a solid color.
	void IToolkitWindow.SetBackground(System.Drawing.Color color)
	{
		if (color != background)
		{
			background = color;
			//If there was already a background brush then delete it
			if (backgroundBrush!=IntPtr.Zero)
				Win32.Api.DeleteObject(backgroundBrush);
			//Create the new cached brush
			backgroundBrush = DrawingSolidBrush.CreateSolidBrush(background);
			//Console.WriteLine("DrawingControlWindow.SetBackground, hwnd="+hwnd);
		}
	}

	void IToolkitWindow.MoveResize(int x, int y, int width, int height)
	{
		dimensions = new Rectangle(x, y, width, height);
		if (hwnd != IntPtr.Zero)
		{
			int leftAdjust, topAdjust, rightAdjust, bottomAdjust;
			Toolkit.GetWindowAdjust(out leftAdjust, out topAdjust, out rightAdjust, out bottomAdjust, flags);
			outsideDimensions = new Rectangle( x - leftAdjust, y - topAdjust, width + leftAdjust + rightAdjust, height + topAdjust + bottomAdjust);
			Win32.Api.SetWindowPos(hwnd, Win32.Api.SetWindowsPosPosition.HWND_TOP, outsideDimensions.Left, outsideDimensions.Top, outsideDimensions.Width, outsideDimensions.Height, Win32.Api.SetWindowsPosFlags.SWP_NOSENDCHANGING);
		}
		//Console.WriteLine("DrawingWindow.MoveResize, " + sink +",["+x+","+y+","+width+","+height+"]");
	}

	// This is the windows visibility according to OS.
	public virtual bool IsMapped
	{
		get
		{
			if (hwnd == IntPtr.Zero)
				throw new ApplicationException("can not get visible state, control is not created yet");
			return Win32.Api.IsWindowVisible(hwnd);
		}
		set
		{
			visible = value;
			if (hwnd != IntPtr.Zero)
				setVisible();
			//Console.WriteLine("DrawingWindow.setIsMapped "+sink+",visible="+value);
		}
	}

	// This is actual visibility based on parent visiblity.
	private bool Visible
	{
		get
		{
			if (!visible)
				return false;
			else if (parent == null)
				return true;
			else
				return parent.Visible;
		}
	}

	// Force an update of all invalidated regions.
	void IToolkitWindow.Update()
	{
		if (hwnd == IntPtr.Zero)
			throw new ApplicationException("Can not update window, window not created yet");
		
		Win32.Api.UpdateWindow(hwnd);
		//Console.WriteLine("DrawingWindow.Update, hwnd="+hwnd);
	}

	// Deprecated
	void IToolkitWindow.SetForeground(Color color)
	{
	}

	void IToolkitWindow.Iconify()
	{
		if (hwnd == IntPtr.Zero)
			throw new ApplicationException("Can not minimize, window not created yet");
		
		Win32.Api.CloseWindow(hwnd);
		//Console.WriteLine("DrawingWindow.Iconify, "+sink);
	}


	// Get a toolkit graphics object for this window.
	IToolkitGraphics IToolkitWindow.GetGraphics()
	{
		if (hwnd == IntPtr.Zero)
			throw new ApplicationException("Can not get graphics object, window not created yet");
		
		//Console.WriteLine("DrawingWindow.GetGraphics, "+sink);
		return new DrawingGraphics (toolkit, Win32.Api.GetDC(hwnd));
	}

	// Get the toolkit parent window.
	IToolkitWindow IToolkitWindow.Parent
	{
		get
		{
			return (IToolkitWindow)parent;
		}
	}

	System.IntPtr IToolkitWindow.GetHwnd()
	{
		return hwnd;
	}

	// Invalidate a rectangle within this window.
	public void Invalidate(int x, int y, int width, int height)
	{
		if (hwnd == IntPtr.Zero)
			throw new ApplicationException("Can not Invalidate, window not created yet");
		
		Win32.Api.RECT r;
		r.left = x;
		r.top = y;
		//TODO: Check if its +1
		r.right = x + width + 1;
		r.bottom = y + height + 1;
		Win32.Api.InvalidateRect(hwnd, ref r, false);
		//Console.WriteLine("DrawingWindow.Invalidate, "+sink + " ["+x+","+y+","+width+","+height+"]");
	}

	// Invalidate this window.
	public void Invalidate()
	{
		if (hwnd != IntPtr.Zero)
			Win32.Api.InvalidateRect(hwnd, IntPtr.Zero, false);
	}

	// Called when Windows wants to erase the form background. Use the provided hdc
	internal void EraseBackground(IntPtr hdc) 
	{
		if (hwnd == IntPtr.Zero)
			throw new ApplicationException("Can not EraseBackground, window not created yet");
		
		if (backgroundBrush!=IntPtr.Zero) 
		{
			Win32.Api.RECT clientRectangle;
			Win32.Api.GetClientRect(hwnd, out clientRectangle);
			clientRectangle = new System.Drawing.Win32.Api.RECT(clientRectangle.left, clientRectangle.top, clientRectangle.right, clientRectangle.bottom);
			Win32.Api.FillRect(hdc, ref clientRectangle,backgroundBrush);
		}
	}

	// This occurs when a top level window (form) receives focus)
	internal void Activate(int wParam, int lParam)
	{
		if ((Win32.Api.ActivateState)(wParam & 0xFF) == Win32.Api.ActivateState.WA_INACTIVE)
			return;
		//Console.WriteLine("DrawingWindow.Activate, " + sink);
		// We must handle this ourselves
		//Win32.Api.SetCursor(Win32.Api.LoadCursorA(IntPtr.Zero, Win32.Api.CursorName.IDC_ARROW));
		//CaptureTopLevel = this;
	}

	private void CaptureTopLevel(DrawingWindow window)
	{
			if (window == null)
			{
				if (toolkit.capturedTopWindow != null)
				{
					//Console.WriteLine("CaptureTopLevel, Releasing " + toolkit.capturedTopWindow);
					Win32.Api.ReleaseCapture();
					toolkit.capturedTopWindow = null;
				}
			}
			else if (toolkit.capturedTopWindow != window.topOfHeirarchy)
			{
				Win32.Api.ReleaseCapture();
				Win32.Api.SetCapture(window.topOfHeirarchy.hwnd);
				//Console.WriteLine("CaptureTopLevel, Switching " + toolkit.capturedTopWindow + " to " + window.topOfHeirarchy);
				toolkit.capturedTopWindow = window.topOfHeirarchy;
			}
	}

	// Post Mouse Move to the window that needs it. Handle Mouse Enter and Leave
	internal void MouseMove(int msg, int wParam, int lParam) 
	{
		
		ToolkitMouseButtons buttons = MapToToolkitMouseButtons(wParam);
		ToolkitKeys keys = MapMouseToToolkitKeys(wParam);
		int x, y;
		
		DrawingWindow actual;
		DrawingWindow found = FindDeepestChild(lParam, out x, out y, out actual);
		if (toolkit.enteredWindow != actual)
		{
			if (toolkit.enteredWindow != null)
			{
				//Console.WriteLine("DrawingWindow.MouseLeave, " + toolkit.enteredWindow.sink);
				// Check to see if the event changes the capture state if so redo
				DrawingWindow prevCapturedWindow = toolkit.capturedWindow;
				toolkit.enteredWindow.sink.ToolkitMouseLeave();
				toolkit.enteredWindow = null;
				if (prevCapturedWindow != toolkit.capturedWindow)
				{
					MouseMove(msg, wParam, lParam);
					return;
				}
			}
			if (actual != null)
			{
				//Console.WriteLine("DrawingWindow.MouseEnter, " + actual.sink);
				toolkit.enteredWindow = actual;
				// Check to see if the event changes the capture state if so redo
				DrawingWindow prevCapturedWindow = toolkit.capturedWindow;
				actual.sink.ToolkitMouseEnter();
				if (prevCapturedWindow != toolkit.capturedWindow)
				{
					MouseMove(msg, wParam, lParam);
					return;
				}
			}
		}
		
		// If there are no child windows to receive the captured input then give it to the window doing the capturing
		if (toolkit.capturedWindow != null && found == null)
			found = toolkit.capturedWindow;

		// If this is the first time over a window, set the capture and try again
		if (toolkit.capturedTopWindow == null)
		{
			if (toolkit.capturedWindow != null)
				CaptureTopLevel(toolkit.capturedWindow);
			else
				CaptureTopLevel(this);
			MouseMove(msg, wParam, lParam);
			return;
		}
		

		Win32.Api.POINT mouse;
		mouse.x= MouseX(lParam) + DimensionsScreen.X;
		mouse.y = MouseY(lParam) + DimensionsScreen.Y;
		DrawingWindow actualWindow = toolkit.DrawingWindow(Win32.Api.WindowFromPoint(mouse));

		if (toolkit.capturedWindow != null)
			toolkit.capturedWindow.sink.ToolkitMouseMove(buttons, keys,0 ,x ,y, 0);
		else if (actualWindow != null && actualWindow.topOfHeirarchy == toolkit.capturedTopWindow)
		{
			// On the window decorations.
			if (found == null)
				CaptureTopLevel(null);
			else
				// Send the message to the client window
				found.sink.ToolkitMouseMove(buttons, keys,0 ,x ,y, 0);
		}
		else
		{
			CaptureTopLevel(null);
			// ?Send the last mouse move beyond the window (Win NT+ behaviour)
			//found.sink.ToolkitMouseMove(buttons, keys,0 ,x ,y, 0);
			/*if (actualWindow != null)
			{
				MouseMove(msg, wParam, lParam);
			}*/
			// We should replay this mouse move
			/*
			// Convert this windows coordinates to the new top windows coordinates.
			int x1 = MouseX(lParam) + DimensionsScreen.X - top.dimensions.X;
			int y1 = MouseY(lParam) + DimensionsScreen.Y - top.dimensions.Y;
			// Now repost this message to the top level window
			lParam = x1 | y1 << 16;

			// Send the message to the client window
			Win32.Api.PostMessageA(hwnd, (Win32.Api.WindowsMessages)msg, wParam, lParam);
			return;
			*/
		}
		
	}
	
	// Returns the dimensions relative to the screen.
	private Rectangle DimensionsScreen
	{
		get
		{
			Rectangle r = new Rectangle(0, 0, dimensions.Width, dimensions.Height);
			for (DrawingWindow w = this; w != null; w = w.parent)
				r.Offset(w.dimensions.Location);
			return r;
		}
	}

	// Called when windows receives WM_MOUSEWHEEL
	internal void MouseWheel( int msg, int wParam, int lParam)
	{
		int wheelDelta = (wParam >> 16)/120;
		int x, y;
		DrawingWindow found = FindDeepestChild(lParam, out x, out y);

		if (found != null)
		{
			found.sink.ToolkitMouseWheel(ToolkitMouseButtons.None, MapMouseToToolkitKeys(wParam), 0, x , y, wheelDelta);
			//Console.WriteLine("DrawingWindow.MouseWheel " + found.sink + " [" + x + "," + y + "], key:" + MapMouseToToolkitKeys(wParam) + ", wheel:" + wheelDelta);
		}
	}

	protected virtual internal void ButtonDown(int msg, int wParam, int lParam)
	{
		int x,y;
		
		DrawingWindow found = FindDeepestChild(lParam, out x, out y);

		if (found != null)
		{
			found.sink.ToolkitMouseDown(MapToToolkitMouseButtons(wParam), MapMouseToToolkitKeys(wParam), 1 ,x ,y ,0);
			//Console.WriteLine("DrawingWindow.ButtonDown " + found.sink + " [" + x + "," + y + "], key:" + MapMouseToToolkitKeys(wParam) + ", button:" + MapToToolkitMouseButtons(wParam));
		}
	}

	internal void ButtonUp(int msg, int wParam, int lParam)
	{
		int x,y;
		DrawingWindow found = FindDeepestChild(lParam, out x, out y);

		if (found != null)
		{
			found.sink.ToolkitMouseUp(MapToToolkitMouseButtons(wParam), MapMouseToToolkitKeys(wParam), 1 ,x ,y ,0);
			//Console.WriteLine("DrawingWindow.ButtonUp " + found.sink + " [" + x + "," + y + "], key:" + MapMouseToToolkitKeys(wParam) + ", button:" + MapToToolkitMouseButtons(wParam));
		}
	}

	internal void DoubleClick( int msg, int wParam, int lParam)
	{
		int x,y;
		DrawingWindow found = FindDeepestChild(lParam, out x, out y);

		if (found != null)
		{
			found.sink.ToolkitMouseDown(MapToToolkitMouseButtons(wParam), MapMouseToToolkitKeys(wParam), 2 ,x ,y ,0);
			//Console.WriteLine("DrawingWindow.DoubleClick " + found.sink + ",[" + x + "," + y + "], key:" + MapMouseToToolkitKeys(wParam) + ", button:" + MapToToolkitMouseButtons(wParam));
		}
	}

	// Returns the bottom child window that is underneath x, y and has a top level window of capturedTopWindow or capturedWindow (if some window has captured the most). The window must be visible and toplevelwindows decorations are excluded. Returns x and y relative to the found windows coordinates.
	private DrawingWindow FindDeepestChild(int lParam, out int x, out int y, out DrawingWindow actualFound)
	{
		// Convert the mouse to screen coordinates
		Point pt = DimensionsScreen.Location;
		x = MouseX(lParam) + pt.X;
		y = MouseY(lParam) + pt.Y;

		DrawingWindow found = null;

		// Child windows lower down the heirachy will always be last in the windows list
		for (int i = 0; i < toolkit.windows.Count; i++)
		{
			DrawingWindow w = toolkit.windows[i] as DrawingWindow;
			if (toolkit.capturedWindow != null)
			{
				if (w.Visible && toolkit.capturedWindow.IsParentOf(w) && w.DimensionsScreen.Contains(x,y))
					found = w;
			}
			else if (w.Visible && toolkit.capturedTopWindow == w.topOfHeirarchy)
			{
				// This will occur when the mouse is over the titlebar of a toplevel window.
				if (w is DrawingTopLevelWindow && w.outsideDimensions.Contains(x,y) && !w.dimensions.Contains(x, y))
				{
					actualFound = null;
					return null;
				}
				else if (w.DimensionsScreen.Contains(x, y))
					found = w;
			}
		}

		actualFound = found;
		// If there are no child windows to receive the captured input then give it to the window doing the capturing
		if (toolkit.capturedWindow != null && found == null)
			found = toolkit.capturedWindow;
	
		if (found != null)
		{
			// Adjust the coordinates relative to the "found" window.
			Rectangle child;
			if (toolkit.capturedWindow == null)
				child = found.DimensionsScreen;
			else
				child = toolkit.capturedWindow.DimensionsScreen;
			x -= child.X;
			y -= child.Y;
		}

		return found;
	}

	private DrawingWindow FindDeepestChild(int lParam, out int x, out int y)
	{
		DrawingWindow actual;
		return FindDeepestChild(lParam, out x, out y, out actual);
	}

	internal void KeyDown( int wParam, int lParam)
	{
		sink.ToolkitKeyDown(MapKeyToToolkitKeys( wParam));
		//Console.WriteLine("DrawingWindow.KeyDown " + sink +", " + (MapKeyToToolkitKeys( wParam)).ToString() + " " + wParam);
	}

	internal void Char( int wParam, int lParam)
	{
		sink.ToolkitKeyChar((char)wParam);
		//Console.WriteLine("DrawingWindow.Char " + sink +", "+ ((char)wParam).ToString());
	}
	
	internal void KeyUp( int wParam, int lParam )
	{
		sink.ToolkitKeyUp(MapKeyToToolkitKeys( wParam));
		//Console.WriteLine("DrawingWindow.KeyUp " + sink +", "+ (MapKeyToToolkitKeys( wParam)).ToString());
	}

	//TODO:
	internal void SetCursor( int cursor )
	{
		Win32.Api.SetCursor(Win32.Api.LoadCursorA(IntPtr.Zero, Win32.Api.CursorName.IDC_ARROW));
	}

	// WM_PAINT Message
	internal void Paint()
	{
		Win32.Api.PAINTSTRUCT myPS = new System.Drawing.Win32.Api.PAINTSTRUCT();
		hdc = Win32.Api.BeginPaint( hwnd, ref myPS );
		if( sink != null )
		{
			DrawingGraphics g = new DrawingGraphics( toolkit, hdc );
			System.Drawing.Graphics gr = ToolkitManager.CreateGraphics( g );
			gr.SetClip(Rectangle.FromLTRB(myPS.rcPaintLeft, myPS.rcPaintTop, myPS.rcPaintRight, myPS.rcPaintBottom));
			sink.ToolkitExpose( gr );
			gr.Dispose();
		}
		//Console.WriteLine( "DrawingWindow.Paint "+ sink);

		Win32.Api.EndPaint( hwnd, ref myPS );
	}

	//WM_SETFOCUS occurs when either mouse or keyboard sets focus
	internal virtual void SetFocus()
	{
		if (sink != null)
			sink.ToolkitFocusEnter();
		//Console.WriteLine( "DrawingWindow.GotFocus "+sink);
	}

	//WM_KILLFOCUS occurs when either mouse or keyboard causes focus to be lost (or windows does)
	internal virtual void KillFocus()
	{
		if (sink != null)
			sink.ToolkitFocusLeave();
		//Console.WriteLine( "DrawingWindow.LostFocus "+sink) ;
	}

	//WM_WINDOWPOSCHANGING
	internal void WindowPosChanging(int lParam)
	{
		//When window is created - CreateWindow(), WindowPosChanging is called when the initial size is set
		//because sink==null, its size and position will be set
		if (sink != null)
		{
			//TODO Pointers are not available yet
			//Win32.Api.WINDOWPOS *pos = (Win32.Api.WINDOWPOS*)lParam;
			Win32.Api.WINDOWPOS pos = (Win32.Api.WINDOWPOS)Marshal.PtrToStructure(new IntPtr(lParam), typeof(Win32.Api.WINDOWPOS));
			if (suspendExternalMoveResizeNotify == true)
				return;
			// If moving
			if (((Win32.Api.SetWindowsPosFlags)pos.flags & Win32.Api.SetWindowsPosFlags.SWP_NOMOVE) == 0)
			{
				int leftAdjust, topAdjust, rightAdjust, bottomAdjust;
				Toolkit.GetWindowAdjust(out leftAdjust, out topAdjust, out rightAdjust, out bottomAdjust, flags);
				sink.ToolkitExternalMove(pos.x + leftAdjust, pos.y + topAdjust );
			}

			// If sizing
			if (((Win32.Api.SetWindowsPosFlags)pos.flags & Win32.Api.SetWindowsPosFlags.SWP_NOSIZE) ==0)
			{
				int leftAdjust, topAdjust, rightAdjust, bottomAdjust;
				Toolkit.GetWindowAdjust(out leftAdjust, out topAdjust, out rightAdjust, out bottomAdjust, flags);
				sink.ToolkitExternalResize( pos.cx - leftAdjust - rightAdjust, pos.cy - topAdjust - bottomAdjust );
			}

			// Now prevent windows from changing the position or size, System.Windows.Control will do that
			
			//TODO Pointers are not available yet
			//pos.flags |= (uint)0x3;
			Marshal.WriteInt32(new IntPtr(lParam), 24, (int)pos.flags | 0x3);

		}
	}

	//WM_SETTINGSCHANGE occurs when some windows setting changes. This is used to notify the app that system settings have changed eg. button colors or form colors
	//We only want the RootTopLevelWindow to receive this
	internal virtual void SettingsChange(int wParam)
	{
	}

	internal virtual void Close()
	{
	}

	//Extract the mouse positions
	protected short MouseX( int value )
	{
		return (short) (0xFFFF & value);
	}

	protected short MouseY( int value )
	{
		return (short) (value >> 16);
	}

	//Map the win32 MouseKeyState to ToolkitMouseButtons
	private ToolkitMouseButtons MapToToolkitMouseButtons( int wParam )
	{
		ToolkitMouseButtons buttons = ToolkitMouseButtons.None; 
		Win32.Api.MouseKeyState fwKeys = (Win32.Api.MouseKeyState) (wParam & 0xFFFF);

		if ((fwKeys & Win32.Api.MouseKeyState.MK_LBUTTON)>0)
			buttons |= ToolkitMouseButtons.Left;

		if ((fwKeys & Win32.Api.MouseKeyState.MK_MBUTTON)>0)
			buttons |= ToolkitMouseButtons.Middle;

		if ((fwKeys & Win32.Api.MouseKeyState.MK_RBUTTON)>0)
			buttons |= ToolkitMouseButtons.Right;

		if ((fwKeys & Win32.Api.MouseKeyState.MK_XBUTTON1)>0)
			buttons |= ToolkitMouseButtons.XButton1;

		if ((fwKeys & Win32.Api.MouseKeyState.MK_XBUTTON2)>0)
			buttons |= ToolkitMouseButtons.XButton2;

		return buttons;
	}

	//Map the win32 MouseKeyState to ToolkitKeys. Alt is handled differently
	private ToolkitKeys MapMouseToToolkitKeys( int wParam )
	{
		ToolkitKeys keys = ToolkitKeys.None;
		Win32.Api.MouseKeyState fwKeys = (Win32.Api.MouseKeyState) (wParam & 0xFFFF);
		if ((fwKeys & Win32.Api.MouseKeyState.MK_CONTROL)>0)
			keys |= ToolkitKeys.Control;

		if ((fwKeys & Win32.Api.MouseKeyState.MK_SHIFT)>0)
			keys |= ToolkitKeys.Shift;

		if (Win32.Api.GetKeyState(Win32.Api.VirtualKeyType.VK_MENU) < 0)
			keys |= ToolkitKeys.Alt;
		
		return keys;

	}

	private ToolkitKeys MapKeyToToolkitKeys( int wParam)
	{
		ToolkitKeys key =  (ToolkitKeys)(wParam & 0xFFFF);
		if (Win32.Api.GetKeyState(Win32.Api.VirtualKeyType.VK_MENU) < 0)
			key |= ToolkitKeys.Alt;
		if (Win32.Api.GetKeyState(Win32.Api.VirtualKeyType.VK_CONTROL) < 0)
			key |= ToolkitKeys.Control;
		if (Win32.Api.GetKeyState(Win32.Api.VirtualKeyType.VK_SHIFT) < 0)
			key |= ToolkitKeys.Shift;
		return key;
	}

	// This is called to set whether a window is visible or not. Can only happen once the window is created.
	protected void setVisible()
	{
		if (visible)
			Win32.Api.ShowWindow(hwnd,Win32.Api.ShowWindowCommand.SW_SHOWNA);
		else
			Win32.Api.ShowWindow(hwnd,Win32.Api.ShowWindowCommand.SW_HIDE);
	}

	abstract internal void CreateWindow();

	// Make sure the parent window is before the child in the heirarchy
	protected void AdjustWindows(DrawingWindow parent)
	{
		// Find all children windows and set their new top of heirarchy
		DrawingWindow newTopOfHeirarchy = parent == null ? this : (parent as DrawingWindow).topOfHeirarchy;
		foreach(DrawingWindow w in toolkit.windows)
			if (this.IsParentOf(w))
				w.topOfHeirarchy = newTopOfHeirarchy;
		this.parent = parent as DrawingWindow;
		// Make sure the parent window is before the child in the heirarchy
		ArrayList windows = toolkit.windows;
		int iChild = windows.IndexOf(this);
		for(DrawingWindow w = parent; w != null; w = w.parent)
		{
			int pos = windows.IndexOf(w);
			if (pos > iChild)
			{
				windows.Remove(w);
				windows.Insert(iChild,w);
			}
			else
				break;
		}
	}

	// Returns whether a DrawingWindow is the parent of this window or is the window
	protected bool IsParentOf(DrawingWindow window)
	{
		for (DrawingWindow w = window; w != null; w = w.parent)
			if (w == this)
				return true;
		return false;
	}

}
}
