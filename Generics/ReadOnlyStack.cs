/*
 * ReadOnlyStack.cs - Wrap a stack to make it read-only.
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

public class ReadOnlyStack<T> : ReadOnlyCollection<T>, IStack<T>
{
	// Internal state.
	protected IStack<T> stack;

	// Constructor.
	public ReadOnlyStack(IStack<T> stack) : base(stack)
			{
				this.stack = stack;
			}

	// Implement the IStack<T> interface.
	public void Clear()
			{
				throw new InvalidOperationException(S._("NotSupp_ReadOnly"));
			}
	public bool Contains(T value)
			{
				return stack.Contains(value);
			}
	public void Push(T value)
			{
				throw new InvalidOperationException(S._("NotSupp_ReadOnly"));
			}
	public T Pop()
			{
				throw new InvalidOperationException(S._("NotSupp_ReadOnly"));
			}
	public T Peek()
			{
				return stack.Peek();
			}
	public T[] ToArray()
			{
				return stack.ToArray();
			}

	// Implement the ICloneable interface.
	public override Object Clone()
			{
				if(stack is ICloneable)
				{
					return new ReadOnlyStack<T>
						((IStack<T>)(((ICloneable)stack).Clone()));
				}
				else
				{
					throw new InvalidOperationException
						(S._("Invalid_NotCloneable"));
				}
			}

}; // class ReadOnlyStack<T>

}; // namespace Generics
