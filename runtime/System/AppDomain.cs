/*
 * AppDomain.cs - Implementation of the "System.AppDomain" class.
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

namespace System
{

public sealed class AppDomain : MarshalByRefObject, _AppDomain
{
	// Internal state.
	private String friendlyName;

	// Construct a new AppDomain instance.
	[TODO]
	private AppDomain(String name)
	{
		friendlyName = name;

		// TODO: create the domain within the runtime engine.
	}

	// Create a new application domain with a specified name.
	public static AppDomain CreateDomain(string friendlyName)
	{
		if(friendlyName == null)
		{
			throw new ArgumentNullException("friendlyName");
		}
		return new AppDomain(friendlyName);
	}

#if !ECMA_COMPAT
	// Load and execute a file containing an assembly.
	[TODO]
	public int ExecuteAssembly(String assemblyFile)
	{
		if(assemblyFile == null)
		{
			throw new ArgumentNullException("assemblyFile");
		}
		// TODO: load and execute the assembly.
		return 0;
	}
#endif

	// Return a string representing the current instance.
	public override String ToString()
	{
		return friendlyName;
	}

	// Unload a specific application domain.
	[TODO]
	public static void Unload(AppDomain domain)
	{
		if(domain == null)
		{
			throw new ArgumentNullException("domain");
		}
		// TODO: unload the domain.
	}

	// Get the friendly name associated with this application domain.
	public String FriendlyName
	{
		get
		{
			return friendlyName;
		}
	}

	// Event that is emitted when an assembly is loaded into this domain.
	public event AssemblyLoadEventHandler AssemblyLoad;

	// Event that is emitted when an application domain is unloaded.
	public event EventHandler DomainUnload;

	// Event that is emitted when an exception is unhandled by the domain.
	public event UnhandledExceptionEventHandler UnhandledException;

}; // class AppDomain

}; // namespace System
