/*
 * ZoneMembershipCondition.cs - Implementation of the
 *		"System.Security.Policy.ZoneMembershipCondition" class.
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

#if CONFIG_POLICY_OBJECTS

using System.Security.Permissions;

[Serializable]
public sealed class ZoneMembershipCondition
	: IMembershipCondition, ISecurityEncodable, ISecurityPolicyEncodable
{
	// Internal state.
	private SecurityZone zone;

	// Constructor.
	public ZoneMembershipCondition(SecurityZone zone)
			{
				this.zone = zone;
			}

	// Get or set this object's properties.
	public SecurityZone SecurityZone
			{
				get
				{
					return zone;
				}
				set
				{
					zone = value;
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
				return new ZoneMembershipCondition(zone);
			}
	public override bool Equals(Object obj)
			{
				ZoneMembershipCondition other;
				other = (obj as ZoneMembershipCondition);
				if(other != null)
				{
					return (other.zone == zone);
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
				return ((int)zone);
			}

}; // class ZoneMembershipCondition

#endif // CONFIG_POLICY_OBJECTS

}; // namespace System.Security.Policy
