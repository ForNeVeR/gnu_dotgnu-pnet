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
				Type type1;
				Type type2;

				// The value must not be null.
				if(value == null)
				{
					return false;
				}

				// The types must be equal.
				type1 = GetType();
				type2 = value.GetType();
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

	// Get a single attribute associated with a particular program item.
	private static Attribute GetAttr(ICustomAttributeProvider element,
									 Type attributeType, bool inherit)
			{
				Object[] attrs;
				if(element == null)
				{
					throw new ArgumentNullException("element");
				}
				if(attributeType == null)
				{
					throw new ArgumentNullException("attributeType");
				}
				attrs = element.GetCustomAttributes(attributeType, inherit);
				if(attrs == null)
				{
					return null;
				}
				else if(attrs.Length == 1)
				{
					return (Attribute)(attrs[0]);
				}
				else
				{
					throw new AmbiguousMatchException
						(_("Reflection_AmbiguousAttr"));
				}
			}

	// Get all attributes associated with a particular program item.
	private static Attribute[] GetAllAttrs(ICustomAttributeProvider element,
									 	   bool inherit)
			{
				Object[] attrs;
				Attribute[] newAttrs;
				if(element == null)
				{
					throw new ArgumentNullException("element");
				}
				attrs = element.GetCustomAttributes(inherit);
				if(attrs == null)
				{
					return null;
				}
				else
				{
					newAttrs = new Attribute [attrs.Length];
					int posn;
					for(posn = 0; posn < attrs.Length; ++posn)
					{
						newAttrs[posn] = (Attribute)(attrs[posn]);
					}
					return newAttrs;
				}
			}

	// Get all attributes associated with a particular program item,
	// that are defined to be of a particular type.
	private static Attribute[] GetAttrs(ICustomAttributeProvider element,
								 	    Type attributeType, bool inherit)
			{
				Object[] attrs;
				Attribute[] newAttrs;
				if(element == null)
				{
					throw new ArgumentNullException("element");
				}
				if(attributeType == null)
				{
					throw new ArgumentNullException("attributeType");
				}
				attrs = element.GetCustomAttributes(attributeType, inherit);
				if(attrs == null)
				{
					return null;
				}
				else
				{
					newAttrs = new Attribute [attrs.Length];
					int posn;
					for(posn = 0; posn < attrs.Length; ++posn)
					{
						newAttrs[posn] = (Attribute)(attrs[posn]);
					}
					return newAttrs;
				}
			}

	// Determine if an attribute is defined on a particular program item.
	private static bool IsAttrDefined(ICustomAttributeProvider element,
									  Type attributeType, bool inherit)
			{
				if(element == null)
				{
					throw new ArgumentNullException("element");
				}
				if(attributeType == null)
				{
					throw new ArgumentNullException("attributeType");
				}
				return element.IsDefined(attributeType, inherit);
			}

	// Get an attribute that is associated with a particular program item.
	public static Attribute GetCustomAttribute(Module element,
											   Type attributeType)
			{
				return GetAttr(element, attributeType, true);
			}
	public static Attribute GetCustomAttribute(ParameterInfo element,
											   Type attributeType)
			{
				return GetAttr(element, attributeType, true);
			}
	public static Attribute GetCustomAttribute(MemberInfo element,
											   Type attributeType)
			{
				return GetAttr(element, attributeType, true);
			}
	public static Attribute GetCustomAttribute(Assembly element,
											   Type attributeType)
			{
				return GetAttr(element, attributeType, true);
			}

#if !ECMA_COMPAT

	// Get an attribute that is associated with a particular program item.
	public static Attribute GetCustomAttribute(Module element,
											   Type attributeType,
											   bool inherit)
			{
				return GetAttr(element, attributeType, inherit);
			}
	public static Attribute GetCustomAttribute(ParameterInfo element,
											   Type attributeType,
											   bool inherit)
			{
				return GetAttr(element, attributeType, inherit);
			}
	public static Attribute GetCustomAttribute(MemberInfo element,
											   Type attributeType,
											   bool inherit)
			{
				return GetAttr(element, attributeType, inherit);
			}
	public static Attribute GetCustomAttribute(Assembly element,
											   Type attributeType,
											   bool inherit)
			{
				return GetAttr(element, attributeType, inherit);
			}

