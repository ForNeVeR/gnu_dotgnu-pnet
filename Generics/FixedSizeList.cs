/*
 * FixedSizeList.cs - Wrap a list to make it fixed-size.
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

public class FixedSizeList<T> : FixedSizeCollection<T>, IList<T>, IRandomAccess
{
	// Internal state.
	protected IList<T> list;

	// Constructor.
	public FixedSizeList(IList<T> list) : base(list)
			{
				this.list = list;
			}

	// Implement the IList<T> interface.
	public int Add(T value)
			{
				throw new InvalidOperationException
					(S._("NotSupp_FixedSizeCollection"));
			}
	public void Clear()
			{
				throw new InvalidOperationException
					(S._("NotSupp_FixedSizeCollection"));
			}
	public bool Contains(T value)
			{
				return list.Contains(value);
			}
	public new IListIterator<T> GetIterator()
			{
				return new FixedSizeListIterator<T>(list.GetIterator());
			}
	public int IndexOf(T value)
			{
				return list.IndexOf(value);
			}
	public void Insert(int index, T value)
			{
				throw new InvalidOperationException
					(S._("NotSupp_FixedSizeCollection"));
			}
	public void Remove(T value)
			{
				throw new InvalidOperationException
					(S._("NotSupp_FixedSizeCollection"));
			}
	public void RemoveAt(int index)
			{
				throw new InvalidOperationException
					(S._("NotSupp_FixedSizeCollection"));
			}
	public bool IsRandomAccess
			{
				get
				{
					return list.IsRandomAccess;
				}
			}
	public T this[int index]
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

	// Implement the ICloneable interface.
	public override Object Clone()
			{
				if(list is ICloneable)
				{
					return new FixedSizeList<T>
						((IList<T>)(((ICloneable)list).Clone()));
				}
				else
				{
					throw new InvalidOperationException
						(S._("Invalid_NotCloneable"));
				}
			}

	// Fixed-size list iterator.
	private sealed class FixedSizeListIterator<T> : IListIterator<T>
	{
		// Internal state.
		protected IListIterator<T> iterator;

		// Constructor.
		public FixedSizeListIterator(IListIterator<T> iterator)
				{
					this.iterator = iterator;
				}

		// Implement the IIterator<T> interface.
		public bool MoveNext()
				{
					return iterator.MoveNext();
				}
		public void Reset()
				{
					iterator.Reset();
				}
		public void Remove()
				{
					throw new InvalidOperationException
						(S._("NotSupp_FixedSizeCollection"));
				}
		T IIterator<T>.Current
				{
					get
					{
						return ((IIterator<T>)iterator).Current;
					}
				}

		// Implement the IListIterator<T> interface.
		public bool MovePrev()
				{
					return iterator.MovePrev();
				}
		public int Position
				{
					get
					{
						return iterator.Position;
					}
				}
		public T Current
				{
					get
					{
						return iterator.Current;
					}
					set
					{
						iterator.Current = value;
					}
				}

	}; // class FixedSizeListIterator<T>

}; // class FixedSizeList<T>

}; // namespace Generics
