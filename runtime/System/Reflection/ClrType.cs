/*
 * ClrType.cs - Implementation of the "System.Reflection.ClrType" class.
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

internal class ClrType : Type, ICloneable, IClrProgramItem
{
	// Internal state.
	internal IntPtr privateData;

	// Constructor.  This class is normally instantiated
	// by the runtime engine, not by the class library.
	internal ClrType() : base() { privateData = IntPtr.Zero; }

	// Implement the ICloneable interface.
	public Object Clone()
			{
				return this;
			}

	// Implement the IClrProgramItem interface.
	public IntPtr ClrHandle
			{
				get
				{
					return privateData;
				}
			}

	// Get the rank of this type, if it is an array.
	public override int GetArrayRank()
			{
				if(IsArray)
				{
					return GetClrArrayRank();
				}
				else
				{
					throw new NotSupportedException(_("NotSupp_NotArrayType"));
				}
			}
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern private int GetClrArrayRank();

	// Get the attribute flags for this type.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern protected override TypeAttributes GetAttributeFlagsImpl();

	// Get the custom attributes for this type.
	public override Object[] GetCustomAttributes(bool inherit)
			{
				return ClrHelpers.GetCustomAttributes(this, inherit);
			}
	public override Object[] GetCustomAttributes(Type type, bool inherit)
			{
				return ClrHelpers.GetCustomAttributes(this, type, inherit);
			}

	// Determine if custom attributes are defined for this type.
	public override bool IsDefined(Type type, bool inherit)
			{
				return ClrHelpers.IsDefined(this, type, inherit);
			}

	// Get the element type for this type.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public override Type GetElementType();

	// Get the hash code for this type.
	public override int GetHashCode()
			{
				// We convert via long to prevent IntPtr throwing
				// an overflow exception on 64-bit platforms.
				return unchecked((int)(privateData.ToInt64()));
			}

	// Get an interface that this type implements.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public override Type GetInterface(String name, bool ignoreCase);

	// Get all interfaces that this type implements.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public override Type[] GetInterfaces();

	// Get a member from this type.  The member could be a field,
	// method, constructor, event, property, or nested type.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern private MemberInfo GetMemberImpl
				(String name, MemberTypes memberTypes,
			     BindingFlags bindingAttr, Binder binder,
				 CallingConventions callingConventions,
				 Type[] types, ParameterModifier[] modifiers);

	// Get a list of members from this type.  Returns an array,
	// which must then be cast to the correct type.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern private Object GetMembersImpl(MemberTypes memberTypes,
										 BindingFlags bindingAttr,
										 Type arrayType);

	// Implementation of "GetConstructor" provided by subclasses.
	protected override ConstructorInfo
					GetConstructorImpl(BindingFlags bindingAttr,
								       Binder binder,
								       CallingConventions callingConventions,
								       Type[] types,
								       ParameterModifier[] modifiers)
			{
				return (ConstructorInfo)GetMemberImpl
							(".ctor", MemberTypes.Constructor,
							 bindingAttr, binder, callingConventions,
							 types, modifiers);
			}

	// Get all constructors for this type.
	public override ConstructorInfo[] GetConstructors(BindingFlags bindingAttr)
			{
				return (ConstructorInfo[])GetMembersImpl
							(MemberTypes.Constructor, bindingAttr,
							 typeof(ConstructorInfo[]));
			}

	// Get an event from this type.
	public override EventInfo GetEvent(String name, BindingFlags bindingAttr)
			{
				return (EventInfo)GetMemberImpl
							(name, MemberTypes.Event,
							 bindingAttr, null, CallingConventions.Any,
							 null, null);
			}

	// Get all events from this type.
	public override EventInfo[] GetEvents(BindingFlags bindingAttr)
			{
				return (EventInfo[])GetMembersImpl
							(MemberTypes.Event, bindingAttr,
							 typeof(EventInfo[]));
			}

	// Get a field from this type.
	public override FieldInfo GetField(String name, BindingFlags bindingAttr)
			{
				return (FieldInfo)GetMemberImpl
							(name, MemberTypes.Field,
							 bindingAttr, null, CallingConventions.Any,
							 null, null);
			}

	// Get all fields from this type.
	public override FieldInfo[] GetFields(BindingFlags bindingAttr)
			{
				return (FieldInfo[])GetMembersImpl
							(MemberTypes.Field, bindingAttr,
							 typeof(FieldInfo[]));
			}

	// Get a member from this type.
	public override MemberInfo GetMember(String name, BindingFlags bindingAttr)
			{
				return GetMemberImpl(name, MemberTypes.All,
									 bindingAttr, null,
									 CallingConventions.Any,
									 null, null);
			}

	// Get all members from this type.
	public override MemberInfo[] GetMembers(BindingFlags bindingAttr)
			{
				return (MemberInfo[])GetMembersImpl
					(MemberTypes.All, bindingAttr, typeof(MemberInfo[]));
			}

	// Implementation of "GetMethod" provided by subclasses.
	protected override MethodInfo
					GetMethodImpl(String name,
								  BindingFlags bindingAttr, Binder binder,
								  CallingConventions callingConventions,
								  Type[] types,
								  ParameterModifier[] modifiers)
			{
				return (MethodInfo)GetMemberImpl
							(name, MemberTypes.Method,
							 bindingAttr, binder, callingConventions,
							 types, modifiers);
			}

	// Get all methods from this type.
	public override MethodInfo[] GetMethods(BindingFlags bindingAttr)
			{
				return (MethodInfo[])GetMembersImpl
							(MemberTypes.Method, bindingAttr,
							 typeof(MethodInfo[]));
			}

	// Get a nested type from this type.
	public override Type GetNestedType(String name, BindingFlags bindingAttr)
			{
				return (Type)GetMemberImpl
							(name, MemberTypes.NestedType,
							 bindingAttr, null, CallingConventions.Any,
							 null, null);
			}

	// Get all nested types from this type.
	public override Type[] GetNestedTypes(BindingFlags bindingAttr)
			{
				return (Type[])GetMembersImpl
							(MemberTypes.NestedType, bindingAttr,
							 typeof(Type[]));
			}

	// Implementation of "GetProperty" provided by subclasses.
	protected override PropertyInfo
					GetPropertyImpl(String name,
									BindingFlags bindingAttr, Binder binder,
								    Type returnType, Type[] types,
								    ParameterModifier[] modifiers)
			{
				return (PropertyInfo)GetMemberImpl
							(name, MemberTypes.Property,
							 bindingAttr, binder, CallingConventions.Any,
							 types, modifiers);
			}

	// Get all properties from this type.
	public override PropertyInfo[] GetProperties(BindingFlags bindingAttr)
			{
				return (PropertyInfo[])GetMembersImpl
							(MemberTypes.Property, bindingAttr,
							 typeof(PropertyInfo[]));
			}

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

	// Get the category of this type.  Array, pointer, byref, primitive, etc.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern private ClrTypeCategory GetClrTypeCategory();

	// Implementation of the "IsArray" property.
	protected override bool IsArrayImpl()
			{
				return (GetClrTypeCategory() == ClrTypeCategory.Array);
			}

	// Implementation of the "IsByRef" property.
	protected override bool IsByRefImpl()
			{
				return (GetClrTypeCategory() == ClrTypeCategory.ByRef);
			}

	// Implementation of the "IsCOMObject" property.
	protected override bool IsCOMObjectImpl()
			{
				return (GetClrTypeCategory() == ClrTypeCategory.COMObject);
			}

	// Implementation of the "IsContextful" property.
	protected override bool IsContextfulImpl()
			{
				return IsSubclassOf(typeof(System.ContextBoundObject));
			}

	// Implementation of the "IsMarshalByRef" property.
	protected override bool IsMarshalByRefImpl()
			{
				return IsSubclassOf(typeof(System.MarshalByRefObject));
			}

	// Implementation of the "IsPointer" property.
	protected override bool IsPointerImpl()
			{
				return (GetClrTypeCategory() == ClrTypeCategory.Pointer);
			}

	// Implementation of the "IsPrimitive" property.
	protected override bool IsPrimitiveImpl()
			{
				return (GetClrTypeCategory() == ClrTypeCategory.Primitive);
			}

	// Determine if the current type is a subclass of another type.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public override bool IsSubclassOf(Type c);

	// Determine if this is a nested type.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern private bool IsClrNestedType();

	// Convert this type into a string.
	public override String ToString()
			{
				return GetClrFullName();
			}

	// Internal methods for supporting the properties.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern private System.Reflection.Assembly GetClrAssembly();

	[MethodImpl(MethodImplOptions.InternalCall)]
	extern private Type GetClrBaseType();

	[MethodImpl(MethodImplOptions.InternalCall)]
	extern private String GetClrFullName();

#if !ECMA_COMPAT
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern private Guid GetClrGUID();
#endif

	[MethodImpl(MethodImplOptions.InternalCall)]
	extern private System.Reflection.Module GetClrModule();

	[MethodImpl(MethodImplOptions.InternalCall)]
	extern private Type GetClrNestedDeclaringType();

	[MethodImpl(MethodImplOptions.InternalCall)]
	extern private String GetClrName();

	[MethodImpl(MethodImplOptions.InternalCall)]
	extern private String GetClrNamespace();

	// Implement overridden properties.
	public override System.Reflection.Assembly Assembly
			{
				get
				{
					return GetClrAssembly();
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
					return GetClrBaseType();
				}
			}
	public override String FullName
			{
				get
				{
					return GetClrFullName();
				}
			}
#if !ECMA_COMPAT
	public override Guid GUID
			{
				get
				{
					return GetClrGUID();
				}
			}
#endif
	public override System.Reflection.Module Module
			{
				get
				{
					return GetClrModule();
				}
			}
	public override String Name
			{
				get
				{
					return GetClrName();
				}
			}
	public override String Namespace
			{
				get
				{
					return GetClrNamespace();
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
					if(IsClrNestedType())
					{
						return GetClrNestedDeclaringType();
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
					if(IsClrNestedType())
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
					if(IsClrNestedType())
					{
						return GetClrNestedDeclaringType();
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
					return new RuntimeTypeHandle(privateData);
				}
			}

}; // class ClrType

}; // namespace System.Reflection