#endif // !ECMA_COMPAT

	// Get all attributes that are associated with a particular program item.
	public static Attribute[] GetCustomAttributes(Assembly element)
			{
				return GetAllAttrs(element, true);
			}
	public static Attribute[] GetCustomAttributes(Module element)
			{
				return GetAllAttrs(element, true);
			}
	public static Attribute[] GetCustomAttributes(ParameterInfo element)
			{
				return GetAllAttrs(element, true);
			}
	public static Attribute[] GetCustomAttributes(MemberInfo element)
			{
				return GetAllAttrs(element, true);
			}

#if !ECMA_COMPAT

	// Get all attributes that are associated with a particular program item.
	public static Attribute[] GetCustomAttributes(Assembly element,
												  bool inherit)
			{
				return GetAllAttrs(element, inherit);
			}
	public static Attribute[] GetCustomAttributes(Module element,
												  bool inherit)
			{
				return GetAllAttrs(element, inherit);
			}
	public static Attribute[] GetCustomAttributes(ParameterInfo element,
												  bool inherit)
			{
				return GetAllAttrs(element, inherit);
			}
	public static Attribute[] GetCustomAttributes(MemberInfo element,
												  bool inherit)
			{
				return GetAllAttrs(element, inherit);
			}

#endif // !ECMA_COMPAT

	// Get all attributes that are associated with a particular program item,
	// and which have a specific type.
	public static Attribute[] GetCustomAttributes
						(ParameterInfo element, Type attributeType)
			{
				return GetAttrs(element, attributeType, true);
			}
	public static Attribute[] GetCustomAttributes
						(MemberInfo element, Type attributeType)
			{
				return GetAttrs(element, attributeType, true);
			}
	public static Attribute[] GetCustomAttributes
						(Module element, Type attributeType)
			{
				return GetAttrs(element, attributeType, true);
			}
	public static Attribute[] GetCustomAttributes
						(Assembly element, Type attributeType)
			{
				return GetAttrs(element, attributeType, true);
			}

#if !ECMA_COMPAT

	// Get all attributes that are associated with a particular program item,
	// and which have a specific type.
	public static Attribute[] GetCustomAttributes
					(ParameterInfo element, Type attributeType, bool inherit)
			{
				return GetAttrs(element, attributeType, inherit);
			}
	public static Attribute[] GetCustomAttributes
						(MemberInfo element, Type attributeType, bool inherit)
			{
				return GetAttrs(element, attributeType, inherit);
			}
	public static Attribute[] GetCustomAttributes
						(Module element, Type attributeType, bool inherit)
			{
				return GetAttrs(element, attributeType, inherit);
			}
	public static Attribute[] GetCustomAttributes
						(Assembly element, Type attributeType, bool inherit)
			{
				return GetAttrs(element, attributeType, inherit);
			}

#endif // !ECMA_COMPAT

	// Determine if an attribute is defined on a particular program item.
	public static bool IsDefined(MemberInfo element, Type attributeType)
			{
				return IsAttrDefined(element, attributeType, true);
			}
	public static bool IsDefined(Module element, Type attributeType)
			{
				return IsAttrDefined(element, attributeType, true);
			}
	public static bool IsDefined(Assembly element, Type attributeType)
			{
				return IsAttrDefined(element, attributeType, true);
			}
	public static bool IsDefined(ParameterInfo element, Type attributeType)
			{
				return IsAttrDefined(element, attributeType, true);
			}

#if !ECMA_COMPAT

	// Determine if an attribute is defined on a particular program item.
	public static bool IsDefined(MemberInfo element, Type attributeType,
								 bool inherit)
			{
				return IsAttrDefined(element, attributeType, inherit);
			}
	public static bool IsDefined(Module element, Type attributeType,
								 bool inherit)
			{
				return IsAttrDefined(element, attributeType, inherit);
			}
	public static bool IsDefined(Assembly element, Type attributeType,
								 bool inherit)
			{
				return IsAttrDefined(element, attributeType, inherit);
			}
	public static bool IsDefined(ParameterInfo element, Type attributeType,
								 bool inherit)
			{
				return IsAttrDefined(element, attributeType, inherit);
			}

#endif // !ECMA_COMPAT

#if !ECMA_COMPAT

	// Get a type identifier for this attribute's type.
	public virtual Object TypeId
			{
				get
				{
					return GetType();
				}
			}

	// Determine if this attribute is a default value.
	public virtual bool IsDefaultAttribute()
			{
				return false;
			}

	// Match this attribute against another.
	public virtual bool Match(Object obj)
			{
				return Equals(obj);
			}

#endif // !ECMA_COMPAT

}; // class Attribute

}; // namespace System
