/*
 * DSA.cs - Implementation of the
 *		"System.Security.Cryptography.DSA" class.
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

public abstract class DSA : AsymmetricAlgorithm
{

	// Create an instance of the default DSA implementation.
	public new static DSA Create()
			{
				return (DSA)(CryptoConfig.CreateFromName
						(CryptoConfig.DSADefault, null));
			}

	// Create an instance of a specific DSA implementation, by name.
	public new static DSA Create(String algName)
			{
				return (DSA)(CryptoConfig.CreateFromName(algName, null));
			}

	// Create a DSA signature for the specified data.
	public abstract byte[] CreateSignature(byte[] rgbHash);

	// Export the parameters for DSA signature generation.
	public abstract DSAParameters ExportParameters
				(bool includePrivateParameters);

	// Import the parameters for DSA signature generation.
	public abstract void ImportParameters(DSAParameters parameters);

	// Verify a DSA signature.
	public abstract bool VerifySignature(byte[] rgbHash, byte[] rgbSignature);

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

}; // class DSA

#endif // !ECMA_COMPAT

}; // namespace System.Security.Cryptography
