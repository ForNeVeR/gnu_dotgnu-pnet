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

public sealed class ListWrapper<T> : IList<T>, ICollection<T>
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
	public IListIterator<T> GetIterator()
			{
				return new ListIterator<T>(list);
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
	public bool IsRandomAccess
			{
				get
				{
					// Recognise ArrayList specially as random-access.
					return (list is System.Collections.ArrayList);
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

	// Implement the IIterable<T> interface.
	IIterator<T> IIterable<T>.GetIterator()
			{
				return new ListIterator<T>(list);
			}

	// Private list iterator implementation.
	private sealed class ListIterator<T> : IListIterator<T>
	{
		// Internal state.
		private IList list;
		private int posn;
		private int removed;
		private bool reset;

		// Constructor.
		public ListIterator(IList list)
				{
					this.list = list;
					posn = -1;
					removed = -1;
					reset = true;
				}

		// Implement the IIterator<T> interface.
		public bool MoveNext()
				{
					if(reset)
					{
						// Start at the beginning of the list.
						posn = 0;
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
						++posn;
					}
					return (posn < list.Count);
				}
		public void Reset()
				{
					posn = -1;
					removed = -1;
					reset = true;
				}
		public void Remove()
				{
					if(posn >= 0 && posn < list.Count)
					{
						list.RemoveAt(posn);
						removed = posn;
					}
					else
					{
						throw new InvalidOperationException
							(S._("Invalid_BadIteratorPosition"));
					}
				}
		T IIterator<T>.Current
				{
					get
					{
						if(posn >= 0 && posn < list.Count && removed == -1)
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

		// Implement the IListIterator<T> interface.
		public bool MovePrev()
				{
					if(reset)
					{
						// Start at the end of the list.
						posn = list.Count - 1;
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
						--posn;
					}
					return (posn >= 0);
				}
		public int Position
				{
					get
					{
						if(posn >= 0 && posn < list.Count && removed == -1)
						{
							return posn;
						}
						else
						{
							throw new InvalidOperationException
								(S._("Invalid_BadIteratorPosition"));
						}
					}
				}
		public T Current
				{
					get
					{
						if(posn >= 0 && posn < list.Count && removed == -1)
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
						if(posn >= 0 && posn < list.Count && removed == -1)
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
