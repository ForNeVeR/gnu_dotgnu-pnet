/*
 * FileCodeGroup.cs - Implementation of the
 *		"System.Security.Policy.FileCodeGroup" class.
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

using System.Collections;
using System.Security.Permissions;

[Serializable]
public sealed class FileCodeGroup : CodeGroup
{
	// Internal state.
	private FileIOPermissionAccess access;

	// Constructor.
	public FileCodeGroup(IMembershipCondition membershipCondition,
					     FileIOPermissionAccess access)
			: base(membershipCondition, null)
			{
				this.access = access;
			}

	// Properties.
	public override String AttributeString
			{
				get
				{
					return null;
				}
			}
	public override String MergeLogic
			{
				get
				{
					return "Union";
				}
			}
	public override String PermissionSetName
			{
				get
				{
					return String.Format(_("Format_FileIOPermSetName"),
										 access.ToString());
				}
			}

	// Make a copy of this code group.
	public override CodeGroup Copy()
			{
				FileCodeGroup group;
				group = new FileCodeGroup(MembershipCondition, access);
				group.Name = Name;
				group.Description = Description;
				IList children = Children;
				if(children != null)
				{
					foreach(CodeGroup child in children)
					{
						group.AddChild(child);
					}
				}
				return group;
			}

	// Create the XML form of this code group.
	[TODO]
	public override void CreateXml(SecurityElement element, PolicyLevel level)
			{
				// TODO
			}

	// Compare two code groups for equality.
	public override bool Equals(Object obj)
			{
				FileCodeGroup cg = (obj as FileCodeGroup);
				if(cg != null)
				{
					if(!base.Equals(cg))
					{
						return false;
					}
					return (cg.access == access);
				}
				else
				{
					return false;
				}
			}

	// Get the hash code for this instance.
	public override int GetHashCode()
			{
				return base.GetHashCode();
			}

	// Resolve the policy for this code group.
	[TODO]
	public override PolicyStatement Resolve(Evidence evidence)
			{
				if(evidence == null)
				{
					throw new ArgumentNullException("evidence");
				}
				// TODO
				return PolicyStatement;
			}

	// Resolve code groups that match specific evidence.
	[TODO]
	public override CodeGroup ResolveMatchingCodeGroups(Evidence evidence)
			{
				// TODO
				return null;
			}

	// Parse the XML form of this code group.
	[TODO]
	public override void ParseXml(SecurityElement element, PolicyLevel level)
			{
				// TODO
			}

}; // class FileCodeGroup

#endif // !ECMA_COMPAT

}; // namespace System.Security.Policy
