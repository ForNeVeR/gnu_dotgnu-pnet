/*
 * XmlNode.cs - Implementation of the "System.Xml.XmlNode" class.
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
using System.Collections;
using System.Text;
using System.Xml.XPath;

#if ECMA_COMPAT
internal
#else
public
#endif
abstract class XmlNode : ICloneable, IEnumerable, IXPathNavigable
{
	// Internal state.
	internal XmlNode  parent;
	internal NodeList list;
	internal int depth;

	// Constructor.  Only accessible to internal subclasses.
	protected internal XmlNode(XmlNode parent)
			{
				this.parent = parent;
				this.list = null;		// Created on demand to save memory.
				if(parent != null)
				{
					depth = parent.depth + 1;
				}
				else
				{
					depth = 1;
				}
			}

	// Get a collection that contains all of the attributes for this node.
	public virtual XmlAttributeCollection Attributes
			{
				get
				{
					return null;
				}
			}

	// Get the attribute collection for node types that don't override
	// "Attributes" according to the specification.
	internal virtual XmlAttributeCollection AttributesInternal
			{
				get
				{
					return Attributes;
				}
			}

	// Get the base URI for this document.
	public virtual String BaseURI
			{
				get
				{
					if(parent != null)
					{
						return parent.BaseURI;
					}
					else
					{
						return String.Empty;
					}
				}
			}

	// Determine if this node type can contain child nodes.
	internal virtual bool CanHaveChildren
			{
				get
				{
					return false;
				}
			}

	// Get the children of this node.
	public virtual XmlNodeList ChildNodes
			{
				get
				{
					return NodeList.GetList(this);
				}
			}

	// Get the first child of this node.
	public virtual XmlNode FirstChild
			{
				get
				{
					return NodeList.GetFirstChild(this);
				}
			}

	// Determine if this node has child nodes.
	public virtual bool HasChildNodes
			{
				get
				{
					return (NodeList.GetFirstChild(this) != null);
				}
			}

	// Collect the inner text versions of a node and all of its children.
	private void CollectInner(StringBuilder builder)
			{
				XmlNode current = NodeList.GetFirstChild(this);
				while(current != null)
				{
					if(NodeList.GetFirstChild(current) == null)
					{
						switch(current.NodeType)
						{
							case XmlNodeType.Text:
							case XmlNodeType.CDATA:
							case XmlNodeType.SignificantWhitespace:
							case XmlNodeType.Whitespace:
							{
								builder.Append(current.InnerText);
							}
							break;

							default: break;
						}
					}
					else
					{
						current.CollectInner(builder);
					}
					current = NodeList.GetNextSibling(this);
				}
			}

	// Get the inner text version of this node.
	public virtual String InnerText
			{
				get
				{
					XmlNode child = NodeList.GetFirstChild(this);
					XmlNode next;
					if(child == null)
					{
						return String.Empty;
					}
					next = NodeList.GetNextSibling(child);
					if(next == null)
					{
						// Special-case the case of a single text child.
						switch(child.NodeType)
						{
							case XmlNodeType.Text:
							case XmlNodeType.CDATA:
							case XmlNodeType.SignificantWhitespace:
							case XmlNodeType.Whitespace:
							{
								return child.Value;
							}
							// Not reached

							default: break;
						}
					}
					StringBuilder builder = new StringBuilder();
					CollectInner(builder);
					return builder.ToString();
				}
				set
				{
					XmlNode child = NodeList.GetFirstChild(this);
					if(child != null && NodeList.GetNextSibling(child) == null)
					{
						// Special-case the case of a single text child.
						switch(child.NodeType)
						{
							case XmlNodeType.Text:
							case XmlNodeType.CDATA:
							case XmlNodeType.SignificantWhitespace:
							case XmlNodeType.Whitespace:
							{
								child.Value = value;
								return;
							}
							// Not reached

							default: break;
						}
					}
					RemoveAll();
					AppendChild(OwnerDocument.CreateTextNode(value));
				}
			}

	// Get the markup that represents the children of this node.
	public virtual String InnerXml
			{
				get
				{
					// TODO
					return null;
				}
				set
				{
					throw new InvalidOperationException
						(S._("Xml_CannotSetInnerXml"));
				}
			}

	// Determine if this node is read-only.
	public virtual bool IsReadOnly
			{
				get
				{
					if(parent != null)
					{
						return parent.IsReadOnly;
					}
					else
					{
						return false;
					}
				}
			}

	// Get the first child element with a specified name.
	public virtual XmlElement this[String name]
			{
				get
				{
					if(list == null)
					{
						return null;
					}
					foreach(XmlNode node in list)
					{
						if(node is XmlElement)
						{
							if(node.Name == name)
							{
								return (XmlElement)node;
							}
						}
					}
					return null;
				}
			}

	// Get the first child element with a specified name and namespace.
	public virtual XmlElement this[String localname, String ns]
			{
				get
				{
					if(list == null)
					{
						return null;
					}
					foreach(XmlNode node in list)
					{
						if(node is XmlElement)
						{
							if(node.LocalName == localname &&
							   node.NamespaceURI == ns)
							{
								return (XmlElement)node;
							}
						}
					}
					return null;
				}
			}

	// Get the last child of this node.
	public virtual XmlNode LastChild
			{
				get
				{
					return NodeList.GetLastChild(this);
				}
			}

	// Get the local name associated with this node.
	public abstract String LocalName { get; }

	// Get the name associated with this node.
	public abstract String Name { get; }

	// Get the namespace URI associated with this node.
	public virtual String NamespaceURI
			{
				get
				{
					return String.Empty;
				}
			}

	// Get the next node immediately following this one.
	public virtual XmlNode NextSibling
			{
				get
				{
					return null;
				}
			}

	// Get the type that is associated with this node.
	public abstract XmlNodeType NodeType { get; }

	// Get the XML markup representing this node and all of its children.
	public virtual String OuterXml
			{
				get
				{
					// TODO
					return null;
				}
			}

	// Get the document that owns this node.
	public virtual XmlDocument OwnerDocument
			{
				get
				{
					if(parent != null)
					{
						if(parent is XmlDocument)
						{
							return (XmlDocument)parent;
						}
						else
						{
							return parent.OwnerDocument;
						}
					}
					else
					{
						return null;
					}
				}
			}

	// Get the parent of this node.
	public virtual XmlNode ParentNode
			{
				get
				{
					if(parent != null && !parent.IsPlaceholder)
					{
						return parent;
					}
					else
					{
						return null;
					}
				}
			}

	// Get the prefix associated with this node.
	public virtual String Prefix
			{
				get
				{
					return String.Empty;
				}
			}

	// Get the previous sibling of this node.
	public virtual XmlNode PreviousSibling
			{
				get
				{
					return null;
				}
			}

	// Get or set the value associated with this node.
	public virtual String Value
			{
				get
				{
					return String.Empty;
				}
				set
				{
					throw new InvalidOperationException
						(S._("Xml_CannotSetValue"));
				}
			}

	// Determine if one node is an ancestor of another.
	private static bool IsAncestorOf(XmlNode node1, XmlNode node2)
			{
				while(node2 != null)
				{
					if(node2 == node1)
					{
						return true;
					}
					node2 = node2.parent;
				}
				return false;
			}

	// Append a new child to this node.
	public virtual XmlNode AppendChild(XmlNode newChild)
			{
				XmlDocument doc;
				if(!CanHaveChildren)
				{
					throw new InvalidOperationException
						(S._("Xml_CannotHaveChildren"));
				}
				if(IsAncestorOf(newChild, this))
				{
					throw new InvalidOperationException(S._("Xml_IsAncestor"));
				}
				if(this is XmlDocument)
				{
					doc = (XmlDocument)this;
				}
				else
				{
					doc = OwnerDocument;
				}
				if(newChild.OwnerDocument != doc)
				{
					throw new ArgumentException
						(S._("Xml_NotSameDocument"), "newChild");
				}
				if(IsReadOnly)
				{
					throw new ArgumentException(S._("Xml_ReadOnly"));
				}
				// TODO: remove from original position and add to the new.
				return newChild;
			}

	// Implement the ICloneable interface.
	Object ICloneable.Clone()
			{
				return CloneNode(true);
			}

	// Clone this node.
	public virtual XmlNode Clone()
			{
				return CloneNode(true);
			}

	// Clone this node in either shallow or deep mode.
	public abstract XmlNode CloneNode(bool deep);

	// Implement the IXPathNavigator interface.
	public XPathNavigator CreateNavigator()
			{
				// TODO
				return null;
			}

	// Implement the IEnumerable interface,
	public IEnumerator GetEnumerator()
			{
				return (NodeList.GetList(this)).GetEnumerator();
			}

	// Get the namespace that corresponds to a particular prefix.
	public virtual String GetNamespaceOfPrefix(String prefix)
			{
				// TODO
				return null;
			}

	// Get the prefix that corresponds to a particular namespace.
	public virtual String GetPrefixOfNamespace(String prefix)
			{
				// TODO
				return null;
			}

	// Insert a new child under this node just after a reference child.
	public virtual XmlNode InsertAfter(XmlNode newChild, XmlNode refChild)
			{
				// TODO
				return newChild;
			}

	// Insert a new child under this node just before a reference child.
	public virtual XmlNode InsertBefore(XmlNode newChild, XmlNode refChild)
			{
				// TODO
				return newChild;
			}

	// Normalize the text nodes underneath this node.
	public virtual void Normalize()
			{
				// TODO
			}

	// Prepend a specific child at the start of this node's child list.
	public virtual XmlNode PrependChild(XmlNode newChild)
			{
				// TODO
				return newChild;
			}

	// Remove all children and attributes from this node.
	public virtual void RemoveAll()
			{
				// TODO
			}

	// Remove a child from this node.
	public virtual XmlNode RemoveChild(XmlNode oldChild)
			{
				// TODO
				return oldChild;
			}

	// Replace a child of this node.
	public virtual XmlNode ReplaceChild(XmlNode newChild, XmlNode oldChild)
			{
				// TODO
				return oldChild;
			}

	// Select a list of nodes matching a particular XPath expression.
	public XmlNodeList SelectNodes(String xpath)
			{
				// TODO
				return null;
			}

	// Select a list of nodes matching a particular XPath expression.
	public XmlNodeList SelectNodes(String xpath, XmlNamespaceManager nsmgr)
			{
				// TODO
				return null;
			}

	// Select a single node matching a particular XPath expression.
	public XmlNode SelectSingleNode(String xpath)
			{
				// TODO
				return null;
			}

	// Select a single node matching a particular XPath expression.
	public XmlNode SelectSingleNode(String xpath, XmlNamespaceManager nsmgr)
			{
				// TODO
				return null;
			}

	// Test if this implementation supports a particular DOM feature.
	public virtual bool Supports(String feature, String version)
			{
				if(feature == "XML" &&
				   (version == "1.0" || version == "2.0"))
				{
					return true;
				}
				else
				{
					return false;
				}
			}

	// Writes the contents of this node to a specified XmlWriter.
	public abstract void WriteContentTo(XmlWriter w);

	// Write this node and all of its contents to a specified XmlWriter.
	public abstract void WriteTo(XmlWriter w);

	// Clone the children from another node into this node.
	internal void CloneChildrenFrom(XmlNode other, bool deep)
			{
				// TODO
			}

	// Determine if this node is a placeholder fragment for nodes that have
	// not yet been added to the main document.
	internal virtual bool IsPlaceholder
			{
				get
				{
					return false;
				}
			}

	// Quickly find the document that owns a node, without recursion.
	internal XmlDocument FindOwnerQuick()
			{
				XmlNode node = this;
				while(node != null)
				{
					if(node is XmlDocument)
					{
						return (XmlDocument)node;
					}
					node = node.parent;
				}
				return null;
			}

	// Emit an event just before a change.  Returns an argument
	// block if an after event will also need to be emitted.
	internal XmlNodeChangedEventArgs EmitBefore
				(XmlNodeChangedAction action,
			     XmlNode oldParent, XmlNode newParent)
			{
				XmlDocument doc = FindOwnerQuick();
				if(doc != null)
				{
					return doc.EmitBefore(action, this, oldParent, newParent);
				}
				else
				{
					return null;
				}
			}
	internal XmlNodeChangedEventArgs EmitBefore(XmlNodeChangedAction action)
			{
				XmlDocument doc = FindOwnerQuick();
				if(doc != null)
				{
					return doc.EmitBefore(action, this, parent, parent);
				}
				else
				{
					return null;
				}
			}

	// Emit an event just after a change.
	internal void EmitAfter(XmlNodeChangedEventArgs args)
			{
				if(args != null)
				{
					FindOwnerQuick().EmitAfter(args);
				}
			}

}; // class XmlNode

}; // namespace System.Xml
