/*
 * Frame.cs - Implementation of the "DotGNU.Images.Frame" class.
 *
 * Copyright (C) 2003  Southern Storm Software, Pty Ltd.
 *
 * This program is free software, you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY, without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program, if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

namespace DotGNU.Images
{

using System;
using System.Runtime.InteropServices;

public class Frame : MarshalByRefObject, IDisposable
{
	// Internal state.
	private Image image;
	private int width;
	private int height;
	private int stride;
	private int maskStride;
	private PixelFormat pixelFormat;
	private int[] palette;
	private int transparentPixel;
	private byte[] data;
	private byte[] mask;

	// Constructor.
	internal Frame(Image image, int width, int height, PixelFormat pixelFormat)
			{
				this.image = image;
				this.width = width;
				this.height = height;
				this.stride = Utils.FormatToStride(pixelFormat, width);
				this.maskStride = (((width + 7) / 8) + 3) & ~3;
				this.pixelFormat = pixelFormat;
				this.palette = null;
				this.transparentPixel = -1;
				this.data = new byte [width * stride];
				this.mask = null;
			}
	private Frame(Image newImage, Frame frame)
			{
				// Clone from the other frame.
				image = newImage;
				width = frame.width;
				height = frame.height;
				stride = frame.stride;
				maskStride = frame.maskStride;
				pixelFormat = frame.PixelFormat;
				if(frame.palette != null)
				{
					if(frame.palette == frame.image.Palette)
					{
						// The palette is a copy of the image's.
						palette = newImage.Palette;
					}
					else
					{
						// The palette is specific to this frame.
						palette = (int[])(frame.palette.Clone());
					}
				}
				transparentPixel = frame.transparentPixel;
				if(frame.data != null)
				{
					data = (byte[])(frame.data.Clone());
				}
				if(frame.mask != null)
				{
					mask = (byte[])(frame.mask.Clone());
				}
			}

	// Destructor.
	~Frame()
			{
				Dispose(false);
			}

	// Get the frame's properties.
	public int Width
			{
				get
				{
					return width;
				}
			}
	public int Height
			{
				get
				{
					return height;
				}
			}
	public int Stride
			{
				get
				{
					return stride;
				}
			}
	public int MaskStride
			{
				get
				{
					return maskStride;
				}
			}
	public PixelFormat PixelFormat
			{
				get
				{
					return pixelFormat;
				}
			}
	public byte[] Data
			{
				get
				{
					return data;
				}
			}
	public byte[] Mask
			{
				get
				{
					return mask;
				}
			}
	public int[] Palette
			{
				get
				{
					// The palette for indexed images, null if an RGB image.
					return palette;
				}
				set
				{
					palette = value;
				}
			}
	public int TransparentPixel
			{
				get
				{
					// Index into "Palette" of the transparent pixel value.
					// Returns -1 if there is no transparent pixel specified.
					return transparentPixel;
				}
				set
				{
					transparentPixel = value;
				}
			}

	// Add a mask to this frame.
	public void AddMask()
			{
				if(mask == null)
				{
					mask = new byte [width * maskStride];
				}
			}

	// Clone this frame into a new image.
	internal Frame CloneFrame(Image newImage)
			{
				return new Frame(newImage, this);
			}

	// Dispose of this object.
	public void Dispose()
			{
				Dispose(true);
				GC.SuppressFinalize(this);
			}
	protected virtual void Dispose(bool disposing)
			{
				data = null;
				mask = null;
			}

	// Get the pixel value at a specific location.
	public int GetPixel(int x, int y)
			{
				// TODO
				return 0;
			}

	// Get the mask value at a specific location.
	public int GetMask(int x, int y)
			{
				// TODO
				return 0;
			}

	// Get the contents of a scan line (the buffer must be
	// at least "Stride" bytes in length).
	public void GetScanLine(int line, byte[] buffer)
			{
				if(line >= 0 && line < height && data != null)
				{
					Array.Copy(data, line * stride, buffer, 0, stride);
				}
			}
	public void GetScanLine(int line, IntPtr buffer)
			{
				if(line >= 0 && line < height && data != null &&
				   buffer != IntPtr.Zero)
				{
					Marshal.Copy(data, line * stride, buffer, stride);
				}
			}

	// Get the contents of a mask line (the buffer must be
	// at least "MaskStride" bytes in length).
	public void GetMaskLine(int line, byte[] buffer)
			{
				if(line >= 0 && line < height && mask != null)
				{
					Array.Copy(mask, line * maskStride, buffer, 0, maskStride);
				}
			}
	public void GetMaskLine(int line, IntPtr buffer)
			{
				if(line >= 0 && line < height && mask != null &&
				   buffer != IntPtr.Zero)
				{
					Marshal.Copy(mask, line * maskStride, buffer, maskStride);
				}
			}

	// Rotate or flip the contents of this frame.
	public void RotateFlip(RotateFlipType rotateFlipType)
			{
				// TODO
			}

	// Get a re-scaled version of this frame.
	public Frame Scale(int newWidth, int newHeight)
			{
				// TODO
				return null;
			}

	// Set the pixel value at a specific location.
	public void SetPixel(int x, int y, int value)
			{
				// TODO
			}

	// Set the mask value at a specific location.
	public void SetMask(int x, int y, int value)
			{
				// TODO
			}

	// Set the contents of a scan line (the buffer must be
	// at least "Stride" bytes in length).
	public void SetScanLine(int line, byte[] buffer)
			{
				if(line >= 0 && line < height && data != null)
				{
					Array.Copy(buffer, 0, data, line * stride, stride);
				}
			}
	public void SetScanLine(int line, IntPtr buffer)
			{
				if(line >= 0 && line < height && data != null &&
				   buffer != IntPtr.Zero)
				{
					Marshal.Copy(buffer, data, line * stride, stride);
				}
			}

	// Set the contents of a mask line (the buffer must be
	// at least "MaskStride" bytes in length).
	public void SetMaskLine(int line, byte[] buffer)
			{
				if(line >= 0 && line < height && mask != null)
				{
					Array.Copy(buffer, 0, mask, line * maskStride, maskStride);
				}
			}
	public void SetMaskLine(int line, IntPtr buffer)
			{
				if(line >= 0 && line < height && mask != null &&
				   buffer != IntPtr.Zero)
				{
					Marshal.Copy(buffer, mask, line * maskStride, maskStride);
				}
			}

}; // class Frame

}; // namespace DotGNU.Images
