/*
 * CodeAccessPermission.cs - Implementation of the
 *		"System.Security.CodeAccessPermission" class.
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
using System.Runtime.CompilerServices;

public abstract class CodeAccessPermission : IPermission
{

	// Constructor.
	protected CodeAccessPermission() {}

	// Assert permissions for the caller.
	public void Assert()
			{
				if(!ClrSecurity.Assert(this, 1))
				{
					throw new SecurityException
						(_("Exception_SecurityNotGranted"));
				}
			}

	// Deny permissions to the caller.
	public void Deny()
			{
				ClrSecurity.Deny(this, 1);
			}

	// Convert this object into a string.
	public override String ToString()
			{
				return ToXml().ToString();
			}

	// Convert an XML value into a permissions value.
	public abstract void FromXml(SecurityElement elem);

	// Convert this permissions object into an XML value.
	public abstract SecurityElement ToXml();

	// Implement the IPermission interface.
	public abstract IPermission Copy();
	public void Demand()
			{
				if(!ClrSecurity.Demand(this, 1))
				{
					throw new SecurityException
						(_("Exception_SecurityNotGranted"));
				}
			}
	public abstract IPermission Intersect(IPermission target);
	public abstract bool IsSubsetOf(IPermission target);
	public virtual IPermission Union(IPermission target)
			{
				// TODO
				return null;
			}

#if !ECMA_COMPAT

	// Revert all permissions for the caller.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static void RevertAll();

	// Revert all assertions for the caller.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static void RevertAssert();

	// Revert all denials for the caller.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static void RevertDeny();

	// Revert all "PermitOnly" permissions for the caller.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static void RevertPermitOnly();

	// Set the caller's permissions to only this object.
	public void PermitOnly()
			{
				ClrSecurity.SetPermitOnlyBlock(1);
				ClrSecurity.PermitOnly(this, 1);
			}

#endif // !ECMA_COMPAT

}; // class CodeAccessPermission

}; // namespace System.Security
