/*
 * XmlTextReader.cs - Implementation of the "System.XmlTextReader" class.
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
using System.IO;
using System.Text;

// This class reads and parses a complete xml file into memory, the
// methods called on it return data from the parses memory file.
// This is a simple however memory inefficient use, but the efficiency
// of parsing should improve.

// Due to the use of XmlNode in the parsing process this class cannot be 
// overloaded from outside without copying XmlNode into the outside lib.

// The parsing has 2 fases. In the first fase, activated and completed at the
// first call to Read(), the nodes are identified and put in the node tree.
// In the second fase every single node is checked. This is done when the node
// is actually pointed to from this class (during operation).

// There is NO checking on namespaces, whenever a xmlns:foo='url' attribute
// is found the namespace is added to a table, when the element containing
// the attribute is left the namespace is removed from the table

// This class can't stand on a attribute yet, since ecma isn't very clear
// about attributes. Attributes can however be accessed by the this[i] and
// this[String] methods if the node is on an element, declaration or xmldeclaration

[TODO]
public class XmlTextReader : XmlReader
{
	//Variables inherited from XmlReader (DO NOT UNCOMMENT):
	// they've now been uncommented because XmlReader should not have state
	// -- Rhys.
	internal XmlNode root;  // The root node, it can be used to access all other nodes
	internal XmlNode current;  // The pointer node

	private ReadState readstate;
	private String rawxml;
	private ArrayList xmlnamespaces;
	private String error;
	private String name;
	private String[] attributes;
	private int attributecount;
	private Stream stream;
	private bool fromstream = false;

	//If set the class ignores the complicated stuff and parses the rawxml 
	//supplied without unneccesary checking
	private bool simplemode = false; 


	[TODO]
	protected XmlTextReader(){}

	[TODO]
	protected XmlTextReader(XmlNameTable nt){}
	
	//Until further notice this constructor uses simplemode
	//All data must be in the stream at the time of read start
	public XmlTextReader(Stream input)
	{
		simplemode = true; //Go simple for now
		stream = input;			
		fromstream = true;
	}
	
	[TODO]
	public XmlTextReader(String url, Stream input){}
	
	[TODO]
	public XmlTextReader(Stream input, XmlNameTable nt){}
	
	[TODO]
	public XmlTextReader(String url, Stream input, XmlNameTable nt){}
	
	[TODO]
	public XmlTextReader(TextReader input){}
	
	[TODO]
	public XmlTextReader(String url, TextReader input){}
	
	[TODO]
	public XmlTextReader(TextReader input, XmlNameTable nt){}
	
	[TODO]
	public XmlTextReader(String url, TextReader input, XmlNameTable nt){}
	
	[TODO]
	public XmlTextReader(Stream xmlFragment, XmlNodeType fragType, XmlParserContext context){}	
		
	[TODO]
	public XmlTextReader(String url){}
	
	[TODO]
	public XmlTextReader(String url, XmlNameTable nt){}

#if !ECMA_COMPAT
	//This constructor is the most simple, just feed it with the xml and you
	//can parse it, but don't expect checking or fancy stuff
	public XmlTextReader(char[] rawXML)
	{
		rawxml = new String(rawXML);
		simplemode = true;
	}
#endif
	
	[TODO]
	public override void Close(){}
	
	[TODO]
	public override String GetAttribute(int i){ return null; }
	
	[TODO]
	public override String GetAttribute(String localName, String namespaceURI){ return null; }

	[TODO]
	public override String GetAttribute(String name){ return null; }
	
	[TODO]
	public TextReader GetRemainder(){ return null; }
	
	[TODO]
	public override String LookupNamespace(String prefix){ return null; }
	
	[TODO]
	public override void MoveToAttribute(int i){}
	
	[TODO]
	public override bool MoveToAttribute(String localName, String namespaceURI){ return false; }
	
	[TODO]
	public override bool MoveToAttribute(String name){ return false; }
	
	[TODO]
	public override bool MoveToElement(){ return false; }
	
	[TODO]
	public override bool MoveToFirstAttribute(){ return false; }
	
	[TODO]
	public override bool MoveToNextAttribute(){ return false; }

	//All data must be in the receiving end of the stream before this
	//method is called for the first time
	public override bool Read()
	{
		if (simplemode)
		{
			if (readstate == ReadState.Initial)
			{
				//If stream, read stream
				if (fromstream)
				{
					StreamReader streamreader = new StreamReader(stream);
					rawxml = streamreader.ReadToEnd();				
				}
				//Don't bother with the works, just go lazy and fast
				current = root = Parse(rawxml);
			}
			else if (readstate == ReadState.Interactive)
			{		
				//Move to next
				if (current.Next == null)
					return false;
				
				//Do some parsing
				try
				{
					if (current.Type == XmlNodeType.Element)
					{
						name = GetName(current.Text);
						attributes = GetAttributes(current.Text, true);
						attributecount = attributes.Length;
					}
					else if (current.Type == XmlNodeType.DocumentType
							|| current.Type == XmlNodeType.XmlDeclaration)
					{
						attributes = GetAttributes(current.Text, false);
						attributecount = attributes.Length;
					}
					else
					{
						name = null;
						attributes = null;
						attributecount = 0;
					}
				}
				catch (Exception e)
				{
					throw new XmlException("Read");
				}
				
				return true;
			}
			else if (readstate == ReadState.Error || readstate == ReadState.Closed)
			{
				//Note: An error string can be found in this.Error
				throw new XmlException("Read");
			}
		}

		// TODO - fix all code paths to return the correct value.
		return false;
	}
	
	[TODO]
	public override bool ReadAttributeValue(){ return false; }
	
	[TODO]
	public int ReadBase64(byte[] array, int offset, int len){ return -1; }
	
	[TODO]
	public int ReadBinHex(byte[] array, int offset, int len){ return -1; }
	
	[TODO]
	public int ReadChars(char[] buffer, int index, int count){ return -1; }
	
	[TODO]
	public override String ReadInnerXml(){ return null; }
	
	[TODO]
	public override String ReadOuterXml(){ return null; }
	
	[TODO]
	public override String ReadString(){ return null; }
	
	[TODO]
	public void ResetState(){}
	
	public override void ResolveEntity()
	{
		throw new InvalidOperationException("ResolveEntity");
	}
	
	public override int AttributeCount 
	{ 
		get
		{
			return attributecount;
		} 
	}
	
	[TODO]
	public override String BaseURI { get{ return null; } }
	
	[TODO]
	public override int Depth
	{ 
		get
		{
			return current.Depth;
		} 
	}
	
	[TODO]
	public override bool EOF { 
		get
		{
			if (current.Next == null)
				return true;
			else
				return false;
		} 
	}
	
	[TODO]
	public Encoding Encoding { get{ return null; } }
	
	[TODO]
	public override bool HasValue { get{ return false; } }
	
	[TODO]
	public override bool IsDefault { get{ return false; } }
	
	[TODO]
	public override bool IsEmptyElement { get{ return false; } }
	
	public override String this[int i]
	{ 
		get
		{
			if (current.Type == XmlNodeType.Element
				|| current.Type == XmlNodeType.XmlDeclaration
				/*|| current.Type == XmlNodeType.Declaration -- ??? */)
			{
				if (i < 0 || i > attributecount || attributecount == 0)
				{
					error = "Attribute index out of range or wrong node type";
					throw new ArgumentOutOfRangeException("this[int 1]");
				}
				//Looks complex, doesn't it :-)

				//TODO: Rhys changed this from the "DIVIDE" string to a
				//special Unicode character because "Split" doesn't allow
				//strings as arguments.  The attributes should be changed
				//into real arrays or lists or something, rather than using
				//a hacky solution like this.

				return attributes[i].Split('\uFFFE')[1];
			}
			else
			{
				return null;
			}
		} 
	}
	
	[TODO]
	public override String this[String name]
	{ 
		get
		{
			if (current.Type == XmlNodeType.Element
				|| current.Type == XmlNodeType.XmlDeclaration
				/*|| current.Type == XmlNodeType.Declaration -- ??? */)
			{
				String[] names;
				String[] values;
				int y;
				int z;
					
				//TODO: Change this!  It is a really bad structure! -- Rhys
				names = new String [attributes.Length];
				values = new String [attributes.Length];
				for (int x = 0; x < attributes.Length; x++)
				{
					names[x] = attributes[x].Split('\uFFFE')[0];
					values[x] = attributes[x].Split('\uFFFE')[1];
				}
				if ((z = Array.IndexOf(names, name)) == -1)
				{
					return null;
				}
				
				return values[z];
			}
			else
			{
				return null;
			}
		} 
	}
	
	[TODO]
	public override String this[String name, String namespaceURI] { get{ return null; } }
	
	[TODO]
	public int LineNumber { get{ return -1; } }
	
	[TODO]
	public int LinePosition { get{ return -1; } }
	
	[TODO]
	public override String LocalName { get{ return null; } }
	
	[TODO] //Get this working on other things then elements
	public override String Name 
	{ 
		get
		{
			return name;
		}
	}
	
	[TODO]
	public override XmlNameTable NameTable { get{ return null; } }
	
	[TODO]
	public override String NamespaceURI { get{ return null; } }
	
	[TODO]
	public bool Namespaces 
	{ 
		get
		{
			return false; //No namespace support yet
		} 
		set
		{
			//No namespace support possible so nothing can be set
		} 
	}
	
	public override XmlNodeType NodeType 
	{ 
		get
		{
			return current.Type;
		} 
	}
	
	[TODO]
	public bool Normalization { get{ return false; } set{} }
	
	[TODO]
	public override String Prefix { get{ return null; } }
	
	[TODO]
	public override char QuoteChar { get{ return '\u0000'; } }
	
	public override ReadState ReadState { 
		get
		{
			return readstate;
		} 
	}
	
	public override String Value 
	{ 
		get
		{
			switch (current.Type)
			{
				case XmlNodeType.Attribute:
					break; //Not supported yet
				
				case XmlNodeType.CDATA:
					return current.Text;
				
				case XmlNodeType.Comment:
					return current.Text;

				case XmlNodeType.DocumentType:
					return current.Text;
					
				case XmlNodeType.SignificantWhitespace:
					break; //Not supported yet
				
				case XmlNodeType.Text:
					return current.Text;
				
				case XmlNodeType.Whitespace:
					return current.Text;
				
				case XmlNodeType.XmlDeclaration:
					return current.Text;
				
				default:
					return String.Empty;
			}

			return String.Empty;
		} 
	}
	
	[TODO]
	public WhitespaceHandling WhitespaceHandling { get{ return WhitespaceHandling.All; } set{} }
	
	[TODO]
	public override String XmlLang { get{ return null; } }
	
	[TODO]
	public XmlResolver XmlResolver { set{} }
	
	[TODO]
	public override XmlSpace XmlSpace { get{ return XmlSpace.None; } }
	
