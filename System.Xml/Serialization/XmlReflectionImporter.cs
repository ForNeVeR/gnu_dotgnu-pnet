/*
 * XmlReflectionImporter.cs - Implementation of the
 *			"System.Xml.Serialization.XmlReflectionImporter" class.
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
using System.Reflection;

[TODO]
public class XmlReflectionImporter
{
	[TODO]
	public XmlReflectionImporter()
			: base()
			{
				// TODO
				throw new NotImplementedException(".ctor");
			}

	[TODO]
	public XmlReflectionImporter
				(String defaultNamespace)
			: base()
			{
				// TODO
				throw new NotImplementedException(".ctor");
			}

	[TODO]
	public XmlReflectionImporter
				(XmlAttributeOverrides attributeOverrides)
			: base()
			{
				// TODO
				throw new NotImplementedException(".ctor");
			}

	[TODO]
	public XmlReflectionImporter
				(XmlAttributeOverrides attributeOverrides,
				 String defaultNamespace)
			: base()
			{
				// TODO
				throw new NotImplementedException(".ctor");
			}

	[TODO]
	public XmlMembersMapping ImportMembersMapping
				(String elementName, String ns,
				 XmlReflectionMember[] members, bool hasWrapperElement)
			{
				// TODO
				throw new NotImplementedException("ImportMembersMapping");
			}

	[TODO]
	public XmlTypeMapping ImportTypeMapping
				(Type type)
			{
				// TODO
				throw new NotImplementedException("ImportTypeMapping");
			}

	[TODO]
	public XmlTypeMapping ImportTypeMapping
				(Type type, String defaultNamespace)
			{
				// TODO
				throw new NotImplementedException("ImportTypeMapping");
			}

	[TODO]
	public XmlTypeMapping ImportTypeMapping
				(Type type, XmlRootAttribute root)
			{
				// TODO
				throw new NotImplementedException("ImportTypeMapping");
			}

	[TODO]
	public XmlTypeMapping ImportTypeMapping
				(Type type, XmlRootAttribute root, String defaultNamespace)
			{
				// TODO
				throw new NotImplementedException("ImportTypeMapping");
			}

	[TODO]
	public void IncludeType
				(Type type)
			{
				// TODO
				throw new NotImplementedException("IncludeType");
			}

	[TODO]
	public void IncludeTypes
				(ICustomAttributeProvider provider)
			{
				// TODO
				throw new NotImplementedException("IncludeTypes");
			}

}; // class XmlReflectionImporter

#endif // !ECMA_COMPAT

}; // namespace System.Xml.Serialization
