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
	private XmlImplementation implementation;
	private String baseURI;
	private XmlElement root;
	private XmlDocumentType docType;
	private bool preserveWhitespace;
	private XmlResolver xmlResolver;
	private NameCache nameCache;
	internal XmlDocumentFragment placeholder;
	internal static readonly String xmlns = "http://www.w3.org/2000/xmlns/";

	// Constructors.
	public XmlDocument() : base(null)
			{
				implementation = new XmlImplementation();
				Initialize();
			}
	protected internal XmlDocument(XmlImplementation imp) : base(null)
			{
				if(imp != null)
				{
					implementation = imp;
				}
				else
				{
					implementation = new XmlImplementation();
				}
				Initialize();
			}
	public XmlDocument(XmlNameTable nt) : base(null)
			{
				if(nt != null)
				{
					implementation = new XmlImplementation(nt);
				}
				else
				{
					implementation = new XmlImplementation();
				}
				Initialize();
			}

	// Initialize the document.
	private void Initialize()
			{
				baseURI = String.Empty;
				root = null;
				docType = null;
				preserveWhitespace = false;
				placeholder = new XmlDocumentFragment(this);
				nameCache = new NameCache(implementation.nameTable);
			}

	// Get the base URI for the document.
	public override String BaseURI
			{
				get
				{
					return baseURI;
				}
			}

	// Get the root element for the document.
	public XmlElement DocumentElement
			{
				get
				{
					return root;
				}
			}

	// Get the document type declaration.
	public virtual XmlDocumentType DocumentType
			{
				get
				{
					return docType;
				}
			}

	// Get the implementation associated with this document.
	public XmlImplementation Implementation
			{
				get
				{
					return implementation;
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

	// Determine if this document is read-only.
	public override bool IsReadOnly
			{
				get
				{
					return false;
				}
			}

	// Get the local name of this node.
	public override String LocalName
			{
				get
				{
					return "#document";
				}
			}

	// Get the name of this node.
	public override String Name
			{
				get
				{
					return "#document";
				}
			}

	// Get the name table associated with this document.
	public XmlNameTable NameTable
			{
				get
				{
					return implementation.nameTable;
				}
			}

	// Get the type of this node.
	public override XmlNodeType NodeType
			{
				get
				{
					return XmlNodeType.Document;
				}
			}

	// Get the document that owns this node.
	public override XmlDocument OwnerDocument
			{
				get
				{
					return null;
				}
			}

	// Get or set the whitespace preservation flag.
	public bool PreserveWhitespace
			{
				get
				{
					return preserveWhitespace;
				}
				set
				{
					preserveWhitespace = value;
				}
			}

	// Set the resolver to use for external resources.
	public XmlResolver XmlResolver
			{
				set
				{
					xmlResolver = value;
				}
			}

	// Clone this document node.
	[TODO]
	public override XmlNode CloneNode(bool deep)
			{
				// TODO
				return null;
			}

	// Create an attribute and associate it with this document.
	public XmlAttribute CreateAttribute(String name)
			{
				int colon = name.LastIndexOf(':');
				if(colon == -1)
				{
					return CreateAttribute(name, String.Empty, String.Empty);
				}
				else
				{
					String prefix = name.Substring(0, colon);
					String localName = name.Substring(colon + 1);
					if(prefix == "xmlns")
					{
						return CreateAttribute(localName, prefix, xmlns);
					}
					else
					{
						return CreateAttribute(localName, prefix, String.Empty);
					}
				}
			}
	public XmlAttribute CreateAttribute(String qualifiedName,
										String namespaceURI)
			{
				int colon = qualifiedName.LastIndexOf(':');
				if(colon == -1)
				{
					return CreateAttribute
						(qualifiedName, String.Empty, namespaceURI);
				}
				else
				{
					String prefix = qualifiedName.Substring(0, colon);
					String localName = qualifiedName.Substring(colon + 1);
					return CreateAttribute(localName, prefix, namespaceURI);
				}
			}
	public virtual XmlAttribute CreateAttribute
				(String localName, String prefix, String namespaceURI)
			{
				if(prefix == "xmlns" && namespaceURI != xmlns)
				{
					throw new ArgumentException
						(S._("Xml_InvalidNamespaceURI"), "namespaceURI");
				}
				NameCache.NameInfo info =
					nameCache.Add(localName, prefix, namespaceURI);
				return new XmlAttribute(placeholder, info);
			}

	// Create a document fragment that is attached to this node.
	public virtual XmlDocumentFragment CreateDocumentFragment()
			{
				return new XmlDocumentFragment(this);
			}

	// Create a text node.
	public virtual XmlNode/*TODO:XmlText*/ CreateTextNode(String text)
			{
				// TODO
				return null;
			}

	// Write the contents of this document to an XML writer.
	[TODO]
	public override void WriteContentTo(XmlWriter xw)
			{
				// TODO
			}

	// Write this document to an XML writer.
	[TODO]
	public override void WriteTo(XmlWriter w)
			{
				// TODO
			}

	// Events.
	public event XmlNodeChangedEventHandler NodeChanged;
	public event XmlNodeChangedEventHandler NodeChanging;
	public event XmlNodeChangedEventHandler NodeInserted;
	public event XmlNodeChangedEventHandler NodeInserting;
	public event XmlNodeChangedEventHandler NodeRemoved;
	public event XmlNodeChangedEventHandler NodeRemoving;

}; // class XmlDocument

}; // namespace System.Xml
