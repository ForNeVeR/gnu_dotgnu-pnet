/*
 * XmlRootAttribute.cs - Implementation of the
 *			"System.Xml.Serialization.XmlRootAttribute" class.
 *
 * Copyright (C) 2003  Free Software Foundation, Inc.
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

namespace System.Xml.Serialization
{

#if !ECMA_COMPAT

using System;
using System.Xml;

[AttributeUsage(AttributeTargets.Class |
                AttributeTargets.Struct |
                AttributeTargets.Enum |
                AttributeTargets.Interface |
                AttributeTargets.ReturnValue)]
public class XmlRootAttribute : Attribute
{
	// Internal state.
	private String dataType;
	private String elementName;
	private bool isNullable;
	private bool isNullableSpecified;
	private String nspace;

	// Constructors.
	public XmlRootAttribute()
			: this(null)
			{
			}
	public XmlRootAttribute(String elementName)
			: base()
			{
				if (elementName == null) { elementName = ""; }
				this.dataType = "";
				this.elementName = elementName;
				this.isNullable = true;
				this.isNullableSpecified = false;
				this.nspace = null;
			}

	// Properties.
	public String DataType
			{
				get { return dataType; }
				set
				{
					if (value == null) { value = ""; }
					dataType = value;
				}
			}
	public String ElementName
			{
				get { return elementName; }
				set
				{
					if (value == null) { value = ""; }
					elementName = value;
				}
			}
	public bool IsNullable
			{
				get { return isNullable; }
				set
				{
					isNullableSpecified = true;
					isNullable = value;
				}
			}
	public bool IsNullableSpecified
			{
				get { return isNullableSpecified; }
			}
	public String Namespace
			{
				get { return nspace; }
				set { nspace = value; }
			}

}; // class XmlRootAttribute

#endif // !ECMA_COMPAT

}; // namespace System.Xml.Serialization
