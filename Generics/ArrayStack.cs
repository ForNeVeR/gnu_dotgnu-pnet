/*
 * ArrayStack.cs - Generic stack class, implemented as an array.
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

public sealed class ArrayStack<T> : IStack<T>, ICapacity, ICloneable
{
	// Internal state.
	private T[] items;
	private int size;

	// The default capacity for stacks.
	private const int DefaultCapacity = 10;

	// Constructors.
	public ArrayStack()
			{
				items = new T [DefaultCapacity];
				size = 0;
			}
	public ArrayStack(int initialCapacity)
			{
				if(initialCapacity < 0)
				{
					throw new ArgumentOutOfRangeException
						("initialCapacity", S._("ArgRange_NonNegative"));
				}
				items = new T [initialCapacity];
				size = 0;
			}

	// Implement the ICollection<T> interface.
	public void CopyTo(T[] array, int index)
			{
				if(array == null)
				{
					throw new ArgumentNullException("array");
				}
				else if(index < 0)
				{
					throw new ArgumentOutOfRangeException
						("index", S._("ArgRange_Array"));
				}
				else if((array.Length - index) < size)
				{
					throw new ArgumentException(S._("Arg_InvalidArrayRange"));
				}
				else if(size > 0)
				{
					Array.Copy(items, 0, array, index, size);
				}
			}
	public int Count
			{
				get
				{
					return size;
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

	// Implement the ICloneable interface.
	public Object Clone()
			{
				ArrayStack<T> stack = (ArrayStack<T>)MemberwiseClone();
				stack.items = (T[])items.Clone();
				return stack;
			}

	// Implement the IIterable<T> interface.
	public IIterator<T> GetIterator()
			{
				return new StackIterator<T>(this);
			}

	// Implement the ICapacity interface.
	public int Capacity
			{
				get
				{
					return items.Length;
				}
				set
				{
					if(value < 0)
					{
						throw new ArgumentOutOfRangeException
							("value", S._("ArgRange_NonNegative"));
					}
					if(value < size)
					{
						throw new ArgumentOutOfRangeException
							("value", S._("Arg_CannotReduceCapacity"));
					}
					if(value != size)
					{
						T[] newItems = new T [value];
						Array.Copy(items, 0, newItems, 0, size);
						items = newItems;
					}
				}
			}

	// Implement the IStack<T> interface.
	public void Clear()
			{
				size = 0;
			}
	public bool Contains(T obj)
			{
				int index;
				if(typeof(T).IsValueType)
				{
					for(index = 0; index < size; ++index)
					{
						if(obj.Equals(items[index]))
						{
							return true;
						}
					}
				}
				else
				{
					for(index = 0; index < size; ++index)
					{
						if(items[index] != null && obj != null)
						{
							if(obj.Equals(items[index]))
							{
								return true;
							}
						}
						else if(items[index] == null && obj == null)
						{
							return true;
						}
					}
				}
				return false;
			}
	public void Push(T value)
			{
				if(size < items.Length)
				{
					// The stack is big enough to hold the new item.
					items[size++] = value;
				}
				else
				{
					// We need to increase the size of the stack.
					int newCapacity = items.Length * 2;
					if(newCapacity <= items.Length)
					{
						newCapacity = items.Length + 1;
					}
					T[] newItems = new T [newCapacity];
					if(size > 0)
					{
						Array.Copy(items, newItems, size);
					}
					items = newItems;
					items[size++] = value;
				}
			}
	public T Pop()
			{
				if(size > 0)
				{
					return items[--size];
				}
				else
				{
					throw new InvalidOperationException
						(S._("Invalid_EmptyStack"));
				}
			}
	public T Peek()
			{
				if(size > 0)
				{
					return items[size - 1];
				}
				else
				{
					throw new InvalidOperationException
						(S._("Invalid_EmptyStack"));
				}
			}
	public T[] ToArray()
			{
				T[] array = new T [size];
				int index;
				for(index = 0; index < size; ++index)
				{
					array[index] = items[size - index - 1];
				}
				return array;
			}

	// Private class for implementing stack iteration.
	private class StackIterator<T> : IIterator<T>
	{
		// Internal state.
		private ArrayStack<T> stack;
		private int position;

		// Constructor.
		public StackIterator(ArrayStack<T> stack)
				{
					this.stack = stack;
					position   = -1;
				}

		// Implement the IIterator<T> interface.
		public bool MoveNext()
				{
					++position;
					if(position < stack.size)
					{
						return true;
					}
					position = stack.size;
					return false;
				}
		public void Reset()
				{
					position = -1;
				}
		public void Remove()
				{
					throw new InvalidOperationException(S._("NotSupp_Remove"));
				}
		public T Current
				{
					get
					{
						if(position < 0 || position >= stack.size)
						{
							throw new InvalidOperationException
								(S._("Invalid_BadIteratorPosition"));
						}
						return stack.items[stack.size - position - 1];
					}
				}

	}; // class StackIterator<T>

}; // class ArrayStack<T>

}; // namespace Generics
