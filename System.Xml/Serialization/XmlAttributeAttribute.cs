/*
 * XmlAttributeAttribute.cs - Implementation of the
 *			"System.Xml.Serialization.XmlAttributeAttribute" class.
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

[TODO]
[AttributeUsage(AttributeTargets.Property |
                AttributeTargets.Field |
                AttributeTargets.Parameter |
                AttributeTargets.ReturnValue)]
public class XmlAttributeAttribute : Attribute
{
	[TODO]
	public XmlAttributeAttribute()
			: base()
			{
				// TODO
				throw new NotImplementedException(".ctor");
			}

	[TODO]
	public XmlAttributeAttribute(String attributeName)
			: base()
			{
				// TODO
				throw new NotImplementedException(".ctor");
			}

	[TODO]
	public XmlAttributeAttribute(Type type)
			: base()
			{
				// TODO
				throw new NotImplementedException(".ctor");
			}

	[TODO]
	public XmlAttributeAttribute(String attributeName, Type type)
			: base()
			{
				// TODO
				throw new NotImplementedException(".ctor");
			}

	[TODO]
	public String AttributeName
			{
				get
				{
					// TODO
					throw new NotImplementedException("AttributeName");
				}
				set
				{
					// TODO
					throw new NotImplementedException("AttributeName");
				}
			}

	[TODO]
	public String DataType
			{
				get
				{
					// TODO
					throw new NotImplementedException("DataType");
				}
				set
				{
					// TODO
					throw new NotImplementedException("DataType");
				}
			}

	[TODO]
	public XmlSchemaForm Form
			{
				get
				{
					// TODO
					throw new NotImplementedException("Form");
				}
				set
				{
					// TODO
					throw new NotImplementedException("Form");
				}
			}

	[TODO]
	public String Namespace
			{
				get
				{
					// TODO
					throw new NotImplementedException("Namespace");
				}
				set
				{
					// TODO
					throw new NotImplementedException("Namespace");
				}
			}

	[TODO]
	public Type Type
			{
				get
				{
					// TODO
					throw new NotImplementedException("Type");
				}
				set
				{
					// TODO
					throw new NotImplementedException("Type");
				}
			}

}; // class XmlAttributeAttribute

#endif // !ECMA_COMPAT

}; // namespace System.Xml.Serialization
