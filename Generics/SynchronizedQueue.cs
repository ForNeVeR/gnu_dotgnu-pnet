/*
 * SynchronizedQueue.cs - Wrap a queue to make it synchronized.
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

public class SynchronizedQueue<T> : SynchronizedCollection<T>, IQueue<T>
{
	// Internal state.
	protected IQueue<T> queue;

	// Constructor.
	public SynchronizedQueue(IQueue<T> queue) : base(queue)
			{
				this.queue = queue;
			}

	// Implement the IQueue<T> interface.
	public void Clear()
			{
				lock(SyncRoot)
				{
					queue.Clear();
				}
			}
	public bool Contains(T value)
			{
				lock(SyncRoot)
				{
					return queue.Contains();
				}
			}
	public void Enqueue(T value)
			{
				lock(SyncRoot)
				{
					queue.Enqueue(value);
				}
			}
	public T Dequeue()
			{
				lock(SyncRoot)
				{
					return queue.Dequeue();
				}
			}
	public T Peek()
			{
				lock(SyncRoot)
				{
					return queue.Peek();
				}
			}
	public T[] ToArray()
			{
				lock(SyncRoot)
				{
					return queue.ToArray();
				}
			}

	// Implement the ICloneable interface.
	public override Object Clone()
			{
				lock(SyncRoot)
				{
					if(queue is ICloneable)
					{
						return new SynchronizedQueue<T>
							((IQueue<T>)(((ICloneable)queue).Clone()));
					}
					else
					{
						throw new InvalidOperationException
							(S._("Invalid_NotCloneable"));
					}
				}
			}

}; // class SynchronizedQueue<T>

}; // namespace Generics
