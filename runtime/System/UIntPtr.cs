/*
 * UIntPtr.cs - Implementation of the "System.UIntPtr" class.
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
using System.Runtime.CompilerServices;
using Platform;

public struct UIntPtr
{
	// Public constants.
	public static readonly UIntPtr Zero = new UIntPtr(0);

	// Internal state.
	unsafe private void *value__;

	// Constructors.
	unsafe public UIntPtr(uint value)
			{
				value__ = (void *)value;
			}
	unsafe public UIntPtr(ulong value)
			{
				if(Size == 4 &&
				   value > ((ulong)(UInt32.MaxValue)))
				{
					throw new OverflowException(_("Overflow_Pointer"));
				}
				value__ = (void *)value;
			}
	unsafe public UIntPtr(void *value)
			{
				value__ = value;
			}

	// Override inherited methods.
	unsafe public override int GetHashCode()
			{
				return unchecked((int)value__);
			}
	unsafe public override bool Equals(Object value)
			{
				if(value is UIntPtr)
				{
					return (value__ == ((UIntPtr)value).value__);
				}
				else
				{
					return false;
				}
			}

	// Numeric conversion.
	unsafe public uint ToUInt32()
			{
				ulong ptr = (ulong)value__;
				if(ptr <= (ulong)(UInt32.MaxValue))
				{
					return unchecked((uint)ptr);
				}
				else
				{
					throw new OverflowException(_("Overflow_Pointer"));
				}
			}
	unsafe public ulong ToUInt64()
			{
				return (ulong)value__;
			}

	// Get the pointer within this object.
	unsafe public void *ToPointer()
			{
				return value__;
			}

	// String conversion.
	unsafe public override String ToString()
			{
				if(Size == 4)
				{
					return ((uint)value__).ToString();
				}
				else
				{
					return ((ulong)value__).ToString();
				}
			}

	// Properties.
	public static int Size
			{
				get
				{
					return Platform.PtrSizes.GetPtrSize();
				}
			}

	// Operators.
	unsafe public static bool operator==(UIntPtr x, UIntPtr y)
			{
				return (x.value__ == y.value__);
			}
	unsafe public static bool operator!=(UIntPtr x, UIntPtr y)
			{
				return (x.value__ != y.value__);
			}
	unsafe public static explicit operator UIntPtr(uint x)
			{
				return new UIntPtr(x);
			}
	unsafe public static explicit operator UIntPtr(ulong x)
			{
				return new UIntPtr(x);
			}
	unsafe public static explicit operator UIntPtr(void *x)
			{
				return new UIntPtr(x);
			}
	unsafe public static explicit operator uint(UIntPtr x)
			{
				return x.ToUInt32();
			}
	unsafe public static explicit operator ulong(UIntPtr x)
			{
				return x.ToUInt64();
			}
	unsafe public static explicit operator void *(UIntPtr x)
			{
				return x.ToPointer();
			}

}; // struct UIntPtr

}; // namespace System
