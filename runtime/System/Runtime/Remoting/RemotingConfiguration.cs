/*
 * RemotingConfiguration.cs - Implementation of the
 *			"System.Runtime.Remoting.RemotingConfiguration" class.
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

namespace System.Runtime.Remoting
{

#if CONFIG_REMOTING

using System.Security.Permissions;

[SecurityPermission(SecurityAction.Demand,
					Flags=SecurityPermissionFlag.RemotingConfiguration)]
public class RemotingConfiguration
{
	// This class cannot be instantiated.
	private RemotingConfiguration() {}

	// Get the identifier for the currently running application
	[TODO]
	public static String ApplicationId
			{
				get
				{
					// TODO
					return null;
				}
			}

	// Get or set the application name.
	[TODO]
	public static String ApplicationName
			{
				get
				{
					// TODO
					return null;
				}
				set
				{
					// TODO
				}
			}

	// Get the process ID for the current process.
	[TODO]
	public static String ProcessId
			{
				get
				{
					// TODO
					return null;
				}
			}

	// Read remoting information from a configuration file.
	[TODO]
	public static void Configure(String filename)
			{
				// TODO
			}

	// Get a list of client types that can be activated remotely.
	[TODO]
	public static ActivatedClientTypeEntry[]
				GetRegisteredActivatedClientTypes()
			{
				// TODO
				return null;
			}

	// Get a list of service types that can be activated by remote clients.
	[TODO]
	public static ActivatedServiceTypeEntry[]
				GetRegisteredActivatedServiceTypes()
			{
				// TODO
				return null;
			}

	// Get a list of well known client types.
	[TODO]
	public static WellKnownClientTypeEntry[]
				GetRegisteredWellKnownClientTypes()
			{
				// TODO
				return null;
			}

	// Get a list of well known service types.
	[TODO]
	public static WellKnownServiceTypeEntry[]
				GetRegisteredWellKnownServiceTypes()
			{
				// TODO
				return null;
			}

	// Determine if activation is allowed on a type.
	[TODO]
	public static bool IsActivationAllowed(Type svrType)
			{
				// TODO
				return false;
			}

	// Determine if a type can be remotely activated.
	[TODO]
	public static ActivatedClientTypeEntry
				IsRemotelyActivatedClientType(Type svrType)
			{
				// TODO
				return null;
			}
	[TODO]
	public static ActivatedClientTypeEntry
				IsRemotelyActivatedClientType
					(String typeName, String assemblyName)
			{
				// TODO
				return null;
			}

	// Determine if a type is a well known client type.
	[TODO]
	public static WellKnownClientTypeEntry
				IsWellKnownClientType(Type svrType)
			{
				// TODO
				return null;
			}
	[TODO]
	public static WellKnownClientTypeEntry
				IsWellKnownClientType
					(String typeName, String assemblyName)
			{
				// TODO
				return null;
			}

	// Register a client type that can be activated remotely.
	[TODO]
	public static void RegisterActivatedClientType
				(ActivatedClientTypeEntry entry)
			{
				// TODO
			}
	public static void RegisterActivatedClientType(Type type, String appUrl)
			{
				RegisterActivatedClientType
					(new ActivatedClientTypeEntry(type, appUrl));
			}

	// Register a service type that can be activated by a remote client.
	[TODO]
	public static void RegisterActivatedServiceType
				(ActivatedServiceTypeEntry entry)
			{
				// TODO
			}
	public static void RegisterActivatedServiceType(Type type)
			{
				RegisterActivatedServiceType
					(new ActivatedServiceTypeEntry(type));
			}

	// Register a well known client type.
	[TODO]
	public static void RegisterWellKnownClientType
				(WellKnownClientTypeEntry entry)
			{
				// TODO
			}
	public static void RegisterWellKnownClientType
				(Type type, String objectUrl)
			{
				RegisterWellKnownClientType
					(new WellKnownClientTypeEntry(type, objectUrl));
			}

	// Register a well known service type entry.
	[TODO]
	public static void RegisterWellKnownServiceType
				(WellKnownServiceTypeEntry entry)
			{
				// TODO
			}
	public static void RegisterWellKnownServiceType
				(Type type, String objectUri, WellKnownObjectMode mode)
			{
				RegisterWellKnownServiceType
					(new WellKnownServiceTypeEntry(type, objectUri, mode));
			}

}; // class RemotingConfiguration

#endif // CONFIG_REMOTING

}; // namespace System.Runtime.Remoting
