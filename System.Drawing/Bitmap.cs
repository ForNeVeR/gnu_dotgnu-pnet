/*
 * Bitmap.cs - Implementation of the "System.Drawing.Bitmap" class.
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

using System.IO;
using System.Runtime.InteropServices;
using System.Runtime.Serialization;
using System.Drawing.Imaging;
using System.Drawing.Design;
using System.ComponentModel;

[Serializable]
[ComVisible(true)]
[Editor("System.Drawing.Design.BitmapEditor, System.Drawing.Design",
		typeof(UITypeEditor))]
public sealed class Bitmap : Image
{
	// Constructors.
	[TODO]
	public Bitmap(Image original)
			{
				// TODO
			}
	[TODO]
	public Bitmap(Image original, Size newSize)
			{
				// TODO
			}
	public Bitmap(Stream stream) : this(stream, false) {}
	[TODO]
	public Bitmap(Stream stream, bool useIcm)
			{
				// TODO
			}
	public Bitmap(String filename) : this(filename, false) {}
	[TODO]
	public Bitmap(String filename, bool useIcm)
			{
				// TODO
			}
	public Bitmap(int width, int height)
			: this(width, height, PixelFormat.Format32bppArgb) {}
	[TODO]
	public Bitmap(int width, int height, PixelFormat format)
			{
				// TODO
			}
	[TODO]
	public Bitmap(int width, int height, Graphics g)
			{
				// TODO
			}
	[TODO]
	public Bitmap(Type type, String resource)
			{
				// TODO
			}
	[TODO]
	public Bitmap(Image original, int width, int height)
			{
				// TODO
			}
	[TODO]
	public Bitmap(int width, int height, int stride,
				  PixelFormat format, IntPtr scan0)
			{
				// TODO
			}
	internal Bitmap(SerializationInfo info, StreamingContext context)
			: base(info, context) {}

	// Clone this bitmap and transform it into a new pixel format
	[TODO]
	public Bitmap Clone(Rectangle rect, PixelFormat format)
			{
				// TODO
				return null;
			}
	[TODO]
	public Bitmap Clone(RectangleF rect, PixelFormat format)
			{
				// TODO
				return null;
			}

	// Create a bitmap from a native icon handle.
	[TODO]
	public static Bitmap FromHicon(IntPtr hicon)
			{
				// TODO
				return null;
			}

	// Create a bitmap from a Windows resource name.
	[TODO]
	public static Bitmap FromResource(IntPtr hinstance, String bitmapName)
			{
				// TODO
				return null;
			}

	// Convert this bitmap into a native bitmap handle.
	[EditorBrowsable(EditorBrowsableState.Advanced)]
	public IntPtr GetHbitmap()
			{
				return GetHbitmap(Color.LightGray);
			}
	[TODO]
	[EditorBrowsable(EditorBrowsableState.Advanced)]
	public IntPtr GetHbitmap(Color background)
			{
				// TODO
				return IntPtr.Zero;
			}

	// Convert this bitmap into a native icon handle.
	[TODO]
	[EditorBrowsable(EditorBrowsableState.Advanced)]
	public IntPtr GetHicon()
			{
				// TODO
				return IntPtr.Zero;
			}

	// Get the color of a specific pixel.
	[TODO]
	public Color GetPixel(int x, int y)
			{
				// TODO
				return Color.Empty;
			}

	// Lock a region of this bitmap.
	[TODO]
	public BitmapData LockBits(Rectangle rect, ImageLockMode flags,
							   PixelFormat format)
			{
				// TODO
				return null;
			}

	// Make a particular color transparent within this bitmap.
	public void MakeTransparent()
			{
				MakeTransparent(Color.LightGray);
			}
	[TODO]
	public void MakeTransparent(Color transparentColor)
			{
				// TODO
			}

	// Set a pixel within this bitmap.
	[TODO]
	public void SetPixel(int x, int y, Color color)
			{
				// TODO
			}

	// Set the resolution for this bitmap.
	public void SetResolution(float dpiX, float dpiY)
			{
				horizontalResolution = dpiX;
				verticalResolution = dpiY;
			}

	// Unlock the bits within this bitmap.
	[TODO]
	public void UnlockBits(BitmapData bitmapData)
			{
				// TODO
			}

}; // class Bitmap

}; // namespace System.Drawing
