/*
 * ListWrapper.cs - Wrap a non-generic list to turn it into a generic one.
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

public sealed class ListWrapper<T>
	: IList<T>, ICollection<T>, IEnumerable<T>, IIterable<T>
{

	// Internal state.
	private System.Collections.IList list;

	// Constructor.
	public ListWrapper(System.Collections.IList list)
			{
				if(list == null)
				{
					throw new ArgumentNullException("list");
				}
				this.list = list;
			}

	// Implement the IList<T> interface.
	public int Add(T value)
			{
				return list.Add(value);
			}
	public void Clear()
			{
				list.Clear();
			}
	public bool Contains(T value)
			{
				return list.Contains(value);
			}
	public int IndexOf(T value)
			{
				return list.IndexOf(value);
			}
	public void Insert(int index, T value)
			{
				list.Insert(index, value);
			}
	public void Remove(T value)
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
	public T this[int index]
			{
				get
				{
					return (T)(list[index]);
				}
				set
				{
					list[index] = value;
				}
			}

	// Implement the ICollection<T> interface.
	public void CopyTo(T[] array, int index)
			{
				list.CopyTo(array, index);
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

	// Implement the IEnumerable<T> interface.
	public IEnumerator<T> GetEnumerator()
			{
				return new EnumeratorWrapper<T>(list.GetEnumerator());
			}

	// Implement the IIterable<T> interface.
	public IIterator<T> GetIterator()
			{
				return new ListIterator<T>(list);
			}

	// Private list iterator implementation.
	private sealed class ListIterator<T> : IEnumerator<T>, IIterator<T>
	{
		// Internal state.
		private IList list;
		private int posn;
		private bool reset;

		// Constructor.
		public ListIterator(IList list)
				{
					this.list = list;
					posn = -1;
					reset = true;
				}

		// Implement the IEnumerator<T> interface.
		public bool MoveNext()
				{
					if(reset)
					{
						// Start at the beginning of the list.
						posn = -1;
						reset = false;
					}
					++posn;
					return (posn < list.Count);
				}
		public void Reset()
				{
					posn = -1;
					reset = true;
				}
		T IEnumertor<T>.Current
				{
					get
					{
						if(posn >= 0 && posn < list.Count)
						{
							return (T)(list[posn]);
						}
						else
						{
							throw new InvalidOperationException
								(S._("Invalid_BadIteratorPosition"));
						}
					}
				}

		// Implement the IIterator<T> interface.
		public bool MovePrev()
				{
					if(reset)
					{
						// Start at the end of the list.
						posn = list.Count;
						reset = false;
					}
					--posn;
					return (posn >= 0);
				}
		public T Current
				{
					get
					{
						if(posn >= 0 && posn < list.Count)
						{
							return (T)(list[posn]);
						}
						else
						{
							throw new InvalidOperationException
								(S._("Invalid_BadIteratorPosition"));
						}
					}
					set
					{
						if(posn >= 0 && posn < list.Count)
						{
							list[posn] = value;
						}
						else
						{
							throw new InvalidOperationException
								(S._("Invalid_BadIteratorPosition"));
						}
					}
				}

	}; // class ListIterator<T>

}; // class ListWrapper<T>

}; // namespace Generics
