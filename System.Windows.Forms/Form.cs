/*
 * Form.cs - Implementation of the
 *			"System.Windows.Forms.Form" class.
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

using System.Drawing.Toolkit;

public class Form : Control
{
	// TODO

	public DialogResult DialogResult
			{
				get
				{
					// TODO
					return DialogResult.None;
				}
				set
				{
					// TODO
				}
			}

	// Create the toolkit window underlying this control.
	internal override IToolkitWindow CreateToolkitWindow(IToolkitWindow parent)
			{
				// TODO: dialog forms
				CreateParams cp = CreateParams;
				IToolkitWindow window =
					ToolkitManager.Toolkit.CreateTopLevelWindow
						(cp.Width, cp.Height);
				window.SetTitle(cp.Caption);
				return window;
			}

	// Determine if this is a top-level control which cannot have parents.
	internal override bool IsTopLevel
			{
				get
				{
					return true;
				}
			}

}; // class Form

}; // namespace System.Windows.Forms
