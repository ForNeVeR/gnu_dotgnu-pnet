/*
 * ContextMenu.cs - Implementation of the
 *			"System.Windows.Forms.ContextMenu" class.
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

namespace System.Windows.Forms
{

using System.Drawing;

public class ContextMenu : Menu
{
	// Internal state.
	private RightToLeft rightToLeft;
	private Control sourceControl;

	// Constructors.
	public ContextMenu() : base(null)
			{
				rightToLeft = RightToLeft.Inherit;
			}
	public ContextMenu(MenuItem[] items) : base(items)
			{
				rightToLeft = RightToLeft.Inherit;
			}

	// Get or set the right-to-left property.
	public virtual RightToLeft RightToLeft
			{
				get
				{
					if(rightToLeft != RightToLeft.Inherit)
					{
						return rightToLeft;
					}
					else if(sourceControl != null)
					{
						return sourceControl.RightToLeft;
					}
					else
					{
						return RightToLeft.No;
					}
				}
				set
				{
					if(rightToLeft != value)
					{
						SuppressUpdates();
						rightToLeft = value;
						AllowUpdates();
					}
				}
			}

	// Get the control that owns this context menu.
	public Control SourceControl
			{
				get
				{
					return sourceControl;
				}
			}

	// Show this context menu at the specified control-relative co-ordinates.
	[TODO]
	public void Show(Control control, Point pos)
			{
				// TODO: display the menu and wait for it to be dismissed
			}

	// Event that is emitted just before the menu pops up.
	public event EventHandler Popup;

	// Add this main menu to a control.
	internal void AddToControl(Control control)
			{
				sourceControl = control;
			}

	// Remove this main menu from its owning control.
	internal void RemoveFromControl()
			{
				sourceControl = null;
			}

}; // class ContextMenu

}; // namespace System.Windows.Forms
