/*
 * DrawingPen.cs - Implementation of pens for System.Drawing.
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

internal sealed class DrawingPen : IToolkitPen
{
	// Internal state.
	private Pen properties;

	// Constructor.
	public DrawingPen(Pen properties)
			{
				this.properties = properties;
			}

	// Map the line style from "System.Drawing" to "Xsharp".
	private static LineStyle MapLineStyle(DashStyle style)
			{
				switch(style)
				{
					// TODO: dashed lines
					case DashStyle.Solid:
					case DashStyle.Dash:
					case DashStyle.Dot:
					case DashStyle.DashDot:
					case DashStyle.DashDotDot:
					case DashStyle.Custom:
					default:
						return LineStyle.LineSolid;
				}
			}

	// Map the cap style from "System.Drawing" to "Xsharp".
	private static CapStyle MapCapStyle(DashCap style)
			{
				switch(style)
				{
					case DashCap.Flat:
					case DashCap.Triangle:
					default:
						return CapStyle.CapButt;
					case DashCap.Round:
						return CapStyle.CapRound;
				}
			}

	// Map the join style from "System.Drawing" to "Xsharp".
	private static JoinStyle MapJoinStyle(LineJoin style)
			{
				switch(style)
				{
					case LineJoin.Miter:
					case LineJoin.MiterClipped:
					default:
						return JoinStyle.JoinMiter;
					case LineJoin.Bevel:
						return JoinStyle.JoinBevel;
					case LineJoin.Round:
						return JoinStyle.JoinRound;
				}
			}

	// Select this pen into a graphics object.
	public void Select(IToolkitGraphics _graphics)
			{
				DrawingGraphics graphics = (_graphics as DrawingGraphics);
				if(graphics != null)
				{
					Xsharp.Graphics g = graphics.graphics;
					g.LineWidth = (int)(properties.Width);
					g.LineStyle = MapLineStyle(properties.DashStyle);
					g.CapStyle = MapCapStyle(properties.DashCap);
					g.JoinStyle = MapJoinStyle(properties.LineJoin);
					g.Foreground = DrawingToolkit.DrawingToXColor
						(properties.Color);
					// TODO: dashed line patterns
				}
			}

	// Dispose of this pen.
	public void Dispose()
			{
				// Nothing to do here in this implementation.
			}

}; // class DrawingPen

}; // namespace System.Drawing.Toolkit
