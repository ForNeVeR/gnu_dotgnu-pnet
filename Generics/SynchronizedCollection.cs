/*
 * SynchronizedCollection.cs - Wrap a collection to make it synchronized.
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

public class SynchronizedCollection<T> : ICollection<T>, ICloneable
{
	// Internal state.
	protected ICollection<T> coll;

	// Constructors.
	public SynchronizedCollection(ICollection<T> coll)
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
				lock(SyncRoot)
				{
					coll.CopyTo(array, index);
				}
			}
	public int Count
			{
				get
				{
					lock(SyncRoot)
					{
						return coll.Count;
					}
				}
			}
	public bool IsFixedSize
			{
				get
				{
					lock(SyncRoot)
					{
						return coll.IsFixedSize;
					}
				}
			}
	public bool IsReadOnly
			{
				get
				{
					lock(SyncRoot)
					{
						return coll.IsReadOnly;
					}
				}
			}
	public bool IsSynchronized
			{
				get
				{
					return true;
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
				lock(SyncRoot)
				{
					return new SynchronizedIterator<T>
						(coll, coll.GetIterator());
				}
			}

	// Implement the ICloneable interface.
	public virtual Object Clone()
			{
				lock(SyncRoot)
				{
					if(coll is ICloneable)
					{
						return new SynchronizedCollection<T>
							((ICollection<T>)(((ICloneable)coll).Clone()));
					}
					else
					{
						throw new InvalidOperationException
							(S._("Invalid_NotCloneable"));
					}
				}
			}

	// Synchronized collection iterator.
	private sealed class SynchronizedIterator<T> : IIterator<T>
	{
		// Internal state.
		protected ICollection<T> coll;
		protected IIterator<T>   iterator;

		// Constructor.
		public SynchronizedIterator(ICollection<T> coll, IIterator<T> iterator)
			{
				this.coll = coll;
				this.iterator = iterator;
			}

		// Implement the IIterator<T> interface.
		public bool MoveNext()
				{
					lock(coll.SyncRoot)
					{
						return iterator.MoveNext();
					}
				}
		public void Reset()
				{
					lock(coll.SyncRoot)
					{
						iterator.Reset();
					}
				}
		public void Remove()
				{
					lock(coll.SyncRoot)
					{
						iterator.Remove();
					}
				}
		public T Current
				{
					get
					{
						lock(coll.SyncRoot)
						{
							return iterator.Current;
						}
					}
				}

	}; // class SynchronizedIterator<T>

}; // class SynchronizedCollection<T>

}; // namespace Generics
