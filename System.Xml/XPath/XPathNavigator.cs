/*
 * XPathNavigator.cs - Implementation of the
 *		"System.Xml.XPath.XPathNavigator" class.
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

using System;
using System.Xml;

namespace System.Xml.XPath
{

[TODO]
#if ECMA_COMPAT
internal
#else
public
#endif
abstract class XPathNavigator : ICloneable
{
	// TODO

	// Constructor.
	protected XPathNavigator() {}

	// Implement the ICloneable interface.
	public abstract Object Clone();

	[TODO]
	public virtual XmlNodeOrder ComparePosition(XPathNavigator nav)
	{
		 throw new NotImplementedException("ComparePosition");
	}

	[TODO]
	public virtual XPathExpression Compile(String xpath)
	{
		 throw new NotImplementedException("Compile");
	}

	[TODO]
	public virtual Object Evaluate(XPathExpression expr)
	{
		 throw new NotImplementedException("Evaluate");
	}

	[TODO]
	public virtual Object Evaluate(XPathExpression expr, 
									XPathNodeIterator context)
	{
		 throw new NotImplementedException("Evaluate");
	}

	[TODO]
	public virtual Object Evaluate(String xpath)
	{
		 throw new NotImplementedException("Evaluate");
	}

	public abstract String GetAttribute(String localName, String namespaceURI);

	public abstract String GetNamespace(String name);

	[TODO]
	public virtual bool IsDescendant(XPathNavigator nav)
	{
		 throw new NotImplementedException("IsDescendant");
	}

	public abstract bool IsSamePosition(XPathNavigator other);

	[TODO]
	public virtual bool Matches(XPathExpression expr)
	{
		 throw new NotImplementedException("Matches");
	}

	[TODO]
	public virtual bool Matches(String xpath)
	{
		 throw new NotImplementedException("Matches");
	}

	public abstract bool MoveTo(XPathNavigator other);

	public abstract bool MoveToAttribute(String localName, String namespaceURI);

	public abstract bool MoveToFirst();

	public abstract bool MoveToFirstAttribute();

	public abstract bool MoveToFirstChild();

	[TODO]
	public bool MoveToFirstNamespace()
	{
		 throw new NotImplementedException("MoveToFirstNamespace");
	}

	public abstract bool MoveToFirstNamespace(
								XPathNamespaceScope namespaceScope);

	public abstract bool MoveToId(String id);

	public abstract bool MoveToNamespace(String name);

	public abstract bool MoveToNext();

	public abstract bool MoveToNextAttribute();

	[TODO]
	public bool MoveToNextNamespace()
	{
		 throw new NotImplementedException("MoveToNextNamespace");
	}

	public abstract bool MoveToNextNamespace(XPathNamespaceScope namespaceScope);

	public abstract bool MoveToParent();

	public abstract bool MoveToPrevious();

	public abstract void MoveToRoot();

	[TODO]
	public virtual XPathNodeIterator Select(XPathExpression expr)
	{
		 throw new NotImplementedException("Select");
	}

	[TODO]
	public virtual XPathNodeIterator Select(String xpath)
	{
		 throw new NotImplementedException("Select");
	}

	[TODO]
	public virtual XPathNodeIterator SelectAncestors(XPathNodeType type, 
													bool matchSelf)
	{
		 throw new NotImplementedException("SelectAncestors");
	}

	[TODO]
	public virtual XPathNodeIterator SelectAncestors(String name, 
													String namespaceURI, 
													bool matchSelf)
	{
		 throw new NotImplementedException("SelectAncestors");
	}

	[TODO]
	public virtual XPathNodeIterator SelectChildren(XPathNodeType type)
	{
		 throw new NotImplementedException("SelectChildren");
	}

	[TODO]
	public virtual XPathNodeIterator SelectChildren(String name, 
													String namespaceURI)
	{
		 throw new NotImplementedException("SelectChildren");
	}

	[TODO]
	public virtual XPathNodeIterator SelectDescendants(XPathNodeType type, 
														bool matchSelf)
	{
		 throw new NotImplementedException("SelectDescendants");
	}

	[TODO]
	public virtual XPathNodeIterator SelectDescendants(String name, 
														String namespaceURI, 
														bool matchSelf)
	{
		 throw new NotImplementedException("SelectDescendants");
	}

	[TODO]
	public override String ToString()
	{
		 throw new NotImplementedException("ToString");
	}

	public abstract String BaseURI 
	{
		get;
	}

	public abstract bool HasAttributes 
	{
		get;
	}

	public abstract bool HasChildren 
	{
		get;
	}

	public abstract bool IsEmptyElement 
	{
		get;
	}

	public abstract String LocalName 
	{
		get;
	}

	public abstract String Name 
	{
		get;
	}

	public abstract XmlNameTable NameTable 
	{
		get;
	}

	public abstract String NamespaceURI 
	{
		get;
	}

	public abstract XPathNodeType NodeType 
	{
		get;
	}

	public abstract String Prefix 
	{
		get;
	}

	public abstract String Value 
	{
		get;
	}

	public abstract String XmlLang 
	{
		get;
	}


}; // class XPathNavigator

}; // namespace System.Xml.XPath
