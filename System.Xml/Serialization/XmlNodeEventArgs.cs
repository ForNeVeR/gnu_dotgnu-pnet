/*
 * XmlNodeEventArgs.cs - Implementation of the
 *			"System.Xml.Serialization.XmlNodeEventArgs" class.
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

public class XmlNodeEventArgs : EventArgs
{
	// Internal state.
	private int lineNumber;
	private int linePosition;
	private String localName;
	private String name;
	private String namespaceURI;
	private XmlNodeType nodeType;
	private Object objectBeingDeserialized;
	private String text;

	// Constructor.
	internal XmlNodeEventArgs
				(int lineNumber, int linePosition, String localName,
				 String name, String namespaceURI, XmlNodeType nodeType,
				 Object objectBeingDeserialized, String text)
			: base()
			{
				this.lineNumber = lineNumber;
				this.linePosition = linePosition;
				this.localName = localName;
				this.name = name;
				this.namespaceURI = namespaceURI;
				this.nodeType = nodeType;
				this.objectBeingDeserialized = objectBeingDeserialized;
				this.text = text;
			}

	// Properties.
	public int LineNumber
			{
				get { return lineNumber; }
			}
	public int LinePosition
			{
				get { return linePosition; }
			}
	public String LocalName
			{
				get { return localName; }
			}
	public String Name
			{
				get { return name; }
			}
	public String NamespaceURI
			{
				get { return namespaceURI; }
			}
	public XmlNodeType NodeType
			{
				get { return nodeType; }
			}
	public Object ObjectBeingDeserialized
			{
				get { return objectBeingDeserialized; }
			}
	public String Text
			{
				get { return text; }
			}

}; // class XmlNodeEventArgs

#endif // !ECMA_COMPAT

}; // namespace System.Xml.Serialization
