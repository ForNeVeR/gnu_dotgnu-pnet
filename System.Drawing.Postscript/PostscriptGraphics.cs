/*
 * PostscriptGraphics.cs - Implementation of the
 *			"System.Drawing.Toolkit.PostscriptGraphics" class.
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

using System.IO;
using System.Drawing;
using System.Drawing.Text;
using System.Drawing.Drawing2D;

internal class PostscriptGraphics : ToolkitGraphicsBase
{
	// Internal state.
	internal TextWriter writer;
	private PostscriptPrintSession session;
	internal IToolkitSelectObject selectObject;

	// Constructor.
	public PostscriptGraphics(IToolkit toolkit, TextWriter writer,
							  PostscriptPrintSession session)
			: base(toolkit)
			{
				this.writer = writer;
				this.session = session;
				this.selectObject = null;
			}

	// Get or set the graphics object's properties.
	public override float DpiX
			{
				get
				{
					return 300.0f;
				}
			}
	public override float DpiY
			{
				get
				{
					return 300.0f;
				}
			}

	// Clear the entire drawing surface.
	public override void Clear(Color color)
			{
				// We assume that the page is already "clear" when
				// we start drawing it, so nothing to do here.
			}

	// Draw a line between two points using the current pen.
	public override void DrawLine(int x1, int y1, int x2, int y2)
			{
				writer.WriteLine("{0} {1} moveto {2} {3} lineto stroke",
								 x1, y1, x2, y2);
			}

	// Draw a set of connected line seguments using the current pen.
	public override void DrawLines(Point[] points)
			{
				int index;
				writer.Write("{0} {1} moveto ", points[0].X, points[0].Y);
				for(index = 1; index < points.Length; ++index)
				{
					writer.Write("{0} {1} lineto ",
								 points[index].X, points[index].Y);
				}
				writer.WriteLine("stroke");
			}

	// Draw a polygon using the current pen.
	public override void DrawPolygon(Point[] points)
			{
				int index;
				writer.Write("{0} {1} moveto ", points[0].X, points[0].Y);
				for(index = 1; index < points.Length; ++index)
				{
					writer.Write("{0} {1} lineto ",
								 points[index].X, points[index].Y);
				}
				writer.WriteLine("closepath stroke");
			}

	// Fill a polygon using the current brush.
	public override void FillPolygon(Point[] points, FillMode fillMode)
			{
				int index;
				writer.Write("{0} {1} moveto ", points[0].X, points[0].Y);
				for(index = 1; index < points.Length; ++index)
				{
					writer.Write("{0} {1} lineto ",
								 points[index].X, points[index].Y);
				}
				writer.WriteLine("closepath fill");
			}

	// Draw an arc within a rectangle defined by four points.
	public override void DrawArc
				(Point[] rect, float startAngle, float sweepAngle)
			{
				// TODO
			}

	// Draw a pie slice within a rectangle defined by four points.
	public override void DrawPie
				(Point[] rect, float startAngle, float sweepAngle)
			{
				// TODO
			}

	// Fill a pie slice within a rectangle defined by four points.
	public override void FillPie
				(Point[] rect, float startAngle, float sweepAngle)
			{
				// TODO
			}

	// Draw a string using the current font and brush.
	public override void DrawString
				(String s, int x, int y, StringFormat format)
			{
				// TODO
			}

	// Draw a string using the current font and brush within a
	// layout rectangle that is defined by four points.
	public override void DrawString
				(String s, Point[] layoutRectangle, StringFormat format)
			{
				// TODO
			}

	// Measure a string using the current font and a given layout rectangle.
	public override Size MeasureString
				(String s, Point[] layoutRectangle,
				 StringFormat format, out int charactersFitted,
				 out int linesFilled, bool ascentOnly)
			{
				// TODO
				charactersFitted = 0;
				linesFilled = 0;
				ascentOnly = false;
				return new Size(0, 0);
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
	public override void SetClipRects(Rectangle[] rects)
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
				// TODO
				return 0;
			}

	// Draw an image at the coordinates
	public override void DrawImage(IToolkitImage image, int x, int y)
			{
				// TODO
			}

}; // class PostscriptGraphics

}; // namespace System.Drawing.Toolkit
