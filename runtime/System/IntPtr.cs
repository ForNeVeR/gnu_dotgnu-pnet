/*
 * IntPtr.cs - Implementation of the "System.IntPtr" class.
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

namespace System
{

using System.Globalization;
using System.Runtime.InteropServices;
using Platform;

public struct IntPtr
{
	unsafe private void *value__;

	// Constructors.
	unsafe public IntPtr(int value)
			{ value__ = (void *)value; }
	unsafe public IntPtr(long value)
			{ value__ = (void *)value; }

	// Override inherited methods.
	unsafe public override int GetHashCode()
			{
				return unchecked(((int)value__) & 0x7FFFFFFF);
			}
	unsafe public override bool Equals(Object value)
			{
				if(value is IntPtr)
				{
					return (value__ == ((IntPtr)value).value__);
				}
				else
				{
					return false;
				}
			}

	// String conversion.
	unsafe public override String ToString()
			{
				return Int32.Format((int)value__, null,
									NumberFormatInfo.InvariantInfo);
			}

	// Properties.
	public int Size
			{
				get
				{
					return Platform.PtrSizes.GetPtrSize();
				}
			}

	// Operators.
	unsafe public static bool operator==(IntPtr x, IntPtr y)
				{ return (x.value__ == y.value__); }
	unsafe public static bool operator!=(IntPtr x, IntPtr y)
				{ return (x.value__ != y.value__); }
	unsafe public static explicit operator IntPtr(int x)
				{ return new IntPtr(x); }
	unsafe public static explicit operator IntPtr(long x)
				{ return new IntPtr(x); }
	unsafe public static explicit operator int(IntPtr x)
				{ return (int)(x.value__); }
	unsafe public static explicit operator long(IntPtr x)
				{ return (long)(x.value__); }

}; // class IntPtr

}; // namespace System
