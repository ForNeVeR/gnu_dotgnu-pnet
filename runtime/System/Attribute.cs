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

public abstract class Attribute
{

	// Constructor.
	protected Attribute()
			{
				// Nothing to do here.
			}

	// Determine if two attributes are equal.
	public override bool Equals(Object value)
			{
				RuntimeType type1;
				RuntimeType type2;

				// The value must not be null.
				if(value == null)
				{
					return false;
				}

				// The types must be equal.
				type1 = (RuntimeType)(GetType());
				type2 = (RuntimeType)(value.GetType());
				if(type1 != type2)
				{
					return false;
				}

				// TODO: compare the field values for equality.
				return false;
			}

	// Get the hash value for this instance.
	public override int GetHashCode()
			{
				// TODO.
				return 0;
			}

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
