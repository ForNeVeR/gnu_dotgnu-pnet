/*
 * FirstMatchCodeGroup.cs - Implementation of the
 *		"System.Security.Policy.FirstMatchCodeGroup" class.
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

using System.Collections;
using System.Security.Permissions;

[Serializable]
public sealed class FirstMatchCodeGroup : CodeGroup
{
	// Constructor.
	public FirstMatchCodeGroup(IMembershipCondition membershipCondition,
					     	   PolicyStatement policy)
			: base(membershipCondition, policy)
			{
				// Nothing to do here.
			}

	// Properties.
	public override String MergeLogic
			{
				get
				{
					return "First Match";
				}
			}

	// Make a copy of this code group.
	public override CodeGroup Copy()
			{
				FirstMatchCodeGroup group;
				group = new FirstMatchCodeGroup
					(MembershipCondition, PolicyStatement);
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

}; // class FirstMatchCodeGroup

#endif // CONFIG_POLICY_OBJECTS

}; // namespace System.Security.Policy
