/*
 * DrawingWindowBuffer.cs - Window Double Buffer class.
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
using Xsharp;

internal class DrawingWindowBuffer : IToolkitWindowBuffer, IDisposable
{
	// Internal state.
	private IToolkit toolkit;
	private Widget widget;
	private Pixmap pixmap;
	private DrawingGraphics graphics;

	// Constructor.
	public DrawingWindowBuffer(IToolkitWindow windowToBuffer)
			{
				toolkit = windowToBuffer.Toolkit;
				widget = windowToBuffer as Widget;
				pixmap = null;
				graphics = null;
			}

	// Create the buffer object for the widget.
	private void CreateBuffer(int width, int height)
			{
				DeleteBuffer();
				pixmap = new Pixmap(widget.Screen, width, height);
			}

	// Delete the buffer object.
	private void DeleteBuffer()
			{
				// Make sure that we dispose of the X graphics object
				// before we dispose of the X pixmap.
				if(graphics != null)
				{
					graphics.graphics.Dispose();
				}
				if(pixmap != null)
				{
					pixmap.Dispose();
					pixmap = null;
				}
			}

	// Begin a double buffer operation.
	public IToolkitGraphics BeginDoubleBuffer()
			{
				// Re-create the buffer if the size has changed.
				if(pixmap == null || graphics != null ||
				   pixmap.Width != widget.Width ||
				   pixmap.Height != widget.Height)
				{
					CreateBuffer(widget.Width, widget.Height);
				}

				// Create a graphics object for the pixmap and return it.
				graphics = new DrawingGraphics
					(toolkit, new Xsharp.Graphics(pixmap));
				return graphics;
			}

	// End a double buffer operation, flusing the buffer back to the widget.
	public void EndDoubleBuffer()
			{
				if(graphics != null)
				{
					// Dispose of the IToolkitGraphics object.
					graphics.Dispose();
					graphics = null;

					// Copy the pixmap contents to the widget.
					Xsharp.Graphics gr = new Xsharp.Graphics(widget);
					gr.BitBlt(0, 0, pixmap);
					gr.Dispose();
				}
			}

	// Dispose of this object.
	public void Dispose()
			{
				DeleteBuffer();
			}

}; // class DrawingWindowBuffer

}; // namespace System.Drawing.Toolkit
