/*
 * SystemBrushes.cs - Implementation of the
 *			"System.Drawing.SystemBrushes" class.
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

public sealed class SystemBrushes
{
	// Cannot instantiate this class.
	private SystemBrushes() {}

	// Get a brush for a system color.
	public static Brush FromSystemColor(Color c)
			{
				return new SolidBrush(c);
			}

	// Standard system brush objects.
	public static Brush ActiveBorder
			{
				get
				{
					return new SolidBrush(SystemColors.ActiveBorder);
				}
			}
	public static Brush ActiveCaption
			{
				get
				{
					return new SolidBrush(SystemColors.ActiveCaption);
				}
			}
	public static Brush ActiveCaptionText
			{
				get
				{
					return new SolidBrush(SystemColors.ActiveCaptionText);
				}
			}
	public static Brush AppWorkspace
			{
				get
				{
					return new SolidBrush(SystemColors.AppWorkspace);
				}
			}
	public static Brush Control
			{
				get
				{
					return new SolidBrush(SystemColors.Control);
				}
			}
	public static Brush ControlDark
			{
				get
				{
					return new SolidBrush(SystemColors.ControlDark);
				}
			}
	public static Brush ControlDarkDark
			{
				get
				{
					return new SolidBrush(SystemColors.ControlDarkDark);
				}
			}
	public static Brush ControlLight
			{
				get
				{
					return new SolidBrush(SystemColors.ControlLight);
				}
			}
	public static Brush ControlLightLight
			{
				get
				{
					return new SolidBrush(SystemColors.ControlLightLight);
				}
			}
	public static Brush ControlText
			{
				get
				{
					return new SolidBrush(SystemColors.ControlText);
				}
			}
	public static Brush Desktop
			{
				get
				{
					return new SolidBrush(SystemColors.Desktop);
				}
			}
	public static Brush GrayText
			{
				get
				{
					return new SolidBrush(SystemColors.GrayText);
				}
			}
	public static Brush Highlight
			{
				get
				{
					return new SolidBrush(SystemColors.Highlight);
				}
			}
	public static Brush HighlightText
			{
				get
				{
					return new SolidBrush(SystemColors.HighlightText);
				}
			}
	public static Brush HotTrack
			{
				get
				{
					return new SolidBrush(SystemColors.HotTrack);
				}
			}
	public static Brush InactiveBorder
			{
				get
				{
					return new SolidBrush(SystemColors.InactiveBorder);
				}
			}
	public static Brush InactiveCaption
			{
				get
				{
					return new SolidBrush(SystemColors.InactiveCaption);
				}
			}
	public static Brush InactiveCaptionText
			{
				get
				{
					return new SolidBrush(SystemColors.InactiveCaptionText);
				}
			}
	public static Brush Info
			{
				get
				{
					return new SolidBrush(SystemColors.Info);
				}
			}
	public static Brush InfoText
			{
				get
				{
					return new SolidBrush(SystemColors.InfoText);
				}
			}
	public static Brush Menu
			{
				get
				{
					return new SolidBrush(SystemColors.Menu);
				}
			}
	public static Brush MenuText
			{
				get
				{
					return new SolidBrush(SystemColors.MenuText);
				}
			}
	public static Brush ScrollBar
			{
				get
				{
					return new SolidBrush(SystemColors.ScrollBar);
				}
			}
	public static Brush Window
			{
				get
				{
					return new SolidBrush(SystemColors.Window);
				}
			}
	public static Brush WindowFrame
			{
				get
				{
					return new SolidBrush(SystemColors.WindowFrame);
				}
			}
	public static Brush WindowText
			{
				get
				{
					return new SolidBrush(SystemColors.WindowText);
				}
			}

}; // class SystemBrushes

}; // namespace System.Drawing
