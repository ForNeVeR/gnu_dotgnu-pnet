/*
 * RootWindow.cs - Root window handling for X applications.
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
using System.Collections;

/// <summary>
/// <para>The <see cref="T:Xsharp.RootWindow"/> class manages the
/// root window for an X display screen.</para>
///
/// <para>The root window is special in that it does not have a parent,
/// and that it cannot be destroyed, resized, or moved.</para>
/// </summary>
public sealed class RootWindow : Widget
{
	// Constructor.  Called from the "Screen" class.
	internal RootWindow(Display dpy, Screen screen, Xlib.Window handle)
			: base(dpy, screen, DrawableKind.Widget, null)
			{
				// Set this window's handle and add it to the handle map.
				this.handle = (Xlib.Drawable)handle;
				if(dpy.handleMap == null)
				{
					dpy.handleMap = new Hashtable();
				}
				dpy.handleMap[(int)handle] = this;

				// Adjust the root window object to match the screen state.
				width = (int)(Xlib.XWidthOfScreen(screen.screen));
				height = (int)(Xlib.XHeightOfScreen(screen.screen));
				mapped = true;
				AutoMapChildren = false;
			}

	/// <summary>
	/// <para>Destroy this window if it is currently active.</para>
	/// </summary>
	///
	/// <remarks>
	/// <para>The root window cannot be destroyed except by closing
	/// the connection to the X display server.  If this method is
	/// called on a root window, the request will be ignored.</para>
	/// </remarks>
	public override void Destroy()
			{
				// Nothing to do here.
			}

	/// <summary>
	/// <para>Move this widget to a new location relative to its parent.</para>
	/// </summary>
	///
	/// <param name="x">
	/// <para>The X co-ordinate of the new top-left widget corner.</para>
	/// </param>
	///
	/// <param name="y">
	/// <para>The Y co-ordinate of the new top-left widget corner.</para>
	/// </param>
	///
	/// <exception cref="T:Xsharp.XException">
	/// <para>Raised if <paramref name="x"/> or <paramref name="y"/>
	/// is out of range.</para>
	/// </exception>
	public override void Move(int x, int y)
			{
				throw new XInvalidOperationException
					(S._("X_NonRootOperation"));
			}

	/// <summary>
	/// <para>Resize this widget to a new sie.</para>
	/// </summary>
	///
	/// <param name="width">
	/// <para>The new width for the widget.</para>
	/// </param>
	///
	/// <param name="height">
	/// <para>The new width for the widget.</para>
	/// </param>
	///
	/// <exception cref="T:Xsharp.XException">
	/// <para>Raised if <paramref name="width"/> or <paramref name="height"/>
	/// is out of range.</para>
	/// </exception>
	public override void Resize(int width, int height)
			{
				throw new XInvalidOperationException
					(S._("X_NonRootOperation"));
			}

	/// <summary>
	/// <para>Map this widget to the screen.</para>
	/// </summary>
	public override void Map()
			{
				throw new XInvalidOperationException
					(S._("X_NonRootOperation"));
			}

	/// <summary>
	/// <para>Unmap this widget from the screen.</para>
	/// </summary>
	public override void Unmap()
			{
				throw new XInvalidOperationException
					(S._("X_NonRootOperation"));
			}

	/// <summary>
	/// <para>Get or set the cursor that is associated with this widget.</para>
	/// </summary>
	///
	/// <value>
	/// <para>The cursor shape to set for the widget.  If the value is
	/// <c>CursorType.XC_inherit_parent</c>, then the widget inherits the
	/// cursor that is set on the parent widget.</para>
	/// </value>
	public override CursorType Cursor
			{
				get
				{
					// We don't know what the root window has set
					// cursor to, so we always return the default.
					return CursorType.XC_inherit_parent;
				}
				set
				{
					// Cannot change the mouse cursor on the root window.
					throw new XInvalidOperationException
						(S._("X_NonRootOperation"));
				}
			}

} // class RootWindow

} // namespace Xsharp
