/*
 * ListView.cs - Implementation of the
 *			"System.Windows.Forms.ListView" class.
 *
 * Copyright (C) 2003  Neil Cawse.
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
	using System;
	using System.Drawing;
	using System.Collections;

	public class ListView : Control
	{
		private ItemActivation activation;
		private ListViewAlignment alignStyle;
		private BorderStyle borderStyle;
		private ColumnHeaderStyle headerStyle;
		private SortOrder sorting;
		private View viewStyle;
		private bool allowColumnReorder;
		private bool autoArrange;
		private bool checkBoxes;
		private bool fullRowSelect;
		private bool gridLines;
		private bool hideSelection;
		private bool labelEdit;
		private bool labelWrap;
		private bool multiSelect;
		private bool scrollable;
		private bool hoverSelection;
		internal int itemCount;
		private ListViewItemCollection items;
		private ColumnHeaderCollection columns;
		private CheckedListViewItemCollection checkedItems;
		private CheckedIndexCollection checkedIndices;
		private SelectedListViewItemCollection selectedItems;
		internal ColumnHeader[] columnHeaders;
		private Hashtable listItemsHashTable;
		private ArrayList listItemsArrayList;
		private SelectedIndexCollection selectedIndices;
		private IComparer listViewItemSorter;
		private ImageList largeImageList;
		private ImageList smallImageList;

		public event LabelEditEventHandler AfterLabelEdit;
		public event LabelEditEventHandler BeforeLabelEdit;
		public event ColumnClickEventHandler ColumnClick;
		public event EventHandler ItemActivate;
		public event ItemCheckEventHandler ItemCheck;
		public event ItemDragEventHandler ItemDrag;
		public event EventHandler SelectedIndexChanged;
		
		public ListView()
		{
			items = new ListViewItemCollection(this);
			columns = new ColumnHeaderCollection(this);
			listItemsHashTable = new Hashtable();
			listItemsArrayList = new ArrayList();
			autoArrange = true;
			hideSelection = true;
			labelWrap = true;
			multiSelect = true;
			scrollable = true;
			activation = ItemActivation.Standard;
			alignStyle = ListViewAlignment.Top;
			borderStyle = BorderStyle.Fixed3D;
			headerStyle = ColumnHeaderStyle.Clickable;
			sorting = SortOrder.None;
			viewStyle = View.LargeIcon;
		}

		[TODO]
		public ItemActivation Activation
		{
			get
			{
				return activation;
			}

			set
			{
				if (activation != value)
				{
					activation = value;
					//TODO
				}
			}
		}

		[TODO]
		public ListViewAlignment Alignment
		{
			get
			{
				return alignStyle;
			}

			set
			{
				if (alignStyle != value)
				{
					alignStyle = value;
					//TODO
				}
			}
		}

		[TODO]
		public bool AllowColumnReorder
		{
			get
			{
				return allowColumnReorder;
			}

			set
			{
				if (allowColumnReorder != value)
				{
					allowColumnReorder = value;
					//TODO
				}
			}
		}

		[TODO]
		public bool AutoArrange
		{
			get
			{
				return autoArrange;
			}

			set
			{
				if (value != autoArrange)
				{
					autoArrange = value;
					//TODO
				}
			}
		}

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
		public BorderStyle BorderStyle
		{
			get
			{
				return borderStyle;
			}

			set
			{
				if (borderStyle != value)
				{
					borderStyle = value;
					//TODO
				}
			}
		}

		[TODO]
		public bool CheckBoxes
		{
			get
			{
				return checkBoxes;
			}

			set
			{
				if (checkBoxes != value)
				{
					checkBoxes = value;
					//TODO
				}
			}
		}

		public CheckedIndexCollection CheckedIndices
		{
			get
			{
				if (checkedIndices == null)
					checkedIndices = new CheckedIndexCollection(this);
				return checkedIndices;
			}
		}

		public CheckedListViewItemCollection CheckedItems
		{
			get
			{
				if (checkedItems == null)
					checkedItems = new CheckedListViewItemCollection(this);
				return checkedItems;
			}
		}

		public ColumnHeaderCollection Columns
		{
			get
			{
				return columns;
			}
		}

		protected override CreateParams CreateParams
		{
			get
			{
				return base.CreateParams;
			}
		}

		[TODO]
		public ListViewItem FocusedItem
		{
			get
			{
				return null;
			}
		}

		protected override Size DefaultSize
		{
			get
			{
				return new Size(121, 97);
			}
		}

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
		public bool FullRowSelect
		{
			get
			{
				return fullRowSelect;
			}

			set
			{
				if (fullRowSelect != value)
				{
					fullRowSelect = value;
					//TODO
				}
			}
		}

		[TODO]
		public ColumnHeaderStyle HeaderStyle
		{
			get
			{
				return headerStyle;
			}

			set
			{
				if (headerStyle != value)
				{
					headerStyle = value;
					//TODO
				}
			}
		}

		[TODO]
		public bool GridLines
		{
			get
			{
				return gridLines;
			}

			set
			{
				if (gridLines != value)
				{
					gridLines = value;
					//TODO
				}
			}
		}

		[TODO]
		public bool HideSelection
		{
			get
			{
				return hideSelection;
			}

			set
			{
				if (value != hideSelection)
				{
					hideSelection = value;
					//TODO
				}
			}
		}

		public bool HoverSelection
		{
			get
			{
				return hoverSelection;
			}

			set
			{
				if (hoverSelection != value)
					hoverSelection = value;
			}
		}

		[TODO]
		public bool LabelEdit
		{
			get
			{
				return labelEdit;
			}

			set
			{
				if (value != labelEdit)
				{
					labelEdit = value;
				}
			}
		}

		[TODO]
		public bool LabelWrap
		{
			get
			{
				return labelWrap;
			}

			set
			{
				if (value != labelWrap)
				{
					labelWrap = value;
					//TODO
				}
			}
		}

		public ImageList LargeImageList
		{
			get
			{
				return largeImageList;
			}

			set
			{
				if (value != largeImageList)
				{
					largeImageList = value;
				}
		}
		}

		public ListViewItemCollection Items
		{
			get
			{
				return items;
			}
		}

		public IComparer ListViewItemSorter
		{
			get
			{
				return listViewItemSorter;
			}

			set
			{
				if (listViewItemSorter != value)
				{
					listViewItemSorter = value;
					Sort();
				}
			}
		}

		[TODO]
		public bool MultiSelect
		{
			get
			{
				return multiSelect;
			}

			set
			{
				if (value != multiSelect)
				{
					multiSelect = value;
					//TODO
				}
			}
		}

		[TODO]
		public bool Scrollable
		{
			get
			{
				return scrollable;
			}

			set
			{
				if (value != scrollable)
				{
					scrollable = value;
					//TODO
				}
			}
		}

		public SelectedIndexCollection SelectedIndices
		{
			get
			{
				if (selectedIndices == null)
					selectedIndices = new SelectedIndexCollection(this);
				return selectedIndices;
			}
		}

		public SelectedListViewItemCollection SelectedItems
		{
			get
			{
				if (selectedItems == null)
					selectedItems = new SelectedListViewItemCollection(this);
				return selectedItems;
			}
		}

		[TODO]
		public ImageList SmallImageList
		{
			get
			{
				return smallImageList;
			}

			set
			{
				if (value != smallImageList)
				{
					smallImageList = value;
					//TODO
				}
			}
		}

		[TODO]
		public SortOrder Sorting
		{
			get
			{
				return sorting;
			}

			set
			{
				if (sorting != value)
				{
					sorting = value;
					//TODO
				}
			}
		}

		[TODO]
		public ImageList StateImageList
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
		public override string Text
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
		public ListViewItem TopItem
		{
			get
			{
				return null;
			}
		}

		[TODO]
		public View View
		{
			get
			{
				return viewStyle;
			}

			set
			{
				if (viewStyle != value)
				{
					viewStyle = value;
					//TODO
				}
			}
		}

		protected override void OnEnabledChanged(EventArgs e)
		{
			base.OnEnabledChanged(e);
		}

		[TODO]
		public void ArrangeIcons(ListViewAlignment value)
		{
		}

		public void ArrangeIcons()
		{
			ArrangeIcons(ListViewAlignment.Default);
		}

		[TODO]
		public void BeginUpdate()
		{
		}

		[TODO]
		public void Clear()
		{
			Items.Clear();
			Columns.Clear();
		}

		protected override void CreateHandle()
		{
			base.CreateHandle();
		}

		protected override void Dispose(bool disposing)
		{
			base.Dispose(disposing);
		}

		[TODO]
		public void EndUpdate()
		{
		}

		[TODO]
		public void EnsureVisible(int index)
		{
		}

		[TODO]
		public ListViewItem GetItemAt(int x, int y)
		{
			return null;
		}

		public Rectangle GetItemRect(int index)
		{
			return GetItemRect(index, ItemBoundsPortion.Entire);
		}

		[TODO]
		public Rectangle GetItemRect(int index, ItemBoundsPortion portion)
		{
			return Rectangle.Empty;
		}

		[TODO]
		protected override bool IsInputKey(Keys keyData)
		{
			return false;
		}

		protected virtual void OnAfterLabelEdit(LabelEditEventArgs e)
		{
			if (AfterLabelEdit != null)
				AfterLabelEdit(this, e);
		}

		protected virtual void OnBeforeLabelEdit(LabelEditEventArgs e)
		{
			if (BeforeLabelEdit != null)
				BeforeLabelEdit(this, e);
		}

		protected virtual void OnColumnClick(ColumnClickEventArgs e)
		{
			if (ColumnClick != null)
				ColumnClick(this, e);
		}

		protected override void OnFontChanged(EventArgs e)
		{
			base.OnFontChanged(e);
		}

		protected override void OnHandleCreated(EventArgs e)
		{
			base.OnHandleCreated(e);
		}

		protected override void OnHandleDestroyed(EventArgs e)
		{
			base.OnHandleDestroyed(e);
		}

		protected virtual void OnItemActivate(EventArgs e)
		{
			if (ItemActivate != null)
				ItemActivate(this, e);
		}

		protected virtual void OnItemCheck(ItemCheckEventArgs ice)
		{
			if (ItemCheck != null)
				ItemCheck(this, ice);
		}

		protected virtual void OnItemDrag(ItemDragEventArgs e)
		{
			if (ItemDrag != null)
				ItemDrag(this, e);
		}

		[TODO]
		protected virtual void OnSelectedIndexChanged(EventArgs e)
		{
			if (SelectedIndexChanged != null)
				SelectedIndexChanged(this, EventArgs.Empty);
		}

		[TODO]
		protected override void OnSystemColorsChanged(EventArgs e)
		{
			base.OnSystemColorsChanged(e);
		}

		[TODO]
		protected void RealizeProperties()
		{
		}

		[TODO]
		public void Sort()
		{
		}

		[TODO]
		public override string ToString()
		{
			return base.ToString();
		}

		protected void UpdateExtendedStyles()
		{
		}
#if !CONFIG_COMPACT_FORMS
		protected override void WndProc(ref Message m)
		{
		}
#endif
	}

	public class CheckedIndexCollection: IList
	{
		private ListView owner;

		public virtual int Count
		{
			get
			{
				if (!owner.CheckBoxes)
					return 0;
				int count = 0;
				foreach(ListViewItem item in owner.Items)
					if (item.Checked)
						count++;
				return count;
			}
		}

		public int this[int index]
		{
			get
			{
				int pos = 0;
				foreach(ListViewItem item in owner.Items)
				{
					if (item.Checked)
					{
						if (pos == index)
							return pos;
						pos++;
					}
				}
				throw new ArgumentOutOfRangeException();
			}
		}

		object IList.this[int index]
		{
			get
			{
				return this[index];
			}
			set
			{
				throw new NotSupportedException();
			}
		}

		public virtual bool IsReadOnly
		{
			get
			{
				return true;
			}
		}

		public CheckedIndexCollection(ListView owner)
		{
			this.owner = owner;
		}

		object ICollection.SyncRoot
		{
			get
			{
				return this;
			}
		}

		bool ICollection.IsSynchronized
		{
			get
			{
				return false;
			}
		}

		bool IList.IsFixedSize
		{
			get
			{
				return true;
			}
		}

		public bool Contains(int checkedIndex)
		{
			if (owner.Items[checkedIndex].Checked)
				return true;
			else
				return false;
		}

		bool IList.Contains(object checkedIndex)
		{
			if (checkedIndex is Int32)
			{
				return Contains((int)checkedIndex);
			}
			else
			{
				return false;
			}
		}

		[TODO]
		public int IndexOf(int checkedIndex)
		{
			//TODO
			return -1;
		}

		[TODO]
		int IList.IndexOf(object checkedIndex)
		{
			if (checkedIndex is Int32)
			{
				return IndexOf((int)checkedIndex);
			}
			else
			{
				return -1;
			}
		}

		int IList.Add(object value)
		{
			throw new NotSupportedException();
		}

		void IList.Clear()
		{
			throw new NotSupportedException();
		}

		void IList.Insert(int index, object value)
		{
			throw new NotSupportedException();
		}

		void IList.Remove(object value)
		{
			throw new NotSupportedException();
		}

		void IList.RemoveAt(int index)
		{
			throw new NotSupportedException();
		}

		[TODO]
		void ICollection.CopyTo(Array dest, int index)
		{
			//TODO
		}

		[TODO]
		public virtual IEnumerator GetEnumerator()
		{
			//TODO
			return null;
		}
	}


	public class CheckedListViewItemCollection: IList
	{
		private ListView owner;

		public virtual int Count
		{
			get
			{
				return owner.CheckedIndices.Count;
			}
		}

		[TODO]
		private ListViewItem[] ItemArray
		{
			get
			{
				return null;
			}
		}

		[TODO]
		public ListViewItem this[int index]
		{
			get
			{
				return null;
			}
		}

		object IList.this[int index]
		{
			get
			{
				return this[index];
			}
			set
			{
				throw new NotSupportedException();
			}
		}

		public virtual bool IsReadOnly
		{
			get
			{
				return true;
			}
		}

		public CheckedListViewItemCollection(ListView owner)
		{
			this.owner = owner;
		}

		object ICollection.SyncRoot
		{
			get
			{
				return this;
			}
		}

		bool ICollection.IsSynchronized
		{
			get
			{
				return false;
			}
		}

		bool IList.IsFixedSize
		{
			get
			{
				return true;
			}
		}

		public bool Contains(ListViewItem item)
		{
			if (item != null && item.ListView == owner && item.Checked)
				return true;
			else
				return false;
		}

		bool IList.Contains(object item)
		{
			if (item is ListViewItem)
				return Contains(item as ListViewItem);
			else
				return false;
		}

		public int IndexOf(ListViewItem item)
		{
			for (int i = 0; i < ItemArray.Length; i++)
				if (ItemArray[i] == item)
					return i;
			return -1;
		}

		int IList.IndexOf(object item)
		{
			if (item is ListViewItem)
				return IndexOf(item as ListViewItem);
			else
				return -1;
		}

		void IList.Clear()
		{
			throw new NotSupportedException();
		}

		int IList.Add(object value)
		{
			throw new NotSupportedException();
		}

		void IList.Insert(int index, object value)
		{
			throw new NotSupportedException();
		}

		void IList.RemoveAt(int index)
		{
			throw new NotSupportedException();
		}

		void IList.Remove(object value)
		{
			throw new NotSupportedException();
		}

		public virtual void CopyTo(Array dest, int index)
		{
			if (Count > 0)
				Array.Copy(ItemArray, 0, dest, index, Count);
		}

		public virtual IEnumerator GetEnumerator()
		{
			if (ItemArray != null)
				return ItemArray.GetEnumerator();
			else
				return new ListViewItem[0].GetEnumerator();
		}
	}

	public class SelectedIndexCollection: IList
	{
		private ListView owner;

		[TODO]
		public virtual int Count
		{
			get
			{
				return 0;
			}
		}

		[TODO]
		public int this[int index]
		{
			get
			{
				return 0;
			}
		}

		public virtual bool IsReadOnly
		{
			get
			{
				return true;
			}
		}

		public SelectedIndexCollection(ListView owner)
		{
			this.owner = owner;
		}

		object IList.this[int index]
		{
			get
			{
				return this[index];
			}
			set
			{
				throw new NotSupportedException();
			}
		}

		object ICollection.SyncRoot
		{
			get
			{
				return this;
			}
		}

		bool ICollection.IsSynchronized
		{
			get
			{
				return false;
			}
		}

		bool IList.IsFixedSize
		{
			get
			{
				return true;
			}
		}

		public bool Contains(int selectedIndex)
		{
			return owner.Items[selectedIndex].Selected;
		}

		bool IList.Contains(object selectedIndex)
		{
			if (selectedIndex is Int32)
				return Contains((int)selectedIndex);
			else
				return false;
		}

		[TODO]
		public int IndexOf(int selectedIndex)
		{
			return -1;
		}

		int IList.IndexOf(object selectedIndex)
		{
			if (selectedIndex is Int32)
				return IndexOf((int)selectedIndex);
			else
				return -1;
		}

		int IList.Add(object value)
		{
			throw new NotSupportedException();
		}

		void IList.Clear()
		{
			throw new NotSupportedException();
		}

		void IList.Insert(int index, object value)
		{
			throw new NotSupportedException();
		}

		void IList.RemoveAt(int index)
		{
			throw new NotSupportedException();
		}

		void IList.Remove(object value)
		{
			throw new NotSupportedException();
		}

		[TODO]
		public virtual void CopyTo(Array dest, int index)
		{
		}

		[TODO]
		public virtual IEnumerator GetEnumerator()
		{
			return null;
		}
	}


	public class SelectedListViewItemCollection: IList
	{
		private ListView owner;

		[TODO]
		private ListViewItem[] SelectedItemArray
		{
			get
			{
				return null;
			}
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
		public ListViewItem this[int index]
		{
			get
			{
				return null;
			}
		}

		public virtual bool IsReadOnly
		{
			get
			{
				return true;
			}
		}

		public SelectedListViewItemCollection(ListView owner)
		{
			this.owner = owner;
		}

		object IList.this[int index]
		{
			get
			{
				return this[index];
			}
			set
			{
				throw new NotSupportedException();
			}
		}

		bool IList.IsFixedSize
		{
			get
			{
				return true;
			}
		}

		object ICollection.SyncRoot
		{
			get
			{
				return this;
			}
		}

		bool ICollection.IsSynchronized
		{
			get
			{
				return false;
			}
		}

		int IList.Add(object value)
		{
			throw new NotSupportedException();
		}

		void IList.Insert(int index, object value)
		{
			throw new NotSupportedException();
		}

		void IList.RemoveAt(int index)
		{
			throw new NotSupportedException();
		}

		void IList.Remove(object value)
		{
			throw new NotSupportedException();
		}

		public virtual void Clear()
		{
			for (int i = 0; i < SelectedItemArray.Length; i++)
				SelectedItemArray[i].Selected = false;
		}

		public bool Contains(ListViewItem item)
		{
			return IndexOf(item) != -1;
		}

		bool IList.Contains(object item)
		{
			if (item is ListViewItem)
				return Contains(item as ListViewItem);
			else
				return false;
		}

		public virtual void CopyTo(Array dest, int index)
		{
			if (Count > 0)
				Array.Copy(SelectedItemArray, 0, dest, index, Count);
		}

		public int IndexOf(ListViewItem item)
		{
			for (int i = 0; i < SelectedItemArray.Length; i++)
			{
				if (SelectedItemArray[i] == item)
					return i;
			}
			return -1;
		}

		public virtual IEnumerator GetEnumerator()
		{
			if (SelectedItemArray != null)
				return SelectedItemArray.GetEnumerator();
			else
				return new ListViewItem[0].GetEnumerator();
		}

		int IList.IndexOf(object item)
		{
			if (item is ListViewItem)
				return IndexOf(item as ListViewItem);
			else
				return -1;
		}
	}


	public class ColumnHeaderCollection: IList
	{
		private ListView owner;

		public virtual ColumnHeader this[int index]
		{
			get
			{
				return owner.columnHeaders[index];
			}
		}

		public virtual int Count
		{
			get
			{
				if (owner.columnHeaders != null)
					return owner.columnHeaders.Length;
				else
					return 0;
			}
		}

		public virtual bool IsReadOnly
		{
			get
			{
				return false;
			}
		}

		public ColumnHeaderCollection(ListView owner)
		{
			this.owner = owner;
		}

		object IList.this[int index]
		{
			get
			{
				return this[index];
			}
			set
			{
				throw new NotSupportedException();
			}
		}

		object ICollection.SyncRoot
		{
			get
			{
				return this;
			}
		}

		bool ICollection.IsSynchronized
		{
			get
			{
				return true;
			}
		}

		bool IList.IsFixedSize
		{
			get
			{
				return false;
			}
		}

		[TODO]
		public virtual ColumnHeader Add(string str, int width, HorizontalAlignment textAlign)
		{
			ColumnHeader columnHeader = new ColumnHeader();
			columnHeader.Text = str;
			columnHeader.Width = width;
			columnHeader.TextAlign = textAlign;
			//TODO
			return columnHeader;
		}

		[TODO]
		public virtual int Add(ColumnHeader value)
		{
			//TODO;
			return Count;
		}

		public virtual void AddRange(ColumnHeader[] values)
		{
			for (int i = 0; i < values.Length; i++)
				Add(values[i]);
		}

		int IList.Add(object value)
		{
			return Add(value as ColumnHeader);
		}

		[TODO]
		public virtual void Clear()
		{
			if (owner.columnHeaders != null)
			{
				owner.columnHeaders = null;
				//TODO
			}
		}

		void ICollection.CopyTo(Array dest, int index)
		{
			if (Count > 0)
				Array.Copy(owner.columnHeaders, 0, dest, index, Count);
		}

		public int IndexOf(ColumnHeader value)
		{
			for (int i = 0; i < Count; i++)
			{
				if (this[i] == value)
					return i;
			}
			return -1;
		}

		int IList.IndexOf(object value)
		{
			if (value is ColumnHeader)
				return IndexOf(value as ColumnHeader);
			else
				return -1;
		}

		public bool Contains(ColumnHeader value)
		{
			return IndexOf(value) != -1;
		}

		bool IList.Contains(object value)
		{
			if (value is ColumnHeader)
				return Contains(value as ColumnHeader);
			else
				return false;
		}

		[TODO]
		public virtual void Remove(ColumnHeader column)
		{
			int i = IndexOf(column);
			if (i != -1)
				RemoveAt(i);
		}

		void IList.Remove(object value)
		{
			if (value is ColumnHeader)
				Remove(value as ColumnHeader);
		}

		public virtual void RemoveAt(int index)
		{
		}

		public virtual IEnumerator GetEnumerator()
		{
			if (owner.columnHeaders != null)
				return owner.columnHeaders.GetEnumerator();
			else
				return new ColumnHeader[0].GetEnumerator();
		}
		[TODO]
		public void Insert(int index, ColumnHeader value)
		{
		}

		[TODO]
		void IList.Insert(int index, object value)
		{
		}

		[TODO]
		public void Insert(int index, string str, int width, HorizontalAlignment textAlign)
		{
		}
	}

	public class ListViewItemCollection: IList
	{
		private ListView owner;

		public virtual int Count
		{
			get
			{
				return owner.itemCount;
			}
		}

		public virtual bool IsReadOnly
		{
			get
			{
				return false;
			}
		}

		public virtual ListViewItem this[int displayIndex]
		{
			get
			{
				return null;
			}

			set
			{
				RemoveAt(displayIndex);
				Insert(displayIndex, value);
			}
		}

		public ListViewItemCollection(ListView owner)
		{
			this.owner = owner;
		}

		object ICollection.SyncRoot
		{
			get
			{
				return this;
			}
		}

		bool ICollection.IsSynchronized
		{
			get
			{
				return true;
			}
		}

		bool IList.IsFixedSize
		{
			get
			{
				return false;
			}
		}

		object IList.this[int index]
		{
			get
			{
				return this[index];
			}
			set
			{
				if (value is ListViewItem)
				{
					this[index] = value as ListViewItem;
				}
				else if (value != null)
				{
					this[index] = new ListViewItem(value.ToString(), -1);
				}
			}
		}

		public virtual ListViewItem Add(string text)
		{
			return Add(text, -1);
		}

		int IList.Add(object item)
		{
			if (item is ListViewItem)
				return IndexOf(Add((ListViewItem)item));
			if (item != null)
				return IndexOf(Add(item.ToString()));
			else
				return -1;
		}

		public virtual ListViewItem Add(string text, int imageIndex)
		{
			ListViewItem listViewItem = new ListViewItem(text, imageIndex);
			Add(listViewItem);
			return listViewItem;
		}

		public virtual ListViewItem Add(ListViewItem value)
		{
			return value;
		}

		[TODO]
		public void AddRange(ListViewItem[] values)
		{

		}

		[TODO]
		public virtual void Clear()
		{
			if (owner.itemCount > 0)
			{
				owner.itemCount = 0;
				//TODO
			}
		}

		public bool Contains(ListViewItem item)
		{
			return IndexOf(item) != -1;
		}

		bool IList.Contains(object item)
		{
			if (item is ListViewItem)
				return Contains(item as ListViewItem);
			else
				return false;
		}

		public virtual void CopyTo(Array dest, int index)
		{
			if (owner.itemCount > 0)
				for (int i = 0; i < Count; i++)
					dest.SetValue(this[i], index++);
		}

		public virtual IEnumerator GetEnumerator()
		{
			ListViewItem[] listViewItems = new ListViewItem[Count];
			CopyTo(listViewItems, 0);
			return listViewItems.GetEnumerator();
		}

		public int IndexOf(ListViewItem item)
		{
			for (int i = 0; i < Count; i++)
				if (this[i] == item)
					return i;
			return -1;
		}

		int IList.IndexOf(object item)
		{
			if (item is ListViewItem)
				return IndexOf((ListViewItem)item);
			else
				return -1;
		}

		void IList.Insert(int index, object item)
		{
			if (item is ListViewItem)
				Insert(index, (ListViewItem)item);
			else if (item != null)
				Insert(index, item.ToString());
		}

		[TODO]
		public virtual void RemoveAt(int index)
		{
		}

		public virtual void Remove(ListViewItem item)
		{
			if (item == null)
				return;
			int i = item.Index;
			if (i >= 0)
			{
				RemoveAt(i);
			}
		}

		void IList.Remove(object item)
		{
			if (item == null || !(item is ListViewItem))
				return;
			Remove(item as ListViewItem);
		}
		public ListViewItem Insert(int index, ListViewItem item)
		{
			return null;
		}

		public ListViewItem Insert(int index, string text)
		{
			return Insert(index, new ListViewItem(text));
		}

		public ListViewItem Insert(int index, string text, int imageIndex)
		{
			return Insert(index, new ListViewItem(text, imageIndex));
		}

	}
}
