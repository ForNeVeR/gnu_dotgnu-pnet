/*
 * Module.cs - Implementation of the "System.Reflection.Module" class.
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
using System.Globalization;
using System.Runtime.CompilerServices;

public class Module : IClrProgramItem, ICustomAttributeProvider
{
	// Internal state.
	private IntPtr privateData;

	// Constructor.
	internal Module() {}

	// Implement the IClrProgramItem interface.
	IntPtr IClrProgramItem.ClrHandle
			{
				get
				{
					return privateData;
				}
			}

	// Get a field from the module type.
	public FieldInfo GetField(String name)
			{
				return GetModuleType().GetField(name);
			}
	public FieldInfo GetField(String name, BindingFlags bindingAttr)
			{
				return GetModuleType().GetField(name, bindingAttr);
			}

	// Get all module type fields that match a specific set of criteria.
	public FieldInfo[] GetFields(BindingFlags bindingAttr)
			{
				return GetModuleType().GetFields(bindingAttr);
			}
	public FieldInfo[] GetFields()
			{
				return GetModuleType().GetFields();
			}

	// Get a method from the module type.
	public MethodInfo GetMethod(String name)
			{
				return GetMethodImpl(name, BindingFlags.Default,
									 null, CallingConventions.Any,
									 null, null);
			}
	public MethodInfo GetMethod(String name, Type[] types)
			{
				return GetMethodImpl(name, BindingFlags.Default,
									 null, CallingConventions.Any,
									 types, null);
			}
#if !ECMA_COMPAT
	public MethodInfo GetMethod(String name,
							    BindingFlags bindingAttr, Binder binder,
								CallingConventions callingConventions,
								Type[] types, ParameterModifier[] modifiers)
			{
				return GetMethodImpl(name, bindingAttr, binder,
									 callingConventions, types, modifiers);
			}
#endif // !ECMA_COMPAT

	// Get a method implementation that matches a specific set of criteria.
#if ECMA_COMPAT
	internal
#endif
	protected virtual MethodInfo GetMethodImpl
				(String name, BindingFlags bindingAttr, Binder binder,
				 CallingConventions callingConventions,
				 Type[] types, ParameterModifier[] modifiers)
			{
				return GetModuleType().GetMethod
						(name, bindingAttr, binder,
						 callingConventions, types, modifiers);
			}

	// Get all module type methods that match a specific set of criteria.
	public MethodInfo[] GetMethods(BindingFlags bindingAttr)
			{
				return GetModuleType().GetMethods(bindingAttr);
			}
	public MethodInfo[] GetMethods()
			{
				return GetModuleType().GetMethods();
			}

	// Convert the module into a string.
	public override String ToString()
			{
				return Name;
			}

	// Get the assembly that contains this module.
	public System.Reflection.Assembly Assembly
			{
				get
				{
					return GetAssembly();
				}
			}

	// Get the fully-qualified name for this module.
	public virtual String FullyQualifiedName
			{
				get
				{
					return GetFullName();
				}
			}

	// Get the name of this module.
	public String Name
			{
				get
				{
					return ClrHelpers.GetName(privateData);
				}
			}

#if !ECMA_COMPAT

	// Get the scope name for this module.
	public virtual String ScopeName
			{
				get
				{
					return ClrHelpers.GetName(privateData);
				}
			}

	// Get custom attributes for this module.
	public Object[] GetCustomAttributes(bool inherit)
			{
				return ClrHelpers.GetCustomAttributes(this, inherit);
			}
	public Object[] GetCustomAttributes(Type type, bool inherit)
			{
				return ClrHelpers.GetCustomAttributes(this, type, inherit);
			}
	
	// Determine if a custom attribute is defined for this module.
	public bool IsDefined(Type type, bool inherit)
			{
				return ClrHelpers.IsDefined(this, type, inherit);
			}

	// Get a type from this module.
	public virtual Type GetType(String name)
			{
				return GetType(name, false, false);
			}
	public virtual Type GetType(String name, bool ignoreCase)
			{
				return GetType(name, false, ignoreCase);
			}
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public virtual Type GetType(String name, bool throwOnError,
									   bool ignoreCase);

	// Return all types that are defined in this module.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public virtual Type[] GetTypes();

	// Determine if this module is a resource.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public bool IsResource();

#else // ECMA_COMPAT

	// Get custom attributes for this module.
	Object[] ICustomAttributeProvider.GetCustomAttributes(bool inherit)
			{
				return ClrHelpers.GetCustomAttributes(this, inherit);
			}
	Object[] ICustomAttributeProvider.GetCustomAttributes
					(Type type, bool inherit)
			{
				return ClrHelpers.GetCustomAttributes(this, type, inherit);
			}
	
	// Determine if a custom attribute is defined for this module.
	bool ICustomAttributeProvider.IsDefined(Type type, bool inherit)
			{
				return ClrHelpers.IsDefined(this, type, inherit);
			}

#endif // ECMA_COMPAT

	// Get the module type.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern internal virtual Type GetModuleType();

	// Get the assembly that contains this module.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern private Assembly GetAssembly();

	// Get the fully-qualified name for this module.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern private String GetFullName();

}; // class Module

}; // namespace System.Reflection