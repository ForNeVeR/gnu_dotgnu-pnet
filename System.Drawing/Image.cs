/*
 * Image.cs - Implementation of the "System.Drawing.Image" class.
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

[Serializable]
[ComVisible(true)]
public abstract class Image
	: MarshalByRefObject, ISerializable, ICloneable, IDisposable
{
	// Internal state.
	internal int flags;
	internal Guid[] frameDimensionsList;
	internal int height;
	internal float horizontalResolution;
	internal ColorPalette palette;
	internal PixelFormat pixelFormat;
	internal int[] propertyIdList;
	internal PropertyItem[] propertyItems;
	internal ImageFormat rawFormat;
	internal float verticalResolution;
	internal int width;

	// Constructors.
	[TODO]
	internal Image()
			{
				// TODO
			}
	[TODO]
	internal Image(SerializationInfo info, StreamingContext context)
			{
				// TODO
			}

	// Destructor.
	~Image()
			{
				Dispose(false);
			}

	// Get this object's properties.
	public int Flags
			{
				get
				{
					return flags;
				}
			}
	public Guid[] FrameDimensionsList
			{
				get
				{
					return frameDimensionsList;
				}
			}
	public int Height
			{
				get
				{
					return height;
				}
			}
	public float HorizontalResolution
			{
				get
				{
					return horizontalResolution;
				}
			}
	public ColorPalette Palette
			{
				get
				{
					return palette;
				}
				set
				{
					palette = value;
				}
			}
	public SizeF PhysicalDimension
			{
				get
				{
					return new SizeF(width, height);
				}
			}
	public PixelFormat PixelFormat
			{
				get
				{
					return pixelFormat;
				}
			}
	public int[] PropertyIdList
			{
				get
				{
					if(propertyIdList == null)
					{
						propertyIdList = new int [0];
					}
					return propertyIdList;
				}
			}
	public PropertyItem[] PropertyItems
			{
				get
				{
					if(propertyItems == null)
					{
						propertyItems = new PropertyItem [0];
					}
					return propertyItems;
				}
			}
	public ImageFormat RawFormat
			{
				get
				{
					return rawFormat;
				}
			}
	public Size Size
			{
				get
				{
					return new Size(width, height);
				}
			}
	public float VerticalResolution
			{
				get
				{
					return verticalResolution;
				}
			}
	public int Width
			{
				get
				{
					return width;
				}
			}

	// Make a copy of this object.
	public virtual Object Clone()
			{
				// TODO
				return null;
			}

	// Dispose of this object.
	public void Dispose()
			{
				Dispose(true);
				GC.SuppressFinalize(this);
			}
	protected virtual void Dispose(bool disposing)
			{
				// TODO
			}

	// Load an image from a file.
	public static Image FromFile(String filename)
			{
				return FromFile(filename, false);
			}
	[TODO]
	public static Image FromFile
				(String filename, bool useEmbeddedColorManagement)
			{
				// TODO
				return null;
			}

	// Convert a HBITMAP object into an image.
	public static Image FromHbitmap(IntPtr hbitmap)
			{
				return FromHbitmap(hbitmap, IntPtr.Zero);
			}
	[TODO]
	public static Image FromHbitmap(IntPtr hbitmap, IntPtr hpalette)
			{
				// TODO
				return null;
			}

	// Load an image from a particular stream.
	public static Image FromStream(Stream stream)
			{
				return FromStream(stream, false);
			}
	[TODO]
	public static Image FromStream
				(Stream stream, bool useEmbeddedColorManagement)
			{
				// TODO
				return null;
			}

	// Get a bounding rectangle for this image.
	[TODO]
	public RectangleF GetBounds(ref GraphicsUnit pageUnit)
			{
				// TODO
				return RectangleF.Empty;
			}

	// Get parameter information for a specific encoder.
	[TODO]
	public EncoderParameters GetEncoderParameterList(Guid encoder)
			{
				// TODO
				return null;
			}

	// Get the number of frames in a specific dimension.
	[TODO]
	public int GetFrameCount(FrameDimension dimension)
			{
				// TODO
				return 1;
			}

	// Get the number of bits per pixel in a specific format.
	public static int GetPixelFormatSize(PixelFormat pixfmt)
			{
				switch(pixfmt)
				{
					case PixelFormat.Format1bppIndexed:		return 1;
					case PixelFormat.Format4bppIndexed:		return 4;
					case PixelFormat.Format8bppIndexed:		return 8;
					case PixelFormat.Format16bppRgb555:
					case PixelFormat.Format16bppRgb565:
					case PixelFormat.Format16bppArgb1555:
					case PixelFormat.Format16bppGrayScale:	return 16;
					case PixelFormat.Format24bppRgb:		return 24;
					case PixelFormat.Format32bppRgb:
					case PixelFormat.Format32bppPArgb:
					case PixelFormat.Format32bppArgb:		return 32;
					case PixelFormat.Format48bppRgb:		return 48;
					case PixelFormat.Format64bppPArgb:
					case PixelFormat.Format64bppArgb:		return 64;
				}
				return 0;
			}

	// Get a specific property item
	[TODO]
	public PropertyItem GetPropertyItem(int propid)
			{
				// TODO
				return null;
			}

	// Get a thumbnail version of this image.
	[TODO]
	public Image GetThumbnailImage(int thumbWidth, int thumbHeight,
		     					   GetThumbnailImageAbort callback,
			    				   IntPtr callbackData)
			{
				// TODO
				return null;
			}

	// Delegate for aborting "GetThumbnailImage" prematurely.
	public delegate bool GetThumbnailImageAbort();

	// Check for specific kinds of pixel formats.
	public static bool IsAlphaPixelFormat(PixelFormat pixfmt)
			{
				return ((pixfmt & PixelFormat.Alpha) != 0);
			}
	public static bool IsCanonicalPixelFormat(PixelFormat pixfmt)
			{
				return ((pixfmt & PixelFormat.Canonical) != 0);
			}
	public static bool IsExtendedPixelFormat(PixelFormat pixfmt)
			{
				return ((pixfmt & PixelFormat.Extended) != 0);
			}

	// Remove a specific property.
	[TODO]
	public void RemoveProperty(int propid)
			{
				// TODO
			}

	// Rotate and/or flip this image.
	[TODO]
	public void RotateFlip(RotateFlipType rotateFlipType)
			{
				// TODO
			}

	// Save this image to a file.
	public void Save(String filename)
			{
				Save(filename, RawFormat);
			}
	[TODO]
	public void Save(String filename, ImageFormat format)
			{
				// TODO
			}
	[TODO]
	public void Save(String filename, ImageCodecInfo encoder,
					 EncoderParameters encoderParameters)
			{
				// TODO
			}

	// Save this image to a stream.
	[TODO]
	public void Save(Stream stream, ImageFormat format)
			{
				// TODO
			}
	[TODO]
	public void Save(Stream stream, ImageCodecInfo encoder,
					 EncoderParameters encoderParameters)
			{
				// TODO
			}

	// Add a frame to the previously saved image file.
	[TODO]
	public void SaveAdd(EncoderParameters encoderParamers)
			{
				// TODO
			}
	[TODO]
	public void SaveAdd(Image image, EncoderParameters encoderParamers)
			{
				// TODO
			}

	// Select a new frame and make it the active one.
	[TODO]
	public int SelectActiveFrame(FrameDimension dimension, int frameIndex)
			{
				// TODO
				return frameIndex;
			}

	// Set a property on this image.
	[TODO]
	public void SetPropertyItem(PropertyItem propitem)
			{
				// TODO
			}

	// Implement the ISerializable interface.
	[TODO]
	void ISerializable.GetObjectData(SerializationInfo info,
									 StreamingContext context)
			{
				// TODO
			}

}; // class Image

}; // namespace System.Drawing
