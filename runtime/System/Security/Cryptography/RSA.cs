/*
 * RSA.cs - Implementation of the
 *		"System.Security.Cryptography.RSA" class.
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

public abstract class RSA : AsymmetricAlgorithm
{
	// Constructor.
	public RSA() {}

	// Create an instance of the default RSA implementation.
	public new static RSA Create()
			{
				return (RSA)(CryptoConfig.CreateFromName
						(CryptoConfig.RSADefault, null));
			}

	// Create an instance of a specific DSA implementation, by name.
	public new static RSA Create(String algName)
			{
				return (RSA)(CryptoConfig.CreateFromName(algName, null));
			}

	// Decrypt a value using the RSA private key.
	public abstract byte[] DecryptValue(byte[] rgb);

	// Encrypt a value using the RSA public key.
	public abstract byte[] EncryptValue(byte[] rgb);

	// Export the parameters for RSA signature generation.
	public abstract RSAParameters ExportParameters
				(bool includePrivateParameters);

	// Import the parameters for RSA signature generation.
	public abstract void ImportParameters(RSAParameters parameters);

	// Reconstruct an asymmetric algorithm object from an XML string.
	[TODO]
	public override void FromXmlString(String xmlString)
			{
				// TODO
			}

	// Get the XML string representation of an asymmetric algorithm object.
	[TODO]
	public override String ToXmlString(bool includePrivateParameters)
			{
				// TODO
				return null;
			}

}; // class RSA

#endif // !ECMA_COMPAT

}; // namespace System.Security.Cryptography
