/*
 * BindingsCollection.cs - Implementation of the
 *			"System.Windows.Forms.BindingsCollection" class.
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
public class BindingsCollection : BaseCollection
{

	[TODO]
	public override int Count
	{
		get 
		{
			return base.Count;
		}
		
	}

	[TODO]
	public Binding this[ int index ]
	{
		get
		{
			return null;
		}
	}

#if CONFIG_COMPONENT_MODEL
	public event CollectionChangeEventHandler CollectionChanged;
#endif

	[TODO]
	protected override ArrayList List
	{
		get 
		{
			return null;
		}
	}

	[TODO]
	protected internal void Add( Binding b )
	{
	}

	[TODO]
	protected internal void Clear()
	{
	}

#if CONFIG_COMPONENT_MODEL
	[TODO]
	protected virtual void OnCollectionChanged( CollectionChangeEventArgs ev )
	{
	}
#endif

	[TODO]
	protected internal void Remove( Binding b )
	{
	}

	[TODO]
	protected internal void RemoveAt( int index )
	{
	}

	[TODO]
	protected internal bool ShouldSerializeMyAll()
	{
		return false;
	}
	
}; // class BindingsCollection
	
}; // namespace System.Windows.Forms
