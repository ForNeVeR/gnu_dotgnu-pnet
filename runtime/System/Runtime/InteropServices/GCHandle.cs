/*
 * GCHandle.cs - Implementation of the
 *			"System.Runtime.InteropServices.GCHandle" class.
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

namespace System.Runtime.InteropServices
{

using System.Runtime.CompilerServices;

public struct GCHandle
{

	// Internal state.
	private int handle;

	// Private constructor that is called from "Alloc".
	private GCHandle(Object value, GCHandleType type)
			{
				handle = InternalAlloc(value, type);
				if(type == GCHandleType.Pinned)
				{
					handle |= 1;
				}
			}

	// Private constructor that boxes up a handle value.
	private GCHandle(int value)
			{
				handle = value;
			}

	// Get the address of a pinned object that is referred to by this handle.
	public IntPtr AddrOfPinnedObject()
			{
				if((handle & 1) != 0)
				{
					// The handle is valid and pinned.
					return InternalAddrOfPinnedObject(handle & ~1);
				}
				else if(handle != 0)
				{
					// The handle has not been pinned.
					throw new InvalidOperationException
						(_("Invalid_GCHandleNotPinned"));
				}
				else
				{
					// The handle has not been initialized.
					throw new InvalidOperationException
						(_("Invalid_GCHandleNotInit"));
				}
			}

	// Internal version of "AddrOfPinnedObject".
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern private static IntPtr InternalAddrOfPinnedObject(int handle);

	// Allocate a handle for a particular object.
	public static GCHandle Alloc(Object obj)
			{
				return new GCHandle(obj, GCHandleType.Normal);
			}
	public static GCHandle Alloc(Object obj, GCHandleType type)
			{
				return new GCHandle(obj, type);
			}

	// Internal version of "Alloc".
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern private static int InternalAlloc(Object value, GCHandleType type);

	// Free this handle.
	public void Free()
			{
				if(handle != 0)
				{
					InternalFree(handle & ~1);
					handle = 0;
				}
				else
				{
					throw new InvalidOperationException
						(_("Invalid_GCHandleNotInit"));
				}
			}

	// Internal version of "Free".
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern private static void InternalFree(int handle);

	// Convert to and from the native integer representation.
	public static explicit operator GCHandle(IntPtr value)
			{
				int handle = (int)value;
				if(InternalValidate(handle))
				{
					return new GCHandle(handle);
				}
				else
				{
					throw new InvalidOperationException
						(_("Invalid_GCHandleInvalid"));
				}
			}
	public static explicit operator IntPtr(GCHandle handle)
			{
				return (IntPtr)(handle.handle);
			}

	// Internal method to validate a handle.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern private static bool InternalValidate(int handle);

	// Determine if this handle is allocated and valid.
	public bool IsAllocated
			{
				get
				{
					return (handle != 0);
				}
			}

	// Get or set the underlying object target for this handle.
	public Object Target
			{
				get
				{
					if(handle != 0)
					{
						return InternalGetTarget(handle & ~1);
					}
					else
					{
						throw new InvalidOperationException
							(_("Invalid_GCHandleNotInit"));
					}
				}
				set
				{
					if(handle != 0)
					{
						InternalSetTarget(handle & ~1, value,
										  (handle & 1) != 0);
					}
					else
					{
						throw new InvalidOperationException
							(_("Invalid_GCHandleNotInit"));
					}
				}
			}

	// Internal implementation of the "Target" property.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern private static Object InternalGetTarget(int handle);

	[MethodImpl(MethodImplOptions.InternalCall)]
	extern private static void InternalSetTarget(int handle, Object value,
												 bool isPinned);

}; // struct GCHandle

}; // namespace System.Runtime.InteropServices