/*
 * Frame.cs - Implementation of the "DotGNU.Images.Frame" class.
 *
 * Copyright (C) 2003  Southern Storm Software, Pty Ltd.
 * Copyright (C) 2003  Neil Cawse.
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
	private int hotspotX;
	private int hotspotY;
	internal byte[] data;
	internal byte[] mask;

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
				this.hotspotX = 0;
				this.hotspotY = 0;
				this.data = new byte [height * stride];
				this.mask = null;
			}
	private Frame(Image newImage, Frame frame, int newWidth, int newHeight, PixelFormat format, bool cloneData)
			{
				// Clone from the other frame.
				image = newImage;
				width = newWidth;
				height = newHeight;
				pixelFormat = format;
				stride =Utils.FormatToStride(pixelFormat, width);
				maskStride = (((width + 7) / 8) + 3) & ~3;
				if(frame.palette != null)
				{
					if(newImage != null && frame.palette == frame.image.Palette)
					{
						// The palette is a copy of the image's.
						palette = newImage.Palette;
					}
					else if (cloneData)
					{
						// The palette is specific to this frame.
						palette = (int[])(frame.palette.Clone());
					}
				}
				transparentPixel = frame.transparentPixel;
				hotspotX = frame.hotspotX;
				hotspotY = frame.hotspotY;
				if(cloneData & frame.data != null)
				{
					data = (byte[])(frame.data.Clone());
				}
				if(cloneData & frame.mask != null)
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
	public int HotspotX
			{
				get
				{
					return hotspotX;
				}
				set
				{
					hotspotX = value;
				}
			}
	public int HotspotY
			{
				get
				{
					return hotspotY;
				}
				set
				{
					hotspotY = value;
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
				return new Frame(newImage, this, width, height, pixelFormat, true);
			}
	
	// Clone a frame without the data.
	internal Frame CloneFrameEmpty(int newWidth, int newHeight, PixelFormat format)
			{
				return new Frame(this.image, this, newWidth, newHeight, format, false);
			}

	internal void NewImage(Image newImage)
			{
				image = newImage;
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
				int ptr = y * stride;
				switch (pixelFormat)
				{
					case (PixelFormat.Format24bppRgb):
						ptr += 3 * x;
						return data[ptr++] | data[ptr++] << 8 | data[ptr++] << 16;
					case (PixelFormat.Format16bppRgb565):
					{
						ptr += 2 * x;
						int g = data[ptr++];
						int r = data[ptr++];
						int b = (g & 0x1F) * 255 / 31;
						g = (r << 3 & 0x38  | g >> 5 & 0x07) * 255 / 63;
						r =  (r >> 3) * 255 / 31;
						return r | g << 8 | b << 16;
					}
					case (PixelFormat.Format16bppRgb555):
					{
						ptr += 2 * x;
						int g = data[ptr++];
						int r = data[ptr++];
						int b = (g & 0x1F) * 255 / 31;
						g =( r << 3 & 0x18  | g >> 5 & 0x07) * 255 / 31;
						r = ( r >> 2 & 0x1F) * 255 / 31;
						return r | g << 8 | b << 16;
					}
					case (PixelFormat.Format8bppIndexed):
						return palette[data[ptr]];
					case (PixelFormat.Format4bppIndexed):
						ptr += x/2;
						if ((x & 0x01) == 0)
							return palette[data[ptr]>>4];
						else
							return palette[data[ptr] & 0x0F];
					case (PixelFormat.Format1bppIndexed):
						ptr += x/8;
						if ((data[ptr] &(1<<7 - (x & 0x03)))== 0)
							return palette[0];
						else
							return palette[1];
					default:
						throw new NotSupportedException();
				}
			}

	// Get the mask value at a specific location.
	public int GetMask(int x, int y)
			{
				int ptr = y * stride + x/8;
				if ((data[ptr] &(1<<7 - (x & 0x03)))== 0)
					return 0;
				else
					return 1;
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
				return BmpResizer.Resize(this, 0, 0, Width, Height, newWidth, newHeight);
			}

	// Set the pixel value at a specific location.
	public void SetPixel(int x, int y, int color)
			{
				int ptr = y * stride;
				switch (pixelFormat)
				{
					case (PixelFormat.Format24bppRgb):
						ptr += 3 * x;
						data[ptr++] = (byte)color;
						data[ptr++] = (byte)(color >> 8);
						data[ptr] = (byte)(color >> 16);
						break;
					case (PixelFormat.Format16bppRgb565):
						ptr += 2 * x;
						data[ptr++] = (byte)((color>>5 & 0xE0) | (color >> 3 & 0x1F)) ;
						data[ptr] = (byte)((color>>16 & 0xF8) | (color >>13 & 0x07));
						break;
					case (PixelFormat.Format16bppRgb555):
						ptr += 2 * x;
						data[ptr++] =  (byte)((color>>6 & 0xE0) | (color>>3 & 0x1F));
						data[ptr] =  (byte)((color>>17 & 0x7C) | (color >>14 & 0x03));
						break;
					case (PixelFormat.Format8bppIndexed):
						ptr += x;
						data[ptr] = (byte)Utils.BestPaletteColor(palette, color);
						break;
					case (PixelFormat.Format4bppIndexed):
						ptr += x/2;
						if ((x & 0x01) == 0)
							data[ptr] = (byte)(Utils.BestPaletteColor(palette, color)<< 4 | data[ptr] & 0x0F);
						else
							data[ptr] =(byte)(Utils.BestPaletteColor(palette, color) & 0x0F | data[ptr] & 0xF0);
						break;
					case (PixelFormat.Format1bppIndexed):
						ptr += x/8;
						if (Utils.BestPaletteColor(palette, color) == 0)
							data[ptr] &= (byte)(~(1<<7 - (x & 0x03)));
						else
							data[ptr] |= (byte)(1<<7 - (x & 0x03));
						break;
				}
			}

	// Set the mask value at a specific location.
	public void SetMask(int x, int y, int value)
			{
				int ptr = y * stride + x/8;
				if (value == 0)
					data[ptr] &= (byte)(~(1<<7 - (x & 0x03)));
				else
					data[ptr] |= (byte)(1<<7 - (x & 0x03));
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

	// Create a new image from the rect (x, y, width, height) of this image.
	// The new image is in a parallelogram defined by 3 points - Top-Left, Top-Right
	// and Bottom-Left. The remaining point is inferred. The top left corner of the
	// bounding rectangle must be (0,0).
	public Frame AdjustImage(int originx1, int originy1, int originx2, int originy2,
		int originx3, int originy3, int destx1, int desty1, int destx2, int desty2,
		int destx3, int desty3)
			{
				// Return if there are no changes.
				if (originx1 == destx1 && originy1 == desty1 && originx2 == destx2 &&
					originy2 == originy2 && originx3 == destx3 && originy3 == desty3)
					return this.CloneFrame(null);
				// TODO
				if (originx1 != originx3 || originy1 != originy2 || desty2 != 0 || destx3 != 0)
					throw new NotSupportedException ("No shearing or rotation yet");
				if (destx1 != 0 || desty1 != 0 || desty2 != 0 || destx3 != 0)
					throw new NotSupportedException ("No shearing or rotation yet");

				// Calculate the inferred points.
				int orginx4 = originx3 - originx1 + originx2;
				int orginy4 = originy2 - originy1 + originy3;
				int destx4 = destx3 - destx1 + destx2;
				int desty4 = desty2 - desty1 + desty3;

				// Calculate the outside bounds.
				int boundsx = destx1;
				int boundsy = desty1;
				int boundsright = destx1;
				int boundsbottom = desty1;

				if (boundsright < destx2)
					boundsright = destx2;
				else
					boundsx = destx2;
				if (boundsbottom < desty2)
					boundsbottom = desty2;
				else
					boundsy = desty2;
				if (boundsright < destx3)
					boundsright = destx3;
				if (boundsx > destx3)
					boundsx = destx3;
				if (boundsbottom < desty3)
					boundsbottom = desty3;
				if (boundsy > desty3)
					boundsy = desty3;
				if (boundsright < destx4)
					boundsright = destx4;
				if (boundsx > destx4)
					boundsx = destx4;
				if (boundsbottom < desty4)
					boundsbottom = desty4;
				if (boundsy > desty4)
					boundsy = desty4;

				if (boundsx != 0 || boundsy != 0)
					throw new ArgumentException();

				Frame f = BmpResizer.Resize(this, originx1, originy1 , originx2 - originx1, originy3 - originy1, destx2 - destx1, desty3 - desty1);
				// TODO: The shearing and rotating.
				//f = new Frame(newImage, boundsright - boundsx, boundsbottom - boundsy, pixelFormat);
				return f;
			}

	// Change the PixelFormat of a particular frame.
	public Frame Reformat(PixelFormat format)
			{
				return BmpReformatter.Reformat(this, format);
			}

	public void Copy(Frame frame, int x, int y)
			{
				// Developers should be aware of any costly conversions.
				// So we don't automatically match the depths.
				if (pixelFormat != frame.pixelFormat)
					throw new ArgumentException();

				//TODO:
				// If we are copying an index bitmap, we need to find the color
				// in the destination palette that is closest to the color we are
				// copying to. We would also need to add colors to the palette,
				// if there is space and optionally optimize the palette.
				// For now we just overwrite the old palette.

				//TODO:
				// The mask is not taken into account when copying. We need to
				// look at adding alpha support.

				if (frame.palette != null)
					palette = (int[])frame.palette.Clone();
				
				int bits = Utils.FormatToBitCount(pixelFormat);
				byte[] destData = Data;
				byte[] sourceData = frame.Data;
				int right = x + frame.width;
				if (width < right)
					right = width;
				int bottom = y + frame.height;
				if (height < bottom)
					bottom = height;

				int pSourceRow = 0;
				int pDestinationRow = y * stride;
				for (; y < bottom ; y++)
				{
					int pSourcePixel = pSourceRow;
					int pDestinationPixel = pDestinationRow + x * bits / 8;
					// Loop through each pixel on this scan line
					int xEnd = pDestinationRow + right*bits/8;
					// TODO: Could be done with Marshal.Copy if speed is an issue.
					while (pDestinationPixel < xEnd)
						destData[pDestinationPixel++] = sourceData[pSourcePixel++];
					// Are there any bits left over?
					if (bits < 8)
					{
						int bitsOver = right * (bits & 0x03);
						if (bitsOver > 0)
						{
							int mask = (1 << (8 - bitsOver)) - 1;
							byte lastByte = (byte)(destData[pDestinationPixel] & mask | sourceData[pSourcePixel++] & ~mask);
							destData[pDestinationPixel++] = lastByte;
						}
					}
					
					pSourceRow += frame.stride;
					pDestinationRow += stride;
				}
			}

}; // class Frame

}; // namespace DotGNU.Images
