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
	public override void Clear(System.Drawing.Color color)
			{
				// TODO
				graphics.Clear();
			}

	// Draw a line between two points using the current pen.
	public override void DrawLine(int x1, int y1, int x2, int y2)
			{
				graphics.DrawLine(x1, y1, x2, y2);
				graphics.DrawPoint(x2, y2);
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
					newPoints[posn].x = RestrictXY(points[posn].X);
					newPoints[posn].y = RestrictXY(points[posn].Y);
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
				int x = RestrictXY(rect[0].X);
				int y = RestrictXY(rect[0].Y);
				int width = RestrictXY(rect[1].X) - x;
				int height = RestrictXY(rect[2].Y) - y;
				graphics.DrawArc(x, y, width, height,
								 -startAngle, -sweepAngle);
			}

	// Draw a pie slice within a rectangle defined by four points.
	public override void DrawPie
				(System.Drawing.Point[] rect,
				 float startAngle, float sweepAngle)
			{
				// Slight bug: this won't work for rotated arcs.
				int x = RestrictXY(rect[0].X);
				int y = RestrictXY(rect[0].Y);
				int width = RestrictXY(rect[1].X) - x;
				int height = RestrictXY(rect[2].Y) - y;
				graphics.DrawPie(x, y, width, height,
								 -startAngle, -sweepAngle);
			}

	// Fill a pie slice within a rectangle defined by four points.
	public override void FillPie
				(System.Drawing.Point[] rect,
				 float startAngle, float sweepAngle)
			{
				// Slight bug: this won't work for rotated arcs.
				int x = RestrictXY(rect[0].X);
				int y = RestrictXY(rect[0].Y);
				int width = RestrictXY(rect[1].X) - x;
				int height = RestrictXY(rect[2].Y) - y;
				graphics.ArcMode = ArcMode.ArcPieSlice;
				graphics.FillArc(x, y, width, height,
								 -startAngle, -sweepAngle);
			}

	// Draw a string using the current font and brush.
	public override void DrawString
				(String s, int x, int y, StringFormat format)
			{
				FontExtents extents = graphics.GetFontExtents(font);
				graphics.DrawString(RestrictXY(x), RestrictXY(y) + extents.Ascent, s, font);
			}

	// Measure a string using the current font and a given layout rectangle.
	public override Size MeasureString
				(String s, System.Drawing.Point[] layoutRectangle,
				 StringFormat format, out int charactersFitted,
				 out int linesFilled, bool ascentOnly)
			{
				// TODO: line wrapping, etc
				int width, ascent, descent;
				charactersFitted = 0;
				linesFilled = 0;
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
				using (Xsharp.Region region = new Xsharp.Region())
				{
					graphics.SetClipRegion( region, 0, 0);
				}
			}

	// Set the clipping region to infinite (i.e. disable clipping).
	public override void SetClipInfinite()
			{
				using (Xsharp.Region region = new Xsharp.Region(short.MinValue, short.MinValue, ushort.MaxValue, ushort.MaxValue))
				{
					graphics.SetClipRegion( region, 0, 0);
				}
			}

	// Set the clipping region to a single rectangle.
	public override void SetClipRect(int x, int y, int width, int height)
			{
				using (Xsharp.Region region = new Xsharp.Region(x, y, width, height))
				{
					graphics.SetClipRegion( region, 0, 0);
				}
			}

	// Set the clipping region to a list of rectangles.
	public override void SetClipRects(System.Drawing.Rectangle[] rects)
			{
				using (Xsharp.Region region = rectsToRegion(rects))
				{
					graphics.SetClipRegion( region);
				}
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

	// Convert a System.Drawing.Region to Xsharp.Region
	private static Xsharp.Region rectsToRegion( System.Drawing.Rectangle[] rectangles)
		{
			Xsharp.Region newRegion =  new Xsharp.Region();
			for( int i = 0; i < rectangles.Length; i++)
			{
				System.Drawing.Rectangle rect = rectangles[i];
				// This implementation has a region size restriction.
				int left = RestrictXY(rect.Left);
				int top = RestrictXY(rect.Top);
				int right = RestrictXY(rect.Right);
				int bottom = RestrictXY(rect.Bottom);
				newRegion.Union( left, top, right - left, bottom - top);
			}
			return newRegion;
		}

	// Make sure a x or y fits within the X drawing position restriction
	// Because internally coordinates are represented by shorts, the x, y
	// position plus the window coordinates cant exceed this. So we make
	// the |min|, |max| doesnt exceed short/2.
	private static int RestrictXY(int value)
	{
		if (value < short.MinValue/2)
			value = short.MinValue/2;
		else if (value > short.MaxValue/2)
			value = short.MaxValue/2;
		return value;
	}

	public override void DrawImage(IToolkitImage image, int x, int y)
	{
		graphics.DrawImage(x, y, (image as DrawingImage).GetNativeImage());
	}

}; // class DrawingGraphics

}; // namespace System.Drawing.Toolkit
