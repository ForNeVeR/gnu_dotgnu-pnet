/*
 * AssemblyName.cs - Implementation of the
 *		"System.Reflection.AssemblyName" class.
 *
 * Copyright (C) 2001  Southern Storm Software, Pty Ltd.
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
using System.Configuration.Assemblies;

public class AssemblyName : ICloneable
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

	// Get the assembly name for a specific file.
	public static AssemblyName GetAssemblyName(String assemblyFile)
			{
				// TODO
				return null;
			}

	// Get or set the code base for the assembly name.
	public virtual String CodeBase
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
	public virtual CultureInfo CultureInfo
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

	// Get or set the assembly name flags.
	public virtual AssemblyNameFlags Flags
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
	public virtual String FullName
			{
				get
				{
					// TODO
					return name;
				}
			}

	// Get or set the hash algorithm for this assembly name.
	public virtual AssemblyHashAlgorithm HashAlgorithm
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
	public virtual StrongNameKeyPair KeyPair
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
	public virtual String Name
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
	public virtual Version Version
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
	public virtual AssemblyVersionCompatibility VersionCompatibility
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
	public virtual Object Clone()
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

}; // class AssemblyName

#endif // !ECMA_COMPAT

}; // namespace System.Reflection
