/*
 * Control.cs - Implementation of the
 *                      "System.Windows.Forms.StatusBarPanelCollection" 
 *                      class.
 *
 * Copyright (C) 2003  Southern Storm Software, Pty Ltd.
 *
 * Contributions from Simon Guindon
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
using System.Collections;

namespace System.Windows.Forms
{
	public class StatusBarPanelCollection : IList, ICollection, IEnumerable	
	{
		private StatusBar owner;
		private ArrayList list;

		public StatusBarPanelCollection(StatusBar owner)
		{
			this.owner = owner;
			list = new ArrayList();
		}

		// Implement the ICollection interface.
		void ICollection.CopyTo(Array array, int index)
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

		// Get the array list that underlies this collection
		Object IList.this[int index]
		{
			get { return List[index]; }
			set { List[index] = value; }
		}

		public StatusBarPanel this[int index]
		{
			get { return (StatusBarPanel)List[index]; }
			set { List[index] = value; }
		}
	
			protected virtual ArrayList List
		{
			get { return list; }
		}
		
		int IList.Add(Object value)
		{
			int result;
			result =  List.Add(value);
			owner.Invalidate();
			return result;
		}

		public virtual int Add(StatusBarPanel value)
		{
			return List.Add(value);
		}

		public virtual StatusBarPanel Add(string text)
		{
			StatusBarPanel panel = new StatusBarPanel();
			panel.Text = text;
			List.Add(panel);
			return panel;
		}

		public virtual void AddRange(StatusBarPanel[] panels)
		{
			List.AddRange(panels);
			owner.Invalidate();
		}

		public virtual void Clear()
		{
			List.Clear();
			owner.Invalidate();
		}

		bool IList.Contains(Object value)
		{
			return List.Contains(value);
		}

		public bool Contains(StatusBarPanel panel)
		{
			return List.Contains(panel);
		}

		int IList.IndexOf(Object value)
		{
			return List.IndexOf(value);
		}

		public int IndexOf(StatusBarPanel panel)
		{
			return List.IndexOf(panel);
		}

		void IList.Insert(int index, Object value)
		{
			List.Insert(index, value);
			owner.Invalidate();
		}

		public virtual void Insert(int index, StatusBarPanel value)
		{
			List.Insert(index, value);
		}

		void IList.Remove(Object value)
		{
			List.Remove(value);
			owner.Invalidate();
		}

		public virtual void Remove(StatusBarPanel value)
		{
			List.Remove(value);
		}

		void IList.RemoveAt(int index)
		{
			List.RemoveAt(index);
		}
	}
}
