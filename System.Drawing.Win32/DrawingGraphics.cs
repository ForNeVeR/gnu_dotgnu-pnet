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

internal class DrawingGraphics : ToolkitGraphicsBase, IDisposable
{
	// Internal state.
	//internal Xsharp.Font font;
	internal IntPtr hdc;
	internal DrawingPen selectedPen;
	internal DrawingBrush selectedBrush;
	internal DrawingFont selectedFont;

	public DrawingGraphics(IToolkit toolkit, IntPtr hdc)
			: base(toolkit)
			{
				this.hdc = hdc;
			}


	// Dispose of this object.
	public override void Dispose()
			{
				DeleteDC();
			}

	// Clear the entire drawing surface.
	public override void Clear(Color color)
			{
				//graphics.Clear();
			}

	// Draw a line between two points using the current pen.
	public override void DrawLine( int x1, int y1, int x2, int y2 )
			{
				IntPtr prevhPen = Win32.Api.SelectObject( hdc, selectedPen.hPen );
				Win32.Api.MoveToEx( hdc, x1, y1,IntPtr.Zero );
				Win32.Api.LineTo( hdc, x2, y2 );
				//set last point
				Win32.Api.SetPixel( hdc, x2, y2, ColorToWin32( selectedPen.properties.Color ) );
				Win32.Api.SelectObject( hdc, prevhPen );
				
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
				Polygon( points, Win32.Api.GetStockObject(Win32.Api.StockObjectType.HOLLOW_BRUSH), selectedPen.hPen);
			}

	// Fill a polygon using the current brush.
	public override void FillPolygon( System.Drawing.Point[] points, FillMode fillMode )
			{
				//TODO: This is a hack. should use regions rather because Poly doesnt use the brush for the border
				ExpandFilledObject(ref points);
				Win32.Api.SetPolyFillMode(hdc, (int)fillMode);
				Polygon( points, selectedBrush.hBrush, Win32.Api.GetStockObject(Win32.Api.StockObjectType.NULL_PEN) );
			}

	// Expand the outside away from the first point of a filled object by 1
	private void ExpandFilledObject( ref System.Drawing.Point[] points)
			{
				Point first = points[0];
				for(int posn = 0; posn < points.Length; posn++)
				{
					int x = points[posn].X;
					int y = points[posn].Y;
					//Move the points 1 away from the first point
					if (x!=first.X)
						x += x > first.X ? 1 : -1;
					if (y!=first.Y)
						y += y > first.Y ? 1 : -1;

					points[posn] = new Point(x, y);
				}
			}

	private void Polygon( System.Drawing.Point[] points, IntPtr hBrush, IntPtr hPen )
			{
				IntPtr prevhPen = Win32.Api.SelectObject( hdc, hPen );
				IntPtr prevhBrush = Win32.Api.SelectObject( hdc, hBrush );
				Win32.Api.POINT[] wPoints = ConvertPoints(points);
				Win32.Api.Polygon(hdc, wPoints, wPoints.Length);
				Win32.Api.SelectObject( hdc, prevhBrush );
				Win32.Api.SelectObject( hdc, prevhPen );
			
			}

	// Draw an arc within a rectangle defined by four points.
	public override void DrawArc( System.Drawing.Point[] rect, float startAngle, float sweepAngle )
			{
				IntPtr prevhPen = Win32.Api.SelectObject( hdc, selectedPen.hPen );
				Rectangle intersect = EllipseIntersect( rect, startAngle, sweepAngle );
				Win32.Api.Arc( hdc, rect[0].X, rect[0].Y, rect[2].X, rect[2].Y, intersect.Left, intersect.Top, intersect.Right, intersect.Bottom );
				Win32.Api.SelectObject( hdc, prevhPen );

			}

	// Draw a pie slice within a rectangle defined by four points.
	public override void DrawPie ( System.Drawing.Point[] rect, float startAngle, float sweepAngle )
			{
				
				Pie( rect, startAngle, sweepAngle, Win32.Api.GetStockObject( Win32.Api.StockObjectType.HOLLOW_BRUSH ), selectedPen.hPen );
			}

