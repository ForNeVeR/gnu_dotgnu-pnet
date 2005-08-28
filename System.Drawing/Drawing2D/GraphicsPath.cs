/*
 * GraphicsPath.cs - Implementation of the
 *			"System.Drawing.Drawing2D.GraphicsPath" class.
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

namespace System.Drawing.Drawing2D
{

public sealed class GraphicsPath : MarshalByRefObject, ICloneable, IDisposable
{
	// Internal state.
	private PathObject first;
	private PathObject last;
	private bool needPenBrush;
	private FillMode fillMode;

	// Convert an integer point array into a float point array.
	private static PointF[] Convert(Point[] pts)
			{
				if(pts == null)
				{
					throw new ArgumentNullException("pts");
				}
				PointF[] fpts = new PointF [pts.Length];
				int posn;
				for(posn = 0; posn < pts.Length; ++posn)
				{
					fpts[posn] = pts[posn];
				}
				return fpts;
			}

	// Convert an integer rectangle array into a float rectangle array.
	private static RectangleF[] Convert(Rectangle[] rects)
			{
				if(rects == null)
				{
					throw new ArgumentNullException("rects");
				}
				RectangleF[] frects = new RectangleF [rects.Length];
				int posn;
				for(posn = 0; posn < rects.Length; ++posn)
				{
					frects[posn] = rects[posn];
				}
				return frects;
			}

	// Constructors.
	public GraphicsPath()
			{
				this.fillMode = FillMode.Alternate;
			}
	public GraphicsPath(FillMode fillMode)
			{
				this.fillMode = fillMode;
			}
	public GraphicsPath(Point[] pts, byte[] types)
			: this(pts, types, FillMode.Alternate) {}
	public GraphicsPath(PointF[] pts, byte[] types)
			: this(pts, types, FillMode.Alternate) {}
	[TODO]
	public GraphicsPath(Point[] pts, byte[] types, FillMode fillMode)
			{
				if(pts == null)
				{
					throw new ArgumentNullException("pts");
				}
				if(types == null)
				{
					throw new ArgumentNullException("types");
				}
				this.fillMode = fillMode;
				// TODO: convert the pts and types arrays
			}
	[TODO]
	public GraphicsPath(PointF[] pts, byte[] types, FillMode fillMode)
			{
				if(pts == null)
				{
					throw new ArgumentNullException("pts");
				}
				if(types == null)
				{
					throw new ArgumentNullException("types");
				}
				this.fillMode = fillMode;
				// TODO: convert the pts and types arrays
			}

	// Destructor.
	~GraphicsPath()
			{
				Dispose(false);
			}

	// Get or set this object's properties.
	public FillMode FillMode
			{
				get
				{
					return fillMode;
				}
				set
				{
					fillMode = value;
				}
			}
	[TODO]
	public PathData PathData
			{
				get
				{
					PathData data = new PathData();
					// TODO
					//data.Points = pts;
					//data.Types = types;
					return data;
				}
			}
	[TODO]
	public PointF[] PathPoints
			{
				get
				{
					// TODO
					//return pts;
					return null;
				}
			}
	[TODO]
	public byte[] PathTypes
			{
				get
				{
					// TODO
					//return types;
					return null;
				}
			}

	// get the number of elements in PathPoints or the PathTypes Array
	[TODO]
	public int PointCount
			{
				get
				{
					// TODO
					return 0;
				}
			}

	// Add an object to this path.
	private void AddPathObject(PathObject obj)
			{
				if(last != null)
				{
					last.next = obj;
				}
				else
				{
					first = obj;
				}
				last = obj;
			}

	// Add an arc to the current figure.
	public void AddArc(Rectangle rect, float startAngle, float sweepAngle)
			{
				AddArc((float)(rect.X), (float)(rect.Y),
					   (float)(rect.Width), (float)(rect.Height),
					   startAngle, sweepAngle);
			}
	public void AddArc(RectangleF rect, float startAngle, float sweepAngle)
			{
				AddArc(rect.X, rect.Y, rect.Width, rect.Height,
					   startAngle, sweepAngle);
			}
	public void AddArc(int x, int y, int width, int height,
					   float startAngle, float sweepAngle)
			{
				AddArc((float)x, (float)y, (float)width, (float)height,
					   startAngle, sweepAngle);
			}
	public void AddArc(float x, float y, float width, float height,
					   float startAngle, float sweepAngle)
			{
				AddPathObject(new ArcPathObject(x, y, width, height,
												startAngle, sweepAngle));
				needPenBrush = true;
			}

	// Add a bezier curve to the current path.
	public void AddBezier(Point pt1, Point pt2, Point pt3, Point pt4)
			{
				AddBezier((float)(pt1.X), (float)(pt1.Y),
						  (float)(pt2.X), (float)(pt2.Y),
						  (float)(pt3.X), (float)(pt3.Y),
						  (float)(pt4.X), (float)(pt4.Y));
			}
	public void AddBezier(PointF pt1, PointF pt2, PointF pt3, PointF pt4)
			{
				AddBezier(pt1.X, pt1.Y, pt2.X, pt2.Y,
						  pt3.X, pt3.Y, pt4.X, pt4.Y);
			}
	public void AddBezier(int x1, int y1, int x2, int y2,
						  int x3, int y3, int x4, int y4)
			{
				AddBezier((float)x1, (float)y1, (float)x2, (float)y2,
						  (float)x3, (float)y3, (float)x4, (float)y4);
			}
	public void AddBezier(float x1, float y1, float x2, float y2,
						  float x3, float y3, float x4, float y4)
			{
				AddPathObject(new BezierPathObject
						(x1, y1, x2, y2, x3, y3, x4, y4));
				needPenBrush = true;
			}

	// Add a set of beziers to the current path.
	public void AddBeziers(Point[] pts)
			{
				AddBeziers(Convert(pts));
			}
	public void AddBeziers(PointF[] pts)
			{
				if(pts == null)
				{
					throw new ArgumentNullException("pts");
				}
				if(pts.Length < 4)
				{
					throw new ArgumentException
						(String.Format
							(S._("Arg_NeedsAtLeastNPoints"), 4));
				}
				int posn = 0;
				while((posn + 4) <= pts.Length)
				{
					AddBezier(pts[posn], pts[posn + 1],
							  pts[posn + 2], pts[posn + 3]);
					posn += 3;
				}
			}

	// Add a closed curve to the current path.
	public void AddClosedCurve(Point[] points)
			{
				AddClosedCurve(Convert(points), 0.5f);
			}
	public void AddClosedCurve(PointF[] points)
			{
				AddClosedCurve(points, 0.5f);
			}
	public void AddClosedCurve(Point[] points, float tension)
			{
				AddClosedCurve(Convert(points), tension);
			}
	public void AddClosedCurve(PointF[] points, float tension)
			{
				if(points == null)
				{
					throw new ArgumentNullException("points");
				}
				if(points.Length < 4)
				{
					throw new ArgumentException
						(String.Format
							(S._("Arg_NeedsAtLeastNPoints"), 4));
				}
				AddPathObject(new ClosedCurvePathObject(points, tension));
			}

	// Add a curve to the current path.
	public void AddCurve(Point[] points)
			{
				if(points == null)
				{
					throw new ArgumentNullException("points");
				}
				AddCurve(Convert(points), 0, points.Length - 1, 0.5f);
			}
	public void AddCurve(PointF[] points)
			{
				if(points == null)
				{
					throw new ArgumentNullException("points");
				}
				AddCurve(points, 0, points.Length - 1, 0.5f);
			}
	public void AddCurve(Point[] points, float tension)
			{
				if(points == null)
				{
					throw new ArgumentNullException("points");
				}
				AddCurve(Convert(points), 0, points.Length - 1, tension);
			}
	public void AddCurve(PointF[] points, float tension)
			{
				if(points == null)
				{
					throw new ArgumentNullException("points");
				}
				AddCurve(points, 0, points.Length - 1, tension);
			}
	public void AddCurve(Point[] points, int offset,
						 int numberOfSegments, float tension)
			{
				AddCurve(Convert(points), offset, numberOfSegments, tension);
			}
	public void AddCurve(PointF[] points, int offset,
						 int numberOfSegments, float tension)
			{
				if(points == null)
				{
					throw new ArgumentNullException("points");
				}
				if(offset < 0 || offset >= (points.Length - 1))
				{
					throw new ArgumentOutOfRangeException
						("offset", S._("Arg_InvalidCurveOffset"));
				}
				if(numberOfSegments < 1 ||
				   (offset + numberOfSegments) >= points.Length)
				{
					throw new ArgumentOutOfRangeException
						("numberOfSegments", S._("Arg_InvalidCurveSegments"));
				}
				AddPathObject(new CurvePathObject
					(points, offset, numberOfSegments, tension));
				needPenBrush = true;
			}

	// Add an ellipse to the current figure.
	public void AddEllipse(Rectangle rect)
			{
				AddEllipse((float)(rect.X), (float)(rect.Y),
					       (float)(rect.Width), (float)(rect.Height));
			}
	public void AddEllipse(RectangleF rect)
			{
				AddEllipse(rect.X, rect.Y, rect.Width, rect.Height);
			}
	public void AddEllipse(int x, int y, int width, int height)
			{
				AddEllipse((float)x, (float)y, (float)width, (float)height);
			}
	public void AddEllipse(float x, float y, float width, float height)
			{
				AddPathObject(new ArcPathObject
					(x, y, width, height, 0.0f, 360f));
			}

	// Add a line to the current figure.
	public void AddLine(Point pt1, Point pt2)
			{
				AddLine((float)(pt1.X), (float)(pt1.Y),
					    (float)(pt2.X), (float)(pt2.Y));
			}
	public void AddLine(PointF pt1, PointF pt2)
			{
				AddLine(pt1.X, pt1.Y, pt2.X, pt2.Y);
			}
	public void AddLine(int x1, int y1, int x2, int y2)
			{
				AddLine((float)x1, (float)y1, (float)x2, (float)y2);
			}
	public void AddLine(float x1, float y1, float x2, float y2)
			{
				AddPathObject(new LinePathObject(x1, y1, x2, y2));
				needPenBrush = true;
			}

	// Add a list of lines to the current figure.
	public void AddLines(Point[] points)
			{
				AddLines(Convert(points));
			}
	public void AddLines(PointF[] points)
			{
				if(points == null)
				{
					throw new ArgumentNullException("points");
				}
				if(points.Length < 2)
				{
					throw new ArgumentException
						(String.Format
							(S._("Arg_NeedsAtLeastNPoints"), 2));
				}
				AddPathObject(new LinesPathObject(points));
				needPenBrush = true;
			}

	// Append another path to this one.  "connect" is intended for figures,
	// which we don't do anything special with here.
	public void AddPath(GraphicsPath addingPath, bool connect)
			{
				if(addingPath == null)
				{
					throw new ArgumentNullException("addingPath");
				}
				PathObject obj = addingPath.first;
				while(obj != null)
				{
					AddPathObject(obj.Clone());
					obj = obj.next;
				}
				if(addingPath.needPenBrush)
				{
					needPenBrush = true;
				}
			}

	// Add a pie section to the current figure.
	public void AddPie(Rectangle rect, float startAngle, float sweepAngle)
			{
				AddPie((float)(rect.X), (float)(rect.Y),
					   (float)(rect.Width), (float)(rect.Height),
					   startAngle, sweepAngle);
			}
	public void AddPie(int x, int y, int width, int height,
					   float startAngle, float sweepAngle)
			{
				AddPie((float)x, (float)y, (float)width, (float)height,
					   startAngle, sweepAngle);
			}
	public void AddPie(float x, float y, float width, float height,
					   float startAngle, float sweepAngle)
			{
				AddPathObject(new PiePathObject(x, y, width, height,
												startAngle, sweepAngle));
			}

	// Add a polygon to this path.
	public void AddPolygon(Point[] points)
			{
				AddPolygon(Convert(points));
			}
	public void AddPolygon(PointF[] points)
			{
				if(points == null)
				{
					throw new ArgumentNullException("points");
				}
				if(points.Length < 2)
				{
					throw new ArgumentException
						(String.Format
							(S._("Arg_NeedsAtLeastNPoints"), 2));
				}
				AddPathObject(new PolygonPathObject(points));
			}

	// Add a rectangle to this path.
	public void AddRectangle(Rectangle rect)
			{
				AddRectangle((RectangleF)rect);
			}
	public void AddRectangle(RectangleF rect)
			{
				AddPathObject(new RectanglePathObject
					(rect.X, rect.Y, rect.Width, rect.Height));
			}

	// Add a list of rectangles to this path.
	public void AddRectangles(Rectangle[] rects)
			{
				AddRectangles(Convert(rects));
			}
	public void AddRectangles(RectangleF[] rects)
			{
				if(rects == null)
				{
					throw new ArgumentNullException("rects");
				}
				int posn;
				for(posn = 0; posn < rects.Length; ++posn)
				{
					AddRectangle(rects[posn]);
				}
			}

	// Add a string to this path.
	public void AddString(String s, FontFamily family,
						  int style, float emSize,
						  Point origin, StringFormat format)
			{
				AddString(s, family, style, emSize, (PointF)origin, format);
			}
	[TODO]
	public void AddString(String s, FontFamily family,
						  int style, float emSize,
						  PointF origin, StringFormat format)
			{
				// TODO
			}
	public void AddString(String s, FontFamily family,
						  int style, float emSize,
						  Rectangle layoutRect, StringFormat format)
			{
				AddString(s, family, style, emSize,
						  (RectangleF)layoutRect, format);
			}
	[TODO]
	public void AddString(String s, FontFamily family,
						  int style, float emSize,
						  RectangleF layoutRect, StringFormat format)
			{
				// TODO
			}

	// Clean all markers from this path.
	public void ClearMarkers()
			{
				// We don't do anything special with markers here.
			}

	// Clone this object.
	public Object Clone()
			{
				GraphicsPath path = new GraphicsPath(fillMode);
				path.needPenBrush = needPenBrush;
				PathObject obj = first;
				while(obj != null)
				{
					path.AddPathObject(obj.Clone());
					obj = obj.next;
				}
				return path;
			}

	// Close all figures within the path.
	public void CloseAllFigures()
			{
				// We don't do anything special with figures here.
			}

	// Close the current figure and start a new one.
	public void CloseFigure()
			{
				// We don't do anything special with figures here.
			}

	// Dispose of this object.
	public void Dispose()
			{
				Dispose(true);
				GC.SuppressFinalize(this);
			}
	private void Dispose(bool disposing)
			{
				// Nothing to do here, because there is no unmanaged state.
			}

	// Flatten this curve into a set of line segments.
	public void Flatten()
			{
				Flatten(null, 0.25f);
			}
	public void Flatten(Matrix matrix)
			{
				Flatten(matrix, 0.25f);
			}
	[TODO]
	public void Flatten(Matrix matrix, float flatness)
			{
				// TODO
			}

	// Get a rectangle that bounds this path.
	public RectangleF GetBounds()
			{
				return GetBounds(null, null);
			}
	public RectangleF GetBounds(Matrix matrix)
			{
				return GetBounds(matrix, null);
			}
	[TODO]
	public RectangleF GetBounds(Matrix matrix, Pen pen)
			{
				// TODO
				return RectangleF.Empty;
			}

	// Get the last point in this path.
	public PointF GetLastPoint()
			{
				PointF[] points = PathPoints;
				return points[points.Length - 1];
			}

	// Determine if a point is visible when drawing an outline with a pen.
	public bool IsOutlineVisible(Point point, Pen pen)
			{
				return IsOutlineVisible((float)(point.X), (float)(point.Y),
										pen, null);
			}
	public bool IsOutlineVisible(PointF point, Pen pen)
			{
				return IsOutlineVisible(point.X, point.Y, pen, null);
			}
	public bool IsOutlineVisible(int x, int y, Pen pen)
			{
				return IsOutlineVisible((float)x, (float)y, pen, null);
			}
	public bool IsOutlineVisible(float x, float y, Pen pen)
			{
				return IsOutlineVisible(x, y, pen, null);
			}
	public bool IsOutlineVisible(Point point, Pen pen, Graphics graphics)
			{
				return IsOutlineVisible((float)(point.X), (float)(point.Y),
										pen, graphics);
			}
	public bool IsOutlineVisible(PointF point, Pen pen, Graphics graphics)
			{
				return IsOutlineVisible(point.X, point.Y, pen, graphics);
			}
	public bool IsOutlineVisible(int x, int y, Pen pen, Graphics graphics)
			{
				return IsOutlineVisible((float)x, (float)y, pen, graphics);
			}
	[TODO]
	public bool IsOutlineVisible(float x, float y, Pen pen, Graphics graphics)
			{
				// TODO
				return false;
			}

	// Determine if a point is visible.
	public bool IsVisible(Point point)
			{
				return IsVisible((float)(point.X), (float)(point.Y), null);
			}
	public bool IsVisible(PointF point)
			{
				return IsVisible(point.X, point.Y, null);
			}
	public bool IsVisible(int x, int y)
			{
				return IsVisible((float)x, (float)y, null);
			}
	public bool IsVisible(float x, float y)
			{
				return IsVisible(x, y, null);
			}
	public bool IsVisible(Point point, Graphics graphics)
			{
				return IsVisible((float)(point.X), (float)(point.Y), graphics);
			}
	public bool IsVisible(PointF point, Graphics graphics)
			{
				return IsVisible(point.X, point.Y, graphics);
			}
	public bool IsVisible(int x, int y, Graphics graphics)
			{
				return IsVisible((float)x, (float)y, graphics);
			}
	[TODO]
	public bool IsVisible(float x, float y, Graphics graphics)
			{
				// TODO
				return false;
			}

	// Reset this path.
	public void Reset()
			{
				first = null;
				last = null;
				needPenBrush = false;
				fillMode = FillMode.Alternate;
			}

	// Reverse the order of the path.
	public void Reverse()
			{
				PathObject obj = first;
				PathObject next;
				first = null;
				last = null;
				while(obj != null)
				{
					next = obj.next;
					obj.next = first;
					first = obj;
					if(last == null)
					{
						last = obj;
					}
				}
			}

	// Set a marker.
	public void SetMarkers()
			{
				// We don't do anything special with markers here.
			}

	// Start a new figure without closing the old one.
	public void StartFigure()
			{
				// We don't do anything special with figures here.
			}

	// Apply a transformation to this path.
	[TODO]
	public void Transform(Matrix matrix)
			{
				// TODO
			}

	// Apply a warp transformation to this path.
	public void Warp(PointF[] destPoints, RectangleF srcRect)
			{
				Warp(destPoints, srcRect, null, WarpMode.Perspective, 0.25f);
			}
	public void Warp(PointF[] destPoints, RectangleF srcRect, Matrix matrix)
			{
				Warp(destPoints, srcRect, matrix, WarpMode.Perspective, 0.25f);
			}
	public void Warp(PointF[] destPoints, RectangleF srcRect,
					 Matrix matrix, WarpMode warpMode)
			{
				Warp(destPoints, srcRect, matrix, warpMode, 0.25f);
			}
	[TODO]
	public void Warp(PointF[] destPoints, RectangleF srcRect,
					 Matrix matrix, WarpMode warpMode, float flatness)
			{
				// TODO
			}

	// Widen the path outline.
	public void Widen(Pen pen)
			{
				Widen(pen, null, 0.25f);
			}
	public void Widen(Pen pen, Matrix matrix)
			{
				Widen(pen, matrix, 0.25f);
			}
	[TODO]
	public void Widen(Pen pen, Matrix matrix, float flatness)
			{
				// TODO
			}

	// Draw this graphics path.
	internal void Draw(Graphics graphics, Pen pen)
			{
				PathObject obj = first;
				while(obj != null)
				{
					obj.Draw(graphics, pen);
					obj = obj.next;
				}
			}

	// Fill this graphics path.
	internal void Fill(Graphics graphics, Brush brush)
			{
				Pen pen;
				if(needPenBrush)
				{
					pen = new Pen(brush);
				}
				else
				{
					pen = null;
				}
				PathObject obj = first;
				while(obj != null)
				{
					obj.Fill(graphics, brush, pen, fillMode);
					obj = obj.next;
				}
				if(needPenBrush)
				{
					pen.Dispose();
				}
			}

	// Base class for path objects.
	private abstract class PathObject
	{
		public PathObject next;

		// Constructor.
		protected PathObject() {}

		// Draw this path object.
		public abstract void Draw(Graphics graphics, Pen pen);

		// Fill this path object.
		public abstract void Fill(Graphics graphics, Brush brush,
								  Pen penBrush, FillMode fillMode);

		// Clone this path object.
		public abstract PathObject Clone();

	}; // class PathObject

	// Arc path objects.
	private sealed class ArcPathObject : PathObject
	{
		// Internal state.
		private float x, y, width, height;
		private float startAngle, sweepAngle;

		// Constructor.
		public ArcPathObject(float x, float y, float width, float height,
					   		 float startAngle, float sweepAngle)
				{
					this.x = x;
					this.y = y;
					this.width = width;
					this.height = height;
					this.startAngle = startAngle;
					this.sweepAngle = sweepAngle;
				}

		// Draw this path object.
		public override void Draw(Graphics graphics, Pen pen)
				{
					graphics.DrawArc(pen, x, y, width, height,
									 startAngle, sweepAngle);
				}

		// Fill this path object.
		public override void Fill(Graphics graphics, Brush brush,
								  Pen penBrush, FillMode fillMode)
				{
					graphics.DrawArc(penBrush, x, y, width, height,
									 startAngle, sweepAngle);
				}

		// Clone this path object.
		public override PathObject Clone()
				{
					return new ArcPathObject(x, y, width, height,
											 startAngle, sweepAngle);
				}

	}; // class ArcPathObject

	// Bezier path objects.
	private sealed class BezierPathObject : PathObject
	{
		// Internal state.
		private float x1, y1, x2, y2, x3, y3, x4, y4;

		// Constructor.
		public BezierPathObject(float x1, float y1, float x2, float y2,
								float x3, float y3, float x4, float y4)
				{
					this.x1 = x1;
					this.y1 = y1;
					this.x2 = x2;
					this.y2 = y2;
					this.x3 = x3;
					this.y3 = y3;
					this.x4 = x4;
					this.y4 = y4;
				}

		// Draw this path object.
		public override void Draw(Graphics graphics, Pen pen)
				{
					graphics.DrawBezier(pen, x1, y1, x2, y2, x3, y3, x4, y4);
				}

		// Fill this path object.
		public override void Fill(Graphics graphics, Brush brush,
								  Pen penBrush, FillMode fillMode)
				{
					graphics.FillBezier
						(brush, x1, y1, x2, y2, x3, y3, x4, y4, fillMode );
				}

		// Clone this path object.
		public override PathObject Clone()
				{
					return new BezierPathObject
						(x1, y1, x2, y2, x3, y3, x4, y4);
				}

	}; // class BezierPathObject

	// Closed curve path objects.
	private sealed class ClosedCurvePathObject : PathObject
	{
		// Internal state.
		private PointF[] points;
		private float tension;

		// Constructor.
		public ClosedCurvePathObject(PointF[] points, float tension)
				{
					this.points = (PointF[])(points.Clone());
					this.tension = tension;
				}

		// Draw this path object.
		public override void Draw(Graphics graphics, Pen pen)
				{
					graphics.DrawClosedCurve(pen, points, tension,
											 FillMode.Alternate);
				}

		// Fill this path object.
		public override void Fill(Graphics graphics, Brush brush,
								  Pen penBrush, FillMode fillMode)
				{
					graphics.FillClosedCurve(brush, points, fillMode, tension);
				}

		// Clone this path object.
		public override PathObject Clone()
				{
					return new ClosedCurvePathObject(points, tension);
				}

	}; // class ClosedCurvePathObject

	// Curve path objects.
	private sealed class CurvePathObject : PathObject
	{
		// Internal state.
		private PointF[] points;
		private int offset, numberOfSegments;
		private float tension;

		// Constructor.
		public CurvePathObject(PointF[] points, int offset,
							   int numberOfSegments, float tension)
				{
					this.points = (PointF[])(points.Clone());
					this.offset = offset;
					this.numberOfSegments = numberOfSegments;
					this.tension = tension;
				}

		// Draw this path object.
		public override void Draw(Graphics graphics, Pen pen)
				{
					graphics.DrawCurve(pen, points, offset,
									   numberOfSegments, tension);
				}

		// Fill this path object.
		public override void Fill(Graphics graphics, Brush brush,
								  Pen penBrush, FillMode fillMode)
				{
					graphics.DrawCurve(penBrush, points, offset,
									   numberOfSegments, tension);
				}

		// Clone this path object.
		public override PathObject Clone()
				{
					return new CurvePathObject
						(points, offset, numberOfSegments, tension);
				}

	}; // class CurvePathObject

	// Line path objects.
	private sealed class LinePathObject : PathObject
	{
		// Internal state.
		private float x1, y1, x2, y2;

		// Constructor.
		public LinePathObject(float x1, float y1, float x2, float y2)
				{
					this.x1 = x1;
					this.y1 = y1;
					this.x2 = x2;
					this.y2 = y2;
				}

		// Draw this path object.
		public override void Draw(Graphics graphics, Pen pen)
				{
					graphics.DrawLine(pen, x1, y1, x2, y2);
				}

		// Fill this path object.
		public override void Fill(Graphics graphics, Brush brush,
								  Pen penBrush, FillMode fillMode)
				{
					graphics.DrawLine(penBrush, x1, y1, x2, y2);
				}

		// Clone this path object.
		public override PathObject Clone()
				{
					return new LinePathObject(x1, y1, x2, y2);
				}

	}; // class LinePathObject

	// Multiple lines path objects.
	private sealed class LinesPathObject : PathObject
	{
		// Internal state.
		private PointF[] points;

		// Constructor.
		public LinesPathObject(PointF[] points)
				{
					this.points = (PointF[])(points.Clone());
				}

		// Draw this path object.
		public override void Draw(Graphics graphics, Pen pen)
				{
					graphics.DrawLines(pen, points);
				}

		// Fill this path object.
		public override void Fill(Graphics graphics, Brush brush,
								  Pen penBrush, FillMode fillMode)
				{
					graphics.DrawLines(penBrush, points);
				}

		// Clone this path object.
		public override PathObject Clone()
				{
					return new LinesPathObject(points);
				}

	}; // class LinesPathObject

	// Pie path objects.
	private sealed class PiePathObject : PathObject
	{
		// Internal state.
		private float x, y, width, height;
		private float startAngle, sweepAngle;

		// Constructor.
		public PiePathObject(float x, float y, float width, float height,
					   		 float startAngle, float sweepAngle)
				{
					this.x = x;
					this.y = y;
					this.width = width;
					this.height = height;
					this.startAngle = startAngle;
					this.sweepAngle = sweepAngle;
				}

		// Draw this path object.
		public override void Draw(Graphics graphics, Pen pen)
				{
					graphics.DrawPie(pen, x, y, width, height,
									 startAngle, sweepAngle);
				}

		// Fill this path object.
		public override void Fill(Graphics graphics, Brush brush,
								  Pen penBrush, FillMode fillMode)
				{
					graphics.FillPie(brush, x, y, width, height,
									 startAngle, sweepAngle);
				}

		// Clone this path object.
		public override PathObject Clone()
				{
					return new ArcPathObject(x, y, width, height,
											 startAngle, sweepAngle);
				}

	}; // class PiePathObject

	// Polygon path objects.
	private sealed class PolygonPathObject : PathObject
	{
		// Internal state.
		private PointF[] points;

		// Constructor.
		public PolygonPathObject(PointF[] points)
				{
					this.points = (PointF[])(points.Clone());
				}

		// Draw this path object.
		public override void Draw(Graphics graphics, Pen pen)
				{
					graphics.DrawPolygon(pen, points);
				}

		// Fill this path object.
		public override void Fill(Graphics graphics, Brush brush,
								  Pen penBrush, FillMode fillMode)
				{
					graphics.FillPolygon(brush, points);
				}

		// Clone this path object.
		public override PathObject Clone()
				{
					return new PolygonPathObject(points);
				}

	}; // class PolygonPathObject

	// Rectangle path objects.
	private sealed class RectanglePathObject : PathObject
	{
		// Internal state.
		private float x, y, width, height;

		// Constructor.
		public RectanglePathObject(float x, float y, float width, float height)
				{
					this.x = x;
					this.y = y;
					this.width = width;
					this.height = height;
				}

		// Draw this path object.
		public override void Draw(Graphics graphics, Pen pen)
				{
					graphics.DrawRectangle(pen, x, y, width, height);
				}

		// Fill this path object.
		public override void Fill(Graphics graphics, Brush brush,
								  Pen penBrush, FillMode fillMode)
				{
					graphics.FillRectangle(brush, x, y, width, height);
				}

		// Clone this path object.
		public override PathObject Clone()
				{
					return new RectanglePathObject(x, y, width, height);
				}

	}; // class RectanglePathObject

}; // class GraphicsPath

}; // namespace System.Drawing.Drawing2D
