/*
 * Button.cs - Implementation of the
 *			"System.Windows.Forms.Button" class.
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

using System.Windows.Forms.Toolkit;

public class Button : ButtonBase
{
	// Internal result.
	private DialogResult result;

	// Contructor.
	public Button() {}

	// Get or set the dialog result associated with this button.
	public virtual DialogResult DialogResult
			{
				get
				{
					return result;
				}
				set
				{
					result = value;
				}
			}

	// Process a button click.
	protected override void OnClick(EventArgs e)
			{
				// Notify the form of the dialog result if necessary.
				if(result != DialogResult.None)
				{
					Form form = (TopLevelControl as Form);
					if(form != null)
					{
						form.DialogResult = result;
					}
				}

				// Perform the default button click behaviour.
				base.OnClick(e);
			}

}; // class Button

}; // namespace System.Windows.Forms