#if !ECMA_COMPAT

	//Returns an arraylist containing XmlNamespaceEntries
	public ArrayList XmlNamespaces
	{
		get
		{
			return xmlnamespaces;
		}
	}

	//Returns a hopefully useful error message of the last error
	public String Error
	{	
		get
		{
			return error;		
		}
	}

#endif

	//---------------------------------------------------------------------
	//Internal parsing code starts here
	//---------------------------------------------------------------------

	[TODO] //Make it work with '
	private String[] GetAttributes (String data, bool cutoff)
	{
		ArrayList values;
		ArrayList names;
		int equalpos;
		String[] rawstuff;
		String[] cookedstuff;
		int x;

		if (cutoff)
		{
			//Cut of the start
			x = data.IndexOfAny(WhitespaceChars);
		}
		else
		{
			x = 0;
		}

		if (x == -1)
			return null;
			
		data = data.Substring(x);
		
		//Every second " marks the end of one attribute
		//so divide on that, THIS CODE DOESN'T WORK WITH ' !!

		rawstuff = data.Split('"');

		names = new ArrayList ();
		values = new ArrayList ();

		for (x = 0; x < (rawstuff.Length / 2); x++)
		{
			//Process names
			if ((equalpos = rawstuff[2*x].IndexOf('=')) == -1)
			{	
				error = "No = in attribute assignment";
				throw new XmlException();
			}

			rawstuff[2*x] = rawstuff[2*x].Substring(0, equalpos - 1);
			rawstuff[2*x] = rawstuff[2*x].Trim();
			
			names.Add(rawstuff[2*x]);		
			
			//Process values
			values.Add(rawstuff[2*x+1]);			
		}

		cookedstuff = new String[values.Count];

		//Put it together
		for (x = 0; x < values.Count; x++)
		{
			cookedstuff[x] = names[x] + "\uFFFE" + values[x];
		}		

		return cookedstuff;
	}

	private String GetName(String data)
	{
		int x = data.IndexOfAny(WhitespaceChars);
		if (x == -1)
			return data;
		
		return data.Substring(0,x);
	}
	
	//Returns the root XmlNode of document raw. Can handle incomplete pieces.
	//Checks only for most basic syntax.
	[TODO] //Change exception Strings and other things
	private XmlNode Parse(String raw)
	{
		XmlNode myroot;
		XmlNode mycurrentnode;
		XmlNode myparent;
		XmlNode newnode;
		
		int s; //The start position of the current node
		int e; //The end position of the current node
		
		int elc; //The position of the next '<'
		int enc; //The position of the next '&'

		String data;
		int depth = 0;
		bool gotel = false; //Got Element switch

		//This method can only be called by a clean reader
		if (readstate != ReadState.Initial)
			throw new XmlException("Wrong readstate for parsing");
		
		//Every Xml document or piece starts with an element like node.
		//So cut of everything before it
		//Only a comment is also valid according to this method
		if ((s = raw.IndexOf('<')) == -1)
			throw new XmlException("No < in document"); 
		
		//First create the document (root) node"
		myroot = new XmlNode(null,null,XmlNodeType.Document, String.Empty, 0);
		mycurrentnode = myroot;
		myparent = myroot;
		
		//Now go into the main parsing routines
		for (;;)
		{
			gotel = false;

			//Everything before an element-like node or an entity is text
			//Whitespace is treated as text, the second fase should filter
			//useless whitespace out
			elc = raw.IndexOf('<', s);
			enc = raw.IndexOf('&', s);

			//Starts with <
			if (elc != -1 && elc < enc)
			{
				e = elc;
				
				//Cut of the beginning into a text node
				if (elc > 0)
				{						
					data = raw.Substring(s, e-s);
					
					newnode = 
						new XmlNode(mycurrentnode, myparent, XmlNodeType.Text, data, depth);
					
					//Change current pointer
					mycurrentnode = newnode;	
				}
				
				//Now determine what exactly this node is
				s = e;

				if (raw.IndexOf("<[CDATA[", s) == s)
				{
					//CDATA section
					gotel = true;	
				
					if ((e = raw.IndexOf("]]>", s)) == -1)
					{
						error = "Unmatched CDATA start";
						readstate = ReadState.Error;
						return null;	// TODO - ???
					}
					
					data = raw.Substring(s + 8, e-3-8-s);
					
					newnode = 
						new XmlNode(mycurrentnode, myparent, XmlNodeType.CDATA, data, depth);
					
					//Change current pointer
					mycurrentnode = newnode;								
				}			
				
				if (raw.IndexOf("<!--", s) == s && !gotel)
				{
					//Comment section
					gotel = true;	
				
					if ((e = raw.IndexOf("-->", s)) == -1)
					{
						error="Unmatched comment start";
						readstate = ReadState.Error;
						return null;	// TODO - ???
					}
					data = raw.Substring(s + 4, e-3-4-s);
					
					newnode = 
						new XmlNode(mycurrentnode, myparent, XmlNodeType.Comment, data, depth);
					
					//Change current pointer
					mycurrentnode = newnode;								
				}	

				if (raw.IndexOf("<!DOCTYPE", s) == s && !gotel)
				{
					//DOCTYPE section
					gotel = true;	
				
					if ((e = raw.IndexOf(">", s)) == -1)
					{
						error="Unmatched DOCTYPE start";
						readstate = ReadState.Error;
						return null;	// TODO - ???
					}
					data = raw.Substring(s+9, e-1-9-s);
					
					newnode = 
						new XmlNode(mycurrentnode, myparent, XmlNodeType.DocumentType, data, depth);
					
					//Change current pointer
					mycurrentnode = newnode;								
					
					//TODO: Parse the contents of the entityreference
				}	

				if (raw.IndexOf("</", s) == s && !gotel)
				{
					//EndElement section
					gotel = true;	
				
					if ((e = raw.IndexOf(">", s)) == -1)
					{
						error="Unmatched EndElement start";
						readstate = ReadState.Error;
						return null;	// TODO - ???
					}					
					data = raw.Substring(s+2, e-1-2-s);
					
					newnode = 
						new XmlNode(mycurrentnode, myparent, XmlNodeType.EndElement, data, depth - 1);
					
					//Change current pointer
					mycurrentnode = newnode;
					
					//Lower depth if not toplevel
					if (!(depth == 0))
					{
						depth--;
						myparent = myparent.Parent; //Set myparent a level up
					}	
				}	

				if (raw.IndexOf("<!ENTITY", s) == s && !gotel)
				{
					//Entity section
					gotel = true;	
				
					if ((e = raw.IndexOf(">", s)) == -1)
					{
						error="Unmatched Entity start";
						readstate = ReadState.Error;
						return null;	// TODO - ???
					}					
					data = raw.Substring(s+8, e-1-8-s);
					
					newnode = 
						new XmlNode(mycurrentnode, myparent, XmlNodeType.Entity, data, depth);
					
					//Change current pointer
					mycurrentnode = newnode;
					
					//TODO: Parse the contents of the entityreference								
				}	

				if (raw.IndexOf("<!NOTATION", s) == s && !gotel)
				{
					//Notation section
					gotel = true;	
				
					if ((e = raw.IndexOf(">", s)) == -1)
					{
						error="Unmatched Notation start";
						readstate = ReadState.Error;
						return null;	// TODO - ???
					}
					
					data = raw.Substring(s+10, e-1-10-s);
					
					newnode = 
						new XmlNode(mycurrentnode, myparent, XmlNodeType.Notation, data, depth);
					
					//Change current pointer
					mycurrentnode = newnode;						
				}	

				if (raw.IndexOf("<?xml", s) == s && !gotel)
				{
					//Notation section
					gotel = true;	
				
					if ((e = raw.IndexOf("?>", s)) == -1)
					{
						error="Unmatched XmlDeclaration start";
						readstate = ReadState.Error;
						return null;	// TODO - ???
					}
										
					data = raw.Substring(s+5, e-2-5-s);
					
					newnode = 
						new XmlNode(mycurrentnode, myparent, XmlNodeType.XmlDeclaration, data, depth);
					
					//Change current pointer
					mycurrentnode = newnode;						
				}	

				if (raw.IndexOf("<?", s) == s && !gotel)
				{
					//ProcessingInstruction section
					gotel = true;	
				
					if ((e = raw.IndexOf("?>", s)) == -1)
					{
						error="Unmatched ProcessingInstruction start";
						readstate = ReadState.Error;
						return null;	// TODO - ???
					}					
					data = raw.Substring(s+2, e-2-2-s);
					
					newnode = 
						new XmlNode(mycurrentnode, myparent, XmlNodeType.ProcessingInstruction, data, depth);
					
					//Change current pointer
					mycurrentnode = newnode;						
				}	
				if (raw.IndexOf("<", s) == s && !gotel)
				{
					//Element section
					gotel = true;	
				
					if ((e = raw.IndexOf(">", s)) == -1)
					{
						error="Unmatched Element start";
						readstate = ReadState.Error;
						return null;	// TODO - ???
					}
					
					data = raw.Substring(s+1, e-1-1-s);
					
					newnode = 
						new XmlNode(mycurrentnode, myparent, XmlNodeType.Element, data, depth);
					
					//Change current pointer
					mycurrentnode = newnode;
					
					//Check if this element can have children (empty element check)
					//and raise depth if so
					if (raw[e-1] != '/')
					{
						depth++;
						myparent = newnode; //Set this element as the new parent
					}
				}	

				
			}			
						
			//Starts with &
			if (enc != -1 && enc < elc)
			{
				e = enc;
				
				//Cut of the beginning into a text node
				if (enc > 0)
				{						
					data = raw.Substring(s, e-s);
					
					newnode = 
						new XmlNode(null, mycurrentnode, XmlNodeType.Text, data, depth);
					
					//Change current pointer
					mycurrentnode = newnode;	
				}					

				//Search for ;
				s = e; 			
				if ((e = raw.IndexOf(';', s)) == -1)
				{
					error="; Missing in entity reference";
					readstate = ReadState.Error;
					return null;	// TODO - ???
				}
				
				data = raw.Substring(s, e-s);
					
				newnode = 
					new XmlNode(null, mycurrentnode, XmlNodeType.EntityReference, data, depth);
					
				//Change current pointer
				mycurrentnode = newnode;					

				//TODO: Parse the contents of the entityreference
			}
		}

#if __CSCC__
		// TODO - csc says that this code is unreachable - Rhys.
		// I need to implement a better reachability test.

		//Finished
		readstate = ReadState.Interactive;
		return myroot;		
#endif
	}
	
	private void HandleWhitespace(XmlNode mynode)
	{
		for (int x = 0; x < mynode.Text.Length; x++)
		{
			if (mynode.Text.IndexOfAny(WhitespaceChars, x, 1) == 0)
				return;
		}
		
		//Else
		mynode.Type = XmlNodeType.Whitespace;
	}
	
	private static readonly char[] WhitespaceChars =
	{'\u0009', '\u000A', '\u000B', '\u000C', '\u000D', '\u0020',
	 '\u00A0', '\u2001', '\u2002', '\u2003', '\u2004', '\u2005',
	 '\u2006', '\u2007', '\u2008', '\u2009', '\u200A', '\u200B',
	 '\u3000', '\uFEFF'};
	
}; //class XmlTextReader

}; //namespace System.Xml
