/*
 * XmlElement.cs - Implementation of the "System.Xml.XmlElement" class.
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

using System;

#if ECMA_COMPAT
internal
#else
public
#endif
class XmlElement : XmlLinkedNode
{
	// Internal state.
	private NameCache.NameInfo name;
	private XmlAttributeCollection attributes;
	private bool isEmpty;

	// Constructor.
	internal XmlElement(XmlNode parent, NameCache.NameInfo name)
			: base(parent)
			{
				this.name = name;
				this.attributes = new XmlAttributeCollection(this);
				this.isEmpty = true;
			}

	// Get a collection that contains all of the attributes for this node.
	public override XmlAttributeCollection Attributes
			{
				get
				{
					return attributes;
				}
			}

	// Determine if this element node has attributes.
	public bool HasAttributes
			{
				get
				{
					return (attributes.Count != 0);
				}
			}

	// Get the inner text version of this node.
	public override String InnerText
			{
				get
				{
					// TODO
					return null;
				}
				set
				{
					// TODO
				}
			}

	// Get the markup that represents the children of this node.
	public override String InnerXml
			{
				get
				{
					// TODO
					return null;
				}
				set
				{
					// TODO
				}
			}

	// Get or set the format of this element.
	public bool IsEmpty
			{
				get
				{
					return isEmpty;
				}
				set
				{
					isEmpty = value;
				}
			}

	// Get the local name associated with this node.
	public override String LocalName
			{
				get
				{
					return name.localName;
				}
			}

	// Get the name associated with this node.
	public override String Name
			{
				get
				{
					return name.name;
				}
			}

	// Get the namespace URI associated with this node.
	public override String NamespaceURI
			{
				get
				{
					return name.ns;
				}
			}

	// Get the next node immediately following this one.
	public override XmlNode NextSibling
			{
				get
				{
					return NodeList.GetNextSibling(this);
				}
			}

	// Get the type that is associated with this node.
	public override XmlNodeType NodeType
			{
				get
				{
					return XmlNodeType.Element;
				}
			}

	// Get the document that owns this node.
	public override XmlDocument OwnerDocument
			{
				get
				{
					XmlNode parent = ParentNode;
					if(parent != null)
					{
						return parent.OwnerDocument;
					}
					else
					{
						return null;
					}
				}
			}

	// Get the previous node immediately preceding this one.
	public override XmlNode PreviousSibling
			{
				get
				{
					return NodeList.GetPreviousSibling(this);
				}
			}

	// Get the prefix associated with this node.
	public override String Prefix
			{
				get
				{
					return name.prefix;
				}
			}

	// Clone this node in either shallow or deep mode.
	public override XmlNode CloneNode(bool deep)
			{
				// TODO
				return null;
			}

	// Get the value of an attribute with a specific name.
	public virtual String GetAttribute(String name)
			{
				// TODO
				return null;
			}

	// Get the value of an attribute with a specific name and namespace.
	public virtual String GetAttribute(String localName, String namespaceURI)
			{
				// TODO
				return null;
			}

	// Get the node of an attribute with a specific name.
	public virtual XmlAttribute GetAttributeNode(String name)
			{
				// TODO
				return null;
			}

	// Get the node of an attribute with a specific name and namespace.
	public virtual XmlAttribute GetAttributeNode
				(String localName, String namespaceURI)
			{
				// TODO
				return null;
			}

	// Get a list of all descendents that match a particular name.
	public virtual XmlNodeList GetElementsByTagName(String name)
			{
				// TODO
				return null;
			}

	// Get a list of all descendents that match a particular name and namespace.
	public virtual XmlNodeList GetElementsByTagName
				(String localName, String namespaceURI)
			{
				// TODO
				return null;
			}

	// Determine if this element has a particular attribute.
	public virtual bool HasAttribute(String name)
			{
				// TODO
				return false;
			}

	// Determine if this element has a particular attribute.
	public virtual bool HasAttribute(String localName, String namespaceURI)
			{
				// TODO
				return false;
			}

	// Remove all children and attributes from this node.
	public override void RemoveAll()
			{
				RemoveAllAttributes();
				base.RemoveAll();
			}

	// Remove all of the attributes from this node.
	public virtual void RemoveAllAttributes()
			{
				// TODO
			}

	// Remove a specified attribute by name.
	public virtual void RemoveAttribute(String name)
			{
				// TODO
			}

	// Remove a specified attribute by name and namespace.
	public virtual void RemoveAttribute(String localName, String namespaceURI)
			{
				// TODO
			}

	// Remove a specified attribute by index.
	public virtual XmlNode RemoveAttributeAt(int i)
			{
				// TODO
				return null;
			}

	// Remove a particular attribute node and return the node.
	public virtual XmlAttribute RemoveAttributeNode(XmlAttribute oldAttr)
			{
				// TODO
				return oldAttr;
			}

	// Remove a particular attribute by name and return the node.
	public virtual XmlAttribute RemoveAttributeNode
				(String localName, String namespaceURI)
			{
				// TODO
				return null;
			}

	// Set an attribute to a specific value.
	public virtual void SetAttribute(String name, String value)
			{
				// TODO
			}

	// Set an attribute to a specific value.
	public virtual void SetAttribute(String localName, String namespaceURI,
									 String value)
			{
				// TODO
			}

	// Set an attribute by node.
	public virtual XmlAttribute SetAttributeNode(XmlAttribute newAttr)
			{
				// TODO
				return null;
			}

	// Create a new attribute node and return it.
	public virtual XmlAttribute SetAttributeNode
				(String localName, String namespaceURI)
			{
				// TODO
				return null;
			}

	// Writes the contents of this node to a specified XmlWriter.
	public override void WriteContentTo(XmlWriter w)
			{
				// TODO
			}

	// Write this node and all of its contents to a specified XmlWriter.
	public override void WriteTo(XmlWriter w)
			{
				// TODO
			}

	// Determine if a particular node type can be inserted as
	// a child of the current node.
	internal override bool CanInsert(XmlNodeType type)
			{
				switch(type)
				{
					case XmlNodeType.Element:
					case XmlNodeType.Text:
					case XmlNodeType.CDATA:
					case XmlNodeType.EntityReference:
					case XmlNodeType.ProcessingInstruction:
					case XmlNodeType.Comment:
					case XmlNodeType.Whitespace:
					case XmlNodeType.SignificantWhitespace:
					{
						return true;
					}
					// Not reached.

					default: break;
				}
				return false;
			}

}; // class XmlElement

}; // namespace System.Xml
