/*
 * FontDialog.cs - Implementation of the
 *			"System.Windows.Forms.FontDialog" class.
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

using System.IO;
using System.Drawing;
using System.ComponentModel;

public class FontDialog : CommonDialog
{
	// Internal state.
	private FontDialogForm form;
	private bool allowScriptChange;
	private bool allowSimulations;
	private bool allowVectorFonts;
	private bool allowVerticalFonts;
	private bool fixedPitchOnly;
	private bool fontMustExist;
	private bool scriptsOnly;
	private bool showApply;
	private bool showColor;
	private bool showEffects;
	private bool showHelp;
	private Color color;
	private Font font;
	private int minSize;
	private int maxSize;

	// Constructor.
	public FontDialog()
			{
				// Make sure that the dialog fields have their default values.
				Reset();
			}

	// Get or set this object's properties.
	public bool AllowScriptChange
			{
				get
				{
					return allowScriptChange;
				}
				set
				{
					allowScriptChange = value;
				}
			}
	public bool AllowSimulations
			{
				get
				{
					return allowSimulations;
				}
				set
				{
					allowSimulations = value;
				}
			}
	public bool AllowVectorFonts
			{
				get
				{
					return allowVectorFonts;
				}
				set
				{
					allowVectorFonts = value;
				}
			}
	public bool AllowVerticalFonts
			{
				get
				{
					return allowVerticalFonts;
				}
				set
				{
					allowVerticalFonts = value;
				}
			}
	public Color Color
			{
				get
				{
					return color;
				}
				set
				{
					color = value;
				}
			}
	public bool FixedPitchOnly
			{
				get
				{
					return fixedPitchOnly;
				}
				set
				{
					fixedPitchOnly = value;
				}
			}
	public Font Font
			{
				get
				{
					return font;
				}
				set
				{
					font = value;
				}
			}
	public bool FontMustExist
			{
				get
				{
					return fontMustExist;
				}
				set
				{
					fontMustExist = value;
				}
			}
	public int MinSize
			{
				get
				{
					return minSize;
				}
				set
				{
					if(value < 0)
					{
						value = 0;
					}
					minSize = value;
				}
			}
	public int MaxSize
			{
				get
				{
					return maxSize;
				}
				set
				{
					if(value < 0)
					{
						value = 0;
					}
					maxSize = value;
				}
			}
	public bool ScriptsOnly
			{
				get
				{
					return scriptsOnly;
				}
				set
				{
					scriptsOnly = value;
				}
			}
	public bool ShowApply
			{
				get
				{
					return showApply;
				}
				set
				{
					showApply = value;
				}
			}
	public bool ShowColor
			{
				get
				{
					return showColor;
				}
				set
				{
					showColor = value;
				}
			}
	public bool ShowEffects
			{
				get
				{
					return showEffects;
				}
				set
				{
					showEffects = value;
				}
			}
	public bool ShowHelp
			{
				get
				{
					return showHelp;
				}
				set
				{
					showHelp = value;
				}
			}

	// Reset the dialog box controls to their default values.
	public override void Reset()
			{
				allowScriptChange = true;
				allowSimulations = true;
				allowVectorFonts = true;
				allowVerticalFonts = true;
				fixedPitchOnly = false;
				fontMustExist = false;
				scriptsOnly = false;
				showApply = false;
				showColor = false;
				showEffects = true;
				showHelp = false;
				color = Color.Black;
				font = null;
				minSize = 0;
				maxSize = 0;
			}

	// Run the dialog box, with a particular parent owner.
	protected override bool RunDialog(IntPtr hWndOwner)
			{
				// This version is not used in this implementation.
				return false;
			}
	internal override DialogResult RunDialog(IWin32Window owner)
			{
				// If the dialog is already visible, then bail out.
				if(form != null)
				{
					return DialogResult.Cancel;
				}

				// Construct the font dialog form.
				form = new FontDialogForm(this);

				// Run the dialog and get its result.
				DialogResult result;
				try
				{
					result = form.ShowDialog(owner);
				}
				finally
				{
					form.DisposeDialog();
					form = null;
				}

				// Return the final dialog result to the caller.
				return result;
			}

	// Convert this object into a string.
	public override string ToString()
			{
				return base.ToString() + ", Font: " +
					   ((font != null) ? font.ToString() : "(none)");
			}

	// Event that is emitted when the user clicks the "Apply" button.
	public event EventHandler Apply;

	// Emit the "Apply" event.
	protected virtual void OnApply(EventArgs e)
			{
				if(Apply != null)
				{
					Apply(this, e);
				}
			}

	// Hook procedure - not used in this implementation.
	protected override IntPtr HookProc(IntPtr hWnd, int msg,
									   IntPtr wparam, IntPtr lparam)
			{
				return IntPtr.Zero;
			}

	// Form that represents the font dialog.
	private class FontDialogForm : Form
	{
		// Internal state.
		private FontDialog dialog;

		// Constructor.
		public FontDialogForm(FontDialog dialog)
				{
					this.dialog = dialog;
					// TODO: create the form
				}

		// Dispose of this dialog.
		public void DisposeDialog()
				{
					Dispose(true);
				}

	}; // class FontDialogForm

}; // class FontDialog

}; // namespace System.Windows.Forms
