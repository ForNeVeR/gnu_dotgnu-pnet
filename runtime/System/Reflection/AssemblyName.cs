/*
 * AssemblyName.cs - Implementation of the
 *		"System.Reflection.AssemblyName" class.
 *
 * Copyright (C) 2001, 2003  Southern Storm Software, Pty Ltd.
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
using System.Globalization;
using System.Runtime.CompilerServices;
using System.Runtime.Serialization;
using System.Configuration.Assemblies;

public sealed class AssemblyName
	: ICloneable
#if CONFIG_SERIALIZATION
	, ISerializable, IDeserializationCallback
#endif
{
	// Internal state.
	private String codeBase;
	private CultureInfo culture;
	private AssemblyNameFlags flags;
	private String name;
	private AssemblyHashAlgorithm hashAlg;
	private StrongNameKeyPair keyPair;
	private Version version;
	private AssemblyVersionCompatibility versionCompat;
	private byte[] publicKey;
	private byte[] publicKeyToken;

	// Constructor.
	public AssemblyName() {}
#if CONFIG_SERIALIZATION
	[TODO]
	internal AssemblyName(SerializationInfo info,
						  StreamingContext context)
			{
				// TODO
			}
#endif

	// Get the assembly name for a specific file.
	[TODO]
	public static AssemblyName GetAssemblyName(String assemblyFile)
			{
				// TODO
				return null;
			}

	// Get or set the code base for the assembly name.
	public String CodeBase
			{
				get
				{
					return codeBase;
				}
				set
				{
					codeBase = value;
				}
			}

	// Get or set the culture associated with the assembly name.
	public CultureInfo CultureInfo
			{
				get
				{
					return culture;
				}
				set
				{
					culture = value;
				}
			}

	// Get the escaped code base for the assembly name.
	[TODO]
	public String EscapedCodeBase
			{
				get
				{
					// TODO
					return codeBase;
				}
			}

	// Get or set the assembly name flags.
	public AssemblyNameFlags Flags
			{
				get
				{
					return flags;
				}
				set
				{
					flags = value;
				}
			}

	// Get the full name of the assembly.
	[TODO]
	public String FullName
			{
				get
				{
					// TODO
					return name;
				}
			}

	// Get or set the hash algorithm for this assembly name.
	public AssemblyHashAlgorithm HashAlgorithm
			{
				get
				{
					return hashAlg;
				}
				set
				{
					hashAlg = value;
				}
			}

	// Get or set the key pair for this assembly name.
	public StrongNameKeyPair KeyPair
			{
				get
				{
					return keyPair;
				}
				set
				{
					keyPair = value;
				}
			}

	// Get or set the simple name of the assembly name.
	public String Name
			{
				get
				{
					return name;
				}
				set
				{
					name = value;
				}
			}

	// Get or set the version information of the assembly name.
	public Version Version
			{
				get
				{
					return version;
				}
				set
				{
					version = value;
				}
			}

	// Get or set the version compatibility value for the assembly name.
	public AssemblyVersionCompatibility VersionCompatibility
			{
				get
				{
					return versionCompat;
				}
				set
				{
					versionCompat = value;
				}
			}

	// Clone this object.
	public Object Clone()
			{
				return MemberwiseClone();
			}

	// Get the public key for the assembly's originator.
	public byte[] GetPublicKey()
			{
				return publicKey;
			}

	// Set the public key for the assembly's originator.
	public void SetPublicKey(byte[] publicKey)
			{
				this.publicKey = publicKey;
			}

	// Get the public key token for the assembly's originator.
	public byte[] GetPublicKeyToken()
			{
				return publicKeyToken;
			}

	// Set the public key token for the assembly's originator.
	public void SetPublicKeyToken(byte[] publicKeyToken)
			{
				this.publicKeyToken = publicKeyToken;
			}

	// Convert this assembly name into a string.
	public override String ToString()
			{
				return FullName;
			}

#if CONFIG_SERIALIZATION

	// Get the serialization data for this object.
	[TODO]
	public void GetObjectData(SerializationInfo info, StreamingContext context)
			{
				if(info == null)
				{
					throw new ArgumentNullException("info");
				}
				// TODO
			}

	// Handle a deserialization callback on this object.
	public void OnDeserialization(Object sender)
			{
				// Nothing to do here.
			}

#endif // CONFIG_SERIALIZATION

}; // class AssemblyName

#endif // !ECMA_COMPAT

}; // namespace System.Reflection
