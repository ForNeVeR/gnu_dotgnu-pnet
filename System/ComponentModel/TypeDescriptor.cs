/*
 * TypeDescriptor.cs - Implementation of the
 *		"System.ComponentModel.ComponentModel.TypeDescriptor" class.
 *
 * Copyright (C) 2003  Southern Storm Software, Pty Ltd.
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

using System;
using System.Reflection;
using System.Collections;
using System.Globalization;
using System.ComponentModel.Design;

public sealed class TypeDescriptor
{
	// Internal state.
	private static IComNativeDescriptorHandler handler;
	private static Hashtable typeTable = new Hashtable();

	// Element that is stored within the type table.
	private sealed class TypeElement
	{
		public TypeConverter converter;

	}; // class TypeElement

	// Cannot instantiate this class.
	private TypeDescriptor() {}

	// Add information to the editor tables.
	public static void AddEditorTable(Type editorBaseType, Hashtable table)
			{
				// Nothing to do here - we don't use editor tables.
			}

	// Create a designer for a specific component.
	public static IDesigner CreateDesigner
				(IComponent component, Type designerBaseType)
			{
				// Get the attributes for the component.
				AttributeCollection attrs = GetAttributes(component, false);

				// Scan the attributes to find a designer attribute.
				foreach(Attribute attr in attrs)
				{
					DesignerAttribute dattr = (attr as DesignerAttribute);
					if(dattr != null)
					{
						Type type = Type.GetType(dattr.DesignerBaseTypeName);
						if(type != null && type == designerBaseType)
						{
							if(component.Site != null)
							{
								ITypeResolutionService trs;
								trs = (ITypeResolutionService)
									component.Site.GetService
										(typeof(ITypeResolutionService));
								if(trs != null)
								{
									type = trs.GetType(dattr.DesignerTypeName);
								}
								else
								{
									type = Type.GetType(dattr.DesignerTypeName);
								}
								if(type != null)
								{
									return (IDesigner)Activator.CreateInstance
										(type,
										 BindingFlags.CreateInstance |
										    BindingFlags.Public |
										    BindingFlags.NonPublic |
										    BindingFlags.Instance,
										 null, null, null, null);
								}
							}
						}
					}
				}

				// We were unable to find a suitable designer declaration.
				return null;
			}

	// Create a new event descriptor.
	[TODO]
	public static EventDescriptor CreateEvent
				(Type componentType, EventDescriptor oldEventDescriptor,
				 params Attribute[] attributes)
			{
				// TODO
				return null;
			}
	[TODO]
	public static EventDescriptor CreateEvent
				(Type componentType, String name, Type type,
				 params Attribute[] attributes)
			{
				// TODO
				return null;
			}

	// Create a new property descriptor.
	[TODO]
	public static PropertyDescriptor CreateProperty
				(Type componentType, PropertyDescriptor oldPropertyDescriptor,
				 params Attribute[] attributes)
			{
				// TODO
				return null;
			}
	[TODO]
	public static PropertyDescriptor CreateProperty
				(Type componentType, String name, Type type,
				 params Attribute[] attributes)
			{
				// TODO
				return null;
			}

	// Get the attributes associated with a particular component.
	public static AttributeCollection GetAttributes(Object component)
			{
				return GetAttributes(component, false);
			}
	public static AttributeCollection GetAttributes
				(Object component, bool noCustomTypeDesc)
			{
				if(component == null)
				{
					throw new ArgumentNullException("component");
				}
				if(noCustomTypeDesc || !(component is ICustomTypeDescriptor))
				{
					return GetAttributes(component.GetType());
				}
				else
				{
					return ((ICustomTypeDescriptor)component).GetAttributes();
				}
			}

	// Get the attributes associated with a particular type of component.
	[TODO]
	public static AttributeCollection GetAttributes(Type componentType)
			{
				// TODO
				return null;
			}

	// Get the name of a component's class.
	public static String GetClassName(Object component)
			{
				return GetClassName(component, false);
			}
	public static String GetClassName
				(Object component, bool noCustomTypeDesc)
			{
				if(component == null)
				{
					throw new ArgumentNullException("component");
				}
				if(noCustomTypeDesc || !(component is ICustomTypeDescriptor))
				{
					return component.GetType().FullName;
				}
				else
				{
					return ((ICustomTypeDescriptor)component).GetClassName();
				}
			}

	// Get the name of a component.
	public static String GetComponentName(Object component)
			{
				return GetComponentName(component, false);
			}
	public static String GetComponentName
				(Object component, bool noCustomTypeDesc)
			{
				if(component == null)
				{
					throw new ArgumentNullException("component");
				}
				if(noCustomTypeDesc || !(component is ICustomTypeDescriptor))
				{
					if(component is IComponent)
					{
						ISite site = ((IComponent)component).Site;
						if(site != null)
						{
							return site.Name;
						}
					}
					return component.GetType().FullName;
				}
				else
				{
					return ((ICustomTypeDescriptor)component)
								.GetComponentName();
				}
			}

	// Get or create a cache element for a type.
	private static TypeElement GetOrCreateElement(Type type)
			{
				TypeElement element = (TypeElement)(typeTable[type]);
				if(element == null)
				{
					element = new TypeElement();
					typeTable[element] = element;
				}
				return element;
			}

	// Get a particular attribute from a type.
	private static Attribute GetAttributeForType(Type type, Type attrType)
			{
				AttributeCollection attrs = GetAttributes(type);
				if(attrs != null)
				{
					return attrs[attrType];
				}
				else
				{
					return null;
				}
			}

	// Get a converter for a specific component's type.
	public static TypeConverter GetConverter(Object component)
			{
				return GetConverter(component, false);
			}
	public static TypeConverter GetConverter
				(Object component, bool noCustomTypeDesc)
			{
				if(component == null)
				{
					throw new ArgumentNullException("component");
				}
				if(noCustomTypeDesc || !(component is ICustomTypeDescriptor))
				{
					return GetConverter(component.GetType());
				}
				else
				{
					return ((ICustomTypeDescriptor)component).GetConverter();
				}
			}

	// Get a converter for a builtin type.
	private static TypeConverter GetBuiltinConverter(Type type)
			{
				if(type == typeof(bool))
				{
					return new BooleanConverter();
				}
				else if(type == typeof(byte))
				{
					return new ByteConverter();
				}
				else if(type == typeof(sbyte))
				{
					return new SByteConverter();
				}
				else if(type == typeof(char))
				{
					return new CharConverter();
				}
				else if(type == typeof(short))
				{
					return new Int16Converter();
				}
				else if(type == typeof(ushort))
				{
					return new UInt16Converter();
				}
				else if(type == typeof(int))
				{
					return new Int32Converter();
				}
				else if(type == typeof(uint))
				{
					return new UInt32Converter();
				}
				else if(type == typeof(long))
				{
					return new Int64Converter();
				}
				else if(type == typeof(ulong))
				{
					return new UInt64Converter();
				}
				else if(type == typeof(float))
				{
					return new SingleConverter();
				}
				else if(type == typeof(double))
				{
					return new DoubleConverter();
				}
				else if(type == typeof(String))
				{
					return new StringConverter();
				}
				else if(type == typeof(Decimal))
				{
					return new DecimalConverter();
				}
				else if(type == typeof(DateTime))
				{
					return new DateTimeConverter();
				}
				else if(type == typeof(TimeSpan))
				{
					return new TimeSpanConverter();
				}
				else if(type == typeof(Guid))
				{
					return new GuidConverter();
				}
				else if(typeof(ICollection).IsAssignableFrom(type))
				{
					return new CollectionConverter();
				}
				else if(type == typeof(CultureInfo))
				{
					return new CultureInfoConverter();
				}
				else if(typeof(Enum).IsAssignableFrom(type))
				{
					return new EnumConverter(type);
				}
				else if(typeof(IComponent).IsAssignableFrom(type) ||
				        typeof(MarshalByValueComponent).IsAssignableFrom(type))
				{
					return new ComponentConverter(type);
				}
				else
				{
					return new ReferenceConverter(type);
				}
			}

	// Get a converter for a specific component type.
	public static TypeConverter GetConverter(Type type)
			{
				lock(typeof(TypeDescriptor))
				{
					TypeElement element = GetOrCreateElement(type);
					if(element.converter != null)
					{
						return element.converter;
					}
					TypeConverterAttribute attr;
					attr = (TypeConverterAttribute)GetAttributeForType
						(type, typeof(TypeConverterAttribute));
					if(attr != null)
					{
						Type converterType =
							Type.GetType(attr.ConverterTypeName);
						element.converter = (TypeConverter)
							Activator.CreateInstance(converterType);
						return element.converter;
					}
					element.converter = GetBuiltinConverter(type);
					return element.converter;
				}
			}

	// Get the default event for a specified component.
	public static EventDescriptor GetDefaultEvent(Object component)
			{
				return GetDefaultEvent(component, false);
			}
	public static EventDescriptor GetDefaultEvent
				(Object component, bool noCustomTypeDesc)
			{
				if(component == null)
				{
					return null;
				}
				if(noCustomTypeDesc || !(component is ICustomTypeDescriptor))
				{
					return GetDefaultEvent(component.GetType());
				}
				else
				{
					return ((ICustomTypeDescriptor)component).GetDefaultEvent();
				}
			}

	// Get the default event for a specified component type.
	[TODO]
	public static EventDescriptor GetDefaultEvent(Type componentType)
			{
				// TODO
				return null;
			}

	// Get the default property for a specified component.
	public static PropertyDescriptor GetDefaultProperty(Object component)
			{
				return GetDefaultProperty(component, false);
			}
	public static PropertyDescriptor GetDefaultProperty
				(Object component, bool noCustomTypeDesc)
			{
				if(component == null)
				{
					return null;
				}
				if(noCustomTypeDesc || !(component is ICustomTypeDescriptor))
				{
					return GetDefaultProperty(component.GetType());
				}
				else
				{
					return ((ICustomTypeDescriptor)component)
								.GetDefaultProperty();
				}
			}

	// Get the default property for a specified component type.
	[TODO]
	public static PropertyDescriptor GetDefaultProperty(Type componentType)
			{
				// TODO
				return null;
			}

	// Get an editor for a specified component.
	public static Object GetEditor(Object component, Type editorBaseType)
			{
				return GetEditor(component, editorBaseType, false);
			}
	public static Object GetEditor
				(Object component, Type editorBaseType, bool noCustomTypeDesc)
			{
				if(component == null)
				{
					throw new ArgumentNullException("component");
				}
				if(noCustomTypeDesc || !(component is ICustomTypeDescriptor))
				{
					return GetEditor(component.GetType(), editorBaseType);
				}
				else
				{
					return ((ICustomTypeDescriptor)component)
								.GetEditor(editorBaseType);
				}
			}

	// Get an editor for a specified component type.
	[TODO]
	public static Object GetEditor(Type type, Type editorBaseType)
			{
				// TODO
				return null;
			}

	// Get all events for a specified component.
	public static EventDescriptorCollection GetEvents(Object component)
			{
				return GetEvents(component, null, false);
			}
	public static EventDescriptorCollection GetEvents
				(Object component, Attribute[] attributes)
			{
				return GetEvents(component, attributes, false);
			}
	public static EventDescriptorCollection GetEvents
				(Object component, bool noCustomTypeDesc)
			{
				return GetEvents(component, null, noCustomTypeDesc);
			}
	public static EventDescriptorCollection GetEvents
				(Object component, Attribute[] attributes,
				 bool noCustomTypeDesc)
			{
				if(component == null)
				{
					return new EventDescriptorCollection(null);
				}
				if(noCustomTypeDesc || !(component is ICustomTypeDescriptor))
				{
					return GetEvents(component.GetType(), attributes);
				}
				else if(attributes == null)
				{
					return ((ICustomTypeDescriptor)component).GetEvents();
				}
				else
				{
					return ((ICustomTypeDescriptor)component)
								.GetEvents(attributes);
				}
			}

	// Get all events for a specified component type.
	public static EventDescriptorCollection GetEvents(Type componentType)
			{
				return GetEvents(componentType, null);
			}
	[TODO]
	public static EventDescriptorCollection GetEvents
				(Type componentType, Attribute[] attributes)
			{
				// TODO
				return null;
			}

	// Get all properties for a specified component.
	public static PropertyDescriptorCollection GetProperties(Object component)
			{
				return GetProperties(component, null, false);
			}
	public static PropertyDescriptorCollection GetProperties
				(Object component, Attribute[] attributes)
			{
				return GetProperties(component, attributes, false);
			}
	public static PropertyDescriptorCollection GetProperties
				(Object component, bool noCustomTypeDesc)
			{
				return GetProperties(component, null, noCustomTypeDesc);
			}
	public static PropertyDescriptorCollection GetProperties
				(Object component, Attribute[] attributes,
				 bool noCustomTypeDesc)
			{
				if(component == null)
				{
					return new PropertyDescriptorCollection(null);
				}
				if(noCustomTypeDesc || !(component is ICustomTypeDescriptor))
				{
					return GetProperties(component.GetType(), attributes);
				}
				else if(attributes == null)
				{
					return ((ICustomTypeDescriptor)component).GetProperties();
				}
				else
				{
					return ((ICustomTypeDescriptor)component)
								.GetProperties(attributes);
				}
			}

	// Get all properties for a specified component type.
	public static PropertyDescriptorCollection GetProperties
				(Type componentType)
			{
				return GetProperties(componentType, null);
			}
	[TODO]
	public static PropertyDescriptorCollection GetProperties
				(Type componentType, Attribute[] attributes)
			{
				// TODO
				return null;
			}

	// Clear properties and events from the cache.
	[TODO]
	public static void Refresh(Assembly assembly)
			{
				// TODO
			}
	[TODO]
	public static void Refresh(Module module)
			{
				// TODO
			}
	[TODO]
	public static void Refresh(Object component)
			{
				// TODO
			}
	[TODO]
	public static void Refresh(Type type)
			{
				// TODO
			}

	// Private class for comparing descriptors by name.
	internal sealed class DescriptorComparer : IComparer
	{
		// Compare two descriptors.
		public int Compare(Object elem1, Object elem2)
				{
					return String.CompareOrdinal
						(((MemberDescriptor)elem1).Name,
						 ((MemberDescriptor)elem2).Name);
				}

	}; // class DescriptorComparer

	// Sort a descriptor array by name.
	public static void SortDescriptorArray(IList infos)
			{
				 ArrayList.Adapter(infos).Sort(new DescriptorComparer());
			}

	// Event that is emitted when a refresh occurs.
	public static event RefreshEventHandler Refreshed;

	// Get or set the COM native descriptor handler (don't use this).
	public static IComNativeDescriptorHandler ComNativeDescriptorHandler
			{
				get
				{
					return handler;
				}
				set
				{
					handler = value;
				}
			}

}; // class TypeDescriptor

#endif // CONFIG_COMPONENT_MODEL

}; // namespace System.ComponentModel
