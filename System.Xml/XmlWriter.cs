/*
 * XmlWriter.cs - Implementation of the "System.XmlWriter" class.
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

public abstract class XmlWriter
{

	//Vars
	WriteState writestate;
		
	//Methods
	protected XmlWriter();
	
	public abstract void Close();
	
	public abstract void Flush();
	
	public abstract String LookupPrefix(String ns);
	
	public void WriteAttributeString(String localName, String ns, String value)
		{
			WriteAttributeString(null, localName, ns, value);
		}
	
	
	public void WriteAttributeString(String localName, String value)
		{
			WriteAttributeString(null, localName, null, value);
		}
	
	public void WriteAttributeString(String prefix, String localName, String ns, String value)
		{
			if (localName == null || localName == System.String.Empty)
				throw new ArgumentException("localName");		
				
			if (writestate != WriteState.Element)
				throw new InvalidOperationException("WriteState");
			
			WriteStartAttribute(prefix , localName, ns);
			WriteString(value);
			WriteEndAttribute();
		}			
	
	[TODO]
	public virtual void WriteAttributes(XmlReader reader, bool defattr)
		{	
			//TODO
		}	
	
	public abstract void WriteBase64(byte[] buffer, int index, int count);
	
	public abstract void WriteBinHex(byte[] buffer, int index, int count);
	
	public abstract void WriteCData(String text);

	public abstract void WriteCharEntity(char ch);
	
	public abstract void WriteChars(char[] buffer, int index, int count);
	
	public abstract void WriteComment(String text);
	
	public abstract void WriteDocType(String name, String pubid, String sysid, String subset);
	
	public void WriteElementString(String localName, String value)
		{
			WriteElementString(localName, null, value);
		}
	
	public void WriteElementString(String localName, String ns, String value)
		{
			if (writestate == WriteState.Closed)
				throw new InvalidOperationException("WriteState");
			
			WriteStartElement(localName, ns);
			
			if (!(value == String.Empty || value == null))
				WriteString(value);
			
			WriteEndElement();	
		}
	
	public abstract void WriteEndAttribute();
	
	public abstract void WriteEndDocument();
	
	public abstract void WriteEndElement();
	
	public abstract void WriteEntityRef(String name);
	
	public abstract void WriteFullEndElement();
	
	public abstract void WriteName(String name);
	
	public abstract void WriteNmToken(String name);

	[TODO]
	public virtual void WriteNode(XmlReader reader, bool defattr)
		{
			//TODO
		}

	public abstract void WriteProcessingInstruction(String name, String text);

	public abstract void WriteQualifiedName(String localName, String ns);

	public abstract void WriteRaw(String data);
	
	public abstract void WriteRaw(char[] buffer, int index, int count);
	
	public abstract void WriteStartAttribute(String prefix, String localName, String ns);
	
	public void WriteStartAttribute(String localName, String ns)
		{
			WriteStartAttribute(null, localName, ns);
		}

	public abstract void WriteStartDocument(bool standalone);

	public abstract void WriteStartDocument();

	public abstract void WriteStartElement(String prefix, String localName, String ns);
	
	public void WriteStartElement(String localName, String ns)
		{
			WriteStartElement(null, localName, ns);
		}
	
	public void WriteStartElement(String localName)
		{
			WriteStartElement(null, localName, null);
		}
	
	public abstract void WriteString(String text);
	
	public abstract void WriteSurrogateCharEntity(char lowChar, char highChar);
	
	public abstract void WriteWhitespace(String ws);
	
	public abstract WriteState WriteState { get; }
	
	public abstract String XmlLang { get; }
		
	public abstract XmlSpace XmlSpace { get; }
		
}; //class XmlWriter

}; //namespace System.Xml

