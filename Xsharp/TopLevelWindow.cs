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
	private bool reparented;
	private bool firstMapDone;
	private IntPtr keyBuffer;
	private InputOnlyWidget focusWidget;
	private InputOnlyWidget defaultFocus;
	private MotifDecorations decorations;
	private MotifFunctions functions;
	private MotifInputType inputType;
	private TopLevelWindow transientFor;

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
				this.reparented = false;
				this.keyBuffer = IntPtr.Zero;
				this.focusWidget = this;
				this.defaultFocus = null;
				this.decorations = MotifDecorations.All;
				this.functions = MotifFunctions.All;
				this.inputType = MotifInputType.Normal;
				this.transientFor = null;

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
								EventMask.FocusChangeMask |
								EventMask.StructureNotifyMask);
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
	internal static Widget GetRoot(Screen screen)
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

	// Perform a MoveResize request.
	internal override void PerformMoveResize
				(IntPtr display, int newX, int newY,
				 int newWidth, int newHeight)
			{
				Xlib.Window handle = GetWidgetHandle();
				XWindowChanges changes = new XWindowChanges();
				ConfigureWindowMask mask = (ConfigureWindowMask)0;

				// If we haven't mapped the window to the screen yet,
				// then set the size hints and bail out with a normal
				// move/resize event.
				if(!firstMapDone)
				{
					XSizeHints hints = new XSizeHints();
					if(newX != 0 || newY != 0)
					{
						hints.flags = SizeHintsMask.USPosition |
									  SizeHintsMask.USSize;
						hints.x = newX;
						hints.y = newY;
					}
					else
					{
						hints.flags = SizeHintsMask.USSize;
					}
					hints.width = newWidth;
					hints.height = newHeight;
					Xlib.XSetWMNormalHints(display, handle, ref hints);
					base.PerformMoveResize
						(display, newX, newY, newWidth, newHeight);
					return;
				}

				// Collect up the changes that need to be performed.
				if(newX != x || newY != y)
				{
					if(newWidth != width || newHeight != height)
					{
						changes.x = newX;
						changes.y = newY;
						changes.width = newWidth;
						changes.height = newHeight;
						mask = ConfigureWindowMask.CWX |
							   ConfigureWindowMask.CWY |
							   ConfigureWindowMask.CWWidth |
							   ConfigureWindowMask.CWHeight;
					}
					else
					{
						changes.x = newX;
						changes.y = newY;
						mask = ConfigureWindowMask.CWX |
							   ConfigureWindowMask.CWY;
					}
				}
				else if(newWidth != width || newHeight != height)
				{
					changes.width = newWidth;
					changes.height = newHeight;
					mask = ConfigureWindowMask.CWWidth |
						   ConfigureWindowMask.CWHeight;
				}

				// Send the reconfiguration request to the window manager.
				if(mask != (ConfigureWindowMask)0)
				{
					Xlib.XReconfigureWMWindow
							(display, handle,
							 Screen.ScreenNumber,
						     (uint)(ConfigureWindowMask.CWStackMode),
							 ref changes);
				}
			}

	/// <summary>
	/// <para>Determine if this widget is currently iconified.</para>
	/// </summary>
	///
	/// <value>
	/// <para>Returns <see langword="true"/> if the widget is iconified;
	/// <see langword="false"/> otherwise.</para>
	/// </value>
	///
	/// <remarks>
	/// <para>Setting this property is equivalent to calling either
	/// <c>Iconify</c> or <c>Deiconify</c>.</para>
	/// </remarks>
	public bool IsIconic
			{
				get
				{
					return iconic;
				}
				set
				{
					if(value)
					{
						Iconify();
					}
					else
					{
						Deiconify();
					}
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
						firstMapDone = true;
						OnMapStateChanged();
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
						OnMapStateChanged();
					}
				}
				finally
				{
					dpy.Unlock();
				}
			}

	/// <summary>
	/// <para>Reparenting is not supported for top-level windows.</para>
	/// </summary>
	public override void Reparent(Widget newParent, int x, int y)
			{
				throw new XInvalidOperationException
					(S._("X_NonTopLevelOperation"));
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
						OnIconicStateChanged(true);
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
						OnIconicStateChanged(false);
						if(!mapped)
						{
							// We are mapped now as well.
							mapped = true;
							firstMapDone = true;
							OnMapStateChanged();
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
	/// because the <c>OnClose</c> method returned
	/// <see langword="false"/>.</para>
	/// </returns>
	public virtual bool Close()
			{
				// Bail out if the window has already been destroyed.
				if(handle == Xlib.Drawable.Zero)
				{
					return true;
				}

				// Ask the "OnClose" method if we can close or not.
				if(!OnClose())
				{
					return false;
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

	// Update the Motif hint information in the X server.
	private void UpdateMotifHints()
			{
				// Build the Motif hint structure.
				Xlib.Xlong[] hint = new Xlib.Xlong [5];
				int flags = 0;
				if(functions != MotifFunctions.All)
				{
					hint[1] = (Xlib.Xlong)(int)functions;
					flags |= 1;
				}
				else
				{
					hint[1] = (Xlib.Xlong)(-1);
				}
				if(decorations != MotifDecorations.All)
				{
					hint[2] = (Xlib.Xlong)(int)decorations;
					flags |= 2;
				}
				else
				{
					hint[2] = (Xlib.Xlong)(-1);
				}
				if(inputType != MotifInputType.Normal)
				{
					hint[3] = (Xlib.Xlong)(int)inputType;
					flags |= 4;
				}
				else
				{
					hint[3] = (Xlib.Xlong)(-1);
				}
				hint[4] = (Xlib.Xlong)(-1);
				hint[0] = (Xlib.Xlong)flags;

				// Set the Motif hint structure on the window.
				try
				{
					IntPtr display = dpy.Lock();
					Xlib.Window handle = GetWidgetHandle();
					Xlib.XChangeProperty
						(display, handle, dpy.wmMwmHints, dpy.wmMwmHints,
						 32, 0 /* PropModeReplace */, hint, 4);
				}
				finally
				{
					dpy.Unlock();
				}
			}

	/// <summary>
	/// <para>Get or set the decorations to display on the window's
	/// border.</para>
	/// </summary>
	///
	/// <value>
	/// <para>The value of this property is the set of decorations
	/// that the window desires from the window manager.  The window
	/// manager might ignore this information.</para>
	/// </value>
	public MotifDecorations Decorations
			{
				get
				{
					return decorations;
				}
				set
				{
					if(decorations != value)
					{
						decorations = value;
						UpdateMotifHints();
					}
				}
			}

	/// <summary>
	/// <para>Get or set the functions to display on the window's
	/// context menu.</para>
	/// </summary>
	///
	/// <value>
	/// <para>The value of this property is the set of functions
	/// that the window desires from the window manager.  The window
	/// manager might ignore this information.</para>
	/// </value>
	public MotifFunctions Functions
			{
				get
				{
					return functions;
				}
				set
				{
					if(functions != value)
					{
						functions = value;
						UpdateMotifHints();
					}
				}
			}

	/// <summary>
	/// <para>Get or set the input type for the window.</para>
	/// </summary>
	///
	/// <value>
	/// <para>The value of this property is the input type.  The window
	/// manager might ignore this information.</para>
	/// </value>
	public MotifInputType InputType
			{
				get
				{
					return inputType;
				}
				set
				{
					if(inputType != value)
					{
						inputType = value;
						UpdateMotifHints();
					}
				}
			}

	/// <summary>
	/// <para>Get or set the transient parent window.</para>
	/// </summary>
	///
	/// <value>
	/// <para>The transient parent window, or <see langword="null"/>
	/// if there is no transient parent.</para>
	/// </value>
	///
	/// <remarks>
	/// <para>Setting this property to <see langword="null"/> or the
	/// current widget will have no effect.</para>
	/// </remarks>
	public TopLevelWindow TransientFor
			{
				get
				{
					return transientFor;
				}
				set
				{
					if(value != null && value != transientFor && value != this)
					{
						// Change the "transient for" hint information.
						try
						{
							// Lock down the display and get the handles.
							IntPtr display = dpy.Lock();
							Xlib.Window handle = GetWidgetHandle();
							Xlib.Window thandle =
								transientFor.GetWidgetHandle();

							// Set the "transient for" hint.
							Xlib.XSetTransientForHint(display, handle, thandle);
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
	/// <para>Method that is called when the window's iconic state
	/// changes.</para>
	/// </summary>
	protected virtual void OnIconicStateChanged(bool iconfiy)
			{
				// Nothing to do in this base class.
			}

	/// <summary>
	/// <para>Method that is called when the window's close box is
	/// clicked by the user.</para>
	/// </summary>
	///
	/// <returns>
	/// <value>Returns <see langword="true"/> to destroy the window,
	/// or <see langword="false"/> to ignore the close message.
	/// </returns>
	protected virtual bool OnClose()
			{
				return true;
			}

	/// <summary>
	/// <para>Raise this widget to the top of its layer.</para>
	/// </summary>
	public override void Raise()
			{
				try
				{
					// Send a message to the window manager to restack us.
					IntPtr display = dpy.Lock();
					Xlib.Window handle = GetWidgetHandle();
					XWindowChanges changes = new XWindowChanges();
					changes.stack_mode = 0;		/* Above */
					Xlib.XReconfigureWMWindow
							(display, handle,
							 Screen.ScreenNumber,
						     (uint)(ConfigureWindowMask.CWStackMode),
							 ref changes);
				}
				finally
				{
					dpy.Unlock();
				}
			}

	/// <summary>
	/// <para>Lower this widget to the bottom of its layer.</para>
	/// </summary>
	public override void Lower()
			{
				try
				{
					// Send a message to the window manager to restack us.
					IntPtr display = dpy.Lock();
					Xlib.Window handle = GetWidgetHandle();
					XWindowChanges changes = new XWindowChanges();
					changes.stack_mode = 1;		/* Below */
					Xlib.XReconfigureWMWindow
							(display, handle,
							 Screen.ScreenNumber,
						     (uint)(ConfigureWindowMask.CWStackMode),
							 ref changes);
				}
				finally
				{
					dpy.Unlock();
				}
			}

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

					case EventType.ConfigureNotify:
					{
						// The window manager may have caused us to move/resize.
						if(xevent.xconfigure.window != xevent.window)
						{
							// SubstructureNotify - not interesting to us.
							break;
						}
						if(xevent.xconfigure.width != width ||
						   xevent.xconfigure.height != height)
						{
							// The size has been changed by the window manager.
							width = xevent.xconfigure.width;
							height = xevent.xconfigure.height;
							OnResize(width, height);
						}
						if(xevent.send_event || !reparented)
						{
							// The window manager moved us to a new position.
							if(x != xevent.xconfigure.x ||
							   y != xevent.xconfigure.y)
							{
								x = xevent.xconfigure.x;
								y = xevent.xconfigure.y;
								OnMove(x, y);
							}
						}
					}
					break;

					case EventType.ReparentNotify:
					{
						// We may have been reparented by the window manager.
						if(xevent.xreparent.window != (Xlib.Window)handle)
						{
							// SubstructureNotify - not interesting to us.
							break;
						}
						if(xevent.xreparent.parent !=
								(Xlib.Window)(screen.RootWindow.handle))
						{
							// Reparented by the window manager.
							reparented = true;
						}
						else
						{
							// Window manager crashed: we are back on the root.
							reparented = false;
							x = xevent.xreparent.x;
							y = xevent.xreparent.y;
							OnMove(x, y);
						}
					}
					break;

					case EventType.MapNotify:
					{
						// The window manager mapped us to the screen.
						if(iconic)
						{
							iconic = false;
							OnIconicStateChanged(false);
						}
						if(!mapped)
						{
							mapped = true;
							OnMapStateChanged();
						}
					}
					break;

					case EventType.UnmapNotify:
					{
						// We were unmapped from the screen.  If "mapped"
						// is true, then we are being iconified by the window
						// manager.  Otherwise, we asked to be withdrawn.
						if(!iconic && mapped)
						{
							iconic = true;
							OnIconicStateChanged(true);
						}
					}
					break;
				}
				base.DispatchEvent(ref xevent);
			}

} // class TopLevelWindow

} // namespace Xsharp
