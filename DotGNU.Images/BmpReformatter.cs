/*
 * BmpConvert.cs - Implementation of the "DotGNU.Images.Convert" class.
 *
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

	public class BmpReformatter
	{
		public static Frame Reformat(Frame oldFrame, PixelFormat format)
				{
					if (oldFrame.PixelFormat == format)
						return oldFrame.CloneFrame(null);
					Frame newFrame = oldFrame.CloneFrameEmpty(oldFrame.Width, oldFrame.Height, format);
					newFrame.data = new byte[newFrame.Height * newFrame.Stride];
					if (oldFrame.Mask != null)
						newFrame.mask = (byte[])oldFrame.Mask.Clone();
								
					// 24 bpp Format.
					if (oldFrame.PixelFormat == PixelFormat.Format24bppRgb)
					{
						switch(format)
						{
							case(PixelFormat.Format16bppRgb555):
								return Reformat24bppTo16bpp(oldFrame, newFrame,  true);
							case(PixelFormat.Format16bppRgb565):
								return Reformat24bppTo16bpp(oldFrame, newFrame, false);
							case(PixelFormat.Format8bppIndexed):
								Octree octree = new Octree (Utils.FormatToBitCount(format));
								octree.Process(oldFrame, newFrame);
								return newFrame;
							default:
								throw new NotSupportedException();
						}
					}
					// 16 bpp Format.
					if (oldFrame.PixelFormat == PixelFormat.Format16bppRgb565 || oldFrame.PixelFormat == PixelFormat.Format16bppRgb555)
					{
						switch (format)
						{
							case(PixelFormat.Format24bppRgb):
								return Reformat16bppTo24bpp(oldFrame, newFrame, oldFrame.PixelFormat == PixelFormat.Format16bppRgb555);
							case(PixelFormat.Format16bppRgb555):
								return Reformat16bpp(oldFrame, newFrame, true);
							case(PixelFormat.Format16bppRgb565):
								return Reformat16bpp(oldFrame, newFrame, false);
							default:
								throw new NotSupportedException();
						}
					}
					// 8 bpp Format.
					else if (oldFrame.PixelFormat == PixelFormat.Format8bppIndexed)
					{
						switch (format)
						{
							case(PixelFormat.Format24bppRgb):
								return Reformat8bppTo24bpp(oldFrame, newFrame);
							case(PixelFormat.Format16bppRgb555):
								return Reformat8bppTo16bpp(oldFrame, newFrame, true);
							case(PixelFormat.Format16bppRgb565):
								return Reformat8bppTo16bpp(oldFrame, newFrame, false);
							default:
								throw new NotSupportedException();
						}
					}
					// 4 bpp Format.
					else if (oldFrame.PixelFormat == PixelFormat.Format4bppIndexed)
					{
						switch (format)
						{
							case(PixelFormat.Format8bppIndexed):
								return Reformat4bppTo8bpp(oldFrame, newFrame);
							case(PixelFormat.Format24bppRgb):
								return Reformat4bppTo24bpp(oldFrame, newFrame);
							case(PixelFormat.Format16bppRgb555):
								return Reformat4bppTo16bpp(oldFrame, newFrame, true);
							case(PixelFormat.Format16bppRgb565):
								return Reformat4bppTo16bpp(oldFrame, newFrame, false);
							default:
								throw new NotSupportedException();
						}
					}
					// 1bpp Format.
					else if (oldFrame.PixelFormat == PixelFormat.Format1bppIndexed)
					{
						switch (format)
						{
							case(PixelFormat.Format4bppIndexed):
								return Reformat1bppTo4bpp(oldFrame, newFrame);
							case(PixelFormat.Format8bppIndexed):
								return Reformat1bppTo8bpp(oldFrame, newFrame);
							case(PixelFormat.Format24bppRgb):
								return Reformat1bppTo24bpp(oldFrame, newFrame);
							case(PixelFormat.Format16bppRgb555):
								return Reformat1bppTo16bpp(oldFrame, newFrame, true);
							case(PixelFormat.Format16bppRgb565):
								return Reformat1bppTo16bpp(oldFrame, newFrame, false);
							default:
								throw new NotSupportedException();
						}
					}
					throw new NotSupportedException();
				}

		// Not supported by .NET
		private static Frame Reformat4bppTo8bpp(Frame oldFrame, Frame newFrame)
				{
					int ptrOld = 0;
					byte[] oldData = oldFrame.Data;
					// Copy palette.
					newFrame.Palette = (int[])oldFrame.Palette.Clone();
					int ptrNew = 0;
					byte[] newData = newFrame.Data;
					for(int y = 0; y < oldFrame.Height; y++)
					{
						ptrOld = y * oldFrame.Stride;
						ptrNew = y * newFrame.Stride;
						bool firstNibble = true;
						byte byteData = 0;
						int newByteEnd = ptrNew + newFrame.Width;
						for(; ptrNew < newByteEnd; ptrNew++)
						{
							int palettePos;
							if (firstNibble)
							{
								byteData = oldData[ptrOld];
								palettePos = (byteData & 0xF0) >> 4;
							}
							else
							{
								palettePos = byteData & 0x0F;
								ptrOld++;
							}
							firstNibble = !firstNibble;
							newData[ptrNew] = (byte)palettePos;
						}
					}
					return newFrame;
				}
		private static Frame Reformat4bppTo24bpp(Frame oldFrame, Frame newFrame)
				{
					int ptrOld = 0;
					byte[] oldData = oldFrame.Data;
					int[] palette = oldFrame.Palette;
					int ptrNew = 0;
					byte[] newData = newFrame.Data;
					for(int y = 0; y < oldFrame.Height; y++)
					{
						ptrOld = y * oldFrame.Stride;
						ptrNew = y * newFrame.Stride;
						bool firstNibble = true;
						byte byteData = 0;
						int newByteEnd = ptrNew + newFrame.Width * 3;
						while(ptrNew < newByteEnd)
						{
							int color;
							if (firstNibble)
							{
								byteData = oldData[ptrOld];
								color = palette[(byteData & 0xF0) >> 4];
							}
							else
							{
								color = palette[byteData & 0x0F];
								ptrOld++;
							}
							firstNibble = !firstNibble;
							newData[ptrNew++] = (byte)color;
							newData[ptrNew++] = (byte)(color>>8);
							newData[ptrNew++] = (byte)(color>>16);
						}
					}
					return newFrame;
				}
		
		private static Frame Reformat4bppTo16bpp(Frame oldFrame, Frame newFrame, bool format555)
				{
					int ptrOld = 0;
					byte[] oldData = oldFrame.Data;
					int[] palette = oldFrame.Palette;
					int ptrNew = 0;
					byte[] newData = newFrame.Data;
					for(int y = 0; y < oldFrame.Height; y++)
					{
						ptrOld = y * oldFrame.Stride;
						ptrNew = y * newFrame.Stride;
						bool firstNibble = true;
						byte byteData = 0;
						int newByteEnd = ptrNew + newFrame.Width * 2;
						while(ptrNew < newByteEnd)
						{
							int color;
							if (firstNibble)
							{
								byteData = oldData[ptrOld];
								color = palette[(byteData & 0xF0) >> 4];
							}
							else
							{
								color = palette[byteData & 0x0F];
								ptrOld++;
							}
							firstNibble = !firstNibble;
							if (format555)
							{
								newData[ptrNew++] = (byte)((color>>6 & 0xE0) | (color>>3 & 0x1F));
								newData[ptrNew++] = (byte)((color>>17 & 0x7C) | (color >>14 & 0x03));
							}
							else
							{
								newData[ptrNew++] = (byte)((color>>5 & 0xE0) | (color >> 3 & 0x1F)) ;
								newData[ptrNew++] = (byte)((color>>16 & 0xF8) | (color >>13 & 0x07));
							}
						}
					}
					return newFrame;
				}
		
		private static Frame Reformat1bppTo24bpp(Frame oldFrame, Frame newFrame)
				{
					int ptrOld = 0;
					byte[] oldData = oldFrame.Data;
					int colorBlack = oldFrame.Palette[0];
					int colorWhite = oldFrame.Palette[1];
					byte colorBlackR = (byte)(colorBlack>>16);
					byte colorBlackG = (byte)(colorBlack>>8);
					byte colorBlackB = (byte)(colorBlack);
					byte colorWhiteR = (byte)(colorWhite>>16);
					byte colorWhiteG = (byte)(colorWhite>>8);
					byte colorWhiteB = (byte)(colorWhite);
					int ptrNew = 0;
					byte[] newData = newFrame.Data;
					for(int y = 0; y < oldFrame.Height; y++)
					{
						ptrOld = y * oldFrame.Stride;
						ptrNew = y * newFrame.Stride;
						int bit = 128;
						byte byteData = 0;
						int newByteEnd = ptrNew + newFrame.Width * 3;
						if (ptrNew < newByteEnd)
							byteData = oldData[ptrOld];
						while(ptrNew < newByteEnd)
						{
							if ((byteData & bit) != 0)
							{
								newData[ptrNew++] = colorWhiteB;
								newData[ptrNew++] = colorWhiteG;
								newData[ptrNew++] = colorWhiteR;
							}
							else
							{
								newData[ptrNew++] = colorBlackB;
								newData[ptrNew++] = colorBlackG;
								newData[ptrNew++] = colorBlackR;
							}
							bit = bit>>1;
							if (bit == 0)
							{
								bit = 128;
								ptrOld++;
								byteData = oldData[ptrOld];
							}
						}
					}
					return newFrame;
				}
		
		// Not supported by .NET
		private static Frame Reformat1bppTo4bpp(Frame oldFrame, Frame newFrame)
				{
					int ptrOld = 0;
					byte[] oldData = oldFrame.Data;
					// Copy palette.
					newFrame.Palette = (int[])oldFrame.Palette.Clone();
					int ptrNew = 0;
					byte[] newData = newFrame.Data;
					for(int y = 0; y < oldFrame.Height; y++)
					{
						ptrOld = y * oldFrame.Stride;
						ptrNew = y * newFrame.Stride;
						int bit = 0;
						bool firstNibble = true;
						byte byteNewData = 0;
						byte byteOldData = 0;
						for(int x = 0; x < oldFrame.Width; x++)
						{
							if (bit == 0)
							{
								bit = 128;
								byteOldData = oldData[ptrOld++];
							}
							if (firstNibble)
							{
								if ((byteOldData & bit) != 0)
									byteNewData =  16;
								else
									byteNewData = 0;
							}
							else
							{
								if ((byteOldData & bit) != 0)
									byteNewData |= 0x01;
								newData[ptrNew++] = byteNewData;
							}
							bit = bit>>1;
							firstNibble = !firstNibble;
							
						}
					}
					return newFrame;
				}
		// Not supported by .NET
		private static Frame Reformat1bppTo8bpp(Frame oldFrame, Frame newFrame)
				{
					int ptrOld = 0;
					byte[] oldData = oldFrame.Data;
					// Copy palette.
					newFrame.Palette = (int[])oldFrame.Palette.Clone();
					int ptrNew = 0;
					byte[] newData = newFrame.Data;
					for(int y = 0; y < oldFrame.Height; y++)
					{
						ptrOld = y * oldFrame.Stride;
						ptrNew = y * newFrame.Stride;
						int bit = 0;
						byte byteData = 0;
						int newByteEnd = ptrNew + newFrame.Width;
						for(; ptrNew < newByteEnd; ptrNew++)
						{
							if (bit == 0)
							{
								bit = 128;
								byteData = oldData[ptrOld++];
							}
							if ((byteData & bit) != 0)
								newData[ptrNew] = 1;
							else
								newData[ptrNew] = 0;
							bit = bit>>1;
						}
					}
					return newFrame;
				}
		private static Frame Reformat1bppTo16bpp(Frame oldFrame, Frame newFrame, bool format555)
				{
					int ptrOld = 0;
					byte[] oldData = oldFrame.Data;
					int colorBlack = oldFrame.Palette[0];
					int colorWhite = oldFrame.Palette[1];
					byte colorBlack1, colorBlack2, colorWhite1, colorWhite2;
					if (format555)
					{
						colorBlack1 = (byte)((colorBlack>>6 & 0xE0) | (colorBlack>>3 & 0x1F));
						colorBlack2 = (byte)((colorBlack>>17 & 0x7C) | (colorBlack >>14 & 0x03));
						colorWhite1 = (byte)((colorWhite>>6 & 0xE0) | (colorWhite>>3 & 0x1F));
						colorWhite2 = (byte)((colorWhite>>17 & 0x7C) | (colorWhite >>14 & 0x03));
					}
					else
					{
						colorBlack1 = (byte)((colorBlack>>5 & 0xE0) | (colorBlack >> 3 & 0x1F)) ;
						colorBlack2 = (byte)((colorBlack>>16 & 0xF8) | (colorBlack >>13 & 0x07));
						colorWhite1 = (byte)((colorWhite>>5 & 0xE0) | (colorWhite >> 3 & 0x1F)) ;
						colorWhite2 = (byte)((colorWhite>>16 & 0xF8) | (colorWhite >>13 & 0x07));
					}
					
					int[] palette = oldFrame.Palette;
					int ptrNew = 0;
					byte[] newData = newFrame.Data;
					for(int y = 0; y < oldFrame.Height; y++)
					{
						ptrOld = y * oldFrame.Stride;
						ptrNew = y * newFrame.Stride;
						int bit = 0;
						byte byteData = 0;
						int newByteEnd = ptrNew + newFrame.Width * 2;
						while(ptrNew < newByteEnd)
						{
							if (bit == 0)
							{
								bit = 128;
								byteData = oldData[ptrOld++];
							}
							if ((byteData & bit) != 0)
							{
								newData[ptrNew++] =  colorWhite1;
								newData[ptrNew++] =  colorWhite2;
							}
							else
							{
								newData[ptrNew++] = colorBlack1;
								newData[ptrNew++] = colorBlack2;
							}
							bit = bit>>1;
						}
					}
					return newFrame;
				}
		
		private static Frame Reformat8bppTo24bpp(Frame oldFrame, Frame newFrame)
				{
					int ptrOld = 0;
					byte[] oldData = oldFrame.Data;
					int[] palette = oldFrame.Palette;
					int ptrNew = 0;
					byte[] newData = newFrame.Data;
					for(int y = 0; y < oldFrame.Height; y++)
					{
						ptrOld = y * oldFrame.Stride;
						ptrNew = y * newFrame.Stride;
						int newByteEnd = ptrNew + newFrame.Width * 3;
						while(ptrNew < newByteEnd)
						{
							int color = palette[oldData[ptrOld++]];
							newData[ptrNew++] = (byte)color;
							newData[ptrNew++] = (byte)(color>>8);
							newData[ptrNew++] = (byte)(color>>16);
						}
					}
					return newFrame;
				}
		private static Frame Reformat8bppTo16bpp(Frame oldFrame, Frame newFrame, bool format555)
				{
					int ptrOld = 0;
					byte[] oldData = oldFrame.Data;
					int[] palette = oldFrame.Palette;
					int ptrNew = 0;
					byte[] newData = newFrame.Data;
					for(int y = 0; y < oldFrame.Height; y++)
					{
						ptrOld = y * oldFrame.Stride;
						ptrNew = y * newFrame.Stride;
						int newByteEnd = ptrNew + newFrame.Width * 2;
						while(ptrNew < newByteEnd)
						{
							int color = palette[oldData[ptrOld++]];
							if (format555)
							{
								newData[ptrNew++] = (byte)((color>>6 & 0xE0) | (color>>3 & 0x1F));
								newData[ptrNew++] = (byte)((color>>17 & 0x7C) | (color >>14 & 0x03));
							}
							else
							{
								newData[ptrNew++] = (byte)((color>>5 & 0xE0) | (color >> 3 & 0x1F)) ;
								newData[ptrNew++] = (byte)((color>>16 & 0xF8) | (color >>13 & 0x07));
							}
						}
					}
					return newFrame;
				}

		private static Frame Reformat16bppTo24bpp(Frame oldFrame, Frame newFrame, bool format555)
				{
					int ptrOld = 0;
					byte[] oldData = oldFrame.Data;
					int[] palette = oldFrame.Palette;
					int ptrNew = 0;
					byte[] newData = newFrame.Data;
					for(int y = 0; y < oldFrame.Height; y++)
					{
						ptrOld = y * oldFrame.Stride;
						ptrNew = y * newFrame.Stride;
						int newByteEnd = ptrNew + newFrame.Width * 3;
						while(ptrNew < newByteEnd)
						{
							byte g = oldData[ptrOld++];
							byte r = oldData[ptrOld++];
							byte b = (byte)(g & 0x1F);
							if (format555)
							{
								g =(byte)( r << 3 & 0x18  | g >> 5 & 0x07);
								g = (byte)((int)g * 255 / 31);
								r = (byte)( r >> 2 & 0x1F);
							}
							else
							{
								g = (byte)(r << 3 & 0x38  | g >> 5 & 0x07);
								g = (byte)((int)g * 255 / 63);
								r =  (byte)(r >> 3);
							}
							r = (byte)((int)r * 255 / 31);
							b = (byte)((int)b * 255 / 31);
							newData[ptrNew++] = b;
							newData[ptrNew++] = g;
							newData[ptrNew++] = r;
						}
					}
					return newFrame;
				}
		private static Frame Reformat16bpp(Frame oldFrame, Frame newFrame, bool formatTo555)
				{
					int ptrOld = 0;
					byte[] oldData = oldFrame.Data;
					int[] palette = oldFrame.Palette;
					int ptrNew = 0;
					byte[] newData = newFrame.Data;
					for(int y = 0; y < oldFrame.Height; y++)
					{
						ptrOld = y * oldFrame.Stride;
						ptrNew = y * newFrame.Stride;
						int newByteEnd = ptrNew + newFrame.Width * 2;
						while(ptrNew < newByteEnd)
						{
							byte b1 = oldData[ptrOld++];
							byte b2 = oldData[ptrOld++];
							if (formatTo555)
							{
								b1 = (byte)((b2 & 0x01) << 7 | (b1 & 0xC0) >> 1 | (b1 & 0x1F));
								b2 = (byte)((b2 & 0xF8) >> 1 | (b2 & 0x07)>>1);
							}
							else
							{
								b2 = (byte)((b2 & 0x7C) << 1 | (b2 & 0x03)<<1 | (b1 & 0x80)>> 7);
								b1 = (byte)((b1 & 0xD0) << 1 | (b1 & 0x1F));
							}
							newData[ptrNew++] = b1;
							newData[ptrNew++] = b2;
						}
					}
					return newFrame;
				}
		private static Frame Reformat24bppTo16bpp(Frame oldFrame, Frame newFrame, bool format555)
				{
					int ptrOld = 0;
					byte[] oldData = oldFrame.Data;
					
					int[] palette = oldFrame.Palette;
					int ptrNew = 0;
					byte[] newData = newFrame.Data;
					for(int y = 0; y < oldFrame.Height; y++)
					{
						ptrOld = y * oldFrame.Stride;
						ptrNew = y * newFrame.Stride;
						int newByteEnd = ptrNew + newFrame.Width * 2;
						while(ptrNew < newByteEnd)
						{
							int color = oldData[ptrOld++] | oldData[ptrOld++] <<8 | oldData[ptrOld++] << 16;
							if (format555)
							{
								newData[ptrNew++] =  (byte)((color>>6 & 0xE0) | (color>>3 & 0x1F));
								newData[ptrNew++] =  (byte)((color>>17 & 0x7C) | (color >>14 & 0x03));
							}
							else
							{
								newData[ptrNew++] = (byte)((color>>5 & 0xE0) | (color >> 3 & 0x1F)) ;
								newData[ptrNew++] = (byte)((color>>16 & 0xF8) | (color >>13 & 0x07));
							}
						}
					}
					return newFrame;
				}
			
		private class Octree
		{
			// The root of the octree.
			private	OctreeNode root;
			private int leafCount = 0;
			private OctreeNode[] reducibleNodes;
			private int maxColorBits;
			// Store the last node quantized.
			private OctreeNode previousNode = null;
			// Cache the previous color quantized.
			private int previousColor = 0;
			// Mask used when getting the appropriate pixels for a given node.
			private static int[] mask = new int[8] { 0x80 , 0x40 , 0x20 , 0x10 , 0x08 , 0x04 , 0x02 , 0x01 };
			public Octree (int maxColorBits)
					{
						this.maxColorBits = maxColorBits;
						reducibleNodes = new OctreeNode[9];
						root = new OctreeNode (0 , maxColorBits , this); 
					}

			// Reduce the depth of the tree.
			private void Reduce ()
					{
						int i;
						// Find the deepest level containing at least one reducible node.
						for (i = maxColorBits - 1; (i > 0) && (reducibleNodes[i] == null); i--);

						// Reduce the node most recently added to the list at level 'index'.
						OctreeNode node = reducibleNodes[i];
						reducibleNodes[i] = node.nextReducible;
						leafCount -= node.Reduce();

						// Just in case we have reduced the last color to be added, and the next color to
						// be added is the same, invalidate the previousNode.
						previousNode = null;
					}

			public void Process(Frame sourceFrame, Frame destFrame)
					{
						byte[] destData = destFrame.Data;
						byte[] sourceData = sourceFrame.Data;
						int color;

						int pSourceRow = 0;
						int pSourcePixel;
						
						// Loop through each row.
						for (int row = 0 ; row <  sourceFrame.Height ; row++)
						{
							// Set the source pixel to the first pixel in this row
							pSourcePixel = pSourceRow ;
							for (int col = 0 ; col < sourceFrame.Width ; col++)
							{
								// Add the color to the octree
								int b = sourceData[pSourcePixel++];
								int g = sourceData[pSourcePixel++];
								int r = sourceData[pSourcePixel++];
								color = b | g << 8 | r << 16;
								// Add a given color value to the octree
								// Check if this request is for the same color as the last
								if (previousColor == color)
								{
									// Just update the previous node
									previousNode.Increment (r, g, b);
								}
								else
								{
									previousColor = color;
									root.AddColor (maxColorBits , 0 , this, r, g, b);
								}
							}
							pSourceRow += sourceFrame.Stride;
						}

						destFrame.Palette = CreatePalette(1<<maxColorBits);

						// Do the second pass.
						pSourceRow = 0;
						pSourcePixel = pSourceRow;
						int pDestinationRow =0;
						int pDestinationPixel = pDestinationRow ;

						// And convert the first pixel, so that I have values going into the loop
						int prevColor = -1;
						byte prevPixelValue = 0;
						for (int row = 0 ; row <  sourceFrame.Height ; row++)
						{
							pSourcePixel = pSourceRow ;
							pDestinationPixel = pDestinationRow ;

							// Loop through each pixel on this scan line
							for (int col = 0 ; col < sourceFrame.Width ; col++ , pDestinationPixel++)
							{
								color = sourceData[pSourcePixel++] | sourceData[pSourcePixel++] << 8 | sourceData[pSourcePixel++] << 16;
								if (color == prevColor)
									destData[pDestinationPixel] = prevPixelValue;
								else
								{
									prevColor = color;
									prevPixelValue = (byte)root.GetPaletteIndex (color , 0);
									destData[pDestinationPixel] = prevPixelValue;
								}
							}
							pSourceRow += sourceFrame.Stride;
							pDestinationRow += destFrame.Stride;
						}		
					}

			// Convert the nodes in the octree to a palette with a maximum of colorCount colors
			public int[] CreatePalette (int colorCount)
					{
						while (leafCount > colorCount)
							Reduce();
						int[] palette = new int[leafCount];
						int paletteIndex = 0;
						root.CreatePalette (palette , ref paletteIndex);
						return palette;
					}

			// Class which encapsulates each node in the tree
			private class OctreeNode
					{
						// Flag indicating that this is a leaf node
						private	bool leaf;
						private	int pixelCount = 0;
						private	int red = 0;
						private	int green = 0;
						private int blue= 0;
						private OctreeNode[] children;
						// Next reducible node
						internal OctreeNode nextReducible;
						// The index of this node in the palette
						private	int paletteIndex;

						// Construct the node
						public OctreeNode (int level , int colorBits , Octree octree)
						{
							// Construct the new node
							leaf = (level == colorBits);
							// If a leaf, increment the leaf count
							if (leaf)
								octree.leafCount++;
							else
							{
								// Otherwise add this to the reducible nodes
								nextReducible = octree.reducibleNodes[level];
								octree.reducibleNodes[level] = this;
								children = new OctreeNode[8];
							}
						}

						// Add a color into the tree
						public void AddColor (int colorBits , int level , Octree octree, int r, int g, int b)
						{
							// Update the color information if this is a leaf
							if (leaf)
							{
								Increment(r, g, b);
								// Setup the previous node
								octree.previousNode = this;
							}
							else
							{
								// Go to the next level down in the tree
								int index = ((r & mask[level]) >> (5 - level)) |
									((g & mask[level]) >> (6 - level)) |
									((b & mask[level]) >> (7 - level));

								OctreeNode	child = children[index];
								if (child == null)
								{
									// Create a new child node & store in the array
									child = new OctreeNode(level + 1 , colorBits , octree); 
									children[index] = child;
								}
								// Add the color to the child node
								child.AddColor (colorBits , level + 1 , octree, r, g, b);
							}
						}

						// Reduce this node by removing all of its children
						public int Reduce ()
						{
							red = green = blue = 0;
							int childrenCount = 0;

							// Loop through all children and add their information to this node
							for (int index = 0; index < 8; index++)
							{
								if (children[index] != null)
								{
									red += children[index].red;
									green += children[index].green;
									blue += children[index].blue;
									pixelCount += children[index].pixelCount;
									childrenCount++;
									children[index] = null;
								}
							}
							// Now change this to a leaf node
							leaf = true;
							// Return the number of nodes to decrement the leaf count by
							return (childrenCount - 1);
						}

						// Traverse the tree, building up the color palette
						public void CreatePalette (int[] palette , ref int paletteIndex)
						{
							if (leaf)
							{
								this.paletteIndex = paletteIndex++;
								// Set the color of the palette entry
								palette[this.paletteIndex] = (red / pixelCount) << 16 | (green / pixelCount) << 8 |(blue / pixelCount);
							}
							else
							{
								// Loop through children looking for leaves
								for (int index = 0; index < 8; index++)
								{
									if (null != children[index])
										children[index].CreatePalette (palette , ref paletteIndex);
								}
							}
						}

						// Return the palette index for the passed color
						public int GetPaletteIndex (int pixel , int level)
						{
							if (leaf)
								return paletteIndex;
							else
							{
								int red = pixel >>16;
								int green = (byte)(pixel >> 8);
								int blue = (byte)pixel;
								int index = ((red & mask[level]) >> (5 - level)) |
									((green & mask[level]) >> (6 - level)) |
									((blue & mask[level]) >> (7 - level));
								if (children[index] != null)
									return children[index].GetPaletteIndex (pixel , level + 1);
								else
									throw new Exception ();
							}
						}

						// Increment the pixel count and add to the color information
						public void Increment (int r, int g, int b)
						{
							pixelCount++;
							red += r;
							green += g;
							blue += b;
						}
					}
				}

	}
}
