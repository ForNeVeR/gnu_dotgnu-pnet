/*
 * PolicyLevel.cs - Implementation of the
 *		"System.Security.Policy.PolicyLevel" class.
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

[Serializable]
public sealed class PolicyLevel
{
	// Internal state.
	private ArrayList fullTrustAssemblies;
	private String label;
	private ArrayList namedPermissionSets;
	private CodeGroup rootCodeGroup;
	private String storeLocation;

	// Constructor.
	internal PolicyLevel(String label)
			{
				this.label = label;
				fullTrustAssemblies = new ArrayList();
				namedPermissionSets = new ArrayList();
			}

	// Properties.
	public IList FullTrustAssemblies
			{
				get
				{
					return fullTrustAssemblies;
				}
			}
	public String Label
			{
				get
				{
					return label;
				}
			}
	public IList NamedPermissionSets
			{
				get
				{
					return namedPermissionSets;
				}
			}
	public CodeGroup RootCodeGroup
			{
				get
				{
					return rootCodeGroup;
				}
				set
				{
					rootCodeGroup = value;
				}
			}
	public String StoreLocation
			{
				get
				{
					return storeLocation;
				}
			}

	// Add an entry to the "full trust assembly" list.
	public void AddFullTrustAssembly(StrongName sn)
			{
				if(sn == null)
				{
					throw new ArgumentNullException("sn");
				}
				AddFullTrustAssembly
					(new StrongNameMembershipCondition
						(sn.PublicKey, sn.Name, sn.Version));
			}
	[TODO]
	public void AddFullTrustAssembly(StrongNameMembershipCondition snMC)
			{
				// TODO
			}

#if CONFIG_PERMISSIONS

	// Add an entry to the "named permission sets" list.
	public void AddNamedPermissionSet(NamedPermissionSet permSet)
			{
				if(permSet == null)
				{
					throw new ArgumentNullException("permSet");
				}
				namedPermissionSets.Add(permSet);
			}

	// Change a named permission set.
	[TODO]
	public NamedPermissionSet ChangeNamedPermissionSet
				(String name, PermissionSet pSet)
			{
				if(name == null)
				{
					throw new ArgumentNullException("name");
				}
				if(pSet == null)
				{
					throw new ArgumentNullException("pSet");
				}
				// TODO
				return null;
			}

	// Get a specific named permission set.
	public NamedPermissionSet GetNamedPermissionSet(String name)
			{
				if(name == null)
				{
					throw new ArgumentNullException("name");
				}
				foreach(NamedPermissionSet set in namedPermissionSets)
				{
					if(set.Name == name)
					{
						return set;
					}
				}
				return null;
			}

	// Remove a named permission set.
	[TODO]
	public NamedPermissionSet RemoveNamedPermissionSet
					(NamedPermissionSet permSet)
			{
				if(permSet == null)
				{
					throw new ArgumentNullException("permSet");
				}
				// TODO
				return permSet;
			}
	[TODO]
	public NamedPermissionSet RemoveNamedPermissionSet(String name)
			{
				if(name == null)
				{
					throw new ArgumentNullException("name");
				}
				// TODO
				return null;
			}

#endif // CONFIG_PERMISSIONS

	// Create a policy level object for the current application domain.
	public static PolicyLevel CreateAppDomainLevel()
			{
				return new PolicyLevel("AppDomain");
			}

	// Load policy information from an XML element.
	[TODO]
	public void FromXml(SecurityElement e)
			{
				// TODO
			}

	// Recover the last backed-up policy configuration.
	public void Recover()
			{
				// Nothing to do here: we don't support backups.
			}

	// Remove an entry from the "full trust assembly" list.
	[TODO]
	public void RemoveFullTrustAssembly(StrongName sn)
			{
				// TODO
			}
	[TODO]
	public void RemoveFullTrustAssembly(StrongNameMembershipCondition snMC)
			{
				// TODO
			}

	// Reset to the default state.
	[TODO]
	public void Reset()
			{
				// TODO
			}

	// Resolve policy information based on supplied evidence.
	[TODO]
	public PolicyStatement Resolve(Evidence evidence)
			{
				if(evidence == null)
				{
					throw new ArgumentNullException("evidence");
				}
				// TODO
				return null;
			}
	[TODO]
	public CodeGroup ResolveMatchingCodeGroups(Evidence evidence)
			{
				if(evidence == null)
				{
					throw new ArgumentNullException("evidence");
				}
				// TODO
				return null;
			}

	// Convert this object into an XML element.
	[TODO]
	public SecurityElement ToXml()
			{
				// TODO
				return null;
			}

}; // class PolicyLevel

#endif // CONFIG_POLICY_OBJECTS

}; // namespace System.Security.Policy
