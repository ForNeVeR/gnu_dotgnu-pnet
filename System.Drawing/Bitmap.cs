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
using System.Security;
using System.Runtime.InteropServices;
using System.Runtime.Serialization;
using System.Drawing.Imaging;
using System.Drawing.Design;
using System.ComponentModel;
using DotGNU.Images;
using System.Drawing.Toolkit;

#if !ECMA_COMPAT
[Serializable]
[ComVisible(true)]
#endif
#if CONFIG_COMPONENT_MODEL_DESIGN
[Editor("System.Drawing.Design.BitmapEditor, System.Drawing.Design",
		typeof(UITypeEditor))]
#endif
public sealed class Bitmap : System.Drawing.Image
{
	// Constructors.
	public Bitmap(Image original)
			: this(original, original.Width, original.Height) {}
	public Bitmap(Image original, Size newSize)
			: this (original, newSize.Width, newSize.Height) {}
	public Bitmap(Stream stream) : this(stream, false) {}
	public Bitmap(Stream stream, bool useIcm)
			{
				DotGNU.Images.Image dgImage = new DotGNU.Images.Image();
				dgImage.Load(stream);
				SetDGImage(dgImage);
			}
	public Bitmap(String filename) : this(filename, false) {}
	public Bitmap(String filename, bool useIcm)
			{
				DotGNU.Images.Image dgImage = new DotGNU.Images.Image();
				dgImage.Load(filename);
				SetDGImage(dgImage);
			}
	public Bitmap(int width, int height)
			: this(width, height, Imaging.PixelFormat.Format32bppArgb) {}
	public Bitmap(int width, int height,
				  System.Drawing.Imaging.PixelFormat format)
			{
				SetDGImage(new DotGNU.Images.Image
					(width, height, (DotGNU.Images.PixelFormat)format));
				dgImage.AddFrame();
			}
	public Bitmap(int width, int height, Graphics g)
			{
				if(g == null)
				{
					throw new ArgumentNullException("g");
				}
				SetDGImage(new DotGNU.Images.Image
					(width, height, DotGNU.Images.PixelFormat.Format32bppArgb));
				dgImage.AddFrame();
			}
	public Bitmap(Type type, String resource)
			{
				Stream stream = type.Module.Assembly.GetManifestResourceStream
					(resource);
				if(stream == null)
				{
					throw new ArgumentException(S._("Arg_UnknownResource"));
				}
				try
				{
					DotGNU.Images.Image dgImage = new DotGNU.Images.Image();
					dgImage.Load(stream);
					SetDGImage(dgImage);
				}
				finally
				{
					stream.Close();
				}
			}
	public Bitmap(Image original, int width, int height)
			{
				if(original.dgImage != null)
				{
					SetDGImage(original.dgImage.Stretch(width, height));
				}
			}
	public Bitmap(int width, int height, int stride,
				  System.Drawing.Imaging.PixelFormat format, IntPtr scan0)
			{
				// We don't support loading bitmaps from unmanaged buffers.
				throw new SecurityException();
			}
	internal Bitmap(DotGNU.Images.Image image) : base(image) {}
#if CONFIG_SERIALIZATION
	internal Bitmap(SerializationInfo info, StreamingContext context)
			: base(info, context) {}
#endif

	// Clone this bitmap and transform it into a new pixel format
	[TODO]
	public Bitmap Clone
				(Rectangle rect, System.Drawing.Imaging.PixelFormat format)
			{
				// TODO
				return null;
			}
	[TODO]
	public Bitmap Clone
				(RectangleF rect, System.Drawing.Imaging.PixelFormat format)
			{
				// TODO
				return null;
			}

	// Create a bitmap from a native icon handle.
	public static Bitmap FromHicon(IntPtr hicon)
			{
				throw new SecurityException();
			}

	// Create a bitmap from a Windows resource name.
	public static Bitmap FromResource(IntPtr hinstance, String bitmapName)
			{
				throw new SecurityException();
			}

	// Convert this bitmap into a native bitmap handle.
#if CONFIG_COMPONENT_MODEL
	[EditorBrowsable(EditorBrowsableState.Advanced)]
#endif
	public IntPtr GetHbitmap()
			{
				return GetHbitmap(Color.LightGray);
			}
	[TODO]
#if CONFIG_COMPONENT_MODEL
	[EditorBrowsable(EditorBrowsableState.Advanced)]
#endif
	public IntPtr GetHbitmap(Color background)
			{
				throw new SecurityException();
			}

	// Convert this bitmap into a native icon handle.
	[TODO]
#if CONFIG_COMPONENT_MODEL
	[EditorBrowsable(EditorBrowsableState.Advanced)]
#endif
	public IntPtr GetHicon()
			{
				throw new SecurityException();
			}

	// Get the color of a specific pixel.
	public Color GetPixel(int x, int y)
			{
				if(dgImage != null)
				{
					int pix = dgImage.GetFrame(0).GetPixel(x, y);
					return Color.FromArgb((pix >> 16) & 0xFF,
										  (pix >> 8) & 0xFF,
										  pix & 0xFF);
				}
				return Color.Empty;
			}

	// Lock a region of this bitmap.
	public BitmapData LockBits(Rectangle rect, ImageLockMode flags,
							   System.Drawing.Imaging.PixelFormat format)
			{
				// Don't use this: it isn't portable.
				return new BitmapData();
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
	public void SetPixel(int x, int y, Color color)
			{
				if(dgImage != null)
				{
					dgImage.GetFrame(0).SetPixel
						(x, y, (color.R << 16) | (color.G << 8) | color.B);
				}
			}

	// Set the resolution for this bitmap.
	public void SetResolution(float dpiX, float dpiY)
			{
				horizontalResolution = dpiX;
				verticalResolution = dpiY;
			}

	// Unlock the bits within this bitmap.
	public void UnlockBits(BitmapData bitmapData)
			{
				// Nothing to do in this implementation.
			}

}; // class Bitmap

}; // namespace System.Drawing
