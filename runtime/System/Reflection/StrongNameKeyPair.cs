/*
 * StrongNameKeyPair.cs - Implementation of the
 *		"System.Reflection.StrongNameKeyPair" class.
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

namespace System.Reflection
{

#if !ECMA_COMPAT

using System;
using System.IO;

public class StrongNameKeyPair
{
	// Internal state.
	private byte[] key;

	// Constructors.
	[TODO]
	public StrongNameKeyPair(byte[] keyPairArray)
			{
				// TODO
			}
	[TODO]
	public StrongNameKeyPair(FileStream keyPairFile)
			{
				// TODO
			}
	[TODO]
	public StrongNameKeyPair(String keyPairContainer)
			{
				// TODO
			}

	// Get the public key.
	public byte[] PublicKey
			{
				get
				{
					return key;
				}
			}

}; // class StrongNameKeyPair

#endif // !ECMA_COMPAT

}; // namespace System.Reflection
