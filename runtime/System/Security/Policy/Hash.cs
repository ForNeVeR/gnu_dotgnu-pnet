/*
 * Hash.cs - Implementation of the
 *		"System.Security.Policy.Hash" class.
 *
 * Copyright (C) 2003  Southern Storm Software, Pty Ltd.
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

namespace System.Security.Policy
{

#if !ECMA_COMPAT

using System.Reflection;
using System.Runtime.Serialization;
using System.Security.Cryptography;

[Serializable]
public sealed class Hash : ISerializable
{
	// Internal state.
	private Assembly assembly;
	private byte[] md5;
	private byte[] sha1;
	private byte[] dataToHash;

	// Constructor.
	public Hash(Assembly assembly)
			{
				if(assembly == null)
				{
					throw new ArgumentNullException("assembly");
				}
				this.assembly = assembly;
			}
	[TODO]
	internal Hash(SerializationInfo info, StreamingContext context)
			{
				// TODO
			}

	// Get the MD5 hash value for the assembly.
	public byte[] MD5
			{
				get
				{
					if(md5 == null)
					{
						md5 = GenerateHash(MD5.Create());
					}
					return md5;
				}
			}

	// Get the SHA1 hash value for the assembly.
	public byte[] SHA1
			{
				get
				{
					if(sha1 == null)
					{
						sha1 = GenerateHash(SHA1.Create());
					}
					return sha1;
				}
			}

	// Generate the hash value for this assembly using a given algorith.
	[TODO]
	public byte[] GenerateHash(HashAlgorithm hashAlg)
			{
				if(hashAlg == null)
				{
					throw new ArgumentNullException("hashAlg");
				}
				if(dataToHash == null)
				{
					// TODO: get the data to be hashed.
				}
				hashAlg.Initialize();
				return hashAlg.ComputeHash(dataToHash);
			}

	// Implement the ISerialization interface.
	[TODO]
	public void GetObjectData(SerializationInfo info, StreamingContext context)
			{
				// TODO
			}

	// Convert this object into a string.
	[TODO]
	public override String ToString()
			{
				// TODO
				return null;
			}

}; // class Hash

#endif // !ECMA_COMPAT

}; // namespace System.Security.Policy
