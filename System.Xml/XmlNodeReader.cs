/*
 * XmlNodeReader.cs - Implementation of the "System.Xml.XmlNodeReader" class.
 *
 * Copyright (C) 2002 Southern Storm Software, Pty Ltd.
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
 
namespace System.Xml
{

#if !ECMA_COMPAT

using System;
using System.IO;
using System.Text;

public class XmlNodeReader : XmlReader
{
	// Internal state.
	private XmlNode startNode;
	private XmlNode currentNode;
	private XmlDocument doc;
	private ReadState readState;

	// Constructor.
	public XmlNodeReader(XmlNode node)
			{
				startNode = currentNode = node;
				doc = node.OwnerDocument;
			}

	// Clean up the resources that were used by this XML reader.
	public override void Close()
			{
				readState = ReadState.Closed;
			}

	// Returns the value of an attribute with a specific index.
	public override String GetAttribute(int i)
			{
				// TODO
				return null;
			}

	// Returns the value of an attribute with a specific name.
	public override String GetAttribute(String name, String namespaceURI)
			{
				// TODO
				return null;
			}

	// Returns the value of an attribute with a specific qualified name.
	public override String GetAttribute(String name)
			{
				// TODO
				return null;
			}

	// Resolve a namespace in the scope of the current element.
	public override String LookupNamespace(String prefix)
			{
				// TODO
				return null;
			}

	// Move the current position to a particular attribute.
	public override void MoveToAttribute(int i)
			{
				// TODO
			}

	// Move the current position to an attribute with a particular name.
	public override bool MoveToAttribute(String name, String ns)
			{
				// TODO
				return false;
			}

	// Move the current position to an attribute with a qualified name.
	public override bool MoveToAttribute(String name)
			{
				// TODO
				return false;
			}

	// Move to the element that owns the current attribute.
	public override bool MoveToElement()
			{
				// TODO
				return false;
			}

	// Move to the first attribute owned by the current element.
	public override bool MoveToFirstAttribute()
			{
				// TODO
				return false;
			}

	// Move to the next attribute owned by the current element.
	public override bool MoveToNextAttribute()
			{
				// TODO
				return false;
			}

	// Read the next node in the input stream.
	public override bool Read()
			{
				// TODO
				return false;
			}

	// Read the next attribute value in the input stream.
	public override bool ReadAttributeValue()
			{
				// TODO
				return false;
			}

	// Read the contents of the current node, including all markup.
	public override String ReadInnerXml()
			{
				// TODO: skip
				return currentNode.InnerXml;
			}

	// Read the current node, including all markup.
	public override String ReadOuterXml()
			{
				// TODO: skip
				return currentNode.OuterXml;
			}

	// Read the contents of an element or text node as a string.
	public override String ReadString()
			{
				// TODO
				return null;
			}

	// Resolve an entity reference.
	public override void ResolveEntity()
			{
				// TODO
			}

	// Skip the current element in the input.
	public override void Skip()
			{
				// TODO
			}

	// Get the number of attributes on the current node.
	public override int AttributeCount
			{
				get
				{
					// TODO
					return 0;
				}
			}

	// Get the base URI for the current node.
	public override String BaseURI
			{
				get
				{
					return currentNode.BaseURI;
				}
			}

	// Determine if this reader can parse and resolve entities.
	public override bool CanResolveEntity
			{
				get
				{
					return true;
				}
			}

	// Get the depth of the current node.
	public override int Depth
			{
				get
				{
					// TODO
					return 0;
				}
			}

	// Determine if we have reached the end of the input stream.
	public override bool EOF
			{
				get
				{
					// TODO
					return false;
				}
			}

	// Determine if the current node can have an associated text value.
	public override bool HasValue
			{
				get
				{
					// TODO
					return false;
				}
			}

	// Determine if the current node's value was generated from a DTD default.
	public override bool IsDefault
			{
				get
				{
					// TODO
					return false;
				}
			}

	// Determine if the current node is an empty element.
	public override bool IsEmptyElement
			{
				get
				{
					// TODO
					return false;
				}
			}

	// Retrieve an attribute value with a specified index.
	public override String this[int i]
			{
				get
				{
					return GetAttribute(i);
				}
			}

	// Retrieve an attribute value with a specified name.
	public override String this[String localname, String namespaceURI]
			{
				get
				{
					return GetAttribute(localname, namespaceURI);
				}
			}

	// Retrieve an attribute value with a specified qualified name.
	public override String this[String name]
			{
				get
				{
					return GetAttribute(name);
				}
			}

	// Get the local name of the current node.
	public override String LocalName
			{
				get
				{
					return currentNode.LocalName;
				}
			}

	// Get the fully-qualified name of the current node.
	public override String Name
			{
				get
				{
					return currentNode.Name;
				}
			}

	// Get the name that that is used to look up and resolve names.
	public override XmlNameTable NameTable
			{
				get
				{
					return doc.NameTable;
				}
			}

	// Get the namespace URI associated with the current node.
	public override String NamespaceURI
			{
				get
				{
					return currentNode.NamespaceURI;
				}
			}

	// Get the type of the current node.
	public override XmlNodeType NodeType
			{
				get
				{
					return currentNode.NodeType;
				}
			}

	// Get the namespace prefix associated with the current node.
	public override String Prefix
			{
				get
				{
					return currentNode.Prefix;
				}
			}

	// Get the quote character that was used to enclose an attribute value.
	public override char QuoteChar
			{
				get
				{
					return '"';
				}
			}

	public override ReadState ReadState
			{
				get
				{
					return readState;
				}
			}

	// Get the text value of the current node.
	public override String Value
			{
				get
				{
					return currentNode.Value;
				}
			}

	// Get the current xml:lang scope.
	public override String XmlLang
			{
				get
				{
					// TODO
					return null;
				}
			}

	// Get the current xml:space scope.
	public override XmlSpace XmlSpace
			{
				get
				{
					// TODO
					return XmlSpace.None;
				}
			}

}; // class XmlNodeReader

#endif // !ECMA_COMPAT

}; // namespace System.Xml
