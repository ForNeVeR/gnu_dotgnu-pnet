/*
 * ApplicationDirectoryMembershipCondition.cs - Implementation of the
 *		"System.Security.Policy.ApplicationDirectoryMembershipCondition" class.
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

[Serializable]
public sealed class ApplicationDirectoryMembershipCondition
	: IMembershipCondition, ISecurityEncodable, ISecurityPolicyEncodable
{
	// Constructor.
	public ApplicationDirectoryMembershipCondition() {}

	// Implement the IMembership interface.
	[TODO]
	public bool Check(Evidence evidence)
			{
				// TODO
				return true;
			}
	public IMembershipCondition Copy()
			{
				return new ApplicationDirectoryMembershipCondition();
			}
	public override bool Equals(Object obj)
			{
				return (obj is ApplicationDirectoryMembershipCondition);
			}
	public override String ToString()
			{
				return _("Security_AppDirMembershipCondition");
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
	public void FromXml(SecurityElement et, PolicyLevel level)
			{
				if(et == null)
				{
					throw new ArgumentNullException("et");
				}
				if(et.Tag != "IMembershipCondition")
				{
					throw new ArgumentException
						(_("Security_PolicyName"));
				}
				if(et.Attribute("version") != "1")
				{
					throw new ArgumentException
						(_("Security_PolicyVersion"));
				}
			}
	public SecurityElement ToXml(PolicyLevel level)
			{
				SecurityElement element;
				element = new SecurityElement("IMembershipCondition");
				element.AddAttribute
					("class",
					 SecurityElement.Escape
					 	(typeof(ApplicationDirectoryMembershipCondition).
		 						AssemblyQualifiedName));
				element.AddAttribute("version", "1");
				return element;
			}

	// Get the hash code for this instance.
	public override int GetHashCode()
			{
				// All instances of this type are identical.
				return GetType().GetHashCode();
			}

}; // class ApplicationDirectoryMembershipCondition

#endif // !ECMA_COMPAT

}; // namespace System.Security.Policy
