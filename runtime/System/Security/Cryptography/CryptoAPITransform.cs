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
using Platform;

// This is a helper class that is used by the DES, TripleDES, RC2,
// and Rijndael symmetric algorithm classes to create encryptor and
// decryptor objects that call through to the engine to do the work.

public sealed class CryptoAPITransform : ICryptoTransform
{
	// Internal state.
	private IntPtr state;
	private byte[] iv;
	private int blockSize;
	private int feedbackBlockSize;
	private CipherMode mode;
	private PaddingMode padding;
	private bool encrypt;

	// Constructor.
	internal CryptoAPITransform(int algorithm, byte[] iv, byte[] key,
								int blockSize, int feedbackBlockSize,
								CipherMode mode, PaddingMode padding,
								bool encrypt)
			{
				if(encrypt)
				{
					state = CryptoMethods.EncryptCreate(algorithm, key);
				}
				else
				{
					state = CryptoMethods.DecryptCreate(algorithm, key);
				}
				this.iv = (byte[])(iv.Clone());
				this.blockSize = blockSize;
				this.feedbackBlockSize = feedbackBlockSize;
				this.mode = mode;
				this.padding = padding;
				this.encrypt = encrypt;
			}

	// Destructor.
	~CryptoAPITransform()
			{
				if(state != IntPtr.Zero)
				{
					CryptoMethods.SymmetricFree(state);
					state = IntPtr.Zero;
				}
			}

	// Determine if multiple blocks can be transformed.
	public bool CanTransformMultipleBlocks
			{
				get
				{
					return true;
				}
			}

	// Get the input block size.
	public int InputBlockSize
			{
				get
				{
					return blockSize;
				}
			}

	// Get the output block size.
	public int OutputBlockSize
			{
				get
				{
					return blockSize;
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
				// TODO
				return 0;
			}

	// Transform the final input block.
	public byte[] TransformFinalBlock(byte[] inputBuffer,
									  int inputOffset,
									  int inputCount)
			{
				// TODO
				return null;
			}

}; // class CryptoAPITransform

#endif // !ECMA_COMPAT

}; // namespace System.Security.Cryptography
