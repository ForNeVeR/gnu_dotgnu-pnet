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
	[TODO]
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
					// TODO: update the dialog box to match.
					title = value;
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
	[TODO]
	internal override DialogResult RunDialog(IWin32Window owner)
			{
				// TODO
				return DialogResult.None;
			}

	// Convert this object into a string.
	public override string ToString()
			{
				return base.ToString() + ": Title: " + Title +
					   ", FileName: " + FileName;
			}

	// Event that is raised to check that a file is OK.
	public event CancelEventHandler FileOk;

}; // class FileDialog

}; // namespace System.Windows.Forms
