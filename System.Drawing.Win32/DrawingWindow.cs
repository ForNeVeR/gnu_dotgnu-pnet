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

internal abstract class DrawingWindow : IToolkitWindow
{
	protected IToolkit toolkit;
	protected IToolkitEventSink sink;
	protected internal IntPtr hwnd;
	protected IntPtr hdc;
	protected DrawingWindow parent;
	private Color background;
	//cached when we create the window
	private IntPtr backgroundBrush = IntPtr.Zero;
	//have we told windows that we want to be notified when the mouse leaves?
	private bool trackingMouse;

	//Windows Class name
	protected string className;
	//This structure needs to be kept in memory
	protected Win32.Api.WNDCLASS windowsClass;
	//Used so we can tell the size of the non client window
	protected Win32.Api.WindowStyle style;
	//Used so we can tell the size of the non client window
	protected Win32.Api.WindowsExtendedStyle extendedStyle;
	//Used so we can tell the size of the non client window
	//Does the window have a menu
	protected bool menu = false;
	protected ToolkitWindowFlags flags = 0;

	//The dimensions before the control has been created
	protected Rectangle dimensions;
	//Whether the control should be visible once its created
	protected bool visible;

	protected bool suspendExternalMoveResizeNotify;
	protected DrawingWindow( IToolkit toolkit )
	{
		this.toolkit = toolkit;
	}

	// Set the window title (top-level windows only).
	void IToolkitWindow.SetTitle(String title)
	{
		if (hwnd == IntPtr.Zero)
			throw new ApplicationException("DrawingWindow.SetTitle ERROR:Cant set title. Hwnd not created yet.");
		if(title == null)
		{
			title = String.Empty;
		}
		Win32.Api.SetWindowTextA(hwnd, title);
		//Console.WriteLine("DrawingWindow.SetTitle, hwnd="+hwnd);
	}

	void IToolkitWindow.Lower()
	{
		if (hwnd == IntPtr.Zero)
			;//Console.WriteLine("DrawingWindow.Lower ERROR: Cant lower window. Hwnd not created yet.");
		else
		{
			Win32.Api.SetWindowPos(hwnd, Win32.Api.SetWindowsPosPosition.HWND_BOTTOM, 0, 0, 0, 0, Win32.Api.SetWindowsPosFlags.SWP_NOMOVE | Win32.Api.SetWindowsPosFlags.SWP_NOSIZE);
			//Console.WriteLine("DrawingWindow.Lower, hwnd="+hwnd);
		}
	}

	void IToolkitWindow.Raise()
	{
		if (hwnd == IntPtr.Zero)
			throw new ApplicationException("DrawingWindow.Raise ERROR: Cant raise window. Hwnd not created yet.");
		
		Win32.Api.SetWindowPos(hwnd, Win32.Api.SetWindowsPosPosition.HWND_TOP, 0, 0, 0, 0, Win32.Api.SetWindowsPosFlags.SWP_NOMOVE | Win32.Api.SetWindowsPosFlags.SWP_NOSIZE);
		//Console.WriteLine("DrawingWindow.Raise, hwnd="+hwnd);
	}

	// Move this window to below one of its siblings.
	// Is this really useful?
	void IToolkitWindow.MoveToBelow(IToolkitWindow sibling)
	{
		throw new NotImplementedException();
	}

	// Move this window to above one of its siblings.
	void IToolkitWindow.MoveToAbove(IToolkitWindow sibling)
	{
		if (hwnd == IntPtr.Zero)
			throw new ApplicationException("DrawingWindow.MoveToAbove ERROR: Cant MoveToAbove. Hwnd not created yet.");
		Win32.Api.SetWindowPos(hwnd, (sibling as DrawingWindow).hwnd, 0, 0, 0, 0, Win32.Api.SetWindowsPosFlags.SWP_NOMOVE | Win32.Api.SetWindowsPosFlags.SWP_NOSIZE);
		//Console.WriteLine("DrawingWindow.MoveToAbove, hwnd="+hwnd);
	}


	void IToolkitWindow.Reparent(IToolkitWindow parent, int x, int y)
	{
	}

	public IToolkit Toolkit
	{
		get
		{
			return toolkit;
		}
	}

