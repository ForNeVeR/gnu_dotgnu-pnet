/*
 * AssemblyBuilder.cs - Implementation of the
 *		"System.Reflection.Emit.AssemblyBuilder" class.
 *
 * Copyright (C) 2001, 2002  Southern Storm Software, Pty Ltd.
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

namespace System.Reflection.Emit
{

#if !ECMA_COMPAT

using System;
using System.IO;
using System.Collections;
using System.Reflection;
using System.Security;
using System.Security.Policy;
using System.Security.Permissions;
using System.Runtime.CompilerServices;
using System.Threading;

public sealed class AssemblyBuilder : Assembly
{
	// Internal state.
	private AssemblyBuilderAccess access;
	private String directory;
	private bool isSynchronized;
	private bool saved;
	private MethodInfo entryPoint;
	private IntPtr writer;
	private ModuleBuilder module;
	private ArrayList detachList;

	// Constructor.  Called from AppDomain.DefineDynamicAssembly.
	internal AssemblyBuilder(AssemblyName name, AssemblyBuilderAccess access,
							 String directory, bool isSynchronized)
			{
				this.access = access;
				this.directory = directory;
				this.isSynchronized = isSynchronized;
				this.saved = false;
				this.entryPoint = null;
				this.detachList = new ArrayList();
				Version version = name.Version;
				lock(typeof(AssemblyBuilder))
				{
					if(version != null)
					{
						this.privateData = ClrAssemblyCreate
							(name.Name, version.Major, version.Minor,
							 version.Build, version.Revision,
							 access, out writer);
					}
					else
					{
						this.privateData = ClrAssemblyCreate
							(name.Name, 0, 0, 0, 0, access, out writer);
					}
				}
				if(this.privateData == IntPtr.Zero)
				{
					// The runtime engine disallowed dynamic assemblies.
					throw new SecurityException
						(_("Emit_NoDynamicAssemblies"));
				}
			}

	// Start a synchronized operation on this assembly builder.
	internal void StartSync()
			{
				if(isSynchronized)
				{
					Monitor.Enter(this);
				}
			}

	// End a synchronized operation on this assembly builder.
	internal void EndSync()
			{
				if(isSynchronized)
				{
					Monitor.Exit(this);
				}
			}

	// Get the code base for this assembly builder.
	public override String CodeBase
			{
				get
				{
					throw new NotSupportedException(_("NotSupp_Builder"));
				}
			}

	// Get the location of the assembly manifest file.
	public override String Location
			{
				get
				{
					throw new NotSupportedException(_("NotSupp_Builder"));
				}
			}

	// Get the entry point for this assembly builder.
	public override MethodInfo EntryPoint
			{
				get
				{
					return entryPoint;
				}
			}
	
	[TODO]
	public void AddResourceFile(String name, String fileName)
			{
		 		throw new NotImplementedException("AddResourceFile");
			}

	[TODO]
	public void AddResourceFile(String name, String fileName, ResourceAttributes attribute)
			{
		 		throw new NotImplementedException("AddResourceFile");
			}

	// Define a dynamic module that is attached to this assembly.
	public ModuleBuilder DefineDynamicModule(String name)
			{
				return DefineDynamicModule(name, false);
			}
	public ModuleBuilder DefineDynamicModule(String name, bool emitSymbolInfo)
			{
				try
				{
					StartSync();
					if(saved)
					{
						throw new InvalidOperationException
							(_("Emit_AlreadySaved"));
					}
					if(name == null)
					{
						throw new ArgumentNullException("name");
					}
					if(name == String.Empty)
					{
						throw new ArgumentException
							(_("Emit_ModuleNameInvalid"));
					}
					if(Char.IsWhiteSpace(name[0]) ||
				   	   name.IndexOf('/') != -1 ||
				   	   name.IndexOf('\\') != -1)
					{
						throw new ArgumentException
							(_("Emit_ModuleNameInvalid"));
					}
					if(module != null)
					{
						// We don't support assemblies with multiple modules.
		 				throw new NotSupportedException
							(_("Emit_SingleModuleOnly"));
					}
					module = new ModuleBuilder
						(this, name, true, emitSymbolInfo);
					return module;
				}
				finally
				{
					EndSync();
				}
			}
	public ModuleBuilder DefineDynamicModule(String name, String fileName)
			{
		 		return DefineDynamicModule(name, fileName, false);
			}
	public ModuleBuilder DefineDynamicModule(String name, String fileName, 
											 bool emitSymbolInfo)
			{
				// We don't support modules in external files.
		 		throw new NotSupportedException(_("Emit_ExternalModule"));
			}

/*
	[TODO]
	public IResourceWriter DefineResource(String name, String description, 
										  String fileName)
			{
		 		throw new NotImplementedException("DefineResource");
			}

	[TODO]
	public IResourceWriter DefineResource(String name, String description, 
										  String fileName, 
										  ResourceAttributes attribute)
			{
		 		throw new NotImplementedException("DefineResource");
			}
*/
	[TODO]
	public void DefineUnmanagedResource(byte[] resource)
			{
		 		throw new NotImplementedException("DefineUnmanagedResource");
			}

	[TODO]
	public void DefineUnmanagedResource(String resourceFileName)
			{
		 		throw new NotImplementedException("DefineUnmanagedResource");
			}

	[TODO]
	public void DefineVersionInfoResource()
			{
		 		throw new NotImplementedException("DefineVersionInfoResource");
			}

	[TODO]
	public void DefineVersionInfoResource(String product, String productVersion
										 ,String company, String copyright, 
										  String trademark)
			{
		 		throw new NotImplementedException("DefineVersionInfoResource");
			}

	[TODO]
	public void EmbedResourceFile(String name, String fileName)
			{
		 		throw new NotImplementedException("EmbedResourceFile");
			}

	[TODO]
	public void EmbedResourceFile(String name, String fileName, 
								  ResourceAttributes attribute)
			{
		 		throw new NotImplementedException("EmbedResourceFile");
			}

	// Get a particular module within this assembly.
	public ModuleBuilder GetDynamicModule(String name)
			{
				if(name == null)
				{
					throw new ArgumentNullException("name");
				}
				else if(name == String.Empty)
				{
					throw new ArgumentException
						(_("Emit_ModuleNameInvalid"));
				}
				if(module != null && module.Name == name)
				{
					return module;
				}
				else
				{
					return null;
				}
			}

	[TODO]
	public override Type[] GetExportedTypes()
			{
		 		throw new NotImplementedException("GetExportedTypes");
			}

	[TODO]
	public override FileStream GetFile(String name)
			{
		 		throw new NotImplementedException("GetFile");
			}

	[TODO]
	public void Save(String assemblyFileName)
			{
		 		throw new NotImplementedException("Save");
			}

	// Set a custom attribute on this assembly.
	public void SetCustomAttribute(CustomAttributeBuilder customBuilder)
			{
				try
				{
					StartSync();
					if(saved)
					{
						throw new InvalidOperationException
							(_("Emit_AlreadySaved"));
					}
					SetCustomAttribute(this, customBuilder);
				}
				finally
				{
					EndSync();
				}
			}
	public void SetCustomAttribute(ConstructorInfo con, byte[] binaryAttribute)
			{
				try
				{
					StartSync();
					if(saved)
					{
						throw new InvalidOperationException
							(_("Emit_AlreadySaved"));
					}
					SetCustomAttribute(this, con, binaryAttribute);
				}
				finally
				{
					EndSync();
				}
			}

	// Set the entry point for this assembly builder.
	public void SetEntryPoint(MethodInfo entryMethod)
			{
				SetEntryPoint(entryMethod, PEFileKinds.ConsoleApplication);
			}
	public void SetEntryPoint(MethodInfo entryMethod, PEFileKinds fileKind)
			{
				if(entryMethod == null)
				{
					throw new ArgumentNullException("entryMethod");
				}
				if(entryMethod.DeclaringType.Assembly != this)
				{
					throw new InvalidOperationException
						(_("Invalid_EntryNotInAssembly"));
				}
				lock(typeof(AssemblyBuilder))
				{
				/*	TODO
					ClrSetEntryPoint(((MethodBuilder)entryMethod).ClrHandle,
								 fileKind);
				*/
				}
				entryPoint = entryMethod;
			}

	// Set custom attributes on a program item in this assembly.
	[TODO]
	internal void SetCustomAttribute
				(IClrProgramItem item, CustomAttributeBuilder customBuilder)
			{
		 		throw new NotImplementedException("SetCustomAttribute");
			}
	[TODO]
	internal void SetCustomAttribute
				(IClrProgramItem item, ConstructorInfo con,
				 byte[] binaryAttribute)
			{
		 		throw new NotImplementedException("SetCustomAttribute");
			}

	// Add declarative security to a program item in this assembly.
	[TODO]
	internal void AddDeclarativeSecurity(IClrProgramItem item,
										 SecurityAction action,
									     PermissionSet pset)
			{
		 		throw new NotImplementedException("AddDeclarativeSecurity");
			}

	// Add an item to this assembly's detach list.
	internal void AddDetach(IDetachItem item)
			{
				lock(typeof(AssemblyBuilder))
				{
					detachList.Add(item);
				}
			}

	// Detach everything used by this assembly, which ensures that
	// all pointers to native structures are invalidated.
	private void Detach()
			{
				lock(typeof(AssemblyBuilder))
				{
					foreach(IDetachItem item in detachList)
					{
						item.Detach();
					}
					privateData = IntPtr.Zero;
				}
			}

	// Create a new assembly.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern private static IntPtr ClrAssemblyCreate
			(String name, int v1, int v2, int v3, int v4,
			 AssemblyBuilderAccess access, out IntPtr writer);

	// Internal version of "SetEntryPoint".
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern private void ClrSetEntryPoint(IntPtr clrMethod,
										 PEFileKinds fileKind);

	// Get the token associated with a particular program item.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern internal static int ClrGetItemToken(IntPtr item);

	// Get the program item associated with a particular token.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern internal static IntPtr ClrGetItemFromToken
			(IntPtr assembly, int token);

}; // class AssemblyBuilder

#endif // !ECMA_COMPAT

}; // namespace System.Reflection.Emit
