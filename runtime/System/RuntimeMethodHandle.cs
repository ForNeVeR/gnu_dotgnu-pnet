/*
 * RuntimeMethodHandle.cs - Implementation of the
 *			"System.RuntimeMethodHandle" class.
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

using System.Runtime.CompilerServices;

public struct RuntimeMethodHandle
{
	// Internal state.
	private IntPtr value__;

	// Internal constructor.
	internal RuntimeMethodHandle(IntPtr value)
			{
				value__ = value;
			}

	// Get the function pointer associated with this method.
	public IntPtr GetFunctionPointer()
			{
				return InternalGetFunctionPointer(value__);
			}

	// Internal version of "GetFunctionPointer".
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern private static IntPtr InternalGetFunctionPointer(IntPtr method);

	// Properties.
	public IntPtr Value
			{
				get
				{
					return value__;
				}
			}

}; // class RuntimeMethodHandle

}; // namespace System