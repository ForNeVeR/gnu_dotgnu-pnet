/*
 * XmlFragmentTextWriter.cs - Implementation of the
 *		"System.Xml.Private.XmlFragmentTextWriter" class.
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
 
namespace System.Xml.Private
{

using System;
using System.IO;
using System.Text;
using System.Globalization;

// This class is used for outputting XML fragments from the
// "InnerXml" and "OuterXml" properties in "XmlNode".

internal class XmlFragmentTextWriter : XmlTextWriter
{
	// Constructor.
	public XmlFragmentTextWriter()
			: base(new StringWriter())
			{
				// Make the writer automatically shift to the content
				// area of the document if it is in the start state.
				autoShiftToContent = true;
			}

	// Close the fragment and return the final string.
	public override String ToString()
			{
				Close();
				return ((StringWriter)writer).ToString();
			}

	// Override some of the XmlTextWriter methods to handle namespaces better.
	public override void WriteStartAttribute
				(String prefix, String localName, String ns)
			{
				if(ns == null || ns.Length == 0)
				{
					if(prefix != null && prefix.Length != 0)
					{
						prefix = "";
					}
				}
				base.WriteStartAttribute(prefix, localName, ns);
			}
	public override void WriteStartElement
				(String prefix, String localName, String ns)
			{
				if(ns == null || ns.Length == 0)
				{
					if(prefix != null && prefix.Length != 0)
					{
						prefix = "";
					}
				}
				base.WriteStartElement(prefix, localName, ns);
			}

}; // class XmlFragmentTextWriter

}; // namespace System.Xml.Private
