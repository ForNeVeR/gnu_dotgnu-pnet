/*
 * TypeConverter.cs - Implementation of the
 *		"System.ComponentModel.ComponentModel.TypeConverter" class.
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

using System;
using System.Collections;
using System.Globalization;
using System.Runtime.InteropServices;

[ComVisible(true)]
public class TypeConverter
{
	// Constructor.
	public TypeConverter()
			{
				// Nothing to do here.
			}

	// Determine if we can convert from a specific type to this one.
	public bool CanConvertFrom(Type sourceType)
			{
				return CanConvertFrom(null, sourceType);
			}
	public virtual bool CanConvertFrom
				(ITypeDescriptorContext context, Type sourceType)
			{
				return false;
			}

	// Determine if we can convert from this type to a specific type.
	public bool CanConvertTo(Type destinationType)
			{
				return CanConvertTo(null, destinationType);
			}
	public virtual bool CanConvertTo
				(ITypeDescriptorContext context, Type destinationType)
			{
				// By default, we can always convert to the string type.
				return (destinationType == typeof(String));
			}

	// Convert from another type to the one represented by this class.
	public Object ConvertFrom(Object value)
			{
				return ConvertFrom(null, CultureInfo.CurrentCulture, value);
			}
	public virtual Object ConvertFrom(ITypeDescriptorContext context,
									  CultureInfo culture,
									  Object value)
			{
				throw GetConvertFromException(value);
			}

	// Convert a string into this type using the invariant culture.
	public Object ConvertFromInvariantString(String text)
			{
				return ConvertFromString
					(null, CultureInfo.InvariantCulture, text);
			}
	public Object ConvertFromInvariantString
				(ITypeDescriptorContext context, String text)
			{
				return ConvertFromString
					(context, CultureInfo.InvariantCulture, text);
			}

	// Convert a string into this type.
	public Object ConvertFromString(String text)
			{
				return ConvertFrom(null, CultureInfo.CurrentCulture, text);
			}
	public Object ConvertFromString
				(ITypeDescriptorContext context, String text)
			{
				return ConvertFrom(context, CultureInfo.CurrentCulture, text);
			}
	public Object ConvertFromString
				(ITypeDescriptorContext context,
				 CultureInfo culture, String text)
			{
				return ConvertFrom(context, culture, text);
			}

	// Convert this object into another type.
	public Object ConvertTo(Object value, Type destinationType)
			{
				return ConvertTo(null, null, value, destinationType);
			}
	public virtual Object ConvertTo(ITypeDescriptorContext context,
									CultureInfo culture,
									Object value, Type destinationType)
			{
				if(destinationType == null)
				{
					throw new ArgumentNullException("destinationType");
				}
				if(destinationType == typeof(String))
				{
					if(value != null)
					{
						return value.ToString();
					}
					else
					{
						return String.Empty;
					}
				}
				else
				{
					throw GetConvertToException(value, destinationType);
				}
			}

	// Convert an object to a culture-invariant string.
	public String ConvertToInvariantString(Object value)
			{
				return ConvertToString
					(null, CultureInfo.InvariantCulture, value);
			}
	public String ConvertToInvariantString
				(ITypeDescriptorContext context, Object value)
			{
				return ConvertToString
					(context, CultureInfo.InvariantCulture, value);
			}

	// Convert an object into a string.
	public String ConvertToString(Object value)
			{
				return (String)ConvertTo(null, CultureInfo.CurrentCulture,
								 		 value, typeof(String));
			}
	public String ConvertToString
				(ITypeDescriptorContext context, Object value)
			{
				return (String)ConvertTo(context, CultureInfo.CurrentCulture,
								 		 value, typeof(String));
			}
	public String ConvertToString
				(ITypeDescriptorContext context,
				 CultureInfo culture, Object value)
			{
				return (String)ConvertTo(context, culture,
										 value, typeof(String));
			}

	// Create an instance of this type of object.
	public Object CreateInstance(IDictionary propertyValues)
			{
				return CreateInstance(null, propertyValues);
			}
	public virtual Object CreateInstance
				(ITypeDescriptorContext context, IDictionary propertyValues)
			{
				return null;
			}

	// Determine if creating new instances is supported.
	public bool GetCreateInstanceSupported()
			{
				return GetCreateInstanceSupported(null);
			}
	public virtual bool GetCreateInstanceSupported
				(ITypeDescriptorContext context)
			{
				return false;
			}

	// Get the properties for an object.
	public PropertyDescriptorCollection GetProperties(Object value)
			{
				return GetProperties(null, value, null);
			}
	public PropertyDescriptorCollection GetProperties
				(ITypeDescriptorContext context, Object value)
			{
				return GetProperties(context, value, null);
			}
	public virtual PropertyDescriptorCollection GetProperties
				(ITypeDescriptorContext context, Object value,
				 Attribute[] attributes)
			{
				return null;
			}

	// Determine if the "GetProperties" method is supported.
	public bool GetPropertiesSupported()
			{
				return GetPropertiesSupported(null);
			}
	public virtual bool GetPropertiesSupported
				(ITypeDescriptorContext context)
			{
				return false;
			}

	// Return a collection of standard values for this data type.
	public ICollection GetStandardValues()
			{
				return GetStandardValues(null);
			}
	public virtual ICollection GetStandardValues
				(ITypeDescriptorContext context)
			{
				return null;
			}

	// Determine if the list of standard values is an exclusive list.
	public bool GetStandardValuesExclusive()
			{
				return GetStandardValuesExclusive(null);
			}
	public virtual bool GetStandardValuesExclusive
				(ITypeDescriptorContext context)
			{
				return false;
			}

	// Determine if "GetStandardValues" is supported.
	public bool GetStandardValuesSupported()
			{
				return GetStandardValuesSupported(null);
			}
	public virtual bool GetStandardValuesSupported
				(ITypeDescriptorContext context)
			{
				return false;
			}

	// Determine if an object is valid for this type.
	public bool IsValid(Object value)
			{
				return IsValid(null, value);
			}
	public virtual bool IsValid(ITypeDescriptorContext context, Object value)
			{
				return true;
			}

	// Get the exception to use when "ConvertFrom" cannot be performed.
	protected Exception GetConvertFromException(Object value)
			{
				return new NotSupportedException(S._("NotSupp_Conversion"));
			}

	// Get the exception to use when "ConvertTo" cannot be performed.
	protected Exception GetConvertToException
				(Object value, Type destinationType)
			{
				return new NotSupportedException(S._("NotSupp_Conversion"));
			}

	// Sort a collection of properties.
	protected PropertyDescriptorCollection SortProperties
				(PropertyDescriptorCollection props, String[] names)
			{
				props.Sort(names);
				return props;
			}

	// nested class
	public class StandardValuesCollection : ICollection, IEnumerable
	{
	
		private ICollection values;
		
		public StandardValuesCollection (ICollection values)
		{
			this.values = values;
		}

		public void CopyTo (Array array, int index)
		{
			values.CopyTo (array, index);
		}

		public IEnumerator GetEnumerator ()
		{
			return values.GetEnumerator ();
		}

		bool ICollection.IsSynchronized
		{
			get 
			{
				return false; 
			}
		}

		object ICollection.SyncRoot
		{
			get 
			{ 
				return null;
			}
		}

		int ICollection.Count
		{
			get 
			{
				return this.Count;
			}
		}

		public int Count
		{
			get
			{
				return values.Count;
			}
		}

		public object this [int index]
		{
			get 
			{
				return (values as IList) [index]; 
			}
		}
	}

}; // class TypeConverter

#endif // CONFIG_COMPONENT_MODEL

}; // namespace System.ComponentModel
