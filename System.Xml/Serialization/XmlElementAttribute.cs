/*
 * XmlElementAttribute.cs - Implementation of the
 *			"System.Xml.Serialization.XmlElementAttribute" class.
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
using System.Xml.Schema;

[AttributeUsage(AttributeTargets.Property |
                AttributeTargets.Field |
                AttributeTargets.Parameter |
                AttributeTargets.ReturnValue)]
public class XmlElementAttribute : Attribute
{
	// Internal state.
	private String dataType;
	private String elementName;
	private XmlSchemaForm form;
	private bool isNullable;
	private String nspace;
	private Type type;

	// Constructors.
	public XmlElementAttribute()
			: this(null, null)
			{
			}
	public XmlElementAttribute(String elementName)
			: this(elementName, null)
			{
			}
	public XmlElementAttribute(Type type)
			: this(null, type)
			{
			}
	public XmlElementAttribute(String elementName, Type type)
			: base()
			{
				if (elementName == null) { elementName = ""; }
				this.dataType = "";
				this.elementName = elementName;
				this.form = XmlSchemaForm.None;
				this.isNullable = false;
				this.nspace = null;
				this.type = type;
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
	public XmlSchemaForm Form
			{
				get { return form; }
				set { form = value; }
			}
	public bool IsNullable
			{
				get { return isNullable; }
				set { isNullable = value; }
			}
	public String Namespace
			{
				get { return nspace; }
				set { nspace = value;}
			}
	public Type Type
			{
				get { return type; }
				set { type = value; }
			}

}; // class XmlElementAttribute

#endif // !ECMA_COMPAT

}; // namespace System.Xml.Serialization
