/*
 * RC2.cs - Implementation of the "System.Security.Cryptography.RC2" class.
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

public abstract class RC2 : SymmetricAlgorithm
{

	// Constructor.
	public RC2()
			: base()
			{
				KeySizeValue = 128;
				BlockSizeValue = 64;
				FeedbackSizeValue = 64;
				LegalBlockSizesValue = new KeySizes [1];
				LegalBlockSizesValue[0] = new KeySizes(64, 64, 64);
				LegalKeySizesValue = new KeySizes [1];
				LegalKeySizesValue[0] = new KeySizes(128, 128, 128);
			}

	// Create a new instance of the RC2 algorithm.
	public new static RC2 Create()
			{
				return (RC2)(CryptoConfig.CreateFromName
						(CryptoConfig.RC2Default, null));
			}
	public new static RC2 Create(String algName)
			{
				return (RC2)(CryptoConfig.CreateFromName(algName, null));
			}

}; // class RC2

#endif // !ECMA_COMPAT

}; // namespace System.Security.Cryptography
