/*
 * DrawingWindow.cs - Implementation of windows for System.Drawing.
 *
 * Copyright (C) 2003  Southern Storm Software, Pty Ltd.
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
using Xsharp;

internal sealed class DrawingWindow : InputOutputWidget, IToolkitWindow
{
	// Internal state.
	private IToolkit toolkit;
	private IToolkitEventSink sink;

	// Constructor.
	public DrawingWindow(IToolkit toolkit, Widget parent,
						 int x, int y, int width, int height)
			: base(parent, x, y, width, height)
			{
				this.toolkit = toolkit;
				this.AutoMapChildren = false;
			}

	// Get the toolkit that owns this window.
	public IToolkit Toolkit
			{
				get
				{
					return toolkit;
				}
			}

	// Get the toolkit parent window.
	IToolkitWindow IToolkitWindow.Parent
			{
				get
				{
					return (IToolkitWindow)Parent;
				}
			}

	// Get the current dimensions of this window.
	public System.Drawing.Rectangle Dimensions
			{
				get
				{
					return new System.Drawing.Rectangle(X, Y, Width, Height);
				}
			}

	// Get or set the mapped state of the window.
	bool IToolkitWindow.IsMapped
			{
				get
				{
					return IsMapped;
				}
				set
				{
					IsMapped = value;
				}
			}

	// Determine if this window currently has the input focus.
	bool IToolkitWindow.Focused
			{
				get
				{
					return Focused;
				}
			}

	// Destroy this window and all of its children.
	void IToolkitWindow.Destroy()
			{
				Destroy();
			}

	// Move or resize this window.
	void IToolkitWindow.MoveResize(int x, int y, int width, int height)
			{
				DrawingToolkit.ValidateWindowPosition(ref x, ref y);
				DrawingToolkit.ValidateWindowSize(ref width, ref height);
				Move(x, y);
				Resize(width, height);
			}

	// Raise this window respective to its siblings.
	void IToolkitWindow.Raise()
			{
				Raise();
			}

	// Lower this window respective to its siblings.
	void IToolkitWindow.Lower()
			{
				Lower();
			}

	// Iconify the window.
	void IToolkitWindow.Iconify()
			{
				// Not used for ordinary windows.
			}

	// Reparent this window to underneath a new parent.
	void IToolkitWindow.Reparent(IToolkitWindow parent, int x, int y)
			{
				if(parent == null)
				{
					Reparent(((DrawingToolkit)Toolkit).placeholder, x, y);
				}
				else
				{
					Reparent((Widget)parent, x, y);
				}
			}

	// Get a toolkit graphics object for this window.
	IToolkitGraphics IToolkitWindow.GetGraphics()
			{
				return new DrawingGraphics
					(toolkit, new Xsharp.Graphics(this));
			}

	// Set the window title (top-level windows only).
	void IToolkitWindow.SetTitle(String title)
			{
				// Not used for ordinary windows.
			}

	// Set the foreground of the window to a solid color.
	void IToolkitWindow.SetForeground(System.Drawing.Color color)
			{
				Foreground = DrawingToolkit.DrawingToXColor(color);
			}

	// Set the background of the window to a solid color.
	void IToolkitWindow.SetBackground(System.Drawing.Color color)
			{
				Background = DrawingToolkit.DrawingToXColor(color);
			}

	// Change the set of supported window decorations and functions.
	void IToolkitWindow.SetWindowFlags(ToolkitWindowFlags flags)
			{
				// Not used for ordinary windows.
			}

	// Move this window to above one of its siblings.
	void IToolkitWindow.MoveToAbove(IToolkitWindow sibling)
			{
				// TODO
			}

	// Move this window to below one of its siblings.
	void IToolkitWindow.MoveToBelow(IToolkitWindow sibling)
			{
				// TODO
			}

	// Get the HWND for this window.  IntPtr.Zero if not supported.
	IntPtr IToolkitWindow.GetHwnd()
			{
				return IntPtr.Zero;
			}

	// Invalidate this window.
	void IToolkitWindow.Invalidate()
			{
				Repaint();
			}

	// Invalidate a rectangle within this window.
	void IToolkitWindow.Invalidate(int x, int y, int width, int height)
			{
				DrawingToolkit.ValidateWindowPosition(ref x, ref y);
				DrawingToolkit.ValidateWindowSize(ref width, ref height);
				Repaint();
			}

	// Force an update of all invalidated regions.
	void IToolkitWindow.Update()
			{
				// TODO
			}

	// Set the event sink to use for this window.
	void IToolkitWindow.SetEventSink(IToolkitEventSink sink)
			{
				this.sink = sink;
			}

	// Map an Xsharp key description into a "ToolkitKeys" value.
	internal static ToolkitKeys MapKey(KeyName key, ModifierMask modifiers)
			{
				// TODO
				return ToolkitKeys.None;
			}

	// Map an Xsharp button name into a "ToolkitMouseButtons" value.
	internal static ToolkitMouseButtons MapButton(ButtonName button)
			{
				switch(button)
				{
					case ButtonName.Button1:
						return ToolkitMouseButtons.Left;
					case ButtonName.Button2:
						return ToolkitMouseButtons.Middle;
					case ButtonName.Button3:
						return ToolkitMouseButtons.Right;
					case ButtonName.Button4:
						return ToolkitMouseButtons.XButton1;
					case ButtonName.Button5:
						return ToolkitMouseButtons.XButton2;
					default:
						return ToolkitMouseButtons.None;
				}
			}

	// Override the button press event from Xsharp.
	protected override void OnButtonPress(int x, int y, ButtonName button,
									      ModifierMask modifiers)
			{
				if(sink != null)
				{
					sink.ToolkitMouseDown
						(MapButton(button),
						 MapKey(KeyName.XK_VoidSymbol, modifiers),
						 1, x, y, 0);
				}
			}

	// Override the button release event from Xsharp.
	protected override void OnButtonRelease(int x, int y, ButtonName button,
									  	    ModifierMask modifiers)
			{
				if(sink != null)
				{
					sink.ToolkitMouseUp
						(MapButton(button),
						 MapKey(KeyName.XK_VoidSymbol, modifiers),
						 1, x, y, 0);
				}
			}

	// Override the button double click event from Xsharp.
	protected override void OnButtonDoubleClick
				(int x, int y, ButtonName button, ModifierMask modifiers)
			{
				if(sink != null)
				{
					sink.ToolkitMouseDown
						(MapButton(button),
						 MapKey(KeyName.XK_VoidSymbol, modifiers),
						 2, x, y, 0);
				}
			}

	// Override the pointer motion event from Xsharp.
	protected override void OnPointerMotion
				(int x, int y, ModifierMask modifiers)
			{
				if(sink != null)
				{
					sink.ToolkitMouseMove
						(ToolkitMouseButtons.None,
						 MapKey(KeyName.XK_VoidSymbol, modifiers),
						 0, x, y, 0);
				}
			}

	// Override the key press event from Xsharp.
	protected override bool OnKeyPress(KeyName key,
									   ModifierMask modifiers, String str)
			{
				if(sink != null)
				{
					// Emit the "KeyDown" event.
					ToolkitKeys keyData = MapKey(key, modifiers);
					if(keyData != ToolkitKeys.None)
					{
						sink.ToolkitKeyDown(keyData);
					}

					// Emit the "KeyChar" event if necessary.
					if(str != null)
					{
						foreach(char ch in str)
						{
							sink.ToolkitKeyChar(ch);
						}
					}
				}

				// The key has been processed.
				return true;
			}

	// Override the mouse enter event from Xsharp.
	protected override void OnEnter(Widget child, int x, int y,
								    ModifierMask modifiers,
								    CrossingMode mode,
								    CrossingDetail detail)
			{
				if(sink != null)
				{
					sink.ToolkitMouseEnter();
				}
			}

	// Override the mouse leave event from Xsharp.
	protected override void OnLeave(Widget child, int x, int y,
								    ModifierMask modifiers,
								    CrossingMode mode,
								    CrossingDetail detail)
			{
				if(sink != null)
				{
					sink.ToolkitMouseLeave();
				}
			}

	// Override the focus enter event from Xsharp.
	protected override void OnFocusIn(Widget other)
			{
				if(sink != null)
				{
					sink.ToolkitFocusEnter();
				}
			}

	// Override the focus leave event from Xsharp.
	protected override void OnFocusOut(Widget other)
			{
				if(sink != null)
				{
					sink.ToolkitFocusLeave();
				}
			}

	// Handle a paint event from Xsharp.
	protected override void OnPaint(Xsharp.Graphics graphics)
			{
				if(sink != null)
				{
					DrawingGraphics g = new DrawingGraphics(toolkit, graphics);
					System.Drawing.Graphics gr =
						ToolkitManager.CreateGraphics(g);
					sink.ToolkitExpose(gr);
					gr.Dispose();
				}
			}

}; // class DrawingWindow

}; // namespace System.Drawing.Toolkit
