/*
 * ComparableWrapper.cs - Wrap a non-generic compable and make it generic.
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

public sealed class ComparableWrapper<T> : IComparable<T>
{

	// Internal state.
	private System.Collections.IComparable cmp;

	// Constructor.
	public ComparableWrapper(System.Collections.IComparable cmp)
			{
				if(cmp == null)
				{
					throw new ArgumentNullException("cmp");
				}
				this.cmp = cmp;
			}

	// Implement the IComparable<T> interface.
	public int CompareTo(T obj)
			{
				return cmp.CompareTo(obj);
			}

}; // class ComparableWrapper<T>

}; // namespace Generics
