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
	internal const String DESDefault =
		"System.Security.Cryptography.DES";
	internal const String TripleDESDefault =
		"System.Security.Cryptography.TripleDES";
	internal const String RC2Default =
		"System.Security.Cryptography.RC2";
	internal const String RijndaelDefault =
		"System.Security.Cryptography.Rijndael";
	internal const String KeyedHashDefault =
		"System.Security.Cryptography.KeyedHashAlgorithm";
	internal const String RNGDefault =
		"System.Security.Cryptography.RandomNumberGenerator";

	// Table that maps algorithm names to implementations.
	private static readonly Object[] algorithms = {

			"SHA",				typeof(SHA1CryptoServiceProvider),
			"SHA1",				typeof(SHA1CryptoServiceProvider),
			"System.Security.Cryptography.SHA1",
								typeof(SHA1CryptoServiceProvider),
			"System.Security.Cryptography.HashAlgorithm",
								typeof(SHA1CryptoServiceProvider),

			"MD5",				typeof(MD5CryptoServiceProvider),
			"System.Security.Cryptography.MD5",
								typeof(MD5CryptoServiceProvider),

			"SHA256",			typeof(SHA256Managed),
			"SHA-256",			typeof(SHA256Managed),
			"System.Security.Cryptography.SHA256",
								typeof(SHA256Managed),

			"SHA384",			typeof(SHA384Managed),
			"SHA-384",			typeof(SHA384Managed),
			"System.Security.Cryptography.SHA384",
								typeof(SHA384Managed),

			"SHA512",			typeof(SHA512Managed),
			"SHA-512",			typeof(SHA512Managed),
			"System.Security.Cryptography.SHA512",
								typeof(SHA512Managed),

#if false
			"RSA",				typeof(RSACryptoServiceProvider),
			"System.Security.Cryptography.RSA",
								typeof(RSACryptoServiceProvider),
			"System.Security.Cryptography.AsymmetricAlgorithm",
								typeof(RSACryptoServiceProvider),

			"DSA",				typeof(DSACryptoServiceProvider),
			"System.Security.Cryptography.DSA",
								typeof(DSACryptoServiceProvider),
#endif

			"RandomNumberGenerator",
								typeof(RNGCryptoServiceProvider),
			"System.Security.Cryptography.RandomNumberGenerator",
								typeof(RNGCryptoServiceProvider),

			"HMACSHA1",			typeof(HMACSHA1),
			"System.Security.Cryptography.HMACSHA1",
								typeof(HMACSHA1),
			"System.Security.Cryptography.KeyedHashAlgorithm",
								typeof(HMACSHA1),

			"MACTripleDES",		typeof(MACTripleDES),
			"System.Security.Cryptography.MACTripleDES",
								typeof(MACTripleDES),

			"DES",				typeof(DESCryptoServiceProvider),
			"System.Security.Cryptography.DES",
								typeof(DESCryptoServiceProvider),

			"3DES",				typeof(TripleDESCryptoServiceProvider),
			"TripleDES",		typeof(TripleDESCryptoServiceProvider),
			"Triple DES",		typeof(TripleDESCryptoServiceProvider),
			"System.Security.Cryptography.TripleDES",
								typeof(TripleDESCryptoServiceProvider),
			"System.Security.Cryptography.SymmetricAlgorithm",
								typeof(TripleDESCryptoServiceProvider),

			"RC2",				typeof(RC2CryptoServiceProvider),
			"System.Security.Cryptography.RC2",
								typeof(RC2CryptoServiceProvider),

			"Rijndael",			typeof(RijndaelManaged),
			"System.Security.Cryptography.Rijndael",
								typeof(RijndaelManaged),
		};

	// Table that maps algorithm names to OID's.
	private static readonly String[] oids = {
			"SHA1",								 "1.3.14.3.2.26",
			"System.Security.Cryptography.SHA1", "1.3.14.3.2.26",
			"System.Security.Cryptography.SHA1CryptoServiceProvider",
												 "1.3.14.3.2.26",
			"System.Security.Cryptography.SHA1Managed",
												 "1.3.14.3.2.26",

			"MD5",								 "1.2.840.113549.2.5",
			"System.Security.Cryptography.MD5CryptoServiceProvider",
												 "1.2.840.113549.2.5",
			"System.Security.Cryptography.MD5Managed",
												 "1.2.840.113549.2.5",
		};


	// Create an instance of a specific cryptographic object.
	public static Object CreateFromName(String name)
			{
				return CreateFromName(name, null);
			}
	public static Object CreateFromName(String name, Object[] args)
			{
				// Bail out if the name is incorrect.
				if(name == null)
				{
					throw new ArgumentNullException("name");
				}

				// Search the "algorithms" table for the name.
				int index;
				for(index = 0; index < algorithms.Length; index += 2)
				{
					if(name.Equals(algorithms[index]))
					{
						return Activator.CreateInstance
							((Type)(algorithms[index + 1]), args);
					}
				}

				// The algorithm does not exist.
				throw new CryptographicException
					(_("Crypto_NoProvider"), name);
			}

	// Determine the size of an OID component.
	private static int OIDSize(uint value)
			{
				if(value < (uint)(1 << 7))
				{
					return 1;
				}
				else if(value < (uint)(1 << 14))
				{
					return 2;
				}
				else if(value < (uint)(1 << 21))
				{
					return 3;
				}
				else if(value < (uint)(1 << 28))
				{
					return 4;
				}
				else
				{
					return 5;
				}
			}

	// Encode an OID component.
	private static int OIDEncode(byte[] result, int index, uint value)
			{
				if(value < (uint)(1 << 7))
				{
					result[index] = (byte)value;
					return 1;
				}
				else if(value < (uint)(1 << 14))
				{
					result[index] = (byte)(0x80 | (value >> 7));
					result[index + 1] = (byte)(value & 0x7F);
					return 2;
				}
				else if(value < (uint)(1 << 21))
				{
					result[index] = (byte)(0x80 | (value >> 14));
					result[index + 1] = (byte)(0x80 | (value >> 7));
					result[index + 2] = (byte)(value & 0x7F);
					return 3;
				}
				else if(value < (uint)(1 << 28))
				{
					result[index] = (byte)(0x80 | (value >> 21));
					result[index + 1] = (byte)(0x80 | (value >> 14));
					result[index + 2] = (byte)(0x80 | (value >> 7));
					result[index + 3] = (byte)(value & 0x7F);
					return 4;
				}
				else
				{
					result[index] = (byte)(0x80 | (value >> 28));
					result[index + 1] = (byte)(0x80 | (value >> 21));
					result[index + 2] = (byte)(0x80 | (value >> 14));
					result[index + 3] = (byte)(0x80 | (value >> 7));
					result[index + 4] = (byte)(value & 0x7F);
					return 5;
				}
			}

	// Encode an object identifier as a byte array.
	public static byte[] EncodeOID(String str)
			{
				// Bail out if the argument is null.
				if(str == null)
				{
					throw new ArgumentNullException("str");
				}

				// Count the number of components and validate the string.
				int count = 0;
				int index;
				char ch;
				bool prevIsDot = true;
				uint value = 0;
				int nbytes = -1;
				for(index = 0; index < str.Length; ++index)
				{
					ch = str[index];
					if(ch == '.')
					{
						if(prevIsDot)
						{
							break;
						}
						if((count == 0 && value > 2) ||
						   (count == 1 && value > 39))
						{
							prevIsDot = true;
							break;
						}
						nbytes += OIDSize(value);
						value = (uint)0;
						++count;
					}
					else if(ch < '0' || ch > '9')
					{
						prevIsDot = false;
						value = value * ((uint)10) + ((uint)ch) - (uint)'0';
					}
					else
					{
						prevIsDot = true;
						break;
					}
				}
				nbytes += OIDSize(value);
				if(prevIsDot || count < 2 || nbytes > 127)
				{
					throw new CryptographicException
						(_("Crypto_InvalidOID"), str);
				}

				// Create the byte array and fill it.
				byte[] result = new byte [nbytes + 2];
				count = 0;
				result[0] = (byte)0x06;
				result[1] = (byte)nbytes;
				nbytes = 2;
				value = (uint)0;
				for(index = 0; index < str.Length; ++index)
				{
					ch = str[index];
					if(ch == '.')
					{
						if(count == 0)
						{
							result[2] = (byte)(40 * value);
						}
						else if(count == 1)
						{
							result[2] = (byte)(result[2] + value);
							nbytes = 3;
						}
						else
						{
							nbytes += OIDEncode(result, nbytes, value);
						}
						value = (uint)0;
						++count;
					}
					else if(ch < '0' || ch > '9')
					{
						value = value * ((uint)10) + ((uint)ch) - (uint)'0';
					}
				}
				if(count == 1)
				{
					result[2] = (byte)(result[2] + value);
				}
				else
				{
					OIDEncode(result, nbytes, value);
				}

				// Done.
				return result;
			}

	// Get the OID of a named algorithm.
	public static String MapNameToOID(String name)
			{
				if(name == null)
				{
					throw new ArgumentNullException("name");
				}
				for(int index = 0; index < oids.Length; index += 2)
				{
					if(oids[index] == name)
					{
						return oids[index + 1];
					}
				}
				return null;
			}

}; // class CryptoConfig

#endif // !ECMA_COMPAT

}; // namespace System.Security.Cryptography