/*
 * Menu.cs - Implementation of the "System.Windows.Forms.Menu" class.
 *
 * Copyright (C) 2003  Southern Storm Software, Pty Ltd.
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

using System.ComponentModel;
using System.Collections;

public abstract class Menu
#if CONFIG_COMPONENT_MODEL
	: Component
#endif
{
	// Internal state.
	private int numItems;
	private MenuItem[] itemList;
	private MenuItemCollection items;
	private int suppressUpdates;

	// Constructor.
	protected Menu(MenuItem[] items)
			{
				if(items != null)
				{
					this.numItems = items.Length;
					this.itemList = new MenuItem [numItems];
					Array.Copy(items, 0, this.itemList, 0, numItems);
					this.items = null;
					this.suppressUpdates = 0;
				}
				else
				{
					this.numItems = 0;
					this.itemList = new MenuItem [0];
					this.items = null;
					this.suppressUpdates = 0;
				}
			}

	// Suppress updates on this menu while it is being modified.
	internal void SuppressUpdates()
			{
				++suppressUpdates;
			}

	// Allow updates on this menu after it was modified.
	[TODO]
	internal void AllowUpdates()
			{
				--suppressUpdates;
				if(suppressUpdates == 0)
				{
					// TODO: force a repaint/recalc of the menu
				}
			}

	// Get or set this object's properties.
	public IntPtr Handle
			{
				get
				{
					// Menu handles are not used in this implementation.
					return IntPtr.Zero;
				}
			}
	public virtual bool IsParent
			{
				get
				{
					return (numItems > 0);
				}
			}
	public MenuItem MdiListItem
			{
				get
				{
					int index;
					MenuItem list;
					for(index = 0; index < numItems; ++index)
					{
						if(itemList[index].MdiList)
						{
							return itemList[index];
						}
						else
						{
							list = itemList[index].MdiListItem;
							if(list != null)
							{
								return list;
							}
						}
					}
					return null;
				}
			}
	public MenuItemCollection MenuItems
			{
				get
				{
					if(items == null)
					{
						items = new MenuItemCollection(this);
					}
					return items;
				}
			}

	// Get the context menu that contains this item.
	public ContextMenu GetContextMenu()
			{
				Menu menu = this;
				while(menu != null && !(menu is ContextMenu))
				{
					if(menu is MenuItem)
					{
						menu = ((MenuItem)menu).parent;
					}
					else
					{
						return null;
					}
				}
				return (ContextMenu)menu;
			}

	// Get the main menu that contains this item.
	public MainMenu GetMainMenu()
			{
				Menu menu = this;
				while(menu != null && !(menu is MainMenu))
				{
					if(menu is MenuItem)
					{
						menu = ((MenuItem)menu).parent;
					}
					else
					{
						return null;
					}
				}
				return (MainMenu)menu;
			}

	// Merge another menu with this one.
	[TODO]
	public virtual void MergeMenu(Menu menuSrc)
			{
				// TODO
			}

	// Convert this object into a string.
	public override String ToString()
			{
				return base.ToString() + ", Item.Count=" + numItems.ToString();
			}

	// Clone the contents of another menu into this one.
	[TODO]
	protected void CloneMenu(Menu menuSrc)
			{
				// TODO
			}

	// Collection of menu items.
	public class MenuItemCollection : IList, ICollection, IEnumerable
	{
		// Internal State.
		private Menu owner;

		// Constructor.
		public MenuItemCollection(Menu owner)
				{
					this.owner = owner;
				}

		// Retrieve a particular item from this collection.
		public virtual MenuItem this[int index]
				{
					get
					{
						if(index < 0 || index >= owner.numItems)
						{
							throw new ArgumentOutOfRangeException
								("index", S._("SWF_MenuItemIndex"));
						}
						return owner.itemList[index];
					}
				}

		// Implement the ICollection interface.
		public void CopyTo(Array array, int index)
				{
					IEnumerator e = GetEnumerator();
					while(e.MoveNext())
					{
						array.SetValue(e.Current, index);
					}
				}
		public int Count
				{
					get
					{
						return owner.numItems;
					}
				}
		bool ICollection.IsSynchronized
				{
					get
					{
						return false;
					}
				}
		Object ICollection.SyncRoot
				{
					get
					{
						return this;
					}
				}

		// Implement the IList interface.
		int IList.Add(Object value)
				{
					MenuItem item = (value as MenuItem);
					if(item != null)
					{
						return Add(item);
					}
					else
					{
						throw new ArgumentException
							(S._("SWF_InvalidMenuItem"), "value");
					}
				}
		public void Clear()
				{
					if(owner.numItems != 0)
					{
						owner.SuppressUpdates();
						while(owner.numItems > 0)
						{
							RemoveAt(owner.numItems - 1);
						}
						owner.AllowUpdates();
					}
				}
		bool IList.Contains(Object value)
				{
					MenuItem item = (value as MenuItem);
					if(item != null)
					{
						return Contains(item);
					}
					else
					{
						return false;
					}
				}
		int IList.IndexOf(Object value)
				{
					MenuItem item = (value as MenuItem);
					if(item != null)
					{
						return IndexOf(item);
					}
					else
					{
						return -1;
					}
				}
		void IList.Insert(int index, Object value)
				{
					MenuItem item = (value as MenuItem);
					if(item != null)
					{
						Add(index, item);
					}
					else
					{
						throw new ArgumentException
							(S._("SWF_InvalidMenuItem"), "value");
					}
				}
		void IList.Remove(Object value)
				{
					MenuItem item = (value as MenuItem);
					if(item != null)
					{
						Remove(item);
					}
				}
		public virtual void RemoveAt(int index)
				{
					if(index < 0 || index >= owner.numItems)
					{
						throw new ArgumentOutOfRangeException
							("index", S._("SWF_MenuItemIndex"));
					}
					else
					{
						owner.SuppressUpdates();
						owner.itemList[index].parent = null;
						MenuItem[] items = new MenuItem [owner.numItems - 1];
						Array.Copy(owner.itemList, 0, items, 0, index);
						Array.Copy(owner.itemList, index + 1, items, index,
								   owner.numItems - index - 1);
						owner.itemList = items;
						--(owner.numItems);
						owner.AllowUpdates();
					}
				}
		bool IList.IsFixedSize
				{
					get
					{
						return false;
					}
				}
		bool IList.IsReadOnly
				{
					get
					{
						return false;
					}
				}
		Object IList.this[int index]
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

		// Implement the IEnumerable interface.
		public IEnumerator GetEnumerator()
				{
					return owner.itemList.GetEnumerator();
				}

		// Add a menu item to this menu.
		public virtual int Add(MenuItem item)
				{
					return Add(owner.numItems, item);
				}
		public virtual int Add(int index, MenuItem item)
				{
					if(item == null)
					{
						throw new ArgumentNullException("item");
					}
					else if(item.parent != null)
					{
						throw new ArgumentException
							(S._("SWF_ItemAlreadyInUse"), "item");
					}
					if(index < 0 || index > owner.numItems)
					{
						throw new ArgumentOutOfRangeException
							("index", S._("SWF_MenuItemIndex"));
					}
					owner.SuppressUpdates();
					MenuItem[] items = new MenuItem [owner.numItems + 1];
					Array.Copy(owner.itemList, 0, items, 0, index);
					items[index] = item;
					item.parent = owner;
					if(index < owner.numItems)
					{
						Array.Copy(owner.itemList, index, items, index + 1,
								   owner.numItems - index);
					}
					owner.itemList = items;
					++(owner.numItems);
					owner.AllowUpdates();
					return index;
				}
		public virtual MenuItem Add(String caption)
				{
					MenuItem item = new MenuItem(caption);
					Add(item);
					return item;
				}
		public virtual MenuItem Add(String caption, EventHandler onClick)
				{
					MenuItem item = new MenuItem(caption, onClick);
					Add(item);
					return item;
				}
		public virtual MenuItem Add(String caption, MenuItem[] items)
				{
					MenuItem item = new MenuItem(caption, items);
					Add(item);
					return item;
				}

		// Add a range of menu items to this menu.
		public virtual void AddRange(MenuItem[] items)
				{
					if(items == null)
					{
						throw new ArgumentNullException("items");
					}
					owner.SuppressUpdates();
					foreach(MenuItem item in items)
					{
						Add(owner.numItems, item);
					}
					owner.AllowUpdates();
				}

		// Determine if this menu contains a particular item
		public bool Contains(MenuItem item)
				{
					foreach(MenuItem i in owner.itemList)
					{
						if(i == item)
						{
							return true;
						}
					}
					return false;
				}

		// Get the index of a specific menu item.
		public int IndexOf(MenuItem item)
				{
					int index;
					for(index = 0; index < owner.numItems; ++index)
					{
						if(owner.itemList[index] == item)
						{
							return index;
						}
					}
					return -1;
				}

		// Remove a specific menu item.
		public virtual void Remove(MenuItem item)
				{
					owner.SuppressUpdates();
					int index = IndexOf(item);
					if(index != -1)
					{
						owner.itemList[index].parent = null;
						MenuItem[] items = new MenuItem [owner.numItems - 1];
						Array.Copy(owner.itemList, 0, items, 0, index);
						Array.Copy(owner.itemList, index + 1, items, index,
								   owner.numItems - index - 1);
						owner.itemList = items;
						--(owner.numItems);
					}
					owner.AllowUpdates();
				}

		// Move a menu item to a new position.
		internal void Move(MenuItem item, int index)
				{
					if(index < 0 || index >= owner.numItems)
					{
						throw new ArgumentOutOfRangeException
							("index", S._("SWF_MenuItemIndex"));
					}
					int oldIndex = IndexOf(item);
					owner.SuppressUpdates();
					if(index < oldIndex)
					{
						RemoveAt(oldIndex);
						Add(index, item);
					}
					else if(index > oldIndex)
					{
						RemoveAt(oldIndex);
						Add(index - 1, item);
					}
					owner.AllowUpdates();
				}

	}; // class MenuItemCollection

}; // class Menu

}; // namespace System.Windows.Forms
