/*
 * PropertyDescriptor.cs - Implementation of the
 *		"System.ComponentModel.ComponentModel.PropertyDescriptor" class.
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

[TODO]
[ComVisible(true)]
public abstract class PropertyDescriptor : MemberDescriptor
{
	// TODO
	public PropertyDescriptor(String name) : base(name) {}
	
	public PropertyDescriptor(String name, Attribute[] attrs) : base(name) {}

	protected PropertyDescriptor(MemberDescriptor desc, Attribute[] attrs) 
				: base(desc) {}
	[TODO]
	public virtual void AddValueChanged(Object component, EventHandler handler)
	{
		 throw new NotImplementedException("AddValueChanged");
	}

	public abstract bool CanResetValue(Object component);

	[TODO]
	protected Object CreateInstance(Type type)
	{
		 throw new NotImplementedException("CreateInstance");
	}

	[TODO]
	public override bool Equals(Object obj)
	{
		 throw new NotImplementedException("Equals");
	}

	[TODO]
	public PropertyDescriptorCollection GetChildProperties()
	{
		 throw new NotImplementedException("GetChildProperties");
	}

	[TODO]
	public PropertyDescriptorCollection GetChildProperties(Attribute[] filter)
	{
		 throw new NotImplementedException("GetChildProperties");
	}

	[TODO]
	public PropertyDescriptorCollection GetChildProperties(Object instance)
	{
		 throw new NotImplementedException("GetChildProperties");
	}

	[TODO]
	public virtual PropertyDescriptorCollection GetChildProperties(Object instance, Attribute[] filter)
	{
		 throw new NotImplementedException("GetChildProperties");
	}

	[TODO]
	public virtual Object GetEditor(Type editorBaseType)
	{
		 throw new NotImplementedException("GetEditor");
	}

	[TODO]
	public override int GetHashCode()
	{
		 throw new NotImplementedException("GetHashCode");
	}

	[TODO]
	protected Type GetTypeFromName(String typeName)
	{
		 throw new NotImplementedException("GetTypeFromName");
	}

	public abstract Object GetValue(Object component);

	[TODO]
	protected virtual void OnValueChanged(Object component, EventArgs e)
	{
		 throw new NotImplementedException("OnValueChanged");
	}

	[TODO]
	public virtual void RemoveValueChanged(Object component, EventHandler handler)
	{
		 throw new NotImplementedException("RemoveValueChanged");
	}

	public abstract void ResetValue(Object component);

	public abstract void SetValue(Object component, Object value);

	public abstract bool ShouldSerializeValue(Object component);

	public abstract Type ComponentType { get; }

	[TODO]
	public virtual TypeConverter Converter 
	{
 		get
		{
			throw new NotImplementedException("Converter");
		}
	}

	[TODO]
	public virtual bool IsLocalizable 
	{
 		get
		{
			throw new NotImplementedException("IsLocalizable");
		}
	}

	public abstract bool IsReadOnly { get; } 

	public abstract Type PropertyType { get; } 

	[TODO]
	public DesignerSerializationVisibility SerializationVisibility 
	{
 		get
		{
			throw new NotImplementedException("SerializationVisibility");
		}
	}

}; // class PropertyDescriptor

#endif // CONFIG_COMPONENT_MODEL

}; // namespace System.ComponentModel
