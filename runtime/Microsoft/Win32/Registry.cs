/*
 * Registry.cs - Implementation of the
 *			"Microsoft.Win32.Registry" class.
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

namespace Microsoft.Win32
{

#if CONFIG_WIN32_SPECIFICS

using System;

public sealed class Registry
{
	// Constructor.
	private Registry() {}

	// Standard registries on the local machine.
	public static readonly RegistryKey ClassesRoot;
	public static readonly RegistryKey CurrentUser;
	public static readonly RegistryKey LocalMachine;
	public static readonly RegistryKey Users;
	public static readonly RegistryKey PerformanceData;
	public static readonly RegistryKey CurrentConfig;
	public static readonly RegistryKey DynData;

	// Initialize the standard registries.
	static Registry()
			{
				ClassesRoot = RegistryKey.OpenRemoteBaseKey
					(RegistryHive.ClassesRoot, String.Empty);
				CurrentUser = RegistryKey.OpenRemoteBaseKey
					(RegistryHive.CurrentUser, String.Empty);
				LocalMachine = RegistryKey.OpenRemoteBaseKey
					(RegistryHive.LocalMachine, String.Empty);
				Users = RegistryKey.OpenRemoteBaseKey
					(RegistryHive.Users, String.Empty);
				PerformanceData = RegistryKey.OpenRemoteBaseKey
					(RegistryHive.PerformanceData, String.Empty);
				CurrentConfig = RegistryKey.OpenRemoteBaseKey
					(RegistryHive.CurrentConfig, String.Empty);
				DynData = RegistryKey.OpenRemoteBaseKey
					(RegistryHive.DynData, String.Empty);
			}

	// Registry providers that have already been allocated.
	private static IRegistryKeyProvider[] providers;

	// Get a registry key provider for a particular hive.
	internal static IRegistryKeyProvider GetProvider
				(RegistryHive hKey, String name)
			{
				int index;

				lock(typeof(Registry))
				{
					// Allocate the "providers" array if necessary.
					if(providers == null)
					{
						providers = new IRegistryKeyProvider[7];
					}

					// See if we already have a provider for this hive.
					index = ((int)hKey) - ((int)(RegistryHive.ClassesRoot));
					if(providers[index] != null)
					{
						return providers[index];
					}

					// Create a Win32 provider if we are on a Windows system.
					if(Win32KeyProvider.IsWin32())
					{
						providers[index] = new Win32KeyProvider
							(name, Win32KeyProvider.HiveToHKey(hKey));
						return providers[index];
					}

					// Try to create a file-based provider for the hive.
					try
					{
						providers[index] = new FileKeyProvider(hKey, name);
						return providers[index];
					}
					catch(NotSupportedException)
					{
						// Could not create the hive directory - fall through.
					}

					// Create a memory-based provider on all other systems.
					providers[index] = new MemoryKeyProvider
						(null, name, name);
					return providers[index];
				}
			}

}; // class Registry

#endif // CONFIG_WIN32_SPECIFICS

}; // namespace Microsoft.Win32
