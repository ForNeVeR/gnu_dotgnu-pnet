/*
 * XmlTypeMapping.cs - Implementation of the
 *			"System.Xml.Serialization.XmlTypeMapping" class.
 *
 * Copyright (C) 2003, 2004  Free Software Foundation, Inc.
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

public class XmlTypeMapping : XmlMapping
{
	// Internal state.
	private String elementName;
	private String ns;
	private String typeFullName;
	private String typeName;

	// Constructor.
	internal XmlTypeMapping
				(String elementName, String ns, String typeFullName,
				 String typeName)
			: base()
			{
				this.elementName = elementName;
				this.ns = ns;
				this.typeFullName = typeFullName;
				this.typeName = typeName;
			}


	// Get the element name.
	public String ElementName
			{
				get { return elementName; }
			}

	// Get the namespace.
	public String Namespace
			{
				get { return ns; }
			}

	// Get the full type name.
	public String TypeFullName
			{
				get { return typeFullName; }
			}

	// Get the type name.
	public String TypeName
			{
				get { return typeName; }
			}

}; // class XmlTypeMapping

#endif // !ECMA_COMPAT

}; // namespace System.Xml.Serialization
