/*
 * CurrencyManager.cs - Implementation of the
 *			"System.Windows.Forms.CurrencyManager" class.
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
public class CurrencyManager: BindingManagerBase
{

	[TODO]
	public override int Count
	{
		get
		{
			return 0;
		}
	}

	[TODO]
	public override Object Current
	{
		get
		{
			return null;
		}
	}

	[TODO]
	public IList List
	{
		get 
		{
			return null;
		}
	}

	[TODO]
	public override int Position
	{
		get 
		{
			return 0;
		}
		
		set
		{
				
		}
	}

	[TODO]
	public override void AddNew()
	{
	}

	[TODO]
	public override void CancelCurrentEdit()
	{
	}

	[TODO]
	public override void EndCurrentEdit()
	{
	}
	
#if CONFIG_COMPONENT_MODEL
	[TODO]	
	public override PropertyDescriptorCollection GetItemProperties()
	{
		return null;
	}
#endif

	[TODO]	

	public void Refresh()
	{
	}

	[TODO]	
	public override void RemoveAt( int index )
	{
   
	}

	[TODO]
	public override void ResumeBinding()
	{
	}

	[TODO]
	public override void SuspendBinding()
	{
	}
	
	public event ItemChangedEventHandler ItemChanged;
	public event EventHandler MetaDataChanged;

	[TODO]
	protected void CheckEmpty()
	{
	}
	
	[TODO]
	protected internal override String GetListName( ArrayList listAccessors )
	{
		return "";
	}
	
	[TODO]
	protected internal override void OnCurrentChanged( EventArgs ev )
	{
	}
	
	[TODO]	
	protected virtual void OnItemChanged( ItemChangedEventArgs ev )
	{
	}

	[TODO]	
	protected override void UpdateIsBinding()
	{
	}
}; // class CurrencyManager
	
}; // namespace System.Windows.Forms
