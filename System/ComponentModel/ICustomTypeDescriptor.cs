/*
 * ICustomTypeDescriptor.cs - Implementation of 
 *					"System.ComponentModel.ICustomTypeDescriptor" 
 *
 * Copyright (C) 2002  Southern Storm Software, Pty Ltd.
 * Copyright (C) 2002  Free Software Foundation,Inc.
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

using System;

namespace System.ComponentModel
{
#if CONFIG_COMPONENT_MODEL
	public interface ICustomTypeDescriptor
	{
		AttributeCollection GetAttributes();

		String GetClassName();

		String GetComponentName();

		TypeConverter GetConverter();

		EventDescriptor GetDefaultEvent();

		PropertyDescriptor GetDefaultProperty();

		Object GetEditor(System.Type editorBaseType);

		EventDescriptorCollection GetEvents();
		
		EventDescriptorCollection GetEvents(Attribute[] arr);

		PropertyDescriptorCollection GetProperties();
		
		PropertyDescriptorCollection GetProperties(Attribute[] arr);

		Object GetPropertyOwner(PropertyDescriptor pd);

	}
#endif	
}//namespace
