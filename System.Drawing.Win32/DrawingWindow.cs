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
using d = System.Diagnostics.Debug;

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

	//The dimensions before the control has been created
	protected Rectangle dimensions;
	//Whether the control should be visible once its created
	protected bool visible;

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
		d.WriteLine("DrawingWindow.SetTitle, hwnd="+hwnd);
	}

	void IToolkitWindow.Lower()
	{
		if (hwnd == IntPtr.Zero)
			d.WriteLine("DrawingWindow.Lower ERROR: Cant lower window. Hwnd not created yet.");
		else
		{
			Win32.Api.SetWindowPos(hwnd, Win32.Api.SetWindowsPosPosition.HWND_BOTTOM, 0, 0, 0, 0, Win32.Api.SetWindowsPosFlags.SWP_NOMOVE | Win32.Api.SetWindowsPosFlags.SWP_NOSIZE);
			d.WriteLine("DrawingWindow.Lower, hwnd="+hwnd);
		}
	}

	void IToolkitWindow.Raise()
	{
		if (hwnd == IntPtr.Zero)
			throw new ApplicationException("DrawingWindow.Raise ERROR: Cant raise window. Hwnd not created yet.");
		
		Win32.Api.SetWindowPos(hwnd, Win32.Api.SetWindowsPosPosition.HWND_TOP, 0, 0, 0, 0, Win32.Api.SetWindowsPosFlags.SWP_NOMOVE | Win32.Api.SetWindowsPosFlags.SWP_NOSIZE);
		d.WriteLine("DrawingWindow.Raise, hwnd="+hwnd);
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
		d.WriteLine("DrawingWindow.MoveToAbove, hwnd="+hwnd);
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

	//Client dimensions
	Rectangle IToolkitWindow.Dimensions
	{
		get
		{
			if (hwnd==IntPtr.Zero)
				return dimensions;
			else
			{
				Win32.Api.RECT rect;
				Win32.Api.GetClientRect(hwnd, out rect);
				return new System.Drawing.Rectangle(rect.top, rect.left, rect.right-rect.left, rect.bottom - rect.top);
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
			// TODO
			return false;
		}
		set
		{
			// TODO
		}
	}

	//Set the focus to this window
	void IToolkitWindow.Focus()
	{
		d.WriteLine("DrawingWindow.Focus:"+ hwnd);
		Win32.Api.SetFocus(hwnd);
	}

	//Destroy window
	public virtual void Destroy()
	{
		//Call will ignore if hwnd = IntPtr.Zero
		Win32.Api.DestroyWindow(hwnd);
		d.WriteLine("DrawingWindow.Destroy, hwnd="+hwnd);
	}

	//After the WM_DESTORY message
	protected virtual void Destroyed()
	{
		d.WriteLine("DrawingWindow.Destroyed, hwnd="+hwnd);
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
			d.WriteLine("DrawingControlWindow.SetBackground, hwnd="+hwnd);
		}
	}

	void IToolkitWindow.MoveResize(int x, int y, int width, int height)
	{
		if (hwnd == IntPtr.Zero)
			dimensions = new Rectangle(x, y, width, height);
		else
		{
			Rectangle rect = (this as IToolkitWindow).Dimensions;
	
			if (x != rect.Left || y != rect.Right || width != rect.Width
				|| height != rect.Height)
			{
				Rectangle outside = OutsideFromClientSize(new Rectangle( x, y, width, height));
				Win32.Api.SetWindowPos(hwnd, Win32.Api.SetWindowsPosPosition.HWND_TOP, outside.Left, outside.Top, outside.Width, outside.Height, Win32.Api.SetWindowsPosFlags.SWP_NOSENDCHANGING);
			}
		}
		d.WriteLine("DrawingWindow.MoveResize, hwnd="+hwnd+",["+x+","+y+","+width+","+height+"]");
	}

	// Set the event sink to use for this window.
	void IToolkitWindow.SetEventSink(IToolkitEventSink sink)
	{
		this.sink = sink;
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
			d.WriteLine("DrawingWindow.setIsMapped hwnd="+hwnd+",visible="+value);
		}
	}

	// Force an update of all invalidated regions.
	void IToolkitWindow.Update()
	{
		if (hwnd == IntPtr.Zero)
			throw new ApplicationException("DrawingWindow.Update ERROR: Cant update. Hwnd not created yet.");
		
		Win32.Api.UpdateWindow(hwnd);
		d.WriteLine("DrawingWindow.Update, hwnd="+hwnd);
	}

	void IToolkitWindow.SetForeground(Color color)
	{
	}

	void IToolkitWindow.Iconify()
	{
		if (hwnd == IntPtr.Zero)
			throw new ApplicationException("DrawingWindow.Iconify ERROR: Cant Iconify. Hwnd not created yet.");
		
		Win32.Api.CloseWindow(hwnd);
		d.WriteLine("DrawingWindow.Iconify, hwnd="+hwnd);
	}


	// Get a toolkit graphics object for this window.
	IToolkitGraphics IToolkitWindow.GetGraphics()
	{
		if (hwnd == IntPtr.Zero)
			throw new ApplicationException("DrawingWindow.GetGraphics ERROR: Cant GetGraphics. Hwnd not created yet.");
		
		d.WriteLine("DrawingWindow.GetGraphics, hwnd="+hwnd);
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
		d.WriteLine("DrawingWindow.Invalidate, hwnd="+hwnd + " ["+x+","+y+","+width+","+height+"]");
	}

	// Invalidate this window.
	public void Invalidate()
	{
		if (hwnd != IntPtr.Zero)
			Win32.Api.InvalidateRect(hwnd, IntPtr.Zero, false);
	}

	//Called when Windows wants to erase the form background. Use the provided hdc
	protected void EraseBackground(IntPtr hdc) 
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
	protected void MouseMove(int wParam, int lParam) 
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
		//d.WriteLine("DrawingWindow.MouseMove [" + (MouseX(lParam)) + "," + (MouseY(lParam)) + "], key:" + MapMouseToToolkitKeys(wParam));
	}

	//Called when windows receives WM_MOUSEWHEEL
	protected void MouseWheel( int wParam, int lParam)
	{
		int wheelDelta = (wParam >> 16)/120;
		sink.ToolkitMouseWheel(ToolkitMouseButtons.None, MapMouseToToolkitKeys(wParam), 0, MouseX(lParam) , MouseY(lParam), wheelDelta);
		d.WriteLine("DrawingWindow.MouseWheel [" + (MouseX(lParam)) + "," + (MouseY(lParam)) + "], key:" + MapMouseToToolkitKeys(wParam) + ", wheel:" + wheelDelta);
	}

	protected void MouseLeave() 
	{
		trackingMouse = false;
		sink.ToolkitMouseLeave();
		d.WriteLine("DrawingWindow.MouseLeave, hwnd="+hwnd);
	}

	protected void ButtonDown(int wParam, int lParam)
	{
		sink.ToolkitMouseDown(MapToToolkitMouseButtons(wParam), MapMouseToToolkitKeys(wParam), 1 ,MouseX(lParam), MouseY(lParam) ,0);
		d.WriteLine("DrawingWindow.ButtonDown [" + (MouseX(lParam)) + "," + (MouseY(lParam)) + "], key:" + MapMouseToToolkitKeys(wParam) + ", button:" + MapToToolkitMouseButtons(wParam));
	}

	protected void ButtonUp(int wParam, int lParam)
	{
		sink.ToolkitMouseUp(MapToToolkitMouseButtons(wParam), MapMouseToToolkitKeys(wParam), 1 ,MouseX(lParam), MouseY(lParam),0);
		d.WriteLine("DrawingWindow.ButtonUp [" + (MouseX(lParam)) + "," + (MouseY(lParam)) + "], key:" + MapMouseToToolkitKeys(wParam) + ", button:" + MapToToolkitMouseButtons(wParam));
	}

	protected void DoubleClick( int wParam, int lParam)
	{
		sink.ToolkitMouseDown(MapToToolkitMouseButtons(wParam), MapMouseToToolkitKeys(wParam), 2 ,MouseX(lParam), MouseY(lParam),0);
		d.WriteLine("DrawingWindow.DoubleClick [" + (MouseX(lParam)) + "," + (MouseY(lParam)) + "], key:" + MapMouseToToolkitKeys(wParam) + ", button:" + MapToToolkitMouseButtons(wParam));
	}

	protected void KeyDown( int wParam, int lParam)
	{
		sink.ToolkitKeyDown(MapKeyToToolkitKeys( wParam));
		d.WriteLine("DrawingWindow.KeyDown " + (MapKeyToToolkitKeys( wParam)).ToString() + " " + wParam);
	}

	protected void Char( int wParam, int lParam)
	{
		sink.ToolkitKeyChar((char)wParam);
		d.WriteLine("DrawingWindow.Char " + ((char)wParam).ToString());
	}
	
	protected void KeyUp( int wParam, int lParam )
	{
		sink.ToolkitKeyUp(MapKeyToToolkitKeys( wParam));
		d.WriteLine("DrawingWindow.KeyUp " + (MapKeyToToolkitKeys( wParam)).ToString());
	}

	//TODO:
	protected void SetCursor( int cursor )
	{
		Win32.Api.SetCursor(Win32.Api.LoadCursorA(IntPtr.Zero, Win32.Api.CursorName.IDC_ARROW));
	}

	//WM_PAINT
	protected void Paint()
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
		d.WriteLine( "DrawingWindow.Paint hwnd="+hwnd );

		Win32.Api.EndPaint( hwnd, ref myPS );
	}

	//WM_SETFOCUS occurs when either mouse or keyboard sets focus
	protected virtual void SetFocus()
	{
		if (sink != null)
			sink.ToolkitFocusEnter();
		d.WriteLine( "DrawingWindow.GotFocus hwnd="+hwnd );
	}

	//WM_KILLFOCUS occurs when either mouse or keyboard causes focus to be lost (or windows does)
	protected virtual void KillFocus()
	{
		if (sink != null)
			sink.ToolkitFocusLeave();
		d.WriteLine( "DrawingWindow.LostFocus hwnd="+hwnd ) ;
	}

	//WM_WINDOWPOSCHANGING
	protected void WindowPosChanging(int lParam)
	{
		//When window is created - CreateWindow(), WindowPosChanging is called when the initial size is set
		//because sink==null, its size and position will be set
		if (sink != null)
		{
		#if false	// cscc has probs with pointers - to be fixed soon -- Rhys
			unsafe
			{
				Win32.Api.WINDOWPOS *pos = (Win32.Api.WINDOWPOS*)lParam;
				Rectangle rect = (this as IToolkitWindow).Dimensions;
					
				//If moving
				if (((*pos).flags & 2) == 0)
				{
					Rectangle offset = OutsideFromClientSize( new Rectangle(0,0,0,0) );
					sink.ToolkitExternalMove( (*pos).x - offset.X, (*pos).y - offset.Y );
				}

				//If sizing
				if (((*pos).flags & 1) ==0)
				{
					Rectangle offset = OutsideFromClientSize( new Rectangle(0,0,0,0) );
					sink.ToolkitExternalResize( (*pos).cx - offset.Width, (*pos).cy - offset.Height );
				}	
				//Now prevent windows from changing the position or size, System.Windows.Control will do that
				(*pos).flags |= (uint)0x3;
			}
		#endif
		}
	}

	//WM_SETTINGSCHANGE occurs when some windows setting changes. This is used to notify the app that system settings have changed eg. button colors or form colors
	//We only want the RootTopLevelWindow to receive this
	protected virtual void SettingsChange(int wParam)
	{
	}

	protected virtual void Close()
	{
	}

	//The main windows loop. Messages are handed off
	protected int WindowsLoop(IntPtr hwnd, int msg, int wParam, int lParam)  
	{
		int retval = 0;
		switch((Win32.Api.WindowsMessages)msg) 
		{

			case Win32.Api.WindowsMessages.WM_SETFOCUS:
				SetFocus();
				break;
			case Win32.Api.WindowsMessages.WM_KILLFOCUS:
				KillFocus();
				break;

			case Win32.Api.WindowsMessages.WM_WINDOWPOSCHANGING:
				WindowPosChanging(lParam);
				break;

			case Win32.Api.WindowsMessages.WM_SYSCOMMAND:
				switch((Win32.Api.SystemCommand)wParam) 
				{
					case(Win32.Api.SystemCommand.SC_RESTORE):
						//TODO
						retval = Win32.Api.DefWindowProcA(hwnd, msg, wParam, lParam);
						break;
					case(Win32.Api.SystemCommand.SC_MAXIMIZE):
						//TODO
						retval = Win32.Api.DefWindowProcA(hwnd, msg, wParam, lParam);
						break;
					case(Win32.Api.SystemCommand.SC_MINIMIZE):
						//TODO
						retval = Win32.Api.DefWindowProcA(hwnd, msg, wParam, lParam);
						break;
					case(Win32.Api.SystemCommand.SC_CLOSE):
						//TODO
						Close();
						break;
					default:
						retval = Win32.Api.DefWindowProcA(hwnd, msg, wParam, lParam);
						break;
				}
					break;	

			case Win32.Api.WindowsMessages.WM_DESTROY:
				Destroyed();
				break;

			case Win32.Api.WindowsMessages.WM_PAINT:
				Paint();
				break;
			case Win32.Api.WindowsMessages.WM_ERASEBKGND:
				EraseBackground( new IntPtr(wParam) );
				retval=1;
				break;

			case Win32.Api.WindowsMessages.WM_SETCURSOR:
				//TEMP
				Win32.Api.SetCursor(Win32.Api.LoadCursorA(IntPtr.Zero, Win32.Api.CursorName.IDC_ARROW));
				retval =1;
				break;

			case Win32.Api.WindowsMessages.WM_MOUSEMOVE:
				MouseMove( wParam, lParam );
				break;
			case Win32.Api.WindowsMessages.WM_MOUSEWHEEL:
				MouseMove( wParam, lParam );
				break;
			case Win32.Api.WindowsMessages.WM_LBUTTONDOWN:
				ButtonDown( wParam | (int)Win32.Api.MouseKeyState.MK_LBUTTON, lParam );
				break;
			case Win32.Api.WindowsMessages.WM_RBUTTONDOWN:
				ButtonDown( wParam | (int)Win32.Api.MouseKeyState.MK_RBUTTON, lParam );
				break;
			case Win32.Api.WindowsMessages.WM_MBUTTONDOWN:
				ButtonDown( wParam | (int)Win32.Api.MouseKeyState.MK_MBUTTON, lParam );
				break;
			case Win32.Api.WindowsMessages.WM_XBUTTONDOWN:
				ButtonDown( wParam, lParam );
				break;
			case Win32.Api.WindowsMessages.WM_LBUTTONUP:
				ButtonUp( wParam | (int)Win32.Api.MouseKeyState.MK_LBUTTON, lParam );
				break;
			case Win32.Api.WindowsMessages.WM_RBUTTONUP:
				ButtonUp( wParam | (int)Win32.Api.MouseKeyState.MK_RBUTTON, lParam );
				break;
			case Win32.Api.WindowsMessages.WM_MBUTTONUP:
				ButtonUp( wParam | (int)Win32.Api.MouseKeyState.MK_MBUTTON, lParam );
				break;
			case Win32.Api.WindowsMessages.WM_XBUTTONUP:
				ButtonUp( wParam, lParam );
				break;
			case Win32.Api.WindowsMessages.WM_LBUTTONDBLCLK:
				DoubleClick( wParam | (int)Win32.Api.MouseKeyState.MK_LBUTTON, lParam );
				break;
			case Win32.Api.WindowsMessages.WM_RBUTTONDBLCLK:
				DoubleClick( wParam | (int)Win32.Api.MouseKeyState.MK_RBUTTON, lParam );
				break;
			case Win32.Api.WindowsMessages.WM_MBUTTONDBLCLK:
				DoubleClick( wParam | (int)Win32.Api.MouseKeyState.MK_MBUTTON, lParam );
				break;
			case Win32.Api.WindowsMessages.WM_XBUTTONDBLCLK:
				DoubleClick( wParam, lParam );
				break;
			case Win32.Api.WindowsMessages.WM_MOUSELEAVE:
				MouseLeave();
				break;

			case Win32.Api.WindowsMessages.WM_KEYDOWN:
			case Win32.Api.WindowsMessages.WM_SYSKEYDOWN:
				KeyDown( wParam, lParam );
				break;
			case Win32.Api.WindowsMessages.WM_CHAR:
				Char( wParam, lParam );
				break;
			case Win32.Api.WindowsMessages.WM_KEYUP:
			case Win32.Api.WindowsMessages.WM_SYSKEYUP:
				KeyUp( wParam, lParam );
				break;

			case Win32.Api.WindowsMessages.WM_SETTINGCHANGE:
				SettingsChange( wParam );
				break;

			default:
				retval = Win32.Api.DefWindowProcA(hwnd, msg, wParam, lParam);
				break;
		}
		return retval;
	}

	//Gives the overall window size, given a client size, including all windows trimmings eg. borders, title. DrawingControlWindow doesnt override
	protected virtual Size OutsideFromClientSize(Size client)
	{
		return OutsideFromClientSize(new Rectangle(0, 0, client.Width, client.Height)).Size;
	}

	//Gives the overall window bounds, given a client client bounds, including all windows trimmings eg. borders, title. DrawingControlWindow doesnt override
	protected virtual Rectangle OutsideFromClientSize(Rectangle client)
	{
		return client;
	}

	//Extract the mouse positions
	protected int MouseX( int value )
	{
		return 0xFFFF & value;
	}

	protected int MouseY( int value )
	{
		return value >> 16;
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

}
}
