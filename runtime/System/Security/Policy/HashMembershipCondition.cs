/*
 * HashMembershipCondition.cs - Implementation of the
 *		"System.Security.Policy.HashMembershipCondition" class.
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

using System.Security.Permissions;
using System.Security.Cryptography;

[Serializable]
public sealed class HashMembershipCondition
	: IMembershipCondition, ISecurityEncodable, ISecurityPolicyEncodable
{
	// Internal state.
	private HashAlgorithm hashAlg;
	private byte[] value;

	// Constructor.
	public HashMembershipCondition(HashAlgorithm hashAlg, byte[] value)
			{
				if(hashAlg == null)
				{
					throw new ArgumentNullException("hashAlg");
				}
				if(value == null)
				{
					throw new ArgumentNullException("value");
				}
				this.hashAlg = hashAlg;
				this.value = value;
			}

	// Get or set this object's properties.
	public HashAlgorithm HashAlgorithm
			{
				get
				{
					return hashAlg;
				}
				set
				{
					if(value == null)
					{
						throw new ArgumentNullException("value");
					}
					hashAlg = value;
				}
			}
	public byte[] HashValue
			{
				get
				{
					return value;
				}
				set
				{
					if(value == null)
					{
						throw new ArgumentNullException("value");
					}
					this.value = value;
				}
			}

	// Implement the IMembership interface.
	[TODO]
	public bool Check(Evidence evidence)
			{
				// TODO
				return true;
			}
	public IMembershipCondition Copy()
			{
				return new HashMembershipCondition(hashAlg, value);
			}
	public override bool Equals(Object obj)
			{
				HashMembershipCondition other;
				other = (obj as HashMembershipCondition);
				if(other != null)
				{
					if(other.hashAlg.GetType() != hashAlg.GetType())
					{
						return false;
					}
					if(other.value.Length != value.Length)
					{
						return false;
					}
					int posn;
					for(posn = 0; posn < value.Length; ++posn)
					{
						if(other.value[posn] != value[posn])
						{
							return false;
						}
					}
					return true;
				}
				else
				{
					return false;
				}
			}
	[TODO]
	public override String ToString()
			{
				// TODO
				return null;
			}

	// Implement the ISecurityEncodable interface.
	public void FromXml(SecurityElement et)
			{
				FromXml(et, null);
			}
	public SecurityElement ToXml()
			{
				return ToXml(null);
			}

	// Implement the ISecurityPolicyEncodable interface.
	[TODO]
	public void FromXml(SecurityElement et, PolicyLevel level)
			{
				// TODO
			}
	[TODO]
	public SecurityElement ToXml(PolicyLevel level)
			{
				// TODO
				return null;
			}

	// Get the hash code for this instance.
	public override int GetHashCode()
			{
				int hash = 0;
				int posn;
				for(posn = 0; posn < value.Length; ++posn)
				{
					hash = (hash << 5) + hash + value[posn];
				}
				return hash;
			}

}; // class HashMembershipCondition

#endif // !ECMA_COMPAT

}; // namespace System.Security.Policy
