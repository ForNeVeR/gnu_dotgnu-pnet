/*
 * Font.cs - Font object for X#.
 *
 * Copyright (C) 2002, 2003  Southern Storm Software, Pty Ltd.
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

namespace Xsharp
{

using System;
using Xsharp.Types;

/// <summary>
/// <para>The <see cref="T:Xsharp.Font"/> class encapsulates
/// the operations on an X font.</para>
/// </summary>
public class Font
{
	// Internal class that keeps track of displays and fontsets.
	private class FontInfo
	{
		public FontInfo next;
		public FontExtents extents;
		public Display dpy;
		public IntPtr fontSet;

	} // class FontInfo

	// Internal state.
	private String family;
	private int pointSize;
	private FontStyle style;
	private String xname;
	private FontInfo infoList;

	/// <summary>
	/// <para>The family name for the default sans-serif font.</para>
	/// </summary>
	public static readonly String SansSerif = "helvetica";

	/// <summary>
	/// <para>The family name for the default serif font.</para>
	/// </summary>
	public static readonly String Serif = "times";

	/// <summary>
	/// <para>The family name for the default fixed-width font.</para>
	/// </summary>
	public static readonly String Fixed = "courier";

	/// <summary>
	/// <para>Constructs a new instance of <see cref="T:Xsharp.Font"/>.
	/// </para>
	/// </summary>
	///
	/// <param name="family">
	/// <para>The name of the font family, or <see langword="null"/> to
	/// use the default sans-serif font.</para>
	/// </param>
	///
	/// <param name="pointSize">
	/// <para>The point size (120 is typically "normal height").</para>
	/// </param>
	///
	/// <param name="style">
	/// <para>Additional styles to apply to the font.</para>
	/// </param>
	public Font(String family, int pointSize, FontStyle style)
			{
				if(family != null)
					this.family = family;
				else
					this.family = SansSerif;
				if(pointSize < 0 || pointSize > 1000)
					this.pointSize = 120;
				else
					this.pointSize = pointSize;
				this.style = style;
				this.xname = null;
				this.infoList = null;
			}

	/// <summary>
	/// <para>Constructs a new instance of <see cref="T:Xsharp.Font"/>
	/// with a default point size and style.</para>
	/// </summary>
	///
	/// <param name="family">
	/// <para>The name of the font family, or <see langword="null"/> to
	/// use the default sans-serif font.</para>
	/// </param>
	public Font(String family)
			: this(family, 120, FontStyle.Normal)
			{
				// Nothing to do here.
			}

	/// <summary>
	/// <para>Constructs a new instance of <see cref="T:Xsharp.Font"/>
	/// with a default style.</para>
	/// </summary>
	///
	/// <param name="family">
	/// <para>The name of the font family, or <see langword="null"/> to
	/// use the default sans-serif font.</para>
	/// </param>
	///
	/// <param name="pointSize">
	/// <para>The point size (120 is typically "normal height").</para>
	/// </param>
	public Font(String family, int pointSize)
			: this(family, pointSize, FontStyle.Normal)
			{
				// Nothing to do here.
			}

	// Internal constructor that is called from "CreateFromXLFD".
	private Font(String name, bool unused)
			{
				this.family = null;
				this.pointSize = 0;
				this.style = FontStyle.Normal;
				this.xname = name;
				this.infoList = null;
			}

	/// <summary>
	/// <para>Get the family name of this font.</para>
	/// </summary>
	///
	/// <value>
	/// <para>The family name, or <see langword="null"/> if this font
	/// was created from an XLFD name.</para>
	/// </value>
	public String Family
			{
				get
				{
					return family;
				}
			}

	/// <summary>
	/// <para>Get the point size of this font.</para>
	/// </summary>
	///
	/// <value>
	/// <para>The point size.</para>
	/// </value>
	public int PointSize
			{
				get
				{
					return pointSize;
				}
			}

	/// <summary>
	/// <para>Get the extra styles that are associated with this font.</para>
	/// </summary>
	///
	/// <value>
	/// <para>The extra styles.</para>
	/// </value>
	public FontStyle Style
			{
				get
				{
					return style;
				}
			}

	/// <summary>
	/// <para>Get the XLFD name of this font.</para>
	/// </summary>
	///
	/// <value>
	/// <para>The XLFD name, or <see langword="null"/> if this font
	/// was not created from an XLFD name.</para>
	/// </value>
	public String XLFD
			{
				get
				{
					return xname;
				}
			}

	/// <summary>
	/// <para>Determine if two font descriptions are equal.</para>
	/// </summary>
	///
	/// <param name="obj">
	/// <para>The font object to compare against.</para>
	/// </param>
	///
	/// <returns>
	/// <para>Returns <see langword="true"/> if the two fonts are equal;
	/// <see langword="false"/> otherwise.</para>
	/// </returns>
	public override bool Equals(Object obj)
			{
				Font other = (obj as Font);
				if(other != null)
				{
					if(this == other)
					{
						return true;
					}
					else
					{
						return (this.family == other.family &&
						        this.pointSize == other.pointSize &&
								this.style == other.style &&
								this.xname == other.xname);
					}
				}
				else
				{
					return false;
				}
			}

	/// <summary>
	/// <para>Get the hash code for a font.</para>
	/// </summary>
	///
	/// <returns>
	/// <para>Returns the hash code.</para>
	/// </returns>
	public override int GetHashCode()
			{
				return (((family == null) ? xname.GetHashCode()
										  : family.GetHashCode()) +
						pointSize + (int)style);
			}

	/// <summary>
	/// <para>Construct a font from an XLFD name.</para>
	/// </summary>
	///
	/// <param name="name">
	/// <para>The XLFD name of the font.</para>
	/// </param>
	///
	/// <returns>
	/// <para>The font object that corresponds to
	/// <paramref name="name"/>.</para>
	/// </returns>
	public static Font CreateFromXLFD(String name)
			{
				if(name == null)
				{
					return new Font(null);
				}
				else
				{
					return new Font(name, true);
				}
			}

	/// <summary>
	/// <para>Measure the width, ascent, and descent of a string,
	/// to calculate its extents when drawn on a graphics context
	/// using this font.</para>
	/// </summary>
	///
	/// <param name="graphics">
	/// <para>The graphics context to measure with.</para>
	/// </param>
	///
	/// <param name="str">
	/// <para>The string to be measured.</para>
	/// </param>
	///
	/// <param name="index">
	/// <para>The starting index in <paramref name="str"/> of the first
	/// character to be measured.</para>
	/// </param>
	///
	/// <param name="count">
	/// <para>The number of characters <paramref name="str"/>
	/// to be measured.</para>
	/// </param>
	///
	/// <param name="width">
	/// <para>The width of the string, in pixels.</para>
	/// </param>
	///
	/// <param name="ascent">
	/// <para>The ascent of the string, in pixels.</para>
	/// </param>
	///
	/// <param name="descent">
	/// <para>The descent of the string, in pixels.</para>
	/// </param>
	///
	/// <exception cref="T:System.ArgumentNullException">
	/// <para>Raised if <paramref name="graphics"/> is <see langword="null"/>.
	/// </para>
	/// </exception>
	public virtual void MeasureString
				(Graphics graphics, String str, int index, int count,
				 out int width, out int ascent, out int descent)
			{
				// Validate the parameters.
				if(graphics == null)
				{
					throw new ArgumentNullException("graphics");
				}
				if(str == null || count == 0)
				{
					width = 0;
					ascent = 0;
					descent = 0;
					return;
				}

				// Extract the substring to be measured.
				// TODO: make this more efficient by avoiding the data copy.
				str = str.Substring(index, count);

				// Get the font set to use to measure the string.
				IntPtr fontSet = GetFontSet(graphics.dpy);
				if(fontSet == IntPtr.Zero)
				{
					width = 0;
					ascent = 0;
					descent = 0;
					return;
				}

				// Get the text extents and decode them into useful values.
				XRectangle overall_ink;
				XRectangle overall_logical;
				try
				{
					IntPtr display = graphics.dpy.Lock();
					Xlib.XSharpTextExtents
						(display, fontSet, str,
						 out overall_ink, out overall_logical);
				}
				finally
				{
					graphics.dpy.Unlock();
				}
				width = overall_logical.width;
				ascent = -(overall_logical.y);
				descent = overall_logical.height + overall_logical.y;

				// Increase the descent to account for underlining.
				// We always draw the underline two pixels below
				// the font base line.
				if((style & FontStyle.Underlined) != 0)
				{
					if(descent < 3)
					{
						descent = 3;
					}
				}
			}

	/// <summary>
	/// <para>Draw a string at a particular position on a
	/// specified graphics context.</para>
	/// </summary>
	///
	/// <param name="graphics">
	/// <para>The graphics context to draw on.</para>
	/// </param>
	///
	/// <param name="x">
	/// <para>The X co-ordinate of the position to start drawing text.</para>
	/// </param>
	///
	/// <param name="y">
	/// <para>The Y co-ordinate of the position to start drawing text.</para>
	/// </param>
	///
	/// <param name="str">
	/// <para>The string to be drawn.</para>
	/// </param>
	///
	/// <param name="index">
	/// <para>The starting index in <paramref name="str"/> of the first
	/// character to be measured.</para>
	/// </param>
	///
	/// <param name="count">
	/// <para>The number of characters <paramref name="str"/>
	/// to be measured.</para>
	/// </param>
	///
	/// <exception cref="T:Xsharp.XException">
	/// <para>One of the co-ordinate values is out of range.</para>
	/// </exception>
	///
	/// <exception cref="T:System.ArgumentNullException">
	/// <para>Raised if <paramref name="graphics"/> is <see langword="null"/>.
	/// </para>
	/// </exception>
	public virtual void DrawString
				(Graphics graphics, int x, int y,
				 String str, int index, int count)
			{
				// Validate the parameters.
				if(x < -32768 || x > 32767 || y < -32768 || y > 32767)
				{
					throw new XException(S._("X_PointCoordRange"));
				}
				if(graphics == null)
				{
					throw new ArgumentNullException("graphics");
				}
				if(str == null || count == 0)
				{
					return;
				}

				// Extract the substring to be measured.
				// TODO: make this more efficient by avoiding the data copy.
				str = str.Substring(index, count);

				// Get the font set to use for the font.
				IntPtr fontSet = GetFontSet(graphics.dpy);
				if(fontSet == IntPtr.Zero)
				{
					return;
				}

				// Draw the string using the specified font set.
				try
				{
					IntPtr display = graphics.dpy.Lock();
					Xlib.XSharpDrawString
							(display, graphics.drawableHandle, graphics.gc,
							 fontSet, x, y, str, (int)style,
							 IntPtr.Zero, graphics.Foreground.value);
				}
				finally
				{
					graphics.dpy.Unlock();
				}
			}

	/// <summary>
	/// <para>Get extent information for this font, when drawing
	/// onto a particular graphics context.</para>
	/// </summary>
	///
	/// <param name="graphics">
	/// <para>The graphics context to get the extent information for.</para>
	/// </param>
	///
	/// <returns>
	/// <para>Returns the extent information.</para>
	/// </returns>
	///
	/// <exception cref="T:System.ArgumentNullException">
	/// <para>Raised if <paramref name="graphics"/> is <see langword="null"/>.
	/// </para>
	/// </exception>
	public virtual FontExtents GetFontExtents(Graphics graphics)
			{
				if(graphics == null)
				{
					throw new ArgumentNullException("graphics");
				}
				FontExtents extents = null;
				GetFontSet(graphics.dpy, out extents);
				return extents;
			}

	// Get the XFontSet structure for this font on a particular display.
	internal IntPtr GetFontSet(Display dpy)
			{
				FontExtents extents;
				return GetFontSet(dpy, out extents);
			}
	internal IntPtr GetFontSet(Display dpy, out FontExtents extents)
			{
				lock(typeof(Font))
				{
					// Map this object to the one that actually stores
					// the font set information.
					Font font = (Font)(dpy.fonts[this]);
					if(font == null)
					{
						font = this;
						dpy.fonts[this] = this;
					}

					// Search for existing font set information.
					FontInfo info = font.infoList;
					while(info != null)
					{
						if(info.dpy == dpy)
						{
							extents = info.extents;
							return info.fontSet;
						}
						info = info.next;
					}

					// Create a new font set.
					IntPtr fontSet;
					try
					{
						IntPtr display = dpy.Lock();
						fontSet = Xlib.XSharpCreateFont
							(display, family, pointSize, (int)style);
						if(fontSet == IntPtr.Zero)
						{
							extents = null;
							return IntPtr.Zero;
						}
					}
					finally
					{
						dpy.Unlock();
					}

					// Associate the font set with the display.
					info = new FontInfo();
					info.next = font.infoList;
					info.extents = new FontExtents(font, fontSet);
					info.dpy = dpy;
					info.fontSet = fontSet;
					font.infoList = info;

					// Return the font set to the caller.
					extents = info.extents;
					return fontSet;
				}
			}

	// Disassociate this font from a particular display.
	internal void Disassociate(Display dpy)
			{
				lock(typeof(Font))
				{
					FontInfo info, prev;
					info = infoList;
					prev = null;
					while(info != null && info.dpy != dpy)
					{
						prev = info;
						info = info.next;
					}
					if(info != null)
					{
						if(prev != null)
						{
							prev.next = info.next;
						}
						else
						{
							infoList = info.next;
						}
						Xlib.XSharpFreeFont(dpy.dpy, info.fontSet);
					}
				}
			}

} // class Font

} // namespace Xsharp
