/*
 * SynchronizedStack.cs - Wrap a stack to make it synchronized.
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

public class SynchronizedStack<T> : SynchronizedCollection<T>, IStack<T>
{
	// Internal state.
	protected IStack<T> stack;

	// Constructor.
	public SynchronizedStack(IStack<T> stack) : base(stack)
			{
				this.stack = stack;
			}

	// Implement the IStack<T> interface.
	public void Clear()
			{
				lock(SyncRoot)
				{
					stack.Clear();
				}
			}
	public bool Contains(T value)
			{
				lock(SyncRoot)
				{
					return stack.Contains(value);
				}
			}
	public void Push(T value)
			{
				lock(SyncRoot)
				{
					stack.Push(value);
				}
			}
	public T Pop()
			{
				lock(SyncRoot)
				{
					return stack.Pop();
				}
			}
	public T Peek()
			{
				lock(SyncRoot)
				{
					return stack.Peek();
				}
			}
	public T[] ToArray()
			{
				lock(SyncRoot)
				{
					return stack.ToArray();
				}
			}
	public bool IsFixedSize
			{
				get
				{
					lock(SyncRoot)
					{
						return stack.IsFixedSize;
					}
				}
			}
	public bool IsReadOnly
			{
				get
				{
					lock(SyncRoot)
					{
						return stack.IsReadOnly;
					}
				}
			}

	// Implement the ICloneable interface.
	public override Object Clone()
			{
				lock(SyncRoot)
				{
					if(stack is ICloneable)
					{
						return new SynchronizedStack<T>
							((IStack<T>)(((ICloneable)stack).Clone()));
					}
					else
					{
						throw new InvalidOperationException
							(S._("Invalid_NotCloneable"));
					}
				}
			}

}; // class SynchronizedStack<T>

}; // namespace Generics
