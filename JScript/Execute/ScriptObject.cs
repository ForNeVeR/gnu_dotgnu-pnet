/*
 * ScriptObject.cs - Root of the "script object" hierarchy.
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
using System.Globalization;
using Microsoft.JScript.Vsa;

public abstract class ScriptObject : IReflect
{
	// Accessible internal state.
	protected ScriptObject parent;
	public VsaEngine engine;

	// Constructor.
	internal ScriptObject(ScriptObject parent)
			{
				this.parent = parent;
				if(parent != null)
				{
					this.engine = parent.engine;
				}
				else
				{
					this.engine = null;
				}
			}

	// Strip a list of members down to just those of a specific type.
	private Array StripMemberList(BindingFlags bindingAttr,
								  MemberTypes memberType,
								  Type arrayElementType)
			{
				// Get the members.
				MemberInfo[] members = GetMembers(bindingAttr);
				if(members == null)
				{
					return Array.CreateInstance(arrayElementType, 0);
				}

				// Count the number of members of the requested type.
				int posn;
				int count = 0;
				for(posn = 0; posn < members.Length; ++posn)
				{
					if(members[posn].MemberType == memberType)
					{
						++count;
					}
				}

				// Create the final member array.
				Array array = Array.CreateInstance(arrayElementType, count);

				// Populate the array and return it.
				count = 0;
				for(posn = 0; posn < members.Length; ++posn)
				{
					if(members[posn].MemberType == memberType)
					{
						array.SetValue(members[posn], count++);
					}
				}
				return array;
			}

	// Implement the IReflect interface.
	public FieldInfo GetField(String name, BindingFlags bindingAttr)
			{
				MemberInfo[] members = GetMember(name, bindingAttr);
				foreach(MemberInfo member in members)
				{
					if(member.MemberType == MemberTypes.Field)
					{
						return (FieldInfo)member;
					}
				}
				return null;
			}
	public virtual FieldInfo[] GetFields(BindingFlags bindingAttr)
			{
				return (FieldInfo[])StripMemberList(bindingAttr,
									   				MemberTypes.Field,
									   				typeof(FieldInfo));
			}
	public abstract MemberInfo[] GetMember
				(String name, BindingFlags bindingAttr);
	public abstract MemberInfo[] GetMembers(BindingFlags bindingAttr);
	public MethodInfo GetMethod(String name, BindingFlags bindingAttr)
			{
				return GetMethod(name, bindingAttr, null,
								 Type.EmptyTypes, null);
			}
	public MethodInfo GetMethod(String name, BindingFlags bindingAttr,
					 			Binder binder, Type[] types,
					 			ParameterModifier[] modifiers)
			{
				// TODO
				return null;
			}
	public virtual MethodInfo[] GetMethods(BindingFlags bindingAttr)
			{
				return (MethodInfo[])StripMemberList(bindingAttr,
									   				 MemberTypes.Method,
									   				 typeof(MethodInfo));
			}
	public PropertyInfo GetProperty(String name, BindingFlags bindingAttr)
			{
				return GetProperty(name, bindingAttr, null,
								   null, Type.EmptyTypes, null);
			}
	public PropertyInfo GetProperty(String name, BindingFlags bindingAttr,
							 		Binder binder, Type returnType,
							 		Type[] types, ParameterModifier[] modifiers)
			{
				// TODO
				return null;
			}
	public virtual PropertyInfo[] GetProperties(BindingFlags bindingAttr)
			{
				return (PropertyInfo[])StripMemberList(bindingAttr,
									   				   MemberTypes.Property,
									   				   typeof(PropertyInfo));
			}
	public virtual Object InvokeMember
					   (String name, BindingFlags invokeAttr,
						Binder binder, Object target, Object[] args,
						ParameterModifier[] modifiers,
						CultureInfo culture, String[] namedParameters)
			{
				// Can only invoke on ourselves.
				if(target != this)
				{
					throw new TargetException();
				}

				// TODO
				return null;
			}
	public virtual Type UnderlyingSystemType
			{
				get
				{
					return GetType();
				}
			}

	// Get the parent of this script object.
	public ScriptObject GetParent()
			{
				return parent;
			}

	// Indexers for this script object.
	public Object this[int index]
			{
				get
				{
					Object value;
					if(index >= 0)
					{
						value = GetPropertyByIndex(index);
					}
					else
					{
						value = GetProperty(Convert.ToString(index));
					}
					if(!(value is Missing))
					{
						return value;
					}
					else
					{
						return null;
					}
				}
				set
				{
					if(index >= 0)
					{
						SetPropertyByIndex(index, value);
					}
					else
					{
						SetProperty(Convert.ToString(index), value,
									PropertyAttributes.None);
					}
				}
			}
	public Object this[double index]
			{
				get
				{
					Object value;
					if(index >= 0.0 && index <= (double)(Int32.MaxValue) &&
					   Math.Round(index) == index)
					{
						value = GetPropertyByIndex((int)index);
					}
					else
					{
						value = GetProperty(Convert.ToString(index));
					}
					if(!(value is Missing))
					{
						return value;
					}
					else
					{
						return null;
					}
				}
				set
				{
					if(index >= 0.0 && index <= (double)(Int32.MaxValue) &&
					   Math.Round(index) == index)
					{
						SetPropertyByIndex((int)index, value);
					}
					else
					{
						SetProperty(Convert.ToString(index), value,
									PropertyAttributes.None);
					}
				}
			}
	public Object this[String name]
			{
				get
				{
					Object value = GetProperty(name);
					if(!(value is Missing))
					{
						return value;
					}
					else
					{
						return null;
					}
				}
				set
				{
					SetProperty(name, value, PropertyAttributes.None);
				}
			}
	public Object this[params Object[] pars]
			{
				get
				{
					if(pars.Length == 0)
					{
						if(!(this is ScriptFunction))
						{
							throw new JScriptException
								(JSError.TooFewParameters);
						}
						else
						{
							throw new JScriptException
								(JSError.FunctionExpected);
						}
					}
					Object index = pars[pars.Length - 1];
					if(index is Int32)
					{
						return this[(int)index];
					}
					else if(index is Double)
					{
						return this[(double)index];
					}
					else
					{
						return this[Convert.ToString(index)];
					}
				}
				set
				{
					if(pars.Length == 0)
					{
						if(!(this is ScriptFunction))
						{
							throw new JScriptException
								(JSError.TooFewParameters);
						}
						else
						{
							throw new JScriptException
								(JSError.CannotAssignToFunctionResult);
						}
					}
					Object index = pars[pars.Length - 1];
					if(index is Int32)
					{
						this[(int)index] = value;
					}
					else if(index is Double)
					{
						this[(double)index] = value;
					}
					else
					{
						this[Convert.ToString(index)] = value;
					}
				}
			}

	// Wrap members to turn them into script objects.
	protected static MemberInfo[] WrapMembers
				(MemberInfo[] members, Object obj)
			{
				// TODO
				return null;
			}
	protected static MemberInfo[] WrapMembers(MemberInfo member, Object obj)
			{
				// TODO
				return null;
			}
	protected static MemberInfo[] WrapMembers
				(MemberInfo[] members, Object obj, SimpleHashtable cache)
			{
				// TODO
				return null;
			}

	// Get a property from this object.  Null if not present.
	internal virtual Object GetProperty(String name)
			{
				// TODO
				return null;
			}

	// Set a property in this object.
	internal virtual void SetProperty(String name, Object value,
							  		  PropertyAttributes attrs)
			{
				// TODO
			}

	// Determine if this object has a specific property.
	internal virtual bool HasOwnProperty(String name)
			{
				// TODO
				return false;
			}

	// Get a property from this object by numeric index.
	internal virtual Object GetPropertyByIndex(int index)
			{
				return GetProperty(Convert.ToString(index));
			}

	// Get a property from this object by numeric index.
	internal virtual void SetPropertyByIndex(int index, Object value)
			{
				SetProperty(Convert.ToString(index), value,
							PropertyAttributes.None);
			}

	// Normalize this object to remove object wrappers.
	internal virtual Object Normalize()
			{
				// By default, no normalization is applied.
				return this;
			}

}; // class ScriptObject

}; // namespace Microsoft.JScript
