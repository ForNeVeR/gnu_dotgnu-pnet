/*
 * XmlReader.cs - Implementation of the "System.XmlReader" class.
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
using System.Text;

// This class reads and parses a complete xml file into memory, the
// methods called on it return data from the parses memory file.
// This is a simple however memory inefficient use, but the efficiency
// of parsing should improve.

// Due to the use of XmlNode in the parsing process this class cannot be 
// overloaded from outside without copying XmlNode into the outside lib.

public abstract class XmlReader
{
	//Variables
	internal protected XmlNode root; //The root node, it can be used to access all other nodes
	internal protected XmlNode current; //The pointer node

	//Methods
	protected XmlReader() {}
	
	public abstract void Close();
	
	public abstract String GetAttribute(int i);
	
	public abstract String GetAttribute(String name, String namespaceURI);
	
	public abstract String GetAttribute(String name);
	
	[TODO]
	public static bool IsName(String str) 
			{
			
			}
	
	[TODO]
	public static bool IsNameToken(String str)
			{

			}
	
	[TODO]
	public virtual bool IsStartElement(String localname, String ns)
			{ return false; }
	
	[TODO]
	public virtual bool IsStartElement(String name) { return false; }
	
	[TODO]
	public virtual bool IsStartElement() { return false; }
	
	public abstract String LookupNamespace(String prefix);
	
	public abstract void MoveToAttribute(int i);
	
	public abstract bool MoveToAttribute(String name, String ns);
	
	public abstract bool MoveToAttribute(String name);
	
	[TODO]
	public virtual XmlNodeType MoveToContent() { return XmlNodeType.None; }
	
	public abstract bool MoveToElement();
	
	public abstract bool MoveToFirstAttribute();
	
	public abstract bool MoveToNextAttribute();
	
	public abstract bool Read();
	
	public abstract bool ReadAttributeValue();
	
	[TODO]
	public virtual String ReadElementString(String localname, String ns)
			{ return null; }
	
	[TODO]
	public virtual String ReadElementString(String name) { return null; }
	
	[TODO]
	public virtual String ReadElementString() { return null; }
	
	[TODO]
	public virtual void ReadEndElement() {}
	
	public abstract String ReadInnerXml();
	
	public abstract String ReadOuterXml();
	
	[TODO]
	public virtual void ReadStartElement(String localname, String ns) {}
	
	[TODO]
	public virtual void ReadStartElement(String name) {}
	
	[TODO]
	public virtual void ReadStartElement() {}
	
	public abstract String ReadString();
	
	public abstract void ResolveEntity();
	
	[TODO]
	public virtual void Skip() {}
	
	public abstract int AttributeCount { get; }
	
	public abstract String BaseURI { get; }
	
	public virtual bool CanResolveEntity
			{
				get
				{
					return false;
				}
			}
	
	public abstract int Depth { get; }
	
	public abstract bool EOF { get; }
	
	[TODO]
	public virtual bool HasAttributes 
			{ 
				get
				{
					// TODO
					return false;
				}
			}
	
	public abstract bool HasValue { get; }
	
	public abstract bool IsDefault { get; }
	
	public abstract bool IsEmptyElement { get; }
	
	public abstract String this[int i] { get; }
	
	public abstract String this[String name] { get; }
	
	public abstract String this[String name, String namespaceURI] { get; }
	
	public abstract String LocalName { get; }
	
	public abstract String Name { get; }
	
	public abstract XmlNameTable NameTable { get; }
	
	public abstract String NamespaceURI { get; }
	
	public abstract XmlNodeType NodeType { get; }
	
	public abstract String Prefix { get; }
	
	public abstract char QuoteChar { get; }
	
	public abstract ReadState ReadState { get; }
	
	public abstract String Value { get; }
	
	public abstract String XmlLang { get; }
	
	public abstract XmlSpace XmlSpace { get; }

}; //class XmlReader

}; //namespace System.Xml