	// Dimensions relative to parent
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
					r.Offset((parent as IToolkitWindow).Dimensions.Location);
				// Get dimensions excluding the window decorations
				int leftAdjust, topAdjust, rightAdjust, bottomAdjust;
				Toolkit.GetWindowAdjust(out leftAdjust, out topAdjust, out rightAdjust, out bottomAdjust, flags);
				r.X += leftAdjust;
				r.Y += topAdjust;
				r.Width -= leftAdjust + rightAdjust;
				r.Height -= topAdjust - bottomAdjust;
				return r;
			}
		}
	}

	//Does the window have focus
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
			return Win32.Api.GetCapture() == hwnd;
		}
		set
		{
			//Console.WriteLine("DrawingWindow.Capture:"+ hwnd + "=" + value);
			if (value)
				Win32.Api.SetCapture(hwnd);
			else
				Win32.Api.ReleaseCapture();
		}
	}

	//Set the focus to this window
	void IToolkitWindow.Focus()
	{
		//Console.WriteLine("DrawingWindow.Focus:"+ hwnd);
		Win32.Api.SetFocus(hwnd);
	}

	//Destroy window
	public virtual void Destroy()
	{
		//Call will ignore if hwnd = IntPtr.Zero
		Win32.Api.DestroyWindow(hwnd);
		//Console.WriteLine("DrawingWindow.Destroy, hwnd="+hwnd);
	}

	//After the WM_DESTORY message
	internal virtual void Destroyed()
	{
		//Console.WriteLine("DrawingWindow.Destroyed, hwnd="+hwnd);
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
			Rectangle rect = (this as IToolkitWindow).Dimensions;
	
			if (x != rect.Left || y != rect.Top || width != rect.Width
				|| height != rect.Height)
			{
				int leftAdjust, topAdjust, rightAdjust, bottomAdjust;
				Toolkit.GetWindowAdjust(out leftAdjust, out topAdjust, out rightAdjust, out bottomAdjust, flags);
				Rectangle outside = new Rectangle( x - leftAdjust, y - topAdjust, width + leftAdjust + rightAdjust, height + topAdjust + bottomAdjust);
				Win32.Api.SetWindowPos(hwnd, Win32.Api.SetWindowsPosPosition.HWND_TOP, outside.Left, outside.Top, outside.Width, outside.Height, Win32.Api.SetWindowsPosFlags.SWP_NOSENDCHANGING);
			}
		}
		//Console.WriteLine("DrawingWindow.MoveResize, hwnd="+hwnd+",["+x+","+y+","+width+","+height+"]");
	}

	//This is the windows visibility
	public virtual bool IsMapped
	{
		get
		{
			if (hwnd == IntPtr.Zero)
				throw new ApplicationException("DrawingWindow.getIsMapped ERROR: Cant getIsMapped. Hwnd not created yet.");
		
			return Win32.Api.IsWindowVisible(hwnd);
		}
		set
		{
			visible = value;
			if (hwnd != IntPtr.Zero)
				setVisible();
			//Console.WriteLine("DrawingWindow.setIsMapped hwnd="+hwnd+",visible="+value);
		}
	}

	// Force an update of all invalidated regions.
	void IToolkitWindow.Update()
	{
		if (hwnd == IntPtr.Zero)
			throw new ApplicationException("DrawingWindow.Update ERROR: Cant update. Hwnd not created yet.");
		
		Win32.Api.UpdateWindow(hwnd);
		//Console.WriteLine("DrawingWindow.Update, hwnd="+hwnd);
	}

	void IToolkitWindow.SetForeground(Color color)
	{
	}

	void IToolkitWindow.Iconify()
	{
		if (hwnd == IntPtr.Zero)
			throw new ApplicationException("DrawingWindow.Iconify ERROR: Cant Iconify. Hwnd not created yet.");
		
		Win32.Api.CloseWindow(hwnd);
		//Console.WriteLine("DrawingWindow.Iconify, hwnd="+hwnd);
	}


	// Get a toolkit graphics object for this window.
	IToolkitGraphics IToolkitWindow.GetGraphics()
	{
		if (hwnd == IntPtr.Zero)
			throw new ApplicationException("DrawingWindow.GetGraphics ERROR: Cant GetGraphics. Hwnd not created yet.");
		
		//Console.WriteLine("DrawingWindow.GetGraphics, hwnd="+hwnd);
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
			throw new ApplicationException("DrawingWindow.Invalidate ERROR: Cant Invalidate. Hwnd not created yet.");
		
		Win32.Api.RECT r;
		r.left = x;
		r.top = y;
		//TODO: Check if its +1
		r.right = x + width + 1;
		r.bottom = y + height + 1;
		Win32.Api.InvalidateRect(hwnd, ref r, false);
		//Console.WriteLine("DrawingWindow.Invalidate, hwnd="+hwnd + " ["+x+","+y+","+width+","+height+"]");
	}

	// Invalidate this window.
	public void Invalidate()
	{
		if (hwnd != IntPtr.Zero)
			Win32.Api.InvalidateRect(hwnd, IntPtr.Zero, false);
	}

	//Called when Windows wants to erase the form background. Use the provided hdc
	internal void EraseBackground(IntPtr hdc) 
	{
		if (hwnd == IntPtr.Zero)
			throw new ApplicationException("DrawingWindow.EraseBackground ERROR: Cant EraseBackground. Hwnd not created yet.");
		
		if (backgroundBrush!=IntPtr.Zero) 
		{
			Win32.Api.RECT clientRectangle;
			Win32.Api.GetClientRect(hwnd, out clientRectangle);
			clientRectangle = new System.Drawing.Win32.Api.RECT(clientRectangle.left, clientRectangle.top, clientRectangle.right, clientRectangle.bottom);
			Win32.Api.FillRect(hdc, ref clientRectangle,backgroundBrush);
		}
	}

	//Called when windows receives WM_MOUSEMOVE
	internal void MouseMove(int wParam, int lParam) 
	{
		if (!trackingMouse) 
		{
			//so we get notified when the mouse leaves this window
			Win32.Api.TRACKMOUSEEVENT tm = new Win32.Api.TRACKMOUSEEVENT();
			tm.hwndTrack = hwnd;
			tm.dwFlags = Win32.Api.TrackMouseEventFlags.TME_LEAVE;
			tm.cbSize = Marshal.SizeOf(tm);
			Win32.Api.TrackMouseEvent(ref tm);
			trackingMouse = true;
			sink.ToolkitMouseEnter();
		}

		sink.ToolkitMouseMove(MapToToolkitMouseButtons(wParam), MapMouseToToolkitKeys(wParam),0 ,MouseX(lParam) , MouseY(lParam), 0);
		//Console.WriteLine("DrawingWindow.MouseMove [" + (MouseX(lParam)) + "," + (MouseY(lParam)) + "], key:" + MapMouseToToolkitKeys(wParam));
	}

	//Called when windows receives WM_MOUSEWHEEL
	internal void MouseWheel( int wParam, int lParam)
	{
		int wheelDelta = (wParam >> 16)/120;
		sink.ToolkitMouseWheel(ToolkitMouseButtons.None, MapMouseToToolkitKeys(wParam), 0, MouseX(lParam) , MouseY(lParam), wheelDelta);
		//Console.WriteLine("DrawingWindow.MouseWheel [" + (MouseX(lParam)) + "," + (MouseY(lParam)) + "], key:" + MapMouseToToolkitKeys(wParam) + ", wheel:" + wheelDelta);
	}

	internal void MouseLeave() 
	{
		trackingMouse = false;
		sink.ToolkitMouseLeave();
		//Console.WriteLine("DrawingWindow.MouseLeave, hwnd="+hwnd);
	}

	internal void ButtonDown(int wParam, int lParam)
	{
		//Console.WriteLine("DrawingWindow.ButtonDown [" + (MouseX(lParam)) + "," + (MouseY(lParam)) + "], hwnd:" + hwnd +", key:" + MapMouseToToolkitKeys(wParam) + ", button:" + MapToToolkitMouseButtons(wParam));
		sink.ToolkitMouseDown(MapToToolkitMouseButtons(wParam), MapMouseToToolkitKeys(wParam), 1 ,MouseX(lParam), MouseY(lParam) ,0);
	}

	internal void ButtonUp(int wParam, int lParam)
	{
		sink.ToolkitMouseUp(MapToToolkitMouseButtons(wParam), MapMouseToToolkitKeys(wParam), 1 ,MouseX(lParam), MouseY(lParam),0);
		//Console.WriteLine("DrawingWindow.ButtonUp [" + (MouseX(lParam)) + "," + (MouseY(lParam)) + "], key:" + MapMouseToToolkitKeys(wParam) + ", button:" + MapToToolkitMouseButtons(wParam));
	}

	internal void DoubleClick( int wParam, int lParam)
	{
		sink.ToolkitMouseDown(MapToToolkitMouseButtons(wParam), MapMouseToToolkitKeys(wParam), 2 ,MouseX(lParam), MouseY(lParam),0);
		//Console.WriteLine("DrawingWindow.DoubleClick [" + (MouseX(lParam)) + "," + (MouseY(lParam)) + "], key:" + MapMouseToToolkitKeys(wParam) + ", button:" + MapToToolkitMouseButtons(wParam));
	}

	internal void KeyDown( int wParam, int lParam)
	{
		sink.ToolkitKeyDown(MapKeyToToolkitKeys( wParam));
		//Console.WriteLine("DrawingWindow.KeyDown " + (MapKeyToToolkitKeys( wParam)).ToString() + " " + wParam);
	}

	internal void Char( int wParam, int lParam)
	{
		sink.ToolkitKeyChar((char)wParam);
		//Console.WriteLine("DrawingWindow.Char " + ((char)wParam).ToString());
	}
	
	internal void KeyUp( int wParam, int lParam )
	{
		sink.ToolkitKeyUp(MapKeyToToolkitKeys( wParam));
		//Console.WriteLine("DrawingWindow.KeyUp " + (MapKeyToToolkitKeys( wParam)).ToString());
	}

	//TODO:
	internal void SetCursor( int cursor )
	{
		Win32.Api.SetCursor(Win32.Api.LoadCursorA(IntPtr.Zero, Win32.Api.CursorName.IDC_ARROW));
	}

	//WM_PAINT
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
		//Console.WriteLine( "DrawingWindow.Paint hwnd="+hwnd );

		Win32.Api.EndPaint( hwnd, ref myPS );
	}

	//WM_SETFOCUS occurs when either mouse or keyboard sets focus
	internal virtual void SetFocus()
	{
		if (sink != null)
			sink.ToolkitFocusEnter();
		//Console.WriteLine( "DrawingWindow.GotFocus hwnd="+hwnd );
	}

	//WM_KILLFOCUS occurs when either mouse or keyboard causes focus to be lost (or windows does)
	internal virtual void KillFocus()
	{
		if (sink != null)
			sink.ToolkitFocusLeave();
		//Console.WriteLine( "DrawingWindow.LostFocus hwnd="+hwnd ) ;
	}

	//WM_WINDOWPOSCHANGING
	internal void WindowPosChanging(int lParam)
	{
		//When window is created - CreateWindow(), WindowPosChanging is called when the initial size is set
		//because sink==null, its size and position will be set
		if (sink != null)
		{
		#if !__CSCC__	// cscc has probs with pointers - to be fixed soon -- Rhys
			unsafe
			{
				Win32.Api.WINDOWPOS *pos = (Win32.Api.WINDOWPOS*)lParam;
				Rectangle rect = (this as IToolkitWindow).Dimensions;
				if (suspendExternalMoveResizeNotify == true)
					return;
				// If minimizing then dont need to know
				//if ((*pos).x == -32000)
				//	return;
				// If moving
				if (((Win32.Api.SetWindowsPosFlags)(*pos).flags & Win32.Api.SetWindowsPosFlags.SWP_NOMOVE) == 0)
				{
					int leftAdjust, topAdjust, rightAdjust, bottomAdjust;
					Toolkit.GetWindowAdjust(out leftAdjust, out topAdjust, out rightAdjust, out bottomAdjust, flags);
					sink.ToolkitExternalMove( (*pos).x + leftAdjust, (*pos).y + topAdjust );
				}

				// If sizing
				if (((Win32.Api.SetWindowsPosFlags)(*pos).flags & Win32.Api.SetWindowsPosFlags.SWP_NOSIZE) ==0)
				{
					int leftAdjust, topAdjust, rightAdjust, bottomAdjust;
					Toolkit.GetWindowAdjust(out leftAdjust, out topAdjust, out rightAdjust, out bottomAdjust, flags);
					sink.ToolkitExternalResize( (*pos).cx - leftAdjust - rightAdjust, (*pos).cy - topAdjust - bottomAdjust );
				}

				// Now prevent windows from changing the position or size, System.Windows.Control will do that
				(*pos).flags |= (uint)0x3;
			}
		#endif
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

	//This is called to set whether a window is visible or not. Can only happen once the window is created
	protected void setVisible()
	{
		if (visible)
			Win32.Api.ShowWindow(hwnd,Win32.Api.ShowWindowCommand.SW_SHOWNA);
		else
			Win32.Api.ShowWindow(hwnd,Win32.Api.ShowWindowCommand.SW_HIDE);
	}

	abstract internal void CreateWindow();

}
}
