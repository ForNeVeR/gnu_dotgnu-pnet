/*
 * DrawingFont.cs - Implementation of fonts for System.Drawing.Win32.
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
	using System.Runtime.InteropServices;
	using DotGNU.Images;
	public class DrawingImage : ToolkitImageBase
	{
		internal Frame imageFrame;
		internal /*Win32.Api.BITMAPINFO*/ byte[] bitMapInfo;
		internal IntPtr hMaskRegion = IntPtr.Zero;

		public DrawingImage(Image image, int frame) : base(image, frame)
		{
			ImageChanged();
		}

		public override void ImageChanged()
		{
			Win32.Api.DeleteObject(hMaskRegion);
			imageFrame = image.GetFrame(frame);

			// Set the size of the structure
			int size = 40;
			if (imageFrame.PixelFormat == PixelFormat.Format16bppRgb565)
				size += 3 * 4;
			else if (imageFrame.PixelFormat == PixelFormat.Format8bppIndexed)
				size += 256 * 4;
			else if (imageFrame.PixelFormat == PixelFormat.Format4bppIndexed)
				size += 40 + 16 * 4;
			else if (imageFrame.PixelFormat == PixelFormat.Format1bppIndexed)
				size += 40 + 2 * 4;
			bitMapInfo = new byte[size];
			WriteInt32(bitMapInfo, 0, 40); //biSize
			WriteInt32(bitMapInfo, 4, imageFrame.Width); //biWidth
			WriteInt32(bitMapInfo, 8, -imageFrame.Height); //biHeight
			WriteInt32(bitMapInfo, 12, 1); //biPlanes
			WriteInt32(bitMapInfo, 14, FormatToBitCount(imageFrame.PixelFormat)); //biBitCount
			if (imageFrame.PixelFormat == PixelFormat.Format16bppRgb565)
			{
				WriteInt32(bitMapInfo, 16, (int)Win32.Api.BitMapInfoCompressionType.BI_BITFIELDS);
				// Setup the masks for 565
				WriteInt32(bitMapInfo, 40, 0xF800); // R Mask
				WriteInt32(bitMapInfo, 44, 0x07E0); // G Mask
				WriteInt32(bitMapInfo, 48, 0x001F); // B Mask
			}
			else
				WriteInt32(bitMapInfo, 16, (int)Win32.Api.BitMapInfoCompressionType.BI_RGB); // biCompression

			WriteInt32(bitMapInfo, 20, 0); // biSizeImage
			WriteInt32(bitMapInfo, 24, 0); // biXPelsPerMeter
			WriteInt32(bitMapInfo, 28, 0); // biYPelsPerMeter
			WriteInt32(bitMapInfo, 32, 0); // biClrUsed
			WriteInt32(bitMapInfo, 36, 0); // biClrImportant
			//Setup palette
			if (imageFrame.Palette != null)
			{
				// Write in RGBQUADS
				for (int i = 0; i < imageFrame.Palette.Length; i++)
					WriteBGR(bitMapInfo, 40 + i * 4, imageFrame.Palette[i]);
			}

			if (imageFrame.Mask == null)
				hMaskRegion = IntPtr.Zero;
			else
				hMaskRegion = MaskToRegion(imageFrame);
		}


		// Convert the mask bits in the frame into a Win32 region.
		// Optimize this by adding the maximum number of rectangles at once and
		// by combining strips on the x axis into one rectangle.

		private IntPtr MaskToRegion (Frame frame)
		{
			int stride = frame.MaskStride;
			int height = frame.Height;
			int width = frame.Width;
			byte[] mask = frame.Mask;

			// This is the maximum rectangles we should add at once because
			// Win98 only allows 4000. So we break this into steps
			int initialCount = 3900;
			// No point in being more than the number of pixels
			if (imageFrame.Width * imageFrame.Height < initialCount)
				initialCount = imageFrame.Width * imageFrame.Height;
			
			uint count = 0;
			// Set the initial quantity of the rectangles.
			// Header is 32 bytes, 16 bytes per rect.
			Byte[] RGNData = new byte[32 + 16 * initialCount];
			// Setup RGNData header size
			RGNData[0] = 32;
			// Set type to RDH_RECTANGLES
			RGNData[4] = 1;
			// Start of array of "RECT"
			uint ptr = 32;

			bool writeRect = false;
			bool writeAlways = false;

			// Scan top to bottom
			for (int y = 0; y < height; y++)
			{
				int xBlockStart = 0;
				bool prevMaskBit = false;
				byte currentByte = 0;

				for(int x = 0; x < width; x++)
				{
					int bitPos = 7 - x % 8;
					if (bitPos == 7)
						currentByte = mask[y * stride + (int)(x / 8)];

					if ((currentByte & 1<<bitPos)==0)
					{
						if (!prevMaskBit)
						{
							xBlockStart = x;
							prevMaskBit = true;
						}
							
					}
					else
					{
						if (prevMaskBit)
							// The current bit requires writing
							writeRect = true;
					}

					// Are we at the end of the line?
					if (x == width - 1)
					{
						if (y == height - 1)
						{
							writeRect = true;
							// End of data so we must always write
							writeAlways = true;
						}
						else if (prevMaskBit)
						{
							// The current bit requires writing
							writeRect = true;
						}
						x++;
					}
					
					if (writeRect)
					{
						count++;
						WriteRect(RGNData, ref ptr, new Rectangle(xBlockStart, y, x - xBlockStart, 1));
								
						if (count == initialCount || writeAlways)
						{
							// Set the rectangle count in RGNHeader
							RGNData[8] = (byte)count;
							RGNData[9] = (byte)(count >> 8);
							RGNData[10] = (byte)(count >> 16);
							RGNData[11] = (byte)(count >> 24);
							
							// Create the new region.
							IntPtr hTemp = Win32.Api.ExtCreateRegion(IntPtr.Zero, 32 + 16 * count, ref RGNData[0]);
							if (hMaskRegion == IntPtr.Zero)
								hMaskRegion = hTemp;
							else
							{
								// OR the new region with the current one.
								Win32.Api.CombineRgn(hMaskRegion, hMaskRegion, hTemp, Win32.Api.RegionCombineMode.RGN_OR);
								Win32.Api.DeleteObject(hTemp);
							}
							count = 0;
							ptr = 32;
							writeAlways = false;
						}
						writeRect = false;
						prevMaskBit = false;
					}
				}
			}
			
			return hMaskRegion;
		}

		// Write a RECT structure at ptr in the array of bytes
		private void WriteRect( byte[] RGNData, ref uint ptr, Rectangle r)
		{
			RGNData[ptr] = (byte)r.X;
			RGNData[ptr+1] = (byte)(r.X >> 8);
			RGNData[ptr+2] = (byte)(r.X >> 16);
			RGNData[ptr+3] = (byte)(r.X >> 24);
			RGNData[ptr+4] = (byte)r.Y;
			RGNData[ptr+5] = (byte)(r.Y >> 8);
			RGNData[ptr+6] = (byte)(r.Y >> 16);
			RGNData[ptr+7] = (byte)(r.Y >> 24);
			RGNData[ptr+8] = (byte)r.Right;
			RGNData[ptr+9] = (byte)(r.Right >> 8);
			RGNData[ptr+10] = (byte)(r.Right >> 16);
			RGNData[ptr+11] = (byte)(r.Right >> 24);
			RGNData[ptr+12] = (byte)r.Bottom;
			RGNData[ptr+13] = (byte)(r.Bottom >> 8);
			RGNData[ptr+14] = (byte)(r.Bottom >> 16);
			RGNData[ptr+15] = (byte)(r.Bottom >> 24);
			ptr += 16;
		}

		// Convert a pixel format into a bit count value.
		private static short FormatToBitCount(PixelFormat pixelFormat)
		{
			switch(pixelFormat)
			{
				case PixelFormat.Format1bppIndexed:
					return 1;

				case PixelFormat.Format4bppIndexed:
					return 4;

				case PixelFormat.Format8bppIndexed:
					return 8;

				case PixelFormat.Format16bppRgb555:
				case PixelFormat.Format16bppRgb565:
				case PixelFormat.Format16bppArgb1555:
				case PixelFormat.Format16bppGrayScale:
					return 16;

				case PixelFormat.Format24bppRgb:
					return 24;

				case PixelFormat.Format32bppRgb:
				case PixelFormat.Format32bppPArgb:
				case PixelFormat.Format32bppArgb:
					return 32;

				case PixelFormat.Format48bppRgb:
					return 48;

				case PixelFormat.Format64bppPArgb:
				case PixelFormat.Format64bppArgb:
					return 64;

				default:
					return 32;
			}
		}

		// Write a BGR value to a buffer as an RGBQUAD.
		private static void WriteBGR(byte[] buffer, int offset, int value)
		{
			buffer[offset] = (byte)value;
			buffer[offset + 1] = (byte)(value >> 8);
			buffer[offset + 2] = (byte)(value >> 16);
			buffer[offset + 3] = (byte)0;
		}

			// Write a little-endian 16-bit integer value to a buffer.
		private static void WriteUInt16(byte[] buffer, int offset, int value)
		{
			buffer[offset] = (byte)value;
			buffer[offset + 1] = (byte)(value >> 8);
		}

		// Write a little-endian 32-bit integer value to a buffer.
		private static void WriteInt32(byte[] buffer, int offset, int value)
		{
			buffer[offset] = (byte)value;
			buffer[offset + 1] = (byte)(value >> 8);
			buffer[offset + 2] = (byte)(value >> 16);
			buffer[offset + 3] = (byte)(value >> 24);
		}


		public override void Dispose()
		{
			Win32.Api.DeleteObject(hMaskRegion);
		}

	}
}
