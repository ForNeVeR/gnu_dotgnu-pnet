/*
 * XmlSchemaImporter.cs - Implementation of the
 *			"System.Xml.Serialization.XmlSchemaImporter" class.
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

[TODO]
public class XmlSchemaImporter
{
	[TODO]
	public XmlSchemaImporter
				(XmlSchemas schemas)
			: base()
			{
				// TODO
				throw new NotImplementedException(".ctor");
			}

	[TODO]
	public XmlSchemaImporter
				(XmlSchemas schemas, CodeIdentifiers typeIdentifiers)
			: base()
			{
				// TODO
				throw new NotImplementedException(".ctor");
			}

	[TODO]
	public XmlMembersMapping ImportAnyType
				(XmlQualifiedName typeName, String elementName)
			{
				// TODO
				throw new NotImplementedException("ImportAnyType");
			}

	[TODO]
	public XmlTypeMapping ImportDerivedTypeMapping
				(XmlQualifiedName name, Type baseType)
			{
				// TODO
				throw new NotImplementedException("ImportDerivedTypeMapping");
			}

	[TODO]
	public XmlTypeMapping ImportDerivedTypeMapping
				(XmlQualifiedName name, Type baseType,
				 bool baseTypeCanBeIndirect)
			{
				// TODO
				throw new NotImplementedException("ImportDerivedTypeMapping");
			}

	[TODO]
	public XmlMembersMapping ImportMembersMapping
				(XmlQualifiedName name)
			{
				// TODO
				throw new NotImplementedException("ImportMembersMapping");
			}

	[TODO]
	public XmlMembersMapping ImportMembersMapping
				(XmlQualifiedName[] names)
			{
				// TODO
				throw new NotImplementedException("ImportMembersMapping");
			}

	[TODO]
	public XmlMembersMapping ImportMembersMapping
				(XmlQualifiedName[] names, Type baseType,
				 bool baseTypeCanBeIndirect)
			{
				// TODO
				throw new NotImplementedException("ImportMembersMapping");
			}

	[TODO]
	public XmlTypeMapping ImportTypeMapping
				(XmlQualifiedName name)
			{
				// TODO
				throw new NotImplementedException("ImportTypeMapping");
			}

}; // class XmlSchemaImporter

#endif // !ECMA_COMPAT

}; // namespace System.Xml.Serialization
