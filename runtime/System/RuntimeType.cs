/*
 * RuntimeType.cs - Implementation of the "System.RuntimeType" class.
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

using System.Reflection;
using System.Globalization;
using System.Runtime.CompilerServices;

internal class RuntimeType : Type, ICloneable
{
	// Internal state.
	internal IntPtr privateData;

	// Constructor.
	internal RuntimeType() : base() { privateData = (IntPtr)0; }

	// Clone this object.
	public Object Clone()
			{
				return this;
			}

	// Get the rank of this type, if it is an array.
	public override int GetArrayRank()
			{
				if(IsArray)
				{
					return InternalGetArrayRank();
				}
				else
				{
					throw new NotSupportedException(_("NotSupp_NotArrayType"));
				}
			}
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern private int InternalGetArrayRank();

	// Get the attribute flags for this type.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern protected override TypeAttributes GetAttributeFlagsImpl();

	// Implementation of "GetConstructor" provided by subclasses.
	protected override ConstructorInfo
					GetConstructorImpl(BindingFlags bindingAttr,
								       Binder binder,
								       CallingConventions callingConventions,
								       Type[] types,
								       ParameterModifier[] modifiers)
			{
				// TODO
				return null;
			}

	// Get all constructors for this type.
	public override ConstructorInfo[] GetConstructors(BindingFlags bindingAttr)
			{
				return GetConstructorsInternal(bindingAttr, true);
			}
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern private ConstructorInfo[] GetConstructorsInternal
						(BindingFlags bindingAttr, bool verifyAccess);

	// Get the custom attributes for this type.
	public override Object[] GetCustomAttributes(bool inherit)
			{
				return System.Reflection.RuntimeHelpers.GetCustomAttributes
							(privateData, (IntPtr)0, inherit);
			}
	public override Object[] GetCustomAttributes(Type type, bool inherit)
			{
				return System.Reflection.RuntimeHelpers.GetCustomAttributes
							(privateData, type, inherit);
			}

	// Determine if custom attributes are defined for this type.
	public override bool IsDefined(Type type, bool inherit)
			{
				return System.Reflection.RuntimeHelpers
							.IsDefined(privateData, type, inherit);
			}

	// Get the element type for this type.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public override Type GetElementType();

	// Get an event from this type.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public override EventInfo GetEvent
				(String name, BindingFlags bindingAttr);

	// Get all events from this type.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public override EventInfo[] GetEvents(BindingFlags bindingAttr);

	// Get a field from this type.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public override FieldInfo GetField
				(String name, BindingFlags bindingAttr);

	// Get all fields from this type.
	public override FieldInfo[] GetFields(BindingFlags bindingAttr)
			{
				return InternalGetFields(bindingAttr, true);
			}
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern private FieldInfo[] InternalGetFields(BindingFlags bindingAttr,
												 bool verifyAccess);

	// Get the hash code for this type.
	public override int GetHashCode()
			{
				// We convert via long to prevent IntPtr throwing
				// an overflow exception on 64-bit platforms.
				return unchecked((int)(long)privateData);
			}

	// Get an interface that this type implements.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public override Type GetInterface(String name, bool ignoreCase);

	// Get all interfaces that this type implements.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public override Type[] GetInterfaces();

	// Get a member from this type.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public override MemberInfo GetMember
			(String name, BindingFlags bindingAttr);

	// Get all members from this type.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public override MemberInfo[] GetMembers(BindingFlags bindingAttr);

	// Implementation of "GetMethod" provided by subclasses.
	protected override MethodInfo
					GetMethodImpl(String name,
								  BindingFlags bindingAttr, Binder binder,
								  CallingConventions callingConventions,
								  Type[] types,
								  ParameterModifier[] modifiers)
			{
				// TODO
				return null;
			}

	// Get all methods from this type.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public override MethodInfo[] GetMethods(BindingFlags bindingAttr);

	// Get a nested type from this type.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public override Type GetNestedType
				(String name, BindingFlags bindingAttr);

	// Get all nested types from this type.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public override Type[] GetNestedTypes(BindingFlags bindingAttr);

	// Implementation of "GetProperty" provided by subclasses.
	protected override PropertyInfo
					GetPropertyImpl(String name,
									BindingFlags bindingAttr, Binder binder,
								    Type returnType, Type[] types,
								    ParameterModifier[] modifiers)
			{
				// TODO
				return null;
			}

	// Get all properties from this type.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public override PropertyInfo[] GetProperties
				(BindingFlags bindingAttr);

	// Get the runtime type handle associated with an object.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern internal static RuntimeTypeHandle
				InternalGetTypeHandleFromObject(Object obj);

	// Get a type from a runtime type handle.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern internal static Type GetTypeFromHandleImpl
				(RuntimeTypeHandle handle);

	// Implementation of the "HasElementType" property.
	protected override bool HasElementTypeImpl()
			{
				return (IsArray || IsPointer || IsByRef);
			}

	// Invoke a member.
	public override Object InvokeMember
						(String name, BindingFlags invokeAttr,
					     Binder binder, Object target, Object[] args,
					     ParameterModifier[] modifiers,
					     CultureInfo culture, String[] namedParameters)
			{
				// TODO
				return null;
			}

	// Implementation of the "IsArray" property.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern protected override bool IsArrayImpl();

	// Implementation of the "IsByRef" property.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern protected override bool IsByRefImpl();

	// Implementation of the "IsCOMObject" property.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern protected override bool IsCOMObjectImpl();

	// Implementation of the "IsContextful" property.
	protected override bool IsContextfulImpl()
			{
				return (InternalIsContextful() != 0);
			}
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern private int InternalIsContextful();


	// Implementation of the "IsMarshalByRef" property.
	protected override bool IsMarshalByRefImpl()
			{
				return (InternalIsMarshalByRef() != 0);
			}
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern private int InternalIsMarshalByRef();

	// Implementation of the "IsPointer" property.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern protected override bool IsPointerImpl();

	// Implementation of the "IsPrimitive" property.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern protected override bool IsPrimitiveImpl();

	// Determine if the current type is a subclass of another type.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public override bool IsSubclassOf(Type c);

	// Determine if this is a nested type.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern private bool IsNestedTypeImpl();

	// Convert this type into a string.
	public override String ToString()
			{
				return InternalGetFullName();
			}

	// Internal methods for supporting the properties.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern private System.Reflection.Assembly InternalGetAssembly();

	[MethodImpl(MethodImplOptions.InternalCall)]
	extern private Type InternalGetBaseType();

	[MethodImpl(MethodImplOptions.InternalCall)]
	extern private String InternalGetFullName();

	[MethodImpl(MethodImplOptions.InternalCall)]
	extern private Guid InternalGetGUID();

	[MethodImpl(MethodImplOptions.InternalCall)]
	extern private System.Reflection.Module InternalGetModule();

	[MethodImpl(MethodImplOptions.InternalCall)]
	extern private Type InternalGetNestedDeclaringType();

	[MethodImpl(MethodImplOptions.InternalCall)]
	extern private RuntimeTypeHandle InternalGetClassHandle();

	[MethodImpl(MethodImplOptions.InternalCall)]
	extern private String InternalGetName();

	[MethodImpl(MethodImplOptions.InternalCall)]
	extern private String InternalGetNamespace();

	// Implement overridden properties.
	public override System.Reflection.Assembly Assembly
			{
				get
				{
					return InternalGetAssembly();
				}
			}
	public override String AssemblyQualifiedName
			{
				get
				{
					return FullName + ", " + Assembly.ToString();
				}
			}
	public override Type BaseType
			{
				get
				{
					return InternalGetBaseType();
				}
			}
	public override String FullName
			{
				get
				{
					return InternalGetFullName();
				}
			}
	public override Guid GUID
			{
				get
				{
					return InternalGetGUID();
				}
			}
	public override System.Reflection.Module Module
			{
				get
				{
					return InternalGetModule();
				}
			}
	public override String Name
			{
				get
				{
					return InternalGetName();
				}
			}
	public override String Namespace
			{
				get
				{
					return InternalGetNamespace();
				}
			}
	public override Type UnderlyingSystemType
			{
				get
				{
					return this;
				}
			}
	public override Type DeclaringType
			{
				get
				{
					if(IsNestedTypeImpl())
					{
						return InternalGetNestedDeclaringType();
					}
					else
					{
						return null;
					}
				}
			}
	public override MemberTypes MemberType
			{
				get
				{
					if(IsNestedTypeImpl())
					{
						return MemberTypes.NestedType;
					}
					else
					{
						return MemberTypes.TypeInfo;
					}
				}
			}
	public override Type ReflectedType
			{
				get
				{
					if(IsNestedTypeImpl())
					{
						return InternalGetNestedDeclaringType();
					}
					else
					{
						return null;
					}
				}
			}
	public override RuntimeTypeHandle TypeHandle
			{
				get
				{
					return InternalGetClassHandle();
				}
			}

}; // class RuntimeType

}; // namespace System
