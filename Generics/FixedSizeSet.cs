/*
 * FixedSizeSet.cs - Wrap a set to make it fixed-size.
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

public class FixedSizeSet<T> : FixedSizeCollection<T>, ISet<T>
{
	// Internal state.
	protected ISet<T> set;

	// Constructor.
	public FixedSizeSet(ISet<T> set) : base(set)
			{
				this.set = set;
			}

	// Implement the ISet<T> interface.
	public void Add(T value)
			{
				throw new InvalidOperationException
					(S._("NotSupp_FixedSizeCollection"));
			}
	public void Clear()
			{
				throw new InvalidOperationException
					(S._("NotSupp_FixedSizeCollection"));
			}
	public bool Contains(T value)
			{
				return set.Contains(value);
			}
	public void Remove(T value)
			{
				throw new InvalidOperationException
					(S._("NotSupp_FixedSizeCollection"));
			}

	// Implement the ICloneable interface.
	public override Object Clone()
			{
				if(set is ICloneable)
				{
					return new FixedSizeSet<T>
						((ISet<T>)(((ICloneable)set).Clone()));
				}
				else
				{
					throw new InvalidOperationException
						(S._("Invalid_NotCloneable"));
				}
			}

}; // class FixedSizeSet<T>

}; // namespace Generics
