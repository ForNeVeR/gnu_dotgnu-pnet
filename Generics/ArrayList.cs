/*
 * ArrayList.cs - Generic array list class.
 *
 * Copyright (c) 2003  Southern Storm Software, Pty Ltd
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

namespace Generics
{

using System;

public class ArrayList<T> : ICollection<T>, IList<T>, ICloneable
{
	// Internal state.
	private int count;
	private T[] store;

	// Simple constructors.
	public ArrayList()
			{
				count = 0;
				store = new T [16];
			}
	public ArrayList(int capacity)
			{
				if(capacity < 0)
				{
					throw new ArgumentOutOfRangeException
						("capacity", S._("ArgRange_NonNegative"));
				}
				count = 0;
				store = new T [capacity];
			}

	// Construct an array list from the contents of a collection.
	public ArrayList(ICollection<T> c)
			{
				IIterator<T> iterator;
				int index;
				if(c == null)
				{
					throw new ArgumentNullException("c");
				}
				count = c.Count;
				store = new T [count];
				iterator = c.GetIterator();
				for(index = 0; iterator.MoveNext(); ++index)
				{
					store[index] = iterator.Current;
				}
			}

	// Reallocate the array to accomodate "n" new entries at "index".
	// This leaves "count" unchanged.
	private void Realloc(int n, int index)
			{
				if((count + n) <= store.Length)
				{
					// The current capacity is sufficient, so just
					// shift the contents of the array upwards.
					int posn = count - 1;
					while(posn >= index)
					{
						store[posn + n] = store[posn];
						--posn;
					}
				}
				else
				{
					// We need to allocate a new array.
					int newCapacity = (((count + n) + 31) & ~31);
					int newCapacity2 = count * 2;
					if(newCapacity2 > newCapacity)
					{
						newCapacity = newCapacity2;
					}
					T[] newStore = new T [newCapacity];
					if(index != 0)
					{
						Array.Copy(store, 0, newStore, 0, index);
					}
					if(count != index)
					{
						Array.Copy(store, index, newStore, index + n,
								   count - index);
					}
					store = newStore;
				}
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

	// Implement the IList<T> interface.
	public virtual int Add(T value)
			{
				if(count >= store.Length)
				{
					Realloc(1, count);
				}
				store[count] = value;
				return count++;
			}
	public virtual void Clear()
			{
				Array.Clear(store, 0, count);
				count = 0;
			}
	public virtual bool Contains(T item)
			{
				int index;
				if(typeof(T).IsValueType)
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
					if(((Object)item) != null)
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
							if(((Object)(store[index])) == null)
							{
								return true;
							}
						}
						return false;
					}
				}
			}
	public virtual int IndexOf(T value)
			{
				return IndexOf(value, 0, Count);
			}
	public virtual void Insert(int index, T value)
			{
				if(index < 0 || index > count)
				{
					throw new ArgumentOutOfRangeException
						("index", S._("ArgRange_Array"));
				}
				Realloc(1, index);
				store[index] = value;
				++count;
			}
	public virtual void Remove(T value)
			{
				int index = Array.IndexOf(store, T, 0, count);
				if(index != -1)
				{
					Delete(1, index);
				}
			}
	public virtual void RemoveAt(int index)
			{
				if(index < 0 || index > count)
				{
					throw new ArgumentOutOfRangeException
						("index", S._("ArgRange_Array"));
				}
				Delete(1, index);
			}
	public virtual bool IsRandomAccess
			{
				get
				{
					return true;
				}
			}
	public virtual T this[int index]
			{
				get
				{
					if(index < 0 || index >= count)
					{
						throw new ArgumentOutOfRangeException
							("index", S._("ArgRange_Array"));
					}
					return store[index];
				}
				set
				{
					if(index < 0 || index >= count)
					{
						throw new ArgumentOutOfRangeException
							("index", S._("ArgRange_Array"));
					}
					store[index] = value;
				}
			}

	// Add the contents of a collection as a range.
	public virtual void AddRange(ICollection<T> c)
			{
				int cCount;
				IIterator<T> iterator;
				if(c == null)
				{
					throw new ArgumentNullException("c");
				}
				cCount = c.Count;
				if((count + cCount) > store.Length)
				{
					Realloc(cCount, count);
				}
				iterator = c.GetIterator();
				while(iterator.MoveNext())
				{
					store[count++] = iterator.Current;
				}
			}

	// Insert the contents of a collection as a range.
	public virtual void InsertRange(int index, ICollection<T> c)
			{
				int cCount;
				IIterator<T> iterator;
				if(c == null)
				{
					throw new ArgumentNullException("c");
				}
				if(index < 0 || index > count)
				{
					throw new ArgumentOutOfRangeException
						("index", S._("ArgRange_Array"));
				}
				cCount = c.Count;
				Realloc(cCount, index);
				iterator = c.GetIterator();
				while(enumerator.MoveNext())
				{
					store[index++] = iterator.Current;
				}
				count += cCount;
			}

	// Remove a range of elements from an array list.
	public virtual void RemoveRange(int index, int count)
			{
				if(index < 0)
				{
					throw new ArgumentOutOfRangeException
						("index", S._("ArgRange_Array"));
				}
				if(count < 0)
				{
					throw new ArgumentOutOfRangeException
						("count", S._("ArgRange_Array"));
				}
				if((this.count - index) < count)
				{
					throw new ArgumentException(S._("Arg_InvalidArrayRange"));
				}
				Delete(count, index);
			}

	// Perform a binary search on an array list.
	public virtual int BinarySearch(T value)
			{
				return BinarySearch(0, Count, value, null);
			}
	public virtual int BinarySearch(T value, IComparer<T> comparer)
			{
				return BinarySearch(0, Count, value, comparer);
			}
	public virtual int BinarySearch(int index, int count,
								    T value, IComparer<T> comparer)
			{
				// Validate the arguments.
				if(index < 0)
				{
					throw new ArgumentOutOfRangeException
						("index", S._("ArgRange_Array"));
				}
				if(count < 0)
				{
					throw new ArgumentOutOfRangeException
						("count", S._("ArgRange_Array"));
				}
				if((Count - index) < count)
				{
					throw new ArgumentException(S._("Arg_InvalidArrayRange"));
				}

				// Perform the binary search.
				int left, right, middle, cmp;
				Object elem;
				IComparable icmp;
				left = index;
				right = index + count - 1;
				while(left <= right)
				{
					middle = (left + right) / 2;
					elem = this[middle];
					if(elem != null && value != null)
					{
						if(comparer != null)
						{
							cmp = comparer.Compare(value, elem);
						}
						else if((icmp = (elem as IComparable)) != null)
						{
							cmp = -(icmp.CompareTo(value));
						}
						else if((icmp = (value as IComparable)) != null)
						{
							cmp = icmp.CompareTo(elem);
						}
						else
						{
							throw new ArgumentException
								(_("Arg_SearchCompare"));
						}
					}
					else if(elem != null)
					{
						cmp = -1;
					}
					else if(value != null)
					{
						cmp = 1;
					}
					else
					{
						cmp = 0;
					}
					if(cmp == 0)
					{
						return middle;
					}
					else if(cmp < 0)
					{
						right = middle - 1;
					}
					else
					{
						left = middle + 1;
					}
				}
				return ~left;
			}

	// Implement the ICloneable interface.
	public virtual Object Clone()
			{
				ArrayList<T> clone = new ArrayList<T>(count);
				clone.count = count;
				Array.Copy(store, 0, clone.store, 0, count);
				return clone;
			}

	// Implement the ICollection<T> interface.
	public virtual void CopyTo(T[] array, int arrayIndex)
			{
				Array.Copy(store, 0, array, arrayIndex, count);
			}
	public virtual int Count
			{
				get
				{
					return count;
				}
			}
	public virtual bool IsFixedSize
			{
				get
				{
					return false;
				}
			}
	public virtual bool IsReadOnly
			{
				get
				{
					return false;
				}
			}
	public virtual bool IsSynchronized
			{
				get
				{
					return false;
				}
			}
	public virtual Object SyncRoot
			{
				get
				{
					return this;
				}
			}

	// Copy from this array list to another array.
	public virtual void CopyTo(T[] array)
			{
				Array.Copy(store, 0, array, 0, count);
			}
	public virtual void CopyTo(int index, T[] array,
							   int arrayIndex, int count)
			{
				// Validate the parameters.
				if(index < 0)
				{
					throw new ArgumentOutOfRangeException
						("index", S._("ArgRange_Array"));
				}
				if(count < 0)
				{
					throw new ArgumentOutOfRangeException
						("count", S._("ArgRange_Array"));
				}
				if((Count - index) < count)
				{
					throw new ArgumentException(S._("Arg_InvalidArrayRange"));
				}

				// Perform the copy.
				if(GetType() == typeof(ArrayList<T>))
				{
					// We can use a short-cut because we know that
					// the list elements are in "store".
					Array.Copy(store, index, array, arrayIndex, count);
				}
				else
				{
					// The list elements may be elsewhere.
					while(count > 0)
					{
						array.SetValue(this[index], arrayIndex);
						++index;
						++arrayIndex;
						--count;
					}
				}
			}

	// Get the index of a value within an array list.
	public virtual int IndexOf(T value, int startIndex)
			{
				int count = Count;
				if(startIndex < 0 || startIndex >= count)
				{
					throw new ArgumentOutOfRangeException
						("startIndex", S._("ArgRange_Array"));
				}
				return IndexOf(value, startIndex, count - startIndex);
			}
	public virtual int IndexOf(T value, int startIndex, int count)
			{
				// Validate the parameters.
				int thisCount = Count;
				if(startIndex < 0 || startIndex >= thisCount)
				{
					throw new ArgumentOutOfRangeException
						("startIndex", S._("ArgRange_Array"));
				}
				if(count < 0)
				{
					throw new ArgumentOutOfRangeException
						("count", S._("ArgRange_Array"));
				}
				if((thisCount - startIndex) < count)
				{
					throw new ArgumentException(S._("Arg_InvalidArrayRange"));
				}

				// Perform the search.
				Object elem;
				while(count > 0)
				{
					elem = this[startIndex];
					if(elem != null && value != null)
					{
						if(value.Equals(elem))
						{
							return startIndex;
						}
					}
					else if(elem == value && value == null)
					{
						return startIndex;
					}
					++startIndex;
					--count;
				}
				return -1;
			}

	// Get the last index of a value within an array list.
	public virtual int LastIndexOf(T value)
			{
				int count = Count;
				return LastIndexOf(value, count - 1, count);
			}
	public virtual int LastIndexOf(T value, int startIndex)
			{
				int count = Count;
				if(startIndex < 0 || startIndex >= count)
				{
					throw new ArgumentOutOfRangeException
						("startIndex", S._("ArgRange_Array"));
				}
				return LastIndexOf(value, startIndex, startIndex + 1);
			}
	public virtual int LastIndexOf(T value, int startIndex, int count)
			{
				// Validate the parameters.
				if(startIndex < 0 || startIndex >= Count)
				{
					throw new ArgumentOutOfRangeException
						("startIndex", S._("ArgRange_Array"));
				}
				if(count < 0)
				{
					throw new ArgumentOutOfRangeException
						("count", S._("ArgRange_Array"));
				}
				if(count > (startIndex + 1))
				{
					throw new ArgumentException(S._("Arg_InvalidArrayRange"));
				}

				// Perform the search.
				Object elem;
				while(count > 0)
				{
					elem = this[startIndex];
					if(elem != null && value != null)
					{
						if(value.Equals(elem))
						{
							return startIndex;
						}
					}
					else if(elem == value && value == null)
					{
						return startIndex;
					}
					--startIndex;
					--count;
				}
				return -1;
			}

	// Construct an array list with repeated copies of the same element.
	public static ArrayList<T> Repeat<T>(T value, int count)
			{
				ArrayList list;
				int index;
				if(count < 0)
				{
					throw new ArgumentOutOfRangeException
						("count", S_("ArgRange_NonNegative"));
				}
				if(count < 16)
				{
					list = new ArrayList<T>();
				}
				else
				{
					list = new ArrayList<T>(count);
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
			}
	public virtual void Reverse(int index, int count)
			{
				if(index < 0)
				{
					throw new ArgumentOutOfRangeException
						("index", S._("ArgRange_Array"));
				}
				if(count < 0)
				{
					throw new ArgumentOutOfRangeException
						("count", S._("ArgRange_Array"));
				}
				if((this.count - index) < count)
				{
					throw new ArgumentException(S._("Arg_InvalidArrayRange"));
				}
				Array.Reverse(store, index, count);
			}

	// Set a range of array list elements to the members of a collection.
	public virtual void SetRange(int index, ICollection<T> c)
			{
				int count;
				IIterator<T> iterator;
				if(c == null)
				{
					throw new ArgumentNullException("c");
				}
				if(index < 0)
				{
					throw new ArgumentOutOfRangeException
						("index", S._("ArgRange_Array"));
				}
				count = c.Count;
				if((this.count - index) < count)
				{
					throw new ArgumentException(S._("Arg_InvalidArrayRange"));
				}
				iterator = c.GetIterator();
				while(iterator.MoveNext())
				{
					store[index++] = iterator.Current;
				}
			}

	// Inner version of "Sort".
	private void InnerSort(int lower, int upper, IComparer<T> comparer)
			{
				// Temporary hack - use a dumb sort until I can figure
				// out what is wrong with the Quicksort code -- Rhys.
				int i, j, cmp;
				T valuei;
				T valuej;
				for(i = lower; i < upper; ++i)
				{
					for(j = i + 1; j <= upper; ++j)
					{
						valuei = this[i];
						valuej = this[j];
						if(comparer != null)
						{
							cmp = comparer.Compare(valuei, valuej);
						}
						else
						{
							cmp = ((IComparable)valuei).CompareTo(valuej);
						}
						if(cmp > 0)
						{
							this[i] = valuej;
							this[j] = valuei;
						}
					}
				}
			}

	// Sort the contents of this array list.
	public virtual void Sort()
			{
				InnerSort(0, Count - 1, null);
			}
	public virtual void Sort(IComparer<T> comparer)
			{
				InnerSort(0, Count - 1, comparer);
			}
	public virtual void Sort(int index, int count, IComparer<T> comparer)
			{
				if(index < 0)
				{
					throw new ArgumentOutOfRangeException
						("index", S._("ArgRange_Array"));
				}
				if(count < 0)
				{
					throw new ArgumentOutOfRangeException
						("count", S._("ArgRange_Array"));
				}
				if((Count - index) < count)
				{
					throw new ArgumentException(S._("Arg_InvalidArrayRange"));
				}
				InnerSort(index, index + count - 1, comparer);
			}

	// Create an array that contains the elements of this array list.
	public virtual T[] ToArray()
			{
				int count = Count;
				T[] array = new T[count];
				int index;
				for(index = 0; index < count; ++index)
				{
					array[index] = this[index];
				}
				return array;
			}
	public virtual Array ToArray(Type type)
			{
				int count = Count;
				Array array = Array.CreateInstance(type, count);
				int index;
				for(index = 0; index < count; ++index)
				{
					array.SetValue(this[index], index);
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
						T[] newStore = new T[count];
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
					store = new T[16];
				}
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
							("value", S._("Arg_CannotReduceCapacity"));
					}
					if(value != store.Length)
					{
						T[] newStore = new T[value];
						int index;
						for(index = 0; index < count; ++index)
						{
							newStore[index] = store[index];
						}
						store = newStore;
					}
				}
			}

	// Get an iterator for this array list.
	public virtual IListIterator<T> GetIterator()
			{
				return new ArrayListIterator<T>(this, 0, Count);
			}
	IIterator<T> IIterable<T>.GetIterator()
			{
				return GetIterator();
			}
	public virtual IListIterator<T> GetIterator(int index, int count)
			{
				if(index < 0)
				{
					throw new ArgumentOutOfRangeException
						("index", S._("ArgRange_Array"));
				}
				if(count < 0)
				{
					throw new ArgumentOutOfRangeException
						("count", S._("ArgRange_Array"));
				}
				if((Count - index) < count)
				{
					throw new ArgumentException(S._("Arg_InvalidArrayRange"));
				}
				return new ArrayListIterator<T>(this, index, index + count);
			}

	// Array list iterator class.
	private class ArrayListIterator<T> : IListIterator<T>
	{
		// Internal state.
		private ArrayList<T> list;
		private int start;
		private int finish;
		private int position;
		private int removed;
		private bool reset;

		// Constructor.
		public ArrayListIterator(ArrayList<T> list, int start, int finish)
				{
					this.list = list;
					this.start = start;
					this.finish = finish;
					position = start - 1;
					removed = -1;
					reset = true;
				}

		// Implement the IIterator<T> interface.
		public bool MoveNext()
				{
					if(reset)
					{
						// Start at the beginning of the range.
						position = start;
						reset = false;
					}
					else if(removed != -1)
					{
						// An item was removed, so re-visit this position.
						position = removed;
						removed = -1;
					}
					else
					{
						++position;
					}
					return (position < finish);
				}
		public void Reset()
				{
					reset = true;
					position = start - 1;
					removed = -1;
				}
		public void Remove()
				{
					if(position < start || position >= finish || removed != -1)
					{
						throw new InvalidOperationException
							(S._("Invalid_BadIteratorPosition"));
					}
					list.RemoveAt(position);
					--finish;
					removed = position;
				}
		T IIterator<T>.Current
				{
					get
					{
						if(position < start || position >= finish ||
						   removed != -1)
						{
							throw new InvalidOperationException
								(S._("Invalid_BadIteratorPosition"));
						}
						return list[position];
					}
				}

		// Implement the IListIterator<T> interface.
		public bool MovePrev()
				{
					if(reset)
					{
						// Start at the end of the range.
						position = finish - 1;
						reset = false;
					}
					else if(removed != -1)
					{
						// An item was removed, so move to just before it.
						position = removed - 1;
						removed = -1;
					}
					else
					{
						--position;
					}
					return (position >= start);
				}
		public int Position
				{
					get
					{
						if(position < start || position >= finish ||
						   removed != -1)
						{
							throw new InvalidOperationException
								(S._("Invalid_BadIteratorPosition"));
						}
						return position;
					}
				}
		public T Current
				{
					get
					{
						if(position < start || position >= finish ||
						   removed != -1)
						{
							throw new InvalidOperationException
								(S._("Invalid_BadIteratorPosition"));
						}
						return list[position];
					}
					set
					{
						if(position < start || position >= finish ||
						   removed != -1)
						{
							throw new InvalidOperationException
								(S._("Invalid_BadIteratorPosition"));
						}
						list[position] = value;
					}
				}

	}; // class ArrayListIterator<T>

}; // class ArrayList<T>

}; // namespace Generics
