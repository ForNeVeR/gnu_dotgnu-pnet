/*
 * EventDescriptorCollection.cs - 
 *		Implementation of "System.ComponentModel.EventDescriptorCollection" 
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

using System;
using System.Collections;

namespace System.ComponentModel
{

#if !ECMA_COMPAT

	public class EventDescriptorCollection: IEnumerable, ICollection, IList
	{
		[TODO]
		public EventDescriptorCollection(EventDescriptor[] events)
		{
			throw new NotImplementedException(".ctor");
		}

		[TODO]
		public int Add(EventDescriptor value)
		{
			throw new NotImplementedException("Add");
		}

		[TODO]
		public void Clear()
		{
			throw new NotImplementedException("Clear");
		}

		[TODO]
		public bool Contains(EventDescriptor value)
		{
			throw new NotImplementedException("Contains");
		}

		[TODO]
		public virtual EventDescriptor Find(String name, bool ignoreCase)
		{
			throw new NotImplementedException("Find");
		}

		[TODO]
		public IEnumerator GetEnumerator()
		{
			throw new NotImplementedException("GetEnumerator");
		}

		[TODO]
		public int IndexOf(EventDescriptor value)
		{
			throw new NotImplementedException("IndexOf");
		}

		[TODO]
		public void Insert(int index, EventDescriptor value)
		{
			throw new NotImplementedException("Insert");
		}

		[TODO]
		protected virtual EventDescriptorCollection InternalSort(
										IComparer comparer)
		{
			throw new NotImplementedException("InternalSort");
		}

		[TODO]
		protected virtual EventDescriptorCollection InternalSort(String[] order)
		{
			throw new NotImplementedException("InternalSort");
		}

		[TODO]
		public void Remove(EventDescriptor value)
		{
			throw new NotImplementedException("Remove");
		}

		[TODO]
		public void RemoveAt(int index)
		{
			throw new NotImplementedException("RemoveAt");
		}

		[TODO]
		public virtual EventDescriptorCollection Sort()
		{
			throw new NotImplementedException("Sort");
		}

		[TODO]
		public virtual EventDescriptorCollection Sort(IComparer comparer)
		{
			throw new NotImplementedException("Sort");
		}

		[TODO]
		public virtual EventDescriptorCollection Sort(String[] order)
		{
			throw new NotImplementedException("Sort");
		}

		[TODO]
		public virtual EventDescriptorCollection Sort(String[] order, 
													  IComparer comparer)
		{
			throw new NotImplementedException("Sort");
		}

		public static readonly EventDescriptorCollection Empty=new EventDescriptorCollection(new EventDescriptor[]{});

		[TODO]
		public int Count 
		{ 
			get
			{
				throw new NotImplementedException("Count");
			}
		}

		[TODO]
		public virtual EventDescriptor this[String name] 
		{
			get
			{
				throw new NotImplementedException("Item(string)");
			}
		}

		[TODO]
		public virtual EventDescriptor this[int index] 
		{
			get
			{
				throw new NotImplementedException("Item(int)");
			}
		}

		// IList implements

		[TODO]
		int IList.Add (Object value)
		{
			return Add((EventDescriptor) value);
		}

		[TODO]
		bool IList.Contains (Object value) 
		{
			return Contains((EventDescriptor) value);
		}

		[TODO]
		int IList.IndexOf (Object value)
		{
			return IndexOf((EventDescriptor) value);
		}

		[TODO]
		void IList.Insert (int index, Object value)
		{
			Insert(index, (EventDescriptor) value);
		}

		[TODO]
		void IList.Remove (Object value)
		{
			Remove((EventDescriptor) value);
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

		[TODO]
		Object IList.this[int index] 
		{
			get 
			{
				throw new NotImplementedException("IList[int]");
			}
			set 
			{
				throw new NotImplementedException("IList[int]");
			}
		}

		// ICollection implements

		[TODO]
		void ICollection.CopyTo (Array array, int index)
		{
			throw new NotImplementedException ("ICollection.CopyTo");
		}

		[TODO]
		bool ICollection.IsSynchronized
		{
			get 
			{
				throw new NotImplementedException("ICollection.IsSynchronized");
			}
		}

		[TODO]
		Object ICollection.SyncRoot
		{
			get 
			{
				throw new NotImplementedException("ICollection.SyncRoot");
			}
		}

	}
#endif
}//namespace
