/*
 * Region.cs - Implementation of the "System.Drawing.Region" class.
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
using System.Drawing.Toolkit;

[ComVisible(false)]
public sealed class Region : MarshalByRefObject, IDisposable
{
	// Constructors.
	public Region()
			{
				// Infinite region
				// TODO
			}
	public Region(GraphicsPath path)
			{
				if(path == null)
				{
					throw new ArgumentNullException("path");
				}
				// TODO
			}
	public Region(Rectangle rect)
			{
				// TODO
			}
	public Region(RectangleF rect)
			{
				// TODO
			}
	public Region(RegionData rgnData)
			{
				if(rgnData == null)
				{
					throw new ArgumentNullException("rgnData");
				}
				// TODO
			}

	// Destructor.
	~Region()
			{
				Dispose();
			}

	// Make an exact copy of this region.
	[TODO]
	public Region Clone()
			{
				// TODO
				return this;
			}

	// Form the complement of subtracting this region from another.
	[TODO]
	public void Complement(GraphicsPath path)
			{
				// TODO
			}
	[TODO]
	public void Complement(Rectangle rect)
			{
				// TODO
			}
	[TODO]
	public void Complement(RectangleF rect)
			{
				// TODO
			}
	[TODO]
	public void Complement(Region region)
			{
				// TODO
			}

	// Dispose of this region.
	public void Dispose()
			{
				// Nothing to do in this implementation.
			}

	// Determine if two regions are equal after applying a transformation.
	[TODO]
	public bool Equals(Region region, Graphics g)
			{
				if(region == null)
				{
					throw new ArgumentNullException("region");
				}
				if(g == null)
				{
					throw new ArgumentNullException("g");
				}
				// TODO
				return false;
			}

	// Subtract another region from this one.
	[TODO]
	public void Exclude(GraphicsPath path)
			{
				// TODO
			}
	[TODO]
	public void Exclude(Rectangle rect)
			{
				// TODO
			}
	[TODO]
	public void Exclude(RectangleF rect)
			{
				// TODO
			}
	[TODO]
	public void Exclude(Region region)
			{
				// TODO
			}

	// Create a region object from a HRGN.
	[TODO]
	public static Region FromHrgn(IntPtr hrgn)
			{
				// TODO
				return new Region();
			}

	// Get the bounds of this region on a particular graphics object.
	[TODO]
	public RectangleF GetBounds(Graphics graphics)
			{
				// TODO
				return RectangleF.Empty;
			}

	// Get a HRGN object for this region.
	[TODO]
	public IntPtr GetHrgn(Graphics g)
			{
				// TODO
				return IntPtr.Zero;
			}

	// Get the raw region data for this region.
	[TODO]
	public RegionData GetRegionData()
			{
				// TODO
				return null;
			}

	// Get an array of rectangles that represents this region.
	public RectangleF[] GetRegionScans(Matrix matrix)
			{
				// TODO
				return null;
			}

	// Form the intersection of this region and another.
	[TODO]
	public void Intersect(GraphicsPath path)
			{
				// TODO
			}
	[TODO]
	public void Intersect(Rectangle rect)
			{
				// TODO
			}
	[TODO]
	public void Intersect(RectangleF rect)
			{
				// TODO
			}
	[TODO]
	public void Intersect(Region region)
			{
				// TODO
			}

	// Determine if this region is empty on a particular graphics object.
	[TODO]
	public bool IsEmpty(Graphics g)
			{
				// TODO
				return false;
			}

	// Determine if this region is infinite on a particular graphics object.
	[TODO]
	public bool IsInfinite(Graphics g)
			{
				// TODO
				return false;
			}

	// Determine if a point is contained within this region.
	[TODO]
	public bool IsVisible(Point point)
			{
				// TODO
				return false;
			}
	[TODO]
	public bool IsVisible(PointF point)
			{
				// TODO
				return false;
			}
	[TODO]
	public bool IsVisible(Rectangle rect)
			{
				// TODO
				return false;
			}
	[TODO]
	public bool IsVisible(RectangleF rect)
			{
				// TODO
				return false;
			}
	[TODO]
	public bool IsVisible(Point point, Graphics g)
			{
				// TODO
				return false;
			}
	[TODO]
	public bool IsVisible(PointF point, Graphics g)
			{
				// TODO
				return false;
			}
	[TODO]
	public bool IsVisible(Rectangle rect, Graphics g)
			{
				// TODO
				return false;
			}
	[TODO]
	public bool IsVisible(RectangleF rect, Graphics g)
			{
				// TODO
				return false;
			}
	[TODO]
	public bool IsVisible(float x, float y)
			{
				// TODO
				return false;
			}
	[TODO]
	public bool IsVisible(int x, int y, Graphics g)
			{
				// TODO
				return false;
			}
	[TODO]
	public bool IsVisible(float x, float y, Graphics g)
			{
				// TODO
				return false;
			}
	[TODO]
	public bool IsVisible(int x, int y, int width, int height)
			{
				// TODO
				return false;
			}
	[TODO]
	public bool IsVisible(float x, float y, float width, float height)
			{
				// TODO
				return false;
			}
	[TODO]
	public bool IsVisible(int x, int y, int width, int height, Graphics g)
			{
				// TODO
				return false;
			}
	[TODO]
	public bool IsVisible(float x, float y, float width,
						  float height, Graphics g)
			{
				// TODO
				return false;
			}

	// Make this region empty.
	[TODO]
	public void MakeEmpty()
			{
				// TODO
			}

	// Make this region infinite.
	[TODO]
	public void MakeInfinite()
			{
				// TODO
			}

	// Transform this region using a specified matrix.
	[TODO]
	public void Transform(Matrix matrix)
			{
				// TODO
			}

	// Translate this matrix by a specific amount.
	[TODO]
	public void Translate(int dx, int dy)
			{
				// TODO
			}
	[TODO]
	public void Translate(float dx, float dy)
			{
				// TODO
			}

	// Form the union of this region and another.
	[TODO]
	public void Union(GraphicsPath path)
			{
				// TODO
			}
	[TODO]
	public void Union(Rectangle rect)
			{
				// TODO
			}
	[TODO]
	public void Union(RectangleF rect)
			{
				// TODO
			}
	[TODO]
	public void Union(Region region)
			{
				// TODO
			}

	// Form the XOR of this region and another.
	[TODO]
	public void Xor(GraphicsPath path)
			{
				// TODO
			}
	[TODO]
	public void Xor(Rectangle rect)
			{
				// TODO
			}
	[TODO]
	public void Xor(RectangleF rect)
			{
				// TODO
			}
	[TODO]
	public void Xor(Region region)
			{
				// TODO
			}

}; // class Region

}; // namespace System.Drawing
