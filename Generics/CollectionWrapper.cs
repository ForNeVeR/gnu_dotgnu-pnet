/*
 * CollectionWrapper.cs - Wrap a non-generic collection and make it generic.
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

public sealed class CollectionWrapper<T> : ICollection<T>
{
	// Internal state.
	private System.Collections.ICollection coll;

	// Constructor.
	public CollectionWrapper(System.Collections.ICollection coll)
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
					if(coll is System.Collections.IDictionary)
					{
						return ((System.Collections.IDictionary)coll)
							.IsFixedSize;
					}
					else if(coll is IList)
					{
						return ((System.Collections.IList)coll).IsFixedSize;
					}
					return false;
				}
			}
	public bool IsReadOnly
			{
				get
				{
					if(coll is System.Collections.IDictionary)
					{
						return ((System.Collections.IDictionary)coll)
							.IsReadOnly;
					}
					else if(coll is IList)
					{
						return ((System.Collections.IList)coll).IsReadOnly;
					}
					return false;
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
				return new EnumeratorWrapper(coll.GetEnumerator());
			}

}; // class CollectionWrapper<T>

}; // namespace Generics
