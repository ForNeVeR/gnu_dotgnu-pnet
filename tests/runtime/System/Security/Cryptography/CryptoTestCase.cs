/*
 * CryptoTestCase.cs - encapsulate a cryptographic algorithm test case.
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

using CSUnit;
using System;
using System.Reflection;
using System.Text;
using System.Security.Cryptography;

public class CryptoTestCase : TestCase
{

	// Constructor.
	public CryptoTestCase(String name)
			: base(name)
			{
				// Nothing to do here.	
			}

	// Set up for the tests.
	protected override void Setup()
			{
				// Nothing to do here.
			}

	// Clean up after the tests.
	protected override void Cleanup()
			{
				// Nothing to do here.
			}
	
	// Determine if two byte blocks are identical.
	public static bool IdenticalBlock(byte[] block1, int offset1,
									  byte[] block2, int offset2,
									  int length)
			{
				while(length > 0)
				{
					if(block1[offset1++] != block2[offset2++])
					{
						return false;
					}
					--length;
				}
				return true;
			}

	// Run a symmetric algorithm test.
	protected void RunSymmetric(SymmetricAlgorithm alg, byte[] key,
								byte[] plaintext, byte[] expected)
			{
				// Set up the algorithm the way we want.
				alg.Mode = CipherMode.ECB;
				alg.Padding = PaddingMode.None;

				// Create an encryptor and run the test forwards.
				ICryptoTransform encryptor = alg.CreateEncryptor(key, null);
				byte[] output = new byte [plaintext.Length * 2];
				byte[] tail;
				int len = encryptor.TransformBlock
						(plaintext, 0, plaintext.Length,
						 output, 0);
				AssertEquals("ECB encrypt length mismatch",
							 len, expected.Length);
				tail = encryptor.TransformFinalBlock
						(plaintext, 0, 0);
				AssertNotNull("ECB encrypt tail should be non-null");
				AssertEquals("ECB encrypt tail should be zero length",
							 tail.Length, 0);
				if(!IdenticalBlock(expected, 0, output, 0,
								   expected.Length))
				{
					Fail("did not encrypt to the expected output");
				}

				// Create a decryptor and run the test backwards.
				ICryptoTransform decryptor = alg.CreateDecryptor(key, null);
				len = decryptor.TransformBlock
						(expected, 0, expected.Length, output, 0);
				AssertEquals("ECB decrypt length mismatch",
							 len, expected.Length);
				tail = decryptor.TransformFinalBlock
						(expected, 0, 0);
				AssertNotNull("ECB decrypt tail should be non-null");
				AssertEquals("ECB decrypt tail should be zero length",
							 tail.Length, 0);
				if(!IdenticalBlock(plaintext, 0, output, 0,
								   plaintext.Length))
				{
					Fail("did not decrypt to the original plaintext");
				}
			}
	protected void RunSymmetric(String name, byte[] key,
								byte[] plaintext, byte[] expected)
			{
				int index = name.IndexOf(':');
				if(index != -1)
				{
					// Use the default algorithm.
					Type type = Type.GetType("System.Security.Cryptography." +
											 name.Substring(0, index),
											 false, false);
					Object[] args;
					if((index + 1) < name.Length)
					{
						args = new Object [1];
						args[0] = name.Substring(index + 1);
					}
					else
					{
						args = new Object [0];
					}
					SymmetricAlgorithm alg = (SymmetricAlgorithm)
						(type.InvokeMember("Create",
										   BindingFlags.DeclaredOnly |
										   BindingFlags.Static |
										   BindingFlags.Public |
										   BindingFlags.InvokeMethod,
										   null, null, args));
					AssertEquals("default key size is wrong",
					             alg.KeySize, key.Length * 8);
					RunSymmetric(alg, key, plaintext, expected);
				}
				else
				{
					// Use the specified algorithm.
					RunSymmetric(SymmetricAlgorithm.Create(name), key,
								 plaintext, expected);
				}
			}

	// Run a hash algorithm test.
	protected void RunHash(HashAlgorithm alg, String value, byte[] expected)
			{
				// Make sure that the hash size is what we expect.
				AssertEquals("hash size is incorrect",
							 alg.HashSize, expected.Length * 8);

				// Convert the string form of the input into a byte array.
				byte[] input = Encoding.ASCII.GetBytes(value);

				// Get the hash value over the input.
				byte[] hash = alg.ComputeHash(input);

				// Compare the hash with the expected value.
				AssertNotNull("returned hash was null", hash);
				AssertEquals("hash length is wrong", hash.Length,
							 expected.Length);
				if(!IdenticalBlock(hash, 0, expected, 0, expected.Length))
				{
					Fail("incorrect hash value produced");
				}
			}
	protected void RunHash(String name, String value, byte[] expected)
			{
				int index = name.IndexOf(':');
				if(index != -1)
				{
					// Use the default algorithm.
					Type type = Type.GetType("System.Security.Cryptography." +
											 name.Substring(0, index),
											 false, false);
					Object[] args;
					if((index + 1) < name.Length)
					{
						args = new Object [1];
						args[0] = name.Substring(index + 1);
					}
					else
					{
						args = new Object [0];
					}
					HashAlgorithm alg = (HashAlgorithm)
						(type.InvokeMember("Create",
										   BindingFlags.DeclaredOnly |
										   BindingFlags.Static |
										   BindingFlags.Public |
										   BindingFlags.InvokeMethod,
										   null, null, args));
					RunHash(alg, value, expected);
				}
				else
				{
					// Use the specified algorithm.
					RunHash(HashAlgorithm.Create(name), value, expected);
				}
			}

	// Check that a size value is in a size list.
	private void CheckSize(String msg, KeySizes[] sizes, int value)
			{
				foreach(KeySizes size in sizes)
				{
					if(value >= size.MinSize && value <= size.MaxSize &&
					   ((value - size.MinSize) % size.SkipSize) == 0)
					{
						return;
					}
				}
				Fail(msg);
			}

	// Test the properties on a symmetric algorithm instance.
	protected void SymmetricPropertyTest(SymmetricAlgorithm alg,
										 int expectedKeySize,
										 int expectedBlockSize)
			{
				// Check the initial property values.
				AssertEquals("initial key size is incorrect",
							 alg.KeySize, expectedKeySize);
				AssertEquals("initial block size is incorrect",
							 alg.BlockSize, expectedBlockSize);
				AssertEquals("initial feedback block size is incorrect",
							 alg.FeedbackSize, expectedBlockSize);
				AssertEquals("initial cipher mode is incorrect",
							 alg.Mode, CipherMode.CBC);
				AssertEquals("initial padding mode is incorrect",
							 alg.Padding, PaddingMode.PKCS7);
				AssertNotNull("legal key size array is null",
							  alg.LegalKeySizes);
				AssertNotNull("legal block size array is null",
							  alg.LegalBlockSizes);

				// Check that the size values are initially valid.
				CheckSize("initial key size is not legal",
						  alg.LegalKeySizes, alg.KeySize);
				CheckSize("initial block size is not legal",
						  alg.LegalBlockSizes, alg.BlockSize);

				// Try setting the key size to all legal values.

				// TODO: check automatic key and IV generation
			}

}; // CryptoTestCase
