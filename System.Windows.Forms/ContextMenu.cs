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
	private ContextMenu nextPopupMenu;
	internal PopupControl popupControl;
	private Control associatedControl;
	private int currentMouseItem;

	internal event EventHandler PopDownAll;

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

	protected virtual void OnPopup(EventArgs e)
			{
				if (Popup != null)
					this.Popup(this, e);
			}

	// Show this context menu at the specified control-relative co-ordinates.
	public void Show(Control control, Point pos)
			{
				associatedControl = control;
				currentMouseItem = -1; // Not over anything
				popupControl = new PopupControl();
				popupControl.MouseMove +=new MouseEventHandler(OnMouseMove);
				popupControl.MouseDown +=new MouseEventHandler(OnMouseDown);
				popupControl.PopDown +=new EventHandler(popupControl_PopDown);
				OnPopup(EventArgs.Empty);
				using (Graphics g = popupControl.CreateGraphics())
				{
					Size size = MeasureItemBounds(g);
					Point pt = control.PointToScreen(pos);
					popupControl.Bounds = new Rectangle(
						pt,
						size);
					popupControl.Show();
				
					g.DrawRectangle(BorderPen, 0, 0, popupControl.Width - 1, popupControl.Height - 1);
					// Draw the menu items
					for (int i = 0; i < MenuItems.Count; i++)
						DrawMenuItem(g, i, false);
				}
			}

	private void OnMouseDown(Object s, MouseEventArgs e)
			{
				int item = ItemFromPoint(new Point(e.X, e.Y));
				if (item != -1)
				{
					MenuItem menuItem = MenuItems[item];
					if (nextPopupMenu != null)
					{
						nextPopupMenu.PopDown();
						nextPopupMenu = null;
					}
					if (menuItem.MenuItems.Count > 0)
					{
						nextPopupMenu = new ContextMenu(menuItem.itemList);
						Point location = new Point(itemBounds[item].Right, itemBounds[item].Y);
						location = popupControl.PointToScreen(location);
						location = associatedControl.PointToClient(location);

						nextPopupMenu.Show( associatedControl, location);
						nextPopupMenu.PopDownAll +=new EventHandler(nextPopupMenu_PopDownAll);
								
					}
					else
						PopDownRecurse();
					menuItem.PerformClick();
				}
			}

	private void OnMouseMove(Object s, MouseEventArgs e)
			{
				int newMouseItem = ItemFromPoint(new Point(e.X, e.Y));
				// Dont worry if the mouse is still on the same item.
				if (newMouseItem == currentMouseItem)
					return;
				using (Graphics g = popupControl.CreateGraphics())
				{
					if (currentMouseItem != -1)
						DrawMenuItem(g, currentMouseItem, false);
					if (newMouseItem != -1)
						DrawMenuItem(g, newMouseItem, true);
				}
				currentMouseItem = newMouseItem;
			}

	// Calculates the position of each MenuItem
	// Returns the bounds of all MenuItems
	private Size MeasureItemBounds(Graphics g)
	{
		Size outside = Size.Empty;
		itemBounds = new Rectangle[MenuItems.Count];
		int y = MenuPaddingOrigin.Y;
		for (int i = 0; i < MenuItems.Count; i++)
		{
			MenuItem item = MenuItems[i];
			int width, height;
			if (item.Text == "-")
			{
				height = -1;
				width = 0;
			}
			else
			{
				if (item.OwnerDraw)
				{
					MeasureItemEventArgs measureItem = new MeasureItemEventArgs(g, i);
					item.OnMeasureItem(measureItem);
					width = measureItem.ItemWidth;
					height = measureItem.ItemHeight;
				}
				else
				{
					// Do the default handling
					SizeF size = g.MeasureString(item.Text, SystemInformation.MenuFont);
					width = Size.Truncate(size).Width;
					height = Size.Truncate(size).Height;
				}
			}
			width += ItemPaddingSize.Width;
			Rectangle bounds =  new Rectangle(MenuPaddingOrigin.X, y, 0, height + ItemPaddingSize.Height);
			itemBounds[i] = bounds;
			y += bounds.Height;
			if (outside.Width < width)
				outside.Width = width;
		}
		if (outside.Width < MinimumItemWidth)
			outside.Width = MinimumItemWidth;
		for (int i = 0; i < MenuItems.Count; i++)
			itemBounds[i].Width = outside.Width;
		outside.Height = y + MenuPaddingSize.Height;
		outside.Width += MenuPaddingSize.Width;
		return outside;
	}

	protected int MinimumItemWidth
	{
		get
		{
			return 100;
		}
	}
	
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

	// Event that is emitted just before the menu pops up.
	public event EventHandler Popup;

	// The window that has captured the mouse has signaled
	private void popupControl_PopDown(object sender, EventArgs e)
	{
		if (PopDownAll != null)
			PopDownAll(this, EventArgs.Empty);
	}

	// The child has asked me to pop down.
	private void nextPopupMenu_PopDownAll(object sender, EventArgs e)
	{
		PopDownRecurse();
	}

	// Pop down the context menu at end of the chain. Causes all to pop down.
	internal void PopDownEnd()
	{
		ContextMenu current = this;
		for(;;)
		{
			if (current.popupControl == null)
				break;
			current = current.nextPopupMenu;
		}
		current.PopDownRecurse();
	}

	// Hide and then pop down the next in the chain.
	internal void PopDownRecurse()
	{
		popupControl.Hide();
		if (PopDownAll != null)
			PopDownAll(this, EventArgs.Empty);
	}

	// Pop down this context menu and its children.
	internal void PopDown()
	{
		if (nextPopupMenu != null)
		{
			nextPopupMenu.PopDown();
			nextPopupMenu = null;
		}
		popupControl.Hide();
	}
}; // class ContextMenu

}; // namespace System.Windows.Forms
