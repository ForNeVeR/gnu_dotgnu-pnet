/*
 * FixedSizeCollection.cs - Wrap a collection to make it fixed-size.
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

public class FixedSizeCollection<T> : ICollection<T>, ICloneable
{
	// Internal state.
	protected ICollection<T> coll;

	// Constructor.
	public FixedSizeCollection(ICollection<T> coll)
			{
				if(coll == null)
				{
					throw new ArgumentNullException("coll");
				}
				this.coll = coll;
			}

	// Implement the ICollection<T> interface.
	public void CopyTo(T[] array, int index)
			{
				coll.CopyTo(array, index);
			}
	public int Count
			{
				get
				{
					return coll.Count;
				}
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
					return coll.IsReadOnly;
				}
			}
	public bool IsSynchronized
			{
				get
				{
					return coll.IsSynchronized;
				}
			}
	public Object SyncRoot
			{
				get
				{
					return coll.SyncRoot;
				}
			}

	// Implement the IIterable<T> interface.
	public IIterator<T> GetIterator()
			{
				return new FixedSizeIterator<T>(coll.GetIterator());
			}

	// Implement the ICloneable interface.
	public virtual Object Clone()
			{
				if(coll is ICloneable)
				{
					return new FixedSizeCollection<T>
						((ICollection<T>)(((ICloneable)coll).Clone()));
				}
				else
				{
					throw new InvalidOperationException
						(S._("Invalid_NotCloneable"));
				}
			}

	// Fixed-size collection iterator.
	private sealed class FixedSizeIterator<T> : IIterator<T>
	{
		// Internal state.
		protected IIterator<T> iterator;

		// Constructor.
		public FixedSizeIterator(IIterator<T> iterator)
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
		public T Current
				{
					get
					{
						return iterator.Current;
					}
				}

	}; // class FixedSizeIterator<T>

}; // class FixedSizeCollection<T>

}; // namespace Generics
