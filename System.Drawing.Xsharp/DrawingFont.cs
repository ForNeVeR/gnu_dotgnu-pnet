/*
 * DrawingFont.cs - Implementation of fonts for System.Drawing.
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

internal sealed class DrawingFont : IToolkitFont
{
	// Internal state.
	private System.Drawing.Font properties;

	// Constructor.
	public DrawingFont(System.Drawing.Font properties)
			{
				this.properties = properties;
			}

	// Select this font into a graphics object.
	public void Select(IToolkitGraphics _graphics)
			{
				DrawingGraphics graphics = (_graphics as DrawingGraphics);
				if(graphics != null)
				{
					// TODO
				}
			}

	// Dispose of this pen.
	public void Dispose()
			{
				// Nothing to do here in this implementation.
			}

}; // class DrawingFont

}; // namespace System.Drawing.Toolkit
