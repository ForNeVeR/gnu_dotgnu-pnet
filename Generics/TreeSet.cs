/*
 * TreeSet.cs - Generic tree class, implementing a set.
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

public sealed class TreeSet<T> : TreeBase<T, bool>, ISet<T>, ICloneable
{
	// Constructors.
	public TreeSet() : base(null) {}
	public TreeSet(IComparer<T> cmp) : base(cmp) {}

	// Implement the ISet<T> interface.
	public void Add(T value)
			{
				AddItem(value, true, true);
			}
	public void Clear()
			{
				ClearAllItems();
			}
	public bool Contains(T value)
			{
				return tree.ContainsItem(value);
			}
	public void Remove(T value)
			{
				tree.RemoveItem(value);
			}

	// Implement the ICollection<T> interface.
	public void CopyTo(T[] array, int index)
			{
				IIterator<T> iterator = GetIterator();
				while(iterator.MoveNext())
				{
					array[index++] = iterator.Current;
				}
			}
	public int Count
			{
				get
				{
					return count;
				}
			}
	public bool IsFixedSize
			{
				get
				{
					return false;
				}
			}
	public bool IsReadOnly
			{
				get
				{
					return false;
				}
			}
	public bool IsSynchronized
			{
				get
				{
					return false;
				}
			}
	public Object SyncRoot
			{
				get
				{
					return this;
				}
			}

	// Implement the IEnumerable<T> interface.
	public IIterator<T> GetIterator()
			{
				return new TreeSetIterator<T>(GetInOrderIterator());
			}

	// Implement the ICloneable interface.
	public Object Clone()
			{
				TreeSet<T> tree = new TreeSet<T>(cmp);
				TreeBaseIterator<T, bool> iterator = GetInOrderIterator();
				while(iterator.MoveNext())
				{
					tree.AddItem(iterator.Key, iterator.Value);
				}
				return tree;
			}

	// Iterator class that implements in-order traversal of a tree set.
	private sealed class TreeSetIterator<T> : IIterator<T>
	{
		// Internal state.
		private TreeBase.TreeBaseIterator<T, bool> iterator;

		// Constructor.
		public TreeSetIterator(TreeBase.TreeBaseIterator<KeyT, bool> iterator)
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
					iterator.Remove();
				}
		public T Current
				{
					get
					{
						return iterator.Key;
					}
				}

	}; // class TreeSetIterator<T>

}; // class TreeSet<T>

}; // namespace Generics
