/*
 * ListBox.cs - Implementation of the
 *			"System.Windows.Forms.ListBox" class.
 *
 * Copyright (C) 2003 Free Software Foundation
 *
 * Contributions from Brian Luft <brian@electroly.com>,
 *                    Cecilio Pardo <cpardo@imayhem.com>
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
using System.Windows.Forms.Themes;
using System.Diagnostics;

namespace System.Windows.Forms
{

[TODO]
public class ListBox : ListControl
{
 	public class ObjectCollection : IList, ICollection, IEnumerable	
 	{
 		private ListBox owner;
 		private ArrayList list;
		internal int maxWidth = 0;

 		public ObjectCollection(ListBox owner)
 		{
 			this.owner = owner;
 			list = new ArrayList();
 		}

 		// Implement the ICollection interface.
 		public void CopyTo(Array array, int index)
 		{
 			List.CopyTo(array, index);
 		}

 		public virtual int Count
  		{
 			get { return List.Count; }
 		}
 	
 		bool ICollection.IsSynchronized
 		{
 			get { return false; }
  		}

 		Object ICollection.SyncRoot
  		{
 			get { return this; }
  		}

 		// Implement the IEnumerable interface.
 		public IEnumerator GetEnumerator()
  		{
 			return List.GetEnumerator();
 		}

 		// Determine if the collection is read-only.
 		public bool IsReadOnly
  		{
 			get { return false; }
  		}

 		bool IList.IsFixedSize
  		{
 			get { return false; }
  		}

		int TextWidth(object value)
		{
			return((int) this.owner.nonClientGraphics.MeasureString(value.ToString(), owner.Font).Width);
		}

 		// Get the array list that underlies this collection
 		Object IList.this[int index]
  		{
 			get { return List[index]; }
 			set 
			{ 
				List[index] = value; 
				if(this.owner != null)
				{
					this.maxWidth = Math.Max(this.maxWidth, TextWidth(value));
					this.owner.CalculateScrollbars();
				}
			}
  		}

 		Object this[int index]
  		{
 			get { return List[index]; }
 			set 
			{ 
				List[index] = value; 
				if(this.owner != null)
				{
					this.maxWidth = Math.Max(this.maxWidth, TextWidth(value));
					this.owner.CalculateScrollbars();
				}
			}
  		}

 		protected virtual ArrayList List
 		{
 			get { return list; }
 		}
 	
 		public int Add(Object value)
 		{
 			int result;
 			result = List.Add(value);
 			if(this.owner != null)
			{
				this.owner.PaintItem(result);
				this.maxWidth = Math.Max(this.maxWidth, TextWidth(value));
				this.owner.CalculateScrollbars();
			}
 			return result;
 		}

 		public virtual void AddRange(Object[] items)
 		{
 			List.AddRange(items);
			
			if(this.owner != null)
			{
				foreach(object value in items)
					this.maxWidth = Math.Max(this.maxWidth, TextWidth(value));
	 			this.owner.Invalidate();
				this.owner.CalculateScrollbars();
			}
 		}

 		public virtual void Clear()
 		{
 			List.Clear();
			if(this.owner != null)
			{
	 			this.owner.Invalidate();
				this.owner.CalculateScrollbars();
			}
 		}

 		public bool Contains(Object value)
 		{
 			return List.Contains(value);
 		}

 		public int IndexOf(Object value)
 		{
 			return List.IndexOf(value);
 		}

 		public void Insert(int index, Object value)
 		{
 			List.Insert(index, value);
			if(this.owner != null)
			{
				this.maxWidth = Math.Max(this.maxWidth, TextWidth(value));	
	 			this.owner.Invalidate();
				this.owner.CalculateScrollbars();
			}
 		}

 		public void Remove(Object value)
 		{
 			List.Remove(value);
			if(this.owner != null)
			{
	 			this.owner.Invalidate();
				this.owner.CalculateScrollbars();
			}
 		}

 		public void RemoveAt(int index)
 		{
 			List.RemoveAt(index);
			if(this.owner != null)
			{
				this.owner.Invalidate();
				this.owner.CalculateScrollbars();
			}
 		}

	}; // class ListBox.ObjectCollection

 	public class SelectedIndexCollection : IList, ICollection, IEnumerable
 	{
 		private ListBox owner;
 		private ArrayList list;

 		public SelectedIndexCollection(ListBox owner)
 		{
 			this.owner = owner;
 			list = new ArrayList();
 		}

 		// Implement the ICollection interface.
 		public void CopyTo(Array array, int index)
 		{
 			List.CopyTo(array, index);
 		}
		
		// Make a copy of this collection into copy.
		public void Copy(SelectedIndexCollection copy)
		{
			copy.Clear();
			foreach(object o in list)
				copy.Add(o);
		}

 		public virtual int Count
  		{
 			get { return List.Count; }
 		}
 	
 		bool ICollection.IsSynchronized
 		{
 			get { return false; }
  		}

 		Object ICollection.SyncRoot
  		{
 			get { return this; }
  		}

 		// Implement the IEnumerable interface.
 		public IEnumerator GetEnumerator()
  		{
 			return List.GetEnumerator();
 		}

 		// Determine if the collection is read-only.
 		public bool IsReadOnly
  		{
 			get { return false; }
  		}

 		bool IList.IsFixedSize
  		{
 			get { return false; }
  		}

 		// Get the array list that underlies this collection
 		Object IList.this[int index]
  		{
 			get { return List[index]; }
 			set 
			{ 
				List[index] = value; 
				
				if(this.owner != null && !this.owner.suppressEvents)
					this.owner.FireSelectedIndexChanged();
			}
  		}

 		public int this[int index]
  		{
 			get { return (int) List[index]; }
 			set 
			{ 
				List[index] = value; 

				if(this.owner != null && !this.owner.suppressEvents)
					this.owner.FireSelectedIndexChanged();
			}
  		}

 		protected virtual ArrayList List
 		{
 			get { return list; }
 		}
 	
 		public int Add(int value)
 		{
 			int result;
 			
			result = List.Add(value);
 			if(this.owner != null)
			{
				this.owner.PaintItem(value);
				if(!this.owner.suppressEvents)
					this.owner.FireSelectedIndexChanged();
			}
			
 			return result;
 		}
		
		public int Add(Object value)
		{
			if(!(value is int))
				throw new ArgumentOutOfRangeException(/* TODO */);
			
			return this.Add((int) value);
		}
		
 		public virtual void AddRange(Object[] values)
 		{
 			List.AddRange(values);

 			if(this.owner != null)
			{
				foreach(object o in values)
					this.owner.PaintItem((int) o);
			
				if(!this.owner.suppressEvents)
					this.owner.FireSelectedIndexChanged();
			}
 		}

 		public virtual void Clear()
 		{
 			List.Clear();

			if(this.owner != null)
			{
 				this.owner.RedrawList();

				if(!this.owner.suppressEvents)
					this.owner.FireSelectedIndexChanged();
			}
 		}

 		public bool Contains(Object value)
 		{
 			return List.Contains(value);
 		}

 		public int IndexOf(Object value)
 		{
 			return List.IndexOf(value);
 		}

 		public void Insert(int index, object value)
 		{
 			List.Insert(index, value);
			if(this.owner != null)
			{
 				this.owner.PaintItem((int) value);

				if(!this.owner.suppressEvents)
					this.owner.FireSelectedIndexChanged();
			}
 		}
		
 		public void Remove(object value)
 		{
 			List.Remove(value);
			if(this.owner != null)
			{
 				this.owner.PaintItem((int) value);

				if(!this.owner.suppressEvents)
					this.owner.FireSelectedIndexChanged();
			}
 		}

 		public void RemoveAt(int index)
 		{
			int value = (int) List[index];
 			List.RemoveAt(index);
			if(this.owner != null)
			{
				this.owner.PaintItem(value);

				if(!this.owner.suppressEvents)
					this.owner.FireSelectedIndexChanged();
			}
 		}

	}; // class ListBox.SelectedIndexCollection

	public class SelectedObjectCollection : IList, ICollection, IEnumerable
	{
		private ListBox owner;
		private SelectedIndexCollection indices;

		public SelectedObjectCollection(ListBox owner)
		{
			this.owner = owner;
			this.indices = owner.selectedIndices;
		}
		
		public virtual int Count
		{
			get
			{
				return indices.Count;
			}
		}
		
		public virtual bool IsReadOnly
		{
			get
			{
				return true;
			}
		}

		public virtual object this[int index]
		{
			get
			{
				IList data = (IList) owner.dataSource;
				int dataInd = (int) indices[index];
				return data[dataInd];
			}

			set
			{
				throw new NotSupportedException(/* TODO */);
			}
		}
	
		public virtual bool Contains(object o)
		{
			IList data = (IList) owner.dataSource;
			int dataInd = data.IndexOf(o);
			if(dataInd == -1)
				return false;
			else
				return indices.Contains(dataInd);
		}

		public virtual void CopyTo(Array dest, int index)
		{
			for(int i = 0; i < this.Count; i++)
				dest.SetValue(this[i],index + i);	
		}

		private class SelectedObjectEnumerator : IEnumerator
		{
			private SelectedObjectCollection owner;
			int ind = -1;
			
			public SelectedObjectEnumerator(SelectedObjectCollection owner)
			{
				this.owner = owner;
			}
			
			public void Reset()
			{
				ind = -1;
			}
			
			public object Current
			{
				get
				{
					if(ind < 0 || ind >= owner.Count)
						throw new InvalidOperationException(/* TODO */);
					else
						return owner[ind];
				}
			}
			
			public bool MoveNext()
			{
				ind++;
				return ind < owner.Count;
			}
		}

		public virtual IEnumerator GetEnumerator()
		{
			return new SelectedObjectEnumerator(this);
		}

		public virtual int IndexOf(object o)
		{
			IList data = (IList) owner.dataSource;
			int dataInd = data.IndexOf(o);
			if(dataInd == -1)
				return -1;
			else
				return indices.IndexOf(dataInd);
		}

		public bool IsSynchronized
		{
			get
			{
				return true;
			}
		}

		public object SyncRoot	
		{
			get
			{
				return this.indices;
			}
		}

		//TODO: Microsoft's impl somehow doesn't have these methods -- how?
		public int Add(object o)
		{
			throw new ArgumentException(/* TODO */);
		}

		public void Clear()
		{	
			throw new ArgumentException(/* TODO */);
		}

		public void Insert( int index, Object item )
		{
			throw new ArgumentException(/* TODO */);
		}

		public void Remove( Object value )
		{
			throw new ArgumentException(/* TODO */);
		}

		public void RemoveAt( int index )
		{
			throw new ArgumentException(/* TODO */);
		}

		public bool IsFixedSize
		{
			get
			{
				return true;	
			}
		}
	} // class SelectedObjectCollection;

	[TODO]
	public const int DefaultItemHeight = 20; // What value? 20 is random

	public const int NoMatches = -1;

	public override Color BackColor
	{
		get
		{
			return base.BackColor;
		}
		set
		{
			if(this.backgroundBrush != null)
				this.backgroundBrush.Dispose();
			this.backgroundBrush = new SolidBrush(value);
			base.BackColor = value;
			this.vertScrollbar.BackColor = SystemColors.Control;
			this.horizScrollbar.BackColor = SystemColors.Control;
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

	public bool HorizontalScrollbar
	{
		get
		{
			return this.horizontalScrollbar;
		}
		set
		{
			this.horizontalScrollbar = value;
			this.CalculateScrollbars();
		}
	}

	[TODO]
	public virtual int ItemHeight
	{
		get
		{
			return 15;
		}
		set
		{	
		}
	}

	public ListBox.ObjectCollection Items
	{	
		get
		{
			return this.internalDataSource;
		}
	}

	public bool MultiColumn
	{
		get
		{
			return this.multiColumn;
		}

		set
		{
			this.multiColumn = value;
			this.Redraw();
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

	public bool ScrollAlwaysVisible
	{
		get
		{
			return this.scrollAlwaysVisible;
		}
		set
		{
			this.scrollAlwaysVisible = value;
			this.CalculateScrollbars();
		}
	}

	public override int SelectedIndex
	{
		get
		{
			if(this.selectedIndices.Count == 0)
				return -1;
			else
				return (int) this.selectedIndices[0];	
		}
		set
		{
			if(this.selectionMode == SelectionMode.None)
				throw new ArgumentException(/* TODO */);

			IList data = (IList) base.dataSource;
			
			if(value < 0 || value >= data.Count)
				throw new ArgumentOutOfRangeException(/* TODO */);
		
			if(this.selectionMode == SelectionMode.One)
				this.selectedIndices.Clear();
			this.selectedIndices.Add(value);
		}
	}

	public ListBox.SelectedIndexCollection SelectedIndices
	{
		get
		{
			return this.selectedIndices;
		}
	}

	public Object SelectedItem
	{
		get
		{
			int selectedIndex = this.SelectedIndex;
		
			if(selectedIndex == -1)
				return null;
			else
			{
				IList data = (IList) base.dataSource;
				return data[selectedIndex];
			}
		}

		set
		{
			if(this.selectionMode == SelectionMode.None)
				throw new ArgumentException(/* TODO */);
		
			IList data = (IList) base.dataSource;
			
			if(data.Contains(value))
			{
				if(this.selectionMode == SelectionMode.One)
					this.selectedIndices.Clear();
				this.selectedIndices.Add(data.IndexOf(value));
			}
			else
			{
				// Microsoft silently ignores when you pass in an item
				// that doesn't exist in the list.
			}
		}
	}

	public ListBox.SelectedObjectCollection SelectedItems
	{
		get
		{
			return this.selectedItems;
		}
	}	

	public virtual SelectionMode SelectionMode
	{
		get
		{
			return this.selectionMode;
		}

		set
		{
			//TODO: validate the current selections
			this.selectionMode = value;
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
	public bool IntegralHeight
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

	public int IndexFromPoint(Point p)
	{
		return this.IndexFromPoint(p.X, p.Y);
	}

	public int IndexFromPoint(int x, int y)
	{
		int screenInd = this.HitTest(x, y);
		
		if(screenInd == -1)
			return ListBox.NoMatches;
		else
		{
			IList data = (IList) base.dataSource;

			screenInd += this.topIndex;
			
			if(screenInd < data.Count)
				return screenInd;
			else
				return ListBox.NoMatches;
		}
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

	protected virtual ObjectCollection CreateItemCollection()
	{
		return new ObjectCollection(this);
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
		base.OnHandleDestroyed( e );
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
	
	internal void FireSelectedIndexChanged()
	{
		this.OnSelectedIndexChanged(new EventArgs());
	}
	
	protected override void OnSelectedIndexChanged(EventArgs e)
	{	
		if(this.SelectedIndexChanged != null)
			this.SelectedIndexChanged(this, e);
	
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

	// Internal instance variables.
	private Graphics nonClientGraphics;
	private VScrollBar vertScrollbar;
	private HScrollBar horizScrollbar;
	private ObjectCollection internalDataSource;
	private SolidBrush backgroundBrush;
	private SolidBrush disabledBrush;
	private int focusedItem = 0;
	private Timer vScrollTimer;
	internal bool suppressEvents = false;
	private bool suppressDraw = false;
	
	// Used for control-dragging in MultiExtended mode.
	private SelectedIndexCollection prevSelectedIndices;
	private int dragBeginInd = 0;

	// Instance variables for properties.
	private bool scrollAlwaysVisible = false;
	private SelectionMode selectionMode = SelectionMode.One;
	private int topIndex = 0;
	internal SelectedIndexCollection selectedIndices;
	private SelectedObjectCollection selectedItems;
	private bool horizontalScrollbar = false;
	private bool multiColumn = false;

	// Constructor
	public ListBox()
	{
		this.vertScrollbar = new VScrollBar();
		this.vertScrollbar.Dock = DockStyle.None;
		this.Controls.Add(this.vertScrollbar);
		this.vertScrollbar.Anchor = AnchorStyles.Top |
		                            AnchorStyles.Bottom |
		                            AnchorStyles.Right;
		
		this.horizScrollbar = new HScrollBar();
		this.horizScrollbar.Dock = DockStyle.None;
		this.Controls.Add(this.horizScrollbar);
		this.horizScrollbar.Anchor = AnchorStyles.Bottom |
		                             AnchorStyles.Left |
		                             AnchorStyles.Right;

		this.BackColor          = SystemColors.Window;
		this.nonClientGraphics  = CreateNonClientGraphics();
		this.internalDataSource = this.CreateItemCollection();
		base.dataSource         = this.internalDataSource;

		this.backgroundBrush     = new SolidBrush(SystemColors.Window);
		this.disabledBrush       = new SolidBrush(SystemColors.Control);
		this.selectedIndices     = new SelectedIndexCollection(this);
		this.prevSelectedIndices = new SelectedIndexCollection(null);
		this.selectedItems       = new SelectedObjectCollection(this);

		this.PositionControls();
		this.CalculateScrollbars();
		
		this.vScrollTimer = new Timer();
		this.vScrollTimer.Tick += new EventHandler(this.VScrollTick);
		this.vScrollTimer.Interval = 1;
		this.vScrollTimer.Stop();

		this.vertScrollbar.Scroll += new ScrollEventHandler(this.OnVScroll);
		this.vertScrollbar.ValueChanged += new EventHandler(this.OnVValueChanged);
		this.horizScrollbar.Scroll += new ScrollEventHandler(this.OnHScroll);
		this.horizScrollbar.ValueChanged += new EventHandler(this.OnHValueChanged);
	}
	
	private void PositionControls()
	{
		this.vertScrollbar.Top = this.BorderWidth;
		this.vertScrollbar.Left = this.Width - 
		                          this.vertScrollbar.Width - 
								  this.BorderWidth;

		this.horizScrollbar.Top = this.Height - 
		                          this.horizScrollbar.Height - 
		                          this.BorderWidth;
		this.horizScrollbar.Left = this.BorderWidth;
	}

	private int FocusedItem
	{
		get
		{
			return this.focusedItem;
		}
		set
		{
			int old = this.focusedItem;
			this.focusedItem = value;
			
			this.PaintItem(old);
			this.PaintItem(value);
		}
	}

	//TODO: Still some issues with interactions between the
	// scrollbars: if the horizontal scrollbar is shown that
	// takes effect on whether the vertical scrollbar is shown,
	// and vice versa.  Need to figure out how to handle this.
	private bool IsHorizScrollbarNeeded
	{
		get
		{
			return this.LongestItemWidth > this.ListArea.Width;
		}
	}
	
	private bool IsVertScrollbarNeeded
	{
		get
		{
			IList data = (IList) base.dataSource;
			return data.Count > this.NumItemsVisible;
		}
	}

	// Determine  the necessity of having the scrollbars,  show/hide
	// them, and enable/disable them if necessary, based on the list
	// items  and  the  various  client-settable  properties.   Also 
	// repositions the scrollbars when necessary.
	private void CalculateScrollbars()
	{
		bool oldVertVisible = this.vertScrollbar.Visible;
		bool oldHorizVisible = this.horizScrollbar.Visible;
	
		if(this.scrollAlwaysVisible)
		{
			this.vertScrollbar.Visible = true;
			this.vertScrollbar.Enabled = this.IsVertScrollbarNeeded;
			
			if(this.horizontalScrollbar)
			{			
				this.horizScrollbar.Visible = true;
				this.horizScrollbar.Enabled = this.IsHorizScrollbarNeeded;
			}
			else
				this.horizScrollbar.Visible = false;
		}
		else
		{
			this.vertScrollbar.Visible = this.IsVertScrollbarNeeded;
			this.vertScrollbar.Enabled = true;
			
			if(this.horizontalScrollbar)
			{
				this.horizScrollbar.Visible = this.IsHorizScrollbarNeeded;
				this.horizScrollbar.Enabled = true;
			}
			else
				this.horizScrollbar.Visible = false;
		}
		
		if(oldVertVisible != this.vertScrollbar.Visible ||
		   oldHorizVisible != this.horizScrollbar.Visible)
		{
			this.PositionControls();
			this.SizeScrollbars();
		}
	}

	// Size the scrollbars so they fit perfectly inside the listbox
	// and with each other.
	private void SizeScrollbars()
	{
		int vertHeight = this.Height - 2 * this.BorderWidth,
		    horizWidth = this.Width -  2 * this.BorderWidth;
	
		if(this.horizScrollbar.Visible && this.vertScrollbar.Visible)
		{
			vertHeight -= this.horizScrollbar.Height;
			horizWidth -= this.vertScrollbar.Width;
		}
		
		this.vertScrollbar.Height = vertHeight;
		this.horizScrollbar.Width = horizWidth;
	}
	
	private int BorderWidth
	{
		get
		{
			return(ThemeManager.MainPainter.GetBorderWidth(this.BorderStyle));	
		}
	}

	// Handle "Paint" events for the list box.
	protected override void OnPaint(PaintEventArgs e)
	{
		Redraw(this.nonClientGraphics);
	}

	private void Redraw()
	{
		Redraw(this.nonClientGraphics);
	}

	// Redraw the entire ListBox.
	private void Redraw(Graphics g)
	{
		ThemeManager.MainPainter.DrawListBox(
			/*     Graphics */ g,
			/*  BorderStyle */ this.BorderStyle,
			/*            x */ 0,
			/*            y */ 0,
			/*        width */ this.Width,
			/*       height */ this.Height,
			/*       corner */ (this.vertScrollbar.Visible && this.horizScrollbar.Visible),
			/* cornerHeight */ this.horizScrollbar.Height,
			/*  cornerWidth */ this.vertScrollbar.Width);
		
		this.RedrawList(g);
	}
	
	private int LongestItemWidth
	{
		get
		{
			int maxWidth = 0;
			
			if(base.dataSource is ObjectCollection)
			{
				// Our own ObjectCollection keeps track of
				// the maximum width
				maxWidth = ((ObjectCollection) base.dataSource).maxWidth;
			}
			else
			{
				// Normal data sources must be grokked to find
				// the longest string, unfortunately.
				foreach(object value in (IList) base.dataSource)
					maxWidth = Math.Max(maxWidth, (int) this.nonClientGraphics.MeasureString(value.ToString(), this.Font).Width);
			}
			
			return maxWidth;
		}
	}

	internal void RedrawList()
	{
		this.RedrawList(this.nonClientGraphics);
	}

	private void RedrawList(Graphics g)
	{
		Rectangle listArea = this.ListArea;
	
		// Redraw the items
		Region oldClip = g.Clip;
		g.Clip = new Region(listArea);
		for(int i = this.TopIndex; 
		    i < this.TopIndex + NumItemsVisible; i++)
		{
			this.PaintItem(g, i);
		}
		g.Clip = oldClip;
		
		// Set up the vertical scrollbar
		IList data = (IList) base.dataSource;
		if(data.Count == 0)
			this.vertScrollbar.Visible = false;
		else
		{
			this.vertScrollbar.Maximum = data.Count;
			this.vertScrollbar.LargeChange = NumItemsVisible;
		}
		
		// Set up the horizontal scrollbar
		this.horizScrollbar.Maximum = this.LongestItemWidth;
		this.horizScrollbar.LargeChange = listArea.Width;
	}
	
	protected override void OnBorderStyleChanged(EventArgs e)
	{
		PositionControls();
		Redraw();
		base.OnBorderStyleChanged(e);
	}
	
	public override Object DataSource 
	{ 
		get
		{
			if(this.dataSource == this.internalDataSource)
				return null;
			else
				return this.dataSource;
		}
		set
		{
			if(value == null)
				this.dataSource = this.internalDataSource;
			else
				this.dataSource = value;
		}
	}
	
	public int TopIndex
	{
		get
		{
			return topIndex;
		}
	}
		
	private int NumItemsVisible
	{
		get
		{
			return this.ListArea.Height / this.ItemHeight;
		}
	}
	
	private bool IsItemVisible(int dataIndex)
	{
		return dataIndex >= this.TopIndex &&
		       dataIndex < this.TopIndex + NumItemsVisible;
	}
	
	private Rectangle ListArea
	{
		get
		{
			Rectangle listArea = new Rectangle(
				this.BorderWidth,
				this.BorderWidth,
				this.Width - 2 * this.BorderWidth,
				this.Height - 2 * this.BorderWidth);
				
			if(this.vertScrollbar.Visible)
				listArea.Width -= this.vertScrollbar.Width;
			if(this.horizScrollbar.Visible)
				listArea.Height -= this.horizScrollbar.Height;
				
			return(listArea);
		}
	}
	
	private Rectangle GetItemRect(int dataIndex)
	{
		if(IsItemVisible(dataIndex))
		{
			int screenIndex = dataIndex - TopIndex;
			Rectangle listArea = this.ListArea;
		
			return new Rectangle(
				listArea.Left,
				listArea.Top + screenIndex * this.ItemHeight,
				listArea.Width,
				this.ItemHeight);
		}
		else
			return new Rectangle(-1, -1, -1, -1);
	}
	
	private void PaintItems(int startInd, int endInd)
	{	
		for(int i = startInd; i <= endInd; i++)
		{
			this.PaintItem(i);
		}
	}
	
	internal void PaintItem(int ind)
	{
		this.PaintItem(this.nonClientGraphics, ind);
	}
	
	private void PaintItem(Graphics g, int dataIndex)
	{
		if(this.suppressDraw)
			return;

		Rectangle realItemRect = this.GetItemRect(dataIndex);
		IList data = (IList) base.dataSource;

		Brush textBrush = SystemBrushes.WindowText;

		// Is this item selected?
		if(this.SelectedIndices.Contains(dataIndex))
		{
			// Paint the selected background
			g.FillRectangle(SystemBrushes.Highlight, realItemRect);
			textBrush = SystemBrushes.HighlightText;
		}
		else
		{
			// Paint the normal background
			if(this.Enabled)
				g.FillRectangle(this.backgroundBrush, realItemRect);
			else
				g.FillRectangle(this.disabledBrush, realItemRect);
		}
		
		if(dataIndex >= this.topIndex && 
		   dataIndex < this.topIndex + this.NumItemsVisible &&
		   dataIndex < data.Count)
		{
			Rectangle itemRect = realItemRect;
			if(this.horizScrollbar.Visible)
			{
				itemRect.X -= this.horizScrollbar.Value;
				itemRect.Width += this.horizScrollbar.Value;
			}
			string s = base.GetItemText(data[dataIndex]);
							
			// Paint the text
			g.DrawString(s, this.Font, textBrush, itemRect, new StringFormat());
			
			// Does this item have the focus rectangle?
			if(this.FocusedItem == dataIndex)
			{
				Color backColor, foreColor;
				
				if(this.SelectedIndices.Contains(dataIndex))
				{
					backColor = SystemColors.Highlight;
					foreColor = SystemColors.HighlightText;
				}
				else
				{
					backColor = this.BackColor;
					foreColor = SystemColors.WindowText;
				}
				
				realItemRect.Height--;
				realItemRect.Width--;

				ThemeManager.MainPainter.DrawFocusRectangle(
					g, realItemRect, foreColor, backColor);
			}
		}
	}
	
	private void OnVScroll(object sender, ScrollEventArgs e)
	{
		if(this.topIndex != this.vertScrollbar.Value)
		{
			if(!this.vScrollTimer.Enabled)
				this.vScrollTimer.Start();
		}
	}
	
	private void VScrollTick(object sender, EventArgs e)
	{
		this.topIndex = this.vertScrollbar.Value;
		RedrawList(this.nonClientGraphics);
		this.vScrollTimer.Stop();
	}
	
	private void OnVValueChanged(object sender, EventArgs e)
	{
		if(this.topIndex != this.vertScrollbar.Value)
		{
			if(!this.vScrollTimer.Enabled)
				this.vScrollTimer.Start();
		}
	}
	
	private void OnHScroll(object sender, ScrollEventArgs e)
	{
		RedrawList(this.nonClientGraphics);
	}
	
	private void OnHValueChanged(object sender, EventArgs e)
	{
		RedrawList(this.nonClientGraphics);
	}
	
	private void ToggleSelected(int dataInd)
	{
		if(this.selectedIndices.Contains(dataInd))
			this.selectedIndices.Remove(dataInd);
		else
			this.selectedIndices.Add(dataInd);
	}
	
	private void ShiftHighlight(int dataInd)
	{
		int lastInd = 0;
		
		if(this.selectedIndices.Count > 0)
			lastInd = (int) this.selectedIndices[this.selectedIndices.Count - 1];
		
		this.ShiftHighlight(dataInd, lastInd, true, true);
	}
	
	// dataInd: select from the last index to dataInd.
	// lastInd: item to start highlight from.
	// clear: clear the selected indices and replace them with this range
	// select: true to select the range, false to unselect the range
	private void ShiftHighlight(int dataInd, int lastInd, bool clear, bool select)
	{
		Debug.Assert(this.selectionMode == SelectionMode.MultiExtended);
	
		// When holding down shift, highlight everything between the last 
		// selection  that wasn't with the Shift key and this one.  To do 
		// this, we'll put Control-clicks and regular clicks at the _end_
		// of  the  selectedIndices,  and anything  gotten through Shift-
		// clicking  at the beginning.  This way, we can always highlight 
		// based on the last item in the list.
		if(this.selectedIndices.Count == 0)
		{
			// Simply select if there is nothing else to go off of.
			this.selectedIndices.Add(dataInd);
		}
		else
		{
			ArrayList invalidated = new ArrayList();
			this.suppressDraw = true;

			int direction = (dataInd > lastInd ? 1 : -1);
			
			this.suppressEvents = true;
			
			if(clear)
			{
				// Start off with a clean slate.
				foreach(int ind in this.selectedIndices)
					invalidated.Add(ind);
				
				this.selectedIndices.Clear();
			}

			for(int i = lastInd; i != dataInd + direction; i += direction)
			{
				if(!invalidated.Contains(i))
					invalidated.Add(i);

				if(clear && select)
				{
					this.selectedIndices.Insert(0, i);
				}
				else if(!clear && select)
				{
					// Need to check if it's already selected
					if(!this.selectedIndices.Contains(i))
						this.selectedIndices.Insert(0, i);
				}
				else if(!clear && !select)
				{
					if(this.selectedIndices.Contains(i))
						this.selectedIndices.Remove(i);
				}
			}

			this.suppressDraw = false;
			foreach(int ind in invalidated)
				this.PaintItem(ind);

			this.suppressEvents = false;
			this.FireSelectedIndexChanged();
		}	
	}
	
	protected override void OnMouseDown(MouseEventArgs e)
	{
		int screenInd = this.HitTest(e.X, e.Y);
		
		if(screenInd != -1)
		{
			int dataInd = screenInd + this.topIndex;
			IList data = (IList) base.dataSource;
			
			if(dataInd >= data.Count)
				return;
		
			// Always move the focus to this item.
			this.FocusedItem = dataInd;
		
			switch(this.selectionMode)
			{
				case SelectionMode.None:
					// Do not do any selecting, only move the focus.
					break;
				
				case SelectionMode.One:
					// Remove any previous selection and focus/select
					// this item.
					this.suppressEvents = true;
					this.MoveSelection(dataInd);
					this.suppressEvents = false;
					this.FireSelectedIndexChanged();
					break;
				
				case SelectionMode.MultiSimple:
					// Move focus to this item and toggle selection.
					this.ToggleSelected(dataInd);
					break;
				
				case SelectionMode.MultiExtended:
					// Microsoft's  ListBox  implementation takes the Shift
					// if the user has both Control and Shift held down, so
					// we'll follow that.
					if((Control.ModifierKeys & Keys.Shift) != 0)
					{
						this.ShiftHighlight(dataInd);
					}
					else if((Control.ModifierKeys & Keys.Control) != 0)
					{
						// When holding down control, move focus
						// and toggle selection.
						this.ToggleSelected(dataInd);
						
						// Explanation:
						// In MultiExtended  selection mode, the user can Ctl- 
						// drag  and select a  region of  list  items,  while 
						// keeping  the  previous  selections.   If  they Ctl-
						// click  and  move,  it needs  to highlight  live as 
						// they move.   However,  if they move  back up a bit, 
						// we have  to restore  what was previously  selected/
						// unselected  there instead of just obliterating the
						// selection  information.   So we'll  keep a copy of 
						// the original selected items list.
						this.selectedIndices.Copy(this.prevSelectedIndices);
						
						// Also, since the user can unselect a region this
						// same way,  we can't rely  on checking  the last
						// item  in selectedIndices.   So  keep  it around.
						this.dragBeginInd = dataInd;
					}
					else
					{
						// Just select this one.
						this.suppressEvents = true;
						this.selectedIndices.Clear();
						this.selectedIndices.Add(dataInd);
						this.suppressEvents = false;
						this.FireSelectedIndexChanged();
					}
					break;
			}
		}
		
		base.OnMouseDown(e);
	}
	
	// This function only applies for single selection mode.
	private void MoveSelection(int newInd)
	{
		Debug.Assert(this.selectionMode == SelectionMode.One);
	
		if(this.selectedIndices.Count > 0)
			this.selectedIndices.Remove(this.selectedIndices[0]);
		this.selectedIndices.Add(newInd);
		
	}
	
	protected override void OnMouseMove(MouseEventArgs e)
	{
		if(e.Button == MouseButtons.Left)
		{
			int screenInd = this.HitTest(e.X, e.Y);

			// Note:
			// Multiple returns are generally bad form, but I'd like
			// to keep the amount of indentation in this function to
			// a sane level.
			if(screenInd == -1)
				return;
		
			bool ctrl = (Control.ModifierKeys & Keys.Control) != 0;
			bool shift = (Control.ModifierKeys & Keys.Shift) != 0;
			int dataInd = screenInd + this.topIndex;
			IList data = (IList) base.dataSource;
				
			if(dataInd >= data.Count)
				return;
				
			if(dataInd == this.focusedItem)
				return;
		
			switch(this.selectionMode)
			{
				case SelectionMode.MultiSimple:
				case SelectionMode.None:
					// Move the focus to this item.
					this.FocusedItem = dataInd;
					break;
					
				case SelectionMode.One:
					this.FocusedItem = dataInd;
					this.suppressEvents = true;
					this.MoveSelection(dataInd);
					this.suppressEvents = false;
					this.FireSelectedIndexChanged();
					break;
					
				case SelectionMode.MultiExtended:
					if(ctrl)
					{
						this.suppressEvents = true;	
					
						// Reset to the selected items that were there
						// when the user started click-dragging.
						this.prevSelectedIndices.Copy(this.selectedIndices);
						
						// We allow the user to either select or unselect
						// items.  We can tell based on whether the first
						// item is selected.
						bool select = this.selectedIndices.Contains(
							this.dragBeginInd);
						
						// ShiftHighlight will take care of firing
						// the event.
						this.ShiftHighlight(dataInd, this.dragBeginInd, 
							false, select);
					}
					else
						this.ShiftHighlight(dataInd);
					
					this.FocusedItem = dataInd;
					break;
			}
		}
	
		base.OnMouseMove(e);
	}
	
	private void EnsureFocusedItemVisible()
	{
		if(!IsItemVisible(this.FocusedItem))
		{
			int oldIndex = this.topIndex;

			if(this.FocusedItem < this.topIndex)
				this.topIndex = this.FocusedItem;
			else if(this.FocusedItem >= this.topIndex + this.NumItemsVisible)
				this.topIndex = this.FocusedItem - this.NumItemsVisible + 1;
				
			if(this.topIndex != oldIndex)
			{
				RedrawList();
				this.vertScrollbar.Value = this.topIndex;
			}
		}
	}

	protected override void OnKeyDown(KeyEventArgs e)
	{
		bool multiEx = (this.selectionMode == SelectionMode.MultiExtended);
		bool shift = (Control.ModifierKeys & Keys.Shift) != 0;
		IList data = (IList) base.dataSource;
		int maxInd = data.Count - 1;

		switch(e.KeyCode)
		{
			case Keys.Up:
			case Keys.Left:
				if(this.FocusedItem > 0)
					this.FocusedItem--;
				
				if(multiEx && shift)
					this.ShiftHighlight(this.FocusedItem);	

				e.Handled = true;
				break;
			
			case Keys.Down:
			case Keys.Right:
				if(this.FocusedItem < maxInd)
					this.FocusedItem++;
					
				if(multiEx && shift)
					this.ShiftHighlight(this.FocusedItem);
			
				e.Handled = true;
				break;
			
			case Keys.PageUp:
				this.FocusedItem = Math.Max(
					0, 
					this.FocusedItem - this.NumItemsVisible);
				
				if(multiEx && shift)
					this.ShiftHighlight(this.FocusedItem);
				
				e.Handled = true;
				break;
			
			case Keys.PageDown:
				this.FocusedItem = Math.Min(
					maxInd, 
					this.FocusedItem + this.NumItemsVisible);
				
				if(multiEx && shift)
					this.ShiftHighlight(this.FocusedItem);
				
				e.Handled = true;
				break;
			
			case Keys.Home:
				this.FocusedItem = 0;
				
				if(multiEx && shift)
					this.ShiftHighlight(this.FocusedItem);
				
				e.Handled = true;
				break;
			
			case Keys.End:
				this.FocusedItem = maxInd;
				
				if(multiEx && shift)
					this.ShiftHighlight(this.FocusedItem);
				
				e.Handled = true;
				break;
			
			case Keys.Space:
				if(this.selectionMode == SelectionMode.MultiSimple)
					this.ToggleSelected(this.FocusedItem);
				break;
				
		}
		
		this.EnsureFocusedItemVisible();
		
		if(e.Handled)
		{
			if(this.selectionMode == SelectionMode.One)
			{
				this.suppressEvents = true;	
				this.MoveSelection(this.FocusedItem);
				this.suppressEvents = false;
				this.FireSelectedIndexChanged();
			}
			else if(multiEx && !shift)
			{
				this.suppressEvents = true;
				this.selectedIndices.Clear();
				this.selectedIndices.Add(this.FocusedItem);
				this.suppressEvents = false;
				this.FireSelectedIndexChanged();
			}
		}
		
		base.OnKeyDown(e);
	}
	
	// Returns the screen index of the item at pt, or -1
	// if there isn't one.  Does not do any bounds checking to see
	// if there's actually an item at that point.
	private int HitTest(int x, int y)
	{
		Rectangle listArea = this.ListArea;
	
		if(listArea.Contains(x, y))
		{
			y -= listArea.Top;
			int ind = y / this.ItemHeight;
			if(ind < this.topIndex + this.NumItemsVisible)
				return y / this.ItemHeight;
		}
		
		return -1;
	}
	
	protected override void OnSizeChanged(EventArgs e)
	{
		this.CalculateScrollbars();
		base.OnSizeChanged(e);
	}

}; // class ListBox
	
}; // namespace System.Windows.Forms

