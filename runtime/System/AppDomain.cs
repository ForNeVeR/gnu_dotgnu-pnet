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

using System.Security;
using System.Reflection;
using System.Globalization;
using System.Reflection.Emit;
using System.Security.Policy;

public sealed class AppDomain : MarshalByRefObject, _AppDomain
{
	// Internal state.
	private String friendlyName;
	private static AppDomain currentDomain;

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

#if !ECMA_COMPAT

	// Get the current domain.
	[TODO]
	public static AppDomain CurrentDomain
			{
				get
				{
					lock(typeof(AppDomain))
					{
						// TODO - this is a temporary hack until the
						// runtime engine has real app domains.
						if(currentDomain == null)
						{
							currentDomain = new AppDomain("current");
						}
						return currentDomain;
					}
				}
			}

	// Get the security evidence that was used to load the app domain.
	[TODO]
	public Evidence Evidence
			{
				get
				{
					// TODO
					return null;
				}
			}

	// Event that is emitted to resolve assemblies.
	public event ResolveEventHandler AssemblyResolve;

	// Event that is emitted on process exit.
	public event EventHandler ProcessExit;

	// Event that is emitted to resolve resources.
	public event ResolveEventHandler ResourceResolve;

	// Event that is emitted to resolve types.
	public event ResolveEventHandler TypeResolve;

	// Get the assemblies in use by this domain.
	[TODO]
	public Assembly[] GetAssemblies()
			{
				// TODO
				return new Assembly [0];
			}

	// Create a new application domain.
	[TODO]
	public static AppDomain CreateDomain(String friendlyName,
										 Evidence evidence,
										 AppDomainSetup setup)
			{
				if(friendlyName == null)
				{
					throw new ArgumentNullException("friendlyName");
				}
				// TODO
				return new AppDomain(friendlyName);
			}

	// Create an instance of an assembly and unwrap its handle.
	[TODO]
	public Object CreateInstanceAndUnwrap(String assemblyName, String typeName)
			{
				// TODO
				return null;
			}
	public Object CreateInstanceAndUnwrap(String assemblyName,
						       			  String typeName,
						       			  Object[] activationAttributes)
			{
				// TODO
				return null;
			}
	public Object CreateInstanceAndUnwrap(String assemblyName,
						       			  String typeName,
						       			  bool ignoreCase,
						       			  BindingFlags bindingAttr,
						       			  Binder binder,
						       			  Object[] args,
						       			  CultureInfo culture,
						       			  Object[] activationAttributes,
						       			  Evidence securityAttributes)
			{
				// TODO
				return null;
			}

	[TODO]
	public AssemblyBuilder DefineDynamicAssembly (AssemblyName name,
						AssemblyBuilderAccess access)
			{
				return DefineDynamicAssembly (name, access, null, null,
											  null, null, null, false);
			}
	
	[TODO]
	public AssemblyBuilder DefineDynamicAssembly (AssemblyName name,
											AssemblyBuilderAccess access,
											Evidence evidence)
			{
				return DefineDynamicAssembly (name, access, null, evidence,
											  null, null, null, false);
			}
	
	[TODO]
	public AssemblyBuilder DefineDynamicAssembly (AssemblyName name,
											AssemblyBuilderAccess access,
											String dir)
			{
				return DefineDynamicAssembly (name, access, dir, null,
												null, null, null, false);
			}
	
	[TODO]
	public AssemblyBuilder DefineDynamicAssembly (AssemblyName name,
											AssemblyBuilderAccess access,
											String dir,
											Evidence evidence)
			{
				return DefineDynamicAssembly (name, access, dir, evidence,
												  null, null, null, false);
			}
	
	[TODO]
	public AssemblyBuilder DefineDynamicAssembly (AssemblyName name,
											 AssemblyBuilderAccess access,
											 PermissionSet requiredPermissions,
											 PermissionSet optionalPermissions,
											 PermissionSet refusedPersmissions)
			{
				return DefineDynamicAssembly (name, access, null, null,
									  requiredPermissions, optionalPermissions,
									  refusedPersmissions, false);
			}
	
	[TODO]
	public AssemblyBuilder DefineDynamicAssembly (AssemblyName name,
											AssemblyBuilderAccess access,
											Evidence evidence,
											PermissionSet requiredPermissions,
											PermissionSet optionalPermissions,
											PermissionSet refusedPersmissions)
			{
				return DefineDynamicAssembly (name, access, null, evidence,
									  requiredPermissions, optionalPermissions,
									  refusedPersmissions, false);
			}
	
	[TODO]
	public AssemblyBuilder DefineDynamicAssembly (AssemblyName name,
											AssemblyBuilderAccess access,
											String dir,
											PermissionSet requiredPermissions,
											PermissionSet optionalPermissions,
											PermissionSet refusedPersmissions)
		{
				return DefineDynamicAssembly (name, access, dir, null,
								requiredPermissions, optionalPermissions,
								refusedPersmissions, false);
		}
	
	[TODO]
	public AssemblyBuilder DefineDynamicAssembly (AssemblyName name,
											AssemblyBuilderAccess access,
											String dir,
											Evidence evidence,
											PermissionSet requiredPermissions,
											PermissionSet optionalPermissions,
											PermissionSet refusedPersmissions)
		{
			return DefineDynamicAssembly (name, access, dir, evidence,
								requiredPermissions, optionalPermissions,
								refusedPersmissions, false);

		}
	
	[TODO]
	public AssemblyBuilder DefineDynamicAssembly (AssemblyName name,
										AssemblyBuilderAccess access,
										string dir,
										Evidence evidence,
										PermissionSet requiredPermissions,
										PermissionSet optionalPermissions,
										PermissionSet refusedPersmissions,
										bool isSynchronized)
	{
		throw new NotImplementedException("DefineDynamicAssembly");
	}

	// Set the cache path.
	[TODO]
	public void SetCachePath(String s)
			{
				// TODO
			}

#endif // !ECMA_COMPAT

}; // class AppDomain

}; // namespace System
