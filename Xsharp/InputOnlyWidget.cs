/*
 * InputOnlyWidget.cs - Widget handling for input-only widgets.
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
/// <para>The <see cref="T:Xsharp.InputOnlyWidget"/> class manages widgets
/// that occupy screen real estate for the purpose of keyboard and pointer
/// handling, but which do not have output functionality.</para>
/// </summary>
public class InputOnlyWidget : Widget
{
	// Internal state.
	private InputEventHandlers handlers;
	private bool focusable;

	/// <summary>
	/// <para>Constructs a new <see cref="T:Xsharp.InputOnlyWidget"/>
	/// instance underneath a specified parent widget.</para>
	/// </summary>
	///
	/// <param name="parent">
	/// <para>The parent of the new widget.</para>
	/// </param>
	///
	/// <param name="x">
	/// <para>The X co-ordinate of the top-left corner of
	/// the new widget.</para>
	/// </param>
	///
	/// <param name="y">
	/// <para>The Y co-ordinate of the top-left corner of
	/// the new widget.</para>
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
	/// <exception cref="T:System.ArgumentNullException">
	/// <para>Raised if <paramref name="parent"/> is <see langword="null"/>.
	/// </para>
	/// </exception>
	///
	/// <exception cref="T:Xsharp.XException">
	/// <para>Raised if <paramref name="x"/>, <paramref name="y"/>,
	/// <paramref name="width"/>, or <paramref name="height"/> are
	/// out of range.</para>
	/// </exception>
	///
	/// <exception cref="T.Xsharp.XInvalidOperationException">
	/// <para>Raised if <paramref name="parent"/> is disposed or the
	/// root window.</para>
	/// </exception>
	public InputOnlyWidget(Widget parent, int x, int y, int width, int height)
			: base(GetDisplay(parent, false), GetScreen(parent),
				   DrawableKind.InputOnlyWidget, parent)
			{
				bool ok = false;
				try
				{
					// Validate the position and size.
					if(x < -32768 || x > 32767 ||
					   y < -32768 || y > 32767)
					{
						throw new XException(S._("X_InvalidPosition"));
					}
					if(width < 1 || width > 32767 ||
					   height < 1 || height > 32767 ||
					   !ValidateSize(width, height))
					{
						throw new XException(S._("X_InvalidSize"));
					}

					// Set the initial position and size of the widget.
					this.x = x;
					this.y = y;
					this.width = width;
					this.height = height;
					this.focusable = true;

					// Lock down the display and create the window handle.
					try
					{
						IntPtr display = dpy.Lock();
						Xlib.Window pwindow = parent.GetWidgetHandle();
						Xlib.Window window = Xlib.XCreateWindow
								(display, pwindow,
								 x, y, (uint)width, (uint)height, (uint)0,
								 screen.DefaultDepth, 2 /* InputOnly */,
								 screen.DefaultVisual,
								 (uint)0, IntPtr.Zero);
						SetWidgetHandle(window);
						if(parent.AutoMapChildren)
						{
							Xlib.XMapWindow(display, window);
						}
					}
					finally
					{
						dpy.Unlock();
					}

					// Push the widget down to the default layer.
					layer = 0x7FFFFFFF;
					Layer = 0;
					ok = true;
				}
				finally
				{
					if(!ok)
					{
						// Creation failed, so detach ourselves from
						// the parent's widget tree.
						Detach();
					}
				}
			}

	// Internal constructor that is used by the "InputOutputWidget" subclass.
	internal InputOnlyWidget(Widget parent, int x, int y,
							 int width, int height, Color background,
							 bool rootAllowed, bool overrideRedirect)
			: base(GetDisplay(parent, rootAllowed), GetScreen(parent),
				   DrawableKind.Widget, parent)
			{
				bool ok = false;
				try
				{
					// Validate the position and size.
					if(x < -32768 || x > 32767 ||
					   y < -32768 || y > 32767)
					{
						throw new XException(S._("X_InvalidPosition"));
					}
					if(width < 1 || width > 32767 ||
					   height < 1 || height > 32767 ||
					   !ValidateSize(width, height))
					{
						throw new XException(S._("X_InvalidSize"));
					}

					// Set the initial position and size of the widget.
					this.x = x;
					this.y = y;
					this.width = width;
					this.height = height;
					this.focusable = true;

					// Lock down the display and create the window handle.
					try
					{
						IntPtr display = dpy.Lock();
						Xlib.Window pwindow = parent.GetWidgetHandle();
						XSetWindowAttributes attrs = new XSetWindowAttributes();
						attrs.override_redirect = overrideRedirect;
						Xlib.Window window = Xlib.XCreateWindow
								(display, pwindow,
								 x, y, (uint)width, (uint)height, (uint)0,
								 screen.DefaultDepth, 1 /* InputOutput */,
								 screen.DefaultVisual,
								 (uint)(CreateWindowMask.CWOverrideRedirect),
								 ref attrs);
						SetWidgetHandle(window);
						if(background.Index == StandardColor.Inherit)
						{
							Xlib.XSetWindowBackgroundPixmap
								(display, window, Xlib.Pixmap.ParentRelative);
						}
						else
						{
							Xlib.XSetWindowBackground(display, window,
													  ToPixel(background));
						}
						if(parent.AutoMapChildren)
						{
							Xlib.XMapWindow(display, window);
						}
					}
					finally
					{
						dpy.Unlock();
					}

					// Push the widget down to the default layer.
					layer = 0x7FFFFFFF;
					Layer = 0;
					ok = true;
				}
				finally
				{
					if(!ok)
					{
						// Creation failed, so detach ourselves from
						// the parent's widget tree.
						Detach();
					}
				}
			}

	// Get the display associated with a parent widget, and also
	// validate the widget.
	private static Display GetDisplay(Widget parent, bool rootAllowed)
			{
				if(parent == null)
				{
					throw new ArgumentNullException("parent");
				}
				if(!rootAllowed && parent is RootWindow)
				{
					throw new XInvalidOperationException
						(S._("X_NonRootParent"));
				}
				return parent.Display;
			}

	// Get the screen associated with a parent widget.
	private static Screen GetScreen(Widget parent)
			{
				if(parent != null)
				{
					return parent.Screen;
				}
				else
				{
					return null;
				}
			}

	/// <summary>
	/// <para>Get or set the focusable state for this widget.</para>
	/// </summary>
	///
	/// <value>
	/// <para>Set to <see langword="true"/> if this widget can receive
	/// the focus during TAB navigation.  The default value is
	/// <see langword="true"/>.</para>
	/// </value>
	///
	/// <remarks>
	/// <para>This property is used during TAB navigation to determine
	/// where to send the focus next.  Widgets that are not focusable
	/// will be ignored during TAB navigation.</para>
	/// </remarks>
	public bool Focusable
			{
				get
				{
					return focusable;
				}
				set
				{
					focusable = value;
				}
			}

	/// <summary>
	/// <para>Determine if this widget currently has the keyboard focus.</para>
	/// </summary>
	///
	/// <value>
	/// <para>Returns <see langword="true"/> if this widget currently
	/// has the keyboard focus.</para>
	/// </value>
	public bool Focused
			{
				get
				{
					TopLevelWindow topLevel = TopLevel;
					if(topLevel != null)
					{
						return topLevel.HasFocus(this);
					}
					else
					{
						return false;
					}
				}
			}

	// Get the input handler list.
	private InputEventHandlers GetHandlers()
			{
				if(handlers == null)
				{
					handlers = new InputEventHandlers();
				}
				return handlers;
			}

	/// <summary>
	/// <para>Event that is raised when any mouse button is pressed
	/// while the pointer is inside this widget.</para>
	/// </summary>
	public event ButtonPressEventHandler ButtonPress
			{
				add
				{
					lock(this)
					{
						InputEventHandlers handlers = GetHandlers();
						SelectInput(EventMask.ButtonPressMask);
						handlers.press += value;
					}
				}
				remove
				{
					lock(this)
					{
						if(handlers != null)
						{
							handlers.press -= value;
							handlers.DeselectButtonPress(this);
						}
					}
				}
			}

	/// <summary>
	/// <para>Event that is raised when any mouse button is released
	/// while the pointer is inside this widget.</para>
	/// </summary>
	public event ButtonReleaseEventHandler ButtonRelease
			{
				add
				{
					lock(this)
					{
						InputEventHandlers handlers = GetHandlers();
						SelectInput(EventMask.ButtonReleaseMask);
						handlers.release += value;
					}
				}
				remove
				{
					lock(this)
					{
						if(handlers != null)
						{
							handlers.release -= value;
							handlers.DeselectButtonRelease(this);
						}
					}
				}
			}

	/// <summary>
	/// <para>Event that is raised when any mouse button is double-clicked
	/// while the pointer is inside this widget.</para>
	/// </summary>
	public event ButtonPressEventHandler ButtonDoubleClick
			{
				add
				{
					lock(this)
					{
						InputEventHandlers handlers = GetHandlers();
						SelectInput(EventMask.ButtonPressMask);
						handlers.doubleClick += value;
					}
				}
				remove
				{
					lock(this)
					{
						if(handlers != null)
						{
							handlers.doubleClick -= value;
							handlers.DeselectButtonPress(this);
						}
					}
				}
			}

	/// <summary>
	/// <para>Event that is raised when the "Select" mouse button is pressed
	/// while the pointer is inside this widget.  Usually this is the
	/// "Left" mouse button.</para>
	/// </summary>
	public event ButtonPressEventHandler SelectPress
			{
				add
				{
					lock(this)
					{
						InputEventHandlers handlers = GetHandlers();
						SelectInput(EventMask.ButtonPressMask);
						handlers.selectPress += value;
					}
				}
				remove
				{
					lock(this)
					{
						if(handlers != null)
						{
							handlers.selectPress -= value;
							handlers.DeselectButtonPress(this);
						}
					}
				}
			}

	/// <summary>
	/// <para>Event that is raised when the "Select" mouse button is released
	/// while the pointer is inside this widget.  Usually this is the
	/// "Left" mouse button.</para>
	/// </summary>
	public event ButtonReleaseEventHandler SelectRelease
			{
				add
				{
					lock(this)
					{
						InputEventHandlers handlers = GetHandlers();
						SelectInput(EventMask.ButtonReleaseMask);
						handlers.selectRelease += value;
					}
				}
				remove
				{
					lock(this)
					{
						if(handlers != null)
						{
							handlers.selectRelease -= value;
							handlers.DeselectButtonRelease(this);
						}
					}
				}
			}

	/// <summary>
	/// <para>Event that is raised when the "Select" mouse button is
	/// double-clicked while the pointer is inside this widget.</para>
	/// </summary>
	public event ButtonPressEventHandler SelectDoubleClick
			{
				add
				{
					lock(this)
					{
						InputEventHandlers handlers = GetHandlers();
						SelectInput(EventMask.ButtonPressMask);
						handlers.selectDoubleClick += value;
					}
				}
				remove
				{
					lock(this)
					{
						if(handlers != null)
						{
							handlers.selectDoubleClick -= value;
							handlers.DeselectButtonPress(this);
						}
					}
				}
			}

	/// <summary>
	/// <para>Event that is raised when the "Menu" mouse button is pressed
	/// while the pointer is inside this widget.  Usually this is the
	/// "Right" mouse button.</para>
	/// </summary>
	public event ButtonPressEventHandler MenuPress
			{
				add
				{
					lock(this)
					{
						InputEventHandlers handlers = GetHandlers();
						SelectInput(EventMask.ButtonPressMask);
						handlers.menuPress += value;
					}
				}
				remove
				{
					lock(this)
					{
						if(handlers != null)
						{
							handlers.menuPress -= value;
							handlers.DeselectButtonPress(this);
						}
					}
				}
			}

	/// <summary>
	/// <para>Event that is raised when the "Menu" mouse button is released
	/// while the pointer is inside this widget.  Usually this is the
	/// "Right" mouse button.</para>
	/// </summary>
	public event ButtonReleaseEventHandler MenuRelease
			{
				add
				{
					lock(this)
					{
						InputEventHandlers handlers = GetHandlers();
						SelectInput(EventMask.ButtonReleaseMask);
						handlers.menuRelease += value;
					}
				}
				remove
				{
					lock(this)
					{
						if(handlers != null)
						{
							handlers.menuRelease -= value;
							handlers.DeselectButtonRelease(this);
						}
					}
				}
			}

	/// <summary>
	/// <para>Event that is raised when the "Menu" mouse button is
	/// double-clicked while the pointer is inside this widget.</para>
	/// </summary>
	public event ButtonPressEventHandler MenuDoubleClick
			{
				add
				{
					lock(this)
					{
						InputEventHandlers handlers = GetHandlers();
						SelectInput(EventMask.ButtonPressMask);
						handlers.menuDoubleClick += value;
					}
				}
				remove
				{
					lock(this)
					{
						if(handlers != null)
						{
							handlers.menuDoubleClick -= value;
							handlers.DeselectButtonPress(this);
						}
					}
				}
			}

	/// <summary>
	/// <para>Event that is raised when the mouse pointer is moved
	/// inside this widget.</para>
	/// </summary>
	public event PointerMotionEventHandler PointerMotion
			{
				add
				{
					lock(this)
					{
						InputEventHandlers handlers = GetHandlers();
						SelectInput(EventMask.PointerMotionMask);
						handlers.motion += value;
					}
				}
				remove
				{
					lock(this)
					{
						if(handlers != null)
						{
							handlers.motion -= value;
							if(handlers.motion == null)
							{
								DeselectInput(EventMask.PointerMotionMask);
							}
						}
					}
				}
			}

	/// <summary>
	/// <para>Event that is raised if a key is pressed when this
	/// widget has the focus.</para>
	/// </summary>
	public event KeyPressEventHandler KeyPress
			{
				add
				{
					lock(this)
					{
						InputEventHandlers handlers = GetHandlers();
						handlers.keyPress += value;
					}
				}
				remove
				{
					lock(this)
					{
						if(handlers != null)
						{
							handlers.keyPress -= value;
						}
					}
				}
			}

	/// <summary>
	/// <para>Event that is raised when the mouse pointer enters
	/// this widget.</para>
	/// </summary>
	public event CrossingEventHandler Enter
			{
				add
				{
					lock(this)
					{
						InputEventHandlers handlers = GetHandlers();
						SelectInput(EventMask.EnterWindowMask);
						handlers.enter += value;
					}
				}
				remove
				{
					lock(this)
					{
						if(handlers != null)
						{
							handlers.enter -= value;
							if(handlers.enter == null)
							{
								DeselectInput(EventMask.EnterWindowMask);
							}
						}
					}
				}
			}

	/// <summary>
	/// <para>Event that is raised when the mouse pointer leaves
	/// this widget.</para>
	/// </summary>
	public event CrossingEventHandler Leave
			{
				add
				{
					lock(this)
					{
						InputEventHandlers handlers = GetHandlers();
						SelectInput(EventMask.LeaveWindowMask);
						handlers.leave += value;
					}
				}
				remove
				{
					lock(this)
					{
						if(handlers != null)
						{
							handlers.leave -= value;
							if(handlers.leave == null)
							{
								DeselectInput(EventMask.LeaveWindowMask);
							}
						}
					}
				}
			}

	/// <summary>
	/// <para>Event that is raised when the keyboard focus enters
	/// this widget.</para>
	/// </summary>
	public event FocusChangeEventHandler FocusIn
			{
				add
				{
					lock(this)
					{
						InputEventHandlers handlers = GetHandlers();
						handlers.focusIn += value;
					}
				}
				remove
				{
					lock(this)
					{
						if(handlers != null)
						{
							handlers.focusIn -= value;
						}
					}
				}
			}

	/// <summary>
	/// <para>Event that is raised when the keyboard focus leaves
	/// this widget.</para>
	/// </summary>
	public event FocusChangeEventHandler FocusOut
			{
				add
				{
					lock(this)
					{
						InputEventHandlers handlers = GetHandlers();
						handlers.focusOut += value;
					}
				}
				remove
				{
					lock(this)
					{
						if(handlers != null)
						{
							handlers.focusOut -= value;
						}
					}
				}
			}

	/// <summary>
	/// <para>Request that the keyboard focus be assigned to this widget.
	/// </para>
	/// </summary>
	///
	/// <remarks>
	/// <para>If the top-level window has the primary focus, then this widget
	/// will receive a <c>FocusIn</c> event immediately.  Otherwise,
	/// this widget will receive a <c>FocusIn</c> event the next time
	/// the top-level window obtains the primary focus.</para>
	///
	/// <para>If the top-level window does not currently have the primary
	/// focus, and its <c>DefaultFocus</c> is set to something other than
	/// this widget, then this widget may never receive the focus as a result
	/// of calling this method.</para>
	/// </remarks>
	public void RequestFocus()
			{
				TopLevelWindow topLevel = TopLevel;
				if(topLevel != null)
				{
					topLevel.SetFocus(this);
				}
			}

	// Dispatch an event to this widget.
	internal override void DispatchEvent(ref XEvent xevent)
			{
				lock(this)
				{
					if(handlers != null)
					{
						handlers.DispatchEvent(this, ref xevent);
					}
				}
			}

	// Dispatch a key event to this widget from the top-level window.
	internal bool DispatchKeyEvent(KeyName key, ModifierMask modifiers,
								   String str)
			{
				lock(this)
				{
					if(FullSensitive && handlers != null &&
					   handlers.keyPress != null)
					{
						if(handlers.keyPress(this, key, modifiers, str))
						{
							return true;
						}
					}
					return false;
				}
			}

	// Dispatch a focus in event to this widget from the top-level window.
	internal void DispatchFocusIn(Widget oldWidget)
			{
				lock(this)
				{
					if(handlers != null && handlers.focusIn != null)
					{
						handlers.focusIn(this, oldWidget);
					}
				}
			}

	// Dispatch a focus out event to this widget from the top-level window.
	internal void DispatchFocusOut(Widget oldWidget)
			{
				lock(this)
				{
					if(handlers != null && handlers.focusOut != null)
					{
						handlers.focusOut(this, oldWidget);
					}
				}
			}

	// Input event handling class.  This is created only if the
	// user selects for input events, to avoid wasting memory if
	// no button/key events are desired on a particular widget.
	private class InputEventHandlers
	{
		public ButtonPressEventHandler press;
		public ButtonReleaseEventHandler release;
		public ButtonPressEventHandler doubleClick;
		public ButtonPressEventHandler selectPress;
		public ButtonReleaseEventHandler selectRelease;
		public ButtonPressEventHandler selectDoubleClick;
		public ButtonPressEventHandler menuPress;
		public ButtonReleaseEventHandler menuRelease;
		public ButtonPressEventHandler menuDoubleClick;
		public PointerMotionEventHandler motion;
		public Xlib.Time lastClickTime;
		public ButtonName lastClickButton;
		public KeyPressEventHandler keyPress;
		public CrossingEventHandler enter;
		public CrossingEventHandler leave;
		public FocusChangeEventHandler focusIn;
		public FocusChangeEventHandler focusOut;

		// Dispatch an event to the widget that owns this handler list.
		public void DispatchEvent(Widget widget, ref XEvent xevent)
				{
					ButtonName button;
					Xlib.Time time;
	
					switch(xevent.type)
					{
						case EventType.ButtonPress:
						{
							// Process generic button events.
							button = xevent.xbutton.button;
							time = xevent.xbutton.time;
							if(doubleClick != null &&
							   lastClickButton == button &&
							   lastClickTime != Xlib.Time.CurrentTime &&
							   (time - lastClickTime) < 500)
							{
								doubleClick(widget, xevent.xbutton.x,
									        xevent.xbutton.y, button,
									        xevent.xbutton.state);
								time = Xlib.Time.CurrentTime;
							}
							else if(press != null)
							{
								press(widget, xevent.xbutton.x,
									  xevent.xbutton.y, button,
									  xevent.xbutton.state);
							}
	
							// Process button events for specific buttons.
							if(button == widget.dpy.selectButton)
							{
								if(selectDoubleClick != null &&
								   lastClickButton == button &&
								   lastClickTime != Xlib.Time.CurrentTime &&
								   (time - lastClickTime) < 500)
								{
									selectDoubleClick(widget, xevent.xbutton.x,
										        	  xevent.xbutton.y, button,
										        	  xevent.xbutton.state);
									time = Xlib.Time.CurrentTime;
								}
								else if(selectPress != null)
								{
									selectPress(widget, xevent.xbutton.x,
										  	    xevent.xbutton.y, button,
										  	    xevent.xbutton.state);
								}
							}
							else if(button == widget.dpy.menuButton)
							{
								if(menuDoubleClick != null &&
								   lastClickButton == button &&
								   lastClickTime != Xlib.Time.CurrentTime &&
								   (time - lastClickTime) < 500)
								{
									menuDoubleClick(widget, xevent.xbutton.x,
										        	xevent.xbutton.y, button,
										        	xevent.xbutton.state);
									time = Xlib.Time.CurrentTime;
								}
								else if(menuPress != null)
								{
									menuPress(widget, xevent.xbutton.x,
										  	  xevent.xbutton.y, button,
										  	  xevent.xbutton.state);
								}
							}
							lastClickTime = time;
							lastClickButton = button;
						}
						break;
	
						case EventType.ButtonRelease:
						{
							// Dispatch a button release event.
							button = xevent.xbutton.button;
							if(release != null)
							{
								release(widget, xevent.xbutton.x,
										xevent.xbutton.y, button,
										xevent.xbutton.state);
							}
							if(button == widget.dpy.selectButton &&
							   selectRelease != null)
							{
								selectRelease(widget, xevent.xbutton.x,
									  	      xevent.xbutton.y, button,
									  	      xevent.xbutton.state);
							}
							else if(button == widget.dpy.menuButton &&
									menuRelease != null)
							{
								menuRelease(widget, xevent.xbutton.x,
									  	    xevent.xbutton.y, button,
									  	    xevent.xbutton.state);
							}
						}
						break;

						case EventType.MotionNotify:
						{
							// Dispatch a pointer motion event.
							if(motion != null)
							{
								motion(widget, xevent.xmotion.x,
									   xevent.xmotion.y,
									   xevent.xmotion.state);
							}
						}
						break;

						case EventType.EnterNotify:
						{
							// Dispatch a widget enter event.
							if(enter != null)
							{
								Widget child = (Widget)(widget.dpy.handleMap
									[(int)(xevent.xcrossing.subwindow)]);
								enter(widget, child,
									  xevent.xcrossing.x,
									  xevent.xcrossing.y,
									  xevent.xcrossing.state,
									  xevent.xcrossing.mode,
									  xevent.xcrossing.detail);
							}
						}
						break;

						case EventType.LeaveNotify:
						{
							// Dispatch a widget leave event.
							if(leave != null)
							{
								Widget child = (Widget)(widget.dpy.handleMap
									[(int)(xevent.xcrossing.subwindow)]);
								leave(widget, child,
									  xevent.xcrossing.x,
									  xevent.xcrossing.y,
									  xevent.xcrossing.state,
									  xevent.xcrossing.mode,
									  xevent.xcrossing.detail);
							}
						}
						break;
					}
				}

		// Deselect ButtonPress events if nothing else is interested in them.
		public void DeselectButtonPress(Widget widget)
				{
					if(((Object)press) == null &&
					   ((Object)doubleClick) == null &&
					   ((Object)selectPress) == null &&
					   ((Object)selectDoubleClick) == null &&
					   ((Object)menuPress) == null &&
					   ((Object)menuDoubleClick) == null)
					{
						widget.DeselectInput(EventMask.ButtonPressMask);
					}
				}

		// Deselect ButtonRelease events if nothing else is interested in them.
		public void DeselectButtonRelease(Widget widget)
				{
					if(((Object)release) == null &&
					   ((Object)selectRelease) == null &&
					   ((Object)menuRelease) == null)
					{
						widget.DeselectInput(EventMask.ButtonReleaseMask);
					}
				}

	} // class InputEventHandlers

} // class InputOnlyWidget

} // namespace Xsharp
