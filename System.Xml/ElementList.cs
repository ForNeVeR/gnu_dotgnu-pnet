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
	private XmlElement element;
	private String name;
	private String namespaceURI;
	private bool uriForm;

	// Create a new element list.
	public ElementList(XmlElement element, String name)
			{
				this.element = element;
				this.name = name;
				this.namespaceURI = null;
				this.uriForm = false;
			}
	public ElementList(XmlElement element, String localName,
					   String namespaceURI)
			{
				this.element = element;
				this.name = localName;
				this.namespaceURI = namespaceURI;
				this.uriForm = true;
			}

	// Get the number of entries in the node list.
	[TODO]
	public override int Count
			{
				get
				{
					// TODO
					return 0;
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
