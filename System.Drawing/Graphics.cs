/*
 * Graphics.cs - Implementation of the "System.Drawing.Graphics" class.
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

namespace System.Drawing
{

using System.Runtime.InteropServices;
using System.Drawing.Drawing2D;
using System.Drawing.Imaging;
using System.Drawing.Text;
using System.Drawing.Toolkit;

#if !ECMA_COMPAT
[ComVisible(false)]
#endif
public sealed class Graphics : MarshalByRefObject, IDisposable
{
	// Internal state.
	private IToolkitGraphics graphics;
	private Region clip;
	private Matrix transform;
	private float pageScale;
	private GraphicsUnit pageUnit;
	internal GraphicsContainer stackTop;
	internal static Graphics defaultGraphics;

	// Default DPI for the screen.
	internal const float DefaultScreenDpi = 96.0f;

	// Constructor.
	internal Graphics(IToolkitGraphics graphics)
			{
				this.graphics = graphics;
				this.clip = null;
				this.transform = null;
				this.pageScale = 1.0f;
				this.pageUnit = GraphicsUnit.World;
				this.stackTop = null;
			}

	// Destructor.
	~Graphics()
			{
				Dispose(false);
			}

	// Dispose of this object.
	public void Dispose()
			{
				Dispose(true);
				GC.SuppressFinalize(this);
			}
	private void Dispose(bool disposing)
			{
				lock(this)
				{
					if(graphics != null)
					{
						graphics.Dispose();
						graphics = null;
					}
				}
			}

	// Get or set this object's properties.
	public Region Clip
			{
				get
				{
					if(clip == null)
					{
						clip = new Region();
					}
					return clip;
				}
				set
				{
					SetClip(value, CombineMode.Replace);
				}
			}
	public RectangleF ClipBounds
			{
				get
				{
					return Clip.GetBounds(this);
				}
			}
	public CompositingMode CompositingMode
			{
				get
				{
					lock(this)
					{
						if(graphics != null)
						{
							return graphics.CompositingMode;
						}
						else
						{
							return CompositingMode.SourceOver;
						}
					}
				}
				set
				{
					lock(this)
					{
						if(graphics != null)
						{
							graphics.CompositingMode = value;
						}
					}
				}
			}
	public CompositingQuality CompositingQuality
			{
				get
				{
					lock(this)
					{
						if(graphics != null)
						{
							return graphics.CompositingQuality;
						}
						else
						{
							return CompositingQuality.Default;
						}
					}
				}
				set
				{
					lock(this)
					{
						if(graphics != null)
						{
							graphics.CompositingQuality = value;
						}
					}
				}
			}
	public float DpiX
			{
				get
				{
					lock(this)
					{
						if(graphics != null)
						{
							return graphics.DpiX;
						}
						else
						{
							return DefaultScreenDpi;
						}
					}
				}
			}
	public float DpiY
			{
				get
				{
					lock(this)
					{
						if(graphics != null)
						{
							return graphics.DpiY;
						}
						else
						{
							return DefaultScreenDpi;
						}
					}
				}
			}
	public InterpolationMode InterpolationMode
			{
				get
				{
					lock(this)
					{
						if(graphics != null)
						{
							return graphics.InterpolationMode;
						}
						else
						{
							return InterpolationMode.Default;
						}
					}
				}
				set
				{
					lock(this)
					{
						if(graphics != null)
						{
							graphics.InterpolationMode = value;
						}
					}
				}
			}
	public bool IsClipEmpty
			{
				get
				{
					return (clip != null && clip.IsEmpty(this));
				}
			}
	public bool IsVisibleClipEmpty
			{
				get
				{
					RectangleF clip = VisibleClipBounds;
					return (clip.Width <= 0.0f && clip.Height <= 0.0f);
				}
			}
	public float PageScale
			{
				get
				{
					return pageScale;
				}
				set
				{
					pageScale = value;
				}
			}
	public GraphicsUnit PageUnit
			{
				get
				{
					return pageUnit;
				}
				set
				{
					pageUnit = value;
				}
			}
	public PixelOffsetMode PixelOffsetMode
			{
				get
				{
					lock(this)
					{
						if(graphics != null)
						{
							return graphics.PixelOffsetMode;
						}
						else
						{
							return PixelOffsetMode.Default;
						}
					}
				}
				set
				{
					lock(this)
					{
						if(graphics != null)
						{
							graphics.PixelOffsetMode = value;
						}
					}
				}
			}
	public Point RenderingOrigin
			{
				get
				{
					lock(this)
					{
						if(graphics != null)
						{
							return graphics.RenderingOrigin;
						}
						else
						{
							return new Point(0, 0);
						}
					}
				}
				set
				{
					lock(this)
					{
						if(graphics != null)
						{
							graphics.RenderingOrigin = value;
						}
					}
				}
			}
	public SmoothingMode SmoothingMode
			{
				get
				{
					lock(this)
					{
						if(graphics != null)
						{
							return graphics.SmoothingMode;
						}
						else
						{
							return SmoothingMode.Default;
						}
					}
				}
				set
				{
					lock(this)
					{
						if(graphics != null)
						{
							graphics.SmoothingMode = value;
						}
					}
				}
			}
	public int TextContrast
			{
				get
				{
					lock(this)
					{
						if(graphics != null)
						{
							return graphics.TextContrast;
						}
						else
						{
							return 4;
						}
					}
				}
				set
				{
					lock(this)
					{
						if(graphics != null)
						{
							graphics.TextContrast = value;
						}
					}
				}
			}
	public TextRenderingHint TextRenderingHint
			{
				get
				{
					lock(this)
					{
						if(graphics != null)
						{
							return graphics.TextRenderingHint;
						}
						else
						{
							return TextRenderingHint.SystemDefault;
						}
					}
				}
				set
				{
					lock(this)
					{
						if(graphics != null)
						{
							graphics.TextRenderingHint = value;
						}
					}
				}
			}
	public Matrix Transform
			{
				get
				{
					lock(this)
					{
						if(transform == null)
						{
							transform = new Matrix();
						}
						return transform;
					}
				}
				set
				{
					lock(this)
					{
						if(value == null || value.IsIdentity)
						{
							transform = null;
						}
						else
						{
							transform = value;
						}
					}
				}
			}
	public RectangleF VisibleClipBounds
			{
				get
				{
					lock(this)
					{
						if(graphics != null)
						{
							return graphics.VisibleClipBounds;
						}
						else
						{
							return RectangleF.Empty;
						}
					}
				}
			}

	// Add a metafile comment.
	public void AddMetafileComment(byte[] data)
			{
				lock(this)
				{
					if(graphics != null)
					{
						graphics.AddMetafileComment(data);
					}
				}
			}

	// Save the current contents of the graphics context in a container.
	public GraphicsContainer BeginContainer()
			{
				lock(this)
				{
					return new GraphicsContainer(this);
				}
			}
	public GraphicsContainer BeginContainer(Rectangle dstRect,
											Rectangle srcRect,
											GraphicsUnit unit)
			{
				return BeginContainer((RectangleF)dstRect,
									  (RectangleF)srcRect, unit);
			}
	public GraphicsContainer BeginContainer(RectangleF dstRect,
											RectangleF srcRect,
											GraphicsUnit unit)
			{
				// Save the current state of the context.
				GraphicsContainer container = BeginContainer();

				// Modify the context information appropriately.
				// TODO

				// Return the saved information to the caller.
				return container;
			}

	// Clear the entire drawing surface.
	public void Clear(Color color)
			{
				lock(this)
				{
					ToolkitGraphics.Clear(color);
				}
			}

	// Draw an arc.
	public void DrawArc(Pen pen, Rectangle rect,
						float startAngle, float sweepAngle)
			{
				DrawArc(pen, rect.X, rect.Y, rect.Width, rect.Height,
						startAngle, sweepAngle);
			}
	public void DrawArc(Pen pen, RectangleF rect,
						float startAngle, float sweepAngle)
			{
				DrawArc(pen, rect.X, rect.Y, rect.Width, rect.Height,
						startAngle, sweepAngle);
			}
	public void DrawArc(Pen pen, int x, int y, int width, int height,
						float startAngle, float sweepAngle)
			{
				Point[] rect = ConvertRectangle(x, y, width, height);
				lock(this)
				{
					SelectPen(pen);
					ToolkitGraphics.DrawArc(rect, startAngle, sweepAngle);
				}
			}
	public void DrawArc(Pen pen, float x, float y, float width, float height,
						float startAngle, float sweepAngle)
			{
				Point[] rect = ConvertRectangle(x, y, width, height);
				lock(this)
				{
					SelectPen(pen);
					ToolkitGraphics.DrawArc(rect, startAngle, sweepAngle);
				}
			}

	// Draw a Bezier spline.
	public void DrawBezier(Pen pen, Point pt1, Point pt2,
						   Point pt3, Point pt4)
			{
				DrawBezier(pen, (float)(pt1.X), (float)(pt1.Y),
						   (float)(pt2.X), (float)(pt2.Y),
						   (float)(pt3.X), (float)(pt3.Y),
						   (float)(pt4.X), (float)(pt4.Y));
			}
	public void DrawBezier(Pen pen, PointF pt1, PointF pt2,
						   PointF pt3, PointF pt4)
			{
				DrawBezier(pen, pt1.X, pt1.Y, pt2.X, pt2.Y,
						   pt3.X, pt3.Y, pt4.X, pt4.Y);
			}
	public void DrawBezier(Pen pen, float x1, float y1, float x2, float y2,
						   float x3, float y3, float x4, float y4)
			{
				int dx1, dy1, dx2, dy2;
				int dx3, dy3, dx4, dy4;
				ConvertPoint(x1, y1, out dx1, out dy1);
				ConvertPoint(x2, y2, out dx2, out dy2);
				ConvertPoint(x3, y3, out dx3, out dy3);
				ConvertPoint(x4, y4, out dx4, out dy4);
				lock(this)
				{
					SelectPen(pen);
					ToolkitGraphics.DrawBezier(dx1, dy1, dx2, dy2,
											   dx3, dy3, dx4, dy4);
				}
			}

	// Draw a series of Bezier splines.
	public void DrawBeziers(Pen pen, Point[] points)
			{
				if(pen == null)
				{
					throw new ArgumentNullException("pen");
				}
				if(points == null)
				{
					throw new ArgumentNullException("points");
				}
				int posn = 0;
				while((posn + 4) <= points.Length)
				{
					DrawBezier(pen, points[posn], points[posn + 1],
							   points[posn + 2], points[posn + 3]);
					posn += 3;
				}
			}
	public void DrawBeziers(Pen pen, PointF[] points)
			{
				if(pen == null)
				{
					throw new ArgumentNullException("pen");
				}
				if(points == null)
				{
					throw new ArgumentNullException("points");
				}
				int posn = 0;
				while((posn + 4) <= points.Length)
				{
					DrawBezier(pen, points[posn], points[posn + 1],
							   points[posn + 2], points[posn + 3]);
					posn += 3;
				}
			}

	// Draw a closed cardinal spline.
	public void DrawClosedCurve(Pen pen, Point[] points)
			{
				DrawClosedCurve(pen, points, 0.5f, FillMode.Alternate);
			}
	public void DrawClosedCurve(Pen pen, PointF[] points)
			{
				DrawClosedCurve(pen, points, 0.5f, FillMode.Alternate);
			}
	public void DrawClosedCurve(Pen pen, Point[] points,
								float tension, FillMode fillMode)
			{
				points = ConvertPoints(points, 4);
				lock(this)
				{
					SelectPen(pen);
					ToolkitGraphics.DrawClosedCurve(points, tension);
				}
			}
	public void DrawClosedCurve(Pen pen, PointF[] points,
								float tension, FillMode fillMode)
			{
				Point[] dpoints = ConvertPoints(points, 4);
				lock(this)
				{
					SelectPen(pen);
					ToolkitGraphics.DrawClosedCurve(dpoints, tension);
				}
			}

	// Draw a cardinal spline.
	public void DrawCurve(Pen pen, Point[] points)
			{
				if(points == null)
				{
					throw new ArgumentNullException("points");
				}
				DrawCurve(pen, points, 0, points.Length - 1, 0.5f);
			}
	public void DrawCurve(Pen pen, PointF[] points)
			{
				if(points == null)
				{
					throw new ArgumentNullException("points");
				}
				DrawCurve(pen, points, 0, points.Length - 1, 0.5f);
			}
	public void DrawCurve(Pen pen, Point[] points, float tension)
			{
				if(points == null)
				{
					throw new ArgumentNullException("points");
				}
				DrawCurve(pen, points, 0, points.Length - 1, tension);
			}
	public void DrawCurve(Pen pen, PointF[] points, float tension)
			{
				if(points == null)
				{
					throw new ArgumentNullException("points");
				}
				DrawCurve(pen, points, 0, points.Length - 1, tension);
			}
	public void DrawCurve(Pen pen, Point[] points,
						  int offset, int numberOfSegments)
			{
				DrawCurve(pen, points, offset, numberOfSegments, 0.5f);
			}
	public void DrawCurve(Pen pen, PointF[] points,
						  int offset, int numberOfSegments)
			{
				DrawCurve(pen, points, offset, numberOfSegments, 0.5f);
			}
	public void DrawCurve(Pen pen, Point[] points,
						  int offset, int numberOfSegments, float tension)
			{
				points = ConvertPoints(points, 4);
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
				lock(this)
				{
					SelectPen(pen);
					ToolkitGraphics.DrawCurve
						(points, offset, numberOfSegments, tension);
				}
			}
	public void DrawCurve(Pen pen, PointF[] points,
						  int offset, int numberOfSegments, float tension)
			{
				Point[] dpoints = ConvertPoints(points, 4);
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
				lock(this)
				{
					SelectPen(pen);
					ToolkitGraphics.DrawCurve
						(dpoints, offset, numberOfSegments, tension);
				}
			}

	// Draw an ellipse.
	public void DrawEllipse(Pen pen, Rectangle rect)
			{
				DrawEllipse(pen, rect.X, rect.Y, rect.Width, rect.Height);
			}
	public void DrawEllipse(Pen pen, RectangleF rect)
			{
				DrawEllipse(pen, rect.X, rect.Y, rect.Width, rect.Height);
			}
	public void DrawEllipse(Pen pen, int x, int y, int width, int height)
			{
				Point[] rect = ConvertRectangle(x, y, width, height);
				lock(this)
				{
					SelectPen(pen);
					ToolkitGraphics.DrawArc(rect, 0.0f, 360.0f);
				}
			}
	public void DrawEllipse(Pen pen, float x, float y,
							float width, float height)
			{
				Point[] rect = ConvertRectangle(x, y, width, height);
				lock(this)
				{
					SelectPen(pen);
					ToolkitGraphics.DrawArc(rect, 0.0f, 360.0f);
				}
			}

	// Draw an icon.
	[TODO]
	public void DrawIcon(Icon icon, Rectangle targetRect)
			{
				// TODO
			}
	[TODO]
	public void DrawIconUnstretched(Icon icon, Rectangle targetRect)
			{
				// TODO
			}
	public void DrawIcon(Icon icon, int x, int y)
			{
				if(icon == null)
				{
					throw new ArgumentNullException("icon");
				}
				DrawIconUnstretched
					(icon, new Rectangle(x, y, icon.Width, icon.Height));
			}

	// Draw an image.
	public void DrawImage(Image image, Point point)
			{
				DrawImage(image, point.X, point.Y);
			}
	[TODO]
	public void DrawImage(Image image, Point[] destPoints)
			{
				// TODO
			}
	public void DrawImage(Image image, PointF point)
			{
				DrawImage(image, point.X, point.Y);
			}
	[TODO]
	public void DrawImage(Image image, PointF[] destPoints)
			{
				// TODO
			}
	[TODO]
	public void DrawImage(Image image, Rectangle rect)
			{
				// TODO
			}
	[TODO]
	public void DrawImage(Image image, RectangleF rect)
			{
				// TODO
			}
	[TODO]
	public void DrawImage(Image image, int x, int y)
			{
				// TODO
			}
	[TODO]
	public void DrawImage(Image image, float x, float y)
			{
				// TODO
			}
	[TODO]
	public void DrawImage(Image image, Point[] destPoints,
						  Rectangle srcRect, GraphicsUnit srcUnit)
			{
				// TODO
			}
	[TODO]
	public void DrawImage(Image image, PointF[] destPoints,
						  RectangleF srcRect, GraphicsUnit srcUnit)
			{
				// TODO
			}
	[TODO]
	public void DrawImage(Image image, Rectangle destRect,
						  Rectangle srcRect, GraphicsUnit srcUnit)
			{
				// TODO
			}
	[TODO]
	public void DrawImage(Image image, RectangleF destRect,
						  RectangleF srcRect, GraphicsUnit srcUnit)
			{
				// TODO
			}
	[TODO]
	public void DrawImage(Image image, int x, int y, int width, int height)
			{
				// TODO
			}
	[TODO]
	public void DrawImage(Image image, int x, int y,
						  Rectangle srcRect, GraphicsUnit srcUnit)
			{
				// TODO
			}
	[TODO]
	public void DrawImage(Image image, Point[] destPoints,
						  Rectangle srcRect, GraphicsUnit srcUnit,
						  ImageAttributes imageAttr)
			{
				// TODO
			}
	[TODO]
	public void DrawImage(Image image, PointF[] destPoints,
						  RectangleF srcRect, GraphicsUnit srcUnit,
						  ImageAttributes imageAttr)
			{
				// TODO
			}
	[TODO]
	public void DrawImage(Image image, float x, float y,
						  RectangleF srcRect, GraphicsUnit srcUnit)
			{
				// TODO
			}
	[TODO]
	public void DrawImage(Image image, float x, float y,
						  float width, float height)
			{
				// TODO
			}
	[TODO]
	public void DrawImage(Image image, Point[] destPoints,
						  Rectangle srcRect, GraphicsUnit srcUnit,
						  ImageAttributes imageAttr,
						  DrawImageAbort callback)
			{
				// TODO
			}
	[TODO]
	public void DrawImage(Image image, PointF[] destPoints,
						  RectangleF srcRect, GraphicsUnit srcUnit,
						  ImageAttributes imageAttr,
						  DrawImageAbort callback)
			{
				// TODO
			}
	[TODO]
	public void DrawImage(Image image, Point[] destPoints,
						  Rectangle srcRect, GraphicsUnit srcUnit,
						  ImageAttributes imageAttr,
						  DrawImageAbort callback, int callbackdata)
			{
				// TODO
			}
	[TODO]
	public void DrawImage(Image image, PointF[] destPoints,
						  RectangleF srcRect, GraphicsUnit srcUnit,
						  ImageAttributes imageAttr,
						  DrawImageAbort callback, int callbackdata)
			{
				// TODO
			}
	[TODO]
	public void DrawImage(Image image, Rectangle destRect,
						  int srcX, int srcY, int srcWidth, int srcHeight,
						  GraphicsUnit srcUnit)
			{
				// TODO
			}
	[TODO]
	public void DrawImage(Image image, Rectangle destRect,
						  float srcX, float srcY,
						  float srcWidth, float srcHeight,
						  GraphicsUnit srcUnit)
			{
				// TODO
			}
	[TODO]
	public void DrawImage(Image image, Rectangle destRect,
						  int srcX, int srcY, int srcWidth, int srcHeight,
						  GraphicsUnit srcUnit, ImageAttributes imageAttr)
			{
				// TODO
			}
	[TODO]
	public void DrawImage(Image image, Rectangle destRect,
						  float srcX, float srcY,
						  float srcWidth, float srcHeight,
						  GraphicsUnit srcUnit, ImageAttributes imageAttr)
			{
				// TODO
			}
	[TODO]
	public void DrawImage(Image image, Rectangle destRect,
						  int srcX, int srcY, int srcWidth, int srcHeight,
						  GraphicsUnit srcUnit, ImageAttributes imageAttr,
						  DrawImageAbort callback)
			{
				// TODO
			}
	[TODO]
	public void DrawImage(Image image, Rectangle destRect,
						  float srcX, float srcY,
						  float srcWidth, float srcHeight,
						  GraphicsUnit srcUnit, ImageAttributes imageAttr,
						  DrawImageAbort callback)
			{
				// TODO
			}
	[TODO]
	public void DrawImage(Image image, Rectangle destRect,
						  int srcX, int srcY, int srcWidth, int srcHeight,
						  GraphicsUnit srcUnit, ImageAttributes imageAttr,
						  DrawImageAbort callback, IntPtr callbackData)
			{
				// TODO
			}
	[TODO]
	public void DrawImage(Image image, Rectangle destRect,
						  float srcX, float srcY,
						  float srcWidth, float srcHeight,
						  GraphicsUnit srcUnit, ImageAttributes imageAttr,
						  DrawImageAbort callback, IntPtr callbackData)
			{
				// TODO
			}

	// Draw an unscaled image.
	public void DrawImageUnscaled(Image image, Point point)
			{
				DrawImage(image, point.X, point.Y);
			}
	public void DrawImageUnscaled(Image image, Rectangle rect)
			{
				DrawImage(image, rect.X, rect.Y);
			}
	public void DrawImageUnscaled(Image image, int x, int y,
								  int width, int height)
			{
				DrawImage(image, x, y);
			}
	public void DrawImageUnscaled(Image image, int x, int y)
			{
				DrawImage(image, x, y);
			}

	// Draw a line between two points.
	public void DrawLine(Pen pen, Point pt1, Point pt2)
			{
				DrawLine(pen, pt1.X, pt1.Y, pt2.X, pt2.Y);
			}
	public void DrawLine(Pen pen, PointF pt1, PointF pt2)
			{
				DrawLine(pen, pt1.X, pt1.Y, pt2.X, pt2.Y);
			}
	public void DrawLine(Pen pen, int x1, int y1, int x2, int y2)
			{
				ConvertPoint(ref x1, ref y1);
				ConvertPoint(ref x2, ref y2);
				lock(this)
				{
					SelectPen(pen);
					ToolkitGraphics.DrawLine(x1, y1, x2, y2);
				}
			}
	public void DrawLine(Pen pen, float x1, float y1, float x2, float y2)
			{
				int dx1, dy1, dx2, dy2;
				ConvertPoint(x1, y1, out dx1, out dy1);
				ConvertPoint(x2, y2, out dx2, out dy2);
				lock(this)
				{
					SelectPen(pen);
					ToolkitGraphics.DrawLine(dx1, dy1, dx2, dy2);
				}
			}

	// Draw a series of connected line segments.
	public void DrawLines(Pen pen, Point[] points)
			{
				points = ConvertPoints(points, 2);
				lock(this)
				{
					SelectPen(pen);
					ToolkitGraphics.DrawLines(points);
				}
			}
	public void DrawLines(Pen pen, PointF[] points)
			{
				Point[] dpoints = ConvertPoints(points, 2);
				lock(this)
				{
					SelectPen(pen);
					ToolkitGraphics.DrawLines(dpoints);
				}
			}

	// Draw a path object.
	public void DrawPath(Pen pen, GraphicsPath path)
			{
				if(pen == null)
				{
					throw new ArgumentNullException("pen");
				}
				if(path == null)
				{
					throw new ArgumentNullException("path");
				}
				path.Draw(this, pen);
			}

	// Draw a pie shape.
	public void DrawPie(Pen pen, Rectangle rect,
						float startAngle, float sweepAngle)
			{
				if(((float)(int)startAngle) == startAngle &&
				   ((float)(int)sweepAngle) == sweepAngle)
				{
					DrawPie(pen, rect.X, rect.Y, rect.Width, rect.Height,
							(int)startAngle, (int)sweepAngle);
				}
				else
				{
					DrawPie(pen, rect.X, rect.Y, rect.Width, rect.Height,
							startAngle, sweepAngle);
				}
			}
	public void DrawPie(Pen pen, RectangleF rect,
						float startAngle, float sweepAngle)
			{
				DrawPie(pen, rect.X, rect.Y, rect.Width, rect.Height,
						startAngle, sweepAngle);
			}
	public void DrawPie(Pen pen, int x, int y, int width, int height,
						int startAngle, int sweepAngle)
			{
				Point[] rect = ConvertRectangle(x, y, width, height);
				lock(this)
				{
					SelectPen(pen);
					ToolkitGraphics.DrawPie(rect, startAngle, sweepAngle);
				}
			}
	public void DrawPie(Pen pen, float x, float y, float width, float height,
						float startAngle, float sweepAngle)
			{
				Point[] rect = ConvertRectangle(x, y, width, height);
				lock(this)
				{
					SelectPen(pen);
					ToolkitGraphics.DrawPie(rect, startAngle, sweepAngle);
				}
			}

	// Draw a polygon.
	public void DrawPolygon(Pen pen, Point[] points)
			{
				points = ConvertPoints(points, 2);
				lock(this)
				{
					SelectPen(pen);
					ToolkitGraphics.DrawPolygon(points);
				}
			}
	public void DrawPolygon(Pen pen, PointF[] points)
			{
				Point[] dpoints = ConvertPoints(points, 2);
				lock(this)
				{
					SelectPen(pen);
					ToolkitGraphics.DrawPolygon(dpoints);
				}
			}

	// Draw a rectangle.
	public void DrawRectangle(Pen pen, Rectangle rect)
			{
				DrawRectangle(pen, rect.X, rect.Y, rect.Width, rect.Height);
			}
	public void DrawRectangle(Pen pen, int x, int y, int width, int height)
			{
				if (width>0 && height>0)
				{
					lock(this)
					{
						SelectPen(pen);
						ToolkitGraphics.DrawPolygon(ConvertRectangle(x, y, width, height));
					}
				}
			}
	public void DrawRectangle(Pen pen, float x, float y,
							  float width, float height)
			{
				if (width>0 && height>0)
				{
					lock(this)
					{
						SelectPen(pen);
						ToolkitGraphics.DrawPolygon(ConvertRectangle(x, y, width, height));
					}
				}
			}

	// Draw a series of rectangles.
	public void DrawRectangles(Pen pen, Rectangle[] rects)
			{
				if(rects == null)
				{
					throw new ArgumentNullException("rects");
				}
				int posn;
				for(posn = 0; posn < rects.Length; ++posn)
				{
					DrawRectangle(pen, rects[posn].X, rects[posn].Y,
								  rects[posn].Width, rects[posn].Height);
				}
			}
	public void DrawRectangles(Pen pen, RectangleF[] rects)
			{
				if(rects == null)
				{
					throw new ArgumentNullException("rects");
				}
				int posn;
				for(posn = 0; posn < rects.Length; ++posn)
				{
					DrawRectangle(pen, rects[posn].X, rects[posn].Y,
								  rects[posn].Width, rects[posn].Height);
				}
			}

	// Draw a string.
	public void DrawString(String s, Font font, Brush brush, PointF point)
			{
				DrawString(s, font, brush, point.X, point.Y, null);
			}
	public void DrawString(String s, Font font, Brush brush,
						   RectangleF layoutRectangle)
			{
				DrawString(s, font, brush, layoutRectangle, null);
			}
	public void DrawString(String s, Font font, Brush brush,
						   PointF point, StringFormat format)
			{
				DrawString(s, font, brush, point.X, point.Y, format);
			}
	public void DrawString(String s, Font font, Brush brush,
						   RectangleF layoutRectangle, StringFormat format)
			{
				if(s == null || s.Length == 0)
				{
					return;
				}
				Point[] rect = ConvertRectangle
					(layoutRectangle.X, layoutRectangle.Y,
					 layoutRectangle.Width, layoutRectangle.Height);
				lock(this)
				{
					SelectFont(font);
					SelectBrush(brush);
					if(format != null &&
					   (format.Alignment != StringAlignment.Near ||
					    format.LineAlignment != StringAlignment.Near))
					{
						// Adjust the rectangle for the alignment values.
						int charactersFitted, linesFilled;
						Size size = ToolkitGraphics.MeasureString
							(s, rect, null,
							 out charactersFitted, out linesFilled, true);
						if(format.Alignment == StringAlignment.Center)
						{
							rect[0].X +=
								(rect[1].X - rect[0].X - size.Width) / 2;
						}
						else if(format.Alignment == StringAlignment.Far)
						{
							rect[0].X = rect[1].X - size.Width;
						}
						if(format.LineAlignment == StringAlignment.Center)
						{
							rect[0].Y +=
								(rect[2].Y - rect[0].Y - size.Height) / 2;
						}
						else if(format.Alignment == StringAlignment.Far)
						{
							rect[0].Y = rect[2].Y - size.Height;
						}
					}
					ToolkitGraphics.DrawString
						(s, rect[0].X, rect[0].Y, format);
				}
			}
	public void DrawString(String s, Font font, Brush brush, float x, float y)
			{
				DrawString(s, font, brush, x, y, null);
			}
	public void DrawString(String s, Font font, Brush brush,
						   float x, float y, StringFormat format)
			{
				if(s == null || s.Length == 0)
				{
					return;
				}
				int dx, dy;
				ConvertPoint(x, y, out dx, out dy);
				lock(this)
				{
					SelectFont(font);
					SelectBrush(brush);
					ToolkitGraphics.DrawString(s, dx, dy, format);
				}
			}

	// Reset the graphics state back to a previous container level.
	public void EndContainer(GraphicsContainer container)
			{
				if(container != null)
				{
					lock(this)
					{
						container.Restore(this);
					}
				}
			}

	// Enumerate the contents of a metafile.
	[TODO]
	public void EnumerateMetafile(Metafile metafile, Point destPoint,
								  EnumerateMetafileProc callback)
			{
				// TODO
			}
	[TODO]
	public void EnumerateMetafile(Metafile metafile, Point[] destPoints,
								  EnumerateMetafileProc callback)
			{
				// TODO
			}
	[TODO]
	public void EnumerateMetafile(Metafile metafile, PointF destPoint,
								  EnumerateMetafileProc callback)
			{
				// TODO
			}
	[TODO]
	public void EnumerateMetafile(Metafile metafile, PointF[] destPoints,
								  EnumerateMetafileProc callback)
			{
				// TODO
			}
	[TODO]
	public void EnumerateMetafile(Metafile metafile, Rectangle destRect,
								  EnumerateMetafileProc callback)
			{
				// TODO
			}
	[TODO]
	public void EnumerateMetafile(Metafile metafile, RectangleF destRect,
								  EnumerateMetafileProc callback)
			{
				// TODO
			}
	[TODO]
	public void EnumerateMetafile(Metafile metafile, Point destPoint,
								  EnumerateMetafileProc callback,
								  IntPtr callbackData)
			{
				// TODO
			}
	[TODO]
	public void EnumerateMetafile(Metafile metafile, Point[] destPoints,
								  EnumerateMetafileProc callback,
								  IntPtr callbackData)
			{
				// TODO
			}
	[TODO]
	public void EnumerateMetafile(Metafile metafile, PointF destPoint,
								  EnumerateMetafileProc callback,
								  IntPtr callbackData)
			{
				// TODO
			}
	[TODO]
	public void EnumerateMetafile(Metafile metafile, PointF[] destPoints,
								  EnumerateMetafileProc callback,
								  IntPtr callbackData)
			{
				// TODO
			}
	[TODO]
	public void EnumerateMetafile(Metafile metafile, Rectangle destRect,
								  EnumerateMetafileProc callback,
								  IntPtr callbackData)
			{
				// TODO
			}
	[TODO]
	public void EnumerateMetafile(Metafile metafile, RectangleF destRect,
								  EnumerateMetafileProc callback,
								  IntPtr callbackData)
			{
				// TODO
			}
	[TODO]
	public void EnumerateMetafile(Metafile metafile, Point destPoint,
								  EnumerateMetafileProc callback,
								  IntPtr callbackData,
								  ImageAttributes imageAttr)
			{
				// TODO
			}
	[TODO]
	public void EnumerateMetafile(Metafile metafile, Point[] destPoints,
								  EnumerateMetafileProc callback,
								  IntPtr callbackData,
								  ImageAttributes imageAttr)
			{
				// TODO
			}
	[TODO]
	public void EnumerateMetafile(Metafile metafile, PointF destPoint,
								  EnumerateMetafileProc callback,
								  IntPtr callbackData,
								  ImageAttributes imageAttr)
			{
				// TODO
			}
	[TODO]
	public void EnumerateMetafile(Metafile metafile, PointF[] destPoints,
								  EnumerateMetafileProc callback,
								  IntPtr callbackData,
								  ImageAttributes imageAttr)
			{
				// TODO
			}
	[TODO]
	public void EnumerateMetafile(Metafile metafile, Rectangle destRect,
								  EnumerateMetafileProc callback,
								  IntPtr callbackData,
								  ImageAttributes imageAttr)
			{
				// TODO
			}
	[TODO]
	public void EnumerateMetafile(Metafile metafile, RectangleF destRect,
								  EnumerateMetafileProc callback,
								  IntPtr callbackData,
								  ImageAttributes imageAttr)
			{
				// TODO
			}
	[TODO]
	public void EnumerateMetafile(Metafile metafile, Point destPoint,
								  Rectangle srcRect, GraphicsUnit srcUnit,
								  EnumerateMetafileProc callback)
			{
				// TODO
			}
	[TODO]
	public void EnumerateMetafile(Metafile metafile, Point[] destPoints,
								  Rectangle srcRect, GraphicsUnit srcUnit,
								  EnumerateMetafileProc callback)
			{
				// TODO
			}
	[TODO]
	public void EnumerateMetafile(Metafile metafile, PointF destPoint,
								  RectangleF srcRect, GraphicsUnit srcUnit,
								  EnumerateMetafileProc callback)
			{
				// TODO
			}
	[TODO]
	public void EnumerateMetafile(Metafile metafile, PointF[] destPoints,
								  RectangleF srcRect, GraphicsUnit srcUnit,
								  EnumerateMetafileProc callback)
			{
				// TODO
			}
	[TODO]
	public void EnumerateMetafile(Metafile metafile, Rectangle destRect,
								  Rectangle srcRect, GraphicsUnit srcUnit,
								  EnumerateMetafileProc callback)
			{
				// TODO
			}
	[TODO]
	public void EnumerateMetafile(Metafile metafile, RectangleF destRect,
								  RectangleF srcRect, GraphicsUnit srcUnit,
								  EnumerateMetafileProc callback)
			{
				// TODO
			}
	[TODO]
	public void EnumerateMetafile(Metafile metafile, Point destPoint,
								  Rectangle srcRect, GraphicsUnit srcUnit,
								  EnumerateMetafileProc callback,
								  IntPtr callbackData)
			{
				// TODO
			}
	[TODO]
	public void EnumerateMetafile(Metafile metafile, Point[] destPoints,
								  Rectangle srcRect, GraphicsUnit srcUnit,
								  EnumerateMetafileProc callback,
								  IntPtr callbackData)
			{
				// TODO
			}
	[TODO]
	public void EnumerateMetafile(Metafile metafile, PointF destPoint,
								  RectangleF srcRect, GraphicsUnit srcUnit,
								  EnumerateMetafileProc callback,
								  IntPtr callbackData)
			{
				// TODO
			}
	[TODO]
	public void EnumerateMetafile(Metafile metafile, PointF[] destPoints,
								  RectangleF srcRect, GraphicsUnit srcUnit,
								  EnumerateMetafileProc callback,
								  IntPtr callbackData)
			{
				// TODO
			}
	[TODO]
	public void EnumerateMetafile(Metafile metafile, Rectangle destRect,
								  Rectangle srcRect, GraphicsUnit srcUnit,
								  EnumerateMetafileProc callback,
								  IntPtr callbackData)
			{
				// TODO
			}
	[TODO]
	public void EnumerateMetafile(Metafile metafile, RectangleF destRect,
								  RectangleF srcRect, GraphicsUnit srcUnit,
								  EnumerateMetafileProc callback,
								  IntPtr callbackData)
			{
				// TODO
			}
	[TODO]
	public void EnumerateMetafile(Metafile metafile, Point destPoint,
								  Rectangle srcRect, GraphicsUnit srcUnit,
								  EnumerateMetafileProc callback,
								  IntPtr callbackData,
								  ImageAttributes imageAttr)
			{
				// TODO
			}
	[TODO]
	public void EnumerateMetafile(Metafile metafile, Point[] destPoints,
								  Rectangle srcRect, GraphicsUnit srcUnit,
								  EnumerateMetafileProc callback,
								  IntPtr callbackData,
								  ImageAttributes imageAttr)
			{
				// TODO
			}
	[TODO]
	public void EnumerateMetafile(Metafile metafile, PointF destPoint,
								  RectangleF srcRect, GraphicsUnit srcUnit,
								  EnumerateMetafileProc callback,
								  IntPtr callbackData,
								  ImageAttributes imageAttr)
			{
				// TODO
			}
	[TODO]
	public void EnumerateMetafile(Metafile metafile, PointF[] destPoints,
								  RectangleF srcRect, GraphicsUnit srcUnit,
								  EnumerateMetafileProc callback,
								  IntPtr callbackData,
								  ImageAttributes imageAttr)
			{
				// TODO
			}
	[TODO]
	public void EnumerateMetafile(Metafile metafile, Rectangle destRect,
								  Rectangle srcRect, GraphicsUnit srcUnit,
								  EnumerateMetafileProc callback,
								  IntPtr callbackData,
								  ImageAttributes imageAttr)
			{
				// TODO
			}
	[TODO]
	public void EnumerateMetafile(Metafile metafile, RectangleF destRect,
								  RectangleF srcRect, GraphicsUnit srcUnit,
								  EnumerateMetafileProc callback,
								  IntPtr callbackData,
								  ImageAttributes imageAttr)
			{
				// TODO
			}

	// Update the clipping region to exclude a particular rectangle.
	public void ExcludeClip(Rectangle rect)
			{
				Clip.Exclude(rect);
				UpdateClip();
			}
	public void ExcludeClip(Region region)
			{
				Clip.Exclude(region);
				UpdateClip();
			}

	// Fill a closed cardinal spline.
	public void FillClosedCurve(Brush brush, Point[] points)
			{
				FillClosedCurve(brush, points, FillMode.Alternate, 0.5f);
			}
	public void FillClosedCurve(Brush brush, PointF[] points)
			{
				FillClosedCurve(brush, points, FillMode.Alternate, 0.5f);
			}
	public void FillClosedCurve(Brush brush, Point[] points,
								FillMode fillMode)
			{
				FillClosedCurve(brush, points, fillMode, 0.5f);
			}
	public void FillClosedCurve(Brush brush, PointF[] points,
								FillMode fillMode)
			{
				FillClosedCurve(brush, points, fillMode, 0.5f);
			}
	public void FillClosedCurve(Brush brush, Point[] points,
								FillMode fillMode, float tension)
			{
				points = ConvertPoints(points, 4);
				lock(this)
				{
					SelectBrush(brush);
					ToolkitGraphics.FillClosedCurve
						(points, tension, fillMode);
				}
			}
	public void FillClosedCurve(Brush brush, PointF[] points,
								FillMode fillMode, float tension)
			{
				Point[] dpoints = ConvertPoints(points, 4);
				lock(this)
				{
					SelectBrush(brush);
					ToolkitGraphics.FillClosedCurve
						(dpoints, tension, fillMode);
				}
			}

	// Fill an ellipse.
	public void FillEllipse(Brush brush, Rectangle rect)
			{
				FillEllipse(brush, rect.X, rect.Y, rect.Width, rect.Height);
			}
	public void FillEllipse(Brush brush, RectangleF rect)
			{
				FillEllipse(brush, rect.X, rect.Y, rect.Width, rect.Height);
			}
	public void FillEllipse(Brush brush, int x, int y, int width, int height)
			{
				Point[] rect = ConvertRectangle(x, y, width, height);
				lock(this)
				{
					SelectBrush(brush);
					ToolkitGraphics.FillPie(rect, 0.0f, 360.0f);
				}
			}
	public void FillEllipse(Brush brush, float x, float y,
							float width, float height)
			{
				Point[] rect = ConvertRectangle(x, y, width, height);
				lock(this)
				{
					SelectBrush(brush);
					ToolkitGraphics.FillPie(rect, 0.0f, 360.0f);
				}
			}

	// Fill the interior of a path.
	public void FillPath(Brush brush, GraphicsPath path)
			{
				if(brush == null)
				{
					throw new ArgumentNullException("brush");
				}
				if(path == null)
				{
					throw new ArgumentNullException("path");
				}
				path.Fill(this, brush);
			}

	// Fill a pie shape.
	public void FillPie(Brush brush, Rectangle rect,
						float startAngle, float sweepAngle)
			{
				if(((float)(int)startAngle) == startAngle &&
				   ((float)(int)sweepAngle) == sweepAngle)
				{
					FillPie(brush, rect.X, rect.Y, rect.Width, rect.Height,
							(int)startAngle, (int)sweepAngle);
				}
				else
				{
					FillPie(brush, rect.X, rect.Y, rect.Width, rect.Height,
							startAngle, sweepAngle);
				}
			}
	public void FillPie(Brush brush, int x, int y, int width, int height,
						int startAngle, int sweepAngle)
			{
				Point[] rect = ConvertRectangle(x, y, width, height);
				lock(this)
				{
					SelectBrush(brush);
					ToolkitGraphics.FillPie(rect, startAngle, sweepAngle);
				}
			}
	public void FillPie(Brush brush, float x, float y,
						float width, float height,
						float startAngle, float sweepAngle)
			{
				Point[] rect = ConvertRectangle(x, y, width, height);
				lock(this)
				{
					SelectBrush(brush);
					ToolkitGraphics.FillPie(rect, startAngle, sweepAngle);
				}
			}

	// Fill a polygon.
	public void FillPolygon(Brush brush, Point[] points)
			{
				FillPolygon(brush, points, FillMode.Alternate);
			}
	public void FillPolygon(Brush brush, PointF[] points)
			{
				FillPolygon(brush, points, FillMode.Alternate);
			}
	public void FillPolygon(Brush brush, Point[] points, FillMode fillMode)
			{
				points = ConvertPoints(points, 2);
				lock(this)
				{
					SelectBrush(brush);
					ToolkitGraphics.FillPolygon(points, fillMode);
				}
			}
	public void FillPolygon(Brush brush, PointF[] points, FillMode fillMode)
			{
				Point[] dpoints = ConvertPoints(points, 2);
				lock(this)
				{
					SelectBrush(brush);
					ToolkitGraphics.FillPolygon(dpoints, fillMode);
				}
			}

	// Fill a rectangle.
	public void FillRectangle(Brush brush, Rectangle rect)
			{
				FillRectangle(brush, rect.X, rect.Y, rect.Width, rect.Height);
			}
	public void FillRectangle(Brush brush, RectangleF rect)
			{
				FillRectangle(brush, rect.X, rect.Y, rect.Width, rect.Height);
			}
	public void FillRectangle(Brush brush, int x, int y, int width, int height)
			{
				lock(this)
				{
					SelectBrush(brush);
					ToolkitGraphics.FillPolygon(ConvertRectangle(x, y, width, height), FillMode.Alternate);
				}
			}
	public void FillRectangle(Brush brush, float x, float y,
							  float width, float height)
			{
				lock(this)
				{
					SelectBrush(brush);
					ToolkitGraphics.FillPolygon(ConvertRectangle(x, y, width, height), FillMode.Alternate);
				}
			}

	// Fill a series of rectangles.
	public void FillRectangles(Brush brush, Rectangle[] rects)
			{
				if(rects == null)
				{
					throw new ArgumentNullException("rects");
				}
				int posn;
				for(posn = 0; posn < rects.Length; ++posn)
				{
					FillRectangle(brush, rects[posn].X, rects[posn].Y,
								  rects[posn].Width, rects[posn].Height);
				}
			}
	public void FillRectangles(Brush brush, RectangleF[] rects)
			{
				if(rects == null)
				{
					throw new ArgumentNullException("rects");
				}
				int posn;
				for(posn = 0; posn < rects.Length; ++posn)
				{
					FillRectangle(brush, rects[posn].X, rects[posn].Y,
								  rects[posn].Width, rects[posn].Height);
				}
			}

	// Fill a region.
	public void FillRegion(Brush brush, Region region)
			{
				RectangleF[] rs = region.GetRegionScans(new Drawing.Drawing2D.Matrix());
				for (int i = 0; i < rs.Length; i++)
				{
					Rectangle b = Rectangle.Truncate(rs[i]);
					FillRectangle(brush, rs[i]);
				}
			}

	// Flush graphics operations to the display device.
	public void Flush()
			{
				Flush(FlushIntention.Flush);
			}
	public void Flush(FlushIntention intention)
			{
				lock(this)
				{
					if(graphics != null)
					{
						graphics.Flush(intention);
					}
				}
			}

	// Create a Graphics object from a HDC.
	public static Graphics FromHdc(IntPtr hdc)
			{
				return new Graphics
					(ToolkitManager.Toolkit.CreateFromHdc(hdc, IntPtr.Zero));
			}
	public static Graphics FromHdc(IntPtr hdc, IntPtr hdevice)
			{
				return new Graphics
					(ToolkitManager.Toolkit.CreateFromHdc(hdc, hdevice));
			}
	public static Graphics FromHdcInternal(IntPtr hdc)
			{
				return FromHdc(hdc);
			}

	// Create a Graphics object from a HWND.
	public static Graphics FromHwnd(IntPtr hwnd)
			{
				return new Graphics
					(ToolkitManager.Toolkit.CreateFromHwnd(hwnd));
			}
	public static Graphics FromHwndInternal(IntPtr hwnd)
			{
				return FromHwnd(hwnd);
			}

	// Create a graphics object for drawing into an image.
	[TODO]
	public static Graphics FromImage(Image image)
			{
				// TODO
				return null;
			}

	// Get the handle for the Windows halftone palette.  Not used here.
	public static IntPtr GetHalftonePalette()
			{
				return ToolkitManager.Toolkit.GetHalftonePalette();
			}

	// Get the HDC associated with this graphics object.
	public IntPtr GetHdc()
			{
				lock(this)
				{
					if(graphics != null)
					{
						return graphics.GetHdc();
					}
					else
					{
						return IntPtr.Zero;
					}
				}
			}

	// Get the nearest color that is supported by this graphics object.
	public Color GetNearestColor(Color color)
			{
				lock(this)
				{
					if(graphics != null)
					{
						return graphics.GetNearestColor(color);
					}
					else
					{
						return color;
					}
				}
			}

	// Intersect a region with the current clipping region.
	public void IntersectClip(Rectangle rect)
			{
				Clip.Intersect(rect);
				UpdateClip();
			}
	public void IntersectClip(RectangleF rect)
			{
				Clip.Intersect(rect);
				UpdateClip();
			}
	public void IntersectClip(Region region)
			{
				Clip.Intersect(region);
				UpdateClip();
			}

	// Determine if a point is within the visible clip region.
	public bool IsVisible(Point point)
			{
				return Clip.IsVisible(point, this);
			}
	public bool IsVisible(PointF point)
			{
				return Clip.IsVisible(point, this);
			}
	public bool IsVisible(Rectangle rect)
			{
				return Clip.IsVisible(rect, this);
			}
	public bool IsVisible(RectangleF rect)
			{
				return Clip.IsVisible(rect, this);
			}
	public bool IsVisible(int x, int y)
			{
				return Clip.IsVisible(x, y, this);
			}
	public bool IsVisible(float x, float y)
			{
				return Clip.IsVisible(x, y, this);
			}
	public bool IsVisible(int x, int y, int width, int height)
			{
				return Clip.IsVisible(x, y, width, height, this);
			}
	public bool IsVisible(float x, float y, float width, float height)
			{
				return Clip.IsVisible(x, y, width, height, this);
			}

	// Measure the character ranges for a string.
	// TODO not complete
	public Region[] MeasureCharacterRanges(String text,
		Font font, RectangleF layoutRect, StringFormat stringFormat)
			{
				RectangleF[] bounds = new RectangleF[text.Length];
				float xMax = 0;
				float yMax = 0;
				float fontHeight = font.GetHeight(this) - 1;
				bool vertical = (stringFormat.FormatFlags & StringFormatFlags.DirectionVertical) != 0;
				if (vertical)
				{
					xMax = layoutRect.Height - 1;
					yMax = layoutRect.Width - 1;
				}
				else
				{
					xMax = layoutRect.Width - 1;
					yMax = layoutRect.Height - 1;
				}
				bool noWrap = (stringFormat.FormatFlags & StringFormatFlags.NoWrap) != 0;
				//TODO stringFormat.LineAlignment;
				int[] lines = new int[20];
				float lineSizeRemaining = yMax;
				int currentLine = 0;
				// First line starts at 0
				lines[currentLine++] = 0;
				int currentPos = 0;
				do
				{
					MeasureLine (ref bounds, ref currentPos, ref text, xMax, this, font, vertical, noWrap);
					lines[currentLine++] = currentPos;
					if (currentPos < text.Length && text[currentPos] == (char)10)
						currentPos++;
					lineSizeRemaining -= fontHeight;
					if (currentPos >= text.Length || lineSizeRemaining < 0)
						break;
				} while (true);
				
				int nextLine = 0;
				float xOffset = 0;
				for (int i = 0; i < text.Length; i++)
				{
					if (lines[nextLine] == i)
					{
						xOffset = 0;
						nextLine++;
						if (stringFormat.Alignment == StringAlignment.Far)
							xOffset = xMax + 1 - bounds[lines[nextLine] - 1].Right;
						else if (stringFormat.Alignment == StringAlignment.Center)
						{
							for (int j = i; j < lines[nextLine]; j++)
								xOffset += bounds[j].Width;
							xOffset = (xMax + 1 - xOffset)/2;
						}
					}
					if (bounds[i] != Rectangle.Empty)
					{
						RectangleF rect = bounds[i];
						bounds[i] = new RectangleF( rect.Left + xOffset + layoutRect.Left, rect.Top + (nextLine - 1) * fontHeight + layoutRect.Top, rect.Width, rect.Height);
					}
				}

				// Now consolidate positions based on character ranges
				Region[] regions = new Region[stringFormat.ranges.Length];
				for (int i = 0; i < stringFormat.ranges.Length; i++)
				{
					CharacterRange range = stringFormat.ranges[i];
					Region region = new Region(RectangleF.Empty);
					for (int j = range.First; j < range.First + range.Length; j++)
						region.Union(bounds[j]);
					regions[i] = region;
				}
				return regions;
			}

	// Measures one full line. Updates the positions of the characters in that line relative to 0,0
	private void MeasureLine(ref RectangleF[] bounds, ref int currentPos,
		ref string text, float maxX, Graphics g, Font f, bool vertical, bool noWrap)
			{
				int x = 0;
				int y = 0;
				do
				{
					char c = text[currentPos];
					if (c == (char)10)
						return;
					// Ignore char 13
					if (c!= (char)13)
					{
						//TODO use Platform specific measure function & take into account kerning
						Size s = g.MeasureString( c.ToString(), f).ToSize();
						int newX = x;
						int newY = y;
						if (vertical)
							newX += s.Height;
						else
							newX += s.Width;
						if (newX > maxX)
						{
							if (!noWrap)
								return;
						}
						else
						{
							if (vertical)
								bounds[currentPos] = new RectangleF( y, x, s.Height, s.Width );
							else
								bounds[currentPos] = new RectangleF( x, y, s.Width, s.Height );
						}
						x = newX;
					}
					currentPos++;
				} while (currentPos < text.Length);
			}

	// Measure the size of a string.
	public SizeF MeasureString(String text, Font font)
			{
				return MeasureString(text, font, new SizeF(0.0f, 0.0f), null);
			}
	public SizeF MeasureString(String text, Font font, int width)
			{
				return MeasureString
					(text, font, new SizeF(width, 999999.0f), null);
			}
	public SizeF MeasureString(String text, Font font, SizeF layoutArea)
			{
				return MeasureString(text, font, layoutArea, null);
			}
	public SizeF MeasureString(String text, Font font,
							   int width, StringFormat format)
			{
				return MeasureString
					(text, font, new SizeF(width, 999999.0f), format);
			}
	public SizeF MeasureString(String text, Font font,
							   PointF origin, StringFormat format)
			{
				return MeasureString
					(text, font, new SizeF(0.0f, 0.0f), format);
			}
	public SizeF MeasureString(String text, Font font,
							   SizeF layoutArea, StringFormat format)
			{
				int charactersFitted, linesFilled;
				return MeasureString(text, font, layoutArea, format,
									 out charactersFitted, out linesFilled);
			}
	public SizeF MeasureString(String text, Font font,
							   SizeF layoutArea, StringFormat format,
							   out int charactersFitted,
							   out int linesFilled)
			{
				if(text == null || text == String.Empty)
				{
					charactersFitted = 0;
					linesFilled = 0;
					return new SizeF(0.0f, 0.0f);
				}
				Point[] rect = ConvertRectangle
						(0.0f, 0.0f, layoutArea.Width, layoutArea.Height);
				lock(this)
				{
					SelectFont(font);
					Size size = ToolkitGraphics.MeasureString
						(text, rect, format,
						 out charactersFitted, out linesFilled, false);
					return new SizeF(ConvertSizeBack(size.Width, size.Height));
				}
			}

	// Multiply the transformation matrix by a specific amount.
	public void MultiplyTransform(Matrix matrix)
			{
				Transform.Multiply(matrix);
				if(transform.IsIdentity)
				{
					transform = null;
				}
			}
	public void MultiplyTransform(Matrix matrix, MatrixOrder order)
			{
				Transform.Multiply(matrix, order);
				if(transform.IsIdentity)
				{
					transform = null;
				}
			}

	// Release a HDC that was obtained via a previous call to "GetHdc".
	public void ReleaseHdc(IntPtr hdc)
			{
				lock(this)
				{
					if(graphics != null)
					{
						graphics.ReleaseHdc(hdc);
					}
				}
			}
	public void ReleaseHdcInternal(IntPtr hdc)
			{
				ReleaseHdc(hdc);
			}

	// Reset the clipping region.
	public void ResetClip()
			{
				Clip = new Region();
				UpdateClip();
			}

	// Reset the transformation matrix to identity.
	public void ResetTransform()
			{
				transform = null;
			}

	// Restore to a previous save point.
	public void Restore(GraphicsState gstate)
			{
				if(gstate != null)
				{
					lock(this)
					{
						gstate.Restore(this);
					}
				}
			}

	// Apply a rotation to the transformation matrix.
	public void RotateTransform(float angle)
			{
				Transform.Rotate(angle);
				if(transform.IsIdentity)
				{
					transform = null;
				}
			}
	public void RotateTransform(float angle, MatrixOrder order)
			{
				Transform.Rotate(angle, order);
				if(transform.IsIdentity)
				{
					transform = null;
				}
			}

	// Save the current graphics state.
	public GraphicsState Save()
			{
				lock(this)
				{
					return new GraphicsState(this);
				}
			}

	// Apply a scaling factor to the transformation matrix.
	public void ScaleTransform(float sx, float sy)
			{
				Transform.Scale(sx, sy);
				if(transform.IsIdentity)
				{
					transform = null;
				}
			}
	public void ScaleTransform(float sx, float sy, MatrixOrder order)
			{
				Transform.Scale(sx, sy, order);
				if(transform.IsIdentity)
				{
					transform = null;
				}
			}

	// Set the clipping region of this graphics object.
	public void SetClip(Graphics g)
			{
				if(g == null)
				{
					throw new ArgumentNullException("g");
				}
				clip = g.Clip.Clone();
				UpdateClip();
			}
	public void SetClip(Graphics g, CombineMode combineMode)
			{
				if(g == null)
				{
					throw new ArgumentNullException("g");
				}
				Region other = g.Clip;
				switch(combineMode)
				{
					case CombineMode.Replace:	clip = other.Clone(); break;

					case CombineMode.Intersect:
					{
						Clip.Intersect(other);
					}
					break;

					case CombineMode.Union:
					{
						Clip.Union(other);
					}
					break;

					case CombineMode.Xor:
					{
						Clip.Xor(other);
					}
					break;

					case CombineMode.Exclude:
					{
						Clip.Exclude(other);
					}
					break;

					case CombineMode.Complement:
					{
						Clip.Complement(other);
					}
					break;

					default: return;
				}
				UpdateClip();
			}
	public void SetClip(GraphicsPath path)
			{
				clip = new Region(path);
				UpdateClip();
			}
	public void SetClip(GraphicsPath path, CombineMode combineMode)
			{
				if(path == null)
				{
					throw new ArgumentNullException("path");
				}
				switch(combineMode)
				{
					case CombineMode.Replace:
					{
						clip = new Region(path);
					}
					break;

					case CombineMode.Intersect:
					{
						Clip.Intersect(path);
					}
					break;

					case CombineMode.Union:
					{
						Clip.Union(path);
					}
					break;

					case CombineMode.Xor:
					{
						Clip.Xor(path);
					}
					break;

					case CombineMode.Exclude:
					{
						Clip.Exclude(path);
					}
					break;

					case CombineMode.Complement:
					{
						Clip.Complement(path);
					}
					break;

					default: return;
				}
				UpdateClip();
			}
	public void SetClip(Rectangle rect)
			{
				clip = new Region(rect);
				UpdateClip();
			}
	public void SetClip(Rectangle rect, CombineMode combineMode)
			{
				switch(combineMode)
				{
					case CombineMode.Replace:
					{
						clip = new Region(rect);
					}
					break;

					case CombineMode.Intersect:
					{
						Clip.Intersect(rect);
					}
					break;

					case CombineMode.Union:
					{
						Clip.Union(rect);
					}
					break;

					case CombineMode.Xor:
					{
						Clip.Xor(rect);
					}
					break;

					case CombineMode.Exclude:
					{
						Clip.Exclude(rect);
					}
					break;

					case CombineMode.Complement:
					{
						Clip.Complement(rect);
					}
					break;

					default: return;
				}
				UpdateClip();
			}
	public void SetClip(RectangleF rect)
			{
				clip = new Region(rect);
				UpdateClip();
			}
	public void SetClip(RectangleF rect, CombineMode combineMode)
			{
				switch(combineMode)
				{
					case CombineMode.Replace:
					{
						clip = new Region(rect);
					}
					break;

					case CombineMode.Intersect:
					{
						Clip.Intersect(rect);
					}
					break;

					case CombineMode.Union:
					{
						Clip.Union(rect);
					}
					break;

					case CombineMode.Xor:
					{
						Clip.Xor(rect);
					}
					break;

					case CombineMode.Exclude:
					{
						Clip.Exclude(rect);
					}
					break;

					case CombineMode.Complement:
					{
						Clip.Complement(rect);
					}
					break;

					default: return;
				}
				UpdateClip();
			}
	public void SetClip(Region region, CombineMode combineMode)
			{
				if(region == null)
				{
					throw new ArgumentNullException("region");
				}
				switch(combineMode)
				{
					case CombineMode.Replace:
					{
						clip = region.Clone();
					}
					break;

					case CombineMode.Intersect:
					{
						Clip.Intersect(region);
					}
					break;

					case CombineMode.Union:
					{
						Clip.Union(region);
					}
					break;

					case CombineMode.Xor:
					{
						Clip.Xor(region);
					}
					break;

					case CombineMode.Exclude:
					{
						Clip.Exclude(region);
					}
					break;

					case CombineMode.Complement:
					{
						Clip.Complement(region);
					}
					break;

					default: return;
				}
				UpdateClip();
			}

	// Transform points from one co-ordinate space to another.
	[TODO]
	public void TransformPoints(CoordinateSpace destSpace,
								CoordinateSpace srcSpace,
								Point[] pts)
			{
				// TODO
			}
	[TODO]
	public void TransformPoints(CoordinateSpace destSpace,
								CoordinateSpace srcSpace,
								PointF[] pts)
			{
				// TODO
			}

	// Translate the clipping region by a specified amount.
	[TODO]
	public void TranslateClip(int dx, int dy)
			{
				Region clip = Clip;
				clip.Translate(dx, dy);
				Clip = clip;
			}
	public void TranslateClip(float dx, float dy)
			{
				Region clip = Clip;
				clip.Translate(dx, dy);
				Clip = clip;
			}

	// Apply a translation to the transformation matrix.
	public void TranslateTransform(float dx, float dy)
			{
				Transform.Translate(dx, dy);
				if(transform.IsIdentity)
				{
					transform = null;
				}
			}
	public void TranslateTransform(float dx, float dy, MatrixOrder order)
			{
				Transform.Translate(dx, dy, order);
				if(transform.IsIdentity)
				{
					transform = null;
				}
			}

	// Delegate that is used to handle abort callbacks on "DrawImage".
#if !ECMA_COMPAT
	[Serializable]
	[ComVisible(false)]
#endif
	public delegate bool DrawImageAbort(IntPtr callbackdata);

	// Delegate that is used to enumerate metafile contents.
#if !ECMA_COMPAT
	[Serializable]
	[ComVisible(false)]
#endif
	public delegate bool EnumerateMetafileProc
			(EmfPlusRecordType recordType, int flags, int dataSize,
			 IntPtr data, PlayRecordCallback callbackData);

	// Convert a point into device pixels.
	private void ConvertPoint(ref int x, ref int y)
			{
				float newX, newY;
				float adjustX, adjustY;

				// Apply the world transform first.
				if(transform != null)
				{
					transform.TransformPoint(x, y, out newX, out newY);
				}
				else if(pageScale == 1.0f)
				{
					// Transform is identity and the page scale is 1,
					// so we can bail out early if we are using pixels
					// for the page unit, as there will be no change.
					if(pageUnit == GraphicsUnit.World ||
					   pageUnit == GraphicsUnit.Pixel)
					{
						return;
					}
					newX = (float)x;
					newY = (float)y;
				}
				else
				{
					newX = (float)x;
					newY = (float)y;
				}

				// Apply the page scale factor.
				if(pageScale != 1.0f)
				{
					newX *= pageScale;
					newY *= pageScale;
				}

				// Apply the page unit to get device co-ordinates.
				switch(pageUnit)
				{
					case GraphicsUnit.World:
					case GraphicsUnit.Pixel:
					default:
					{
						// We are finished - no more adjustments are necessary.
						x = (int)newX;
						y = (int)newY;
						return;
					}
					// Not reached.

					case GraphicsUnit.Display:
					{
						adjustX = DpiX / 75.0f;
						adjustY = DpiY / 75.0f;
					}
					break;

					case GraphicsUnit.Point:
					{
						adjustX = DpiX / 72.0f;
						adjustY = DpiY / 72.0f;
					}
					break;

					case GraphicsUnit.Inch:
					{
						adjustX = DpiX;
						adjustY = DpiY;
					}
					break;

					case GraphicsUnit.Document:
					{
						adjustX = DpiX / 300.0f;
						adjustY = DpiY / 300.0f;
					}
					break;

					case GraphicsUnit.Millimeter:
					{
						adjustX = DpiX / 25.4f;
						adjustY = DpiY / 25.4f;
					}
					break;
				}
				x = (int)(newX * adjustX);
				y = (int)(newY * adjustY);
			}
	private void ConvertPoint(float x, float y, out int dx, out int dy)
			{
				float newX, newY;
				float adjustX, adjustY;

				// Apply the world transform first.
				if(transform != null)
				{
					transform.TransformPoint(x, y, out newX, out newY);
				}
				else
				{
					newX = x;
					newY = y;
				}

				// Apply the page scale factor.
				if(pageScale != 1.0f)
				{
					newX *= pageScale;
					newY *= pageScale;
				}

				// Apply the page unit to get device co-ordinates.
				switch(pageUnit)
				{
					case GraphicsUnit.World:
					case GraphicsUnit.Pixel:
					default:
					{
						// We are finished - no more adjustments are necessary.
						dx = (int)newX;
						dy = (int)newY;
						return;
					}
					// Not reached.

					case GraphicsUnit.Display:
					{
						adjustX = DpiX / 75.0f;
						adjustY = DpiY / 75.0f;
					}
					break;

					case GraphicsUnit.Point:
					{
						adjustX = DpiX / 72.0f;
						adjustY = DpiY / 72.0f;
					}
					break;

					case GraphicsUnit.Inch:
					{
						adjustX = DpiX;
						adjustY = DpiY;
					}
					break;

					case GraphicsUnit.Document:
					{
						adjustX = DpiX / 300.0f;
						adjustY = DpiY / 300.0f;
					}
					break;

					case GraphicsUnit.Millimeter:
					{
						adjustX = DpiX / 25.4f;
						adjustY = DpiY / 25.4f;
					}
					break;
				}
				dx = (int)(newX * adjustX);
				dy = (int)(newY * adjustY);
			}

	// Convert a list of points into device pixels.
	private Point[] ConvertPoints(Point[] points, int minPoints)
			{
				// Validate the parameter.
				if(points == null)
				{
					throw new ArgumentNullException("points");
				}
				if(points.Length < minPoints)
				{
					throw new ArgumentException
						(String.Format
							(S._("Arg_NeedsAtLeastNPoints"), minPoints));
				}
		
				// If we are using the identity transformation,
				// then bail out early using the "points" array.
				if((pageUnit == GraphicsUnit.World && transform == null) ||
					pageUnit == GraphicsUnit.Pixel)
				{
					if(pageScale == 1.0f)
					{
						return points;
					}
				}

				// Convert the "points" array.
				Point[] newPoints = new Point [points.Length];
				int x, y;
				int posn;
				for(posn = 0; posn < points.Length; ++posn)
				{
					x = points[posn].X;
					y = points[posn].Y;
					ConvertPoint(ref x, ref y);
					newPoints[posn] = new Point(x, y);
				}
				return newPoints;
			}
	private Point[] ConvertPoints(PointF[] points, int minPoints)
			{
				// Validate the parameter.
				if(points == null)
				{
					throw new ArgumentNullException("points");
				}
				if(points.Length < minPoints)
				{
					throw new ArgumentException
						(String.Format
							(S._("Arg_NeedsAtLeastNPoints"), minPoints));
				}

				// Convert the "points" array.
				Point[] newPoints = new Point [points.Length];
				int x, y;
				int posn;
				for(posn = 0; posn < points.Length; ++posn)
				{
					ConvertPoint(points[posn].X, points[posn].Y, out x, out y);	
					newPoints[posn] = new Point(x, y);
				}
				return newPoints;
			}

	// Convert a rectangle into a set of 4 device co-ordinates.
	// The result may be a parallelogram, not a rectangle.
	private Point[] ConvertRectangle(int x, int y, int width, int height)
			{
				Point[] points = new Point [4];
				points[0] = new Point(x, y);
				points[1] = new Point(x + width - 1, y);
				points[2] = new Point(x + width - 1, y + height - 1);
				points[3] = new Point(x, y + height - 1);
				return ConvertPoints(points, 4);
			}
	private Point[] ConvertRectangle(float x, float y,
									 float width, float height)
			{
				PointF[] points = new PointF [4];
				points[0] = new PointF(x, y);
				points[1] = new PointF(x + width - 1, y);
				points[2] = new PointF(x + width - 1, y + height - 1);
				points[3] = new PointF(x, y + height - 1);
				return ConvertPoints(points, 4);
			}

	// Convert a size value from device co-ordinates to graphics units.
	private SizeF ConvertSizeBack(int width, int height)
			{
				float newX, newY, adjustX, adjustY;

				// Bail out early if the context is using pixel units.
				if(IsPixelUnits())
				{
					return new SizeF((float)width, (float)height);
				}

				// Apply the page unit to get page co-ordinates.
				newX = (float)width;
				newY = (float)height;
				switch(pageUnit)
				{
					case GraphicsUnit.World:
					case GraphicsUnit.Pixel:
					default:
					{
						adjustX = 1.0f;
						adjustY = 1.0f;
					}
					break;

					case GraphicsUnit.Display:
					{
						adjustX = DpiX / 75.0f;
						adjustY = DpiY / 75.0f;
					}
					break;

					case GraphicsUnit.Point:
					{
						adjustX = DpiX / 72.0f;
						adjustY = DpiY / 72.0f;
					}
					break;

					case GraphicsUnit.Inch:
					{
						adjustX = DpiX;
						adjustY = DpiY;
					}
					break;

					case GraphicsUnit.Document:
					{
						adjustX = DpiX / 300.0f;
						adjustY = DpiY / 300.0f;
					}
					break;

					case GraphicsUnit.Millimeter:
					{
						adjustX = DpiX / 25.4f;
						adjustY = DpiY / 25.4f;
					}
					break;
				}
				newX /= adjustX;
				newY /= adjustY;

				// Apply the inverse of the page scale factor.
				if(pageScale != 1.0f)
				{
					newX /= pageScale;
					newY /= pageScale;
				}

				// Apply the inverse of the world transform.
				if(transform != null)
				{
					transform.TransformSizeBack
						(newX, newY, out adjustX, out adjustY);
					return new SizeF(adjustX, adjustY);
				}
				else
				{
					return new SizeF(newX, newY);
				}
			}

	// Get the toolkit graphics object underlying this object.
	internal IToolkitGraphics ToolkitGraphics
			{
				get
				{
					if(graphics != null)
					{
						return graphics;
					}
					throw new ObjectDisposedException("graphics");
				}
			}

	// Get the default graphics object for the current toolkit.
	internal static Graphics DefaultGraphics
			{
				get
				{
					lock(typeof(Graphics))
					{
						if(defaultGraphics == null)
						{
							defaultGraphics = new Graphics
								(ToolkitManager.Toolkit.GetDefaultGraphics());
							defaultGraphics.PageUnit = GraphicsUnit.Pixel;
						}
						return defaultGraphics;
					}
				}
			}

	// Select a pen into the toolkit graphics object.
	private void SelectPen(Pen pen)
			{
				if(pen == null)
				{
					throw new ArgumentNullException("pen");
				}
				if(graphics == null)
				{
					throw new ObjectDisposedException("graphics");
				}
				IToolkitPen tpen = pen.GetPen(graphics.Toolkit);
				tpen.Select(graphics);
			}

	// Select a brush into the toolkit graphics object.
	private void SelectBrush(Brush brush)
			{
				if(brush == null)
				{
					throw new ArgumentNullException("brush");
				}
				if(graphics == null)
				{
					throw new ObjectDisposedException("graphics");
				}
				IToolkitBrush tbrush = brush.GetBrush(graphics.Toolkit);
				tbrush.Select(graphics);
			}

	// Select a font into the toolkit graphics object.
	private void SelectFont(Font font)
			{
				if(font == null)
				{
					throw new ArgumentNullException("font");
				}
				if(graphics == null)
				{
					throw new ObjectDisposedException("graphics");
				}
				IToolkitFont tfont = font.GetFont(graphics.Toolkit);
				tfont.Select(graphics);
			}

	// Update the clipping region within the IToolkitGraphics object.
	private void UpdateClip()
			{
				RectangleF[] rectsF = clip.GetRegionScans(new Matrix());
				Rectangle[] rects = new Rectangle[rectsF.Length];
				for(int i=0;i < rectsF.Length; i++)
					rects[i] = Rectangle.Truncate(rectsF[i]);
				graphics.SetClipRects(rects);
			}

	// Determine if this graphics object is using 1-to-1 pixel mappings.
	internal bool IsPixelUnits()
			{
				if((pageUnit == GraphicsUnit.Pixel ||
				    pageUnit == GraphicsUnit.World) &&
				   transform == null && pageScale == 1.0f)
				{
					return true;
				}
				return false;
			}

	// Get the line spacing of a font, in pixels.
	internal int GetLineSpacing(Font font)
			{
				lock(this)
				{
					SelectFont(font);
					return ToolkitGraphics.GetLineSpacing();
				}
			}

}; // class Graphics

}; // namespace System.Drawing
