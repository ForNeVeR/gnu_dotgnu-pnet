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
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Drawing.Toolkit;

internal class DrawingFont : IToolkitFont
{
	// Internal state.
	private System.Drawing.Font properties;
	private IToolkit toolkit;
	
	public DrawingFont(IToolkit toolkit, System.Drawing.Font properties)
			{
				this.toolkit = toolkit;
				this.properties = properties;
			}

	// Select this font into a graphics object.
	public void Select(IToolkitGraphics _graphics)
			{
				DrawingGraphics graphics = (_graphics as DrawingGraphics);
				if(graphics != null)
				{
					lock(this)
					{
						/*if(xfont == null)
						{
							xfont = new Xsharp.Font
								(MapFamilyName(properties.Name),
								 (int)(properties.SizeInPoints * 10.0f),
								 (Xsharp.FontStyle)(properties.Style));
						}
						graphics.font = xfont;*/
					}
				}
			}

	// Dispose of this font.
	public void Dispose()
			{
				
			}

	// Get the raw HFONT for this toolkit font.  IntPtr.Zero if none.
	public IntPtr GetHfont()
			{
				// Nothing to do here in this implementation.
				return IntPtr.Zero;
			}

	// Get the LOGFONT information for this toolkit font.
	public void ToLogFont(Object lf, IToolkitGraphics graphics)
			{
				// Nothing to do here in this implementation.
			}

	// Map a Windows-style family name to an Xsharp-style family name.
	private static String MapFamilyName(String name)
			{
				if(String.Compare(name, "Times", true) == 0 ||
				   String.Compare(name, "Times New Roman", true) == 0)
				{
					return null;//Xsharp.Font.Serif;
				}
				else if(String.Compare(name, "Helvetica", true) == 0 ||
				        String.Compare(name, "Helv", true) == 0 ||
				        String.Compare
							(name, "Microsoft Sans Serif", true) == 0 ||
				        String.Compare(name, "Arial", true) == 0 ||
				        String.Compare(name, 0, "Arial ", 0, 6, true) == 0)
				{
					return null; //Xsharp.Font.SansSerif;
				}
				else if(String.Compare(name, "Courier", true) == 0 ||
				        String.Compare(name, "Courier New", true) == 0)
				{
					return null; //Xsharp.Font.Fixed;
				}
				else
				{
					return null; //Xsharp.Font.Serif;
				}
			}

}; // class DrawingFont

}; // namespace System.Drawing.Toolkit
