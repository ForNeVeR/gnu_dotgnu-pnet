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
using System.Globalization;

// This class reads and parses a complete xml file into memory, the
// methods called on it return data from the parses memory file.
// This is a simple however memory inefficient use, but the efficiency
// of parsing should improve.

// Due to the use of XmlNode in the parsing process this class cannot be 
// overloaded from outside without copying XmlNode into the outside lib.

public abstract class XmlReader
{
	//Variables
	internal XmlNode root; //The root node, it can be used to access all other nodes
	internal XmlNode current; //The pointer node

	//Methods
	protected XmlReader() {}
	
	public abstract void Close();
	
	public abstract String GetAttribute(int i);
	
	public abstract String GetAttribute(String name, String namespaceURI);
	
	public abstract String GetAttribute(String name);
	
	public static bool IsName(string str) 
			{
				if (Char.IsLetter(str, 0) ||str.StartsWith("_") || str.StartsWith(":"))
				{ 
					return true;
				}
				else
				{ 
					// Dosen't start with the right chars
					return false;
				}
			}
		
	public static bool IsNameToken(String str)
			{
				CharEnumerator e = str.GetEnumerator();
				Char c;
				int x = 0, num = 0;
				while (e.MoveNext() && x < 1)
				{	
					c = e.Current;
					num = (int)c;
					if(Char.IsLetterOrDigit(c) ||
					         c == '.' || c == '_' || 
						 c == '-' || c == ':' || Char.GetUnicodeCategory(c) == UnicodeCategory.ConnectorPunctuation || num == 0x00B7 ||
num == 0x02D0 || num == 0x02D1 || num == 0x0387 || num == 0x0640 || 
num == 0x0E46 || num == 0x0EC6 || num == 0x3005 || 
(num >= 0x3031 && num <= 0x3035) || (num >= 0x309D && num <= 0x309E) || 
(num >= 0x30FC && num <= 0x30FE))
					{
						++x;
					}
				}
				if (x < 1) 
				{
					return false;
				}
				else
				{
					return true;
				}	
			
        		}                		
	
	
	public virtual bool IsStartElement(string localname, string ns)
			{
				XmlNodeType node = MoveToContent(); 
				if (node == XmlNodeType.None)
				{
					// End Of Input
					return false;
				}
				// Test localname and ns
				if (localname == this.LocalName 
				&& ns == this.NamespaceURI)	
				{
					return true;
				}
				else
				{
					return false;	
				}
			}
	
	public virtual bool IsStartElement(string name) 
		{
			XmlNodeType node = MoveToContent();
			if (node == XmlNodeType.None)
			{
				// End Of Input
				return false;
			}
			// Test the name of the Xml Node
			if (name == this.Name)
			{
				return true;
			}
			else
			{	
				return false;
			}
		}			

	public virtual bool IsStartElement() 
		{
			XmlNodeType node = MoveToContent();
			if (node == XmlNodeType.None)
			{
				// End Of Input
				return false;
			}
			else
			{
				// It's an Element
				return true;
			}
		}
	
	public abstract String LookupNamespace(String prefix);
	
	public abstract void MoveToAttribute(int i);
	
	public abstract bool MoveToAttribute(String name, String ns);
	
	public abstract bool MoveToAttribute(String name);
	
	public virtual XmlNodeType MoveToContent() 
		{
			// Current Node can't contain content
			if (!(this.NodeType == XmlNodeType.Attribute || 
			this.NodeType == XmlNodeType.CDATA ||
			this.NodeType == XmlNodeType.Element || 
			this.NodeType == XmlNodeType.EndElement ||
			this.NodeType == XmlNodeType.EntityReference ||
			this.NodeType == XmlNodeType.EndEntity || 
			this.NodeType == XmlNodeType.Text))
			{
				XmlNodeType n;
				do
				{
					n = this.NodeType;
				} while (this.Read() && !(n == 
				XmlNodeType.Attribute || 
				n == XmlNodeType.CDATA ||
				n == XmlNodeType.Element || 
				n == XmlNodeType.EndElement ||
				n == XmlNodeType.EntityReference ||
				n == XmlNodeType.EndEntity || 
				n == XmlNodeType.Text || 
				n == XmlNodeType.Entity)); 
				
				return this.NodeType;
			}
			else
			{
				return this.NodeType;
			}
		}
				
	public abstract bool MoveToElement();
	
	public abstract bool MoveToFirstAttribute();
	
	public abstract bool MoveToNextAttribute();
	
	public abstract bool Read();
	
	public abstract bool ReadAttributeValue();
	
	public virtual string ReadElementString(string localname, string ns)
			{
				XmlNodeType node = MoveToContent(); 
				if (node == XmlNodeType.None)
				{
					// Not an Element
					throw new XmlException();
				}
				// Test localname and ns
				if (localname == this.LocalName 
				&& ns == this.NamespaceURI)	
				{
					// Read contents of this element
					return this.ReadString();
				}
				else
				{
					// No node with localname and ns
					throw new XmlException();
				}
		}
	
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

