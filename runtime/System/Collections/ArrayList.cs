/*
 * ArrayList.cs - Implementation of the "System.Collections.ArrayList" class.
 *
 * Copyright (C) 2001  Southern Storm Software, Pty Ltd.
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

namespace System.Collections
{

using System;

public class ArrayList : ICloneable, ICollection, IEnumerable, IList
{
	private int count;
	private Object[] store;
	private int generation;

	// Simple constructors.
	public ArrayList()
	{
		count = 0;
		store = new Object [16];
		generation = 0;
	}
	public ArrayList(int capacity)
	{
		if(capacity < 0)
		{
			throw new ArgumentOutOfRangeException
				("capacity",
				 Environment.GetEnvironmentString("ArgRange_NonNegative"));
		}
		count = 0;
		store = new Object [capacity];
		generation = 0;
	}

	// Construct an array list from the contents of a collection.
	public ArrayList(ICollection c)
	{
		IEnumerator enumerator;
		int index;
		if(c == null)
		{
			throw new ArgumentNullException("c");
		}
		count = c.Count;
		store = new Object [count];
		enumerator = c.GetEnumerator();
		for(index = 0; enumerator.MoveNext(); ++index)
		{
			store[index] = enumerator.Current;
		}
		generation = 0;
	}

	// Reallocate the array to accomodate "n" new entries at "index".
	// This leaves "count" unchanged".
	private void Realloc(int n, int index)
	{
		if(!(store.Length))
		{
			store = new Object[16];
		}
	}

	// Shift elements to accomodate "n" new entries at "index".
	// This leaves "count" unchanged".
	private void Shift(int n, int index)
	{
	}

	// Delete "n" entries from the list at "index".
	// This modifies "count".
	private void Delete(int n, int index)
	{
		while((index + n) < count)
		{
			store[index] = store[index + n];
			++index;
		}
		count -= n;
	}

	// Implement the IList interface.
	public int Add(Object value)
	{
		if(count >= store.Length)
		{
			Realloc(1, count);
		}
		store[count] = value;
		++generation;
		return count++;
	}
	public void Clear()
	{
		Array.Clear(store, 0, count);
		count = 0;
		++generation;
	}
	public bool Contains(Object item)
	{
		int index;
		if(item != null)
		{
			for(index = 0; index < count; ++index)
			{
				if(item.Equals(store[index]))
				{
					return true;
				}
			}
			return false;
		}
		else
		{
			for(index = 0; index < count; ++index)
			{
				if(store[index] == null)
				{
					return true;
				}
			}
			return false;
		}
	}
	public int IndexOf(Object value)
	{
		return Array.IndexOf(store, value, 0, count);
	}
	public void Insert(int index, Object value)
	{
		if(index < 0 || index > count)
		{
			throw new ArgumentOutOfRangeException
				("index", _("ArgRange_Array"));
		}
		if(count >= store.Length)
		{
			Realloc(1, index);
		}
		else
		{
			Shift(1, index);
		}
		store[index] = value;
		++generation;
		return count++;
	}
	public void Remove(Object value)
	{
		int index = Array.IndexOf(store, value, 0, count);
		if(index != -1)
		{
			Delete(1, index);
			++generation;
		}
	}
	public void RemoveAt(int index)
	{
		if(index < 0 || index > count)
		{
			throw new ArgumentOutOfRangeException
				("index", _("ArgRange_Array"));
		}
		Delete(1, index);
		++generation;
	}
	public bool IsFixedSize
	{
		get
		{
			return false;
		}
	}
	public bool IsReadOnly
	{
		get
		{
			return false;
		}
	}
	public Object this[int index]
	{
		get
		{
			if(index < 0 || index >= count)
			{
				throw new ArgumentOutOfRangeException
					("index", _("ArgRange_Array"));
			}
			return store[index];
		}
		set
		{
			if(index < 0 || index >= count)
			{
				throw new ArgumentOutOfRangeException
					("index", _("ArgRange_Array"));
			}
			store[index] = value;
			++generation;
		}
	}

	// Add the contents of a collection as a range.
	public virtual void AddRange(ICollection c)
	{
		int cCount;
		IEnumerator enumerator;
		if(c == null)
		{
			throw new ArgumentNullException("c");
		}
		cCount = c.Count;
		if((count + cCount) > store.Length)
		{
			Realloc(cCount, count);
		}
		enumerator = c.GetEnumerator();
		while(enumerator.MoveNext())
		{
			store[count++] = enumerator.Current;
		}
		++generation;
	}

	// Insert the contents of a collection as a range.
	public virtual void InsertRange(ICollection c, int index)
	{
		int cCount;
		IEnumerator enumerator;
		if(c == null)
		{
			throw new ArgumentNullException("c");
		}
		if(index < 0 || index > count)
		{
			throw new ArgumentOutOfRangeException
				("index", _("ArgRange_Array"));
		}
		cCount = c.Count;
		if((count + cCount) > store.Length)
		{
			Realloc(cCount, index);
		}
		else
		{
			Shift(cCount, index);
		}
		enumerator = c.GetEnumerator();
		while(enumerator.MoveNext())
		{
			store[index++] = enumerator.Current;
		}
		count += cCount;
		++generation;
	}

	// Remove a range of elements from an array list.
	public virtual void RemoveRange(int index, int count)
	{
		if(index < 0)
		{
			throw new ArgumentOutOfRangeException
				("index", _("ArgRange_Array"));
		}
		if(count < 0)
		{
			throw new ArgumentOutOfRangeException
				("count", _("ArgRange_Array"));
		}
		if((this.count - index) < count)
		{
			throw new ArgumentException(_("Arg_InvalidArrayRange"));
		}
		Delete(count, index);
		++generation;
	}

	// Perform a binary search on an array list.
	public virtual int BinarySearch(Object value)
	{
		return Array.BinarySearch(store, 0, count, value);
	}
	public virtual int BinarySearch(Object value, IComparer comparer)
	{
		return Array.BinarySearch(store, 0, count, value, comparer);
	}
	public virtual int BinarySearch(int index, int count,
								    Object value, IComparer comparer)
	{
		if(index < 0)
		{
			throw new ArgumentOutOfRangeException
				("index", _("ArgRange_Array"));
		}
		if(count < 0)
		{
			throw new ArgumentOutOfRangeException
				("count", _("ArgRange_Array"));
		}
		if((this.count - index) < count)
		{
			throw new ArgumentException(_("Arg_InvalidArrayRange"));
		}
		return Array.BinarySearch(store, index, count, value, comparer);
	}

	// Implement the ICloneable interface.
	public Object Clone()
	{
		ArrayList clone = new ArrayList(count);
		clone.count = count;
		clone.generation = generation;
		Array.Copy(store, 0, clone.store, 0, count);
		return clone;
	}

	// Implement the ICollection interface.
	public void CopyTo(Array array, int arrayIndex)
	{
		Array.Copy(store, 0, array, arrayIndex, count);
	}
	public int Count
	{
		get
		{
			return count;
		}
	}
	public bool IsSynchronized
	{
		get
		{
			return false;
		}
	}
	public Object SyncRoot
	{
		get
		{
			return this;
		}
	}

	// Copy from this array list to another array.
	public virtual void CopyTo(Array array)
	{
		Array.Copy(store, 0, array, 0, count);
	}
	public virtual void CopyTo(int index, Array array,
							   int arrayIndex, int count)
	{
		if(index < 0)
		{
			throw new ArgumentOutOfRangeException
				("index", _("ArgRange_Array"));
		}
		if(count < 0)
		{
			throw new ArgumentOutOfRangeException
				("count", _("ArgRange_Array"));
		}
		if((this.count - index) < count)
		{
			throw new ArgumentException(_("Arg_InvalidArrayRange"));
		}
		Array.Copy(store, index, array, arrayIndex, count);
	}

	// Get the index of a value within an array list.
	public virtual int IndexOf(Object value, int startIndex)
	{
		if(startIndex < 0 || startIndex >= count)
		{
			throw new ArgumentOutOfRangeException
				("startIndex", _("ArgRange_Array"));
		}
		return Array.IndexOf(store, value, startIndex, count - startIndex);
	}
	public virtual int IndexOf(Object value, int startIndex, int count)
	{
		if(startIndex < 0 || startIndex >= this.count)
		{
			throw new ArgumentOutOfRangeException
				("startIndex", _("ArgRange_Array"));
		}
		if(count < 0)
		{
			throw new ArgumentOutOfRangeException
				("count", _("ArgRange_Array"));
		}
		if((this.count - startIndex) < count)
		{
			throw new ArgumentException(_("Arg_InvalidArrayRange"));
		}
		return Array.IndexOf(store, value, startIndex, count);
	}

	// Get the last index of a value within an array list.
	public virtual int LastIndexOf(Object value)
	{
		if(count > 0)
		{
			return Array.LastIndexOf(store, value, count - 1, count);
		}
		else
		{
			return -1;
		}
	}
	public virtual int LastIndexOf(Object value, int startIndex)
	{
		if(startIndex < 0 || startIndex >= count)
		{
			throw new ArgumentOutOfRangeException
				("startIndex", _("ArgRange_Array"));
		}
		return Array.LastIndexOf(store, value, startIndex);
	}
	public virtual int LastIndexOf(Object value, int startIndex, int count)
	{
		if(startIndex < 0 || startIndex >= this.count)
		{
			throw new ArgumentOutOfRangeException
				("startIndex", _("ArgRange_Array"));
		}
		if(count < 0)
		{
			throw new ArgumentOutOfRangeException
				("count", _("ArgRange_Array"));
		}
		if(count > (startIndex + 1))
		{
			throw new ArgumentException(_("Arg_InvalidArrayRange"));
		}
		return Array.LastIndexOf(store, value, startIndex, count);
	}

	// Construct an array list with repeated copies of the same element.
	public static ArrayList Repeat(Object value, int count)
	{
		ArrayList list;
		int index;
		if(count < 0)
		{
			throw new ArgumentOutOfRangeException
				("count", _("ArgRange_NonNegative"));
		}
		if(count < 16)
		{
			list = new ArrayList();
		}
		else
		{
			list = new ArrayList(count);
		}
		list.Realloc(count, 0);
		for(index = 0; index < count; ++index)
		{
			list.store[index] = value;
		}
		list.count = count;
		return list;
	}

	// Reverse the contents of this array list.
	public virtual void Reverse()
	{
		Array.Reverse(store, 0, count);
		++generation;
	}
	public virtual void Reverse(int index, int count)
	{
		if(index < 0)
		{
			throw new ArgumentOutOfRangeException
				("index", _("ArgRange_Array"));
		}
		if(count < 0)
		{
			throw new ArgumentOutOfRangeException
				("count", _("ArgRange_Array"));
		}
		if((this.count - index) < count)
		{
			throw new ArgumentException(_("Arg_InvalidArrayRange"));
		}
		Array.Reverse(store, index, count);
		++generation;
	}

	// Set a range of array list elements to the members of a collection.
	public virtual void SetRange(int index, ICollection c)
	{
		int count;
		IEnumerator enumerator;
		if(c == null)
		{
			throw new ArgumentNullException("c");
		}
		if(index < 0)
		{
			throw new ArgumentOutOfRangeException
				("index", _("ArgRange_Array"));
		}
		count = c.Count;
		if((this.count - index) < count)
		{
			throw new ArgumentException(_("Arg_InvalidArrayRange"));
		}
		enumerator = c.GetEnumerator();
		while(enumerator.MoveNext())
		{
			store[index++] = enumerator.Current;
		}
		++generation;
	}

	// Sort the contents of this array list.
	public virtual void Sort()
	{
		Array.Sort(store, 0, count);
		++generation;
	}
	public virtual void Sort(IComparer comparer)
	{
		Array.Sort(store, 0, count, comparer);
		++generation;
	}
	public virtual void Sort(int index, int count, IComparer comparer)
	{
		if(index < 0)
		{
			throw new ArgumentOutOfRangeException
				("index", _("ArgRange_Array"));
		}
		if(count < 0)
		{
			throw new ArgumentOutOfRangeException
				("count", _("ArgRange_Array"));
		}
		if((this.count - index) < count)
		{
			throw new ArgumentException(_("Arg_InvalidArrayRange"));
		}
		Array.Sort(store, index, count, comparer);
		++generation;
	}

	// Create an array that contains the elements of this array list.
	public virtual Object[] ToArray()
	{
		Object[] array = new Object[count];
		int index;
		for(index = 0; index < count; ++index)
		{
			array[index] = store[index];
		}
		return array;
	}
	public virtual Array ToArray(Type type)
	{
		Array array = Array.CreateInstance(type, count);
		int index;
		for(index = 0; index < count; ++index)
		{
			array.SetValue(store[index], index);
		}
		return array;
	}

	// Trim the array list to its actual size.
	public virtual void TrimToSize()
	{
		if(count != 0)
		{
			if(count != store.Length)
			{
				Object[] newStore = new Object[count];
				int index;
				for(index = 0; index < count; ++index)
				{
					newStore[index] = store[index];
				}
				store = newStore;
			}
		}
		else if(store.Length != 16)
		{
			store = new Object[16];
		}
		++generation;
	}

	// Get or set the current capacity of the array list.
	public virtual int Capacity
	{
		get
		{
			return store.Length;
		}
		set
		{
			if(value <= 0)
			{
				value = 16;
			}
			if(value < count)
			{
				throw new ArgumentOutOfRangeException
					("value", _("Arg_CannotReduceCapacity"));
			}
			if(value != store.Length)
			{
				Object[] newStore = new Object[value];
				int index;
				for(index = 0; index < count; ++index)
				{
					newStore[index] = store[index];
				}
				store = newStore;
			}
			++generation;
		}
	}

	// Get an enumerator for this array list.
	public IEnumerator GetEnumerator()
	{
		return new ArrayListEnumerator(this, 0, count);
	}
	public virtual IEnumerator GetEnumerator(int index, int count)
	{
		if(index < 0)
		{
			throw new ArgumentOutOfRangeException
				("index", _("ArgRange_Array"));
		}
		if(count < 0)
		{
			throw new ArgumentOutOfRangeException
				("count", _("ArgRange_Array"));
		}
		if((this.count - index) < count)
		{
			throw new ArgumentException(_("Arg_InvalidArrayRange"));
		}
		return new ArrayListEnumerator(this, index, count);
	}

	// Array list enumerator class.
	private class ArrayListEnumerator : IEnumerator
	{
		private ArrayList list;
		private int start;
		private int finish;
		private int position;
		private int generation;

		public ArrayListEnumerator(ArrayList list, int start, int finish)
		{
			this.list = list;
			this.start = start;
			this.finish = finish;
			position = start - 1;
			generation = list.generation;
		}

		public bool MoveNext()
		{
			if(generation != list.generation)
			{
				throw new InvalidOperationException
					(_("Invalid_CollectionModified"));
			}
			++position;
			return (position <= finish);
		}

		public void Reset()
		{
			position = start - 1;
		}

		public Object Current
		{
			get
			{
				if(generation != list.generation)
				{
					throw new InvalidOperationException
						(_("Invalid_CollectionModified"));
				}
				else if(position < 0)
				{
					throw new InvalidOperationException
						(_("Invalid_BadEnumeratorPosition"));
				}
				return list.store[position];
			}
		}
	}

	// Adapt an IList to appear to look like an ArrayList.
	public static ArrayList Adapter(IList list)
	{
		if(list == null)
		{
			throw new ArgumentNullException("list");
		}
		return new IListWrapper(list);
	}

	// Wrapper class for IList.
	private class IListWrapper : ArrayList
	{
	} // class IListWrapper

	// Pass-through wrapper class that encapsulates another array list.
	private class PassThroughWrapper : ArrayList
	{
		protected ArrayList list;

		public PassThroughWrapper(ArrayList list)
		{
			this.list = list;
		}

		// Implement the IList interface.
		public int Add(Object value)
		{
			return list.Add(value);
		}
		public void Clear()
		{
			list.Clear();
		}
		public bool Contains(Object item)
		{
			return list.Contains(item);
		}
		public int IndexOf(Object value)
		{
			return list.IndexOf(value);
		}
		public void Insert(int index, Object value)
		{
			list.Insert(index, value);
		}
		public void Remove(Object value)
		{
			list.Remove(value);
		}
		public void RemoveAt(int index)
		{
			list.RemoveAt(index);
		}
		public bool IsFixedSize
		{
			get
			{
				return list.IsFixedSize;
			}
		}
		public bool IsReadOnly
		{
			get
			{
				return list.IsReadOnly;
			}
		}
		public Object this[int index]
		{
			get
			{
				return list[index];
			}
			set
			{
				list[index] = value;
			}
		}

		// Range-related methods.
		public override void AddRange(ICollection c)
		{
			list.AddRange(c);
		}
		public override void InsertRange(ICollection c, int index)
		{
			list.InsertRange(c, index);
		}
		public override void RemoveRange(int index, int count)
		{
			list.RemoveRange(index, count);
		}
		public override void SetRange(int index, ICollection c)
		{
			list.SetRange(index, c);
		}

		// Searching methods.
		public override int BinarySearch(Object value)
		{
			return list.BinarySearch(value);
		}
		public override int BinarySearch(Object value, IComparer comparer)
		{
			return list.BinarySearch(value, comparer);
		}
		public override int BinarySearch(int index, int count,
									    Object value, IComparer comparer)
		{
			return list.BinarySearch(index, count, value, comparer);
		}
		public override int IndexOf(Object value, int startIndex)
		{
			return list.IndexOf(value, startIndex);
		}
		public override int IndexOf(Object value, int startIndex, int count)
		{
			return list.IndexOf(value, startIndex, count);
		}
		public override int LastIndexOf(Object value)
		{
			return list.LastIndexOf(value);
		}
		public override int LastIndexOf(Object value, int startIndex)
		{
			return list.LastIndexOf(value, startIndex);
		}
		public override int LastIndexOf(Object value, int startIndex, int count)
		{
			return list.LastIndexOf(value, startIndex, count);
		}

		// Implement the ICollection interface.
		public void CopyTo(Array array, int arrayIndex)
		{
			return list.CopyTo(array, arrayIndex);
		}
		public int Count
		{
			get
			{
				return list.count;
			}
		}
		public bool IsSynchronized
		{
			get
			{
				return list.IsSynchronized;
			}
		}
		public Object SyncRoot
		{
			get
			{
				return list.SyncRoot;
			}
		}

		// Copy from this array list to another array.
		public override void CopyTo(Array array)
		{
			list.CopyTo(array);
		}
		public override void CopyTo(int index, Array array,
							   	   int arrayIndex, int count)
		{
			list.CopyTo(index, array, arrayIndex, count);
		}

		// Reverse the contents of this array list.
		public override void Reverse()
		{
			list.Reverse();
		}
		public override void Reverse(int index, int count)
		{
			list.Reverse(index, count);
		}

		// Sort the contents of this array list.
		public override void Sort()
		{
			list.Sort();
		}
		public override void Sort(IComparer comparer)
		{
			list.Sort(comparer);
		}
		public override void Sort(int index, int count, IComparer comparer)
		{
			list.Sort(index, count, comparer);
		}

		// Create an array that contains the elements of this array list.
		public override Object[] ToArray()
		{
			return list.ToArray();
		}
		public override Array ToArray(Type type)
		{
			return list.ToArray(type);
		}

		// Trim the array list to its actual size.
		public override void TrimToSize()
		{
			list.TrimToSize();
		}

		// Get or set the current capacity of the array list.
		public override int Capacity
		{
			get
			{
				return list.Capacity;
			}
			set
			{
				list.Capacity = value;
			}
		}

		// Get an enumerator for this array list.
		public IEnumerator GetEnumerator()
		{
			return list.GetEnumerator();
		}
		public override IEnumerator GetEnumerator(int index, int count)
		{
			return list.GetEnumerator(index, count);
		}

	} // class PassThroughWrapper

	// Adapt an array list to appear to have a fixed size.
	public static ArrayList FixedSize(ArrayList list)
	{
		if(list == null)
		{
			throw new ArgumentNullException("list");
		}
		return new FixedSizeWrapper(list);
	}

	// Wrapper class for fixed size lists.
	private class FixedSizeWrapper : PassThroughWrapper
	{
		public FixedSizeWrapper(ArrayList list)
			: base(list)
		{
		}

		// Implement the IList interface.
		public int Add(Object value)
		{
			throw new NotSupportedException(_("NotSupp_FixedSizeCollection"));
		}
		public void Clear()
		{
			throw new NotSupportedException(_("NotSupp_FixedSizeCollection"));
		}
		public void Insert(int index, Object value)
		{
			throw new NotSupportedException(_("NotSupp_FixedSizeCollection"));
		}
		public void Remove(Object value)
		{
			throw new NotSupportedException(_("NotSupp_FixedSizeCollection"));
		}
		public void RemoveAt(int index)
		{
			throw new NotSupportedException(_("NotSupp_FixedSizeCollection"));
		}
		public bool IsFixedSize
		{
			get
			{
				return true;
			}
		}
		public bool IsReadOnly
		{
			get
			{
				return list.IsReadOnly;
			}
		}
		public Object this[int index]
		{
			get
			{
				return list[index];
			}
			set
			{
				list[index] = value;
			}
		}

		// Range-related methods.
		public override void AddRange(ICollection c)
		{
			throw new NotSupportedException(_("NotSupp_FixedSizeCollection"));
		}
		public override void InsertRange(ICollection c, int index)
		{
			throw new NotSupportedException(_("NotSupp_FixedSizeCollection"));
		}
		public override void RemoveRange(int index, int count)
		{
			throw new NotSupportedException(_("NotSupp_FixedSizeCollection"));
		}

		// Implement the ICloneable interface.
		public Object Clone()
		{
			return new FixedSizeWrapper((ArrayList)(list.Clone()));
		}

		// Trim the array list to its actual size.
		public override void TrimToSize()
		{
			throw new NotSupportedException(_("NotSupp_FixedSizeCollection"));
		}

	} // class FixedSizeWrapper

	// Adapt an array list to get access to a sub-range.
	public virtual ArrayList GetRange(int index, int count)
	{
		if(index < 0)
		{
			throw new ArgumentOutOfRangeException
				("index", _("ArgRange_Array"));
		}
		if(count < 0)
		{
			throw new ArgumentOutOfRangeException
				("count", _("ArgRange_Array"));
		}
		if((this.count - index) < count)
		{
			throw new ArgumentException(_("Arg_InvalidArrayRange"));
		}
		return new RangeWrapper(this, index, count);
	}

	// Wrapper class for sub-range array lists.
	private class RangeWrapper : ArrayList
	{
		private int index;
		private int count;

		public RangeWrapper(ArrayList list, int index, int count)
			: base(list)
		{
			this.index = index;
			this.count = count;
			generation = list.generation;
		}

		// Determine if the underlying array list has been changed.
		private void UnderlyingCheck()
		{
			if(generation != list.generation)
			{
				throw new InvalidOperationException
					(_("Invalid_UnderlyingModified"));
			}
		}

		// Implement the IList interface.
		public int Add(Object value)
		{
			UnderlyingCheck();
			list.Insert(index + count, value);
			generation = list.generation;
			return index + count;
		}
		public void Clear()
		{
			UnderlyingCheck();
			list.Clear();
			generation = list.generation;
		}
		public bool Contains(Object item)
		{
			UnderlyingCheck();
			return list.IndexOf(item);
		}
		public int IndexOf(Object value)
		{
			UnderlyingCheck();
			return list.IndexOf(value, index, count);
		}
		public void Insert(int index, Object value)
		{
			UnderlyingCheck();
			if(index < 0 || index >= count)
			{
				throw new ArgumentOutOfRangeException
					("index", _("ArgRange_Array"));
			}
			list.Insert(index + this.index, value);
			generation = list.generation;
		}
		public void Remove(Object value)
		{
			UnderlyingCheck();
			int index = list.IndexOf(value, index, count);
			if(index != -1)
			{
				list.RemoveAt(index);
			}
			generation = list.generation;
		}
		public void RemoveAt(int index)
		{
			UnderlyingCheck();
			list.RemoveAt(index + this.index);
			generation = list.generation;
		}
		public bool IsFixedSize
		{
			get
			{
				return list.IsFixedSize;
			}
		}
		public bool IsReadOnly
		{
			get
			{
				return list.IsReadOnly;
			}
		}
		public Object this[int index]
		{
			get
			{
				UnderlyingCheck();
				if(index < 0 || index >= count)
				{
					throw new ArgumentOutOfRangeException
						("index", _("ArgRange_Array"));
				}
				return list[index + this.index];
			}
			set
			{
				UnderlyingCheck();
				if(index < 0 || index >= count)
				{
					throw new ArgumentOutOfRangeException
						("index", _("ArgRange_Array"));
				}
				list[index + this.index] = value;
				generation = list.generation;
			}
		}

		// Range-related methods.
		public override void AddRange(ICollection c)
		{
			UnderlyingCheck();
			list.InsertRange(c, index + count);
			generation = list.generation;
		}
		public override void InsertRange(ICollection c, int index)
		{
			UnderlyingCheck();
			list.InsertRange(c, index + this.index);
			generation = list.generation;
		}
		public override void RemoveRange(int index, int count)
		{
			UnderlyingCheck();
			list.RemoveRange(index + this.index, count);
			generation = list.generation;
		}
		public override void SetRange(int index, ICollection c)
		{
			UnderlyingCheck();
			list.SetRange(index + this.index, c);
			generation = list.generation;
		}

		// Implement the ICloneable interface.
		public Object Clone()
		{
			return new RangeWrapper((ArrayList)(list.Clone()), index, count);
		}

		// Searching methods.
		public override int BinarySearch(Object value)
		{
			UnderlyingCheck();
			return list.BinarySearch(index, count, value, (IComparer)null);
		}
		public override int BinarySearch(Object value, IComparer comparer)
		{
			UnderlyingCheck();
			return list.BinarySearch(index, count, value, comparer);
		}
		public override int BinarySearch(int index, int count,
									     Object value, IComparer comparer)
		{
			UnderlyingCheck();
			return list.BinarySearch(index + this.index, count,
									 value, comparer);
		}
		public override int IndexOf(Object value, int startIndex)
		{
			UnderlyingCheck();
			return list.IndexOf(value, startIndex);
		}
		public override int IndexOf(Object value, int startIndex, int count)
		{
			UnderlyingCheck();
			return list.IndexOf(value, startIndex, count);
		}
		public override int LastIndexOf(Object value)
		{
			UnderlyingCheck();
			return list.LastIndexOf(value);
		}
		public override int LastIndexOf(Object value, int startIndex)
		{
			UnderlyingCheck();
			return list.LastIndexOf(value, startIndex);
		}
		public override int LastIndexOf(Object value, int startIndex, int count)
		{
			UnderlyingCheck();
			return list.LastIndexOf(value, startIndex, count);
		}

		// Implement the ICollection interface.
		public void CopyTo(Array array, int arrayIndex)
		{
			return list.CopyTo(array, arrayIndex);
		}
		public int Count
		{
			get
			{
				UnderlyingCheck();
				return count;
			}
		}
		public bool IsSynchronized
		{
			get
			{
				return list.IsSynchronized;
			}
		}
		public Object SyncRoot
		{
			get
			{
				return list.SyncRoot;
			}
		}

		// Copy from this array list to another array.
		public override void CopyTo(Array array)
		{
			list.CopyTo(array);
		}
		public override void CopyTo(int index, Array array,
							   	   int arrayIndex, int count)
		{
			list.CopyTo(index, array, arrayIndex, count);
		}

		// Reverse the contents of this array list.
		public override void Reverse()
		{
			list.Reverse();
		}
		public override void Reverse(int index, int count)
		{
			list.Reverse(index, count);
		}

		// Sort the contents of this array list.
		public override void Sort()
		{
			list.Sort();
		}
		public override void Sort(IComparer comparer)
		{
			list.Sort(comparer);
		}
		public override void Sort(int index, int count, IComparer comparer)
		{
			list.Sort(index, count, comparer);
		}

		// Create an array that contains the elements of this array list.
		public override Object[] ToArray()
		{
			return list.ToArray();
		}
		public override Array ToArray(Type type)
		{
			return list.ToArray(type);
		}

		// Trim the array list to its actual size.
		public override void TrimToSize()
		{
			list.TrimToSize();
		}

		// Get or set the current capacity of the array list.
		public override int Capacity
		{
			get
			{
				return list.Capacity;
			}
			set
			{
				list.Capacity = value;
			}
		}

		// Get an enumerator for this array list.
		public IEnumerator GetEnumerator()
		{
			return list.GetEnumerator();
		}
		public override IEnumerator GetEnumerator(int index, int count)
		{
			return list.GetEnumerator(index, count);
		}

	} // class RangeWrapper

	// Adapt an array list to appear to be read-only.
	public static ArrayList ReadOnly(ArrayList list)
	{
		if(list == null)
		{
			throw new ArgumentNullException("list");
		}
		return new ReadOnlyWrapper(list);
	}

	// Wrapper class for read-only lists.
	private class ReadOnlyWrapper : PassThroughWrapper
	{
		public ReadOnlyWrapper(ArrayList list)
			: base(list)
		{
		}

		// Implement the IList interface.
		public int Add(Object value)
		{
			throw new NotSupportedException(_("NotSupp_ReadOnly"));
		}
		public void Clear()
		{
			throw new NotSupportedException(_("NotSupp_ReadOnly"));
		}
		public void Insert(int index, Object value)
		{
			throw new NotSupportedException(_("NotSupp_ReadOnly"));
		}
		public void Remove(Object value)
		{
			throw new NotSupportedException(_("NotSupp_ReadOnly"));
		}
		public void RemoveAt(int index)
		{
			throw new NotSupportedException(_("NotSupp_ReadOnly"));
		}
		public bool IsReadOnly
		{
			get
			{
				return true;
			}
		}
		public Object this[int index]
		{
			get
			{
				return list[index];
			}
			set
			{
				throw new NotSupportedException(_("NotSupp_ReadOnly"));
			}
		}

		// Range-related methods.
		public override void AddRange(ICollection c)
		{
			throw new NotSupportedException(_("NotSupp_ReadOnly"));
		}
		public override void InsertRange(ICollection c, int index)
		{
			throw new NotSupportedException(_("NotSupp_ReadOnly"));
		}
		public override void RemoveRange(int index, int count)
		{
			throw new NotSupportedException(_("NotSupp_ReadOnly"));
		}
		public override void SetRange(int index, ICollection c)
		{
			throw new NotSupportedException(_("NotSupp_ReadOnly"));
		}

		// Implement the ICloneable interface.
		public Object Clone()
		{
			return new ReadOnlyWrapper((ArrayList)(list.Clone()));
		}

		// Reverse the contents of this array list.
		public override void Reverse()
		{
			throw new NotSupportedException(_("NotSupp_ReadOnly"));
		}
		public override void Reverse(int index, int count)
		{
			throw new NotSupportedException(_("NotSupp_ReadOnly"));
		}

		// Sort the contents of this array list.
		public override void Sort()
		{
			throw new NotSupportedException(_("NotSupp_ReadOnly"));
		}
		public override void Sort(IComparer comparer)
		{
			throw new NotSupportedException(_("NotSupp_ReadOnly"));
		}
		public override void Sort(int index, int count, IComparer comparer)
		{
			throw new NotSupportedException(_("NotSupp_ReadOnly"));
		}

		// Trim the array list to its actual size.
		public override void TrimToSize()
		{
			throw new NotSupportedException(_("NotSupp_ReadOnly"));
		}

		// Get or set the current capacity of the array list.
		public override int Capacity
		{
			get
			{
				return list.Capacity;
			}
			set
			{
				throw new NotSupportedException(_("NotSupp_ReadOnly"));
			}
		}

	} // class ReadOnlyWrapper

	// Adapt an array list to appear to be synchonrized
	public static ArrayList Synchronized(ArrayList list)
	{
		if(list == null)
		{
			throw new ArgumentNullException("list");
		}
		return new SynchronizedWrapper(list);
	}

	// Wrapper class for synchronized lists.
	private class SynchronizedWrapper : ArrayList
	{
		private ArrayList list;
		private Object syncRoot;

		public SynchronizedWrapper(ArrayList list)
		{
			this.list = list;
			syncRoot = list.SyncRoot;
		}

	} // class SynchronizedWrapper

}; // class ArrayList

}; // namespace System.Collections
