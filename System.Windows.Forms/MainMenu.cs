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

public class MainMenu : Menu
{
	// Internal state.
	private RightToLeft rightToLeft;
	private Form ownerForm;

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
				ownerForm = form;
			}

	// Remove this main menu from its owning form.
	internal void RemoveFromForm()
			{
				ownerForm = null;
			}

}; // class MainMenu

}; // namespace System.Windows.Forms
