/*
 * CommonDialog.cs - Implementation of the "System.Windows.Forms.CommonDialog" class.
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
public abstract class CommonDialog
#if CONFIG_COMPONENT_MODEL
	: Component
#endif
{
	[TODO]
	public CommonDialog()
	{
		throw new NotImplementedException(".ctor");
	}

	[TODO]
	protected virtual IntPtr HookProc(IntPtr hWnd, int msg, IntPtr wparam, IntPtr lparam)
	{
		throw new NotImplementedException("HookProc");
	}

	[TODO]
	protected virtual void OnHelpRequest(EventArgs e)
	{
		throw new NotImplementedException("OnHelpRequest");
	}

	[TODO]
	protected virtual IntPtr OwnerWndProc(IntPtr hWnd, int msg, IntPtr wparam, IntPtr lparam)
	{
		throw new NotImplementedException("OwnerWndProc");
	}

	public abstract void Reset();
	protected abstract bool RunDialog(IntPtr hwndOwner);

	[TODO]
	public DialogResult ShowDialog()
	{
		throw new NotImplementedException("ShowDialog");
	}

	[TODO]
	public DialogResult ShowDialog(IWin32Window owner)
	{
		throw new NotImplementedException("ShowDialog");
	}

	[TODO]
	public event EventHandler HelpRequest
	{
		add
		{
			throw new NotImplementedException("HelpRequest");
		}
		remove
		{
			throw new NotImplementedException("HelpRequest");
		}
	}

}; // class CommonDialog

}; // namespace System.Windows.Forms
