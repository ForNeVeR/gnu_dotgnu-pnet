/*
 * ToolkitGraphicsBase.cs - Implementation of the
 *			"System.Drawing.Toolkit.ToolkitGraphicsBase" class.
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

using System.Drawing.Text;
using System.Drawing.Drawing2D;

// This base class provides some common functionality which should
// help to make it easier to write "IToolkitGraphics" handlers.

public abstract class ToolkitGraphicsBase : IToolkitGraphics
{
	// Dirty bit flags for changed values.
	[Flags]
	public enum DirtyFlags
	{
		CompositingMode		= (1 << 1),
		CompositingQuality	= (1 << 2),
		InterpolationMode	= (1 << 3),
		PixelOffsetMode		= (1 << 4),
		RenderingOrigin		= (1 << 5),
		SmoothingMode		= (1 << 6),
		TextContrast		= (1 << 7),
		TextRenderingHint	= (1 << 8),
		All					= -1

	}; // enum DirtyFlags

	// Internal state.
	protected IToolkit toolkit;
	protected Region clip;
	protected CompositingMode compositingMode;
	protected CompositingQuality compositingQuality;
	protected InterpolationMode interpolationMode;
	protected PixelOffsetMode pixelOffsetMode;
	protected Point renderingOrigin;
	protected SmoothingMode smoothingMode;
	protected int textContrast;
	protected TextRenderingHint textRenderingHint;
	protected DirtyFlags dirtyFlags;

	// Constructor.
	protected ToolkitGraphicsBase(IToolkit toolkit)
			{
				this.toolkit = toolkit;
				clip = null;
				compositingMode = CompositingMode.SourceOver;
				compositingQuality = CompositingQuality.Default;
				interpolationMode = InterpolationMode.Default;
				pixelOffsetMode = PixelOffsetMode.Default;
				renderingOrigin = new Point(0, 0);
				smoothingMode = SmoothingMode.Default;
				textContrast = 4;
				textRenderingHint = TextRenderingHint.SystemDefault;
				dirtyFlags = DirtyFlags.All;
			}

	// Get or set the graphics object's properties.
	public IToolkit Toolkit
			{
				get
				{
					return toolkit;
				}
			}
	public virtual CompositingMode CompositingMode
			{
				get
				{
					return compositingMode;
				}
				set
				{
					if(compositingMode != value)
					{
						compositingMode = value;
						dirtyFlags |= DirtyFlags.CompositingMode;
					}
				}
			}
	public virtual CompositingQuality CompositingQuality
			{
				get
				{
					return compositingQuality;
				}
				set
				{
					if(compositingQuality != value)
					{
						compositingQuality = value;
						dirtyFlags |= DirtyFlags.CompositingQuality;
					}
				}
			}
	public virtual float DpiX
			{
				get
				{
					// Assume a display with a default DPI of 75.
					return 75.0f;
				}
			}
	public virtual float DpiY
			{
				get
				{
					// Assume a display with a default DPI of 75.
					return 75.0f;
				}
			}
	public virtual InterpolationMode InterpolationMode
			{
				get
				{
					return interpolationMode;
				}
				set
				{
					if(interpolationMode != value)
					{
						interpolationMode = value;
						dirtyFlags |= DirtyFlags.InterpolationMode;
					}
				}
			}
	public virtual PixelOffsetMode PixelOffsetMode
			{
				get
				{
					return pixelOffsetMode;
				}
				set
				{
					if(pixelOffsetMode != value)
					{
						pixelOffsetMode = value;
						dirtyFlags |= DirtyFlags.PixelOffsetMode;
					}
				}
			}
	public virtual Point RenderingOrigin
			{
				get
				{
					return renderingOrigin;
				}
				set
				{
					if(renderingOrigin != value)
					{
						renderingOrigin = value;
						dirtyFlags |= DirtyFlags.RenderingOrigin;
					}
				}
			}
	public virtual SmoothingMode SmoothingMode
			{
				get
				{
					return smoothingMode;
				}
				set
				{
					if(smoothingMode != value)
					{
						smoothingMode = value;
						dirtyFlags |= DirtyFlags.SmoothingMode;
					}
				}
			}
	public virtual int TextContrast
			{
				get
				{
					return textContrast;
				}
				set
				{
					if(textContrast != value)
					{
						textContrast = value;
						dirtyFlags |= DirtyFlags.TextContrast;
					}
				}
			}
	public virtual TextRenderingHint TextRenderingHint
			{
				get
				{
					return textRenderingHint;
				}
				set
				{
					if(textRenderingHint != value)
					{
						textRenderingHint = value;
						dirtyFlags |= DirtyFlags.TextRenderingHint;
					}
				}
			}
	public virtual RectangleF VisibleClipBounds
			{
				get
				{
					// This should be overridden in subclasses.
					return RectangleF.Empty;
				}
			}

	// Determine if a particular section of the dirty flags are set.
	protected bool IsDirty(DirtyFlags flags)
			{
				return ((dirtyFlags & flags) != 0);
			}

	// Check if dirty flags are set and also clear them.
	protected bool CheckDirty(DirtyFlags flags)
			{
				bool result = ((dirtyFlags & flags) != 0);
				dirtyFlags &= ~flags;
				return result;
			}

	// Clear specific dirty flags.
	protected void ClearDirty(DirtyFlags flags)
			{
				dirtyFlags &= ~flags;
			}

	// Dispose of this object.
	public virtual void Dispose()
			{
				// Nothing to do in this base class.
			}

	// Clear the entire drawing surface.
	public abstract void Clear();

	// Draw a line between two points using the current pen.
	public abstract void DrawLine(int x1, int y1, int x2, int y2);

	// Draw a set of connected line seguments using the current pen.
	public abstract void DrawLines(Point[] points);

	// Draw a polygon using the current pen.
	public abstract void DrawPolygon(Point[] points);

	// Fill a polygon using the current brush.
	public abstract void FillPolygon(Point[] points, FillMode fillMode);

	// Draw a bezier curve using the current pen.
	[TODO]
	public virtual void DrawBezier(int x1, int y1, int x2, int y2,
								   int x3, int y3, int x4, int y4)
			{
				// TODO: implement a default bezier drawing algorithm
				// for systems that don't have their own.
			}

	// Draw an arc within a rectangle defined by four points.
	public abstract void DrawArc
			(Point[] rect, float startAngle, float sweepAngle);

	// Draw a pie slice within a rectangle defined by four points.
	public abstract void DrawPie
			(Point[] rect, float startAngle, float sweepAngle);

	// Fill a pie slice within a rectangle defined by four points.
	public abstract void FillPie
			(Point[] rect, float startAngle, float sweepAngle);

	// Draw a closed cardinal curve using the current pen.
	[TODO]
	public virtual void DrawClosedCurve(Point[] points, float tension)
			{
				// TODO: implement a default curve drawing algorithm
				// for systems that don't have their own.
			}

	// Fill a closed cardinal curve using the current brush.
	[TODO]
	public virtual void FillClosedCurve
				(Point[] points, float tension, FillMode fillMode)
			{
				// TODO: implement a default curve drawing algorithm
				// for systems that don't have their own.
			}

	// Draw a cardinal curve using the current pen.
	[TODO]
	public virtual void DrawCurve(Point[] points, int offset,
				   				  int numberOfSegments, float tension)
			{
				// TODO: implement a default curve drawing algorithm
				// for systems that don't have their own.
			}

	// Flush the graphics subsystem
	public virtual void Flush(FlushIntention intention)
			{
				// Nothing to do in the base class.
			}

	// Get the nearest color to a specified one.
	public virtual Color GetNearestColor(Color color)
			{
				// By default, we assume that the display is true color.
				return color;
			}

	// Add a metafile comment.
	public virtual void AddMetafileComment(byte[] data)
			{
				// Nothing to do in the base class.
			}

	// Get the HDC associated with this graphics object.
	public virtual IntPtr GetHdc()
			{
				// Nothing to do in the base class.
				return IntPtr.Zero;
			}

	// Release a HDC that was obtained using "GetHdc()".
	public virtual void ReleaseHdc(IntPtr hdc)
			{
				// Nothing to do in the base class.
			}

	// Set the clipping region to empty.
	public abstract void SetClipEmpty();

	// Set the clipping region to infinite (i.e. disable clipping).
	public abstract void SetClipInfinite();

	// Set the clipping region to a single rectangle.
	public abstract void SetClipRect(int x, int y, int width, int height);

	// Set the clipping region to a list of rectangles.
	public abstract void SetClipRects(Rectangle[] rects);

	// Set the clipping region to a complex mask.
	public abstract void SetClipMask(Object mask, int topx, int topy);

}; // class ToolkitGraphicsBase

}; // namespace System.Drawing.Toolkit
