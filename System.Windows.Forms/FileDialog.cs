/*
 * FileDialog.cs - Implementation of the
 *			"System.Windows.Forms.FileDialog" class.
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
using System.ComponentModel;

public abstract class FileDialog : CommonDialog
{
	// Internal state.
	private bool addExtension;
	internal bool checkFileExists;
	private bool checkPathExists;
	private bool dereferenceLinks;
	private String defaultExt;
	private String fileName;
	private String[] fileNames;
	private String filter;
	private int filterIndex;
	private String initialDirectory;
	private bool restoreDirectory;
	private bool showHelp;
	private bool validateNames;
	private String title;
	private FileDialogForm form;

	// Event identifier for the "FileOk" event.
	protected static readonly object EventFileOk = new Object();

	// Constructor.
	internal FileDialog()
			{
				// Make sure that the dialog fields have their default values.
				Reset();
			}

	// Get or set this object's properties.
	public bool AddExtension
			{
				get
				{
					return addExtension;
				}
				set
				{
					addExtension = value;
				}
			}
	public virtual bool CheckFileExists
			{
				get
				{
					return checkFileExists;
				}
				set
				{
					checkFileExists = value;
				}
			}
	public bool CheckPathExists
			{
				get
				{
					return checkPathExists;
				}
				set
				{
					checkPathExists = value;
				}
			}
	public String DefaultExt
			{
				get
				{
					return defaultExt;
				}
				set
				{
					defaultExt = value;
				}
			}
	public bool DereferenceLinks
			{
				get
				{
					return dereferenceLinks;
				}
				set
				{
					dereferenceLinks = value;
				}
			}
	[TODO]
	public String FileName
			{
				get
				{
					return fileName;
				}
				set
				{
					// TODO: update the dialog box to match.
					fileName = value;
					fileNames = null;
				}
			}
	public String[] FileNames
			{
				get
				{
					if(fileNames == null)
					{
						if(fileName != null && fileName.Length > 0)
						{
							fileNames = new String [] {fileName};
							return fileNames;
						}
						else
						{
							return new String [0];
						}
					}
					else
					{
						return fileNames;
					}
				}
			}
	[TODO]
	public String Filter
			{
				get
				{
					return filter;
				}
				set
				{
					// TODO: validate the filter string.
					// TODO: update the dialog box to match.
					filter = value;
				}
			}
	[TODO]
	public int FilterIndex
			{
				get
				{
					return filterIndex;
				}
				set
				{
					// TODO: validate the filter index.
					// TODO: update the dialog box to match.
					filterIndex = value;
				}
			}
	public String InitialDirectory
			{
				get
				{
					return initialDirectory;
				}
				set
				{
					initialDirectory = value;
				}
			}
	protected virtual IntPtr Instance
			{
				get
				{
					// Not used in this implementation.
					return IntPtr.Zero;
				}
			}
	protected int Options
			{
				get
				{
					// Not used in this implementation.
					return 0;
				}
			}
	public bool RestoreDirectory
			{
				get
				{
					return restoreDirectory;
				}
				set
				{
					restoreDirectory = value;
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
	public String Title
			{
				get
				{
					if(title == null || title == String.Empty)
					{
						return DefaultTitle;
					}
					return title;
				}
				set
				{
					title = value;
					if(form != null)
					{
						if(value == null)
						{
							form.Text = String.Empty;
						}
						else
						{
							form.Text = value;
						}
					}
				}
			}
	internal virtual String DefaultTitle
			{
				get
				{
					// Overridden by subclasses to supply "Open" or "Save As".
					return String.Empty;
				}
			}
	public bool ValidateNames
			{
				get
				{
					return validateNames;
				}
				set
				{
					validateNames = value;
				}
			}
	internal virtual String OkButtonName
			{
				get
				{
					return S._("SWF_FileDialog_OpenButton", "Open");
				}
			}

	// Hook procedure - not used in this implementation.
	protected override IntPtr HookProc(IntPtr hWnd, int msg,
									   IntPtr wparam, IntPtr lparam)
			{
				return IntPtr.Zero;
			}

	// Raise the "FileOk" event.
	protected void OnFileOk(CancelEventArgs e)
			{
				if(FileOk != null)
				{
					FileOk(this, e);
				}
			}

	// Reset the dialog box controls to their default values.
	[TODO]
	public override void Reset()
			{
				addExtension = true;
				checkFileExists = false;
				checkPathExists = true;
				dereferenceLinks = true;
				defaultExt = String.Empty;
				fileName = String.Empty;
				fileNames = null;
				filter = "All files (*.*)|*.*";
				filterIndex = 1;
				initialDirectory = String.Empty;
				restoreDirectory = false;
				showHelp = false;
				validateNames = true;
				title = String.Empty;
				// TODO: update an active dialog box form to match.
			}

	// Run the dialog box.
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

				// Construct the file dialog form.
				form = new FileDialogForm(this);

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
				return result;
			}

	// Convert this object into a string.
	public override string ToString()
			{
				return base.ToString() + ": Title: " + Title +
					   ", FileName: " + FileName;
			}

	// Event that is raised to check that a file is OK.
	public event CancelEventHandler FileOk;

	// Form that defines the UI of a file dialog.
	private sealed class FileDialogForm : Form
	{
		// Internal state.
		private FileDialog fileDialogParent;
		private VBoxLayout vbox;
		private HBoxLayout hbox1;
		private HBoxLayout hbox2;
		private HBoxLayout hbox3;
		private ListBox listBox;
		private ComboBox directory;
		private Button upButton;
		private Label nameLabel;
		private TextBox name;
		private Label typeLabel;
		private ComboBox type;
		private Button okButton;
		private Button cancelButton;

		// Constructor.
		public FileDialogForm(FileDialog fileDialogParent)
				{
					// Record the parent for later access.
					this.fileDialogParent = fileDialogParent;

					// Set the caption to that specified in the parent.
					Text = fileDialogParent.Title;

					// Construct the layout boxes for the file dialog.
					vbox = new VBoxLayout();
					vbox.Dock = DockStyle.Fill;
					hbox1 = new HBoxLayout();
					listBox = new ListBox();
					hbox2 = new HBoxLayout();
					hbox3 = new HBoxLayout();
					vbox.Controls.Add(hbox1);
					vbox.Controls.Add(listBox);
					vbox.Controls.Add(hbox2);
					vbox.Controls.Add(hbox3);
					vbox.StretchControl = listBox;

					// Add the top line (directory name and up button).
					directory = new ComboBox();
					upButton = new Button();
					upButton.FlatStyle = FlatStyle.Popup;
					upButton.Text = "Up";	// TODO: change to an image.
					hbox1.StretchControl = directory;
					hbox1.Controls.Add(directory);
					hbox1.Controls.Add(upButton);

					// The second line is "listBox", already created above.

					// Add the third line (file name fields).
					nameLabel = new Label();
					nameLabel.Text = "File name:";	// TODO: translate.
					name = new TextBox();
					okButton = new Button();
					okButton.Text = fileDialogParent.OkButtonName;
					hbox2.StretchControl = name;
					hbox2.Controls.Add(nameLabel);
					hbox2.Controls.Add(name);
					hbox2.Controls.Add(okButton);

					// Add the fourth line (file type fields).
					typeLabel = new Label();
					typeLabel.Text = "Files of type:";	// TODO: translate.
					type = new ComboBox();
					cancelButton = new Button();
					cancelButton.Text = S._("SWF_MessageBox_Cancel", "Cancel");
					hbox3.StretchControl = type;
					hbox3.Controls.Add(typeLabel);
					hbox3.Controls.Add(type);
					hbox3.Controls.Add(cancelButton);

					// Add the top-level vbox to the dialog and set the size.
					Controls.Add(vbox);
					Size size = vbox.RecommendedSize;
					if(size.Width < 500)
					{
						size.Width = 500;
					}
					if(size.Height < 300)
					{
						size.Height = 300;
					}
					ClientSize = size;
				}

		// Dispose of this dialog.
		public void DisposeDialog()
				{
					Dispose(true);
				}

	}; // class FileDialogForm

}; // class FileDialog

}; // namespace System.Windows.Forms
