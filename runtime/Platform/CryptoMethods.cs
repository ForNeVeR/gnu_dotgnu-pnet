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
	public const int MD5       = 0;
	public const int SHA1      = 1;
	public const int SHA256    = 2;
	public const int SHA384    = 3;
	public const int SHA512    = 4;
	public const int DES       = 5;
	public const int TripleDES = 6;
	public const int RC2       = 7;
	public const int Rijndael  = 8;

	// Determine if a particular algorithm is supported.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static bool AlgorithmSupported(int algorithm);

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

	// Determine if a DES key value is "semi-weak".
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static bool IsSemiWeakKey(byte[] key, int offset);

	// Determine if a DES key value is "weak".
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static bool IsWeakKey(byte[] key, int offset);

	// Determine if two DES keys are the same.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static bool SameKey(byte[] key1, int offset1,
									  byte[] key2, int offset2);

	// Generate a number of bytes of random material.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static void GenerateRandom
				(byte[] buf, int offset, int count);

	// Create a symmetric block encryption context.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static IntPtr EncryptCreate(int algorithm, byte[] key);

	// Create a symmetric block decryption context.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static IntPtr DecryptCreate(int algorithm, byte[] key);

	// Encrypt a single block.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static void Encrypt(IntPtr state, byte[] inBuffer,
									  int inOffset, byte[] outBuffer,
									  int outOffset);

	// Decrypt a single block.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static void Decrypt(IntPtr state, byte[] inBuffer,
									  int inOffset, byte[] outBuffer,
									  int outOffset);

	// Free a symmetric block context.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static void SymmetricFree(IntPtr state);

}; // class CryptoMethods

#endif // !ECMA_COMPAT

}; // namespace Platform
