/*
 * ModuleBuilder.cs - Implementation of the
 *		"System.Reflection.Emit.ModuleBuilder" class.
 *
 * Copyright (C) 2002  Southern Storm Software, Pty Ltd.
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
using System.Security;
using System.Resources;
using System.Reflection;
using System.Security.Policy;
using System.Diagnostics.SymbolStore;
using System.Runtime.InteropServices;
using System.Runtime.CompilerServices;

public class ModuleBuilder : Module
{
	// Internal state.
	private AssemblyBuilder assembly;
	private String name;
	private bool transient;
	private TypeBuilder moduleType;

	// Constructor.
	internal ModuleBuilder(AssemblyBuilder assembly, String name,
						   bool transient)
			{
				// Initialize the fields within this object.
				this.assembly = assembly;
				this.name = name;
				this.transient = transient;

				// Create a new module within the assembly.
				privateData = ClrModuleCreate(assembly.privateData, name);

				// Create the module type for this module.
				moduleType = new TypeBuilder(assembly, this, "<Module>",
											 null, TypeAttributes.NotPublic,
											 null, null,
											 PackingSize.Unspecified, 0, null);
			}

	// Get the fully qualified name of this module.
	public override String FullyQualifiedName
			{
				get
				{
					return name;
				}
			}

	// Start a synchronized operation on this module.
	private void StartSync()
			{
				assembly.StartSync();
			}

	// End a synchronized operation on this module.
	private void EndSync()
			{
				assembly.EndSync();
			}

	// Create the global functions in this module.
	[TODO]
	public void CreateGlobalFunctions()
			{
				// TODO
			}

	// TODO - debug symbol support

	// Define a document for source.
	[TODO]
	public ISymbolDocumentWriter DefineDocument
				(String url, Guid language,
				 Guid languageVendor, Guid documentType)
			{
				// TODO
				return null;
			}

	// Define an enumerated type within this module.
	[TODO]
	public EnumBuilder DefineEnum(String name, TypeAttributes visibility,
								  Type underlyingType)
			{
				try
				{
					StartSync();
					// TODO
					return null;
				}
				finally
				{
					EndSync();
				}
			}

	// Define a global method within this module.
	[TODO]
	public MethodBuilder DefineGlobalMethod
				(String name, MethodAttributes attributes,
				 CallingConventions callingConvention,
				 Type returnType, Type[] parameterTypes)
			{
				try
				{
					StartSync();
					// TODO
					return null;
				}
				finally
				{
					EndSync();
				}
			}
	public MethodBuilder DefineGlobalMethod
				(String name, MethodAttributes attributes,
				 Type returnType, Type[] parameterTypes)
			{
				return DefineGlobalMethod(name, attributes,
										  CallingConventions.Standard,
										  returnType, parameterTypes);
			}

	// Define initialized data as a global field.
	[TODO]
	public FieldBuilder DefineInitializedData
				(String name, byte[] data, FieldAttributes attributes)
			{
				try
				{
					StartSync();
					// TODO
					return null;
				}
				finally
				{
					EndSync();
				}
			}

	// Define a global PInvoke method.
	[TODO]
	public MethodBuilder DefinePInvokeMethod
				(String name, String dllName, String entryName,
				 MethodAttributes attributes,
				 CallingConventions callingConvention,
				 Type returnType, Type[] parameterTypes,
				 CallingConvention nativeCallConv,
				 CharSet nativeCharSet)
			{
				try
				{
					StartSync();
					// TODO
					return null;
				}
				finally
				{
					EndSync();
				}
			}
	public MethodBuilder DefinePInvokeMethod
				(String name, String dllName,
				 MethodAttributes attributes,
				 CallingConventions callingConvention,
				 Type returnType, Type[] parameterTypes,
				 CallingConvention nativeCallConv,
				 CharSet nativeCharSet)
			{
				return DefinePInvokeMethod(name, dllName, name,
										   attributes, callingConvention,
										   returnType, parameterTypes,
										   nativeCallConv, nativeCharSet);
			}

	// Define a resource within a module.
	[TODO]
	public IResourceWriter DefineResource
				(String name, String description,
				 ResourceAttributes attribute)
			{
				try
				{
					StartSync();
					// TODO
					return null;
				}
				finally
				{
					EndSync();
				}
			}
	public IResourceWriter DefineResource(String name, String description)
			{
				return DefineResource(name, description,
									  ResourceAttributes.Public);
			}

	// Define a type witin this module.
	[TODO]
	private TypeBuilder DefineType(String name, TypeAttributes attr,
								   Type parent, Type[] interfaces,
								   PackingSize packSize, int typeSize)
			{
				try
				{
					StartSync();
					// TODO
					return null;
				}
				finally
				{
					EndSync();
				}
			}
	public TypeBuilder DefineType(String name)
			{
				return DefineType(name, TypeAttributes.NotPublic,
								  null, null, PackingSize.Unspecified, 0);
			}
	public TypeBuilder DefineType(String name, TypeAttributes attr)
			{
				return DefineType(name, attr, null, null,
								  PackingSize.Unspecified, 0);
			}
	public TypeBuilder DefineType(String name, TypeAttributes attr,
								  Type parent)
			{
				return DefineType(name, attr, parent, null,
								  PackingSize.Unspecified, 0);
			}
	public TypeBuilder DefineType(String name, TypeAttributes attr,
								  Type parent, int typeSize)
			{
				return DefineType(name, attr, parent, null,
								  PackingSize.Unspecified, typeSize);
			}
	public TypeBuilder DefineType(String name, TypeAttributes attr,
								  Type parent, PackingSize packSize)
			{
				return DefineType(name, attr, parent, null,
								  packSize, 0);
			}
	public TypeBuilder DefineType(String name, TypeAttributes attr,
								  Type parent, Type[] interfaces)
			{
				return DefineType(name, attr, parent, interfaces,
								  PackingSize.Unspecified, 0);
			}
	public TypeBuilder DefineType(String name, TypeAttributes attr,
								  Type parent, PackingSize packSize,
								  int typeSize)
			{
				return DefineType(name, attr, parent, null,
								  packSize, typeSize);
			}

	// Define uninitialized data as a global field.
	[TODO]
	public FieldBuilder DefineUninitializedData
				(String name, byte[] data, FieldAttributes attributes)
			{
				try
				{
					StartSync();
					// TODO
					return null;
				}
				finally
				{
					EndSync();
				}
			}

	// Define an unmanaged resource within this module.
	[TODO]
	public void DefineUnmanagedResource(byte[] resource)
			{
				try
				{
					StartSync();
					// TODO
				}
				finally
				{
					EndSync();
				}
			}
	[TODO]
	public void DefineUnmanagedResource(String resourceFileName)
			{
				try
				{
					StartSync();
					// TODO
				}
				finally
				{
					EndSync();
				}
			}

	// Get a method on an array class.
	[TODO]
	public MethodInfo GetArrayMethod
				(Type arrayClass, String methodName,
				 CallingConventions callingConvention,
				 Type returnType, Type[] parameterTypes)
			{
				try
				{
					StartSync();
					// TODO
					return null;
				}
				finally
				{
					EndSync();
				}
			}

	// Get the token for a method on an array class.
	[TODO]
	public MethodToken GetArrayMethodToken
				(Type arrayClass, String methodName,
				 CallingConventions callingConvention,
				 Type returnType, Type[] parameterTypes)
			{
				try
				{
					StartSync();
					// TODO
					return new MethodToken(0);
				}
				finally
				{
					EndSync();
				}
			}

	// Get the token for a constructor within this module.  Returns
	// a member reference if the constructor is in another assembly.
	[TODO]
	public MethodToken GetConstructorToken(ConstructorInfo con)
			{
				// TODO
				return new MethodToken(0);
			}

	// Get the token for a field within this module.  Returns
	// a member reference if the constructor is in another assembly.
	[TODO]
	public FieldToken GetFieldToken(FieldInfo field)
			{
				// TODO
				return new FieldToken(0);
			}

	// Get the token for a method within this module.  Returns
	// a member reference if the constructor is in another assembly.
	[TODO]
	public MethodToken GetMethodToken(MethodInfo method)
			{
				// TODO
				return new MethodToken(0);
			}

	// Get a token for a signature within this module.
	[TODO]
	public SignatureToken GetSignatureToken(SignatureHelper sigHelper)
			{
				// TODO
				return new SignatureToken(0);
			}
	[TODO]
	public SignatureToken GetSignatureToken(byte[] sigBytes, int sigLength)
			{
				// TODO
				return new SignatureToken(0);
			}

	// Get a token for a string constant within this module.
	[TODO]
	public StringToken GetStringConstant(String str)
			{
				// TODO
				return new StringToken(0);
			}

	// Get the symbol writer associated with this module.
	[TODO]
	public ISymbolWriter GetSymWriter()
			{
				// TODO
				return null;
			}

	// Find a type within this module.  Returns NULL if not present.
	[TODO]
	private Type FindType(String className)
			{
				// TODO
				return null;
			}

	// Find a type within this module, while ignoring the case in names.
	// Returns NULL if not present.
	[TODO]
	private Type FindTypeIgnoreCase(String className)
			{
				// TODO
				return null;
			}

	// Get a type from within this module.
	public override Type GetType(String className)
			{
				return FindType(className);
			}
	public override Type GetType(String className, bool ignoreCase)
			{
				if(ignoreCase)
				{
					return FindTypeIgnoreCase(className);
				}
				else
				{
					return FindType(className);
				}
			}
	public override Type GetType(String className, bool throwOnError,
								 bool ignoreCase)
			{
				Type type;
				if(ignoreCase)
				{
					type = FindTypeIgnoreCase(className);
				}
				else
				{
					type = FindType(className);
				}
				if(type == null && throwOnError)
				{
					throw new TypeLoadException(_("Exception_TypeLoad"));
				}
				return type;
			}

	// Get all classes that were defined in this module.
	[TODO]
	public override Type[] GetTypes()
			{
				// TODO
				return null;
			}

	// Get a type token by name.
	[TODO]
	public TypeToken GetTypeToken(String name)
			{
				// TODO
				return new TypeToken(0);
			}

	// Get a type token by type.
	[TODO]
	public TypeToken GetTypeToken(Type type)
			{
				// TODO
				return new TypeToken(0);
			}

	// Determine if this module is transient.
	public bool IsTransient()
			{
				return transient;
			}

	// Set a custom attribute on this module.
	[TODO]
	public void SetCustomAttribute(CustomAttributeBuilder customBuilder)
			{
				try
				{
					StartSync();
					// TODO
				}
				finally
				{
					EndSync();
				}
			}
	[TODO]
	public void SetCustomAttribute(ConstructorInfo con,
								   byte[] binaryAttribute)
			{
				try
				{
					StartSync();
					// TODO
				}
				finally
				{
					EndSync();
				}
			}

	// Set a custom attribute that is stored with symbolic information.
	[TODO]
	public void SetSymCustomAttribute(String name, byte[] data)
			{
				try
				{
					StartSync();
					// TODO
				}
				finally
				{
					EndSync();
				}
			}

	// Set the user entry point for this module.
	[TODO]
	public void SetUserEntryPoint(MethodInfo entryPoint)
			{
				try
				{
					StartSync();
					// TODO
				}
				finally
				{
					EndSync();
				}
			}

	// Create a module within an assembly.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern private static IntPtr ClrModuleCreate(IntPtr assembly, String name);

}; // class ModuleBuilder

#endif // !ECMA_COMPAT

}; // namespace System.Reflection.Emit
