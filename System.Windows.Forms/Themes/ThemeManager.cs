/*
 * ThemeManager.cs - Implementation of the
 *			"System.Windows.Forms.Themes.ThemeManager" class.
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

namespace System.Windows.Forms.Themes
{

internal sealed class ThemeManager
{
	// Internal state.
	private static IThemePainter mainPainter;
	private static IThemePainter systemPainter;

	// Initialize the theme painters.
	static ThemeManager()
			{
				mainPainter = new DefaultThemePainter();
				systemPainter = mainPainter;
			}

	// Get the main .NET theme painter.
	public static IThemePainter MainPainter
			{
				get
				{
					return mainPainter;
				}
			}

	// Get the system theme painter, which will usually be the same
	// as the main painter, but may differ in some themes.
	public static IThemePainter SystemPainter
			{
				get
				{
					return systemPainter;
				}
			}

	// Get the painter for a particular flat style.
	public static IThemePainter PainterForStyle(FlatStyle style)
			{
				if(style == FlatStyle.System)
				{
					return SystemPainter;
				}
				else
				{
					return MainPainter;
				}
			}

}; // class ThemeManager

}; // namespace System.Windows.Forms.Themes
