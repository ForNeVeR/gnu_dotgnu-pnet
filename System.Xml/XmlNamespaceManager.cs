/*
 * XmlNamespaceManager.cs - Implementation of the "System.XmlNamespaceManager" class.
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

[TODO]
public class XmlNamespaceManager : IEnumerable
{
	//TODO: The scope stuff
	
	private XmlNameTable nametable;

	public XmlNamespaceManager(XmlNameTable nameTable)
			{
				if (nameTable == 0)
					throw new ArgumentNullException("nameTable");
				
				nametable = nametable;
			}
	
	[TODO]
	public virtual void AddNamespace(String prefix, String uri)
			{
				if (prefix == "xml" || prefix == "xmlns")
					throw new ArgumentException("prefix", _("Xml_InvalidPrefix"));
					
				if (prefix == 0)
					throw new ArgumentNullException("prefix");
					
				if (uri == 0)
					throw new ArgumentNullException("uri");

				String newentry = prefix + "/<!>/" + uri;
			
				//PROBLEM: This method should replace the
				//entry in nametable that has the same value,
				//if it already exists. But XmlNameTable 
				//must leave existing entries unchanged 
				//when a String with the same value applies	
					
			}
	
	[TODO]
	public virtual IEnumerator GetEnumerator() {}

	[TODO]
	public virtual bool HasNamespace(string prefix) {}
	
	[TODO]
	public virtual string LookupNamespace(string prefix) {}
	
	[TODO]
	public virtual string LookupPrefix(string uri) {}
	
	[TODO]
	public virtual bool PopScope() {}
	
	[TODO]
	public virtual void PushScope() {}

	[TODO]
	public virtual void RemoveNamespace(string prefix, string uri) {}
	
	[TODO]
	public virtual string DefaultNamespace 
			{ 
				get
				{
				
				}
			}
	
	[TODO]
	public XmlNameTable NameTable
			{
				get
				{
				
				}
			}

}; //class XmlNamespaceManager

}; //namespace System.Xml