/*
 * SoapSchemaImporter.cs - Implementation of the
 *			"System.Xml.Serialization.SoapSchemaImporter" class.
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
public class SoapSchemaImporter
{
	[TODO]
	public SoapSchemaImporter
				(XmlSchemas schemas)
			: base()
			{
				// TODO
				throw new NotImplementedException(".ctor");
			}

	[TODO]
	public SoapSchemaImporter
				(XmlSchemas schemas, CodeIdentifiers typeIdentifiers)
			: base()
			{
				// TODO
				throw new NotImplementedException(".ctor");
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
				(String name, String ns, SoapSchemaMember member)
			{
				// TODO
				throw new NotImplementedException("ImportMembersMapping");
			}

	[TODO]
	public XmlMembersMapping ImportMembersMapping
				(String name, String ns, SoapSchemaMember[] members)
			{
				// TODO
				throw new NotImplementedException("ImportMembersMapping");
			}

	[TODO]
	public XmlMembersMapping ImportMembersMapping
				(String name, String ns, SoapSchemaMember[] members,
				 bool hasWrapperElement)
			{
				// TODO
				throw new NotImplementedException("ImportMembersMapping");
			}

	[TODO]
	public XmlMembersMapping ImportMembersMapping
				(String name, String ns, SoapSchemaMember[] members,
				 bool hasWrapperElement, Type baseType,
				 bool baseTypeCanBeIndirect)
			{
				// TODO
				throw new NotImplementedException("ImportMembersMapping");
			}

}; // class SoapSchemaImporter

#endif // !ECMA_COMPAT

}; // namespace System.Xml.Serialization
