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

	// Destroy this window and all of its children.
	void IToolkitWindow.Destroy()
			{
				Destroy();
			}

	// Move or resize this window.
	void IToolkitWindow.MoveResize(int x, int y, int width, int height)
			{
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
				// TODO
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

	// Set the background of the window to a solid color.
	void IToolkitWindow.SetBackground(System.Drawing.Color color)
			{
				Background = DrawingToolkit.DrawingToXColor(color);
			}

	// Change the set of supported decorations and functions.
	void IToolkitWindow.SetFunctions(ToolkitDecorations decorations,
							 		 ToolkitFunctions functions)
			{
				// Not used for ordinary windows.
			}

	// Handle a paint event from Xsharp.
	protected override void OnPaint(Xsharp.Graphics graphics)
			{
				if(Expose != null)
				{
					DrawingGraphics g = new DrawingGraphics(toolkit, graphics);
					System.Drawing.Graphics gr =
						ToolkitManager.CreateGraphics(g);
					Expose(gr);
					gr.Dispose();
				}
			}

	// Event that is emitted for an expose on this window.
	public event ToolkitExposeHandler Expose;

}; // class DrawingWindow

}; // namespace System.Drawing.Toolkit
