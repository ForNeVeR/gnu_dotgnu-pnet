/*
 * XmlDocumentNavigator.cs - Implementation of the
 *		"System.Xml.Private.XmlDocumentNavigator" class.
 *
 * Copyright (C) 2004  Southern Storm Software, Pty Ltd.
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

namespace System.Xml.Private
{

#if CONFIG_XPATH

using System;
using System.Xml;
using System.Xml.XPath;


internal class XmlDocumentNavigator : XPathNavigator, IHasXmlNode
{
	private XmlNode node;
	private XmlDocument document;

	public XmlDocumentNavigator(XmlNode node) : base()
	{
		this.node = node;
		this.document = (node is XmlDocument) ? 
							(XmlDocument)node : node.OwnerDocument;
	}

	public XmlDocumentNavigator(XmlDocumentNavigator copy)
	{
		this.node = copy.node;
		this.document = copy.document;
	}

	public override XPathNavigator Clone()
	{
		return new XmlDocumentNavigator(this);
	}

	public override String GetAttribute(String localName, String namespaceURI)
	{
		XmlAttribute attr = node.Attributes[localName, namespaceURI] ;

		if(attr != null)
		{
			return attr.Value;
		}
		return null;
	}


	public override String GetNamespace(String name)
	{
		return node.GetNamespaceOfPrefix(name);
	}
	
	public override bool IsSamePosition(XPathNavigator other)
	{
		XmlDocumentNavigator nav = (other as XmlDocumentNavigator);
		return ((nav != null) && (nav.node == node));
	}
	
	public override bool MoveTo(XPathNavigator other)
	{
		XmlDocumentNavigator nav = (other as XmlDocumentNavigator);
		if(nav != null)
		{
			node = nav.node;
			document = nav.document;
			return true;
		}
		return false;
	}

	public override bool MoveToAttribute(String localName, String namespaceURI)
	{
		if(node.Attributes != null)
		{
			foreach(XmlAttribute attr in node.Attributes)
			{
				// TODO : can this be an Object Ref compare ?
				if(attr.LocalName == localName && 
					attr.NamespaceURI == namespaceURI)
				{
					node = attr;
					return true;
				}
			}
		}
		return false;
	}

	public override bool MoveToFirst()
	{
		// TODO : is this correct ?. Will a Text qualify as a first node ?
		if(node.NodeType != XmlNodeType.Attribute && node.ParentNode != null)
		{
			node = node.ParentNode.FirstChild;
			return true;
		}
		return false;
	}

	public override bool MoveToFirstAttribute()
	{
		if(NodeType == XPathNodeType.Element && node.Attributes != null)
		{
			if(node.Attributes.Count != 0)
			{
				node = node.Attributes[0];
				return true;
			}
		}
		return false;
	}

	public override bool MoveToFirstChild()
	{
		XmlNode next = node.FirstChild;
		// TODO: implement normalization
		while(next!= null && 
				(next.NodeType == XmlNodeType.EntityReference || 
				next.NodeType == XmlNodeType.DocumentType ||
				next.NodeType == XmlNodeType.XmlDeclaration))
		{
			next = next.NextSibling;
		}

		if(next != null)
		{
			node = next;
			return true;
		}
		return false;
	}

	public override bool MoveToFirstNamespace(XPathNamespaceScope namespaceScope)
	{
		return false;
	}

	public override bool MoveToId(String id)
	{
		return false;
	}

	public override bool MoveToNamespace(String name)
	{
		return false;
	}

	public override bool MoveToNext()
	{
		XmlNode next = node.NextSibling;
		// TODO: implement normalization
		while(next!= null && 
				(next.NodeType == XmlNodeType.EntityReference || 
				next.NodeType == XmlNodeType.DocumentType ||
				next.NodeType == XmlNodeType.XmlDeclaration))
		{
			next = next.NextSibling;
		}
		
		if(next	!= null)
		{
			node = next;
			return true;	
		}
		
		return false;
	}

	public override bool MoveToNextAttribute()
	{
		if(NodeType == XPathNodeType.Attribute)
		{
			int i;
			XmlElement owner = ((XmlAttribute)node).OwnerElement;

			if(owner == null)
			{
				return false;
			}
			
			XmlAttributeCollection list = owner.Attributes;
			
			if(list == null)
			{
				return false;
			}

			for(i=0 ; i<list.Count ; i++)
			{
				// This should be a lot faster
				if(((Object)list[i]) == ((Object)node))
				{
					i++; /* Move to Next */
					break;
				}
			}

			if(i != list.Count)
			{
				node = list[i];
				return true;
			}
		}
		return false;
	}

	public override bool MoveToNextNamespace(XPathNamespaceScope namespaceScope)
	{
		return false;
	}
	

	public override bool MoveToParent()
	{
		if(node.NodeType == XmlNodeType.Attribute)
		{
			XmlElement owner = ((XmlAttribute)node).OwnerElement;
			if(owner != null)
			{
				node = owner;
				return true;
			}
		}
		else if (node.ParentNode != null)
		{
			node = node.ParentNode;
			return true;
		}
		return false;
	}

	public override bool MoveToPrevious()
	{
		if(node.PreviousSibling != null)
		{
			node = node.PreviousSibling;
			return true;
		}
		return false;
	}

	public override void MoveToRoot()
	{
		// TODO: make sure we don't use this for fragments
		if(document != null && document.DocumentElement != null)
		{
			node = document;
		}
		return;
	}
	
	public override String BaseURI 
	{
		get
		{
			return node.BaseURI;
		}
	}

	public override bool HasAttributes
	{
		get
		{
			if(node.Attributes != null)
			{
				return (node.Attributes.Count != 0);
			}
			return false;
		}
	}

	public override bool HasChildren
	{
		get
		{
			return (node.FirstChild != null);	
		}
	}

	public override bool IsEmptyElement
	{
		get
		{
			if(node.NodeType == XmlNodeType.Element)
			{
				return ((XmlElement)node).IsEmpty;
			}
			return false;
		}
	}

	public override String LocalName
	{
		get
		{
			XPathNodeType nodeType = NodeType;
			
			if(nodeType == XPathNodeType.Element ||
				nodeType == XPathNodeType.Attribute ||
				nodeType == XPathNodeType.ProcessingInstruction ||
				nodeType == XPathNodeType.Namespace)
			{
				return node.LocalName;
			}
			return String.Empty;
		}
	}

	public override String Name
	{
		get
		{
			XPathNodeType nodeType = NodeType;
			
			if(nodeType == XPathNodeType.Element ||
				nodeType == XPathNodeType.Attribute ||
				nodeType == XPathNodeType.ProcessingInstruction ||
				nodeType == XPathNodeType.Namespace)
			{
				return node.Name;
			}
			return String.Empty;
		}
	}

	public override XmlNameTable NameTable
	{
		get
		{
			return document.NameTable;
		}
	}

	public override String NamespaceURI
	{
		get
		{
			return node.NamespaceURI;
		}
	}

	public override XPathNodeType NodeType
	{
		get
		{
			switch(node.NodeType)
			{
				case XmlNodeType.Element:
				{
					return XPathNodeType.Element;
				}
				break;
				case XmlNodeType.Comment:
				{
					return XPathNodeType.Comment;
				}
				break;
				case XmlNodeType.Attribute:
				{
					return XPathNodeType.Attribute;
				}
				break;
				case XmlNodeType.Text:
				{
					return XPathNodeType.Text;
				}
				break;
				case XmlNodeType.Whitespace:
				{
					return XPathNodeType.Whitespace;
				}
				break;
				case XmlNodeType.SignificantWhitespace:
				{
					return XPathNodeType.SignificantWhitespace;
				}
				break;
				case XmlNodeType.ProcessingInstruction:
				{
					return XPathNodeType.ProcessingInstruction;
				}
				break;
				case XmlNodeType.Document:
				{
					return XPathNodeType.Root;
				}
				break;
			}
			// TODO resources
			throw new InvalidOperationException(
				String.Format("Invalid XPathNodeType for: {0}", 
							 node.NodeType)); 
		}
	}

	public override String Prefix
	{
		get
		{
			return node.Prefix;
		}
	}

	public override String Value
	{
		get
		{
			switch(NodeType)
			{
				case XPathNodeType.Attribute:
				case XPathNodeType.Comment:
				case XPathNodeType.ProcessingInstruction:
				{
					return node.Value;
				}
				break;
				case XPathNodeType.Text:
				case XPathNodeType.Whitespace:
				case XPathNodeType.SignificantWhitespace:
				{
					// TODO : normalize
					return node.Value;
				}
				break;
				case XPathNodeType.Element:
				case XPathNodeType.Root:
				{
					return node.InnerText;
				}
				break;
				case XPathNodeType.Namespace:
				{
					// TODO ?
					return String.Empty;
				}
				break;
			}
			return String.Empty;
		}
	}

	public override String XmlLang
	{
		get
		{
			return String.Empty;
		}
	}

	public override String ToString()
	{
		return String.Format("<XPathNavigator {0} , {1}>", node,document);
	}

	internal XmlNode CurrentNode
	{
		get
		{
			return this.node;
		}
	}

	XmlNode IHasXmlNode.GetNode()
	{
		return this.node;
	}
}

#endif /* CONFIG_XPATH */

}
