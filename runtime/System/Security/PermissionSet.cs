/*
 * PermissionSet.cs - Implementation of the
 *		"System.Security.PermissionSet" class.
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

namespace System.Security
{

using System;
using System.Collections;
using System.Security.Permissions;

public class PermissionSet : ICollection, IEnumerable
{

	// Internal state.
	private PermissionState state;
	private ArrayList permissions;

	// Constructors.
	public PermissionSet(PermissionState state)
			{
				if(state != PermissionState.Unrestricted &&
				   state != PermissionState.None)
				{
					throw new ArgumentException(_("Arg_PermissionState"));
				}
				this.state = state;
				permissions = new ArrayList();
			}
	public PermissionSet(PermissionSet permSet)
			{
				if(permSet != null)
				{
					state = permSet.state;
					permissions = new ArrayList(permSet.permissions.Count);
					int posn;
					for(posn = 0; posn < permSet.permissions.Count; ++posn)
					{
						permissions[posn] =
							((IPermission)(permSet.permissions[posn])).Copy();
					}
				}
				else
				{
					state = PermissionState.Unrestricted;
					permissions = new ArrayList();
				}
			}

	// Find the index of a permission by type.
	private int FindPermission(Type type)
			{
				int posn;
				for(posn = 0; posn < permissions.Count; ++posn)
				{
					if(permissions[posn].GetType() == type)
					{
						return posn;
					}
				}
				return -1;
			}

	// Add a permission value to this set.
	public virtual IPermission AddPermission(IPermission perm)
			{
				if(perm == null)
				{
					throw new ArgumentNullException("perm");
				}
				int index = FindPermission(perm.GetType());
				if(index == -1)
				{
					permissions.Add(perm);
					return perm;
				}
				else
				{
					return (IPermission)(permissions[index]);
				}
			}

	// Assert the permissions in this set.
	public virtual void Assert()
			{
				int posn;
				CodeAccessPermission perm;
				for(posn = 0; posn < permissions.Count; ++posn)
				{
					perm = (permissions[posn] as CodeAccessPermission);
					if(perm != null)
					{
						perm.Assert();
					}
				}
			}

	// Return a copy of this permission set.
	public virtual PermissionSet Copy()
			{
				return new PermissionSet(this);
			}

	// Demand the permissions in this set.
	public virtual void Demand()
			{
				int posn;
				IPermission perm;
				CodeAccessPermission caperm;
				for(posn = 0; posn < permissions.Count; ++posn)
				{
					perm = ((IPermission)(permissions[posn]));
					caperm = (perm as CodeAccessPermission);
					if(caperm != null)
					{
						if(!ClrSecurity.Demand(caperm, 1))
						{
							throw new SecurityException
								(_("Exception_SecurityNotGranted"));
						}
					}
					else
					{
						perm.Demand();
					}
				}
			}

	// Deny the permissions in this set.
	public virtual void Deny()
			{
				int posn;
				CodeAccessPermission perm;
				for(posn = 0; posn < permissions.Count; ++posn)
				{
					perm = (permissions[posn] as CodeAccessPermission);
					if(perm != null)
					{
						ClrSecurity.Deny(perm, 1);
					}
				}
			}

	// Convert an XML security element into a permission set.
	[TODO]
	public virtual void FromXml(SecurityElement et)
			{
				// TODO
			}

	// Determine if this permission set is a subset of another.
	[TODO]
	public virtual bool IsSubsetOf(PermissionSet target)
			{
				// TODO
				return false;
			}

	// Permit only the permissions in this set.
	public virtual void PermitOnly()
			{
				int posn;
				CodeAccessPermission perm;
				ClrSecurity.SetPermitOnlyBlock(1);
				for(posn = 0; posn < permissions.Count; ++posn)
				{
					perm = (permissions[posn] as CodeAccessPermission);
					if(perm != null)
					{
						ClrSecurity.PermitOnly(perm, 1);
					}
				}
			}

	// Convert this object into a string.
	public override String ToString()
			{
				return ToXml().ToString();
			}

	// Convert this permission set into an XML security element.
	[TODO]
	public virtual SecurityElement ToXml()
			{
				// TODO
				return null;
			}

	// Form the union of this security set and another.
	[TODO]
	public virtual PermissionSet Union(PermissionSet other)
			{
				// TODO
				return null;
			}

	// Implement the ICollection interface.
	public virtual void CopyTo(Array array, int index)
			{
				permissions.CopyTo(array, index);
			}
	public virtual int Count
			{
				get
				{
					return permissions.Count;
				}
			}
	public virtual bool IsSynchronized
			{
				get
				{
					return false;
				}
			}
	public virtual Object SyncRoot
			{
				get
				{
					return this;
				}
			}

	// Implement the IEnumerable interface.
	public virtual IEnumerator GetEnumerator()
			{
				return permissions.GetEnumerator();
			}

#if !ECMA_COMPAT

	// Determine if this permission set is read-only.
	public virtual bool IsReadOnly
			{
				get
				{
					return false;
				}
			}

	// Determine if the set contains permissions that do not
	// derive from CodeAccessPermission.
	public bool ContainsNonCodeAccessPermisssions()
			{
				int posn;
				Type type = typeof(CodeAccessPermission);
				for(posn = 0; posn < permissions.Count; ++posn)
				{
					if(!type.IsAssignableFrom(permissions[posn].GetType()))
					{
						return true;
					}
				}
				return false;
			}

	// Get a permission object of a specific type from this set.
	public virtual IPermission GetPermission(Type permClass)
			{
				if(permClass != null)
				{
					int index = FindPermission(permClass);
					if(index != -1)
					{
						return (IPermission)(permissions[index]);
					}
				}
				return null;
			}

	// Form the intersection of this permission set and another.
	[TODO]
	public virtual PermissionSet Intersect(PermissionSet other)
			{
				// TODO
				return null;
			}

	// Determine if this permission set is empty.
	public virtual bool IsEmpty()
			{
				return (permissions.Count == 0);
			}

	// Determine if this permission set is unrestricted.
	public virtual bool IsUnrestricted()
			{
				return (state == PermissionState.Unrestricted);
			}

	// Remove a particular permission from this set.
	public virtual IPermission RemovePermission(Type permClass)
			{
				if(permClass == null)
				{
					int index = FindPermission(permClass);
					if(index != -1)
					{
						IPermission perm = (IPermission)(permissions[index]);
						permissions.RemoveAt(index);
						return perm;
					}
				}
				return null;
			}

	// Set a permission into this permission set.
	public virtual IPermission SetPermission(IPermission perm)
			{
				if(perm != null)
				{
					int index = FindPermission(perm.GetType());
					if(index != -1)
					{
						permissions[index] = perm;
					}
					else
					{
						permissions.Add(perm);
					}
					return perm;
				}
				else
				{
					return null;
				}
			}

#endif // !ECMA_COMPAT

}; // class PermissionSet

}; // namespace System.Security
