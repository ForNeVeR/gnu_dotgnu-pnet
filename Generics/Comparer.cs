/*
 * Comparer.cs - Default generic comparer implementation.
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

public sealed class Comparer<T> : IComparer<T>
{

	// Compare two items using the IComparable<T> or IComparable interfaces.
	public int Compare(T a, T b)
			{
				IComparable<T> cmp;
				System.IComparable cmp2;

				// Deal with the "null" cases if T is not a value type.
				if(!(typeof(T).IsValueType))
				{
					if(a == null)
					{
						if(b == null)
						{
							return 0;
						}
						else
						{
							return -1;
						}
					}
					else if(b == null)
					{
						return 1;
					}
				}

				// Try using the IComparable<T> interface.
				cmp = (a as IComparable<T>);
				if(cmp != null)
				{
					return cmp.CompareTo(b);
				}
				cmp = (b as IComparable<T>);
				if(cmp != null)
				{
					return -(cmp.CompareTo(a));
				}

				// Try using the System.IComparable interface.
				cmp2 = (a as System.IComparable);
				if(cmp2 != null)
				{
					return cmp2.CompareTo(b);
				}
				cmp2 = (b as System.IComparable);
				if(cmp2 != null)
				{
					return -(cmp2.CompareTo(a));
				}

				// We were unable to compare the values.
				throw new ArgumentException(_("Arg_ABMustBeComparable"));
			}

}; // class Comparer<T>

}; // namespace Generics
