/*
 * ColorTranslator.cs - Implementation of the
 *			"System.Drawing.Printing.ColorTranslator" class.
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

public sealed class ColorTranslator
{
	// Convert OLE system color indicators into KnownColor values.
	private static readonly KnownColor[] oleKnownColors = {
				KnownColor.ScrollBar,
				KnownColor.Desktop,
				KnownColor.ActiveCaption,
				KnownColor.InactiveCaption,
				KnownColor.Menu,
				KnownColor.Window,
				KnownColor.WindowFrame,
				KnownColor.MenuText,
				KnownColor.WindowText,
				KnownColor.ActiveCaptionText,
				KnownColor.ActiveBorder,
				KnownColor.InactiveBorder,
				KnownColor.AppWorkspace,
				KnownColor.Highlight,
				KnownColor.HighlightText,
				KnownColor.Control,
				KnownColor.ControlDark,
				KnownColor.GrayText,
				KnownColor.ControlText,
				KnownColor.InactiveCaptionText,
				KnownColor.ControlLightLight,
				KnownColor.ControlDarkDark,
				KnownColor.ControlLight,
				KnownColor.InfoText,
				KnownColor.Info,
			};

	// Convert KnownColor values into OLE system color indicators.
	private static readonly uint[] oleSystemColors = {
				0x8000000a, 0x80000002, 0x80000009, 0x8000000c,
				0x8000000f, 0x80000010, 0x80000015, 0x80000016,
				0x80000014, 0x80000012, 0x80000001, 0x80000011,
				0x8000000d, 0x8000000e, 0x8000000d, 0x8000000b,
				0x80000003, 0x80000013, 0x80000018, 0x80000017,
				0x80000004, 0x80000007, 0x80000000, 0x80000005,
				0x80000006, 0x80000008
			};

	// This class cannot be instantiated.
	private ColorTranslator() {}

	// Transform a HTML color name into a "Color" value.
	[TODO]
	public static Color FromHtml(String htmlColor)
			{
				// TODO
				return Color.Empty;
			}

	// Transform an OLE color value into a "Color" value.
	public static Color FromOle(int oleColor)
			{
				return FromWin32(oleColor);
			}

	// Transform a Win32 color value into a "Color" value.
	// This also understands system color indicators, for compatibility.
	public static Color FromWin32(int win32Color)
			{
				if((win32Color & (int)0xFF000000) != (int)0x80000000)
				{
					return Color.FromArgb(win32Color & 0xFF,
									      (win32Color >> 8) & 0xFF,
									      (win32Color >> 16) & 0xFF);
				}
				else
				{
					int sysColor = ((win32Color >> 24) & 0xFF);
					if(sysColor >= 0 && sysColor < oleKnownColors.Length)
					{
						return new Color(oleKnownColors[sysColor]);
					}
					else
					{
						return Color.Empty;
					}
				}
			}

	// Convert a "Color" value into a HTML color name.
	[TODO]
	public static String ToHtml(Color color)
			{
				// TODO
				return null;
			}

	// Convert a "Color" value into an OLE color value.
	public static int ToOle(Color color)
			{
				if(color.IsKnownColor)
				{
					int value = (int)(color.ToKnownColor());
					if(value >= 1 && value <= oleSystemColors.Length)
					{
						return (int)(oleSystemColors[value - 1]);
					}
				}
				return ToWin32(color);
			}

	// Convert a "Color" value into a Win32 color value.
	// This does not support system color indicators, for compatibility.
	public static int ToWin32(Color color)
			{
				return (color.R | (color.G << 8) | (color.B << 16));
			}

}; // class ColorTranslator

}; // namespace System.Drawing
