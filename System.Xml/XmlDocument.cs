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

	// Create a CDATA section.
	public virtual XmlCDataSection CreateCDataSection(String data)
			{
				return new XmlCDataSection(placeholder, data);
			}

	// Create a comment node.
	public virtual XmlComment CreateComment(String text)
			{
				return new XmlComment(placeholder, text);
			}

	// Create a document fragment that is attached to this node.
	public virtual XmlDocumentFragment CreateDocumentFragment()
			{
				return new XmlDocumentFragment(this);
			}

	// Create a document type that is attached to this node.
	public virtual XmlDocumentType CreateDocumentType
				(String name, String publicId,
				 String systemId, String internalSubset)
			{
				return new XmlDocumentType
					(placeholder, name, publicId, systemId, internalSubset);
			}

	// Create a significant whitespace node.
	public virtual XmlSignificantWhitespace CreateSignificantWhitespace
				(String text)
			{
				return new XmlSignificantWhitespace(placeholder, text);
			}

	// Create a text node.
	public virtual XmlText CreateTextNode(String text)
			{
				return new XmlText(placeholder, text);
			}

	// Create a whitespace node.
	public virtual XmlWhitespace CreateWhitespace(String text)
			{
				return new XmlWhitespace(placeholder, text);
			}

	// Create an XML declaration node.
	public virtual XmlDeclaration CreateXmlDeclaration
				(String version, String encoding, String standalone)
			{
				return new XmlDeclaration
					(placeholder, version, encoding, standalone);
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

	// Emit an event before a particular node change.  Returns an
	// argument block if "EmitAfter" may be needed, or null if not.
	internal XmlNodeChangedEventArgs EmitBefore
					(XmlNodeChangedAction action, XmlNode node,
				 	 XmlNode oldParent, XmlNode newParent)
			{
				XmlNodeChangedEventArgs args = null;
				switch(action)
				{
					case XmlNodeChangedAction.Insert:
					{
						if(NodeInserting != null)
						{
							args = new XmlNodeChangedEventArgs
								(action, node, oldParent, newParent);
							NodeInserting(this, args);
						}
						else if(NodeInserted != null)
						{
							args = new XmlNodeChangedEventArgs
								(action, node, oldParent, newParent);
						}
					}
					break;

					case XmlNodeChangedAction.Remove:
					{
						if(NodeRemoving != null)
						{
							args = new XmlNodeChangedEventArgs
								(action, node, oldParent, newParent);
							NodeRemoving(this, args);
						}
						else if(NodeRemoved != null)
						{
							args = new XmlNodeChangedEventArgs
								(action, node, oldParent, newParent);
						}
					}
					break;

					case XmlNodeChangedAction.Change:
					{
						if(NodeChanging != null)
						{
							args = new XmlNodeChangedEventArgs
								(action, node, oldParent, newParent);
							NodeChanging(this, args);
						}
						else if(NodeChanged != null)
						{
							args = new XmlNodeChangedEventArgs
								(action, node, oldParent, newParent);
						}
					}
					break;
				}
				return args;
			}

	// Emit an event after a particular node change.
	internal new void EmitAfter(XmlNodeChangedEventArgs args)
			{
				switch(args.Action)
				{
					case XmlNodeChangedAction.Insert:
					{
						if(NodeInserted != null)
						{
							NodeInserted(this, args);
						}
					}
					break;

					case XmlNodeChangedAction.Remove:
					{
						if(NodeRemoved != null)
						{
							NodeRemoved(this, args);
						}
					}
					break;

					case XmlNodeChangedAction.Change:
					{
						if(NodeChanged != null)
						{
							NodeChanged(this, args);
						}
					}
					break;
				}
			}

}; // class XmlDocument

}; // namespace System.Xml
