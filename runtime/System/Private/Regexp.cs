/*
 * Regexp.cs - A utility regexp class
 *
 * Copyright (C) 2001  Southern Storm Software, Pty Ltd.
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

using System;
using Platform;
using System.Runtime.CompilerServices;

namespace System.Private
{
internal sealed class Regexp : IDisposable
{
	private IntPtr compiled;
	private bool disposed=false;

	internal Regexp(String pattern)
	{
		compiled=RegexpMethods.CompileInternal(pattern,0);
		if(compiled==IntPtr.Zero)
		{
			throw new ArgumentException("pattern");
		}
	}
	internal bool Match(String input)
	{
		CheckDisposed();
		int result=RegexpMethods.ExecInternal(compiled,input,0);
		return (result==0);
	}
	private void CheckDisposed()
	{
		if(disposed)throw new ObjectDisposedException("Regexp");
	}
	internal void Dispose()
	{
		CheckDisposed();
		if(!disposed)RegexpMethods.FreeInternal(this.compiled);
		disposed=true;
		compiled=IntPtr.Zero;
	}
	void IDisposable.Dispose()
	{
		Dispose();
	}
}; // class Regexp
}; // namespace System.Private
