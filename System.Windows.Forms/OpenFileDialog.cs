/*
 * OpenFileDialog.cs - Implementation of the
 *			"System.Windows.Forms.OpenFileDialog" class.
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

public sealed class OpenFileDialog : FileDialog
{
	// Internal state.
	private bool multiselect;
	private bool readOnlyChecked;
	private bool showReadOnly;

	// Constructor
	public OpenFileDialog() {}

	// Get or set this object's properties.
	public override bool CheckFileExists
			{
				get
				{
					return base.CheckFileExists;
				}
				set
				{
					base.CheckFileExists = value;
				}
			}
	public bool Multiselect
			{
				get
				{
					return multiselect;
				}
				set
				{
					multiselect = value;
				}
			}
	[TODO]
	public bool ReadOnlyChecked
			{
				get
				{
					return readOnlyChecked;
				}
				set
				{
					// TODO: update the dialog box to match.
					readOnlyChecked = value;
				}
			}
	public bool ShowReadOnly
			{
				get
				{
					return showReadOnly;
				}
				set
				{
					showReadOnly = value;
				}
			}
	internal override String DefaultTitle
			{
				get
				{
					return S._("SWF_FileDialog_OpenTitle", "Open");
				}
			}

	// Open the file specified by this dialog box.
	public Stream OpenFile()
			{
				String filename = FileName;
				if(filename == null || filename.Length == 0)
				{
					throw new ArgumentNullException("FileName");
				}
				return new FileStream(filename, FileMode.Open,
									  FileAccess.Read, FileShare.Read);
			}

	// Reset the contents of the dialog box.
	public override void Reset()
			{
				base.Reset();
				checkFileExists = true;
				multiselect = false;
				readOnlyChecked = false;
				showReadOnly = false;
			}

}; // class OpenFileDialog

}; // namespace System.Windows.Forms
