/*
 * CryptoAPITransform.cs - Implementation of the
 *		"System.Security.Cryptography.CryptoAPITransform" class.
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

namespace System.Security.Cryptography
{

#if !ECMA_COMPAT

using System;

// We don't use native cryptographic service providers in this
// implementation, so we just wrap some other transform object.

public sealed class CryptoAPITransform : ICryptoTransform
{
	// Internal state.
	private ICryptoTransform transform;

	// Constructor.
	internal CryptoAPITransform(ICryptoTransform transform)
			{
				this.transform = transform;
			}

	// Destructor.
	~CryptoAPITransform()
			{
				// Nothing to do here.
			}

	// Determine if multiple blocks can be transformed.
	public bool CanTransformMultipleBlocks
			{
				get
				{
					return transform.CanTransformMultipleBlocks;
				}
			}

	// Get the input block size.
	public int InputBlockSize
			{
				get
				{
					return transform.InputBlockSize;
				}
			}

	// Get the output block size.
	public int OutputBlockSize
			{
				get
				{
					return transform.OutputBlockSize;
				}
			}

	// Get the key handle.
	public IntPtr KeyHandle
			{
				get
				{
					// We don't support key handles in this implementation.
					return IntPtr.Zero;
				}
			}

	// Transform an input block into an output block.
	public int TransformBlock(byte[] inputBuffer, int inputOffset,
							  int inputCount, byte[] outputBuffer,
							  int outputOffset)
			{
				return transform.TransformBlock(inputBuffer, inputOffset,
												inputCount, outputBuffer,
												outputOffset);
			}

	// Transform the final input block into a hash value.
	public byte[] TransformFinalBlock(byte[] inputBuffer,
									  int inputOffset,
									  int inputCount)
			{
				return transform.TransformFinalBlock(inputBuffer, inputOffset,
													 inputCount);
			}

}; // class CryptoAPITransform

#endif // !ECMA_COMPAT

}; // namespace System.Security.Cryptography
