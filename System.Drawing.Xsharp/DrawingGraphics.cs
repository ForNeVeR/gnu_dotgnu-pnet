/*
 * DrawingGraphics.cs - Implementation of graphics drawing for System.Drawing.
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

internal sealed class DrawingGraphics : ToolkitGraphicsBase
{
	// Internal state.
	internal Xsharp.Graphics graphics;
	internal Xsharp.Font font;

	// Constructor.
	public DrawingGraphics(IToolkit toolkit, Xsharp.Graphics graphics)
			: base(toolkit)
			{
				this.graphics = graphics;
				this.font = null;
			}

	// Dispose of this object.
	public override void Dispose()
			{
				if(graphics != null)
				{
					graphics.Dispose();
					graphics = null;
				}
			}

	// Clear the entire drawing surface.
	public override void Clear()
			{
				graphics.Clear();
			}

	// Draw a line between two points using the current pen.
	public override void DrawLine(int x1, int y1, int x2, int y2)
			{
				graphics.DrawLine(x1, y1, x2, y2);
			}

	// Convert an array of "System.Drawing.Point" objects into an array of
	// "Xsharp.Point" objects.
	private static Xsharp.Point[] ConvertPoints
				(System.Drawing.Point[] points, bool dupFirst)
			{
				Xsharp.Point[] newPoints;
				if(dupFirst)
				{
					newPoints = new Xsharp.Point [points.Length + 1];
				}
				else
				{
					newPoints = new Xsharp.Point [points.Length];
				}
				int posn;
				for(posn = 0; posn < points.Length; ++posn)
				{
					newPoints[posn].x = points[posn].X;
					newPoints[posn].y = points[posn].Y;
				}
				if(dupFirst)
				{
					newPoints[points.Length] = newPoints[0];
				}
				return newPoints;
			}

	// Draw a set of connected line seguments using the current pen.
	public override void DrawLines(System.Drawing.Point[] points)
			{
				graphics.DrawLines(ConvertPoints(points, false));
			}

	// Draw a polygon using the current pen.
	public override void DrawPolygon(System.Drawing.Point[] points)
			{
				graphics.DrawLines(ConvertPoints(points, true));
			}

	// Fill a polygon using the current brush.
	public override void FillPolygon
				(System.Drawing.Point[] points, FillMode fillMode)
			{
				if(fillMode == FillMode.Alternate)
				{
					graphics.FillRule = FillRule.EvenOddRule;
				}
				else
				{
					graphics.FillRule = FillRule.WindingRule;
				}
				graphics.FillPolygon(ConvertPoints(points, false));
			}

	// Draw an arc within a rectangle defined by four points.
	public override void DrawArc
				(System.Drawing.Point[] rect,
				 float startAngle, float sweepAngle)
			{
				// Slight bug: this won't work for rotated arcs.
				int width = rect[1].X - rect[0].X;
				int height = rect[2].Y - rect[0].Y;
				graphics.DrawArc(rect[0].X, rect[0].Y, width, height,
								 startAngle, sweepAngle);
			}

	// Draw a pie slice within a rectangle defined by four points.
	public override void DrawPie
				(System.Drawing.Point[] rect,
				 float startAngle, float sweepAngle)
			{
				// Slight bug: this won't work for rotated arcs.
				int width = rect[1].X - rect[0].X;
				int height = rect[2].Y - rect[0].Y;
				graphics.DrawPie(rect[0].X, rect[0].Y, width, height,
								 startAngle, sweepAngle);
			}

	// Fill a pie slice within a rectangle defined by four points.
	public override void FillPie
				(System.Drawing.Point[] rect,
				 float startAngle, float sweepAngle)
			{
				// Slight bug: this won't work for rotated arcs.
				int width = rect[1].X - rect[0].X;
				int height = rect[2].Y - rect[0].Y;
				graphics.ArcMode = ArcMode.ArcPieSlice;
				graphics.FillArc(rect[0].X, rect[0].Y, width, height,
								 startAngle, sweepAngle);
			}

	// Draw a string using the current font and brush.
	public override void DrawString
				(String s, int x, int y, StringFormat format)
			{
				FontExtents extents = graphics.GetFontExtents(font);
				graphics.DrawString(x, y + extents.Ascent - 1, s, font);
			}

	// Measure a string using the current font and a given layout rectangle.
	public override Size MeasureString
				(String s, System.Drawing.Point[] layoutRectangle,
				 StringFormat format, out int charactersFitted,
				 out int linesFilled, bool ascentOnly)
			{
				// TODO: line wrapping, etc
				int width, ascent, descent;
				graphics.MeasureString
					(s, font, out width, out ascent, out descent);
				if(!ascentOnly)
				{
					return new Size(width, ascent + descent);
				}
				else
				{
					return new Size(width, ascent);
				}
			}

	// Flush the graphics subsystem
	public override void Flush(FlushIntention intention)
			{
				if(intention == FlushIntention.Flush)
				{
					((DrawingToolkit)Toolkit).app.Display.Flush();
				}
				else
				{
					((DrawingToolkit)Toolkit).app.Display.Sync();
				}
			}

	// Get the HDC associated with this graphics object.
	public override IntPtr GetHdc()
			{
				// We don't use HDC's in this implementation.
				return IntPtr.Zero;
			}

	// Release a HDC that was obtained using "GetHdc()".
	public override void ReleaseHdc(IntPtr hdc)
			{
				// We don't use HDC's in this implementation.
			}

	// Set the clipping region to empty.
	public override void SetClipEmpty()
			{
				// TODO
			}

	// Set the clipping region to infinite (i.e. disable clipping).
	public override void SetClipInfinite()
			{
				// TODO
			}

	// Set the clipping region to a single rectangle.
	public override void SetClipRect(int x, int y, int width, int height)
			{
				// TODO
			}

	// Set the clipping region to a list of rectangles.
	public override void SetClipRects(System.Drawing.Rectangle[] rects)
			{
				// TODO
			}

	// Set the clipping region to a complex mask.
	public override void SetClipMask(Object mask, int topx, int topy)
			{
				// TODO
			}

	// Get the line spacing for the font selected into this graphics object.
	public override int GetLineSpacing()
			{
				FontExtents extents = graphics.GetFontExtents(font);
				return extents.Ascent + extents.Descent;
			}

}; // class DrawingGraphics

}; // namespace System.Drawing.Toolkit
