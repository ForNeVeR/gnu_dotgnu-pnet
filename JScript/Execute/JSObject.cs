/*
 * JSObject.cs - Common base for JScript objects.
 *
 * Copyright (C) 2003 Southern Storm Software, Pty Ltd.
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
 
namespace Microsoft.JScript
{

using System;
using System.Reflection;
using System.Collections;
using System.Globalization;
using System.Runtime.InteropServices.Expando;
using Microsoft.JScript.Vsa;

public class JSObject : ScriptObject, IEnumerable
#if !ECMA_COMPAT
	, IExpando
#endif
{
	// Internal state.
	private Property properties;
	private Hashtable overflow;

	// Constructors.
	public JSObject() : this(null) {}
	internal JSObject(ScriptObject parent)
			: base(parent)
			{
				properties = null;
				overflow = null;
			}
	internal JSObject(ScriptObject parent, VsaEngine engine)
			: this(parent)
			{
				this.engine = engine;
			}

	// Implement the IEnumerable interface.
	IEnumerator IEnumerable.GetEnumerator()
			{
				return ForIn.JScriptGetEnumerator(this);
			}

	// Get member information for this object.
	public override MemberInfo GetMember(String name, BindingFlags bindingAttr)
			{
				// TODO
				return null;
			}
	public override MemberInfo[] GetMembers(BindingFlags bindingAttr)
			{
				// TODO
				return null;
			}

	// Implement the IExpando interface.
	public FieldInfo AddField(String name)
			{
				// TODO
				return null;
			}
#if !ECMA_COMPAT
	MethodInfo IExpando.AddMethod(String name, Delegate method)
			{
				// Not used by JScript.
				return null;
			}
	PropertyInfo IExpando.AddProperty(String name)
			{
				// Not used by JScript.
				return null;
			}
	void IExpando.RemoveMember(MemberInfo m)
			{
				// TODO
			}
#endif

	// Set the value of a field member.
	public void SetMemberValue2(String name, Object value)
			{
				// TODO
			}

	// Convert this object into a string.
	public override String ToString()
			{
				// Find the "toString" method on the object or its prototype.
				JSObject temp = this;
				ScriptFunction toStr;
				do
				{
					toStr = (temp.GetProperty("toString") as ScriptFunction);
					if(toStr != null)
					{
						// Invoke the "toString" method.
						return (toStr.Invoke(this)).ToString();
					}
					temp = (temp.GetParent() as JSObject);
				}
				while(temp != null);
				return String.Empty;
			}

	// Get the internal "[[Class]]" property for this object.
	internal virtual String ClassName
			{
				get
				{
					return "Object";
				}
			}

	// Get a property from this object.  Null if not present.
	internal override Object GetProperty(String name)
			{
				Property prop;

				// Check the overflow hash table first, if it exists.
				if(overflow != null)
				{
					prop = (Property)(overflow[name]);
					if(prop != null)
					{
						return prop.value;
					}
					else
					{
						return null;
					}
				}

				// Scan the property list.
				prop = properties;
				while(prop != null)
				{
					if(prop.name == name)
					{
						return prop.value;
					}
					prop = prop.next;
				}

				// Could not find the property.
				return null;
			}

	// Determine if a property is enumerable.
	internal bool IsEnumerable(String name)
			{
				Property prop;

				// Check the overflow hash table first, if it exists.
				if(overflow != null)
				{
					prop = (Property)(overflow[name]);
					if(prop != null)
					{
						return (prop.attrs & PropertyAttributes.DontEnum) == 0;
					}
					else
					{
						return false;
					}
				}

				// Scan the property list.
				prop = properties;
				while(prop != null)
				{
					if(prop.name == name)
					{
						return (prop.attrs & PropertyAttributes.DontEnum) == 0;
					}
					prop = prop.next;
				}

				// Could not find the property.
				return false;
			}

	// Determine if this object has a specific property.
	internal override bool HasOwnProperty(String name)
			{
				Property prop;

				// Check the overflow hash table first, if it exists.
				if(overflow != null)
				{
					prop = (Property)(overflow[name]);
					return (prop != null);
				}

				// Scan the property list.
				prop = properties;
				while(prop != null)
				{
					if(prop.name == name)
					{
						return true;
					}
					prop = prop.next;
				}

				// Could not find the property.
				return false;
			}

	// Set a property in this object.
	internal override void SetProperty(String name, Object value,
							  		   PropertyAttributes attrs)
			{
				Property prop, prev;
				int num;

				// Normalize null values.
				if(value == null)
				{
					value = DBNull.Value;
				}

				// Check the overflow hash table first, if it exists.
				if(overflow != null)
				{
					prop = (Property)(overflow[name]);
					if(prop == null)
					{
						overflow[name] = new Property(name, value, attrs);
					}
					else if((prop.attrs & PropertyAttributes.ReadOnly) == 0)
					{
						prop.value = value;
					}
					return;
				}

				// Scan for a property with the given name.
				prop = properties;
				prev = null;
				num = 0;
				while(prop != null)
				{
					if(prop.name == name)
					{
						if((prop.attrs & PropertyAttributes.ReadOnly) == 0)
						{
							prop.value = value;
						}
						return;
					}
					prev = prop;
					prop = prop.next;
					++num;
				}

				// Add a new property to the list.
				if(num < 8)
				{
					prop = new Property(name, value, attrs);
					if(prev != null)
					{
						prev.next = prop;
					}
					else
					{
						properties = prop;
					}
					return;
				}

				// The list is too big, so switch to a hash table.
				overflow = new Hashtable(32);
				prop = properties;
				while(prop != null)
				{
					overflow[prop.name] = prop;
					prev = prop;
					prop = prop.next;
					prev.next = null;
				}
				properties = null;
				overflow[name] = new Property(name, value, attrs);
			}

	// Add a builtin method to a prototype.
	internal void AddBuiltin(EngineInstance inst, String name)
			{
				MethodInfo method = GetType().GetMethod(name);
				SetProperty(name, new BuiltinFunction
					(inst.GetFunctionPrototype(), name, method),
					PropertyAttributes.None);
			}

	// Storage for a property.
	private class Property
	{
		// Accessible internal state.
		public Property next;
		public String name;
		public Object value;
		public PropertyAttributes attrs;

		// Constructor.
		public Property(String name, Object value, PropertyAttributes attrs)
				{
					this.next = null;
					this.name = name;
					this.value = value;
					this.attrs = attrs;
				}

	}; // class Property

}; // class JSObject

}; // namespace Microsoft.JScript
