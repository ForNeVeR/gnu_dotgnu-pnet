/*
 * ReadOnlyList.cs - Wrap a list to make it read-only.
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

public class ReadOnlyList<T> : ReadOnlyCollection<T>, IList<T>
{
	// Internal state.
	protected IList<T> list;

	// Constructor.
	public ReadOnlyList(IList<T> list) : base(list)
			{
				this.list = list;
			}

	// Implement the IList<T> interface.
	public int Add(T value)
			{
				throw new InvalidOperationException(S._("NotSupp_ReadOnly"));
			}
	public void Clear()
			{
				throw new InvalidOperationException(S._("NotSupp_ReadOnly"));
			}
	public bool Contains(T value)
			{
				return list.Contains(value);
			}
	public new IListIterator<T> GetIterator()
			{
				return new ReadOnlyListIterator<T>(list.GetIterator());
			}
	public int IndexOf(T value)
			{
				return list.IndexOf(value);
			}
	public void Insert(int index, T value)
			{
				throw new InvalidOperationException(S._("NotSupp_ReadOnly"));
			}
	public void Remove(T value)
			{
				throw new InvalidOperationException(S._("NotSupp_ReadOnly"));
			}
	public void RemoveAt(int index)
			{
				throw new InvalidOperationException(S._("NotSupp_ReadOnly"));
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
					throw new InvalidOperationException
						(S._("NotSupp_ReadOnly"));
				}
			}

	// Implement the ICloneable interface.
	public override Object Clone()
			{
				if(list is ICloneable)
				{
					return new ReadOnlyList<T>
						((IList<T>)(((ICloneable)list).Clone()));
				}
				else
				{
					throw new InvalidOperationException
						(S._("Invalid_NotCloneable"));
				}
			}

	// Read-only list iterator class.
	private sealed class ReadOnlyListIterator<T> : IListIterator<T>
	{
		// Internal state.
		protected IListIterator<T> iterator;

		// Constructor.
		public ReadOnlyListIterator(IListIterator<T> iterator)
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
						(S._("NotSupp_ReadOnly"));
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
						throw new InvalidOperationException
							(S._("NotSupp_ReadOnly"));
					}
				}

	}; // class ReadOnlyListIterator<T>

}; // class ReadOnlyList<T>

}; // namespace Generics
