/*
 * LinkedList.cs - Generic doubly-linked list class.
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

public class LinkedList<T> : ICollection<T>, IList<T>, IIterable<T>, ICloneable
{
	// Structure of a list node.
	private class Node<T>
	{
		public T       data;
		public Node<T> next;
		public Node<T> prev;

		public Node(T data)
				{
					this.data = data;
					this.next = null;
					this.prev = null;
				}

	}; // class Node

	// Internal state.
	private Node<T> first;
	private Node<T> last;
	private int     count;

	// Constructor.
	public LinkedList()
			{
				first = null;
				last = null;
				count = 0;
			}

	// Get a particular node in the list by index.
	private Node<T> Get(int index)
			{
				Node<T> current;
				if(index < 0 || index >= count)
				{
					throw new ArgumentOutOfRangeException
						("index", S._("ArgRange_Array"));
				}
				if(index <= (count / 2))
				{
					// Search forwards from the start of the list.
					current = first;
					while(index > 0)
					{
						current = current.next;
						--index;
					}
					return current;
				}
				else
				{
					// Search backwards from the end of the list.
					current = last;
					++index;
					while(index < count)
					{
						current = current.prev;
						++index;
					}
					return current;
				}
			}

	// Remove a node from this list.
	private void Remove(Node<T> node)
			{
				if(node.next != null)
				{
					node.next.prev = node.prev;
				}
				else
				{
					last = node.prev;
				}
				if(node.prev != null)
				{
					node.prev.next = node.next;
				}
				else
				{
					first = node.next;
				}
				--count;
			}

	// Insert a data item before a specific node.
	private void InsertBefore(Node<T> node, T value)
			{
				Node<T> newNode = new Node<T>(value);
				newNode.next = node;
				newNode.prev = node.prev;
				node.prev = newNode;
				if(newNode.prev == null)
				{
					first = newNode;
				}
				++count;
			}

	// Implement the ICollection<T> interface.
	public virtual void CopyTo(T[] array, int index)
			{
				IIterator<T> iterator = GetIterator();
				while(iterator.MoveNext())
				{
					array[index++] = iterator.Current;
				}
			}
	public virtual int Count
			{
				get
				{
					return count;
				}
			}
	public virtual bool IsSynchronized
			{
				get
				{
					return false;
				}
			}
	public virtual Object SyncRoot
			{
				get
				{
					return this;
				}
			}

	// Implement the IList<T> interface.
	public virtual int Add(T value)
			{
				int index = count;
				AddLast(value);
				return index;
			}
	public virtual void Clear()
			{
				first = null;
				last = null;
				count = 0;
			}
	public virtual bool Contains(T value)
			{
				Node<T> current = first;
				if(typeof(T).IsValueType)
				{
					while(current != null)
					{
						if(value.Equals(current.data))
						{
							return true;
						}
					}
					return false;
				}
				else
				{
					if(((Object)value) != null)
					{
						while(current != null)
						{
							if(value.Equals(current.data))
							{
								return true;
							}
						}
						return false;
					}
					else
					{
						while(current != null)
						{
							if(((Object)(current.data)) == null)
							{
								return true;
							}
						}
						return false;
					}
				}
			}
	public virtual IListIterator<T> GetIterator()
			{
				return new ListIterator<T>(this);
			}
	public virtual int IndexOf(T value)
			{
				int index = 0;
				Node<T> current = first;
				if(typeof(T).IsValueType)
				{
					while(current != null)
					{
						if(value.Equals(current.data))
						{
							return index;
						}
						++index;
					}
					return -1;
				}
				else
				{
					if(((Object)value) != null)
					{
						while(current != null)
						{
							if(value.Equals(current.data))
							{
								return index;
							}
							++index;
						}
						return -1;
					}
					else
					{
						while(current != null)
						{
							if(((Object)(current.data)) == null)
							{
								return index;
							}
							++index;
						}
						return -1;
					}
				}
			}
	public virtual void Insert(int index, T value)
			{
				if(index == count)
				{
					AddLast(value);
				}
				else
				{
					Node<T> current = Get(index);
					InsertBefore(current, value);
				}
			}
	public virtual void Remove(T value)
			{
				Node<T> current = first;
				if(typeof(T).IsValueType)
				{
					while(current != null)
					{
						if(value.Equals(current.data))
						{
							Remove(current);
							return;
						}
					}
				}
				else
				{
					if(((Object)value) != null)
					{
						while(current != null)
						{
							if(value.Equals(current.data))
							{
								Remove(current);
								return;
							}
						}
					}
					else
					{
						while(current != null)
						{
							if(((Object)(current.data)) == null)
							{
								Remove(current);
								return;
							}
						}
					}
				}
			}
	public virtual void RemoveAt(int index)
			{
				Remove(Get(index));
			}
	public virtual bool IsFixedSize
			{
				get
				{
					return false;
				}
			}
	public virtual bool IsReadOnly
			{
				get
				{
					return false;
				}
			}
	public virtual T this[int index]
			{
				get
				{
					return Get(index).data;
				}
				set
				{
					Get(index).data = value;
				}
			}

	// Implement the IIterable<T> interface.
	IIterator<T> IIterator<T>.GetIterator()
			{
				return GetIterator();
			}

	// Implement the ICloneable interface.
	public virtual Object Clone()
			{
				LinkedList<T> clone = new LinkedList<T>();
				IIterator<T> e = GetIterator();
				while(e.MoveNext())
				{
					clone.AddLast(e.Current);
				}
				return clone;
			}

	// Add a data item to the end of this list.
	public virtual void AddLast(T data)
			{
				Node<T> node = new Node<T>(item);
				node.prev = last;
				if(last != null)
				{
					last.next = node;
				}
				else
				{
					first = node;
				}
				last = node;
				++count;
			}

	// Add a data item to the front of this list.
	public virtual void AddFirst(T data)
			{
				Node<T> node = new Node<T>(item);
				node.next = first;
				if(first != null)
				{
					first.prev = node;
				}
				else
				{
					last = node;
				}
				first = node;
				++count;
			}

	// Remove the last data item in the list.
	public virtual T RemoveLast()
			{
				if(last != null)
				{
					Node<T> node = last;
					if(node.prev != null)
					{
						node.prev.next = null;
					}
					else
					{
						first = null;
					}
					last = node.prev;
					--count;
					return node.data;
				}
				else
				{
					throw new InvalidOperationException
						(S._("Invalid_EmptyList"));
				}
			}

	// Remove the first data item in the list.
	public virtual T RemoveFirst()
			{
				if(first != null)
				{
					Node<T> node = first;
					if(node.next != null)
					{
						node.next.prev = null;
					}
					else
					{
						last = null;
					}
					first = node.next;
					--count;
					return node.data;
				}
				else
				{
					throw new InvalidOperationException
						(S._("Invalid_EmptyList"));
				}
			}

	// Remove a data item from the list (from the front).
	public T Remove()
			{
				return RemoveFirst();
			}

	// Wrap a list to make it synchronized.
	public static LinkedList<T> Synchronized<T>(LinkedList<T> list)
			{
				if(list == null)
				{
					throw new ArgumentNullException("list");
				}
				return new SynchronizedList<T>(list);
			}

	// Wrap a list to make it read-only.
	public static LinkedList<T> ReadOnly<T>(LinkedList<T> list)
			{
				if(list == null)
				{
					throw new ArgumentNullException("list");
				}
				return new ReadOnlyList<T>(list);
			}

	// Iterator class for lists.
	private class ListIterator<T> : IListIterator<T>
	{
		// Internal state, accessible to "LinkedList<T>".
		public LinkedList<T> list;
		public Node<T> posn;
		public int     index;
		public bool    reset;
		public bool    removed;

		// Constructor.
		public ListIterator(LinkedList<T> list)
				{
					this.list = list;
					this.posn = null;
					this.index = -1;
					this.reset = true;
					this.removed = false;
				}

		// Implement the IIterator<T> interface.
		public bool MoveNext()
				{
					if(reset)
					{
						posn = list.first;
						index = 0;
						reset = false;
					}
					else if(posn != null)
					{
						posn = posn.next;
						if(!removed)
						{
							++index;
						}
					}
					removed = false;
					return (posn != null);
				}
		public void Reset()
				{
					posn = null;
					index = -1;
					reset = true;
					removed = false;
				}
		public void Remove()
				{
					if(posn == null || removed)
					{
						throw new InvalidOperationException
							(S._("Invalid_BadIteratorPosition"));
					}
					list.Remove(posn);
					removed = true;
				}
		T IIterator<T>.Current
				{
					get
					{
						if(posn == null || removed)
						{
							throw new InvalidOperationException
								(S._("Invalid_BadIteratorPosition"));
						}
						return posn.data;
					}
				}

		// Implement the IListIterator<T> interface.
		public bool MovePrev()
				{
					if(reset)
					{
						posn = list.last;
						index = list.count - 1;
						reset = false;
					}
					else if(posn != null)
					{
						posn = posn.prev;
						--index;
					}
					return (posn != null);
				}
		public int Position
				{
					get
					{
						if(posn == null || removed)
						{
							throw new InvalidOperationException
								(S._("Invalid_BadIteratorPosition"));
						}
						return index;
					}
				}
		public T Current
				{
					get
					{
						if(posn == null || removed)
						{
							throw new InvalidOperationException
								(S._("Invalid_BadIteratorPosition"));
						}
						return posn.data;
					}
					set
					{
						if(posn == null || removed)
						{
							throw new InvalidOperationException
								(S._("Invalid_BadIteratorPosition"));
						}
						posn.data = value;
					}
				}

	}; // class ListIterator<T>

	// Wrapper class for synchronized lists.
	private sealed class SynchronizedList<T> : IList<T>
	{
		// Internal state.
		private LinkedList<T> list;

		// Constructor.
		public SynchronizedList(LinkedList<T> list)
				{
					this.list = list;
				}

		// Override the parent's API, wrapping everything in a lock.
		public override void CopyTo(T[] array, int index)
				{
					lock(SyncRoot)
					{
						list.CopyTo(array, index);
					}
				}
		public override int Count
				{
					get
					{
						lock(SyncRoot)
						{
							return list.Count;
						}
					}
				}
		public override bool IsSynchronized
				{
					get
					{
						return true;
					}
				}
		public override Object SyncRoot
				{
					get
					{
						return list.SyncRoot;
					}
				}
		public override int Add(T value)
				{
					lock(SyncRoot)
					{
						return list.Add(value);
					}
				}
		public override void Clear()
				{
					lock(SyncRoot)
					{
						list.Clear();
					}
				}
		public override bool Contains(T value)
				{
					lock(SyncRoot)
					{
						return list.Contains(value);
					}
				}
		public override IListIterator<T> GetIterator()
				{
					lock(SyncRoot)
					{
						return new SynchronizedListIterator
							(list.GetIterator());
					}
				}
		public override int IndexOf(T value)
				{
					lock(SyncRoot)
					{
						return list.IndexOf(value);
					}
				}
		public override void Insert(int index, T value)
				{
					lock(SyncRoot)
					{
						list.Insert(index, value);
					}
				}
		public override void Remove(T value)
				{
					lock(SyncRoot)
					{
						list.Remove(value);
					}
				}
		public override void RemoveAt(int index)
				{
					lock(SyncRoot)
					{
						list.RemoveAt(index);
					}
				}
		public override bool IsFixedSize
				{
					get
					{
						lock(SyncRoot)
						{
							return list.IsFixedSize;
						}
					}
				}
		public override bool IsReadOnly
				{
					get
					{
						lock(SyncRoot)
						{
							return list.IsReadOnly;
						}
					}
				}
		public override T this[int index]
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
		public override Object Clone()
				{
					lock(SyncRoot)
					{
						return new SynchronizedList<T>
							((LinkedList<T>)(list.Clone()));
					}
				}
		public override void AddLast(T data)
				{
					lock(SyncRoot)
					{
						list.AddLast(data);
					}
				}
		public override void AddFirst(T data)
				{
					lock(SyncRoot)
					{
						list.AddFirst(data);
					}
				}
		public override T RemoveLast()
				{
					lock(SyncRoot)
					{
						return list.RemoveLast();
					}
				}
		public override T RemoveFirst()
				{
					lock(SyncRoot)
					{
						return list.RemoveFirst();
					}
				}

	}; // class SynchronizedList<T>

	// Wrapper class for read-only lists.
	private sealed class ReadOnlyList<T> : IList<T>
	{
		// Internal state.
		private LinkedList<T> list;

		// Constructor.
		public ReadOnlyList(LinkedList<T> list)
				{
					this.list = list;
				}

		// Override the parent's API, stubbing non-read methods.
		public override void CopyTo(T[] array, int index)
				{
					list.CopyTo(array, index);
				}
		public override int Count
				{
					get
					{
						return list.Count;
					}
				}
		public override bool IsSynchronized
				{
					get
					{
						return list.IsSynchronized;
					}
				}
		public override Object SyncRoot
				{
					get
					{
						return list.SyncRoot;
					}
				}
		public override int Add(T value)
				{
					throw new InvalidOperationException
						(S._("NotSupp_ReadOnly"));
				}
		public override void Clear()
				{
					throw new InvalidOperationException
						(S._("NotSupp_ReadOnly"));
				}
		public override bool Contains(T value)
				{
					return list.Contains(value);
				}
		public override IListIterator<T> GetIterator()
				{
					return new ReadOnlyListIterator(list.GetIterator());
				}
		public override int IndexOf(T value)
				{
					return list.IndexOf(value);
				}
		public override void Insert(int index, T value)
				{
					throw new InvalidOperationException
						(S._("NotSupp_ReadOnly"));
				}
		public override void Remove(T value)
				{
					throw new InvalidOperationException
						(S._("NotSupp_ReadOnly"));
				}
		public override void RemoveAt(int index)
				{
					throw new InvalidOperationException
						(S._("NotSupp_ReadOnly"));
				}
		public override bool IsFixedSize
				{
					get
					{
						return list.IsFixedSize;
					}
				}
		public override bool IsReadOnly
				{
					get
					{
						return true;
					}
				}
		public override T this[int index]
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
		public override Object Clone()
				{
					lock(SyncRoot)
					{
						return new ReadOnlyList<T>
							((LinkedList<T>)(list.Clone()));
					}
				}
		public override void AddLast(T data)
				{
					throw new InvalidOperationException
						(S._("NotSupp_ReadOnly"));
				}
		public override void AddFirst(T data)
				{
					throw new InvalidOperationException
						(S._("NotSupp_ReadOnly"));
				}
		public override T RemoveLast()
				{
					throw new InvalidOperationException
						(S._("NotSupp_ReadOnly"));
				}
		public override T RemoveFirst()
				{
					throw new InvalidOperationException
						(S._("NotSupp_ReadOnly"));
				}

	}; // class ReadOnlyList<T>

}; // class LinkedList<T>

}; // namespace Generics
