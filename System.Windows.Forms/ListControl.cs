/*
 * ListControl.cs - Implementation of the
 *			"System.Windows.Forms.ListControl" class.
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
using System.Reflection;

namespace System.Windows.Forms
{

[TODO]
public abstract class ListControl : Control
{
	Object data_source = null;
	String display_member = "";
	String value_member = "";

	[TODO]
	public Object DataSource
	{
		get
		{
			return data_source;
		}
		set
		{
			data_source = value;
		}
	}

	[TODO]
	public String DisplayMember
	{
		get
		{
			return display_member;
		}
		set
		{
			display_member = value;
		}
	}

	public abstract int SelectedIndex { get; set; }  

	[TODO]
	public Object SelectedValue
	{
		get
		{
			return null;
		}
		
		set 
		{
		}
	}

	[TODO]
	public String ValueMember
	{
		get
		{
			return value_member;
		}
		set
		{
			value_member = value;
		}
	}

	public String GetItemText( Object i )
	{
		if ( i == null )
		{
			return null;
		}
		
		if ( display_member == null || display_member == "" )
		{
			return i.ToString();
		}
		
		PropertyInfo pi;
		try 
		{
			pi = i.GetType().GetProperty( display_member );
		}
		catch ( AmbiguousMatchException ) 
		{
			return null;
		}
		
		if ( pi == null )
		{
			return null;
		}
		
		Object v = pi.GetValue( i, null );
		
		if ( v == null )
		{
			return null;
		}
		
		return v.ToString();
	}

	public event EventHandler DataSourceChanged;
	public event EventHandler DisplayMemberChanged;
	public event EventHandler SelectedValueChanged;
	public event EventHandler ValueMemberChanged;


	[TODO]
	protected ListControl()
	{
	}
	
	[TODO]
	protected CurrencyManager DataManager
	{
		get
			{
				return null;
			}
	}

	[TODO]
	protected override bool IsInputKey( Keys k )
	{
		return false;
	}

	[TODO]
	protected virtual void OnDataSourceChanged( EventArgs ev )
	{
	}

	[TODO]
	protected virtual void OnDisplayMemberChanged( EventArgs ev )
	{
	}

	[TODO]
	protected virtual void OnSelectedIndexChanged( EventArgs ev )
	{
	}

	[TODO]
	protected virtual void OnSelectedValueChanged( EventArgs ev )
	{
	}

	[TODO]
	protected virtual void OnValueMemberChanged( EventArgs ev )
	{
	}
	
	protected abstract void RefreshItem( int index );
	
}; // class ListControl
	
}; // namespace System.Windows.Forms
