/*
 * IReflect.cs - Implementation of the "System.Reflection.IReflect" interface.
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

public interface IReflect
{

	FieldInfo GetField(String name, BindingFlags bindingAttr);
	FieldInfo[] GetFields(BindingFlags bindingAttr);
	MemberInfo GetMember(String name, BindingFlags bindingAttr);
	MemberInfo[] GetMembers(BindingFlags bindingAttr);
	MethodInfo GetMethod(String name, BindingFlags bindingAttr);
	MethodInfo GetMethod(String name, BindingFlags bindingAttr,
						 Binder binder, Type[] types,
						 ParameterModifier[] modifiers);
	MethodInfo[] GetMethods(BindingFlags bindingAttr);
	PropertyInfo GetProperty(String name, BindingFlags bindingAttr);
	PropertyInfo GetProperty(String name, BindingFlags bindingAttr,
							 Binder binder, Type returnType,
							 Type[] types, ParameterModifier[] modifiers);
	PropertyInfo[] GetProperties(BindingFlags bindingAttr);
	Object InvokeMember(String name, BindingFlags invokeAttr,
						Binder binder, Object target, Object[] args,
						ParameterModifier[] modifiers,
						CultureInfo culture, String[] namedParameters);
	Type UnderlyingSystemType { get; }

}; // interface IReflect

}; // namespace System.Reflection
