/*
 * Attribute.cs - Implementation of the "System.Attribute" class.
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

public class Attribute
{

	// Get an attribute that is associated with a particular program item.
	public static Attribute GetCustomAttribute(Module module, Type type)
			{ return null; }
	public static Attribute GetCustomAttribute(ParameterInfo param, Type type)
			{ return null; }
	public static Attribute GetCustomAttribute(MemberInfo member, Type type)
			{ return null; }
	public static Attribute GetCustomAttribute(Assembly assembly, Type type)
			{ return null; }
	public static Attribute[] GetCustomAttributes(Assembly assembly)
			{ return null; }
	public static Attribute[] GetCustomAttributes(Module module)
			{ return null; }
	public static Attribute[] GetCustomAttributes(ParameterInfo param)
			{ return null; }
	public static Attribute[] GetCustomAttributes(MemberInfo member)
			{ return null; }
	public static Attribute[] GetCustomAttributes
									(ParameterInfo param, Type type)
			{ return null; }
	public static Attribute[] GetCustomAttributes
									(MemberInfo member, Type type)
			{ return null; }
	public static Attribute[] GetCustomAttributes
									(Module module, Type type)
			{ return null; }
	public static Attribute[] GetCustomAttributes
									(Assembly assembly, Type type)
			{ return null; }

	// Determine if an attribute is defined on a particular program item.
	public static bool IsDefined(MemberInfo member, Type type)
			{ return false; }
	public static bool IsDefined(Module module, Type type)
			{ return false; }
	public static bool IsDefined(Assembly assembly, Type type)
			{ return false; }
	public static bool IsDefined(ParameterInfo param, Type type)
			{ return false; }

}; // class Attribute

}; // namespace System
