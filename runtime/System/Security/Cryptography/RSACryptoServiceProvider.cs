/*
 * RSACryptoServiceProvider.cs - Implementation of the
 *		"System.Security.Cryptography.RSACryptoServiceProvider" class.
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
using System.IO;
using Platform;

// Note: this class assumes that the RSA parameters can be accessed
// in main memory.  This may not necessarily be the case if the
// system is using smart cards or some other kind of crypto dongle.
// We will modify this class when and if that becomes an issue.
//
// This implementation is based on the RSA description in the second
// edition of "Applied Cryptography: Protocols, Algorithms, and
// Source Code in C", Bruce Schneier, John Wiley & Sons, Inc, 1996.

public class RSACryptoServiceProvider : RSA
{
	// Internal state.
	private bool persistKey;
	private RSAParameters rsaParams;

	// Constructors.
	public RSACryptoServiceProvider()
			: this(0, null) {}
	public RSACryptoServiceProvider(CspParameters parameters)
			: this(0, parameters) {}
	public RSACryptoServiceProvider(int dwKeySize)
			: this(dwKeySize, null) {}
	public RSACryptoServiceProvider(int dwKeySize, CspParameters parameters)
			{
				byte[] key;
				int result;

				// See "DSACryptoServiceProvider" for explainatory comments.
				if(parameters != null && parameters.KeyContainerName != null)
				{
					// Attempt to get an RSA key from the user's keychain.
					key = CryptoMethods.GetKey(CryptoMethods.RSAEncrypt,
											   parameters.KeyContainerName,
											   parameters.Flags,
											   out result);
					if(key != null)
					{
						// The "ASN1ToPublic" method will determine if
						// the key is X.509, bare public, or private.
						rsaParams.ASN1ToPublic(key, 0, key.Length);
						key.Initialize();
						persistKey = true;
					}
					else if(result == CryptoMethods.UnknownKey)
					{
						throw new CryptographicException
							(_("Crypt_UnknownKey"),
							 parameters.KeyContainerName);
					}
					else if(result == CryptoMethods.NotPermitted)
					{
						throw new CryptographicException
							(_("Crypt_NoKeyAccess"),
							 parameters.KeyContainerName);
					}
					else if(result == CryptoMethods.GenerateKey)
					{
						// Generate a new key for the user.
						// TODO
					}
				}
			}

	// Destructor.
	~RSACryptoServiceProvider()
			{
				rsaParams.Clear();
			}

	// Get the name of the key exchange algorithm.
	public override String KeyExchangeAlgorithm
			{
				get
				{
					return "RSA-PKCS1-KeyEx";
				}
			}

	// Get or set the size of the key modulus, in bits.
	public override int KeySize
			{
				get
				{
					return KeySizeValue;
				}
				set
				{
					base.KeySize = value;
				}
			}

	// Determine if the key should be persisted in the CSP.
	public bool PersistKeyInCsp
			{
				get
				{
					return persistKey;
				}
				set
				{
					persistKey = value;
				}
			}

	// Get the name of the signature algorithm.
	public override String SignatureAlgorithm
			{
				get
				{
					// W3C identifier for the RSA-SHA1 algorithm.
					return "http://www.w3.org/2000/09/xmldsig#rsa-sha1";
				}
			}

	// Decrypt a value using the RSA private key and optional OAEP padding.
	public byte[] Decrypt(byte[] rgb, bool fOAEP)
			{
				// TODO
				return null;
			}

	// Decrypt a value using the RSA private key.
	public override byte[] DecryptValue(byte[] rgb)
			{
				return Decrypt(rgb, false);
			}

	// Encrypt a value using the RSA private key and optional OAEP padding.
	public byte[] Encrypt(byte[] rgb, bool fOAEP)
			{
				// TODO
				return null;
			}

	// Encrypt a value using the RSA public key.
	public override byte[] EncryptValue(byte[] rgb)
			{
				return Encrypt(rgb, false);
			}

	// Export the parameters for RSA signature generation.
	public override RSAParameters ExportParameters
				(bool includePrivateParameters)
			{
				if(rsaParams.Modulus == null)
				{
					throw new CryptographicException
						(_("Crypto_RSAParamsNotSet"));
				}
				if(includePrivateParameters)
				{
					return rsaParams;
				}
				else
				{
					return rsaParams.ClonePublic();
				}
			}

	// Import the parameters for DSA signature generation.
	public override void ImportParameters(RSAParameters parameters)
			{
				// We need at least Modulus and Exponent for public key ops.
				if(parameters.Modulus == null || parameters.Exponent == null ||
				   CryptoMethods.NumZero(parameters.Modulus) ||
				   CryptoMethods.NumZero(parameters.Exponent))
				{
					throw new CryptographicException
						(_("Crypto_InvalidRSAParams"));
				}
				rsaParams = parameters;
			}

	// Convert a "halg" value into a HashAlgorithm instance.
	private static HashAlgorithm ObjectToHashAlg(Object halg)
			{
				HashAlgorithm alg;
				if(halg == null)
				{
					throw new ArgumentNullException("halg");
				}
				else if(halg is String)
				{
					alg = HashAlgorithm.Create((String)halg);
				}
				else if(halg is HashAlgorithm)
				{
					alg = (HashAlgorithm)halg;
				}
				else if(halg is Type &&
				        ((Type)halg).IsSubclassOf(typeof(HashAlgorithm)))
				{
					alg = (HashAlgorithm)(Activator.CreateInstance((Type)halg));
				}
				else
				{
					throw new ArgumentException
						(_("Crypto_NeedsHash"), "halg");
				}
				if(!(alg is SHA1) && !(alg is MD5))
				{
					throw new CryptographicException
						(_("Crypto_MD5OrSHA1"));
				}
				return alg;
			}

	// Perform a sign operation.
	private byte[] Sign(byte[] rgbHash, Type alg)
			{
				// TODO
				return null;
			}

	// Compute a hash value over a buffer and sign it.
	public byte[] SignData(byte[] buffer, Object halg)
			{
				HashAlgorithm alg = ObjectToHashAlg(halg);
				return Sign(alg.ComputeHash(buffer), alg.GetType());
			}

	// Compute a hash value over a buffer fragment and sign it.
	public byte[] SignData(byte[] buffer, int offset, int count, Object halg)
			{
				HashAlgorithm alg = ObjectToHashAlg(halg);
				return Sign(alg.ComputeHash(buffer, offset, count),
							alg.GetType());
			}

	// Compute a hash value over a stream and sign it.
	public byte[] SignData(Stream inputStream, Object halg)
			{
				HashAlgorithm alg = ObjectToHashAlg(halg);
				return Sign(alg.ComputeHash(inputStream), alg.GetType());
			}

	// Sign an explicit hash value.
	public byte[] SignHash(byte[] rgbHash, String str)
			{
				if(str == null || str == "SHA1")
				{
					return Sign(rgbHash, typeof(SHA1CryptoServiceProvider));
				}
				else if(str == "MD5")
				{
					return Sign(rgbHash, typeof(MD5CryptoServiceProvider));
				}
				else
				{
					throw new CryptographicException
						(_("Crypto_MD5OrSHA1"));
				}
			}

	// Perform a verify operation.
	private bool Verify(byte[] rgbHash, Type alg, byte[] rgbSignature)
			{
				// TODO
				return false;
			}

	// Verify the signature on a buffer of data.
	public bool VerifyData(byte[] buffer, Object halg, byte[] rgbSignature)
			{
				HashAlgorithm alg = ObjectToHashAlg(halg);
				return Verify(alg.ComputeHash(buffer), alg.GetType(),
							  rgbSignature);
			}

	// Verify a signature from an explicit hash value.
	public bool VerifyHash(byte[] rgbHash, String str, byte[] rgbSignature)
			{
				if(str == null || str == "SHA1")
				{
					return Verify(rgbHash, typeof(SHA1CryptoServiceProvider),
								  rgbSignature);
				}
				else if(str == "MD5")
				{
					return Verify(rgbHash, typeof(MD5CryptoServiceProvider),
								  rgbSignature);
				}
				else
				{
					throw new CryptographicException
						(_("Crypto_MD5OrSHA1"));
				}
			}

}; // class RSACryptoServiceProvider

#endif // !ECMA_COMPAT

}; // namespace System.Security.Cryptography
