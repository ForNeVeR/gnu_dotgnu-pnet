/*
 * ListViewItem.cs - Implementation of the
 *			"System.Windows.Forms.ListViewItem" class.
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
using System.Collections;
using System.Drawing;
#if CONFIG_SERIALIZATION
using System.Runtime.Serialization;
#endif

	public class ListViewItem: ICloneable,
#if CONFIG_SERIALIZATION
		ISerializable
#endif
	{
		private int imageIndex;
		private object tag;
		internal ListView listView;

		[TODO]
		public Color BackColor
		{
			get
			{
				//TODO
				if (listView == null)
					return Color.Red;
				else
					return listView.BackColor;
			}

			set
			{
			}
		}

		[TODO]
		public Rectangle Bounds
		{
			get
			{
				if (listView == null)
					return Rectangle.Empty;
				else
					return listView.GetItemRect(Index);
			}
		}

		[TODO]
		public bool Checked
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
		public bool Focused
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
		public Font Font
		{
			get
			{
				if (listView == null)
					return null;
				else
					return listView.Font;
			}

			set
			{
			}
		}

		[TODO]
		public Color ForeColor
		{
			get
			{
				//TODO
				if (listView == null)
					return Color.Red;
				else
					return listView.ForeColor;
			}

			set
			{
			}
		}

		[TODO]
		public int ImageIndex
		{
			get
			{
				return imageIndex;
			}

			set
			{
				imageIndex = value;
				//TODO
			}
		}

		public ImageList ImageList
		{
			get
			{
				if (listView == null)
					return null;
				switch (listView.View)
				{
					case View.LargeIcon:
						return listView.LargeImageList;
					case View.Details:
					case View.SmallIcon:
					case View.List:
						return listView.SmallImageList;

					default:
						return null;
				}
				
			}
		}

		public int Index
		{
			get
			{
				if (listView == null)
					return -1;
				return 0;
			}
		}

		public ListView ListView
		{
			get
			{
				return listView;
			}
		}

		[TODO]
		public bool Selected
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
		public int StateImageIndex
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
		public ListViewSubItemCollection SubItems
		{
			get
			{
				return null;
			}
		}

		public object Tag
		{
			get
			{
				return tag;
			}

			set
			{
				tag = value;
			}
		}

		[TODO]
		public string Text
		{
			get
			{
				return String.Empty;
			}

			set
			{
			}
		}

		[TODO]
		public bool UseItemStyleForSubItems
		{
			get
			{
				return false;
			}

			set
			{
			}
		}

		public ListViewItem() : base()
		{
		}

		public ListViewItem(string text) : this(text, -1)
		{
		}

		public ListViewItem(string text, int imageIndex) : this()
		{
			this.imageIndex = imageIndex;
			Text = text;
		}

		public ListViewItem(string[] items) : this(items, -1)
		{
		}

		public ListViewItem(string[] items, int imageIndex, Color foreColor, Color backColor, Font font) : this(items, imageIndex)
		{
			ForeColor = foreColor;
			BackColor = backColor;
			Font = font;
		}

		[TODO]
		public ListViewItem(string[] items, int imageIndex) : this()
		{
			this.imageIndex = imageIndex;
			//TODO
		}

		[TODO]
		public ListViewItem(ListViewSubItem[] subItems, int imageIndex) : this()
		{
		}

		[TODO]
		public virtual object Clone()
		{
			return this;
		}

		[TODO]
		public void BeginEdit()
		{
		}

		public virtual void EnsureVisible()
		{
			if (listView != null)
				listView.EnsureVisible(Index);
		}

		public Rectangle GetBounds(ItemBoundsPortion portion)
		{
			if (listView != null)
				return listView.GetItemRect(Index, portion);
			else
				return Rectangle.Empty;
		}

		public virtual void Remove()
		{
			if (listView != null)
				listView.Items.RemoveAt(Index);
		}

		public override string ToString()
		{
			return "ListViewItem: \"" + Text + "\"";
		}

#if CONFIG_SERIALIZATION
		[TODO]
		void ISerializable.GetObjectData(SerializationInfo info, StreamingContext context)
		{
			Serialize(info, context);
		}

		[TODO]
		protected virtual void Deserialize(SerializationInfo info, StreamingContext context)
		{
		}

		[TODO]
		protected virtual void Serialize(SerializationInfo info, StreamingContext context)
		{
		}

#endif
		
		public class ListViewSubItem
		{
			internal ListViewItem owner;
			private string text;

			[TODO]
			public Color BackColor
			{
				get
				{
					return owner.listView.BackColor;
				}

				set
				{
				}
			}

			[TODO]
			public Font Font
			{
				get
				{
					return owner.listView.Font;
				}

				set
				{
				}
			}

			[TODO]
			public Color ForeColor
			{
				get
				{
					return Color.Red;
				}

				set
				{
				}
			}

			[TODO]
			public string Text
			{
				get
				{
					if (text != null)
						return text;
					else
						return "";
				}

				set
				{
					text = value;
				}
			}

			public ListViewSubItem(ListViewItem owner, string text)
			{
				this.owner = owner;
				this.text = text;
			}

			public ListViewSubItem(ListViewItem owner, string text, Color foreColor, Color backColor, Font font)
			{
				this.owner = owner;
				this.text = text;
			}

			public ListViewSubItem()
			{}

			[TODO]
			public void ResetStyle()
			{
			}

			public override string ToString()
			{
				return "ListViewSubItem: \"" + Text + "\"";
			}
		}


		public class ListViewSubItemCollection: IList
		{
			private ListViewItem owner;

			[TODO]
			public virtual int Count
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
					return false;
				}
			}

			[TODO]
			public ListViewSubItem this[int index]
			{
				get
				{
					return null;
				}

				set
				{
				}
			}

			public ListViewSubItemCollection(ListViewItem owner)
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

			[TODO]
			object IList.this[int index]
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
			public void AddRange(string[] items)
			{
			}

			[TODO]
			public void AddRange(ListViewSubItem[] items)
			{
			}

			[TODO]
			public void AddRange(string[] items, Color foreColor, Color backColor, Font font)
			{
			}

			[TODO]
			public ListViewSubItem Add(ListViewSubItem item)
			{
				return null;
			}

			[TODO]
			public ListViewSubItem Add(string text)
			{
				return null;
			}

			[TODO]
			public ListViewSubItem Add(string text, Color foreColor, Color backColor, Font font)
			{
				return null;
			}

			[TODO]
			int IList.Add(object item)
			{
				return -1;
			}

			[TODO]
			public virtual void Clear()
			{
			}

			[TODO]
			public bool Contains(ListViewSubItem subItem)
			{
				return false;
			}

			[TODO]
			bool IList.Contains(object subItem)
			{
				return false;
			}

			[TODO]
			public void Insert(int index, ListViewSubItem item)
			{
			}

			[TODO]
			void IList.Insert(int index, object item)
			{
			}

			[TODO]
			void IList.Remove(object item)
			{
			}

			[TODO]
			public void Remove(ListViewSubItem item)
			{
			}

			[TODO]
			public virtual void RemoveAt(int index)
			{
			}

			[TODO]
			void ICollection.CopyTo(Array dest, int index)
			{
			}

			[TODO]
			public virtual IEnumerator GetEnumerator()
			{
				return null;
			}

			[TODO]
			public int IndexOf(ListViewSubItem subItem)
			{
				return -1;
			}

			[TODO]
			int IList.IndexOf(object subItem)
			{
				return 0;
			}
		}
	}
}
