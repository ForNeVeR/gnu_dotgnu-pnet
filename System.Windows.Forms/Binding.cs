/*
 * Binding.cs - Implementation of the
 *			"System.Windows.Forms.Binding" class.
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

namespace System.Windows.Forms
{

[TODO]
public class Binding
{
	private BindingMemberInfo bindingMemberInfo;
	
	
	[TODO]
	public Binding( String propertyName, Object dataSource, String dataMember ) 
	{
		bindingMemberInfo = new BindingMemberInfo( dataMember );
	}

	[TODO]
	public BindingManagerBase BindingManagerBase
	{
		get
		{
			return null;
		}
	}

	[TODO]
	public BindingMemberInfo BindingMemberInfo
	{
		get
		{
			return bindingMemberInfo;
		}
	}

	[TODO]
	public Control Control
	{
		get 
		{
			return null;
		}
	}

	[TODO]
	public Object DataSource
	{
		get
		{
			return null;
		}
	}

	[TODO]
	public bool IsBinding
	{
		get
		{
			return false;
		}
	}

	[TODO]
	public string PropertyName
	{
		get 
		{
			return null;
		}
	}

	[TODO]
	public event ConvertEventHandler Format;

	[TODO]
	public event ConvertEventHandler Parse;


	[TODO]
	protected virtual void OnFormat( ConvertEventArgs cev )
	{
	}

	[TODO]
	protected virtual void OnParse( ConvertEventArgs cev )
	{
	}
	
}; // class Binding
	
}; // namespace System.Windows.Forms
