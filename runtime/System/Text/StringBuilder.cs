/*
 * StringBuilder.cs - Implementation of the "System.Text.StringBuilder" class.
 *
 * Copyright (C) 2001  Southern Storm Software, Pty Ltd.
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

namespace System.Text
{

using System;
using System.Runtime.CompilerServices;

public sealed class StringBuilder
{
	// Internal state.
	private String buildString;
	private int    maxCapacity;
	private bool   needsCopy;

	// Minimum recommended capacity.
	private const int MinCapacity = 16;

	// Value to use to round up length resizes.
	// Must be a power of 2 minus 1.
	private const int ResizeRound = 31;

	// Constructors.
	public StringBuilder() : this(MinCapacity) {}
	public StringBuilder(int capacity)
			{
				if(capacity < MinCapacity)
				{
					capacity = MinCapacity;
				}
				buildString = String.NewBuilder(null, capacity);
				maxCapacity = Int32.MaxValue;
				needsCopy = false;
			}
	public StringBuilder(String value)
			{
				if(value != null)
				{
					buildString = String.NewBuilder(null, MinCapacity);
				}
				else
				{
					int capacity = value.Length;
					if(capacity < MinCapacity)
					{
						capacity = MinCapacity;
					}
					buildString = String.NewBuilder(value, capacity);
				}
				maxCapacity = Int32.MaxValue;
				needsCopy = false;
			}
	public StringBuilder(int capacity, int maxCapacity)
			{
				if(maxCapacity < 1)
				{
					throw new ArgumentOutOfRangeException
						("maxCapacity", _("ArgRange_MaxStrCapacity"));
				}
				else if(capacity < 0 || capacity > maxCapacity)
				{
					throw new ArgumentOutOfRangeException
						("capacity", _("ArgRange_StrCapacity"));
				}
				if(capacity < MinCapacity && maxCapacity >= MinCapacity)
				{
					capacity = MinCapacity;
				}
				buildString = String.NewBuilder(null, capacity);
				this.maxCapacity = maxCapacity;
				needsCopy = false;
			}
	public StringBuilder(String value, int capacity)
			{
				if(value != null)
				{
					if(capacity < value.Length)
					{
						buildString = String.NewBuilder(value, -1);
					}
					else
					{
						buildString = String.NewBuilder(value, capacity);
					}
				}
				else
				{
					if(capacity < MinCapacity)
					{
						capacity = MinCapacity;
					}
					buildString = String.NewBuilder(null, capacity);
				}
				maxCapacity = Int32.MaxValue;
				needsCopy = false;
			}
	public StringBuilder(String value, int startIndex,
						 int length, int capacity)
			{
				// Shrink the substring to the legitimate part of the string.
				if(value != null)
				{
					if(startIndex < 0)
					{
						length += startIndex;
						startIndex = 0;
					}
					else if(startIndex > value.Length)
					{
						startIndex = 0;
						length = 0;
					}
					if(length < 0)
					{
						length = 0;
					}
					if((value.Length - startIndex) < length)
					{
						length = value.Length - startIndex;
					}
				}
				else
				{
					startIndex = 0;
					length = 0;
				}

				// Validate the capacity.
				if(capacity < length)
				{
					capacity = length;
				}
				if(capacity < MinCapacity)
				{
					capacity = MinCapacity;
				}

				// Allocate a new string builder and fill it.
				buildString = String.NewBuilder(null, capacity);
				if(length > 0)
				{
					String.Copy(buildString, 0, value, startIndex, length);
				}
				maxCapacity = Int32.MaxValue;
				needsCopy = false;
			}

	// Append space to a string builder.  Returns the actual
	// number of characters that can be appended.
	private int AppendSpace(int length)
			{
				int buildLen = buildString.length;
				int newLength;

				// Determine the new length.
				newLength = buildLen + length;
				if(newLength > maxCapacity)
				{
					newLength = maxCapacity;
					length = newLength - buildLen;
				}

				// Reallocate the string as necessary.
				if(needsCopy)
				{
					int capacity = buildString.capacity;
					if(capacity < newLength)
					{
						capacity = (newLength + ResizeRound)
										& ~ResizeRound;
					}
					if(capacity > maxCapacity)
					{
						capacity = maxCapacity;
					}
					buildString = String.NewBuilder(buildString, capacity);
					needsCopy = false;
				}
				else if(newLength > buildString.capacity)
				{
					newLength = (newLength + ResizeRound) & ~ResizeRound;
					if(newLength > maxCapacity)
					{
						newLength = maxCapacity;
					}
					buildString = String.NewBuilder(buildString, newLength);
				}

				// Return the actual length to the caller.
				return length;
			}

	// Append a string to this builder.
	//
	// Note: the ECMA spec is unclear as to what happens when the
	// maximum capacity overflows.  We have chosen to append as
	// much as possible and truncate the rest.
	public StringBuilder Append(String value)
			{
				if(value != null)
				{
					// Append some extra space to the string.
					int valueLen = AppendSpace(value.Length);

					// Copy the contents of the value into place.
					String.Copy(buildString, buildString.length,
								value, 0, valueLen);
					buildString.length += valueLen;
				}
				return this;
			}

	// Append other types of values to this builder.
	public StringBuilder Append(bool value)
			{
				return Append(value.ToString());
			}
	[CLSCompliant(false)]
	public StringBuilder Append(sbyte value)
			{
				return Append(value.ToString());
			}
	public StringBuilder Append(byte value)
			{
				return Append(value.ToString());
			}
	public StringBuilder Append(short value)
			{
				return Append(value.ToString());
			}
	[CLSCompliant(false)]
	public StringBuilder Append(ushort value)
			{
				return Append(value.ToString());
			}
	public StringBuilder Append(char value)
			{
				int length = AppendSpace(1);
				String.CharFill(buildString, buildString.length,
								length, value);
				buildString.length += length;
				return this;
			}
	public StringBuilder Append(char value, int repeatCount)
			{
				if(repeatCount < 0)
				{
					throw new ArgumentOutOfRangeException
						("repeatCount", _("ArgRange_NonNegative"));
				}
				else if(repeatCount == 0)
				{
					return this;
				}
				else
				{
					repeatCount = AppendSpace(repeatCount);
					String.CharFill(buildString, buildString.length,
								    repeatCount, value);
					buildString.length += repeatCount;
					return this;
				}
			}
	public StringBuilder Append(int value)
			{
				return Append(value.ToString());
			}
	[CLSCompliant(false)]
	public StringBuilder Append(uint value)
			{
				return Append(value.ToString());
			}
	public StringBuilder Append(long value)
			{
				return Append(value.ToString());
			}
	[CLSCompliant(false)]
	public StringBuilder Append(ulong value)
			{
				return Append(value.ToString());
			}
	public StringBuilder Append(float value)
			{
				return Append(value.ToString());
			}
	public StringBuilder Append(double value)
			{
				return Append(value.ToString());
			}
	public StringBuilder Append(Decimal value)
			{
				return Append(value.ToString());
			}
	public StringBuilder Append(Object value)
			{
				if(value != null)
				{
					return Append(value.ToString());
				}
				else
				{
					return this;
				}
			}
	public StringBuilder Append(String value, int startIndex, int length)
			{
				if(value != null)
				{
					if(startIndex < 0 || startIndex > value.Length)
					{
						throw new ArgumentOutOfRangeException
							("startIndex", _("ArgRange_Array"));
					}
					else if(length < 0 || (value.Length - startIndex) < length)
					{
						throw new ArgumentOutOfRangeException
							("length", _("ArgRange_Array"));
					}
					length = AppendSpace(length);
					String.Copy(buildString, buildString.length,
							    value, startIndex, length);
					buildString.length += length;
					return this;
				}
				else if(startIndex != 0 || length != 0)
				{
					throw new ArgumentNullException("value");
				}
				else
				{
					return this;
				}
			}
	public StringBuilder Append(char[] value)
			{
				if(value != null)
				{
					return Append(value, 0, value.Length);
				}
				else
				{
					return this;
				}
			}
	public StringBuilder Append(char[] value, int startIndex, int length)
			{
				if(value != null)
				{
					if(startIndex < 0 || startIndex > value.Length)
					{
						throw new ArgumentOutOfRangeException
							("startIndex", _("ArgRange_Array"));
					}
					else if(length < 0 || (value.Length - startIndex) < length)
					{
						throw new ArgumentOutOfRangeException
							("length", _("ArgRange_Array"));
					}
				}
				else if(startIndex != 0 || length != 0)
				{
					throw new ArgumentNullException("value");
				}
				length = AppendSpace(length);
				String.CharFill(buildString, buildString.length,
							    value, startIndex, length);
				buildString.length += length;
				return this;
			}

	// Appended formatted values to this string builder.
	public StringBuilder AppendFormat(String format, Object arg0)
			{
				return Append(String.Format(format, arg0));
			}
	public StringBuilder AppendFormat(String format, Object arg0, Object arg1)
			{
				return Append(String.Format(format, arg0, arg1));
			}
	public StringBuilder AppendFormat(String format, Object arg0, Object arg1,
									  Object arg2)
			{
				return Append(String.Format(format, arg0, arg1, arg2));
			}
	public StringBuilder AppendFormat(String format, params Object[] args)
			{
				return Append(String.Format(format, args));
			}
	public StringBuilder AppendFormat(IFormatProvider provider,
									  String format, params Object[] args)
			{
				return Append(String.Format(provider, format, args));
			}

	// Ensure that this string builder has a specific capacity available.
	public int EnsureCapacity(int capacity)
			{
				if(capacity < 0)
				{
					throw new ArgumentOutOfRangeException
						("capacity", _("ArgRange_NonNegative"));
				}
				else if(capacity <= buildString.capacity)
				{
					return buildString.capacity;
				}
				capacity = (capacity + ResizeRound) & ~ResizeRound;
				if(capacity > maxCapacity)
				{
					capacity = maxCapacity;
				}
				buildString = String.NewBuilder(buildString, capacity);
				needsCopy = false;
				return buildString.capacity;
			}

	// Determine if two StringBuilders are equal.
	public bool Equals(StringBuilder sb)
			{
				if(sb != null)
				{
					return String.Equals(buildString, sb.buildString);
				}
				else
				{
					return false;
				}
			}

	// Insert space to a string builder.  Returns the actual
	// number of characters that can be inserted.
	private int InsertSpace(int index, int length)
			{
				int buildLen = buildString.length;
				int newLength;

				// Validate the index.
				if(index < 0 || index > buildLen)
				{
					throw new ArgumentOutOfRangeException
						("index", _("ArgRange_StringIndex"));
				}

				// Determine the new length.
				newLength = buildLen + length;
				if(newLength > maxCapacity)
				{
					newLength = maxCapacity;
					length = newLength - buildLen;
				}

				// Reallocate the string as necessary.
				if(needsCopy)
				{
					int capacity = buildString.capacity;
					if(capacity < newLength)
					{
						capacity = (newLength + ResizeRound)
										& ~ResizeRound;
					}
					if(capacity > maxCapacity)
					{
						capacity = maxCapacity;
					}
					buildString = String.NewBuilder(buildString, capacity);
					needsCopy = false;
				}
				else if(newLength > buildString.capacity)
				{
					newLength = (newLength + ResizeRound) & ~ResizeRound;
					if(newLength > maxCapacity)
					{
						newLength = maxCapacity;
					}
					buildString = String.NewBuilder(buildString, newLength);
				}

				// Move the characters after the index up.
				if(index < buildLen && length > 0)
				{
					String.InsertSpace(buildString, index, index + length);
				}

				// Return the actual length to the caller.
				return length;
			}

	// Insert a string into this builder.
	//
	// Note: the ECMA spec is unclear as to what happens when the
	// maximum capacity overflows.  We have chosen to insert as
	// much as possible and truncate the rest.
	public StringBuilder Insert(int index, String value)
			{
				if(value != null)
				{
					// Insert some extra space to the string.
					int valueLen = InsertSpace(index, value.Length);

					// Copy the contents of the value into place.
					String.Copy(buildString, index, value, 0, valueLen);
				}
				return this;
			}

	// Insert other types of values into this builder.
	public StringBuilder Insert(int index, bool value)
			{
				return Insert(index, value.ToString());
			}
	[CLSCompliant(false)]
	public StringBuilder Insert(int index, sbyte value)
			{
				return Insert(index, value.ToString());
			}
	public StringBuilder Insert(int index, byte value)
			{
				return Insert(index, value.ToString());
			}
	public StringBuilder Insert(int index, short value)
			{
				return Insert(index, value.ToString());
			}
	[CLSCompliant(false)]
	public StringBuilder Insert(int index, ushort value)
			{
				return Insert(index, value.ToString());
			}
	public StringBuilder Insert(int index, char value)
			{
				int length = InsertSpace(index, 1);
				String.CharFill(buildString, index, length, value);
				buildString.length += length;
				return this;
			}
	public StringBuilder Insert(int index, int value)
			{
				return Insert(index, value.ToString());
			}
	[CLSCompliant(false)]
	public StringBuilder Insert(int index, uint value)
			{
				return Insert(index, value.ToString());
			}
	public StringBuilder Insert(int index, long value)
			{
				return Insert(index, value.ToString());
			}
	[CLSCompliant(false)]
	public StringBuilder Insert(int index, ulong value)
			{
				return Insert(index, value.ToString());
			}
	public StringBuilder Insert(int index, float value)
			{
				return Insert(index, value.ToString());
			}
	public StringBuilder Insert(int index, double value)
			{
				return Insert(index, value.ToString());
			}
	public StringBuilder Insert(int index, Decimal value)
			{
				return Insert(index, value.ToString());
			}
	public StringBuilder Insert(int index, Object value)
			{
				if(value != null)
				{
					return Insert(index, value.ToString());
				}
				else
				{
					return this;
				}
			}
	public StringBuilder Insert(int index, char[] value)
			{
				if(value != null)
				{
					return Insert(index, value, 0, value.Length);
				}
				else
				{
					return this;
				}
			}
	public StringBuilder Insert(int index, char[] value,
								int startIndex, int length)
			{
				if(value != null)
				{
					if(startIndex < 0 || startIndex > value.Length)
					{
						throw new ArgumentOutOfRangeException
							("startIndex", _("ArgRange_Array"));
					}
					else if((value.Length - startIndex) > length)
					{
						throw new ArgumentOutOfRangeException
							("length", _("ArgRange_Array"));
					}
				}
				else if(startIndex != 0 || length != 0)
				{
					throw new ArgumentNullException("value");
				}
				length = InsertSpace(index, length);
				String.CharFill(buildString, index, value, startIndex, length);
				buildString.length += length;
				return this;
			}
	public StringBuilder Insert(int index, String value, int count)
			{
				if(count < 0)
				{
					throw new ArgumentOutOfRangeException
						("count", _("ArgRange_NonNegative"));
				}
				while(count > 0)
				{
					Insert(index, value);
					if(buildString.length >= maxCapacity)
					{
						break;
					}
					--count;
				}
				return this;
			}

	// Remove a range of characters from this string builder.
	public StringBuilder Remove(int startIndex, int length)
			{
				if(startIndex < 0 || startIndex > buildString.length)
				{
					throw new ArgumentOutOfRangeException
						("startIndex", _("ArgRange_StringIndex"));
				}
				else if((buildString.length - startIndex) > length)
				{
					throw new ArgumentOutOfRangeException
						("length", _("ArgRange_StringRange"));
				}
				else if(needsCopy)
				{
					buildString = String.NewBuilder
						(buildString, buildString.capacity);
					needsCopy = false;
				}
				String.RemoveSpace(buildString, startIndex, length);
				return this;
			}

	// Replace all occurrences of a specific old string with a new string.
	public StringBuilder Replace(char oldChar, char newChar)
			{
				if(oldChar != newChar && buildString.length > 0)
				{
					buildString = buildString.Replace(oldChar, newChar);
					needsCopy = false;
				}
				return this;
			}
	public StringBuilder Replace(String oldValue, String newValue)
			{
				return Replace(oldValue, newValue, 0, buildString.length);
			}
	public StringBuilder Replace(String oldValue, String newValue,
								 int startIndex, int count)
			{
				if(oldValue == null)
				{
					throw new ArgumentNullException("oldValue");
				}
				else if(oldValue.Length == 0)
				{
					throw new ArgumentException
						(_("ArgRange_StringNonEmpty"));
				}
				else if(startIndex < 0 || startIndex > buildString.length)
				{
					throw new ArgumentOutOfRangeException
						("startIndex", _("ArgRange_StringIndex"));
				}
				else if((buildString.length - startIndex) > count)
				{
					throw new ArgumentOutOfRangeException
						("count", _("ArgRange_StringRange"));
				}
				if(count > 0)
				{
					String temp = buildString;
					buildString = String.NewBuilder
						(null, buildString.capacity);
					needsCopy = false;
					if(startIndex > 0)
					{
						Append(temp, 0, startIndex);
					}
					Append(temp.Substring(startIndex, count)
							   .Replace(oldValue, newValue));
					if((startIndex + count) < temp.length)
					{
						Append(temp, startIndex + count,
							   temp.length - (startIndex + count));
					}
				}
				return this;
			}
	public StringBuilder Replace(char oldChar, char newChar,
								 int startIndex, int count)
			{
				if(startIndex < 0 || startIndex > buildString.length)
				{
					throw new ArgumentOutOfRangeException
						("startIndex", _("ArgRange_StringIndex"));
				}
				else if((buildString.length - startIndex) > count)
				{
					throw new ArgumentOutOfRangeException
						("count", _("ArgRange_StringRange"));
				}
				else if(oldChar == newChar)
				{
					return this;
				}
				if(count > 0)
				{
					String temp = buildString;
					buildString = String.NewBuilder
						(null, buildString.capacity);
					needsCopy = false;
					if(startIndex > 0)
					{
						Append(temp, 0, startIndex);
					}
					Append(temp.Substring(startIndex, count)
							   .Replace(oldChar, newChar));
					if((startIndex + count) < temp.length)
					{
						Append(temp, startIndex + count,
							   temp.length - (startIndex + count));
					}
				}
				return this;
			}

	// Convert this string builder into a string.
	public override String ToString()
			{
				if(needsCopy)
				{
					// Already in its real string form.
					return buildString;
				}
				else if(buildString.Length > (buildString.capacity / 2))
				{
					// Return the builder itself if more than half full.
					needsCopy = true;
					return buildString;
				}
				else
				{
					// Create a new string with the contents.
					return String.Copy(buildString);
				}
			}

	// Convert a sub-range of this string builder into a string.
	public String ToString(int startIndex, int length)
			{
				// Let the String class do the work, including validation.
				return buildString.Substring(startIndex, length);
			}

	// Get or set the capacity of this string builder.
	public int Capacity
			{
				get
				{
					return buildString.length;
				}
				set
				{
					if(value < buildString.length)
					{
						throw new ArgumentException
							(_("ArgRange_StrCapacity"));
					}
					EnsureCapacity(value);
				}
			}

	// Get or set a particular character within this string builder.
	// The "IndexerName" attribute ensures that the get
	// accessor method is named according to the ECMA spec.
	[IndexerName("Chars")]
	public char this[int index]
			{
				get
				{
					return buildString.GetChar(index);
				}
				set
				{
					buildString.SetChar(index, value);
				}
			}

	// Get or set the length of this string builder.
	public int Length
			{
				get
				{
					return buildString.Length;
				}
				set
				{
					int buildLen;
					if(value < 0 || value > maxCapacity)
					{
						throw new ArgumentOutOfRangeException
							("value", _("ArgRange_StrBuilderLength"));
					}
					buildLen = buildString.length;
					if(value < buildLen)
					{
						Remove(value, buildLen - value);
					}
					else if(value > buildLen)
					{
						Append(' ', value - buildLen);
					}
				}
			}

	// Get the maximum capacity for this string builder.
	public int MaxCapacity
			{
				get
				{
					return maxCapacity;
				}
			}

}; // class StringBuilder

}; // namespace System.Text
