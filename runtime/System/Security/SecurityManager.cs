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

#if !ECMA_COMPAT && (CONFIG_PERMISSIONS || CONFIG_POLICY_OBJECTS)

using System.Collections;
using System.Security.Policy;

public sealed class SecurityManager
{
	// Cannot instantiate this class.
	private SecurityManager() {}

#if CONFIG_PERMISSIONS

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

#endif

#if CONFIG_POLICY_OBJECTS

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

	// Get an enumerator for the policy hierarchy.
	[TODO]
	public static IEnumerator PolicyHierarchy()
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

#if CONFIG_PERMISSIONS

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

#endif

	// Resolve policy group information.
	[TODO]
	public static IEnumerator ResolvePolicyGroups(Evidence evidence)
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

	// Get the zone and origin information
	[TODO]
	public static void GetZoneAndOrigin(out ArrayList zone,
										out ArrayList origin)
			{
				// TODO
				zone = null;
				origin = null;
			}

#endif // CONFIG_POLICY_OBJECTS

}; // class SecurityManager

#endif // !ECMA_COMPAT && (CONFIG_PERMISSIONS || CONFIG_POLICY_OBJECTS)


}; // namespace System.Security
