/*
 * XmlDocument.cs - Implementation of the "System.Xml.XmlDocument" class.
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
class XmlDocument : XmlNode
{
	// Internal state.
	private String prefix;
	private String localName;
	private String ns;
	private String name;

	// Constructor.
	internal XmlDocument(XmlNode owner, String prefix,
					    String localName, String ns)
			: base(owner)
			{
				this.prefix = prefix;
				this.localName =
					((localName != null) ? localName : String.Empty);
				this.ns = ((ns != null) ? ns : String.Empty);
				if(prefix != null)
				{
					name = prefix + ":" + localName;
				}
				else
				{
					name = localName;
					prefix = String.Empty;
				}
			}

	// Get the base URI for this document.
	public override String BaseURI
			{
				get
				{
					return parent.BaseURI;
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

	// Get the local name associated with this node.
	public override String LocalName
			{
				get
				{
					return localName;
				}
			}

	// Get the name associated with this node.
	public override String Name
			{
				get
				{
					return name;
				}
			}

	// Get the namespace URI associated with this node.
	public override String NamespaceURI
			{
				get
				{
					return ns;
				}
			}

	// Get the type that is associated with this node.
	public override XmlNodeType NodeType
			{
				get
				{
					return XmlNodeType.Attribute;
				}
			}

	// Get the document that owns this node.
	public override XmlDocument OwnerDocument
			{
				get
				{
					XmlElement owner = OwnerElement;
					if(owner != null)
					{
						return owner.OwnerDocument;
					}
					else
					{
						return null;
					}
				}
			}

	// Get the element that owns this attribute.
	public virtual XmlElement OwnerElement
			{
				get
				{
					return (XmlElement)parent;
				}
			}

	// Get the parent of this node.
	public override XmlNode ParentNode
			{
				get
				{
					return OwnerElement;
				}
			}

	// Get the prefix associated with this node.
	public override String Prefix
			{
				get
				{
					return prefix;
				}
			}

	// Determine if the attribute value was explictly specified.
	public virtual bool Specified
			{
				get
				{
					// TODO
					return true;
				}
			}

	// Get or set the value associated with this node.
	public override String Value
			{
				get
				{
					return null;
				}
				set
				{
					// TODO
				}
			}

	// Clone this node in either shallow or deep mode.
	public override XmlNode CloneNode(bool deep)
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

}; // class XmlDocument

}; // namespace System.Xml
