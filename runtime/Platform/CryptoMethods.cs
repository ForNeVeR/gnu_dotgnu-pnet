/*
 * CryptoMethods.cs - Implementation of the "Platform.CryptoMethods" class.
 *
 * Copyright (C) 2002  Southern Storm Software, Pty Ltd.
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

namespace Platform
{

#if !ECMA_COMPAT

using System;
using System.Runtime.CompilerServices;

internal sealed class CryptoMethods
{
	// Identifiers for the algorithms.
	public const int MD5    = 0;
	public const int SHA1   = 1;
	public const int SHA256 = 2;
	public const int SHA384 = 3;
	public const int SHA512 = 4;

	// Create a new hash algorithm context.  Throws "NotImplementedException"
	// if the algorithm is not supported.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static IntPtr HashNew(int algorithm);

	// Reset a hash context back to its original state.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static void HashReset(IntPtr state);

	// Update a hash context with a block of bytes.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static void HashUpdate(IntPtr state, byte[] buffer,
										 int offset, int count);

	// Finalize a hash context and get the hash value.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static void HashFinal(IntPtr state, byte[] hash);

	// Free a hash context that is no longer required.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static void HashFree(IntPtr state);

}; // class CryptoMethods

#endif // !ECMA_COMPAT

}; // namespace Platform
