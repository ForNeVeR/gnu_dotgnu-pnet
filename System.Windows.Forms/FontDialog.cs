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
				if(form != null)
				{
					form.UpdateDialog();
				}
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
	internal void EmitApply(EventArgs e)
			{
				OnApply(e);
			}

	// Hook procedure - not used in this implementation.
	protected override IntPtr HookProc(IntPtr hWnd, int msg,
									   IntPtr wparam, IntPtr lparam)
			{
				return IntPtr.Zero;
			}

	// Control that is used to fill empty space in layouts.
	private class EmptyControl : Control
	{
		// Constructor.
		public EmptyControl()
				{
					TabStop = false;
					SetStyle(ControlStyles.Selectable, false);
				}

	}; // class EmptyControl

	// Sizes to display in the size list.
	private static readonly int[] sizes =
		{8, 9, 10, 11, 12, 14, 16, 18, 20, 22,
		 24, 28, 30, 32, 34, 36, 38, 40};

	// Form that represents the font dialog.
	private class FontDialogForm : Form
	{
		// Internal state.
		private FontDialog dialog;
		private HBoxLayout hbox;		// Main layout for dialog.
		private VBoxLayout vbox1;		// Left-hand side.
		private VBoxLayout vbox2;		// Push buttons on right-hand side.
		private GridLayout grid;		// Font name and size grid.
		private HBoxLayout hbox2;		// Grid plus effects.
		private Button okButton;
		private Button cancelButton;
		private Button applyButton;
		private Button helpButton;
		private TextBox name;
		private TextBox size;
		private ListBox nameList;
		private ListBox sizeList;
		private Control sample;
		private GroupBox effects;

		// Constructor.
		public FontDialogForm(FontDialog dialog)
				{
					// Record the parent for later access.
					this.dialog = dialog;

					// Set the title.
					Text = S._("SWF_FontDialog_Title", "Font");

					// Construct the layout boxes for the font dialog.
					hbox = new HBoxLayout();
					hbox.Dock = DockStyle.Fill;
					vbox1 = new VBoxLayout();
					vbox2 = new VBoxLayout();
					hbox2 = new HBoxLayout();
					grid = new GridLayout(2, 3);
					grid.StretchColumn = 0;
					effects = new GroupBox();
					effects.Text = S._("SWF_FontDialog_Effects", "Effects");
					effects.Width = 60;
					hbox.Controls.Add(vbox1);
					hbox.Controls.Add(vbox2);
					hbox.StretchControl = vbox1;
					hbox2.Controls.Add(grid);
					hbox2.Controls.Add(effects);
					hbox2.StretchControl = grid;
					vbox1.Controls.Add(hbox2);
					vbox1.StretchControl = hbox2;

					// Create the main display area.
					Label label;
					label = new Label();
					label.Text = S._("SWF_FontDialog_Name", "Font:");
					name = new TextBox();
					nameList = new ListBox();
					grid.SetControl(0, 0, label);
					grid.SetControl(0, 1, name);
					grid.SetControl(0, 2, nameList);
					label = new Label();
					label.Text = S._("SWF_FontDialog_Size", "Size:");
					size = new TextBox();
					size.Width = 40;
					sizeList = new ListBox();
					sizeList.Width = 40;
					grid.SetControl(1, 0, label);
					grid.SetControl(1, 1, size);
					grid.SetControl(1, 2, sizeList);

					// Create the buttons.
					okButton = new Button();
					okButton.Text = S._("SWF_MessageBox_OK", "OK");
					cancelButton = new Button();
					cancelButton.Text = S._("SWF_MessageBox_Cancel", "Cancel");
					applyButton = new Button();
					applyButton.Text = S._("SWF_MessageBox_Apply", "Apply");
					helpButton = new Button();
					helpButton.Text = S._("SWF_MessageBox_Help", "Help");
					vbox2.Controls.Add(okButton);
					vbox2.Controls.Add(cancelButton);
					vbox2.Controls.Add(applyButton);
					vbox2.Controls.Add(helpButton);
					vbox2.Controls.Add(new EmptyControl());
					AcceptButton = okButton;
					CancelButton = cancelButton;

					// Create the effects and sample boxes.
					sample = new Control();
					sample.ForeColor = SystemColors.WindowText;
					sample.BackColor = SystemColors.Window;
					sample.BorderStyleInternal = BorderStyle.Fixed3D;
					sample.Height = 60;
					vbox1.Controls.Add(sample);

					// Add the top-level hbox to the dialog and set the size.
					Controls.Add(hbox);
					Size size = hbox.RecommendedSize;
					if(size.Width < 450)
					{
						size.Width = 450;
					}
					if(size.Height < 280)
					{
						size.Height = 280;
					}
					ClientSize = size;
					MinimumSize = size;
					MinimizeBox = false;
					ShowInTaskbar = false;

					// Fill in the font names and sizes.
					nameList.BeginUpdate();
					foreach(FontFamily family in FontFamily.Families)
					{
						nameList.Items.Add(family.Name);
					}
					nameList.EndUpdate();
					sizeList.BeginUpdate();
					foreach(int value in sizes)
					{
						sizeList.Items.Add(value);
					}
					sizeList.EndUpdate();

					// Hook up interesting events.
					okButton.Click += new EventHandler(AcceptDialog);
					cancelButton.Click += new EventHandler(CancelDialog);
					applyButton.Click += new EventHandler(ApplyButtonClicked);
					helpButton.Click += new EventHandler(HelpButtonClicked);
					nameList.SelectedIndexChanged
						+= new EventHandler(NameIndexChanged);
					sizeList.SelectedIndexChanged
						+= new EventHandler(SizeIndexChanged);

					// Match the requested settings from the dialog parent.
					UpdateDialog();
				}

		// Dispose of this dialog.
		public void DisposeDialog()
				{
					Dispose(true);
				}

		// Update the dialog to match the "FontDialog" properties.
		public void UpdateDialog()
				{
					int index;
					applyButton.Visible = dialog.ShowApply;
					helpButton.Visible = dialog.ShowHelp;
					HelpButton = dialog.ShowHelp;
					Font font = dialog.Font;
					if(font == null)
					{
						// Use the default font to start with.
						font = this.Font;
					}
					index = nameList.Items.IndexOf(font.Name);
					if(index >= 0)
					{
						nameList.SelectedIndex = index;
					}
					else
					{
						name.Text = font.Name;
					}
				#if false
					switch(font.Style & (FontStyle.Bold | FontStyle.Italic))
					{
						case FontStyle.Regular:
						default:
						{
							index = 0;
						}
						break;

						case FontStyle.Bold:
						{
							index = 1;
						}
						break;

						case FontStyle.Italic:
						{
							index = 2;
						}
						break;

						case FontStyle.Bold | FontStyle.Italic:
						{
							index = 3;
						}
						break;
					}
					styleList.SelectedIndex = index;
				#endif
					index = sizeList.Items.IndexOf((int)(font.Size));
					if(index >= 0)
					{
						sizeList.SelectedIndex = index;
					}
					else
					{
						size.Text = ((int)(font.Size)).ToString();
					}
				}

		// Set the font in the return dialog object.
		private void SetFont()
				{
					// TODO
				}

		// Process a help request on the form.
		protected override void OnHelpRequested(HelpEventArgs e)
				{
					base.OnHelpRequested(e);
					dialog.EmitHelpRequest(e);
				}

		// Handle the "accept" button on this dialog.
		public void AcceptDialog(Object sender, EventArgs e)
				{
					SetFont();
					DialogResult = DialogResult.OK;
				}

		// Handle the "cancel" button on this dialog.
		private void CancelDialog(Object sender, EventArgs e)
				{
					DialogResult = DialogResult.Cancel;
				}

		// Handle the "apply" button on this dialog.
		private void ApplyButtonClicked(Object sender, EventArgs e)
				{
					SetFont();
					dialog.EmitApply(e);
				}

		// Handle the "help" button on this dialog.
		private void HelpButtonClicked(Object sender, EventArgs e)
				{
					dialog.EmitHelpRequest(e);
				}

		// Handle a "closing" event on this form.
		protected override void OnClosing(CancelEventArgs e)
				{
					base.OnClosing(e);
					e.Cancel = true;
					DialogResult = DialogResult.Cancel;
				}

		// Handle a change of index in the font name list.
		private void NameIndexChanged(Object sender, EventArgs e)
				{
					int index = nameList.SelectedIndex;
					if(index >= 0)
					{
						name.Text = (nameList.Items[index]).ToString();
					}
					else
					{
						name.Text = String.Empty;
					}
				}

		// Handle a change of index in the size list.
		private void SizeIndexChanged(Object sender, EventArgs e)
				{
					int index = sizeList.SelectedIndex;
					if(index >= 0)
					{
						size.Text = (sizeList.Items[index]).ToString();
					}
				}

	}; // class FontDialogForm

}; // class FontDialog

}; // namespace System.Windows.Forms
