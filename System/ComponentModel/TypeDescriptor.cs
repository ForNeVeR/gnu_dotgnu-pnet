/*
 * TypeDescriptor.cs - Implementation of "TypeDescriptor" 
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
using System.Collections;
using System.Reflection;

namespace System.ComponentModel
{
#if !ECMA_COMPAT
	public sealed class TypeDescriptor
	{
		[TODO]
		public static void AddEditorTable(Type editorBaseType, 
						Hashtable table)
		{
			throw new NotImplementedException("AddEditorTable");
		}
/*
		[TODO]
		public static EventDescriptor CreateEvent(Type componentType, 
						String name, Type type, Attribute[] attributes)
		{
			throw new NotImplementedException("CreateEvent");
		}
		
		[TODO]
		public static EventDescriptor CreateEvent(Type componentType, 
			EventDescriptor oldEventDescriptor, Attribute[] attributes)
		{
			throw new NotImplementedException("CreateEvent");
		}
*/
		[TODO]
		public static PropertyDescriptor CreateProperty(Type componentType, 
				String name, Type type, Attribute[] attributes)
		{
			throw new NotImplementedException("CreateProperty");
		}

		[TODO]
		public static PropertyDescriptor CreateProperty(Type componentType, 
						PropertyDescriptor oldPropertyDescriptor, 
						Attribute[] attributes)
		{
			throw new NotImplementedException("CreateProperty");
		}

		[TODO]
		public static AttributeCollection GetAttributes(Type componentType)
		{
			throw new NotImplementedException("GetAttributes");
		}

		[TODO]
		public static AttributeCollection GetAttributes(Object component)
		{
			throw new NotImplementedException("GetAttributes");
		}

		[TODO]
		public static AttributeCollection GetAttributes(Object component, 
					bool noCustomTypeDesc)
		{
			throw new NotImplementedException("GetAttributes");
		}

		[TODO]
		public static String GetClassName(Object component)
		{
			throw new NotImplementedException("GetClassName");
		}

		[TODO]
		public static String GetClassName(Object component, bool noCustomTypeDesc)
		{
			throw new NotImplementedException("GetClassName");
		}

		[TODO]
		public static String GetComponentName(Object component)
		{
			throw new NotImplementedException("GetComponentName");
		}

		[TODO]
		public static String GetComponentName(Object component, bool noCustomTypeDesc)
		{
			throw new NotImplementedException("GetComponentName");
		}

		[TODO]
		public static TypeConverter GetConverter(Object component)
		{
			throw new NotImplementedException("GetConverter");
		}

		[TODO]
		public static TypeConverter GetConverter(Object component, bool noCustomTypeDesc)
		{
			throw new NotImplementedException("GetConverter");
		}

		[TODO]
		public static TypeConverter GetConverter(Type type)
		{
			throw new NotImplementedException("GetConverter");
		}
/*
		[TODO]
		public static EventDescriptor GetDefaultEvent(Object component)
		{
			throw new NotImplementedException("GetDefaultEvent");
		}
		
		[TODO]
		public static EventDescriptor GetDefaultEvent(Type componentType)
		{
			throw new NotImplementedException("GetDefaultEvent");
		}

		[TODO]
		public static EventDescriptor GetDefaultEvent(Object component, 
								bool noCustomTypeDesc)
		{
			throw new NotImplementedException("GetDefaultEvent");
		}
*/

		[TODO]
		public static PropertyDescriptor GetDefaultProperty(Object component)
		{
			throw new NotImplementedException("GetDefaultProperty");
		}

		[TODO]
		public static PropertyDescriptor GetDefaultProperty(Type componentType)
		{
			throw new NotImplementedException("GetDefaultProperty");
		}

		[TODO]
		public static PropertyDescriptor GetDefaultProperty(Object component, bool noCustomTypeDesc)
		{
			throw new NotImplementedException("GetDefaultProperty");
		}

		[TODO]
		public static Object GetEditor(Object component, Type editorBaseType)
		{
			throw new NotImplementedException("GetEditor");
		}

		[TODO]
		public static Object GetEditor(Type componentType, Type editorBaseType)
		{
			throw new NotImplementedException("GetEditor");
		}

		[TODO]
		public static Object GetEditor(Object component, Type editorBaseType, bool noCustomTypeDesc)
		{
			throw new NotImplementedException("GetEditor");
		}
/*
		[TODO]
		public static EventDescriptorCollection GetEvents(Object component)
		{
			throw new NotImplementedException("GetEvents");
		}

		[TODO]
		public static EventDescriptorCollection GetEvents(Type componentType)
		{
			throw new NotImplementedException("GetEvents");
		}

		[TODO]
		public static EventDescriptorCollection GetEvents(Object component, 
											Attribute[] attributes)
		{
			throw new NotImplementedException("GetEvents");
		}

		[TODO]
		public static EventDescriptorCollection GetEvents(Object component, 
					bool noCustomTypeDesc)
		{
			throw new NotImplementedException("GetEvents");
		}

		[TODO]
		public static EventDescriptorCollection GetEvents(Type componentType, 
							Attribute[] attributes)
		{
			throw new NotImplementedException("GetEvents");
		}

		[TODO]
		public static EventDescriptorCollection GetEvents(Object component, 
				Attribute[] attributes, bool noCustomTypeDesc)
		{
			throw new NotImplementedException("GetEvents");
		}
*/
		[TODO]
		public static PropertyDescriptorCollection GetProperties(Object component)
		{
			throw new NotImplementedException("GetProperties");
		}

		[TODO]
		public static PropertyDescriptorCollection GetProperties(Type componentType)
		{
			throw new NotImplementedException("GetProperties");
		}

		[TODO]
		public static PropertyDescriptorCollection GetProperties(Object component, Attribute[] attributes)
		{
			throw new NotImplementedException("GetProperties");
		}

		[TODO]
		public static PropertyDescriptorCollection GetProperties(Object component, bool noCustomTypeDesc)
		{
			throw new NotImplementedException("GetProperties");
		}

		[TODO]
		public static PropertyDescriptorCollection GetProperties(Type componentType, Attribute[] attributes)
		{
			throw new NotImplementedException("GetProperties");
		}

		[TODO]
		public static PropertyDescriptorCollection GetProperties(Type componentType, Attribute[] attributes, bool noCustomTypeDesc)
		{
			throw new NotImplementedException("GetProperties");
		}

		[TODO]
		public static void Refresh(Assembly assembly)
		{
			throw new NotImplementedException("Refresh");
		}

		[TODO]
		public static void Refresh(Module module)
		{
			throw new NotImplementedException("Refresh");
		}

		[TODO]
		public static void Refresh(Object component)
		{
			throw new NotImplementedException("Refresh");
		}

		[TODO]
		public static void Refresh(Type type)
		{
			throw new NotImplementedException("Refresh");
		}

	}
#endif	
}//namespace
