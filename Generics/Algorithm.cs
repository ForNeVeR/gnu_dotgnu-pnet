/*
 * Algorithm.cs - Useful generic algorithms.
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

public class Algorithm
{

	// Swap two items.
	public static void Swap<T>(ref T x, ref T y)
			{
				T temp = x;
				x = y;
				y = temp;
			}

	// Swap two items, defined by iterator positions.
	public static void Swap<T>(IIterator<T> iter1, IIterator<T> iter2)
			{
				T temp = iter1.Current;
				iter1.Current = iter2.Current;
				iter2.Current = temp;
			}

	// Determine the minimum of two items.
	public static T Min<T>(T x, T y)
			{
				if(x < y)
				{
					return x;
				}
				else
				{
					return y;
				}
			}
	public static T Min<T>(T x, T y, IComparer<T> cmp)
			{
				if(cmp.Compare(x, y) < 0)
				{
					return x;
				}
				else
				{
					return y;
				}
			}

	// Determine the maximum of two items.
	public static T Max<T>(T x, T y)
			{
				if(x > y)
				{
					return x;
				}
				else
				{
					return y;
				}
			}
	public static T Max<T>(T x, T y, IComparer<T> cmp)
			{
				if(cmp.Compare(x, y) > 0)
				{
					return x;
				}
				else
				{
					return y;
				}
			}

	// Determine the median of three values.
	public static T Median<T>(T x, T y, T z)
			{
				if(x < y)
				{
					if(y < z)
					{
						return y;
					}
					else if(x < z)
					{
						return z;
					}
					else
					{
						return x;
					}
				}
				else if(x < z)
				{
					return x;
				}
				else if(y < z)
				{
					return z;
				}
				else
				{
					return y;
				}
			}
	public static T Median<T>(T x, T y, T z, IComparer<T> cmp)
			{
				if(cmp.Compare(x, y) < 0)
				{
					if(cmp.Compare(y, z) < 0)
					{
						return y;
					}
					else if(cmp.Compare(x, z) < 0)
					{
						return z;
					}
					else
					{
						return x;
					}
				}
				else if(cmp.Compare(x, z) < 0)
				{
					return x;
				}
				else if(cmp.Compare(y, z) < 0)
				{
					return z;
				}
				else
				{
					return y;
				}
			}

	// Determine if the content of two enumerations are equal.
	public static bool Equals<T>(IEnumerator<T> e1, IEnumerator<T> e2)
			{
				for(;;)
				{
					if(!e1.MoveNext())
					{
						return !(e2.MoveNext());
					}
					if(!e2.MoveNext())
					{
						return false;
					}
					if(e1.Current != e2.Current)
					{
						return false;
					}
				}
			}
	public static bool Equals<T>(IEnumerator<T> e1, IEnumerator<T> e2,
								 IComparer<T> cmp)
			{
				for(;;)
				{
					if(!e1.MoveNext())
					{
						return !(e2.MoveNext());
					}
					if(!e2.MoveNext())
					{
						return false;
					}
					if(cmp.Compare(e1.Current, e2.Current) != 0)
					{
						return false;
					}
				}
			}

	// Determine if the content of two enumerations are not equal.
	public static bool NotEquals<T>(IEnumerator<T> e1, IEnumerator<T> e2)
			{
				for(;;)
				{
					if(!e1.MoveNext())
					{
						return e2.MoveNext();
					}
					if(!e2.MoveNext())
					{
						return true;
					}
					if(e1.Current == e2.Current)
					{
						return false;
					}
				}
			}
	public static bool NotEquals<T>(IEnumerator<T> e1, IEnumerator<T> e2,
								    IComparer<T> cmp)
			{
				for(;;)
				{
					if(!e1.MoveNext())
					{
						return e2.MoveNext();
					}
					if(!e2.MoveNext())
					{
						return true;
					}
					if(cmp.Compare(e1.Current, e2.Current) == 0)
					{
						return false;
					}
				}
			}

	// Determine if two collections are equal.
	public static bool Equals<T>(ICollection<T> c1, ICollection<T> c2)
			{
				return Equals<T>(c1.GetEnumerator(), c2.GetEnumerator());
			}
	public static bool Equals<T>(ICollection<T> c1, ICollection<T> c2,
								 IComparer<T> cmp)
			{
				return Equals<T>(c1.GetEnumerator(), c2.GetEnumerator(), cmp);
			}

	// Determine if two collections are not equal.
	public static bool NotEquals<T>(ICollection<T> c1, ICollection<T> c2)
			{
				return NotEquals<T>(c1.GetEnumerator(), c2.GetEnumerator());
			}
	public static bool NotEquals<T>(ICollection<T> c1, ICollection<T> c2,
								    IComparer<T> cmp)
			{
				return NotEquals<T>(c1.GetEnumerator(),
									c2.GetEnumerator(), cmp);
			}

	// Find a particular value in an enumeration, beginning with
	// the next item.  The enumerator will be positioned on the
	// found value, or the end of the enumeration.
	public static bool Find<T>(IEnumerator<T> e, T value)
			{
				while(e.MoveNext())
				{
					if(e.Current == value)
					{
						return true;
					}
				}
				return false;
			}
	public static bool Find<T>(IEnumerator<T> e, T value, IComparer<T> cmp)
			{
				while(e.MoveNext())
				{
					if(cmp.Compare(e.Current, value) == 0)
					{
						return true;
					}
				}
				return false;
			}

	// Find the position within an enumeration that satisfies a
	// particular predicate condition.
	public static bool Find<T>(IEnumerator<T> e, Predicate<T> pred)
			{
				while(e.MoveNext())
				{
					if(pred(e.Current))
					{
						return true;
					}
				}
				return false;
			}

}; // class Algorithm

}; // namespace Generics
