/*
 * SynchronizedList.cs - Wrap a list to make it synchronized.
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

public class SynchronizedList<T> : SynchronizedCollection<T>, IList<T>
{
	// Internal state.
	protected IList<T> list;

	// Constructor.
	public SynchronizedList(IList<T> list) : base(list)
			{
				this.list = list;
			}

	// Implement the IList<T> interface.
	public int Add(T value)
			{
				lock(SyncRoot)
				{
					return list.Add(value);
				}
			}
	public void Clear()
			{
				lock(SyncRoot)
				{
					list.Clear();
				}
			}
	public bool Contains(T value)
			{
				lock(SyncRoot)
				{
					return list.Contains(value);
				}
			}
	public new IListIterator<T> GetIterator()
			{
				lock(SyncRoot)
				{
					return new SynchronizedListIterator<T>
						(list, list.GetIterator());
				}
			}
	public int IndexOf(T value)
			{
				lock(SyncRoot)
				{
					return list.IndexOf(value);
				}
			}
	public void Insert(int index, T value)
			{
				lock(SyncRoot)
				{
					list.Insert(index, value);
				}
			}
	public void Remove(T value)
			{
				lock(SyncRoot)
				{
					list.Remove(value);
				}
			}
	public void RemoveAt(int index)
			{
				lock(SyncRoot)
				{
					list.RemoveAt(index);
				}
			}
	public bool IsFixedSize
			{
				get
				{
					lock(SyncRoot)
					{
						return list.IsFixedSize;
					}
				}
			}
	public bool IsReadOnly
			{
				get
				{
					lock(SyncRoot)
					{
						return list.IsReadOnly;
					}
				}
			}
	public bool IsRandomAccess
			{
				get
				{
					lock(SyncRoot)
					{
						return list.IsRandomAccess;
					}
				}
			}
	public T this[int index]
			{
				get
				{
					lock(SyncRoot)
					{
						return list[index];
					}
				}
				set
				{
					lock(SyncRoot)
					{
						list[index] = value;
					}
				}
			}

	// Implement the ICloneable interface.
	public override Object Clone()
			{
				if(list is ICloneable)
				{
					return new SynchronizedList<T>
						((IList<T>)(((ICloneable)list).Clone()));
				}
				else
				{
					throw new InvalidOperationException
						(S._("Invalid_NotCloneable"));
				}
			}

	// Synchronized list iterator.
	private sealed class SynchronizedListIterator<T> : IListIterator<T>
	{
		// Internal state.
		protected IList<T> list;
		protected IListIterator<T> iterator;

		// Constructor.
		public SynchronizedListIterator
					(IList<T> list, IListIterator<T> iterator)
				{
					this.list = list;
					this.iterator = iterator;
				}

		// Implement the IIterator<T> interface.
		public bool MoveNext()
				{
					lock(list.SyncRoot)
					{
						return iterator.MoveNext();
					}
				}
		public void Reset()
				{
					lock(list.SyncRoot)
					{
						iterator.Reset();
					}
				}
		public void Remove()
				{
					lock(list.SyncRoot)
					{
						iterator.Remove();
					}
				}
		T IIterator<T>.Current
				{
					get
					{
						lock(list.SyncRoot)
						{
							return ((IIterator<T>)iterator).Current;
						}
					}
				}

		// Implement the IListIterator<T> interface.
		public bool MovePrev()
				{
					lock(list.SyncRoot)
					{
						return iterator.MovePrev();
					}
				}
		public int Position
				{
					get
					{
						lock(list.SyncRoot)
						{
							return iterator.Position;
						}
					}
				}
		public T Current
				{
					get
					{
						lock(list.SyncRoot)
						{
							return iterator.Current;
						}
					}
					set
					{
						lock(list.SyncRoot)
						{
							iterator.Current = value;
						}
					}
				}

	}; // class SynchronizedListIterator<T>

}; // class SynchronizedList<T>

}; // namespace Generics
