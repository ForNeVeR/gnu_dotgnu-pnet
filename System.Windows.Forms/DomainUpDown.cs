/*
 * DomainUpDown.cs - Implementation of the
 *			"System.Windows.Forms.DomainUpDown" class.
 *
 * Copyright (C) 2003 Free Software Foundation
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

using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Reflection;
using System.Windows.Forms.Themes;

namespace System.Windows.Forms
{

[TODO]
public class DomainUpDown : UpDownBase
{
	private DomainUpDownItemCollection domainItems;
	private int domainIndex;
	private bool sorted;
	private bool wrap;
	private DomainUpDown parent;

	[TODO]
	[DefaultMember("Item")]
 	public class DomainUpDownItemCollection : ArrayList	
 	{
		private DomainUpDown owner;

		internal void DomainUpDownItemCollection(DomainUpDown owner)
		{
			this.owner = owner;
		}

		[TODO]
		public override int Add(object item)
		{
			return base.Add(item);
		}

		[TODO]
		public override void Insert(int index, object item)
		{
			base.Insert(index, item);
		}

		[TODO]
		public override void Remove(object item)
		{
			base.Remove(item);
		}

		[TODO]
		public override void RemoveAt(int item)
		{
			base.RemoveAt(item);
		}

		[TODO]
		[DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
		[Browsable(false)]
		public override object this[int index]
		{
			get
			{
				return base[index];
			}
			set
			{
				base[index] = value;
			}
		}
	}; // class  DomainUpDownItemCollection

	private class DomainUpDownCompare : IComparer
	{
		public int Compare(object a, object b)
		{
			return a.ToString().CompareTo(b.ToString());
		}
	}; // class  DomainUpDownCompare
		

	public void DomainUpDown()
	{
		domainItems = new DomainUpDownItemCollection();
		domainIndex = -1;
	}

	[TODO]
	protected override AccessibleObject CreateAccessibilityInstance()
	{
		throw new NotImplementedException("CreateAccessibilityInstance");
	}

	[TODO]
	public override void DownButton()
	{
		throw new NotImplementedException("DownButton");
	}

	[TODO]
	protected override void OnChanged(object source, EventArgs e)
	{
		throw new NotImplementedException("OnChanged");
	}

	[TODO]
	protected void OnSelectesItemChanged(object source, EventArgs e)
	{
		throw new NotImplementedException("OnSelectedItemChanged");
	}
	
	[TODO]
	protected override void OnTextBoxKeyDown(object source, KeyEventArgs e)
	{
		throw new NotImplementedException("OnTextBoxKeyDown");
	}

	[TODO]
	public override string ToString()
	{
		throw new NotImplementedException("ToString");
	}

	[TODO]
	public override void UpButton()
	{
		throw new NotImplementedException("UpButton");
	}

	[TODO]
	protected override void UpdateEditText()
	{
		throw new NotImplementedException("UpdateEditText");
	}

	public DomainUpDownItemCollection Items
	{
		get
		{
			return domainItems;
		}
	}

	[TODO]
	[BrowsableAttribute(false)]
	[DefaultValue(-1)]
	public int SelectedIndex
	{
		get
		{
			return domainIndex;
		}
		set
		{
			if ((value < -1) || (value >= domainItems.Count))
				throw new ArgumentException("value", value.ToString());
			domainIndex = value;
			// FIXME Update display
		}
	}

	[TODO]
	[BrowsableAttribute(false)]
	[DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
	public object SelectedItem
	{
		get
		{
			if (domainIndex >= 0)
			{
				return domainItems[domainIndex];
			}
			return null;
		}
		set
		{
		}
	}

	[TODO]
	[DefaultValue(false)]	
	public bool Sorted
	{
		get
		{
			return sorted;
		}
		set
		{
			// sorting must be implemented
			sorted = value;
		}
	}

	[Localizable(true)]
	[DefaultValue(false)]	
	public bool Wrap
	{
		get
		{
			return wrap;
		}
		set
		{
			wrap = value;
		}
	}

	public event EventHandler SelectedItemChanged;


}; // class DomainUpDown
	
}; // namespace System.Windows.Forms

