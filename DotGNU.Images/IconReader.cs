/*
 * IconReader.cs - Implementation of the "DotGNU.Images.IconReader" class.
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
using System.IO;

internal sealed class IconReader
{
	// Load a Windows icon image from the specified stream.  The first
	// 4 bytes have already been read and discarded.  If "hotspots" is
	// "true", then the image is actually a Windows cursor with hotspots.
	public static void Load(Stream stream, Image image, bool hotspots)
			{
				byte[] buffer = new byte [1024];
				int offset = 4;
				int numImages, index;
				int[] offsetList;
				int width, height, colors;
				PixelFormat format;
				Frame frame;
				int[] palette;
				int paletteCount;
				int paletteIndex;

				// Read the number of images in the file.
				if(stream.Read(buffer, 0, 2) != 2)
				{
					throw new FormatException();
				}
				numImages = Utils.ReadUInt16(buffer, 0);
				offset += 2;

				// Read the resource directory.
				offsetList = new int [numImages];
				for(index = 0; index < numImages; ++index)
				{
					if(stream.Read(buffer, 0, 16) != 16)
					{
						throw new FormatException();
					}
					offset += 16;
					width = buffer[0];
					height = buffer[1];
					colors = buffer[2];
					if(colors == 0)
					{
						colors = 256;
						format = PixelFormat.Format8bppIndexed;
					}
					else if(colors == 2)
					{
						format = PixelFormat.Format1bppIndexed;
					}
					else if(colors == 16)
					{
						format = PixelFormat.Format4bppIndexed;
					}
					else
					{
						throw new FormatException();
					}
					frame = image.AddFrame(width, height, format);
					if(hotspots)
					{
						frame.HotspotX = Utils.ReadUInt16(buffer, 4);
						frame.HotspotY = Utils.ReadUInt16(buffer, 6);
					}
					offsetList[index] = Utils.ReadInt32(buffer, 12);
				}

				// Read the contents of the images in the stream.
				for(index = 0; index < numImages; ++index)
				{
					// Seek to the start of the image.
					Utils.Seek(stream, offset, offsetList[index]);
					offset = offsetList[index];

					// Get the frame that this image corresponds to.
					frame = image.GetFrame(index);

					// Skip the DIB header, which we don't need.
					// Monochrome cursors don't have a DIB header.
					if(!hotspots ||
					   frame.PixelFormat != PixelFormat.Format1bppIndexed)
					{
						if(stream.Read(buffer, 0, 40) != 40)
						{
							throw new FormatException();
						}
						offset += 40;
					}

					// Read the palette information if necessary.
					// Monochrome cursors don't have a palette.
					if(!hotspots ||
					   frame.PixelFormat != PixelFormat.Format1bppIndexed)
					{
						paletteCount =
							(1 << Utils.FormatToBitCount(frame.PixelFormat));
						if(stream.Read(buffer, 0, paletteCount * 4)
								!= paletteCount * 4)
						{
							throw new FormatException();
						}
						offset += paletteCount * 4;
						palette = new int [paletteCount];
						for(paletteIndex = 0; paletteIndex < paletteCount;
							++paletteIndex)
						{
							palette[paletteIndex] = Utils.ReadBGR
								(buffer, paletteIndex * 4);
						}
					}
					else
					{
						palette = new int[] {0, 0x00FFFFFF};
					}
					frame.Palette = palette;

					// Read the main part of the icon or cursor.
					BmpReader.LoadBitmapData(stream, frame, false, true);
					offset += frame.Height * frame.Stride;

					// Read the mask.
					BmpReader.LoadBitmapData(stream, frame, true, true);
					offset += frame.Height * frame.MaskStride;

					// Invert the mask, because we want 1 to mean "active".
					InvertMask(frame);
					image.LoadFormat = Image.Icon;
				}
			}

	// Invert the mask in a frame.
	private static void InvertMask(Frame frame)
			{
				byte[] mask = frame.Mask;
				int posn;
				for(posn = 0; posn < mask.Length; ++posn)
				{
					mask[posn] = (byte)(mask[posn] ^ 0xFF);
				}
			}

}; // class IconReader

}; // namespace DotGNU.Images
