/*
 * MainMenu.cs - Implementation of the
 *			"System.Windows.Forms.MainMenu" class.
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

	public class MainMenu : Menu
	{
		// Internal state.
		private RightToLeft rightToLeft;
		private Form ownerForm;
		private ContextMenu menuPopup;
		private int currentMouseItem = -1;

		// Constructors.
		public MainMenu() : base(null)
		{
			rightToLeft = RightToLeft.Inherit;
		}
		public MainMenu(MenuItem[] items) : base(items)
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
				else if(ownerForm != null)
				{
					return ownerForm.RightToLeft;
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

		// Clone this main menu.
		[TODO]
		public virtual MainMenu CloneMenu()
		{
			// TODO
			return this;
		}

		// Get the form that owns this menu.
		public Form GetForm()
		{
			return ownerForm;
		}

		// Convert this object into a string.
		public override String ToString()
		{
			if(ownerForm != null)
			{
				return base.ToString() + ", GetForm: " +
					ownerForm.ToString();
			}
			else
			{
				return base.ToString();
			}
		}

#if CONFIG_COMPONENT_MODEL

	// Dispose of this menu.
	protected override void Dispose(bool disposing)
			{
				base.Dispose(disposing);
			}

#endif

		// Add this main menu to a form.
		internal void AddToForm(Form form)
		{
			// Measure the positions
			ownerForm = form;
			using (Graphics g = ownerForm.CreateNonClientGraphics())
			{
				MeasureItemBounds(g);
			}
			// Cause the form to reposition its controls 
			// now that the client area has changed
			form.PerformLayout();
		}

		// Remove this main menu from its owning form.
		internal void RemoveFromForm()
		{
			// Cause the form to reposition its controls 
			// now that the client area has changed
			ownerForm.PerformLayout();
			ownerForm = null;
		}

		internal void OnPaint()
		{		
			using (Graphics g = ownerForm.CreateNonClientGraphics())
			{
				for (int i = 0; i < MenuItems.Count; i++)
					DrawMenuItem(g, i, false);
			}
		}

		// Calculates the position of each MenuItem
		private void MeasureItemBounds(Graphics g)
		{
			itemBounds = new Rectangle[MenuItems.Count];
			int x = MenuPaddingOrigin.X;
			Rectangle menuBounds = new Rectangle(MenuPaddingOrigin.X, MenuPaddingOrigin.Y, ownerForm.Width - MenuPaddingSize.Width, ownerForm.Height - MenuPaddingSize.Height);
			for (int i = 0; i < MenuItems.Count; i++)
			{
				MenuItem item = MenuItems[i];
				int width;
				if (item.OwnerDraw)
				{
					MeasureItemEventArgs measureItem = new MeasureItemEventArgs(g, i, SystemInformation.MenuHeight);
					item.OnMeasureItem(measureItem);
					width = measureItem.ItemWidth;
				}
				else
				{
					// Do the default handling
					SizeF size = g.MeasureString(item.Text, SystemInformation.MenuFont);
					width = Size.Truncate(size).Width;
				}
				Rectangle bounds =  new Rectangle(x, 0, width + ItemPaddingSize.Width, SystemInformation.MenuHeight);
				bounds.IntersectsWith(menuBounds);
				itemBounds[i] = bounds;
				x += bounds.Width;
			}
		}

		internal void OnMouseDown( MouseEventArgs e)
		{
			// Get the mouse coordinate relative to the control
			int y = e.Y + SystemInformation.MenuHeight;
			// Search the main menu
			int item = ItemFromPoint(new Point(e.X, y));
			if (item != -1)
			{
				if (menuPopup != null)
				{
					menuPopup.popupControl.Hide();
					menuPopup = null;
				}
				MenuItemCollection subMenus = MenuItems[item].MenuItems;
				if (subMenus.Count > 0)
				{
					menuPopup = new ContextMenu(MenuItems[item].itemList);
					Point pt = new Point(itemBounds[item].Left, 0);
					menuPopup.Show(ownerForm, pt);
				}
				else
					MenuItems[item].PerformClick();
			}
		}

		
		internal void OnMouseMove(MouseEventArgs e)
		{
			// Get the mouse coordinate relative to the control
			int y = e.Y + SystemInformation.MenuHeight;
			int newMouseItem = ItemFromPoint(new Point(e.X, y));
			// Dont worry if the mouse is still on the same item.
			if (newMouseItem == currentMouseItem)
				return;
			using (Graphics g = ownerForm.CreateNonClientGraphics())
			{
				if (currentMouseItem != -1)
					DrawMenuItem(g, currentMouseItem, false);
				if (newMouseItem != -1)
					DrawMenuItem(g, newMouseItem, true);
			}
			currentMouseItem = newMouseItem;
		}

		internal void OnMouseLeave()
		{
			if (currentMouseItem == -1)
				return;
			using (Graphics g = ownerForm.CreateNonClientGraphics())
				DrawMenuItem(g, currentMouseItem, false);
			currentMouseItem = -1;
		}
	
}; // class MainMenu

}; // namespace System.Windows.Forms
