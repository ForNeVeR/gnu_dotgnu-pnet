/*
 * SecurityManager.cs - Implementation of the
 *		"System.Security.SecurityManager" class.
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

namespace System.Security
{

#if !ECMA_COMPAT

using System.Collections;
using System.Security.Policy;

public class SecurityManager
{

	// Determine if a specific permission has been granted.
	[TODO]
	public static bool IsGranted(IPermission perm)
			{
				if(perm == null)
				{
					return true;
				}
				// TODO
				return false;
			}

	// Load policy level information from a file.
	[TODO]
	public static PolicyLevel LoadPolicyLevelFromFile
				(String path, PolicyLevelType type)
			{
				// TODO
				return null;
			}

	// Load policy level information from a string.
	[TODO]
	public static PolicyLevel LoadPolicyLevelFromString
				(String str, PolicyLevelType type)
			{
				// TODO
				return null;
			}

	// Save a particular policy level.
	[TODO]
	public static void SavePolicyLevel(PolicyLevel level)
			{
				// TODO
			}

	// Resolve policy information.
	[TODO]
	public static PermissionSet ResolvePolicy
				(Evidence evidence, PermissionSet reqdPset,
				 PermissionSet optPset, PermissionSet denyPset,
				 out PermissionSet denied)
			{
				// TODO
				denied = null;
				return null;
			}
	[TODO]
	public static PermissionSet ResolvePolicy(Evidence evidence)
			{
				// TODO
				return null;
			}

	// Resolve policy group information.
	[TODO]
	public static IEnumerator ResolvePolicyGroups(Evidence evidence)
			{
				// TODO
				return null;
			}

	// Get an enumerator for the policy hierarchy.
	[TODO]
	public static IEnumerator PolicyHeirarchy()
			{
				// TODO
				return null;
			}

	// Save policy information.
	[TODO]
	public static void SavePolicy()
			{
				// TODO
			}

	// Get or set the execution rights flag.
	[TODO]
	public static bool CheckExecutionRights
			{
				get
				{
					// TODO
					return true;
				}
				set
				{
					// TODO
				}
			}

	// Determine if security features have been enabled.
	[TODO]
	public static bool SecurityEnabled
			{
				get
				{
					// TODO
					return true;
				}
				set
				{
					// TODO
				}
			}

	// Print information about the granted permissions for the
	// current application domain's assemblies.
	[TODO]
	public static void PrintGrantInfo()
			{
				// TODO
			}

}; // class SecurityManager

#endif // !ECMA_COMPAT

}; // namespace System.Security
