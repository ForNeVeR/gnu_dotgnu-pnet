/*
 * MemberDescriptor.cs - Implementation of the
 *		"System.ComponentModel.ComponentModel.MemberDescriptor" class.
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

namespace System.ComponentModel
{

#if CONFIG_COMPONENT_MODEL

using System.Collections;
using System.Reflection;
using System.Runtime.InteropServices;

[ComVisible(true)]
public abstract class MemberDescriptor
{
	// Internal state.
	private AttributeCollection attributeCollection;
	private Attribute[] attributes;
	private String category;
	private String description;
	private bool designTimeOnly;
	private bool isBrowsable;
	private String displayName;
	private String name;

	// Constructors.
	protected MemberDescriptor(MemberDescriptor descr)
			: this(descr, null) {}
	protected MemberDescriptor(String name)
			: this(name, null) {}
	[TODO]
	protected MemberDescriptor
				(MemberDescriptor descr, Attribute[] newAttributes)
			{
				// TODO
				name = descr.Name;
			}
	[TODO]
	protected MemberDescriptor(String name, Attribute[] newAttributes)
			{
				// TODO
				this.name = name;
			}

	// Properties.
	public virtual AttributeCollection Attributes
			{
				get
				{
					if(attributeCollection == null)
					{
						attributeCollection = CreateAttributeCollection();
					}
					return attributeCollection;
				}
			}
	[TODO]
	public virtual String Category
			{
				get
				{
					// TODO
					return category;
				}
			}
	[TODO]
	public virtual String Description
			{
				get
				{
					// TODO
					return description;
				}
			}
	[TODO]
	public virtual bool DesignTimeOnly
			{
				get
				{
					// TODO
					return designTimeOnly;
				}
			}
	[TODO]
	public virtual String DisplayName
			{
				get
				{
					// TODO
					return displayName;
				}
			}
	[TODO]
	public virtual bool IsBrowsable
			{
				get
				{
					// TODO
					return isBrowsable;
				}
			}
	public virtual String Name
			{
				get
				{
					if(name != null)
					{
						return name;
					}
					else
					{
						return "";
					}
				}
			}

	// Determine if this member descriptor is equal to another.
	[TODO]
	public override bool Equals(Object obj)
			{
				MemberDescriptor other = (obj as MemberDescriptor);
				if(other != null)
				{
					// TODO
					return false;
				}
				else
				{
					return false;
				}
			}

	// Get a hash code for this object.
	public override int GetHashCode()
			{
				return NameHashCode;
			}

	// Get or set the entire attribute array.
	[TODO]
	protected virtual Attribute[] AttributeArray
			{
				get
				{
					return attributes;
				}
				set
				{
					attributeCollection = null;
					attributes = value;
				}
			}

	// Get the hash code for the member name.
	protected virtual int NameHashCode
			{
				get
				{
					if(name != null)
					{
						return name.GetHashCode();
					}
					else
					{
						return 0;
					}
				}
			}

	// Create the attribute collection.
	protected virtual AttributeCollection CreateAttributeCollection()
			{
				return new AttributeCollection(AttributeArray);
			}

	// Fill a list with all attributes.
	protected virtual void FillAttributes(IList attributeList)
			{
				if(attributes != null)
				{
					foreach(Object obj in attributes)
					{
						attributeList.Add(obj);
					}
				}
			}

	// Find a method by reflection in a component class.
	protected static MethodInfo FindMethod
				(Type componentClass, String name,
				 Type[] args, Type returnType)
			{
				return FindMethod(componentClass, name, args, returnType, true);
			}
	protected static MethodInfo FindMethod
				(Type componentClass, String name,
				 Type[] args, Type returnType, bool publicOnly)
			{
				MethodInfo method;
				method = componentClass.GetMethod
					(name, (publicOnly ? BindingFlags.Public |
						   				 BindingFlags.Static |
										 BindingFlags.Instance
									   : BindingFlags.Public |
									     BindingFlags.NonPublic |
										 BindingFlags.Static |
										 BindingFlags.Instance),
					 null, args, null);
				if(method != null && method.ReturnType == returnType)
				{
					return method;
				}
				else
				{
					return null;
				}
			}

	// Get the object to be invoked.
	[TODO]
	protected static Object GetInvokee(Type componentClass, Object component)
			{
				// TODO
				return component;
			}

	// Get a component site for an object.
	[TODO]
	protected static ISite GetSite(Object component)
			{
				// TODO
				return null;
			}

}; // class MemberDescriptor

#endif // CONFIG_COMPONENT_MODEL

}; // namespace System.ComponentModel
