/*
 * CheckListBox.cs - Implementation of the
 *			"System.Windows.Forms.CheckListBox" class.
 *
 * Copyright (C) 2003 Neil Cawse.
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
using System.Collections;
using System.ComponentModel;
using System.Drawing.Design;


	[TODO]
	public class CheckedListBox : ListBox
	{
		private CheckedItemCollection checkedItems;
		private CheckedIndexCollection checkedIndices;
		private bool checkOnClick;
		private int lastSelected;

		public event ItemCheckEventHandler ItemCheck;
		
		public CheckedListBox()
		{
			lastSelected = -1;
		}

#if CONFIG_COMPONENT_MODEL || CONFIG_EXTENDED_DIAGNOSTICS
		[DefaultValue(false)]
#endif
		public bool CheckOnClick
		{
			get
			{
				return checkOnClick;
			}

			set
			{
				checkOnClick = value;
			}
		}

#if CONFIG_COMPONENT_MODEL || CONFIG_EXTENDED_DIAGNOSTICS
		[DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
		[Browsable(false)]
#endif
		public CheckedIndexCollection CheckedIndices
		{
			get
			{
				if (checkedIndices == null)
					checkedIndices = new CheckedIndexCollection(this);
				return checkedIndices;
			}
		}

#if CONFIG_COMPONENT_MODEL || CONFIG_EXTENDED_DIAGNOSTICS
		[DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
		[Browsable(false)]
#endif
		public CheckedItemCollection CheckedItems
		{
			get
			{
				if (checkedItems == null)
				{
					checkedItems = new CheckedItemCollection(this);
				}
				return checkedItems;
			}
		}

		[TODO]
#if CONFIG_COMPONENT_MODEL || CONFIG_EXTENDED_DIAGNOSTICS
		[EditorBrowsable(EditorBrowsableState.Never)]
		[Browsable(false)]
#endif
		public override object DataSource
		{
			get
			{
				return base.DataSource;
			}

			set
			{
				base.DataSource = value;
			}
		}

		[TODO]
#if CONFIG_COMPONENT_MODEL || CONFIG_EXTENDED_DIAGNOSTICS
		[EditorBrowsable(EditorBrowsableState.Never)]
		[Browsable(false)]
#endif
		public override string DisplayMember
		{
			get
			{
				return base.DisplayMember;
			}

			set
			{
				base.DisplayMember = value;
			}
		}

		[TODO]
#if CONFIG_COMPONENT_MODEL || CONFIG_EXTENDED_DIAGNOSTICS
		[DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
		[EditorBrowsable(EditorBrowsableState.Never)]
		[Browsable(false)]
#endif
		public override DrawMode DrawMode
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
#if CONFIG_COMPONENT_MODEL || CONFIG_EXTENDED_DIAGNOSTICS
		[DefaultValue(false)]
#endif
		public bool ThreeDCheckBoxes 
		{
			get
			{
				return false;
			}
			set
			{
			}
		}

		protected override ListBox.ObjectCollection CreateItemCollection()
		{
			return new ObjectCollection(this);
		}

		[TODO]
		public CheckState GetItemCheckState(int index)
		{
			return CheckState.Indeterminate;
		}

		[TODO]
#if CONFIG_COMPONENT_MODEL || CONFIG_EXTENDED_DIAGNOSTICS
		[DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
		[EditorBrowsable(EditorBrowsableState.Never)]
		[Browsable(false)]
#endif
		public override int ItemHeight
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
#if !CONFIG_SMALL_CONSOLE
#if CONFIG_COMPONENT_MODEL || CONFIG_EXTENDED_DIAGNOSTICS
		[Editor("ListControlStringCollectionEditor, System.Design", typeof(UITypeEditor))]
		[DesignerSerializationVisibility(DesignerSerializationVisibility.Content)]
		[Localizable(true)]
#endif
#endif
		public new ObjectCollection Items
		{
			get
			{
				return base.Items as ObjectCollection;
			}
		}

		[TODO]
		public override SelectionMode SelectionMode
		{
			get
			{
				return base.SelectionMode;
			}

			set
			{
			}
		}

		[TODO]
		protected override AccessibleObject CreateAccessibilityInstance()
		{
			return null;
		}

		public bool GetItemChecked(int index)
		{
			return GetItemCheckState(index) != CheckState.Unchecked;
		}

		protected override void OnClick(EventArgs e)
		{
			base.OnClick(e);
		}

		protected override void OnHandleCreated(EventArgs e)
		{
			base.OnHandleCreated(e);
		}

		[TODO]
		protected override void OnDrawItem(DrawItemEventArgs e)
		{
		}
		
		[TODO]
		public void SetItemCheckState(int index, CheckState value)
		{
		}

		public void SetItemChecked(int index, bool value)
		{
			if (value)
				SetItemCheckState(index, CheckState.Checked);
			else
				SetItemCheckState(index, CheckState.Unchecked);
		}

		protected override void OnBackColorChanged(EventArgs e)
		{
			base.OnBackColorChanged(e);
		}

		protected override void OnFontChanged(EventArgs e)
		{
			base.OnFontChanged(e);
		}

		protected override void OnKeyPress(KeyPressEventArgs e)
		{
			base.OnKeyPress(e);
		}

		protected virtual void OnItemCheck(ItemCheckEventArgs ice)
		{
			if (ItemCheck != null)
				ItemCheck(this, ice);
		}

		protected override void OnMeasureItem(MeasureItemEventArgs e)
		{
			base.OnMeasureItem(e);
		}

		protected override void OnSelectedIndexChanged(EventArgs e)
		{
			base.OnSelectedIndexChanged(e);
			lastSelected = base.SelectedIndex;
		}

#if !CONFIG_COMPACT_FORMS
		protected override void WndProc(ref Message m)
		{
			// Not used in this implementation
		}
#endif

		public class CheckedIndexCollection: IList
		{
			private CheckedListBox owner;

			public virtual int Count
			{
				get
				{
					return owner.CheckedItems.Count;
				}
			}

			public virtual bool IsReadOnly
			{
				get
				{
					return true;
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

			internal CheckedIndexCollection(CheckedListBox owner)
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

			void IList.RemoveAt(int index)
			{
				throw new NotSupportedException();
			}
			
			void IList.Remove(object value)
			{
				throw new NotSupportedException();
			}

			public bool Contains(int index)
			{
				return IndexOf(index) != -1;
			}

			bool IList.Contains(object index)
			{
				if (index is Int32)
					return Contains((int)index);
				else
					return false;
			}

			public virtual void CopyTo(Array dest, int index)
			{
				for (int i = 0; i < owner.CheckedItems.Count; i++)
					dest.SetValue(this[i], i + index);
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

			public virtual IEnumerator GetEnumerator()
			{
				int[] items = new int[Count];
				CopyTo(items, 0);
				return items.GetEnumerator();
			}

			[TODO]
			public int IndexOf(int index)
			{
				return 0;
			}

			int IList.IndexOf(object index)
			{
				if (index is Int32)
					return IndexOf((int)index);
				else
					return -1;
			}
		}

		public new class ObjectCollection : ListBox.ObjectCollection
		{
			private CheckedListBox owner;

			public ObjectCollection(CheckedListBox owner) : base(owner)
			{
				this.owner = owner;
			}

			public int Add(object item, bool isChecked)
			{
				if (isChecked)
					return Add(item, CheckState.Checked);
				else
					return Add(item, CheckState.Unchecked);
			}

			public int Add(object item, CheckState check)
			{
				int pos = base.Add(item);
				owner.SetItemCheckState(pos, check);
				return pos;
			}
		}

		public class CheckedItemCollection: IList
		{
			private CheckedListBox owner;

			[TODO]
			public virtual int Count
			{
				get
				{
					return 0;
				}
			}

			[TODO]
			public virtual object this[int index]
			{
				get
				{
					return 0;
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

			public virtual bool Contains(object item)
			{
				return IndexOf(item) != -1;
			}

			[TODO]
			public virtual int IndexOf(object item)
			{
				return 0;
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
			public virtual void CopyTo(Array dest, int index)
			{
			}

			[TODO]
			public virtual IEnumerator GetEnumerator()
			{
				return null;
			}

			internal CheckedItemCollection(CheckedListBox owner) : base()
			{
				this.owner = owner;
			} 

		}
	}
}
