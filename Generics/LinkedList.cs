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

public class LinkedList<T>
	: IDeque<T>, IQueue<T>, IStack<T>, IList<T>, ICloneable
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

	// Implement the IDeque<T> interface.
	public virtual void PushFront(T value)
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
	public virtual void PushBack(T value)
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
	public virtual T PopFront()
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
	public virtual T PopBack()
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
	public virtual T PeekFront()
			{
				if(first != null)
				{
					return first.data;
				}
				else
				{
					throw new InvalidOperationException
						(S._("Invalid_EmptyList"));
				}
			}
	public virtual T PeekEnd()
			{
				if(last != null)
				{
					return last.data;
				}
				else
				{
					throw new InvalidOperationException
						(S._("Invalid_EmptyList"));
				}
			}
	public virtual T[] ToArray()
			{
				T[] array = new T [Count];
				CopyTo(array, 0);
				return array;
			}
	bool IDeque<T>.IsFixedSize
			{
				get
				{
					return IsFixedSize;
				}
			}
	bool IDeque<T>.IsReadOnly
			{
				get
				{
					return IsReadOnly;
				}
			}

	// Implement the IQueue<T> interface privately.
	void IQueue<T>.Enqueue(T value)
			{
				PushBack(value);
			}
	T IQueue<T>.Dequeue()
			{
				return PopFront();
			}
	T IQueue<T>.Peek()
			{
				return PeekFront();
			}
	T[] IQueue<T>.ToArray()
			{
				return ToArray();
			}
	bool IQueue<T>.IsFixedSize
			{
				get
				{
					return IsFixedSize;
				}
			}
	bool IQueue<T>.IsReadOnly
			{
				get
				{
					return IsReadOnly;
				}
			}

	// Implement the IStack<T> interface privately.
	void IStack<T>.Push(T value)
			{
				PushFront(value);
			}
	T IStack<T>.Pop()
			{
				return PopFront();
			}
	T IStack<T>.Peek()
			{
				return PeekFront();
			}
	T[] IStack<T>.ToArray()
			{
				return ToArray();
			}
	bool IStack<T>.IsFixedSize
			{
				get
				{
					return IsFixedSize;
				}
			}
	bool IStack<T>.IsReadOnly
			{
				get
				{
					return IsReadOnly;
				}
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
				PushBack(value);
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
						current = current.next;
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
							current = current.next;
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
							current = current.next;
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
						current = current.next;
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
							current = current.next;
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
							current = current.next;
						}
						return -1;
					}
				}
			}
	public virtual void Insert(int index, T value)
			{
				if(index == count)
				{
					PushBack(value);
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
						current = current.next;
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
							current = current.next;
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
							current = current.next;
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
	public virtual bool IsRandomAccess
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
					clone.PushBack(e.Current);
				}
				return clone;
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

}; // class LinkedList<T>

}; // namespace Generics
