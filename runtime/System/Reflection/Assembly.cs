/*
 * Assembly.cs - Implementation of the "System.Reflection.Assembly" class.
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

namespace System.Reflection
{

using System;
using System.IO;
using System.Globalization;
using System.Security;
using System.Security.Policy;
using System.Runtime.CompilerServices;

public class Assembly : IClrProgramItem, ICustomAttributeProvider
{

	// Built-in handle for the assembly.  This must be the first field.
	internal IntPtr privateData;

	// Private constructor.  Normally called by the runtime engine only.
	internal Assembly() {}

	// Implement the IClrProgramItem interface.
	IntPtr IClrProgramItem.ClrHandle
			{
				get
				{
					return privateData;
				}
			}

	// Create an instance of a specific type within this assembly.
	public Object CreateInstance(String typeName)
			{
				return CreateInstance(typeName, false,
									  BindingFlags.Public |
									  BindingFlags.Instance,
									  null, null, null, null);
			}
#if !ECMA_COMPAT
	public Object CreateInstance(String typeName, bool ignoreCase)
			{
				return CreateInstance(typeName, ignoreCase,
									  BindingFlags.Public |
									  BindingFlags.Instance,
									  null, null, null, null);
			}
	public
#else  // ECMA_COMPAT
	private
#endif // ECMA_COMPAT
	Object CreateInstance(String typeName, bool ignoreCase,
						  BindingFlags bindingAttr, Binder binder,
						  Object[] args, CultureInfo culture,
						  Object[] activationAttributes)
			{
				Type type = GetType(typeName, false, ignoreCase);
				if(type == null)
				{
					return null;
				}
				return Activator.CreateInstance(type, bindingAttr,
												binder, args, culture,
												activationAttributes);
			}

#if !ECMA_COMPAT

	// Create a qualified type name.
	public static String CreateQualifiedName(String assemblyName,
											 String typeName)
			{
				return typeName + ", " + assemblyName;
			}

#endif // !ECMA_COMPAT

#if !ECMA_COMPAT

	// Get the custom attributes associated with this assembly.
	public Object[] GetCustomAttributes(bool inherit)
			{
				return ClrHelpers.GetCustomAttributes(this, inherit);
			}
	public Object[] GetCustomAttributes(Type type, bool inherit)
			{
				return ClrHelpers.GetCustomAttributes(this, type, inherit);
			}

	// Determine if custom attributes are associated with this assembly.
	public bool IsDefined(Type type, bool inherit)
			{
				return ClrHelpers.IsDefined(this, type, inherit);
			}

#else  // ECMA_COMPAT

	// Get the custom attributes associated with this assembly.
	Object[] ICustomAttributeProvider.GetCustomAttributes(bool inherit)
			{
				return ClrHelpers.GetCustomAttributes(this, inherit);
			}
	Object[] ICustomAttributeProvider.GetCustomAttributes
					(Type type, bool inherit)
			{
				return ClrHelpers.GetCustomAttributes(this, type, inherit);
			}

	// Determine if custom attributes are associated with this assembly.
	bool ICustomAttributeProvider.IsDefined(Type type, bool inherit)
			{
				return ClrHelpers.IsDefined(this, type, inherit);
			}

#endif // ECMA_COMPAT


	// The following three methods are not strictly speaking
	// ECMA-compatible, but they are useful in ECMA systems.

	// Get the assembly that called the method that called this method.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static Assembly GetCallingAssembly();

	// Get the assembly that called this method.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static Assembly GetExecutingAssembly();

	// Get the assembly that contained the program entry point.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static Assembly GetEntryAssembly();

#if !ECMA_COMPAT

	// Get an array of all exported types in an assembly.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public virtual Type[] GetExportedTypes();

	// Get a file stream for a particular public manifest file.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public virtual FileStream GetFile(String name);

	// Get file streams for all public manifest files.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public virtual FileStream[] GetFiles(bool getResourceModules);
	public virtual FileStream[] GetFiles()
			{
				return GetFiles(false);
			}

	// Get information about a particular manifest resource.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public virtual ManifestResourceInfo
			GetManifestResourceInfo(String resourceName);

	// Get the names of all manifest resources in this assembly.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public virtual String[] GetManifestResourceNames();

	// Get a stream for a particular manifest resource.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public virtual Stream GetManifestResourceStream(String name);

	// Get a stream for a particular manifest resource, scoped by a type.
	public virtual Stream GetManifestResourceStream(Type type, String name)
			{
				if(name == null)
				{
					return null;
				}
				else if(type == null)
				{
					return GetManifestResourceStream(type.Namespace + "." +
													 name);
				}
				else
				{
					return GetManifestResourceStream(name);
				}
			}

#else // ECMA_COMPAT

	// Get a file stream for a particular public manifest file.
	// Not strictly speaking ECMA-compatible, but needed for I18N.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public virtual FileStream GetFile(String name);

	// Get a stream for a particular manifest resource.
	// Not strictly speaking ECMA-compatible, but needed for I18N.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public virtual Stream GetManifestResourceStream(String name);

#endif // ECMA_COMPAT

	// Get a particular type from this assembly.
	public virtual Type GetType(String typeName)
			{
				return GetType(typeName, false, false);
			}
#if !ECMA_COMPAT
	public virtual Type GetType(String typeName, bool throwOnError)
			{
				return GetType(typeName, throwOnError, false);
			}
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public Type GetType(String typeName, bool throwOnError,
							   bool ignoreCase);
#else
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern internal Type GetType(String typeName, bool throwOnError,
							     bool ignoreCase);
#endif

	// Get an array of all types in an assembly.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public virtual Type[] GetTypes();

	// Error codes for "LoadFromName" and "LoadFromFile".
	private const int LoadError_OK			  = 0;
	private const int LoadError_InvalidName   = 1;
	private const int LoadError_FileNotFound  = 2;
	private const int LoadError_BadImage      = 3;
	private const int LoadError_Security      = 4;

	// Internal version of "Load".
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern private static Assembly LoadFromName(String name, out int error,
												Assembly parent);

	// Internal version of "LoadFrom".
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern internal static Assembly LoadFromFile(String name, out int error,
												 Assembly parent);

	// Throw an exception based on a load error.
	private static void ThrowLoadError(String name, int error)
			{
				if(error == LoadError_InvalidName)
				{
					throw new ArgumentException(_("Reflection_AssemblyName"));
				}
				else if(error == LoadError_FileNotFound)
				{
					throw new FileNotFoundException
						(String.Format(_("Reflection_AssemblyFile"), name));
				}
				else if(error == LoadError_BadImage)
				{
					throw new BadImageFormatException
						(String.Format(_("Reflection_BadImage"), name));
				}
				else
				{
					throw new SecurityException
						(String.Format(_("Reflection_AssemblySecurity"), name));
				}
			}

	// Load a particular assembly.
	public static Assembly Load(String assemblyString)
			{
				Assembly assembly;
				int error;
				if(assemblyString == null)
				{
					throw new ArgumentNullException("assemblyString");
				}
				if(assemblyString.Length >= 7 &&
				   String.Compare(assemblyString, 0, "file://", 0, 7, true)
				   		== 0)
				{
					assembly = LoadFromFile(assemblyString.Substring(7),
											out error,
										    GetCallingAssembly());
				}
				else
				{
					assembly = LoadFromName(assemblyString, out error,
											GetCallingAssembly());
				}
				if(error == LoadError_OK)
				{
					return assembly;
				}
				else
				{
					ThrowLoadError(assemblyString, error);
					return null;
				}
			}

	// Load a particular assembly from a file.
#if !ECMA_COMPAT
	public
#else
	internal
#endif
	static Assembly LoadFrom(String assemblyFile)
			{
				if(assemblyFile == null)
				{
					throw new ArgumentNullException("assemblyFile");
				}
				int error;
				Assembly assembly = LoadFromFile(assemblyFile, out error,
											     GetCallingAssembly());
				if(error == LoadError_OK)
				{
					return assembly;
				}
				else
				{
					ThrowLoadError(assemblyFile, error);
					return null;
				}
			}

#if !ECMA_COMPAT
	public static Assembly LoadWithPartialName (string partialName)
			{
				return LoadWithPartialName (partialName, null);
			}

	[TODO]
	public static Assembly LoadWithPartialName (string partialName, 
												Evidence securityEvidence)
			{
				return LoadFrom(partialName);
			}
#endif

	[TODO]
	public virtual AssemblyName GetName()
	{
		AssemblyName name=new AssemblyName();
		name.Name=ClrHelpers.GetName(privateData);
		name.Version=new Version(0,0,0,0);
		return name;
	}

	// Convert this assembly into a string.
	public override String ToString()
			{
				String name = FullName;
				if(name != null)
				{
					return name;
				}
				else
				{
					return base.ToString();
				}
			}

	// Get the full name associated with this assembly.
	public virtual String FullName
			{
				get
				{
					return ClrHelpers.GetName(privateData);
				}
			}

#if !ECMA_COMPAT

	// Get the code base associated with this assembly.
	[TODO]
	public virtual String CodeBase
			{
				get
				{
					// TODO
					return null;
				}
			}
	
	// Get the entry point for this assembly.
	public virtual MethodInfo EntryPoint
			{
				get
				{
					return (MethodInfo)(MethodBase.GetMethodFromHandle
								(GetEntryPoint()));
				}
			}

	// Get the security evidence for this assembly.
	[TODO]
	public virtual Evidence Evidence
			{
				get
				{
					// TODO
					return null;
				}
			}

	// Determine if this assembly was loaded from the global assembly cache.
	public bool GlobalAssemblyCache
			{
				get
				{
					// We don't use a GAC in this implementation, or if
					// we do then we leave it up to the engine to decide.
					return false;
				}
			}

	// Get the location where this assembly was loaded from.
	[TODO]
	public virtual String Location
			{
				get
				{
					// TODO
					return null;
				}
			}

	// Get the assembly that a particular type resides in.
	public static Assembly GetAssembly(Type type)
			{
				if(type == null)
				{
					throw new ArgumentNullException("type");
				}
				return type.Assembly;
			}

	// Get the entry point method for this assembly.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern private RuntimeMethodHandle GetEntryPoint();

#endif // !ECMA_COMPAT

	// Get the full pathname of a satellite file underneath
	// the directory containing this assembly.  Returns null
	// if it isn't possible to retrieve the path or the file
	// doesn't exist.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern internal String GetSatellitePath(String filename);

}; // class Assembly

}; // namespace System.Reflection
