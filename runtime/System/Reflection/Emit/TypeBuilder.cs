/*
 * TypeBuilder.cs - Implementation of the
 *		"System.Reflection.Emit.TypeBuilder" class.
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
using System.Globalization;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Security;
using System.Security.Permissions;

public sealed class TypeBuilder : Type
{
	// Internal state.
	private AssemblyBuilder assembly;
	private Module module;
	private String name;
	private String nspace;
	private TypeAttributes attr;
	private Type parent;
	private Type[] interfaces;
	private PackingSize packingSize;
	private int typeSize;
	private Type declaringType;
	private ClrType type;
	private TypeToken token;

	// Constants.
	public const int UnspecifiedTypeSize = 0;

	// Constructor.
	internal TypeBuilder(AssemblyBuilder assembly, Module module,
						 String name, String nspace,
						 TypeAttributes attr, Type parent, Type[] interfaces,
						 PackingSize packingSize, int typeSize,
						 Type declaringType)
			{
				this.assembly = assembly;
				this.module = module;
				this.name = name;
				this.nspace = nspace;
				this.attr = attr;
				this.parent = parent;
				this.interfaces = interfaces;
				this.packingSize = packingSize;
				this.typeSize = typeSize;
				this.declaringType = declaringType;
				this.type = null;
				this.token = new TypeToken(0);	// TODO
			}

	// Get the attribute flags for this type.
	protected override TypeAttributes GetAttributeFlagsImpl()
			{
				return attr;
			}

	// Get the assembly associated with this type.
	public override Assembly Assembly
			{
				get
				{
					return module.Assembly;
				}
			}

	// Get the full qualified assembly name of this type.
	public override String AssemblyQualifiedName
			{
				get
				{
					return Assembly.CreateQualifiedName
						(module.Assembly.FullName, name);
				}
			}

	// Get the base type for this type.
	public override Type BaseType
			{
				get
				{
					return parent;
				}
			}

	// Get the declaring type for this type.
	public override Type DeclaringType
			{
				get
				{
					return declaringType;
				}
			}

	// Get the full name of this type.
	public override String FullName
			{
				get
				{
					if(declaringType != null)
					{
						return declaringType.FullName + "+" + name;
					}
					else if(nspace != null)
					{
						return nspace + "." + name;
					}
					else
					{
						return name;
					}
				}
			}

	// Get the GUID of this type.
	public override Guid GUID
			{
				get
				{
					throw new NotSupportedException(_("NotSupp_Builder"));
				}
			}

	// Retrieve the module that this type is defined within.
	public override Module Module
			{
				get
				{
					return module;
				}
			}

	// Get the name of this type.
	public override String Name
			{
				get
				{
					return name;
				}
			}

	// Get the namespace of this type.
	public override String Namespace
			{
				get
				{
					return nspace;
				}
			}

	// Get the packing size of this type.
	public PackingSize PackingSize
			{
				get
				{
					return packingSize;
				}
			}

	// Get the reflected type for this type.
	public override Type ReflectedType
			{
				get
				{
					return declaringType;
				}
			}

	// Get the total size of this type.
	public int Size
			{
				get
				{
					return typeSize;
				}
			}

	// Get the type handle for this type.
	public override RuntimeTypeHandle TypeHandle
			{
				get
				{
					throw new NotSupportedException(_("NotSupp_Builder"));
				}
			}

	// Get the type token for this type.
	public TypeToken TypeToken
			{
				get
				{
					return token;
				}
			}

	// Get the underlying system type for this type.
	[TODO]
	public override Type UnderlyingSystemType
			{
				get
				{
					// TODO: enumerated type handling.
					return this;
				}
			}

	// Check that the type has not yet been created.
	private void CheckNotCreated()
			{
				if(type != null)
				{
					throw new NotSupportedException(_("NotSupp_TypeCreated"));
				}
			}

	// Check that the type has been created.
	private void CheckCreated()
			{
				if(type == null)
				{
					throw new NotSupportedException
						(_("NotSupp_TypeNotCreated"));
				}
			}

	// Start a synchronized type builder operation.
	private void StartSync()
			{
				assembly.StartSync();
				if(type != null)
				{
					throw new NotSupportedException(_("NotSupp_TypeCreated"));
				}
			}

	// End a synchronized type builder operation.
	private void EndSync()
			{
				assembly.EndSync();
			}

	// Add declarative security to this type.
	[TODO]
	public void AddDeclarativeSecurity(SecurityAction action,
									   PermissionSet pset)
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

	// Add an interface to this type's implementation list.
	public void AddInterfaceImplementation(Type interfaceType)
			{
				try
				{
					// Start builder synchronization.
					StartSync();

					// Validate the parameters.
					if(interfaceType == null)
					{
						throw new ArgumentNullException("interfaceType");
					}

					// Bail out if the supplied parameter is not an interface.
					// We should probably throw an exception, but MS doesn't.
					if(!interfaceType.IsInterface)
					{
						return;
					}

					// Bail out if this type is inherited by the interface
					// so that we cannot create circular class structures.
					if(IsAssignableFrom(interfaceType))
					{
						return;
					}

					// Determine if we already have this interface.
					if(interfaces != null)
					{
						int index;
						for(index = 0; index < interfaces.Length; ++index)
						{
							if(interfaceType.Equals(interfaces[index]))
							{
								return;
							}
						}
					}

					// Add the interface to the list.
					Type[] newInterfaces;
					if(interfaces != null)
					{
						newInterfaces = new Type [interfaces.Length + 1];
						Array.Copy(interfaces, newInterfaces,
								   interfaces.Length);
						newInterfaces[interfaces.Length] = interfaceType;
					}
					else
					{
						newInterfaces = new Type [1];
						newInterfaces[0] = interfaceType;
					}
					interfaces = newInterfaces;
				}
				finally
				{
					EndSync();
				}
			}

	// Create this type.
	[TODO]
	public Type CreateType()
			{
				// TODO
				return null;
			}

	// Define a constructor for this class.
	[TODO]
	public ConstructorBuilder DefineConstructor
				(MethodAttributes attributes,
				 CallingConventions callingConvention,
				 Type[] parameterTypes)
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

	// Define a default constructor for this class.
	public ConstructorBuilder DefineDefaultConstructor
				(MethodAttributes attributes)
			{
				return DefineConstructor(attributes,
										 CallingConventions.Standard,
										 null);
			}

	// Define an event for this class.
	[TODO]
	public EventBuilder DefineEvent
				(String name, EventAttributes attributes, Type eventType)
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

	// Define a field for this class.
	[TODO]
	public FieldBuilder DefineField
				(String name, Type type, FieldAttributes attributes)
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

	// Define static initialized data within this class.
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

	// Define a method for this class.
	[TODO]
	public MethodBuilder DefineMethod
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
	public MethodBuilder DefineMethod
				(String name, MethodAttributes attributes,
				 Type returnType, Type[] parameterTypes)
			{
				return DefineMethod(name, attributes,
									CallingConventions.Standard,
									returnType, parameterTypes);
			}

	// Define a method override declaration for this class.
	[TODO]
	public void DefineMethodOverride
				(MethodInfo methodInfoBody, MethodInfo methodInfoDeclaration)
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

	// Define a nested type within this class.
	[TODO]
	private TypeBuilder DefineNestedType
				(String name, TypeAttributes attr, Type parent,
				 Type[] interfaces, int typeSize, PackingSize packSize)
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
	public TypeBuilder DefineNestedType(String name)
			{
				return DefineNestedType(name,
									    TypeAttributes.NestedPrivate,
										null, null, 0,
										PackingSize.Unspecified);
			}
	public TypeBuilder DefineNestedType(String name, TypeAttributes attr)
			{
				return DefineNestedType(name, attr, null, null, 0,
										PackingSize.Unspecified);
			}
	public TypeBuilder DefineNestedType(String name, TypeAttributes attr,
										Type parent)
			{
				return DefineNestedType(name, attr, parent, null, 0,
										PackingSize.Unspecified);
			}
	public TypeBuilder DefineNestedType(String name, TypeAttributes attr,
										Type parent, int typeSize)
			{
				return DefineNestedType(name, attr, parent, null, typeSize,
										PackingSize.Unspecified);
			}
	public TypeBuilder DefineNestedType(String name, TypeAttributes attr,
										Type parent, PackingSize packSize)
			{
				return DefineNestedType(name, attr, parent, null, 0, packSize);
			}
	public TypeBuilder DefineNestedType(String name, TypeAttributes attr,
										Type parent, Type[] interfaces)
			{
				return DefineNestedType(name, attr, parent, interfaces,
										0, PackingSize.Unspecified);
			}

	// Define a PInvoke method for this class.
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

	// Define a property for this class.
	public PropertyBuilder DefineProperty
				(String name, PropertyAttributes attributes,
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

	// Define a type initializer for this class.
	public ConstructorBuilder DefineTypeInitializer()
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

	// Define uninitialized data within this class.
	public FieldBuilder DefineUninitializedData
				(String name, int size, FieldAttributes attributes)
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

	// Implementation of "GetConstructor" provided by subclasses.
	protected override ConstructorInfo
					GetConstructorImpl(BindingFlags bindingAttr,
								       Binder binder,
								       CallingConventions callingConventions,
								       Type[] types,
								       ParameterModifier[] modifiers)
			{
				CheckCreated();
				return type.GetConstructor(bindingAttr, binder,
										   callingConventions,
										   types, modifiers);
			}

	// Get all constructors for this type.
	public override ConstructorInfo[] GetConstructors(BindingFlags bindingAttr)
			{
				CheckCreated();
				return type.GetConstructors(bindingAttr);
			}

	// Get the custom attributes that are associated with this member.
	public override Object[] GetCustomAttributes(bool inherit)
			{
				CheckCreated();
				return type.GetCustomAttributes(inherit);
			}
	public override Object[] GetCustomAttributes(Type type, bool inherit)
			{
				CheckCreated();
				return this.type.GetCustomAttributes(type, inherit);
			}

	// Determine if custom attributes are defined for this member.
	public override bool IsDefined(Type type, bool inherit)
			{
				CheckCreated();
				return this.type.IsDefined(type, inherit);
			}

	// Get the element type.
	public override Type GetElementType()
			{
				throw new NotSupportedException(_("NotSupp_Builder"));
			}

	// Get an event from this type.
	public override EventInfo GetEvent(String name, BindingFlags bindingAttr)
			{
				CheckCreated();
				return type.GetEvent(name, bindingAttr);
			}

	// Get the list of all events within this type.
	public override EventInfo[] GetEvents()
			{
				CheckCreated();
				return type.GetEvents();
			}
	public override EventInfo[] GetEvents(BindingFlags bindingAttr)
			{
				CheckCreated();
				return type.GetEvents(bindingAttr);
			}

	// Get a field from this type.
	public override FieldInfo GetField(String name, BindingFlags bindingAttr)
			{
				CheckCreated();
				return type.GetField(name, bindingAttr);
			}

	// Get the list of all fields within this type.
	public override FieldInfo[] GetFields(BindingFlags bindingAttr)
			{
				CheckCreated();
				return type.GetFields(bindingAttr);
			}

	// Get an interface from within this type.
	public override Type GetInterface(String name, bool ignoreCase)
			{
				CheckCreated();
				return type.GetInterface(name, ignoreCase);
			}

	// Get an interface mapping for this type.
	public override InterfaceMapping GetInterfaceMap(Type interfaceType)
			{
				CheckCreated();
				return type.GetInterfaceMap(interfaceType);
			}

	// Get the list of all interfaces that are implemented by this type.
	public override Type[] GetInterfaces()
			{
				if(type != null)
				{
					return type.GetInterfaces();
				}
				else if(interfaces == null)
				{
					return new Type [0];
				}
				else
				{
					return (Type[])(interfaces.Clone());
				}
			}

	// Get a list of members that have a specific name.
	public override MemberInfo[] GetMember
				(String name, MemberTypes type, BindingFlags bindingAttr)
			{
				CheckCreated();
				return this.type.GetMember(name, type, bindingAttr);
			}

	// Get a list of all members in this type.
	public override MemberInfo[] GetMembers(BindingFlags bindingAttr)
			{
				CheckCreated();
				return type.GetMembers(bindingAttr);
			}

	// Implementation of "GetMethod".
	protected override MethodInfo GetMethodImpl
				(String name, BindingFlags bindingAttr,
				 Binder binder, CallingConventions callConvention,
				 Type[] types, ParameterModifier[] modifiers)
			{
				CheckCreated();
				return type.GetMethod(name, bindingAttr, binder,
									  callConvention, types, modifiers);
			}

	// Get a list of all methods in this type.
	public override MethodInfo[] GetMethods(BindingFlags bindingAttr)
			{
				CheckCreated();
				return type.GetMethods(bindingAttr);
			}

	// Get a nested type that is contained within this type.
	public override Type GetNestedType(String name, BindingFlags bindingAttr)
			{
				CheckCreated();
				return type.GetNestedType(name, bindingAttr);
			}

	// Get a list of all nested types in this type.
	public override Type[] GetNestedTypes(BindingFlags bindingAttr)
			{
				CheckCreated();
				return type.GetNestedTypes(bindingAttr);
			}

	// Get a list of all properites in this type.
	public override PropertyInfo[] GetProperties(BindingFlags bindingAttr)
			{
				CheckCreated();
				return type.GetProperties(bindingAttr);
			}

	// Get a specific property from within this type.
	protected override PropertyInfo GetPropertyImpl
				(String name, BindingFlags bindingAttr, Binder binder,
				 Type returnType, Type[] types, ParameterModifier[] modifiers)
			{
				CheckCreated();
				return type.GetProperty(name, bindingAttr, binder,
										returnType, types, modifiers);
			}

	// Determine if this type has an element type.
	protected override bool HasElementTypeImpl()
			{
				throw new NotSupportedException(_("NotSupp_Builder"));
			}

	// Invoke a member of this type.
	public override Object InvokeMember
				(String name, BindingFlags invokeAttr, Binder binder,
				 Object target, Object[] args, ParameterModifier[] modifiers,
				 CultureInfo culture, String[] namedParameters)
			{
				CheckCreated();
				return type.InvokeMember(name, invokeAttr, binder,
										 target, args, modifiers,
										 culture, namedParameters);
			}

	// Determine if this type is an array.
	protected override bool IsArrayImpl()
			{
				return false;
			}

	// Determine if this type is a "by reference" type.
	protected override bool IsByRefImpl()
			{
				return false;
			}
	
	// Determine if this type imports a COM type.
	protected override bool IsCOMObjectImpl()
			{
				return false;
			}

	// Determine if this is a pointer type.
	protected override bool IsPointerImpl()
			{
				return false;
			}

	// Determine if this is a primitive type.
	protected override bool IsPrimitiveImpl()
			{
				return false;
			}

	// Determine if this type is a subclass of "c".
	public override bool IsSubclassOf(Type c)
			{
				if(c == null)
				{
					// Can never be a subclass of "null".
					return false;
				}
				else if(type != null)
				{
					// We have been created, so use the underlying type.
					if(c is TypeBuilder)
					{
						Type otherType = ((TypeBuilder)c).type;
						if(otherType != null)
						{
							return type.IsSubclassOf(otherType);
						}
						else
						{
							return false;
						}
					}
					else
					{
						return type.IsSubclassOf(c);
					}
				}
				else
				{
					// This type isn't created yet, so scan up the
					// tree looking for the specified type.
					if(this == c || parent == c)
					{
						return true;
					}
					else if(parent != null)
					{
						return parent.IsSubclassOf(c);
					}
					else
					{
						return false;
					}
				}
			}

	// Set a custom attribute on this type builder.
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

	// Set the parent of this type.
	public void SetParent(Type parent)
			{
				if(parent == null)
				{
					throw new ArgumentNullException("parent");
				}
				try
				{
					StartSync();
					this.parent = parent;
				}
				finally
				{
					EndSync();
				}
			}

	// Convert this object into a string.
	public override String ToString()
			{
				return name;
			}

}; // class TypeBuilder

#endif // !ECMA_COMPAT

}; // namespace System.Reflection.Emit
