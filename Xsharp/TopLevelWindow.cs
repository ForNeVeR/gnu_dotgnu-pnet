/*
 * TopLevelWindow.cs - Widget handling for top-level application windows.
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
using System.Runtime.InteropServices;
using Xsharp.Types;
using Xsharp.Events;

/// <summary>
/// <para>The <see cref="T:Xsharp.TopLevelWindow"/> class manages
/// top-level application windows.</para>
/// </summary>
public class TopLevelWindow : InputOutputWidget
{
	// Internal state.
	private String name;
	private bool iconic;
	private bool hasPrimaryFocus;
	private IntPtr keyBuffer;
	private InputOnlyWidget focusWidget;
	private InputOnlyWidget defaultFocus;

	/// <summary>
	/// <para>Constructs a new <see cref="T:Xsharp.TopLevelWindow"/>
	/// instance.</para>
	/// </summary>
	///
	/// <param name="name">
	/// <para>The initial name to display in the title bar.</para>
	/// </param>
	///
	/// <param name="width">
	/// <para>The width of the new window.</para>
	/// </param>
	///
	/// <param name="height">
	/// <para>The height of the new window.</para>
	/// </param>
	///
	/// <exception cref="T:Xsharp.XException">
	/// <para>Raised if <paramref name="width"/> or <paramref name="height"/>
	/// is out of range.</para>
	/// </exception>
	///
	/// <remarks>
	/// <para>The new top-level window will be created on the default
	/// screen of the primary display.  If the primary display is
	/// not open, this constructor will open it.</para>
	/// </remarks>
	public TopLevelWindow(String name, int width, int height)
			: this(null, name, width, height)
			{
				// Nothing to do here: the next constructor does all the work.
			}

	/// <summary>
	/// <para>Constructs a new <see cref="T:Xsharp.TopLevelWindow"/>
	/// instance.</para>
	/// </summary>
	///
	/// <param name="screen">
	/// <para>The screen to display the new top-level window on, or
	/// <see langword="null"/> to use the default screen of the
	/// primary display.</para>
	/// </param>
	///
	/// <param name="name">
	/// <para>The initial name to display in the title bar.  If this
	/// is <see langword="null"/> then the empty string will be used.</para>
	/// </param>
	///
	/// <param name="width">
	/// <para>The width of the new widget.</para>
	/// </param>
	///
	/// <param name="height">
	/// <para>The height of the new widget.</para>
	/// </param>
	///
	/// <exception cref="T:Xsharp.XException">
	/// <para>Raised if <paramref name="width"/> or <paramref name="height"/>
	/// is out of range.</para>
	/// </exception>
	public TopLevelWindow(Screen screen, String name, int width, int height)
			: base(GetRoot(screen), 0, 0, width, height,
			       new Color(StandardColor.Foreground),
			       new Color(StandardColor.Background),
				   true, false)
			{
				// Initialize this object's state.
				this.name = ((name != null) ? name : String.Empty);
				this.iconic = false;
				this.hasPrimaryFocus = false;
				this.keyBuffer = IntPtr.Zero;
				this.focusWidget = this;
				this.defaultFocus = null;

				// Set the initial WM properties.
				try
				{
					// Lock down the display and get the window handle.
					IntPtr display = dpy.Lock();
					Xlib.Window handle = GetWidgetHandle();

					// Set the title bar and icon names.
					Xlib.XStoreName(display, handle, name);
					Xlib.XSetIconName(display, handle, name);

					// Ask for "WM_DELETE_WINDOW" and "WM_TAKE_FOCUS".
					Xlib.Atom[] protocols = new Xlib.Atom [2];
					protocols[0] = dpy.wmDeleteWindow;
					protocols[1] = dpy.wmTakeFocus;
					Xlib.XSetWMProtocols(display, handle, protocols, 2);

					// Top-level widgets receive all key and focus events.
					SelectInput(EventMask.KeyPressMask |
								EventMask.FocusChangeMask);
				}
				finally
				{
					dpy.Unlock();
				}
			}

	/// <summary>
	/// <para>Destroy an instance of <see cref="T:Xsharp.TopLevelWindow"/>.
	/// </para>
	/// </summary>
	~TopLevelWindow()
			{
				if(keyBuffer != IntPtr.Zero)
				{
					Marshal.FreeHGlobal(keyBuffer);
					keyBuffer = IntPtr.Zero;
				}
			}

	// Helper method to get the root window of a specified screen.
	private static Widget GetRoot(Screen screen)
			{
				if(screen == null)
				{
					return Xsharp.Application.Primary
								.Display.DefaultRootWindow;
				}
				else
				{
					return screen.RootWindow;
				}
			}

	/// <summary>
	/// <para>Map this widget to the screen.</para>
	/// </summary>
	public override void Map()
			{
				try
				{
					IntPtr display = dpy.Lock();
					if(!mapped)
					{
						// Use "XMapRaised" to notify the window manager
						// that we want to be brought to the top.
						Xlib.XMapRaised(display, GetWidgetHandle());
						mapped = true;
						MapStateChanged();
					}
				}
				finally
				{
					dpy.Unlock();
				}
			}

	/// <summary>
	/// <para>Unmap this widget from the screen.</para>
	/// </summary>
	public override void Unmap()
			{
				try
				{
					IntPtr display = dpy.Lock();
					if(mapped)
					{
						// Send a "withdraw" message to the window manager,
						// which will take care of unmapping the window for us.
						Xlib.XWithdrawWindow
							(display, GetWidgetHandle(), screen.ScreenNumber);
						mapped = false;
						MapStateChanged();
					}
				}
				finally
				{
					dpy.Unlock();
				}
			}

	/// <summary>
	/// <para>Iconify this window.</para>
	/// </summary>
	public void Iconify()
			{
				try
				{
					IntPtr display = dpy.Lock();
					if(!iconic)
					{
						// Send an "iconify" message to the window manager,
						// which will take care of iconifying the window.
						Xlib.XIconifyWindow
							(display, GetWidgetHandle(), screen.ScreenNumber);
						iconic = true;
						if(IconicState != null)
						{
							IconicState(this, true);
						}
					}
				}
				finally
				{
					dpy.Unlock();
				}
			}

	/// <summary>
	/// <para>De-iconify this window.</para>
	/// </summary>
	public void Deiconify()
			{
				try
				{
					IntPtr display = dpy.Lock();
					if(iconic)
					{
						// Use "XMapRaised" to notify the window manager
						// that we want to be de-iconified.
						Xlib.XMapRaised(display, GetWidgetHandle());
						iconic = false;
						if(IconicState != null)
						{
							IconicState(this, false);
						}
						if(!mapped)
						{
							// We are mapped now as well.
							mapped = true;
							MapStateChanged();
						}
					}
				}
				finally
				{
					dpy.Unlock();
				}
			}

	/// <summary>
	/// <para>Close this window, as if the user had clicked the close box.
	/// </para>
	/// </summary>
	///
	/// <returns>
	/// <para>Returns <see langword="true"/> if the window was destroyed,
	/// or <see langword="false"/> if the window is still active
	/// because one of the close event handlers reported
	/// <see langword="false"/>.</para>
	/// </returns>
	public bool Close()
			{
				// Bail out if the window has already been destroyed.
				if(handle == Xlib.Drawable.Zero)
				{
					return true;
				}

				// Call each of the "Closed" event handlers in turn,
				// until one returns false or the window is destroyed.
				if(Closed != null)
				{
					Delegate[] list = Closed.GetInvocationList();
					int index;
					ClosedEventHandler handler;
					for(index = 0; index < list.Length; ++index)
					{
						handler = (ClosedEventHandler)(list[index]);
						if(!handler(this))
						{
							// This handler wanted us to bail out.
							return (handle == Xlib.Drawable.Zero);
						}
						else if(handle == Xlib.Drawable.Zero)
						{
							// The handler destroyed the window: we assume
							// that it didn't want the standard quit logic.
							return true;
						}
					}
				}

				// Destroy the window.
				Destroy();

				// If this was the last undestroyed top-level window
				// that was still mapped, then quit the application.
				Widget child = Parent.TopChild;
				TopLevelWindow tchild;
				bool sawActive = false;
				while(child != null)
				{
					tchild = (child as TopLevelWindow);
					if(tchild != null)
					{
						if(tchild.mapped &&
						   tchild.handle != Xlib.Drawable.Zero)
						{
							sawActive = true;
							break;
						}
					}
					child = child.NextBelow;
				}
				if(!sawActive)
				{
					dpy.Quit();
				}

				// The window no longer exists.
				return true;
			}

	/// <summary>
	/// <para>Get or set the name in the window's title bar.</para>
	/// </summary>
	///
	/// <value>
	/// <para>The value of this property is the name to display in
	/// the title bar.  If the value is set to <see langword="null"/>,
	/// then the empty string will be used.</para>
	/// </value>
	public String Name
			{
				get
				{
					return name;
				}
				set
				{
					if(name != value)
					{
						name = ((value != null) ? value : String.Empty);
						try
						{
							// Lock down the display and get the window handle.
							IntPtr display = dpy.Lock();
							Xlib.Window handle = GetWidgetHandle();

							// Set the title bar and icon names.
							Xlib.XStoreName(display, handle, name);
							Xlib.XSetIconName(display, handle, name);
						}
						finally
						{
							dpy.Unlock();
						}
					}
				}
			}

	/// <summary>
	/// <para>Get or set the child widget that gets the keyboard focus
	/// by default when the top-level window receives the focus.</para>
	/// </summary>
	///
	/// <value>
	/// <para>The widget that receives the focus, or <see langword="null"/>
	/// if there is no default focus.</para>
	/// </value>
	///
	/// <exception cref="T.Xsharp.XInvalidOperationException">
	/// <para>Raised if the widget is not a child of this top-level window.
	/// </para>
	/// </exception>
	public Widget DefaultFocus
			{
				get
				{
					return defaultFocus;
				}
				set
				{
					if(value == null)
					{
						defaultFocus = null;
					}
					else
					{
						if(!(value is InputOnlyWidget) ||
						   value.TopLevel != this)
						{
							throw new XInvalidOperationException
								(S._("X_InvalidFocusChild"));
						}
					}
				}
			}

	/// <summary>
	/// <para>Event that is raised when the window's iconic state
	/// changes.</para>
	/// </summary>
	public event IconicStateEventHandler IconicState;

	/// <summary>
	/// <para>Event that is raised when the window's close box is
	/// clicked by the user.</para>
	/// </summary>
	///
	/// <remarks>
	/// <para>The system will call each close handler in turn.  If one
	/// of them returns <see langword="false"/>, the process stops
	/// and no further actions are taken.</para>
	///
	/// <para>If all handlers return <see langword="true"/>, or there
	/// are no handlers registered, then <c>Destroy</c> will be called
	/// to destroy the window.  If this was the last mapped top-level
	/// window attached to the screen, then <c>Display.Quit</c> will
	/// then be called to quit the application.</para>
	///
	/// <para>If an event handler wants to remove the window from the
	/// screen without destroying it or quitting, it should call
	/// <c>Unmap</c> and then return <see langword="false"/>.</para>
	///
	/// <para>If an event handler wants to destroy a window without
	/// quitting, it should call <c>Destroy</c> and then return
	/// <see langword="false"/>.</para>
	/// </remarks>
	public event ClosedEventHandler Closed;

	// Detect that this top-level window has gained the primary focus.
	private void PrimaryFocusIn()
			{
				if(!hasPrimaryFocus)
				{
					hasPrimaryFocus = true;
					if(defaultFocus != null)
					{
						focusWidget = defaultFocus;
						focusWidget.DispatchFocusIn(null);
					}
					else if(focusWidget != null)
					{
						focusWidget.DispatchFocusIn(null);
					}
				}
			}

	// Set the focus widget to a specific child.
	internal void SetFocus(InputOnlyWidget widget)
			{
				if(!hasPrimaryFocus)
				{
					focusWidget = widget;
				}
				else if(focusWidget != widget)
				{
					InputOnlyWidget oldFocus = focusWidget;
					focusWidget = widget;
					oldFocus.DispatchFocusOut(widget);
					if(focusWidget == widget)
					{
						widget.DispatchFocusIn(oldFocus);
					}
				}
			}

	// Determine if a specific child currently has the focus.
	internal bool HasFocus(InputOnlyWidget widget)
			{
				return (hasPrimaryFocus && focusWidget == widget);
			}

	// Dispatch an event to this widget.
	internal override void DispatchEvent(ref XEvent xevent)
			{
				switch(xevent.type)
				{
					case EventType.ClientMessage:
					{
						// Handle messages from the window manager.
						if(xevent.xclient.message_type == dpy.wmProtocols)
						{
							if(xevent.xclient.l(0) ==
									(int)(dpy.wmDeleteWindow))
							{
								// User wants the window to close.
								Close();
							}
							if(xevent.xclient.l(0) == (int)(dpy.wmTakeFocus))
							{
								// We were given the primary input focus.
								PrimaryFocusIn();
							}
						}
					}
					break;

					case EventType.FocusIn:
					{
						// This window has received the focus.
						PrimaryFocusIn();
					}
					break;

					case EventType.FocusOut:
					{
						// This window has lost the focus.
						if(hasPrimaryFocus)
						{
							hasPrimaryFocus = false;
							if(focusWidget != null)
							{
								focusWidget.DispatchFocusOut(null);
							}
						}
					}
					break;

					case EventType.KeyPress:
					{
						// Convert the event into a symbol and a string.
						if(keyBuffer == IntPtr.Zero)
						{
							keyBuffer = Marshal.AllocHGlobal(32);
						}
						Xlib.KeySym keysym = 0;
						int len = Xlib.XLookupString
							(ref xevent.xkey, keyBuffer, 32,
							 ref keysym, IntPtr.Zero);
						String str;
						if(len > 0)
						{
							str = Marshal.PtrToStringAnsi(keyBuffer, len);
						}
						else
						{
							str = null;
						}

						// Dispatch the event.
						Widget widget = focusWidget;
						InputOnlyWidget io;
						while(widget != null)
						{
							io = (widget as InputOnlyWidget);
							if(io != null)
							{
								if(io.DispatchKeyEvent
									((KeyName)keysym, xevent.xkey.state, str))
								{
									break;
								}
							}
							if(widget == this)
							{
								break;
							}
							widget = widget.Parent;
						}
					}
					break;
				}
				base.DispatchEvent(ref xevent);
			}

} // class TopLevelWindow

} // namespace Xsharp
