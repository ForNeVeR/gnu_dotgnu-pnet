/*
 * CryptoConfig.cs - Implementation of the
 *		"System.Security.Cryptography.CryptoConfig" class.
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

public class CryptoConfig
{
	// Default algorithms of various categories.
	internal const String AsymmetricDefault =
		"System.Security.Cryptography.AsymmetricAlgorithm";
	internal const String HashDefault =
		"System.Security.Cryptography.HashAlgorithm";
	internal const String SymmetricDefault =
		"System.Security.Cryptography.SymmetricAlgorithm";
	internal const String MD5Default =
		"System.Security.Cryptography.MD5";
	internal const String SHA1Default =
		"System.Security.Cryptography.SHA1";
	internal const String SHA256Default =
		"System.Security.Cryptography.SHA256";
	internal const String SHA384Default =
		"System.Security.Cryptography.SHA384";
	internal const String SHA512Default =
		"System.Security.Cryptography.SHA512";

	// Create an instance of a specific cryptographic object.
	public static Object CreateFromName(String name)
			{
				return CreateFromName(name, null);
			}
	public static Object CreateFromName(String name, Object[] args)
			{
				if(name == null)
				{
					throw new ArgumentNullException("name");
				}
				// TODO
				return null;
			}

	// Encode an object identifier as a byte array.
	public static byte[] EncodeOID(String str)
			{
				if(str == null)
				{
					throw new ArgumentNullException("str");
				}
				// TODO
				return null;
			}

	// Get the OID of a named algorithm.
	public static String MapNameToOID(String name)
			{
				if(name == null)
				{
					throw new ArgumentNullException("name");
				}
				// TODO
				return null;
			}

}; // class CryptoConfig

#endif // !ECMA_COMPAT

}; // namespace System.Security.Cryptography
