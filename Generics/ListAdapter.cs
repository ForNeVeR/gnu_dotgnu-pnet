/*
 * ListAdapter.cs - Adapt a generic list into a non-generic one.
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

public sealed class ListAdapter<T>
	: System.Collections.IList, System.Collections.ICollection,
	  System.Collections.IEnumerable
{

	// Internal state.
	private IList<T> list;

	// Constructor.
	public ListAdapter(IList<T> list)
			{
				if(list == null)
				{
					throw new ArgumentNullException("list");
				}
				this.list = list;
			}

	// Determine if a value is type-compatible with the generic list.
	private bool CheckType(Object value)
			{
				if(typeof(T).IsValueType)
				{
					if(value == null || !(value is T))
					{
						return false;
					}
				}
				else
				{
					if(value != null && !(value is T))
					{
						return false;
					}
				}
				return true;
			}

	// Check type compatibility and throw an exception if invalid.
	private void CheckTypeAndThrow(Object value)
			{
				if(!CheckType(value))
				{
					// The value is not type-compatible with the list.
					throw new InvalidOperationException
						(S._("Invalid_ValueType"));
				}
			}

	// Implement the non-generic IList interface.
	public int Add(Object value)
			{
				CheckTypeAndThrow(value);
				return list.Add((T)value);
			}
	public void Clear()
			{
				list.Clear();
			}
	public bool Contains(Object value)
			{
				if(CheckType(value))
				{
					return list.Contains((T)value);
				}
				else
				{
					return false;
				}
			}
	public int IndexOf(Object value)
			{
				if(CheckType(value))
				{
					return list.IndexOf((T)value);
				}
				else
				{
					return -1;
				}
			}
	public void Insert(int index, Object value)
			{
				CheckTypeAndThrow(value);
				list.Insert(index, (T)value);
			}
	public void Remove(Object value)
			{
				if(CheckType(value))
				{
					list.Remove((T)value);
				}
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
					CheckTypeAndThrow(value);
					list[index] = (T)value;
				}
			}

	// Implement the non-generic ICollection interface.
	public void CopyTo(Array array, int index)
			{
				IIterator<T> iterator = list.GetIterator();
				while(iterator.MoveNext())
				{
					array.SetValue(iterator.Current, index++);
				}
			}
	public int Count
			{
				get
				{
					return list.Count;
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

	// Implement the non-generic IEnumerable interface.
	public System.Collections.IEnumerator GetEnumerator()
			{
				return new EnumeratorAdapter<T>(list.GetIterator());
			}

}; // class ListAdapter<T>

}; // namespace Generics
