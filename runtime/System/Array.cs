/*
 * Array.cs - Implementation of the "System.Array" class.
 *
 * Copyright (C) 2001, 2002  Southern Storm Software, Pty Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

namespace System
{

using System.Collections;
using System.Reflection;
using System.Runtime.CompilerServices;

public abstract class Array : ICloneable, ICollection, IEnumerable, IList
{

	// Constructor.
	protected Array()
	{
		// Nothing to do here.
	}

	// Inner version of "BinarySearch" used by the methods below
	// once the arguments have been validated.
	private static int InnerBinarySearch(Array array, int lower,
										 int upper, Object value,
										 IComparer comparer)
	{
		int left, right, middle, cmp;
		Object elem;
		IComparable icmp;
		left = lower;
		right = upper;
		while(left <= right)
		{
			middle = (left + right) / 2;
			elem = array.GetValue(middle);
			if(elem != null && value != null)
			{
				if(comparer != null)
				{
					cmp = comparer.Compare(value, elem);
				}
				else if((icmp = (elem as IComparable)) != null)
				{
					cmp = -(icmp.CompareTo(value));
				}
				else if((icmp = (value as IComparable)) != null)
				{
					cmp = icmp.CompareTo(elem);
				}
				else
				{
					throw new ArgumentException(_("Arg_SearchCompare"));
				}
			}
			else if(elem != null)
			{
				cmp = -1;
			}
			else if(value != null)
			{
				cmp = 1;
			}
			else
			{
				cmp = 0;
			}
			if(cmp == 0)
			{
				return middle;
			}
			else if(cmp < 0)
			{
				right = middle - 1;
			}
			else
			{
				left = middle + 1;
			}
		}
		return ~left;
	}

	// Perform a binary search within an array for a value.
	public static int BinarySearch(Array array, Object value)
	{
		return BinarySearch(array, value, (IComparer)null);
	}

	// Perform a binary search within an array sub-range for a value.
	public static int BinarySearch(Array array, int index, int length,
								   Object value)
	{
		return BinarySearch(array, index, length, value, (IComparer)null);
	}

	// Perform a binary search within an array for a value,
	// using a specific element comparer.
	public static int BinarySearch(Array array, Object value,
								   IComparer comparer)
	{
		if(array == null)
		{
			throw new ArgumentNullException("array");
		}
		else if(array.Rank != 1)
		{
			throw new RankException(_("Arg_RankMustBe1"));
		}
		return InnerBinarySearch(array, array.GetLowerBound(0),
								 array.GetUpperBound(0), value, comparer);
	}

	// Perform a binary search within an array sub-range for a value,
	// using a specific element comparer.
	public static int BinarySearch(Array array, int index, int length,
								   Object value, IComparer comparer)
	{
		if(array == null)
		{
			throw new ArgumentNullException("array");
		}
		else if(array.Rank != 1)
		{
			throw new RankException(_("Arg_RankMustBe1"));
		}
		else if(index < array.GetLowerBound(0))
		{
			throw new ArgumentOutOfRangeException
				("index", _("ArgRange_Array"));
		}
		else if(length < 0)
		{
			throw new ArgumentOutOfRangeException
				("length", _("ArgRange_Array"));
		}
		else if(index > array.GetUpperBound(0) ||
		        length > (array.GetUpperBound(0) - index + 1))
		{
			throw new ArgumentException(_("Arg_InvalidArrayRange"));
		}
		return InnerBinarySearch(array, index, index + length - 1,
								 value, null);
	}

	// Clear the contents of an array.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static void Clear(Array array, int index, int length);

	// Initialize the contents of an array of value types.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public void Initialize();

	// Clone this array.
	public virtual Object Clone()
	{
		return MemberwiseClone();
	}

	// Copy the contents of one array into another.
	public static void Copy(Array sourceArray, Array destinationArray,
							int length)
	{
		if(sourceArray == null)
		{
			throw new ArgumentNullException("sourceArray");
		}
		else if(destinationArray == null)
		{
			throw new ArgumentNullException("destinationArray");
		}
		Copy(sourceArray, sourceArray.GetLowerBound(0),
		     destinationArray, destinationArray.GetLowerBound(0),
			 length);
	}

	// Internal array copy method for similarly-typed arrays.
	// The engine can assume that the parameters have been validated,
	// and indicate the relative indices into the arrays.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern internal static void InternalCopy
					(Array sourceArray, int sourceIndex,
				     Array destinationArray,
					 int destinationIndex, int length);

	// Copy the contents of one array into another (general-purpose version).
	public static void Copy(Array sourceArray, int sourceIndex,
						    Array destinationArray,
						    int destinationIndex, int length)
	{
		// Validate the parameters.
		if(sourceArray == null)
		{
			throw new ArgumentNullException("sourceArray");
		}
		if(destinationArray == null)
		{
			throw new ArgumentNullException("destinationArray");
		}
		if(sourceArray.Rank != destinationArray.Rank)
		{
			throw new RankException(_("Arg_MustBeSameRank"));
		}
		int srcLower = sourceArray.GetLowerBound(0);
		int srcLength = sourceArray.Length;
		int dstLower = destinationArray.GetLowerBound(0);
		int dstLength = destinationArray.Length;
		if(sourceIndex < srcLower)
		{
			throw new ArgumentOutOfRangeException
				("sourceIndex", _("ArgRange_Array"));
		}
		if(destinationIndex < dstLower)
		{
			throw new ArgumentOutOfRangeException
				("destinationIndex", _("ArgRange_Array"));
		}
		if(length < 0)
		{
			throw new ArgumentOutOfRangeException
				("length", _("ArgRange_NonNegative"));
		}
		if((sourceIndex - srcLower) >= srcLength ||
		   (destinationIndex - dstLower) >= dstLength)
		{
			throw new ArgumentException(_("Arg_InvalidArrayRange"));
		}

		// Get the array element types.
		Type arrayType1 = sourceArray.GetType().GetElementType();
		Type arrayType2 = destinationArray.GetType().GetElementType();

		// Is this a simple array copy of the same element type?
		if(arrayType1 == arrayType2)
		{
			InternalCopy
				(sourceArray, sourceIndex - srcLower,
				 destinationArray, destinationIndex - dstLower,
				 length);
			return;
		}

		// Copy the array contents the hard way.
		if(!Convert.CanConvert(arrayType1, arrayType2))
		{
			throw new ArrayTypeMismatchException
				(_("Exception_ArrayTypeMismatch"));
		}
		int index;
		for(index = 0; index < length; ++index)
		{
			destinationArray.SetRelative
				(Convert.ConvertObject
					(sourceArray.GetRelative(sourceIndex + index), arrayType2),
				 destinationIndex + index);
		}
	}

	// Implement the ICollection interface.
	public virtual void CopyTo(Array array, int index)
	{
		if(array == null)
		{
			throw new ArgumentNullException("array");
		}
		else if(array.Rank != 1 || Rank != 1)
		{
			throw new RankException(_("Arg_RankMustBe1"));
		}
		else
		{
			Copy(this, GetLowerBound(0), array,
			     index + array.GetLowerBound(0), Length);
		}
	}
	public int Count
	{
		get
		{
			return Length;
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

	// Internal versions of "CreateInstance".
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern private static Array CreateArray(IntPtr elementType,
										    int rank, int length1,
										    int length2, int length3);
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern private static Array CreateArray(IntPtr elementType,
										    int[] lengths,
										    int[] lowerBounds);

	// Create a single-dimensional array instance.
	public static Array CreateInstance(Type elementType, int length)
	{
		IntPtr typeHandle;
		typeHandle = ClrHelpers.TypeToClrHandle(elementType, "elementType");
		if(length < 0)
		{
			throw new ArgumentOutOfRangeException
				("length", _("ArgRange_NonNegative"));
		}
		return CreateArray(typeHandle, 1, length, 0, 0);
	}

	// Create a double-dimensional array instance.
	public static Array CreateInstance(Type elementType,
									   int length1, int length2)
	{
		IntPtr typeHandle;
		typeHandle = ClrHelpers.TypeToClrHandle(elementType, "elementType");
		if(length1 < 0)
		{
			throw new ArgumentOutOfRangeException
				("length1", _("ArgRange_NonNegative"));
		}
		if(length2 < 0)
		{
			throw new ArgumentOutOfRangeException
				("length2", _("ArgRange_NonNegative"));
		}
		return CreateArray(typeHandle, 2, length1, length2, 0);
	}

	// Create a triple-dimensional array instance.
	public static Array CreateInstance(Type elementType, int length1,
									   int length2, int length3)
	{
		IntPtr typeHandle;
		typeHandle = ClrHelpers.TypeToClrHandle(elementType, "elementType");
		if(length1 < 0)
		{
			throw new ArgumentOutOfRangeException
				("length1", _("ArgRange_NonNegative"));
		}
		if(length2 < 0)
		{
			throw new ArgumentOutOfRangeException
				("length2", _("ArgRange_NonNegative"));
		}
		if(length3 < 0)
		{
			throw new ArgumentOutOfRangeException
				("length3", _("ArgRange_NonNegative"));
		}
		return CreateArray(typeHandle, 3, length1, length2, length3);
	}

	// Create an array instance from an array of length values.
	public static Array CreateInstance(Type elementType, int[] lengths)
	{
		int index;
		IntPtr typeHandle;
		typeHandle = ClrHelpers.TypeToClrHandle(elementType, "elementType");
		if(lengths == null)
		{
			throw new ArgumentNullException("lengths");
		}
		if(lengths.Length < 1)
		{
			throw new ArgumentException(_("Arg_MustHaveOneElement"));
		}
		for(index = lengths.Length - 1; index >= 0; --index)
		{
			if(lengths[index] < 0)
			{
				throw new ArgumentOutOfRangeException
					("lengths[" + (lengths[index]).ToString() + "]",
					 _("ArgRange_NonNegative"));
			}
		}
		return CreateArray(typeHandle, lengths, null);
	}

	// Create an array instance from an array of length values,
	// and an array of lower bounds.
	public static Array CreateInstance(Type elementType, int[] lengths,
									   int[] lowerBounds)
	{
		int index;
		IntPtr typeHandle;
		typeHandle = ClrHelpers.TypeToClrHandle(elementType, "elementType");
		if(lengths == null)
		{
			throw new ArgumentNullException("lengths");
		}
		if(lowerBounds == null)
		{
			throw new ArgumentNullException("lowerBounds");
		}
		if(lengths.Length != lowerBounds.Length)
		{
			throw new ArgumentException(_("Arg_MustBeSameSize"));
		}
		if(lengths.Length < 1)
		{
			throw new ArgumentException(_("Arg_MustHaveOneElement"));
		}
		for(index = lengths.Length - 1; index >= 0; --index)
		{
			if(lengths[index] < 0)
			{
				throw new ArgumentOutOfRangeException
					("lengths[" + (lengths[index]).ToString() + "]",
					 _("ArgRange_NonNegative"));
			}
		}
		return CreateArray(typeHandle, lengths, lowerBounds);
	}

	// Implement the IEnumerable interface.
	public virtual IEnumerator GetEnumerator()
	{
		int rank = Rank;
		if(rank == 1)
		{
			return new ArrayEnumerator1(this);
		}
		else if(rank == 2)
		{
			return new ArrayEnumerator2(this);
		}
		else
		{
			return new ArrayEnumeratorN(this, rank);
		}
	}

	// Private class for enumerating a single-dimensional array's contents.
	private sealed class ArrayEnumerator1 : IEnumerator
	{
		private Array array;
		private int   lower;
		private int   upper;
		private int   posn;

		// Constructor.
		public ArrayEnumerator1(Array array)
				{
					this.array = array;
					this.lower = array.GetLowerBound(0);
					this.upper = array.GetUpperBound(0);
					this.posn  = this.lower - 1;
				}

		// Move to the next element in the array.
		public bool MoveNext()
				{
					if(posn < upper)
					{
						++posn;
						return true;
					}
					else
					{
						return false;
					}
				}

		// Reset the enumerator.
		public void Reset()
				{
					posn = lower - 1;
				}

		// Get the current object.
		public Object Current
				{
					get
					{
						if(posn >= lower && posn <= upper)
						{
							return array.GetValue(posn);
						}
						else
						{
							throw new InvalidOperationException
								(_("Invalid_BadEnumeratorPosition"));
						}
					}
				}

	};

	// Private class for enumerating a double-dimensional array's contents.
	private sealed class ArrayEnumerator2 : IEnumerator
	{
		private Array array;
		private int   lower1;
		private int   lower2;
		private int   upper1;
		private int   upper2;
		private int   posn1;
		private int   posn2;

		// Constructor.
		public ArrayEnumerator2(Array array)
				{
					this.array  = array;
					this.lower1 = array.GetLowerBound(0);
					this.lower2 = array.GetLowerBound(1);
					this.upper1 = array.GetUpperBound(0);
					this.upper2 = array.GetUpperBound(1);
					this.posn1 = lower1 - 1;
					this.posn2 = upper2;
				}

		// Move to the next element in the array.
		public bool MoveNext()
				{
					if(posn2 >= upper2)
					{
						// Start a new row.
						if(posn1 < upper1)
						{
							posn2 = lower2;
							if(posn2 >= upper2)
							{
								// The inner dimension is zero, so the
								// enumerator never returns anything.
								return false;
							}
							++posn1;
							return true;
						}
						else
						{
							return false;
						}
					}
					else
					{
						// Advance within the current row.
						++posn2;
						return true;
					}
				}

		// Reset the enumerator.
		public void Reset()
				{
					posn1 = lower1 - 1;
					posn2 = upper2;
				}

		// Get the current object.
		public Object Current
				{
					get
					{
						if(posn1 >= lower1 && posn1 <= upper1 &&
						   posn2 >= lower2 && posn2 <= upper2)
						{
							return array.GetValue(posn1, posn2);
						}
						else
						{
							throw new InvalidOperationException
								(_("Invalid_BadEnumeratorPosition"));
						}
					}
				}

	};

	// Private class for enumerating a multi-dimensional array's contents.
	private sealed class ArrayEnumeratorN : IEnumerator
	{
		private Array array;
		private int   rank;
		private long  length;
		private long  posn;
		private int[] lower;
		private int[] upper;
		private int[] aposn;

		// Constructor.
		public ArrayEnumeratorN(Array array, int rank)
				{
					int dim;
					this.array = array;
					this.rank  = rank;
					length = array.LongLength;
					posn = -1;
					this.lower = new int [rank];
					this.upper = new int [rank];
					this.aposn = new int [rank];
					for(dim = 0; dim < rank; ++dim)
					{
						this.lower[dim] = array.GetLowerBound(dim);
						this.upper[dim] = array.GetUpperBound(dim);
					}
				}

		// Move to the next element in the array.
		public bool MoveNext()
				{
					if(posn < length)
					{
						++posn;
						return true;
					}
					else
					{
						return false;
					}
				}

		// Reset the enumerator.
		public void Reset()
				{
					posn = -1;
				}

		// Get the current object.
		public Object Current
				{
					get
					{
						if(posn >= 0 && posn < length)
						{
							int dim;
							long temp = posn;
							long len;
							for(dim = rank - 1; dim >= 0; --dim)
							{
								len = ((long)(upper[dim])) -
								      ((long)(lower[dim]));
								aposn[dim] = unchecked((int)(temp % len)) +
											 lower[dim];
								temp /= len;
							}
							return array.GetValue(aposn);
						}
						else
						{
							throw new InvalidOperationException
								(_("Invalid_BadEnumeratorPosition"));
						}
					}
				}

	};

	// Get the length of an array rank.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public int GetLength(int dimension);

	// Get the lower bound of an array rank.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public int GetLowerBound(int dimension);

	// Get the upper bound of an array rank.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public int GetUpperBound(int dimension);

	// Internal versions of "GetValue".
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern private Object Get(int index1, int index2, int index3);
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern private Object Get(int[] indices);

	// Internal version of "GetValue" that uses relative indexing.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern private Object GetRelative(int index);

	// Get the value at a particular index within a multi-dimensional array.
	public Object GetValue(int[] indices)
	{
		if(indices == null)
		{
			throw new ArgumentNullException("indices");
		}
		if(indices.Length != Rank)
		{
			throw new ArgumentException(_("Arg_MustBeSameSize"));
		}
		return Get(indices);
	}

	// Get the value at a particular index within a single-dimensional array.
	public Object GetValue(int index)
	{
		if(Rank != 1)
		{
			throw new ArgumentException(_("Arg_RankMustBe1"));
		}
		return Get(index, 0, 0);
	}

	// Get the value at a particular index within a double-dimensional array.
	public Object GetValue(int index1, int index2)
	{
		if(Rank != 2)
		{
			throw new ArgumentException(_("Arg_RankMustBe2"));
		}
		return Get(index1, index2, 0);
	}

	// Get the value at a particular index within a triple-dimensional array.
	public Object GetValue(int index1, int index2, int index3)
	{
		if(Rank != 3)
		{
			throw new ArgumentException(_("Arg_RankMustBe3"));
		}
		return Get(index1, index2, index3);
	}

	// Inner version of "IndexOf".
	public static int InnerIndexOf(Array array, Object value,
								   int start, int length)
	{
		Object elem;
		while(length > 0)
		{
			elem = array.GetValue(start);
			if(value != null && elem != null)
			{
				if(value.Equals(elem))
				{
					return start;
				}
			}
			else if(value == null && elem == null)
			{
				return start;
			}
			++start;
			--length;
		}
		return array.GetLowerBound(0) - 1;
	}

	// Inner version of "LastIndexOf".
	public static int InnerLastIndexOf(Array array, Object value,
								       int start, int length)
	{
		Object elem;
		start += length - 1;
		while(length > 0)
		{
			elem = array.GetValue(start);
			if(value != null && elem != null)
			{
				if(value.Equals(elem))
				{
					return start;
				}
			}
			else if(value == null && elem == null)
			{
				return start;
			}
			--start;
			--length;
		}
		return array.GetLowerBound(0) - 1;
	}

	// Get the first index of a specific value within an array.
	public static int IndexOf(Array array, Object value)
	{
		if(array == null)
		{
			throw new ArgumentNullException("array");
		}
		if(array.Rank != 1)
		{
			throw new RankException(_("Arg_RankMustBe1"));
		}
		return InnerIndexOf(array, value, array.GetLowerBound(0),
					array.GetUpperBound(0) - array.GetLowerBound(0) + 1);
	}

	// Get the last index of a specific value within an array.
	public static int LastIndexOf(Array array, Object value)
	{
		if(array == null)
		{
			throw new ArgumentNullException("array");
		}
		if(array.Rank != 1)
		{
			throw new RankException(_("Arg_RankMustBe1"));
		}
		return InnerLastIndexOf(array, value, array.GetLowerBound(0),
					array.GetUpperBound(0) - array.GetLowerBound(0) + 1);
	}

	// Get the first index of a specific value within an array,
	// starting at a particular index.
	public static int IndexOf(Array array, Object value, int startIndex)
	{
		if(array == null)
		{
			throw new ArgumentNullException("array");
		}
		if(array.Rank != 1)
		{
			throw new RankException(_("Arg_RankMustBe1"));
		}
		if(startIndex < array.GetLowerBound(0) ||
		   startIndex > array.GetUpperBound(0))
		{
			throw new ArgumentOutOfRangeException
				("startIndex", _("Arg_InvalidArrayIndex"));
		}
		return InnerIndexOf(array, value, startIndex,
							array.GetUpperBound(0) - startIndex + 1);
	}

	// Get the last index of a specific value within an array,
	// starting at a particular index.
	public static int LastIndexOf(Array array, Object value, int startIndex)
	{
		if(array == null)
		{
			throw new ArgumentNullException("array");
		}
		if(array.Rank != 1)
		{
			throw new RankException(_("Arg_RankMustBe1"));
		}
		if(startIndex < array.GetLowerBound(0) ||
		   startIndex > array.GetUpperBound(0))
		{
			throw new ArgumentOutOfRangeException
				("startIndex", _("Arg_InvalidArrayIndex"));
		}
		return InnerLastIndexOf(array, value, array.GetLowerBound(0),
								startIndex - array.GetLowerBound(0) + 1);
	}

	// Get the first index of a specific value within an array,
	// starting at a particular index, searching for "count" items.
	public static int IndexOf(Array array, Object value,
							  int startIndex, int count)
	{
		if(array == null)
		{
			throw new ArgumentNullException("array");
		}
		if(array.Rank != 1)
		{
			throw new RankException(_("Arg_RankMustBe1"));
		}
		if(startIndex < array.GetLowerBound(0) ||
		   startIndex > array.GetUpperBound(0) ||
		   count < 0 ||
		   count > (array.GetUpperBound(0) - startIndex + 1))
		{
			throw new ArgumentOutOfRangeException(_("Arg_InvalidArrayRange"));
		}
		return InnerIndexOf(array, value, startIndex, count);
	}

	// Get the last index of a specific value within an array,
	// starting at a particular index, searching for "count" items.
	public static int LastIndexOf(Array array, Object value,
							      int startIndex, int count)
	{
		if(array == null)
		{
			throw new ArgumentNullException("array");
		}
		if(array.Rank != 1)
		{
			throw new RankException(_("Arg_RankMustBe1"));
		}
		if(startIndex < array.GetLowerBound(0) ||
		   startIndex > array.GetUpperBound(0) ||
		   count < 0 ||
		   count > (startIndex - array.GetLowerBound(0) + 1))
		{
			throw new ArgumentOutOfRangeException(_("Arg_InvalidArrayRange"));
		}
		return InnerLastIndexOf(array, value, startIndex - count + 1, count);
	}

	// Inner version of "Reverse".
	public static void InnerReverse(Array array, int lower, int upper)
	{
		Object temp;
		while(lower < upper)
		{
			temp = array.GetValue(lower);
			array.SetValue(array.GetValue(upper), lower);
			array.SetValue(temp, upper);
			++lower;
			--upper;
		}
	}

	// Reverse the order of elements in an array.
	public static void Reverse(Array array)
	{
		if(array == null)
		{
			throw new ArgumentNullException("array");
		}
		if(array.Rank != 1)
		{
			throw new RankException(_("Arg_RankMustBe1"));
		}
		InnerReverse(array, array.GetLowerBound(0), array.GetUpperBound(0));
	}

	// Reverse the order of elements in an array sub-range.
	public static void Reverse(Array array, int index, int length)
	{
		if(array == null)
		{
			throw new ArgumentNullException("array");
		}
		if(array.Rank != 1)
		{
			throw new RankException(_("Arg_RankMustBe1"));
		}
		if(index < array.GetLowerBound(0))
		{
			throw new ArgumentOutOfRangeException
				("index", _("ArgRange_Array"));
		}
		if(length < 0)
		{
			throw new ArgumentOutOfRangeException
				("length", _("ArgRange_Array"));
		}
		if(index > array.GetUpperBound(0) ||
	       length > (array.GetUpperBound(0) - index + 1))
		{
			throw new ArgumentException(_("Arg_InvalidArrayRange"));
		}
		InnerReverse(array, index, index + length - 1);
	}

	// Internal versions of "SetValue".
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern private void Set(Object value, int index1, int index2, int index3);
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern private void Set(Object value, int[] indices);

	// Internal version of "SetValue" that uses relative indexing.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern private void SetRelative(Object value, int index);

	// Set the value at a particular index within a multi-dimensional array.
	public void SetValue(Object value, int[] indices)
	{
		if(indices == null)
		{
			throw new ArgumentNullException("indices");
		}
		if(indices.Length != Rank)
		{
			throw new ArgumentException(_("Arg_MustBeSameSize"));
		}
		Set(value, indices);
	}

	// Set the value at a particular index within a single-dimensional array.
	public void SetValue(Object value, int index)
	{
		if(Rank != 1)
		{
			throw new ArgumentException(_("Arg_RankMustBe1"));
		}
		Set(value, index, 0, 0);
	}

	// Set the value at a particular index within a double-dimensional array.
	public void SetValue(Object value, int index1, int index2)
	{
		if(Rank != 2)
		{
			throw new ArgumentException(_("Arg_RankMustBe2"));
		}
		Set(value, index1, index2, 0);
	}

	// Set the value at a particular index within a triple-dimensional array.
	public void SetValue(Object value, int index1, int index2, int index3)
	{
		if(Rank != 3)
		{
			throw new ArgumentException(_("Arg_RankMustBe3"));
		}
		Set(value, index1, index2, index3);
	}

	// Inner version of "Sort".  Based on the Quicksort implementation
	// described in R. Sedgewick, "Algorithms in C++", Addison-Wesley, 1992.
	public static void InnerSort(Array keys, Array items,
						         int lower, int upper,
						         IComparer comparer)
	{
		int i, j, cmp;
		Object testKey;
		Object valuei;
		Object valuej;
		if(lower < upper)
		{
			// Partition the array.
			testKey = keys.GetValue(upper);
			i = lower - 1;
			j = upper;
			for(;;)
			{
				do
				{
					++i;
					valuei = keys.GetValue(i);
					if(comparer != null)
					{
						cmp = comparer.Compare(valuei, testKey);
					}
					else
					{
						cmp = ((IComparable)valuei).CompareTo(testKey);
					}
				}
				while(cmp < 0);
				do
				{
					--j;
					valuej = keys.GetValue(j);
					if(comparer != null)
					{
						cmp = comparer.Compare(valuej, testKey);
					}
					else
					{
						cmp = ((IComparable)valuej).CompareTo(testKey);
					}
				}
				while(cmp > 0);
				if(i >= j)
				{
					break;
				}
				keys.SetValue(valuej, i);
				keys.SetValue(valuei, j);
				if(items != null)
				{
					valuei = items.GetValue(i);
					valuej = items.GetValue(j);
					items.SetValue(valuej, i);
					items.SetValue(valuei, j);
				}
			}
			valuei = keys.GetValue(i);
			valuej = keys.GetValue(upper);
			keys.SetValue(valuej, i);
			keys.SetValue(valuei, upper);
			if(items != null)
			{
				valuei = items.GetValue(i);
				valuej = items.GetValue(upper);
				items.SetValue(valuej, i);
				items.SetValue(valuei, upper);
			}

			// Sort the sub-partitions.
			InnerSort(keys, items, lower, i - 1, comparer);
			InnerSort(keys, items, i + 1, upper, comparer);
		}
	}

	// Sort an array of keys.
	public static void Sort(Array array)
	{
		Sort(array, (IComparer)null);
	}

	// Sort an array of keys using a comparer.
	public static void Sort(Array array, IComparer comparer)
	{
		if(array == null)
		{
			throw new ArgumentNullException("array");
		}
		if(array.Rank != 1)
		{
			throw new RankException(_("Arg_RankMustBe1"));
		}
		InnerSort(array, null, array.GetLowerBound(0),
				  array.GetUpperBound(0), comparer);
	}

	// Sort an array of keys and items.
	public static void Sort(Array keys, Array items)
	{
		Sort(keys, items, (IComparer)null);
	}

	// Sort an array of keys and items using a comparer.
	public static void Sort(Array keys, Array items, IComparer comparer)
	{
		if(keys == null)
		{
			throw new ArgumentNullException("keys");
		}
		if(keys.Rank != 1)
		{
			throw new RankException(_("Arg_RankMustBe1"));
		}
		if(items != null)
		{
			if(items.Rank != 1)
			{
				throw new RankException(_("Arg_RankMustBe1"));
			}
			if(items.GetLowerBound(0) != keys.GetLowerBound(0))
			{
				throw new ArgumentException(_("Arg_LowBoundsMustMatch"));
			}
			if(items.Length < keys.Length)
			{
				throw new ArgumentException(_("Arg_ShortItemsArray"));
			}
		}
		InnerSort(keys, items, keys.GetLowerBound(0),
				  keys.GetUpperBound(0), comparer);
	}

	// Sort an array sub-range of keys.
	public static void Sort(Array array, int index, int length)
	{
		Sort(array, index, length, (IComparer)null);
	}

	// Sort an array sub-range of keys using a comparer.
	public static void Sort(Array array, int index, int length,
							IComparer comparer)
	{
		if(array == null)
		{
			throw new ArgumentNullException("array");
		}
		if(array.Rank != 1)
		{
			throw new RankException(_("Arg_RankMustBe1"));
		}
		if(index < array.GetLowerBound(0))
		{
			throw new ArgumentOutOfRangeException
				("index", _("ArgRange_Array"));
		}
		if(length < 0)
		{
			throw new ArgumentOutOfRangeException
				("length", _("ArgRange_Array"));
		}
		if(index > array.GetUpperBound(0) ||
		   length > (array.GetUpperBound(0) - index + 1))
		{
			throw new ArgumentException(_("Arg_InvalidArrayRange"));
		}
		InnerSort(array, null, index, index + length - 1, comparer);
	}

	// Sort an array sub-range of keys and items.
	public static void Sort(Array keys, Array items, int index, int length)
	{
		Sort(keys, items, index, length, (IComparer)null);
	}

	// Sort an array sub-range of keys and items using a comparer.
	public static void Sort(Array keys, Array items,
							int index, int length,
							IComparer comparer)
	{
		if(keys == null)
		{
			throw new ArgumentNullException("keys");
		}
		if(keys.Rank != 1)
		{
			throw new RankException(_("Arg_RankMustBe1"));
		}
		if(index < keys.GetLowerBound(0))
		{
			throw new ArgumentOutOfRangeException
				("index", _("ArgRange_Array"));
		}
		if(length < 0)
		{
			throw new ArgumentOutOfRangeException
				("length", _("ArgRange_Array"));
		}
		if(index > keys.GetUpperBound(0) ||
		   length > (keys.GetUpperBound(0) - index + 1))
		{
			throw new ArgumentException(_("Arg_InvalidArrayRange"));
		}
		if(items != null)
		{
			if(items.Rank != 1)
			{
				throw new RankException(_("Arg_RankMustBe1"));
			}
			if(index < items.GetLowerBound(0))
			{
				throw new ArgumentOutOfRangeException
					("index", _("ArgRange_Array"));
			}
			if(index > items.GetUpperBound(0) ||
			   length > (items.GetUpperBound(0) - index + 1))
			{
				throw new ArgumentException(_("Arg_InvalidArrayRange"));
			}
		}
		InnerSort(keys, items, index, index + length - 1, comparer);
	}

	// Implement the "IList" interface.
	int IList.Add(Object value)
	{
		throw new NotSupportedException(_("NotSupp_FixedSizeCollection"));
	}
	void IList.Clear()
	{
		Clear(this, 0, Length);
	}
	bool IList.Contains(Object value)
	{
		return (IndexOf(this, value) >= GetLowerBound(0));
	}
	int IList.IndexOf(Object value)
	{
		return IndexOf(this, value);
	}
	void IList.Insert(int index, Object value)
	{
		throw new NotSupportedException(_("NotSupp_FixedSizeCollection"));
	}
	void IList.Remove(Object value)
	{
		throw new NotSupportedException(_("NotSupp_FixedSizeCollection"));
	}
	void IList.RemoveAt(int index)
	{
		throw new NotSupportedException(_("NotSupp_FixedSizeCollection"));
	}
	public virtual bool IsFixedSize
	{
		get
		{
			return true;
		}
	}
	public virtual bool IsReadOnly
	{
		get
		{
			return false;
		}
	}
	Object IList.this[int index]
	{
		get
		{
			return GetValue(index);
		}
		set
		{
			SetValue(value, index);
		}
	}

	// Internal implementation of the "Length" and "Rank" properties.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern private int GetLength();
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern private int GetRank();

	// Properties.
	public int Length
	{
		get
		{
			return GetLength();
		}
	}
	public long LongLength
	{
		get
		{
			// Normally this will return the same as "Length", but it
			// is theoretically possible that really huge arrays could
			// exist on 64-bit systems in the future, so we calculate
			// the value carefully.
			long len = 1;
			int ranks = GetRank();
			while(ranks > 0)
			{
				--ranks;
				len *= (long)(GetUpperBound(ranks) - GetLowerBound(ranks) + 1);
			}
			return len;
		}
	}
	public int Rank
	{
		get
		{
			return GetRank();
		}
	}

}; // class Array

}; // namespace System
