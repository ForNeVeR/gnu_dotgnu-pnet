/*
 * XmlSerializer.cs - Implementation of the
 *			"System.Xml.Serialization.XmlSerializer" class.
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
using System.IO;

[TODO]
public class XmlSerializer
{
	[TODO]
	protected XmlSerializer()
			: base()
			{
				// TODO
				throw new NotImplementedException(".ctor");
			}

	[TODO]
	public XmlSerializer
				(Type type)
			: base()
			{
				// TODO
				throw new NotImplementedException(".ctor");
			}

	[TODO]
	public XmlSerializer
				(XmlTypeMapping xmlTypeMapping)
			: base()
			{
				// TODO
				throw new NotImplementedException(".ctor");
			}

	[TODO]
	public XmlSerializer
				(Type type, String defaultNamespace)
			: base()
			{
				// TODO
				throw new NotImplementedException(".ctor");
			}

	[TODO]
	public XmlSerializer
				(Type type, Type[] extraTypes)
			: base()
			{
				// TODO
				throw new NotImplementedException(".ctor");
			}

	[TODO]
	public XmlSerializer
				(Type type, XmlAttributeOverrides overrides)
			: base()
			{
				// TODO
				throw new NotImplementedException(".ctor");
			}

	[TODO]
	public XmlSerializer
				(Type type, XmlRootAttribute root)
			: base()
			{
				// TODO
				throw new NotImplementedException(".ctor");
			}

	[TODO]
	public XmlSerializer
				(Type type, XmlAttributeOverrides overrides,
				 Type[] extraTypes, XmlRootAttribute root,
				 String defaultNamespace)
			: base()
			{
				// TODO
				throw new NotImplementedException(".ctor");
			}

	[TODO]
	public virtual bool CanDeserialize
				(XmlReader xmlReader)
			{
				// TODO
				throw new NotImplementedException("CanDeserialize");
			}

	[TODO]
	protected virtual XmlSerializationReader CreateReader()
			{
				// TODO
				throw new NotImplementedException("CreateReader");
			}

	[TODO]
	protected virtual XmlSerializationWriter CreateWriter()
			{
				// TODO
				throw new NotImplementedException("CreateWriter");
			}

	[TODO]
	public Object Deserialize
				(Stream stream)
			{
				// TODO
				throw new NotImplementedException("Deserialize");
			}

	[TODO]
	public Object Deserialize
				(TextReader textReader)
			{
				// TODO
				throw new NotImplementedException("Deserialize");
			}

	[TODO]
	public Object Deserialize
				(XmlReader xmlReader)
			{
				// TODO
				throw new NotImplementedException("Deserialize");
			}

	[TODO]
	protected virtual Object Deserialize
				(XmlSerializationReader reader)
			{
				// TODO
				throw new NotImplementedException("Deserialize");
			}

	[TODO]
	public static XmlSerializer[] FromMappings
				(XmlMapping[] mappings)
			{
				// TODO
				throw new NotImplementedException("FromMappings");
			}

	[TODO]
	public static XmlSerializer[] FromTypes
				(Type[] types)
			{
				// TODO
				throw new NotImplementedException("FromTypes");
			}

	[TODO]
	protected virtual void Serialize
				(Object o, XmlSerializationWriter writer)
			{
				// TODO
				throw new NotImplementedException("Serialize");
			}

	[TODO]
	public void Serialize
				(Stream stream, Object o)
			{
				// TODO
				throw new NotImplementedException("Serialize");
			}

	[TODO]
	public void Serialize
				(TextWriter textWriter, Object o)
			{
				// TODO
				throw new NotImplementedException("Serialize");
			}

	[TODO]
	public void Serialize
				(XmlWriter xmlWriter, Object o)
			{
				// TODO
				throw new NotImplementedException("Serialize");
			}

	[TODO]
	public void Serialize
				(Stream stream, Object o,
				 XmlSerializerNamespaces namespaces)
			{
				// TODO
				throw new NotImplementedException("Serialize");
			}

	[TODO]
	public void Serialize
				(TextWriter textWriter, Object o,
				 XmlSerializerNamespaces namespaces)
			{
				// TODO
				throw new NotImplementedException("Serialize");
			}

	[TODO]
	public void Serialize
				(XmlWriter xmlWriter, Object o,
				 XmlSerializerNamespaces namespaces)
			{
				// TODO
				throw new NotImplementedException("Serialize");
			}


	// Events.
	public event XmlAttributeEventHandler UnknownAttribute;
	public event XmlElementEventHandler UnknownElement;
	public event XmlNodeEventHandler UnknownNode;
	public event UnreferencedObjectEventHandler UnreferencedObject;

}; // class XmlSerializer

#endif // !ECMA_COMPAT

}; // namespace System.Xml.Serialization
