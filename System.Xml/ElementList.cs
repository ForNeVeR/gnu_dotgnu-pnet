/*
 * ElementList.cs - Implementation of the "System.Xml.ElementList" class.
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

internal class ElementList : XmlNodeList
{
	// Internal state.
	private XmlNode parent;
	private XmlDocument doc;
	private String name;
	private String namespaceURI;
	private String matchAll;
	private bool uriForm;
	private bool docChanged;
	private XmlNode lastItem;
	private int lastItemAt;

	// Create a new element list.
	private ElementList(XmlNode parent)
			{
				this.parent = parent;
				this.doc = parent.OwnerDocument;
				this.matchAll = doc.NameTable.Add("*");
				this.docChanged = false;
				this.lastItem = null;
				this.lastItemAt = -1;
				this.doc.NodeInserted +=
					new XmlNodeChangedEventHandler(DocumentChanged);
				this.doc.NodeRemoved +=
					new XmlNodeChangedEventHandler(DocumentChanged);
			}
	public ElementList(XmlNode parent, String name)
			: this(parent)
			{
				this.name = doc.NameTable.Add(name);
				this.namespaceURI = null;
				this.uriForm = false;
			}
	public ElementList(XmlNode parent, String localName,
					   String namespaceURI)
			: this(parent)
			{
				this.name =
					(localName != null ? doc.NameTable.Add(localName) : null);
				this.namespaceURI =
					(namespaceURI != null
						? doc.NameTable.Add(namespaceURI) : null);
				this.uriForm = true;
			}

	// Track changes to the document that may affect the search order.
	private void DocumentChanged(Object sender, XmlNodeChangedEventArgs args)
			{
				docChanged = true;
			}

	// Get the node that follows another in pre-order traversal.
	private XmlNode GetFollowingNode(XmlNode node)
			{
				XmlNode current = node.FirstChild;
				if(current == null)
				{
					// We don't have any children, so look for a next sibling.
					current = node;
					while(current != null && current != parent &&
					      current.NextSibling == null)
					{
						current = current.ParentNode;
					}
					if(current != null && current != parent)
					{
						current = current.NextSibling;
					}
				}
				if(current == parent)
				{
					// We've finished the traversal.
					return null;
				}
				else
				{
					// This is the next node in sequence.
					return current;
				}
			}

	// Determine if a node matches the selection criteria.
	private bool NodeMatches(XmlNode node)
			{
				if(node.NodeType != XmlNodeType.Element)
				{
					return false;
				}
				if(!uriForm)
				{
					if(((Object)name) == ((Object)matchAll) ||
					   ((Object)name) == ((Object)(node.Name)))
					{
						return true;
					}
				}
				else
				{
					if(((Object)name) == ((Object)matchAll) ||
					   ((Object)name) == ((Object)(node.LocalName)))
					{
						if(((Object)namespaceURI) == ((Object)matchAll) ||
						   ((Object)namespaceURI) ==
						   		((Object)(node.NamespaceURI)))
						{
							return true;
						}
					}
				}
				return false;
			}

	// Get the number of entries in the node list.
	public override int Count
			{
				get
				{
					int count = 0;
					XmlNode current = parent;
					while((current = GetFollowingNode(current)) != null)
					{
						if(NodeMatches(current))
						{
							++count;
						}
					}
					return count;
				}
			}

	// Get a particular item within this node list.
	[TODO]
	public override XmlNode Item(int i)
			{
				// TODO
				return null;
			}

	// Implement the "IEnumerable" interface.
	[TODO]
	public override IEnumerator GetEnumerator()
			{
				// TODO
				return null;
			}

}; // class ElementList

}; // namespace System.Xml
