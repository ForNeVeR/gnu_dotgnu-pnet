/*
 * XmlAttributeEventArgs.cs - Implementation of the
 *			"System.Xml.Serialization.XmlAttributeEventArgs" class.
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

public class XmlAttributeEventArgs : EventArgs
{
	// Internal state.
	private XmlAttribute attr;
	private int lineNumber;
	private int linePosition;
	private Object objectBeingDeserialized;

	// Constructor.
	internal XmlAttributeEventArgs
				(XmlAttribute attr, int lineNumber, int linePosition,
				 Object objectBeingDeserialized)
			: base()
			{
				this.attr = attr;
				this.lineNumber = lineNumber;
				this.linePosition = linePosition;
				this.objectBeingDeserialized = objectBeingDeserialized;
			}

	// Properties.
	public XmlAttribute Attr
			{
				get { return attr; }
			}
	public int LineNumber
			{
				get { return lineNumber; }
			}
	public int LinePosition
			{
				get { return linePosition; }
			}
	public Object ObjectBeingDeserialized
			{
				get { return objectBeingDeserialized; }
			}

}; // class XmlAttributeEventArgs

#endif // !ECMA_COMPAT

}; // namespace System.Xml.Serialization
