/*
 * FontExtents.cs - Extent information for a font.
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

namespace Xsharp
{

using System;
using Xsharp.Types;

/// <summary>
/// <para>The <see cref="T:Xsharp.FontExtents"/> class encapsulates
/// extent information for a font, including ascent, descent,
/// and maximum character width information.</para>
/// </summary>
public sealed class FontExtents
{
	// Internal state.
	private int ascent;
	private int descent;
	private int maxWidth;

	// Constructor.
	internal FontExtents(Font font, IntPtr fontSet)
			{
				XRectangle max_ink;
				XRectangle max_logical;

				// Get the extent information for the font.
				Xlib.XSharpFontExtents(fontSet, out max_ink, out max_logical);

				// Convert the extent information into values that make sense.
				ascent = -(max_logical.y);
				descent = max_logical.height + max_logical.y;
				maxWidth = max_logical.width;

				// Increase the descent to account for underlining.
				// We always draw the underline two pixels below
				// the font base line.
				if((font.Style & FontStyle.Underlined) != 0)
				{
					if(descent < 3)
					{
						descent = 3;
					}
				}
			}

	/// <summary>
	/// <para>Get the font's ascent, in pixels.</para>
	/// </summary>
	public int Ascent
			{
				get
				{
					return ascent;
				}
			}

	/// <summary>
	/// <para>Get the font's desscent, in pixels.</para>
	/// </summary>
	public int Descent
			{
				get
				{
					return descent;
				}
			}

	/// <summary>
	/// <para>Get the font's maximum character width, in pixels.</para>
	/// </summary>
	public int MaxCharWidth
			{
				get
				{
					return maxWidth;
				}
			}

} // class FontExtents

} // namespace Xsharp
