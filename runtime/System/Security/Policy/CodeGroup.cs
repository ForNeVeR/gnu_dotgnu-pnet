/*
 * CodeGroup.cs - Implementation of the
 *		"System.Security.Policy.CodeGroup" class.
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

[Serializable]
public abstract class CodeGroup
{
	// Internal state.
	private IMembershipCondition membershipCondition;
	private PolicyStatement policy;
	private IList children;
	private String description;
	private String name;

	// Constructor.
	public CodeGroup(IMembershipCondition membershipCondition,
					 PolicyStatement policy)
			{
				if(membershipCondition == null)
				{
					throw new ArgumentNullException("membershipCondition");
				}
				this.membershipCondition = membershipCondition;
				this.policy = policy;
				this.children = new ArrayList();
			}

	// Properties.
	public virtual String AttributeString
			{
				get
				{
					// TODO
					return null;
				}
			}
	public IList Children
			{
				get
				{
					return children;
				}
				set
				{
					if(value == null)
					{
						throw new ArgumentNullException("value");
					}
					children = value;
				}
			}
	public String Description
			{
				get
				{
					return description;
				}
				set
				{
					description = value;
				}
			}
	public IMembershipCondition MembershipCondition
			{
				get
				{
					return membershipCondition;
				}
				set
				{
					if(value == null)
					{
						throw new ArgumentNullException("value");
					}
					membershipCondition = value;
				}
			}
	public abstract String MergeLogic { get; }
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
	public virtual String PermissionSetName
			{
				get
				{
					// TODO
					return null;
				}
			}
	public PolicyStatement PolicyStatement
			{
				get
				{
					return policy;
				}
				set
				{
					policy = value;
				}
			}

	// Add a child to this code group.
	public void AddChild(CodeGroup group)
			{
				if(group == null)
				{
					throw new ArgumentNullException("group");
				}
				Children.Add(group);
			}

	// Make a copy of this code group.
	public abstract CodeGroup Copy();

	// Create the XML form of this code group.
	protected virtual void CreateXml
				(SecurityElement element, PolicyLevel level)
			{
				// Nothing to do in the base class.
			}

	// Compare two code groups for equality.
	public override bool Equals(Object obj)
			{
				CodeGroup cg = (obj as CodeGroup);
				if(cg != null)
				{
					return Equals(cg, false);
				}
				else
				{
					return false;
				}
			}
	public bool Equals(CodeGroup cg, bool compareChildren)
			{
				if(cg == null)
				{
					return false;
				}
				if(Name != cg.Name || Description != cg.Description ||
				   !MembershipCondition.Equals(cg.MembershipCondition))
				{
					return false;
				}
				if(compareChildren)
				{
					IList list1 = Children;
					IList list2 = cg.Children;
					if(list1.Count != list2.Count)
					{
						return false;
					}
					int posn;
					for(posn = 0; posn < list1.Count; ++posn)
					{
						if(!((CodeGroup)(list1[posn])).Equals
								(((CodeGroup)(list2[posn])), true))
						{
							return false;
						}
					}
				}
				return true;
			}

	// Convert an XML security element into a code group.
	public void FromXml(SecurityElement et)
			{
				FromXml(et, null);
			}
	public void FromXml(SecurityElement et, PolicyLevel level)
			{
				if(et == null)
				{
					throw new ArgumentNullException("et");
				}
				if(et.Tag != "CodeGroup")
				{
					throw new ArgumentException
						(_("Security_CodeGroupName"));
				}
				if(et.Attribute("version") != "1")
				{
					throw new ArgumentException
						(_("Security_PolicyVersion"));
				}
				// TODO
				ParseXml(et, level);
			}

	// Get the hash code for this instance.
	public override int GetHashCode()
			{
				return membershipCondition.GetHashCode();
			}

	// Remove a child from this code group.
	public void RemoveChild(CodeGroup group)
			{
				if(group == null)
				{
					throw new ArgumentNullException("group");
				}
				if(!(Children.Contains(group)))
				{
					throw new ArgumentException
						(_("Security_NotCodeGroupChild"));
				}
			}

	// Resolve the policy for this code group.
	public abstract PolicyStatement Resolve(Evidence evidence);

	// Resolve code groups that match specific evidence.
	public abstract CodeGroup ResolveMatchingCodeGroups(Evidence evidence);

	// Convert a code group into an XML security element
	public SecurityElement ToXml()
			{
				return ToXml(null);
			}
	public SecurityElement ToXml(PolicyLevel level)
			{
				SecurityElement element;
				element = new SecurityElement("CodeGroup");
				element.AddAttribute
					("class",
					 SecurityElement.Escape(typeof(CodeGroup).
					 						AssemblyQualifiedName));
				element.AddAttribute("version", "1");
				// TODO
				CreateXml(element, level);
				return element;
			}

	// Parse the XML form of this code group.
	protected virtual void ParseXml
				(SecurityElement element, PolicyLevel level)
			{
				// Nothing to do in the base class.
			}

}; // class CodeGroup

#endif // !ECMA_COMPAT

}; // namespace System.Security.Policy
