/*
 * AsymmetricAlgorithm.cs - Implementation of the
 *		"System.Security.Cryptography.AsymmetricAlgorithm" class.
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

public abstract class AsymmetricAlgorithm
{
	// Storage for values from subclasses.
	protected int KeySizeValue;
	protected KeySizes[] LegalKeySizesValue;

	// Create an instance of the default asymmetric algorithm.
	public static AsymmetricAlgorithm Create()
			{
				return (AsymmetricAlgorithm)
					(CryptoConfig.CreateFromName
						(CryptoConfig.AsymmetricDefault, null));
			}

	// Create an instance of a specific asymmetric algorithm, by name.
	public static AsymmetricAlgorithm Create(String algName)
			{
				return (AsymmetricAlgorithm)
					(CryptoConfig.CreateFromName(algName, null));
			}

	// Get the name of the key exchange algorithm.
	public abstract String KeyExchangeAlgorithm { get; }

	// Get or set the size of the key modulus, in bits.
	public virtual int KeySize
			{
				get
				{
					return KeySizeValue;
				}
				set
				{
					KeySizes.Validate(LegalKeySizesValue, value,
									  "Crypto_InvalidKeySize");
					KeySizeValue = value;
				}
			}

	// Get the legal key sizes for this asymmetric algorithm.
	public virtual KeySizes[] LegalKeySizes
			{
				get
				{
					return LegalKeySizesValue;
				}
			}

	// Get the name of the signature algorithm.
	public abstract String SignatureAlgorithm { get; }

	// Reconstruct an asymmetric algorithm object from an XML string.
	public abstract void FromXmlString(String xmlString);

	// Get the XML string representation of an asymmetric algorithm object.
	public abstract String ToXmlString(bool includePrivateParameters);

}; // class AsymmetricAlgorithm

#endif // !ECMA_COMPAT

}; // namespace System.Security.Cryptography
