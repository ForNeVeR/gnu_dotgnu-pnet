/*
 * FileDialog.cs - Implementation of the "System.Windows.Forms.FileDialog" class.
 *
 * Copyright (C) 2003  Southern Storm Software, Pty Ltd.
 * Copyright (C) 2003  Free Software Foundation, Inc.
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

[TODO]
public abstract class FileDialog : CommonDialog
{

	[TODO]
	protected static readonly object EventFileOk = null;

	[TODO]
	public bool AddExtension
	{
		get
		{
			throw new NotImplementedException("AddExtension");
		}
		set
		{
			throw new NotImplementedException("AddExtension");
		}
	}

	[TODO]
	public virtual bool CheckFileExists
	{
		get
		{
			throw new NotImplementedException("CheckFileExists");
		}
		set
		{
			throw new NotImplementedException("CheckFileExists");
		}
	}

	[TODO]
	public bool CheckPathExists
	{
		get
		{
			throw new NotImplementedException("CheckPathExists");
		}
		set
		{
			throw new NotImplementedException("CheckPathExists");
		}
	}

	[TODO]
	public string DefaultExt
	{
		get
		{
			throw new NotImplementedException("DefaultExt");
		}
		set
		{
			throw new NotImplementedException("DefaultExt");
		}
	}

	[TODO]
	public bool DereferenceLinks
	{
		get
		{
			throw new NotImplementedException("DereferenceLinks");
		}
		set
		{
			throw new NotImplementedException("DereferenceLinks");
		}
	}

	[TODO]
	public string FileName
	{
		get
		{
			throw new NotImplementedException("FileName");
		}
		set
		{
			throw new NotImplementedException("FileName");
		}
	}

	[TODO]
	public string[] FileNames
	{
		get
		{
			throw new NotImplementedException("FileNames");
		}
	}

	[TODO]
	public string Filter
	{
		get
		{
			throw new NotImplementedException("Filter");
		}
		set
		{
			throw new NotImplementedException("Filter");
		}
	}

	[TODO]
	public int FilterIndex
	{
		get
		{
			throw new NotImplementedException("FilterIndex");
		}
		set
		{
			throw new NotImplementedException("FilterIndex");
		}
	}

	[TODO]
	public string InitialDirectory
	{
		get
		{
			throw new NotImplementedException("InitialDirectory");
		}
		set
		{
			throw new NotImplementedException("InitialDirectory");
		}
	}

	[TODO]
	protected virtual IntPtr Instance
	{
		get
		{
			throw new NotImplementedException("Instance");
		}
	}

	[TODO]
	protected int Options
	{
		get
		{
			throw new NotImplementedException("Options");
		}
	}

	[TODO]
	public bool RestoreDirectory
	{
		get
		{
			throw new NotImplementedException("RestoreDirectory");
		}
		set
		{
			throw new NotImplementedException("RestoreDirectory");
		}
	}

	[TODO]
	public bool ShowHelp
	{
		get
		{
			throw new NotImplementedException("ShowHelp");
		}
		set
		{
			throw new NotImplementedException("ShowHelp");
		}
	}

	[TODO]
	public string Title
	{
		get
		{
			throw new NotImplementedException("Title");
		}
		set
		{
			throw new NotImplementedException("Title");
		}
	}

	[TODO]
	public bool ValidateNames
	{
		get
		{
			throw new NotImplementedException("ValidateNames");
		}
		set
		{
			throw new NotImplementedException("ValidateNames");
		}
	}

	[TODO]
	protected override IntPtr HookProc(IntPtr hWnd, int msg, IntPtr wparam, IntPtr lparam)
	{
		throw new NotImplementedException("HookProc");
	}

	[TODO]
	protected void OnFileOk(CancelEventArgs e)
	{
		throw new NotImplementedException("OnFileOk");
	}

	[TODO]
	public override void Reset()
	{
		throw new NotImplementedException("Reset");
	}

	[TODO]
	protected override bool RunDialog(IntPtr hWndOwner)
	{
		throw new NotImplementedException("RunDialog");
	}

	[TODO]
	public override string ToString()
	{
		throw new NotImplementedException("ToString");
	}

	[TODO]
	public event CancelEventHandler FileOk
	{
		add
		{
			throw new NotImplementedException("FileOk");
		}
		remove
		{
			throw new NotImplementedException("FileOk");
		}
	}

}; // class FileDialog

}; // namespace System.Windows.Forms
