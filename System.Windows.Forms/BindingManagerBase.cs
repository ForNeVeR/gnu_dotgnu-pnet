/*
 * BindingManagerBase.cs - Implementation of the
 *			"System.Windows.Forms.BindingManagerBase" class.
 *
 * Copyright (C) 2003 Free Software Foundation
 *
 * Contributions from Cecilio Pardo <cpardo@imayhem.com>
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
using System.ComponentModel;
using System.Collections;

namespace System.Windows.Forms
{

[TODO]
public abstract class BindingManagerBase
{
	[TODO]
	public BindingManagerBase()
	{
	}

	[TODO]
	public BindingsCollection Bindings
	{
		get
		{
			return null;
		}
		
	}

	public abstract int Count 
	{
		get;
	}

	public abstract Object Current
	{
		get;
	}

	public abstract int Position 
	{
		get;
		set;
	}

	public abstract void AddNew();
	public abstract void CancelCurrentEdit();
	public abstract void EndCurrentEdit();
	public abstract PropertyDescriptorCollection GetItemProperties();

	[TODO]
	protected internal virtual 
	PropertyDescriptorCollection GetItemProperties( ArrayList dataSources, 
													ArrayList listAccessors )
	{
		return null;
	}

	[TODO]
	protected virtual PropertyDescriptorCollection GetItemProperties( 
													Type listType, 
													 int offset, 
													 ArrayList dataSources, 
													ArrayList listAccessors )
	{
		return null;
	}
	
	public abstract void RemoveAt( int index );
	
	public abstract void ResumeBinding();
	
	public abstract void SuspendBinding();


	public event EventHandler CurrentChanged;
	
	protected EventHandler onCurrentChangedHandler;
	
	public event EventHandler PositionChanged;
	protected EventHandler onPositionChangedHandler;


	protected internal abstract string GetListName( ArrayList listAccessors );
	protected internal abstract void OnCurrentChanged( EventArgs ev );

	[TODO]
	protected void PullData()
	{
	}
	
	[TODO]
	protected void PushData()
	{
	}

	protected abstract void UpdateIsBinding();
	
}; // class BindingManagerBase
	
}; // namespace System.Windows.Forms