	// Fill a pie slice within a rectangle defined by four points.
	public override void FillPie ( System.Drawing.Point[] rect, float startAngle, float sweepAngle )
			{
				Pie( rect, startAngle, sweepAngle, selectedBrush.hBrush, /*Win32.Api.GetStockObject( Win32.Api.StockObjectType.NULL_PEN)*/IntPtr.Zero );
			}

	private void Pie ( System.Drawing.Point[] rect, float startAngle, float sweepAngle, IntPtr hBrush, IntPtr hPen )
			{
				IntPtr prevBrush = Win32.Api.SelectObject( hdc, hBrush );
				IntPtr prevPen = Win32.Api.SelectObject( hdc, hPen );
				Rectangle intersect = EllipseIntersect( rect, startAngle, sweepAngle );
						
				Win32.Api.Pie( hdc, rect[0].X, rect[0].Y, rect[2].X, rect[2].Y, intersect.Left, intersect.Top, intersect.Right, intersect.Bottom );
				Win32.Api.SelectObject( hdc, prevPen );
				Win32.Api.SelectObject( hdc, prevBrush );

			}

	//Top left of return rectangle is the one intersect, bottom right is the other.
	private Rectangle EllipseIntersect( System.Drawing.Point[] rect, float startAngle, float sweepAngle )
			{
				double centerX = (rect[0].X+rect[2].X)/2;
				double centerY = (rect[0].Y+rect[2].Y)/2;
				double theta1 = (startAngle+sweepAngle)*Math.PI/180;
				double theta2 = startAngle*Math.PI/180;
				double a = (rect[2].X - rect[0].X)/2;
				double b = (rect[2].Y - rect[0].Y)/2;
				double r1 = a*b/Math.Sqrt(Math.Pow(b*Math.Cos(theta1), 2)+Math.Pow(a*Math.Sin(theta1), 2));
				double r2 = a*b/Math.Sqrt(Math.Pow(b*Math.Cos(theta2), 2)+Math.Pow(a*Math.Sin(theta2), 2));
				int p1X = (int)(r1 * Math.Cos(theta1) + 0.5 + centerX);
				int p1Y = (int)(r1 * Math.Sin(theta1) + 0.5 + centerY);
				int p2X = (int)(r2 * Math.Cos(theta2) + 0.5 + centerX);
				int p2Y = (int)(r2 * Math.Sin(theta2) + 0.5 + centerY);
				return new Rectangle(p1X, p1Y, p2X - p1X, p2Y - p1Y);
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

				Win32.Api.SelectObject(hdc, selectedFont.hFont);
				Win32.Api.SetTextColor(hdc, ColorToWin32(selectedBrush.color));
				Win32.Api.SetBkMode(hdc,Win32.Api.BackGroundModeType.TRANSPARENT);
				Win32.Api.TextOutA(hdc, x, y , s, s.Length);

			}

	// Measure a string using the current font and a given layout rectangle.
	public override Size MeasureString( String s, System.Drawing.Point[] layoutRectangle, StringFormat format, out int charactersFitted, out int linesFilled, bool ascentOnly )
			{
				// TODO: line wrapping, etc
				Win32.Api.SelectObject(hdc, selectedFont.hFont);
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
					return new Size(size.cx, size.cy - 1); /*ascent + descent*/
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
				DeleteDC();
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
				Win32.Api.TEXTMETRIC lptm;
				Win32.Api.GetTextMetrics(hdc, out lptm);
				return lptm.tmHeight;
			}

	public static int ColorToWin32( Color color) 
			{
				return color.R | color.G<<8 | color.B<<16;
			}

	//Under 95/98 an object(fonts, pens or brushes) cant be disposed when it is select into a dc
	//This leads to a leak. So, if a dc is disposed, select out all the object types so when the objects are actually disposed, they will actually dispose.
	private void DeleteDC()
	{
		Win32.Api.DeleteObject(hdc);
		hdc = IntPtr.Zero;
	}

	internal DrawingPen SelectPen
	{
		set
		{
			selectedPen = value;
		}
	}

	internal DrawingBrush SelectBrush
	{
		set
		{
			selectedBrush = value;
		}
	}

	internal DrawingFont SelectFont
	{
		set
		{
			selectedFont = value;
		}
	}

}; // class DrawingGraphics


}; // namespace System.Drawing.Toolkit
