/*
 * DrawingGraphics.cs - Implementation of graphics drawing for System.Drawing.Win32.
 * Copyright (C) 2003  Neil Cawse.
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

internal class DrawingGraphics : ToolkitGraphicsBase, IDisposable
{
	// Internal state.
	//internal Xsharp.Font font;
	internal IntPtr hdc;
	internal DrawingPen selectedPen;
	internal DrawingBrush selectedBrush;

	public DrawingGraphics(IToolkit toolkit, IntPtr hdc)
			: base(toolkit)
			{
				this.hdc = hdc;
				//this.font = null;
			}


	// Dispose of this object.
	public override void Dispose()
			{
				Win32.Api.DeleteObject(hdc);
				hdc = IntPtr.Zero;
			}

	// Clear the entire drawing surface.
	public override void Clear()
			{
				//graphics.Clear();
			}

	// Draw a line between two points using the current pen.
	public override void DrawLine( int x1, int y1, int x2, int y2 )
			{
				Win32.Api.MoveToEx( hdc, x1, y1,IntPtr.Zero );
				Win32.Api.LineTo( hdc, x2, y2 );
				//set last point
				Win32.Api.SetPixel( hdc, x2, y2, ColorToWin32( selectedPen.properties.Color ) );
			}

	// Draw a set of connected line seguments using the current pen.
	public override void DrawLines( System.Drawing.Point[] points )
			{
				for ( int i = 1;i < points.Length; i++ )
				{
					DrawLine( points[i-1].X,points[i-1].Y,points[i].X,points[i].Y );
				}

			}

	private Win32.Api.POINT[] ConvertPoints( System.Drawing.Point[] points ) 
			{
				Win32.Api.POINT[] wPoints = new Win32.Api.POINT[points.Length];
				for( int i = 0; i < points.Length; i++ )
				{
					wPoints[i].x = points[i].X;
					wPoints[i].y = points[i].Y;
				}
				return wPoints;
			}

	// Draw a polygon using the current pen.
	public override void DrawPolygon( System.Drawing.Point[] points )
			{
				IntPtr prevBrush = Win32.Api.SelectObject(hdc, Win32.Api.GetStockObject(Win32.Api.StockObjectType.HOLLOW_BRUSH));
				Win32.Api.POINT[] wPoints = ConvertPoints(points);
				Win32.Api.Polygon(hdc, wPoints, wPoints.Length);
				Win32.Api.SelectObject(hdc, prevBrush);
			}

	// Fill a polygon using the current brush.
	public override void FillPolygon
				( System.Drawing.Point[] points, FillMode fillMode )
			{
				
				Win32.Api.SetPolyFillMode(hdc, (int)fillMode);
				Win32.Api.POINT[] wPoints = ConvertPoints(points);
				Win32.Api.Polygon(hdc, wPoints, wPoints.Length);

			}

	// Draw an arc within a rectangle defined by four points.
	public override void DrawArc
				( System.Drawing.Point[] rect,
				 float startAngle, float sweepAngle )
			{

				int p1X, p1Y, p2X, p2Y;
				RectangleAngleToPoints( rect, startAngle, sweepAngle, out p1X, out p1Y, out p2X, out p2Y );
				Win32.Api.Arc( hdc, rect[0].X, rect[0].Y, rect[2].X, rect[2].Y, p1X, p1Y, p2X, p2Y );

			}

	// Draw a pie slice within a rectangle defined by four points.
	public override void DrawPie ( System.Drawing.Point[] rect, float startAngle, float sweepAngle )
			{

				IntPtr prevBrush = Win32.Api.SelectObject(hdc, Win32.Api.GetStockObject(Win32.Api.StockObjectType.HOLLOW_BRUSH));
				int p1X, p1Y, p2X, p2Y;
				RectangleAngleToPoints( rect, startAngle, sweepAngle, out p1X, out p1Y, out p2X, out p2Y );
				Win32.Api.Pie( hdc, rect[0].X, rect[0].Y, rect[2].X, rect[2].Y, p1X, p1Y, p2X, p2Y);
				Win32.Api.SelectObject( hdc, prevBrush );

			}

	// Fill a pie slice within a rectangle defined by four points.
	public override void FillPie (System.Drawing.Point[] rect, float startAngle, float sweepAngle)
			{

				IntPtr prevPen = Win32.Api.SelectObject( hdc, Win32.Api.GetStockObject( Win32.Api.StockObjectType.NULL_PEN ) );
				int p1X, p1Y, p2X, p2Y;
				RectangleAngleToPoints( rect, startAngle, sweepAngle, out p1X, out p1Y, out p2X, out p2Y );
				Win32.Api.Pie( hdc, rect[0].X, rect[0].Y, rect[2].X, rect[2].Y, p1X, p1Y, p2X, p2Y );
				Win32.Api.SelectObject( hdc, prevPen );

			}

	//Woops this is works only for a square - id better get some trig going
	private void RectangleAngleToPoints(System.Drawing.Point[] rect, float startAngle, float sweepAngle,
		out int p1X, out int p1Y, out int p2X, out int p2Y)
			{

				double centerX = (rect[0].X+rect[2].X)/2;
				double centerY = (rect[0].Y+rect[2].Y)/2;
				double r = Math.Sqrt((Math.Pow(rect[0].X-rect[2].X,2))+Math.Pow(rect[0].Y-rect[2].Y,2))/2;
				double a = startAngle*Math.PI/180;
				double b = (startAngle+sweepAngle)*Math.PI/180;
				p1X = (int)(Math.Cos(b)*r+centerX+.5);
				p1Y = (int)(Math.Sin(b)*r+centerY+.5);
				p2X = (int)(Math.Cos(a)*r+centerX+.5);
				p2Y = (int)(Math.Sin(a)*r+centerY+.5);

			}

	// Draw a string using the current font and brush.
	public override void DrawString
				(String s, int x, int y, StringFormat format)
			{
				//FontExtents extents = graphics.GetFontExtents(font);
				//graphics.DrawString(x, y + extents.Ascent - 1, s, font);

				//GDI does support writing text with a brush so we get the brush color - if available
				/*Could do but is it slow?:
				 * 1) Select hatch brush
				2) BeginPath
				3) TextOut
				4) EndPath
				5) StrokeAndFillPath*/

				Win32.Api.SetTextColor(hdc, ColorToWin32(selectedBrush.color));
				Win32.Api.SetBkMode(hdc,Win32.Api.BackGroundModeType.TRANSPARENT);
				Win32.Api.TextOutA(hdc, x, y , s, s.Length);

			}

	// Measure a string using the current font and a given layout rectangle.
	public override Size MeasureString( String s, System.Drawing.Point[] layoutRectangle, StringFormat format, out int charactersFitted, out int linesFilled, bool ascentOnly )
			{
				// TODO: line wrapping, etc
				Win32.Api.SIZE size;
				size.cx = 0;
				size.cy = 0;
				Win32.Api.GetTextExtentPoint32A(hdc, s, s.Length, ref size);

				Win32.Api.TEXTMETRIC tm;
				Win32.Api.GetTextMetrics(hdc, out tm);
				
				charactersFitted = 0;
				linesFilled = 0;
				if(!ascentOnly)
				{
					return new Size(size.cx, size.cy); /*ascent + descent*/
				}
				else
				{
					return new Size(size.cx,tm.tmAscent);
				}
				
			}

	// Not implementing Flush
	public override void Flush(FlushIntention intention)
			{
			}

	// Get the HDC associated with this graphics object.
	public override IntPtr GetHdc()
			{
				return hdc;
			}

	// Release a HDC that was obtained using "GetHdc()".
	public override void ReleaseHdc(IntPtr hdc)
			{
				Win32.Api.DeleteObject(hdc);
			}

	// Set the clipping region to empty.
	public override void SetClipEmpty()
			{
				//Hope this is right!
				SetClipRect( 0, 0, 0, 0 );
			}

	// Set the clipping region to infinite (i.e. disable clipping).
	public override void SetClipInfinite()
			{
				Win32.Api.SelectClipRgn( hdc, IntPtr.Zero );
			}

	// Set the clipping region to a single rectangle.
	public override void SetClipRect( int x, int y, int width, int height )
			{
				IntPtr region = Win32.Api.CreateRectRgn( x, y, x + width, y + height );
				Win32.Api.SelectClipRgn( hdc, region );
				Win32.Api.DeleteObject( region );
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
				/*FontExtents extents = graphics.GetFontExtents(font);
				return extents.Ascent + extents.Descent;*/
		return 0;
	
			}

	public static int ColorToWin32( Color color) 
			{
				return color.R | color.G<<8 | color.B<<16;
			}

}; // class DrawingGraphics


}; // namespace System.Drawing.Toolkit
