/*
 * ListBox.cs - Implementation of the
 *			"System.Windows.Forms.ListBox" class.
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

using System.Drawing;
using System.Collections;

namespace System.Windows.Forms
{

[TODO]
public class ListBox : ListControl
{

	[TODO]
	public class ObjectCollection : IList, ICollection, IEnumerable
	{
		[TODO]
		public ObjectCollection( ListBox l )
		{
		}

		[TODO]
		public ObjectCollection( ListBox l, object[] o )
		{
		}

		[TODO]
		public ObjectCollection( ListBox l, ListBox.ObjectCollection oc )
		{
		}

		[TODO]
		public virtual int Count
		{
			get
			{
				return 0;
			}
		}

		[TODO]
		public virtual bool IsReadOnly
		{
			get
			{
				return true;
			}
		}

		[TODO]
		public virtual Object this[ int index ]
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
		public int Add( Object item )
		{
			return 0;
		}

		[TODO]
		public void AddRange( Object[] items )
		{
		}

		[TODO]
		public void AddRange( ListBox.ObjectCollection oc )
		{
		}

		[TODO]
		public virtual void Clear()
		{	
		}

		[TODO]
		public virtual bool Contains( Object value )
		{
			return false;
		}

		[TODO]
		public void CopyTo( object[] dest, int index )
		{
		}

		[TODO]
		public virtual IEnumerator GetEnumerator()
		{
			return null;
		}

		[TODO]
		public virtual int IndexOf( Object value )
		{
			return -1;
		}

		[TODO]
		public virtual void Insert( int index, Object item )
		{
		}

		[TODO]
		public virtual void Remove( Object value )
		{
		}

		[TODO]
		public virtual void RemoveAt( int index )
		{
		}


		

		[TODO]
		public void CopyTo( Array array, int index )
		{
		}

		[TODO]
		public bool IsSynchronized
		{
			get
			{
				return true;
			}
		}

		[TODO]
		public Object SyncRoot	
		{
			get
			{
				return null;
			}
		}

		[TODO]
		public bool IsFixedSize
		{
			get
			{
				return false;	
			}
		}
	}

	[TODO]
	public class SelectedIndexCollection : IList, ICollection, IEnumerable
	{
		[TODO]
		public SelectedIndexCollection( ListBox owner )
		{
		}

		[TODO]
		public virtual int Count
		{
			get
			{
				return 0;
			}
		}

		[TODO]
		public virtual bool IsReadOnly
		{
			get
			{
				return true;
			}
		}

		[TODO]
		public int this[ int index ]
		{
			get
			{
				return -1;
			}
		}

		[TODO]
		public bool Contains( int index )
		{
			return false;
		}

		[TODO]
		public virtual void CopyTo( Array dest, int index )
		{
		}

		[TODO]
		public virtual IEnumerator GetEnumerator()
		{
			return null;
		}

		[TODO]
		public int IndexOf( int selectedIndex )
		{
			return -1;
		}

		[TODO]
		public bool IsSynchronized
		{
			get
			{
				return true;
			}
		}

		[TODO]
		public Object SyncRoot	
		{
			get
			{
				return null;
			}
		}

		[TODO]
		public int Add( Object item )
		{
			return 0;
		}

		[TODO]
		public void Clear()
		{	
		}

		[TODO]
		public bool Contains( Object value )
		{
			return false;
		}

		[TODO]
		public int IndexOf( Object value )
		{
			return -1;
		}

		[TODO]
		public void Insert( int index, Object item )
		{
		}

		[TODO]
		public void Remove( Object value )
		{
		}

		[TODO]
		public void RemoveAt( int index )
		{
		}

		[TODO]
		public bool IsFixedSize
		{
			get
			{
				return false;	
			}
		}

		[TODO]	
		private Object IList.this[ int index ]
		{
			get
			{
				return null;
			}
			
			set
			{
			}
		}
	}


	[TODO]
	public class SelectedObjectCollection : IList, ICollection, IEnumerable
	{
		[TODO]	
		public SelectedObjectCollection( ListBox owner )
		{
		}

		[TODO]	
		public virtual int Count
		{
			get
			{
				return 0;
			}
		}
		
		[TODO]	
		public virtual bool IsReadOnly
		{
			get
			{
				return true;
			}
		}

		[TODO]	
		public virtual Object this[ int index ]
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
		public virtual bool Contains( Object o )
		{
			return false;
		}

		[TODO]	
		public virtual void CopyTo( Array a, int index )
		{
		}

		[TODO]	
		public virtual IEnumerator GetEnumerator()
		{
			return null;
		}

		[TODO]	
		public virtual int IndexOf( Object o )
		{
			return -1;
		}

		[TODO]
		public bool IsSynchronized
		{
			get
			{
				return true;
			}
		}

		[TODO]
		public Object SyncRoot	
		{
			get
			{
				return null;
			}
		}

		[TODO]
		public int Add( Object item )
		{
			return 0;
		}

		[TODO]
		public void Clear()
		{	
		}

		[TODO]
		public void Insert( int index, Object item )
		{
		}

		[TODO]
		public void Remove( Object value )
		{
		}

		[TODO]
		public void RemoveAt( int index )
		{
		}

		[TODO]
		public bool IsFixedSize
		{
			get
			{
				return false;	
			}
		}
	}	

	[TODO]
	public ListBox()
	{
	}

	[TODO]
	public const int DefaultItemHeight = 20; // What value? 20 is random
	public const int NoMatches = 20; // What value? 20 is random

	[TODO]
	public override Color BackColor
	{
		get
		{
			return base.BackColor;
		}

		set
		{
			base.BackColor = value;
		}
	}

	[TODO]
	public override Image BackgroundImage
	{
		get
		{	
			return base.BackgroundImage;
		}

		set
		{
			base.BackgroundImage = value;
		}
	}	

	[TODO]
	public int ColumnWidth
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
	public virtual DrawMode DrawMode
	{
		get
		{
			return DrawMode.Normal;
		}

		set
		{
		}
	}

	[TODO]
	public override Color ForeColor
	{
		get
		{
			return base.ForeColor;
		}

		set
		{
			base.ForeColor = value;
		}
	}

	[TODO]
	public int HorizontalExtent
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
	public bool HorizontalScrollbar
	{
		get
		{
			return false;
		}

		set
		{
		}
	}

	[TODO]
	public virtual int ItemHeight
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
	public ListBox.ObjectCollection Items
	{	
		get
		{
			return null;
		}
	}

	[TODO]
	public bool MultiColumn
	{
		get
		{
			return false;
		}

		set
		{
		}
	}

	[TODO]
	public int PreferredHeight
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
	public override RightToLeft RightToLeft
	{
		get
		{
			return base.RightToLeft;
		}
		set
		{
			base.RightToLeft = value;
		}
	}

	[TODO]
	public bool ScrollAlwaysVisible
	{
		get
		{
			return true;
		}
		set
		{
		}
	}

	[TODO]
	public override int SelectedIndex
	{
		get
		{
			return base.SelectedIndex;
		}
		set
		{
			base.SelectedIndex = value;
		}
	}

	[TODO]
	public ListBox.SelectedIndexCollection SelectedIndices
	{
		get
		{
			return null;
		}
	}

	[TODO]
	public Object SelectedItem
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
	public ListBox.SelectedObjectCollection SelectedItems
	{
		get
		{
			return null;
		}
	}	

	[TODO]
	public virtual SelectionMode SelectionMode
	{
		get
		{
			return SelectionMode.One;
		}

		set
		{
		}
	}

	[TODO]
	public bool Sorted
	{
		get
		{
			return false;
		}
		
		set
		{
		}
	}

	[TODO]
	public override String Text
	{
		get
		{	
			return base.Text;
		}

		set
		{
			base.Text = value;
		}
	}

	[TODO]
	public int TopIndex
	{
		get
		{
			return -1;
		}

		set
		{
		}
	}

	[TODO]
	public bool UseTabStops
	{
		get
		{
			return false;
		}

		set
		{
		}
	}

	[TODO]
	public void BeginUpdate()
	{
	}
	
	[TODO]
	public void ClearSelected()
	{
	}

	[TODO]
	public void EndUpdate()
	{
	}

	[TODO]
	public int FindString( String s )
	{
		return -1;
	}

	[TODO]
	public int FindString( String s, int index )
	{
		return -1;
	}

	[TODO]
	public int FindStringExact( String s )
	{
		return -1;
	}

	[TODO]
	public int FindStringExact( String s, int index )
	{
		return -1;
	}

	[TODO]
	public int GetItemHeight()
	{
		return 0;
	}

	[TODO]
	public Rectangle GetItemRectangle( int index )
	{
		return new Rectangle();
	}

	[TODO]
	public bool GetSelected( int index )
	{
		return false;
	}

	[TODO]
	public int IndexFromPoint( Point p )
	{
		return -1;
	}

	[TODO]
	public int IndexFromPoint( int x, int y )
	{
		return -1;
	}

	[TODO]
	public override void Refresh()
	{
		base.Refresh();
	}

	[TODO]
	public void SetSelected( int index, bool value )
	{
	}

	[TODO]
	public override String ToString()
	{
		return base.ToString();
	}

	[TODO]
	public event DrawItemEventHandler DrawItem;

	[TODO]
	public event MeasureItemEventHandler MeasureItem;

	[TODO]
	public event EventHandler SelectedIndexChanged;

	[TODO]
	protected override CreateParams CreateParams
	{
		get
		{
			return base.CreateParams;
		}
	}

	[TODO]
	protected override Size DefaultSize
	{
		get
		{
			return base.DefaultSize;
		}
	}

	[TODO]
	protected virtual ObjectCollection CreateItemCollection()
	{
		return null;
	}

	[TODO]
	protected override void OnChangeUICues( UICuesEventArgs e )
	{
		base.OnChangeUICues( e );
	}

	[TODO]
	protected override void OnDataSourceChanged( EventArgs e )
	{
		base.OnDataSourceChanged( e );
	}

	[TODO]
	protected override void OnDisplayMemberChanged( EventArgs e )
	{
		base.OnDisplayMemberChanged( e );
	}

	[TODO]
	protected virtual void OnDrawItem( DrawItemEventArgs e )
	{
	}

	[TODO]
	protected override void OnFontChanged( EventArgs e )
	{
		base.OnFontChanged( e );
	}

	[TODO]
	protected override void OnHandleCreated( EventArgs e )
	{
		base.OnHandleCreated( e );
	}

	[TODO]
	protected override void OnHandleDestroyed( EventArgs e )
	{
		OnHandleDestroyed( e );
	}

	[TODO]
	protected virtual void OnMeasureItem( MeasureItemEventArgs e )
	{
	}
	
	[TODO]
	protected override void OnParentChanged( EventArgs e )
	{
		base.OnParentChanged( e );
	}

	[TODO]
	protected override void OnResize( EventArgs e )
	{
		base.OnResize( e );
	}
	
	[TODO]
	protected override void OnSelectedIndexChanged( EventArgs e )
	{	
		base.OnSelectedIndexChanged( e );
	}

	[TODO]
	protected override void OnSelectedValueChanged( EventArgs e )
	{
		base.OnSelectedValueChanged( e );
	}

	[TODO]
	protected override void RefreshItem( int index )
	{
		base.RefreshItem( index );
	}

	[TODO]
	protected override void SetBoundsCore( 
		int x, int y, int width, int height, BoundsSpecified specified )
	{
		base.SetBoundsCore( x, y, width, height, specified );
	}

	[TODO]	
	protected virtual void Sort()
	{
	}

#if !CONFIG_COMPACT_FORMS

	protected override void WndProc(ref Message m)
	{
		base.WndProc(ref m);
	}

#endif // !CONFIG_COMPACT_FORMS
	
}; // class ListBox
	
}; // namespace System.Windows.Forms

