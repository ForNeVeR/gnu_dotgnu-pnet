/*
 * Type.cs - Implementation of the "System.Type" class.
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

public abstract class Type : MemberInfo
{

	// The only instance of "Missing" in the system.
	public static readonly Object Missing = System.Reflection.Missing.Value;

	// The delimiter for type names.
	public static readonly char Delimiter = '.';

	// An empty array of types.
	public static readonly Type[] EmptyTypes = new Type [0];

	// Cached copies of useful type descriptors.
	private static readonly Type objectType = typeof(Object);
	private static readonly Type valueType  = typeof(ValueType);
	private static readonly Type enumType   = typeof(Enum);

	// Constructor.
	protected Type() : base() {}

	// Type equality testing.
	public override bool Equals(Object obj)
			{
				if(obj == null || !(obj is Type))
				{
					return false;
				}
				return (UnderlyingSystemType ==
						((Type)obj).UnderlyingSystemType);
			}
	public bool Equals(Type obj)
			{
				if(obj == null)
				{
					return false;
				}
				return (UnderlyingSystemType == obj.UnderlyingSystemType);
			}

	// Get the rank of this type, if it is an array.
	public virtual int GetArrayRank()
			{
				throw new NotSupportedException(_("NotSupp_NotArrayType"));
			}

	// Get the attribute flags for this type.
	protected abstract TypeAttributes GetAttributeFlagsImpl();

	// Validate the "types" argument to a "GetConstructor" or "GetMethod" call.
	private static void ValidateTypes(Type[] types)
			{
				int posn;
				if(types == null)
				{
					throw new ArgumentNullException("types");
				}
				for(posn = 0; posn < types.Length; ++posn)
				{
					if(types[posn] == null)
					{
						throw new ArgumentNullException("types");
					}
				}
			}

	// Get a constructor from this type.
	public ConstructorInfo GetConstructor(BindingFlags bindingAttr,
										  Binder binder, Type[] types,
										  ParameterModifier[] modifiers)
			{
				return GetConstructor(bindingAttr, binder,
									  CallingConventions.Any,
									  types, modifiers);
			}
	public ConstructorInfo GetConstructor(Type[] types)
			{
				return GetConstructor(BindingFlags.Public |
									  BindingFlags.Instance, null,
									  CallingConventions.Any,
									  types, null);
			}
	public ConstructorInfo GetConstructor(BindingFlags bindingAttr,
										  Binder binder,
										  CallingConventions callingConventions,
										  Type[] types,
										  ParameterModifier[] modifiers)
			{
				ValidateTypes(types);
				return GetConstructorImpl(bindingAttr, binder,
										  callingConventions,
										  types, modifiers);
			}

	// Implementation of "GetConstructor" provided by subclasses.
	protected abstract ConstructorInfo
					GetConstructorImpl(BindingFlags bindingAttr,
								       Binder binder,
								       CallingConventions callingConventions,
								       Type[] types,
								       ParameterModifier[] modifiers);

	// Get all constructors for this type.
	public abstract ConstructorInfo[] GetConstructors(BindingFlags bindingAttr);
	public ConstructorInfo[] GetConstructors()
			{
				return GetConstructors(BindingFlags.Instance |
									   BindingFlags.Public);
			}

	// Get the default members within this type.
	public virtual MemberInfo[] GetDefaultMembers()
			{
				// TODO
				return null;
			}

	// Get the element type for this type.
	public abstract Type GetElementType();

	// Get an event from this type.
	public abstract EventInfo GetEvent(String name, BindingFlags bindingAttr);
	public EventInfo GetEvent(String name)
			{
				return GetEvent(name, BindingFlags.Public |
									  BindingFlags.Instance |
									  BindingFlags.Static);
			}

	// Get all events from this type.
	public abstract EventInfo[] GetEvents(BindingFlags bindingAttr);
	public EventInfo[] GetEvents()
			{
				return GetEvents(BindingFlags.Public |
								 BindingFlags.Instance |
								 BindingFlags.Static);
			}

	// Get a field from this type.
	public abstract FieldInfo GetField(String name, BindingFlags bindingAttr);
	public FieldInfo GetField(String name)
			{
				return GetField(name, BindingFlags.Public |
									  BindingFlags.Instance |
									  BindingFlags.Static);
			}

	// Get all fields from this type.
	public abstract FieldInfo[] GetFields(BindingFlags bindingAttr);
	public FieldInfo[] GetFields()
			{
				return GetFields(BindingFlags.Public |
								 BindingFlags.Instance |
								 BindingFlags.Static);
			}

	// Get the hash code for this type.
	public override int GetHashCode()
			{
				Type underlying = UnderlyingSystemType;
				if(underlying != this)
				{
					return underlying.GetHashCode();
				}
				else
				{
					return base.GetHashCode();
				}
			}

	// Get an interface that this type implements.
	public abstract Type GetInterface(String name, bool ignoreCase);
	public Type GetInterface(String name)
			{
				return GetInterface(name, false);
			}

	// Get all interfaces that this type implements.
	public abstract Type[] GetInterfaces();

	// Get a member from this type.
	public abstract MemberInfo GetMember(String name, BindingFlags bindingAttr);
	public MemberInfo GetMember(String name)
			{
				return GetMember(name, BindingFlags.Public |
									   BindingFlags.Instance |
									   BindingFlags.Static);
			}

	// Get all members from this type.
	public abstract MemberInfo[] GetMembers(BindingFlags bindingAttr);
	public MemberInfo[] GetMembers()
			{
				return GetMembers(BindingFlags.Public |
								  BindingFlags.Instance |
								  BindingFlags.Static);
			}

	// Get a method from this type.
	public MethodInfo GetMethod(String name, BindingFlags bindingAttr)
			{
				if(name == null)
				{
					throw new ArgumentNullException("name");
				}
				return GetMethodImpl(name, bindingAttr, null,
								     CallingConventions.Any, null, null);
			}
	public MethodInfo GetMethod(String name, BindingFlags bindingAttr,
								Binder binder, Type[] types,
								ParameterModifier[] modifiers)
			{
				if(name == null)
				{
					throw new ArgumentNullException("name");
				}
				ValidateTypes(types);
				return GetMethodImpl(name, bindingAttr, binder,
								     CallingConventions.Any,
								     types, modifiers);
			}
	public MethodInfo GetMethod(String name, Type[] types)
			{
				if(name == null)
				{
					throw new ArgumentNullException("name");
				}
				ValidateTypes(types);
				return GetMethodImpl(name, BindingFlags.Public |
									       BindingFlags.Instance |
									       BindingFlags.Static,
								     null, CallingConventions.Any,
								     types, null);
			}
	public MethodInfo GetMethod(String name, Type[] types,
								ParameterModifier[] modifiers)
			{
				if(name == null)
				{
					throw new ArgumentNullException("name");
				}
				ValidateTypes(types);
				return GetMethodImpl(name, BindingFlags.Public |
									       BindingFlags.Instance |
									       BindingFlags.Static,
								     null, CallingConventions.Any,
								     types, modifiers);
			}
	public MethodInfo GetMethod(String name)
			{
				if(name == null)
				{
					throw new ArgumentNullException("name");
				}
				return GetMethodImpl(name, BindingFlags.Public |
									       BindingFlags.Instance |
									       BindingFlags.Static,
								     null, CallingConventions.Any,
								     null, null);
			}
	public MethodInfo GetMethod(String name, BindingFlags bindingAttr,
								Binder binder,
								CallingConventions callingConventions,
								Type[] types,
								ParameterModifier[] modifiers)
			{
				if(name == null)
				{
					throw new ArgumentNullException("name");
				}
				ValidateTypes(types);
				return GetMethodImpl(name, bindingAttr, binder,
									 callingConventions, types, modifiers);
			}

	// Implementation of "GetMethod" provided by subclasses.
	protected abstract MethodInfo
					GetMethodImpl(String name,
								  BindingFlags bindingAttr, Binder binder,
								  CallingConventions callingConventions,
								  Type[] types,
								  ParameterModifier[] modifiers);

	// Get all methods from this type.
	public abstract MethodInfo[] GetMethods(BindingFlags bindingAttr);
	public MethodInfo[] GetMethods()
			{
				return GetMethods(BindingFlags.Public |
								  BindingFlags.Instance |
								  BindingFlags.Static);
			}

	// Get a nested type from this type.
	public abstract Type GetNestedType(String name, BindingFlags bindingAttr);
	public Type GetNestedType(String name)
			{
				return GetNestedType(name,
									 BindingFlags.Public |
								     BindingFlags.Instance |
								     BindingFlags.Static);
			}

	// Get all nested types from this type.
	public abstract Type[] GetNestedTypes(BindingFlags bindingAttr);
	public Type[] GetNestedTypes()
			{
				return GetNestedTypes(BindingFlags.Public |
								      BindingFlags.Instance |
								      BindingFlags.Static);
			}

	// Get a property from this type.
	public PropertyInfo GetProperty(String name)
			{
				if(name == null)
				{
					throw new ArgumentNullException("name");
				}
				return GetPropertyImpl(name,
									   BindingFlags.Public |
									   BindingFlags.Instance |
									   BindingFlags.Static,
									   null, null, null, null);
			}
	public PropertyInfo GetProperty(String name, Type returnType)
			{
				if(name == null)
				{
					throw new ArgumentNullException("name");
				}
				if(returnType == null)
				{
					throw new ArgumentNullException("returnType");
				}
				return GetPropertyImpl(name,
									   BindingFlags.Public |
									   BindingFlags.Instance |
									   BindingFlags.Static,
									   null, returnType, null, null);
			}
	public PropertyInfo GetProperty(String name, Type[] types)
			{
				if(name == null)
				{
					throw new ArgumentNullException("name");
				}
				ValidateTypes(types);
				return GetPropertyImpl(name,
									   BindingFlags.Public |
									   BindingFlags.Instance |
									   BindingFlags.Static,
									   null, null, types, null);
			}
	public PropertyInfo GetProperty(String name, Type returnType, Type[] types)
			{
				if(name == null)
				{
					throw new ArgumentNullException("name");
				}
				if(returnType == null)
				{
					throw new ArgumentNullException("returnType");
				}
				ValidateTypes(types);
				return GetPropertyImpl(name,
									   BindingFlags.Public |
									   BindingFlags.Instance |
									   BindingFlags.Static,
									   null, returnType, types, null);
			}
	public PropertyInfo GetProperty(String name, BindingFlags bindingAttr)
			{
				if(name == null)
				{
					throw new ArgumentNullException("name");
				}
				return GetPropertyImpl(name, bindingAttr,
									   null, null, null, null);
			}
	public PropertyInfo GetProperty(String name, Type returnType,
								    Type[] types,
									ParameterModifier[] modifiers)
			{
				if(name == null)
				{
					throw new ArgumentNullException("name");
				}
				if(returnType == null)
				{
					throw new ArgumentNullException("returnType");
				}
				ValidateTypes(types);
				return GetPropertyImpl(name,
									   BindingFlags.Public |
									   BindingFlags.Instance |
									   BindingFlags.Static,
									   null, returnType, types, modifiers);
			}
	public PropertyInfo GetProperty(String name,
								    BindingFlags bindingAttr, Binder binder,
								    Type returnType, Type[] types,
									ParameterModifier[] modifiers)
			{
				if(name == null)
				{
					throw new ArgumentNullException("name");
				}
				if(returnType == null)
				{
					throw new ArgumentNullException("returnType");
				}
				ValidateTypes(types);
				return GetPropertyImpl(name, bindingAttr,
									   binder, returnType,
									   types, modifiers);
			}

	// Implementation of "GetProperty" provided by subclasses.
	protected abstract PropertyInfo
					GetPropertyImpl(String name,
									BindingFlags bindingAttr, Binder binder,
								    Type returnType, Type[] types,
								    ParameterModifier[] modifiers);

	// Get all properties from this type.
	public abstract PropertyInfo[] GetProperties(BindingFlags bindingAttr);
	public PropertyInfo[] GetProperties()
			{
				return GetProperties(BindingFlags.Public |
								     BindingFlags.Instance |
								     BindingFlags.Static);
			}

	// Get a type by name.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static Type GetType(String name, bool throwOnError,
									  bool ignoreCase);
	public static Type GetType(String name, bool throwOnError)
			{
				return GetType(name, throwOnError, false);
			}
	public static Type GetType(String name)
			{
				return GetType(name, false, false);
			}

	// Get the types of the objects in a specified array.
	public static Type[] GetTypeArray(Object[] args)
			{
				int len, posn;
				Type[] types;
				if(args == null)
				{
					throw new ArgumentNullException("args");
				}
				len = args.Length;
				types = new Type[len];
				for(posn = 0; posn < len; ++posn)
				{
					types[posn] = args[posn].GetType();
				}
				return types;
			}

	// Get a type from a class identifier.
	//
	// This functionality is not supported, as it is
	// intended for implementing COM, which we don't have.
	public static Type GetTypeFromCLSID(Guid clsid, String server,
										bool throwOnError)
			{
				if(throwOnError)
				{
					throw new TypeLoadException();
				}
				return null;
			}
	public static Type GetTypeFromCLSID(Guid clsid, bool throwOnError)
			{
				return GetTypeFromCLSID(clsid, null, throwOnError);
			}
	public static Type GetTypeFromCLSID(Guid clsid, string server)
			{
				return GetTypeFromCLSID(clsid, server, false);
			}

	// Get a type from a program identifier.
	//
	// This functionality is not supported, as it is
	// intended for implementing COM, which we don't have.
	public static Type GetTypeFromProgID(String progID, String server,
										 bool throwOnError)
			{
				if(throwOnError)
				{
					throw new TypeLoadException();
				}
				return null;
			}
	public static Type GetTypeFromProgID(String progID, bool throwOnError)
			{
				return GetTypeFromProgID(progID, null, throwOnError);
			}
	public static Type GetTypeFromProgID(String progID, string server)
			{
				return GetTypeFromProgID(progID, server, false);
			}

	// Get the runtime type handle associated with an object.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static RuntimeTypeHandle GetTypeHandle(Object obj);

	// Get a type from a runtime type handle.
	[MethodImpl(MethodImplOptions.InternalCall)]
	extern public static Type GetTypeFromHandle(RuntimeTypeHandle handle);

	// Implementation of the "HasElementType" property.
	protected abstract bool HasElementTypeImpl();

	// Invoke a member.
	public abstract Object InvokeMember
						(String name, BindingFlags invokeAttr,
					     Binder binder, Object target, Object[] args,
					     ParameterModifier[] modifiers,
					     CultureInfo culture, String[] namedParameters);
	public Object InvokeMember(String name, BindingFlags invokeAttr,
					     	   Binder binder, Object target, Object[] args)
			{
				return InvokeMember(name, invokeAttr, binder, target, args,
									null, null, null);
			}
	public Object InvokeMember(String name, BindingFlags invokeAttr,
					     	   Binder binder, Object target, Object[] args,
							   CultureInfo culture)
			{
				return InvokeMember(name, invokeAttr, binder, target, args,
									null, culture, null);
			}

	// Implementation of the "IsArray" property.
	protected abstract bool IsArrayImpl();

	// Determine if this type is assignable from another type.
	public virtual bool IsAssignableFrom(Type c)
			{
				// TODO
				return false;
			}

	// Implementation of the "IsByRef" property.
	protected abstract bool IsByRefImpl();

	// Implementation of the "IsCOMObject" property.
	protected abstract bool IsCOMObjectImpl();

	// Implementation of the "IsContextful" property.
	protected virtual bool IsContextfulImpl()
			{
				return (typeof(ContextBoundObject)).IsAssignableFrom(this);
			}

	// Determine if an object is an instance of this type.
	public virtual bool IsInstanceOfType(Object obj)
			{
				// TODO
				return false;
			}

	// Implementation of the "IsMarshalByRef" property.
	protected virtual bool IsMarshalByRefImpl()
			{
				return (typeof(MarshalByRefObject)).IsAssignableFrom(this);
			}

	// Implementation of the "IsPointer" property.
	protected abstract bool IsPointerImpl();

	// Implementation of the "IsPrimitive" property.
	protected abstract bool IsPrimitiveImpl();

	// Determine if the current type is a subclass of another type.
	public virtual bool IsSubclassOf(Type c)
			{
				Type current = this;
				if(c == current)
				{
					return false;
				}
				while(current != null)
				{
					if(current == c)
					{
						return true;
					}
					current = current.BaseType;
				}
				return false;
			}

	// Implementation of the "IsValueType" property.
	protected virtual bool IsValueTypeImpl()
			{
				if(this == valueType || this == enumType)
				{
					return false;
				}
				else
				{
					return IsSubclassOf(valueType);
				}
			}

	// Convert this type into a string.
	public override String ToString()
			{
				return "Type: " + Name;
			}

	// Abstract properties.
	public abstract System.Reflection.Assembly Assembly { get; }
	public abstract String AssemblyQualifiedName { get; }
	public abstract Type BaseType { get; }
	public abstract String FullName { get; }
	public abstract Guid GUID { get; }
	public abstract System.Reflection.Module Module { get; }
	public abstract String Namespace { get; }
	public abstract Type UnderlyingSystemType { get; }
	public abstract RuntimeTypeHandle TypeHandle { get; }

	// Implemented properties.
	public override Type DeclaringType { get { return this; } }
	public bool HasElementType { get { return HasElementTypeImpl(); } }
	public bool IsArray { get { return IsArrayImpl(); } }
	public bool IsByRef { get { return IsByRefImpl(); } }
	public bool IsCOMObject { get { return IsCOMObjectImpl(); } }
	public bool IsContextful { get { return IsContextfulImpl(); } }
	public bool IsMarshalByRef { get { return IsMarshalByRefImpl(); } }
	public bool IsPointer { get { return IsPointerImpl(); } }
	public bool IsPrimitive { get { return IsPrimitiveImpl(); } }
	public bool IsValueType { get { return IsValueTypeImpl(); } }
	public override Type ReflectedType { get { return this; } }

	// Test for various type attributes.
	public bool IsAbstract
			{
				get
				{
					return ((GetAttributeFlagsImpl() &
							 TypeAttributes.Abstract) != 0);
				}
			}
	public bool IsAnsiClass
			{
				get
				{
					return ((GetAttributeFlagsImpl() &
							 TypeAttributes.StringFormatMask) ==
							 	TypeAttributes.AnsiClass);
				}
			}
	public bool IsAutoClass
			{
				get
				{
					return ((GetAttributeFlagsImpl() &
							 TypeAttributes.StringFormatMask) ==
							 	TypeAttributes.AutoClass);
				}
			}
	public bool IsAutoLayout
			{
				get
				{
					return ((GetAttributeFlagsImpl() &
							 TypeAttributes.LayoutMask) ==
							 	TypeAttributes.AutoLayout);
				}
			}
	public bool IsClass
			{
				get
				{
					return ((GetAttributeFlagsImpl() &
							 TypeAttributes.ClassSemanticsMask) ==
							 	TypeAttributes.Class) &&
						   !IsSubclassOf(valueType);
				}
			}
	public bool IsEnum
			{
				get
				{
					return IsSubclassOf(enumType);
				}
			}
	public bool IsExplicitLayout
			{
				get
				{
					return ((GetAttributeFlagsImpl() &
							 TypeAttributes.LayoutMask) ==
							 	TypeAttributes.ExplicitLayout);
				}
			}
	public bool IsImport
			{
				get
				{
					return ((GetAttributeFlagsImpl() &
							 TypeAttributes.Import) != 0);
				}
			}
	public bool IsInterface
			{
				get
				{
					return ((GetAttributeFlagsImpl() &
							 TypeAttributes.ClassSemanticsMask) ==
							 	TypeAttributes.Interface);
				}
			}
	public bool IsLayoutSequential
			{
				get
				{
					return ((GetAttributeFlagsImpl() &
							 TypeAttributes.LayoutMask) ==
							 	TypeAttributes.LayoutSequential);
				}
			}
	public bool IsNestedAssembly
			{
				get
				{
					return ((GetAttributeFlagsImpl() &
							 TypeAttributes.VisibilityMask) ==
							 	TypeAttributes.NestedAssembly);
				}
			}
	public bool IsNestedFamANDAssem
			{
				get
				{
					return ((GetAttributeFlagsImpl() &
							 TypeAttributes.VisibilityMask) ==
							 	TypeAttributes.NestedFamANDAssem);
				}
			}
	public bool IsNestedFamORAssem
			{
				get
				{
					return ((GetAttributeFlagsImpl() &
							 TypeAttributes.VisibilityMask) ==
							 	TypeAttributes.NestedFamORAssem);
				}
			}
	public bool IsNestedFamily
			{
				get
				{
					return ((GetAttributeFlagsImpl() &
							 TypeAttributes.VisibilityMask) ==
							 	TypeAttributes.NestedFamily);
				}
			}
	public bool IsNestedPrivate
			{
				get
				{
					return ((GetAttributeFlagsImpl() &
							 TypeAttributes.VisibilityMask) ==
							 	TypeAttributes.NestedPrivate);
				}
			}
	public bool IsNestedPublic
			{
				get
				{
					return ((GetAttributeFlagsImpl() &
							 TypeAttributes.VisibilityMask) ==
							 	TypeAttributes.NestedPublic);
				}
			}
	public bool IsNotPublic
			{
				get
				{
					return ((GetAttributeFlagsImpl() &
							 TypeAttributes.VisibilityMask) ==
							 	TypeAttributes.NotPublic);
				}
			}
	public bool IsPublic
			{
				get
				{
					return ((GetAttributeFlagsImpl() &
							 TypeAttributes.VisibilityMask) ==
							 	TypeAttributes.Public);
				}
			}
	public bool IsSealed
			{
				get
				{
					return ((GetAttributeFlagsImpl() &
							 TypeAttributes.Sealed) != 0);
				}
			}
	public bool IsSerializable
			{
				get
				{
					return ((GetAttributeFlagsImpl() &
							 TypeAttributes.Serializable) != 0);
				}
			}
	public bool IsSpecialName
			{
				get
				{
					return ((GetAttributeFlagsImpl() &
							 TypeAttributes.SpecialName) != 0);
				}
			}
	public bool IsUnicodeClass
			{
				get
				{
					return ((GetAttributeFlagsImpl() &
							 TypeAttributes.StringFormatMask) ==
							 	TypeAttributes.UnicodeClass);
				}
			}

	// Cached copy of the default binder.
	private static Binder defaultBinder;

	// Get the default binder in use by the system.
	public Binder DefaultBinder
			{
				get
				{
					if(defaultBinder == null)
					{
						lock(typeof(Type))
						{
							if(defaultBinder != null)
							{
								defaultBinder = new DefaultBinder();
							}
						}
					}
					return defaultBinder;
				}
			}

	// Get the type initializer for this type.
	public ConstructorInfo TypeInitializer
			{
				get
				{
					return GetConstructorImpl(BindingFlags.Public |
											  BindingFlags.NonPublic |
											  BindingFlags.Static,
											  null,
											  CallingConventions.Any,
											  EmptyTypes,
											  null);
				}
			}

}; // class Type

}; // namespace System
