/*
 * XmlTextReader.cs - Implementation of the "System.Xml.XmlTextReader" class.
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
using System.IO;
using System.Text;

public class XmlTextReader : XmlReader
#if !ECMA_COMPAT
	, IXmlLineInfo
#endif
{
	// Internal state.
	private bool namespaces;
	private bool normalize;
	private char quoteChar;
	private XmlNameTable nameTable;
	private XmlNamespaceManager namespaceManager;
	private XmlDocument document;
	private ReadState readState;
	private WhitespaceHandling whitespace;
	private String xmlLang;
	private XmlSpace xmlSpace;
	private XmlResolver xmlResolver;
	private int lineNumber;
	private int linePosition;
	private TextReader reader;
	private String baseURI;
	private Encoding encoding;
	private int ungetch;
	private XmlNodeType nodeType;
	private String prefix;
	private String localName;
	private String namespaceURI;
	private String value;
	private XmlAttributeCollection attributes;
	private int attributeIndex;
	private int depth;
	private bool isEmpty;

	// Constructors.
	protected XmlTextReader()
			: this(new NameTable())
			{
				// Nothing to do here
			}
	protected XmlTextReader(XmlNameTable nt)
			{
				if(nt == null)
				{
					throw new ArgumentNullException("nt");
				}
				namespaces = true;
				normalize = false;
				quoteChar = '"';
				nameTable = nt;
				namespaceManager = new XmlNamespaceManager(nt);
				document = new XmlDocument(nt);
				readState = ReadState.Initial;
				whitespace = WhitespaceHandling.All;
				xmlLang = String.Empty;
				xmlSpace = XmlSpace.None;
				xmlResolver = new XmlUrlResolver();
				lineNumber = 1;
				linePosition = 0;
				baseURI = String.Empty;
				encoding = null;
				ungetch = -1;
				nodeType = XmlNodeType.None;
				prefix = String.Empty;
				localName = String.Empty;
				namespaceURI = String.Empty;
				value = String.Empty;
				attributes = null;
				attributeIndex = -1;
				depth = 1;
				isEmpty = false;
			}
	public XmlTextReader(Stream input)
			: this(String.Empty, input, new NameTable())
			{
				// Nothing to do here
			}
	public XmlTextReader(String url, Stream input)
			: this(url, input, new NameTable())
			{
				// Nothing to do here
			}
	public XmlTextReader(Stream input, XmlNameTable nt)
			: this(String.Empty, input, nt)
			{
				// Nothing to do here
			}
	public XmlTextReader(String url, Stream input, XmlNameTable nt)
			: this(nt)
			{
				if(input == null)
				{
					throw new ArgumentNullException("input");
				}
				if(url != null)
				{
					baseURI = nt.Add(url);
				}
				else
				{
					baseURI = String.Empty;
				}
				StreamReader sr = new StreamReader(input, true);
				encoding = sr.CurrentEncoding;
				reader = sr;
			}
	public XmlTextReader(TextReader input)
			: this(String.Empty, input, new NameTable())
			{
				// Nothing to do here
			}
	public XmlTextReader(String url, TextReader input)
			: this(url, input, new NameTable())
			{
				// Nothing to do here
			}
	public XmlTextReader(TextReader input, XmlNameTable nt)
			: this(String.Empty, input, nt)
			{
				// Nothing to do here
			}
	public XmlTextReader(String url, TextReader input, XmlNameTable nt)
			: this(nt)
			{
				if(url != null)
				{
					baseURI = nt.Add(url);
				}
				else
				{
					baseURI = String.Empty;
				}
				reader = input;
				if(input is StreamReader)
				{
					encoding = ((StreamReader)input).CurrentEncoding;
				}
			}
	[TODO]
	public XmlTextReader(Stream xmlFragment, XmlNodeType fragType,
						 XmlParserContext context)
			: this((context != null ? context.NameTable : new NameTable()))
			{
				if(xmlFragment == null)
				{
					throw new ArgumentNullException("xmlFragment");
				}
				if(fragType != XmlNodeType.Element &&
				   fragType != XmlNodeType.Attribute &&
				   fragType != XmlNodeType.Document)
				{
					throw new XmlException(S._("Xml_InvalidNodeType"));
				}
				// TODO
			}
	[TODO]
	public XmlTextReader(String xmlFragment, XmlNodeType fragType,
						 XmlParserContext context)
			: this((context != null ? context.NameTable : new NameTable()))
			{
				if(xmlFragment == null)
				{
					throw new ArgumentNullException("xmlFragment");
				}
				if(fragType != XmlNodeType.Element &&
				   fragType != XmlNodeType.Attribute &&
				   fragType != XmlNodeType.Document)
				{
					throw new XmlException(S._("Xml_InvalidNodeType"));
				}
				// TODO
			}
	public XmlTextReader(String url)
			: this(url, new NameTable())
			{
				// Nothing to do here
			}
	[TODO]
	public XmlTextReader(String url, XmlNameTable nt)
			: this(nt)
			{
				if(url == null)
				{
					throw new ArgumentNullException("url");
				}
				// TODO - handle non-file streams
				if(url != null)
				{
					baseURI = nt.Add(url);
				}
				else
				{
					baseURI = String.Empty;
				}
				StreamReader sr = new StreamReader(url, true);
				encoding = sr.CurrentEncoding;
				reader = sr;
			}

	// Clean up the resources that were used by this XML reader.
	[TODO]
	public override void Close()
			{
				// TODO
			}

	// Returns the value of an attribute with a specific index.
	public override String GetAttribute(int i)
			{
				if(attributes != null)
				{
					if(i >= 0 && i < attributes.Count)
					{
						return attributes[i].Value;
					}
				}
				throw new ArgumentOutOfRangeException
					("i", S._("Xml_InvalidAttributeIndex"));
			}

	// Returns the value of an attribute with a specific name.
	public override String GetAttribute(String name, String namespaceURI)
			{
				XmlAttribute attribute;
				if(attributes != null)
				{
					attribute = attributes[name, namespaceURI];
					if(attribute != null)
					{
						return attribute.Value;
					}
				}
				return null;
			}

	// Returns the value of an attribute with a specific qualified name.
	public override String GetAttribute(String name)
			{
				XmlAttribute attribute;
				if(attributes != null)
				{
					attribute = attributes[name];
					if(attribute != null)
					{
						return attribute.Value;
					}
				}
				return null;
			}

	// Get the remainder of the the current XML stream.
	[TODO]
	public TextReader GetRemainder()
			{
				// TODO
				return null;
			}

	// Resolve a namespace in the scope of the current element.
	public override String LookupNamespace(String prefix)
			{
				return namespaceManager.LookupNamespace(prefix);
			}

	// Move the current position to a particular attribute.
	public override void MoveToAttribute(int i)
			{
				if(attributes != null)
				{
					if(i >= 0 && i < attributes.Count)
					{
						attributeIndex = i;
						return;
					}
				}
				throw new ArgumentOutOfRangeException
					("i", S._("Xml_InvalidAttributeIndex"));
			}

	// Move the current position to an attribute with a particular name.
	public override bool MoveToAttribute(String name, String ns)
			{
				XmlAttribute attribute;
				if(attributes != null)
				{
					attribute = attributes[name, ns];
					if(attribute != null)
					{
						attributeIndex = attributes.IndexOf(attribute);
						return true;
					}
				}
				return false;
			}

	// Move the current position to an attribute with a qualified name.
	public override bool MoveToAttribute(String name)
			{
				XmlAttribute attribute;
				if(attributes != null)
				{
					attribute = attributes[name];
					if(attribute != null)
					{
						attributeIndex = attributes.IndexOf(attribute);
						return true;
					}
				}
				return false;
			}

	// Move to the element that owns the current attribute.
	public override bool MoveToElement()
			{
				if(attributeIndex != -1)
				{
					attributeIndex = -1;
					return true;
				}
				else
				{
					return false;
				}
			}

	// Move to the first attribute owned by the current element.
	public override bool MoveToFirstAttribute()
			{
				if(attributes != null && attributes.Count > 0)
				{
					attributeIndex = 0;
					return true;
				}
				return false;
			}

	// Move to the next attribute owned by the current element.
	public override bool MoveToNextAttribute()
			{
				int index;
				if(attributeIndex != -1)
				{
					if(attributes != null)
					{
						index = attributeIndex + 1;
						if(index <= 0 || index >= attributes.Count)
						{
							return false;
						}
						else
						{
							attributeIndex = index;
							return true;
						}
					}
					else
					{
						return false;
					}
				}
				else
				{
					return MoveToFirstAttribute();
				}
			}

	// Read the next character.
	private int ReadChar()
			{
				++linePosition;
				if(ungetch != -1)
				{
					int ch = ungetch;
					ungetch = -1;
					return ch;
				}
				else
				{
					return reader.Read();
				}
			}

	// Clear the node information.
	private void ClearNodeInfo()
			{
				nodeType = XmlNodeType.None;
				prefix = String.Empty;
				localName = String.Empty;
				namespaceURI = String.Empty;
				value = String.Empty;
				attributes = null;
				attributeIndex = -1;
				isEmpty = false;
			}

	// Unget the last character and throw a read error.
	private void UngetAndThrow(int ch)
			{
				--linePosition;
				if(ch != -1)
				{
					ungetch = ch;
				}
				readState = ReadState.Error;
				ClearNodeInfo();
				throw new XmlException(S._("Xml_ReaderError"));
			}

	// Read an identifier from the input stream.
	private String ReadIdentifier()
			{
				// TODO
				return null;
			}

	// Set the name information from an identifier.
	private void SetName(String identifier)
			{
				int index = identifier.LastIndexOf(':');
				if(index >= 0)
				{
					prefix = nameTable.Add(identifier.Substring(0, index));
					localName = nameTable.Add(identifier.Substring(index + 1));
				}
				else
				{
					prefix = String.Empty;
					localName = nameTable.Add(identifier);
				}
				namespaceURI = String.Empty;
			}

	// Skip white space characters.
	private void SkipWhite()
			{
				int ch;
				while((ch = ReadChar()) != -1)
				{
					if(!Char.IsWhiteSpace((char)ch))
					{
						ungetch = ch;
						break;
					}
				}
			}

	// Expect the next character to be a certain value.
	private void Expect(char expected)
			{
				int ch = ReadChar();
				if(ch != expected)
				{
					UngetAndThrow(ch);
				}
			}

	// Read the next node in the input stream.
	[TODO]
	public override bool Read()
			{
				int ch;
				StringBuilder builder;
				String name;
				int count;

				// Validate the current state of the stream.
				if(readState == ReadState.EndOfFile)
				{
					return false;
				}
				else if(reader == null)
				{
					throw new XmlException(S._("Xml_ReaderClosed"));
				}
				else if(readState == ReadState.Error)
				{
					throw new XmlException(S._("Xml_ReaderError"));
				}

				// Skip white space in the input stream.  TODO: collect
				// up significant white space.
				while((ch = ReadChar()) != -1)
				{
					if(!Char.IsWhiteSpace((char)ch))
					{
						break;
					}
					if(ch == '\n')
					{
						++lineNumber;
						linePosition = 0;
					}
				}
				if(ch == -1)
				{
					// We've reached the end of the stream.  Throw
					// an error if we haven't closed all elements.
					if(linePosition > 1)
					{
						--linePosition;
					}
					readState = ReadState.EndOfFile;
					ClearNodeInfo();
					return false;
				}

				// Determine what to do based on the next character.
				if(ch == '<')
				{
					// Some kind of tag.
					ch = ReadChar();
					if(ch == '/')
					{
						// End element tag.
						name = ReadIdentifier();
						SkipWhite();
						Expect('>');
						nodeType = XmlNodeType.Element;
						SetName(name);
						value = String.Empty;
						attributes = null;
						attributeIndex = -1;
						isEmpty = false;
						--depth;
					}
					else if(ch == '!')
					{
						// Comment, CDATA, or document type information.
						ch = ReadChar();
						if(ch == '-')
						{
							// Parse the "<!--" comment start sequence.
							ch = ReadChar();
							if(ch != '-')
							{
								UngetAndThrow(ch);
							}

							// Search for the "-->" comment end sequence.
							builder = new StringBuilder();
							count = 0;
							while((ch = ReadChar()) != -1)
							{
								builder.Append((char)ch);
								if(ch == '-')
								{
									++count;
								}
								else if(ch == '>')
								{
									if(count >= 2)
									{
										builder.Remove(builder.Length - 3, 3);
										break;
									}
									count = 0;
								}
								else
								{
									count = 0;
								}
								if(ch == '\n')
								{
									++lineNumber;
									linePosition = 0;
								}
							}
							if(ch != '>')
							{
								UngetAndThrow(ch);
							}

							// Create a comment node and return.
							ClearNodeInfo();
							nodeType = XmlNodeType.Comment;
							value = builder.ToString();
							return true;
						}
						else if(ch == '[')
						{
							// TODO: CDATA nodes.
						}
						else if(ch == 'D')
						{
							// TODO: document type nodes.
						}
						else
						{
							UngetAndThrow(ch);
						}
					}
					else if(ch == '?')
					{
						// Processing instruction.
						// TODO
					}
					else if(XmlConvert.IsNameStart((char)ch, true))
					{
						// Start element tag.
						// TODO
					}
					else
					{
						UngetAndThrow(ch);
					}
				}
				else
				{
					// Text and entity references.
					// TODO
				}

				return false;
			}

	// Read the next attribute value in the input stream.
	[TODO]
	public override bool ReadAttributeValue()
			{
				// TODO
				return false;
			}

	// Read base64 data from the current element.
	[TODO]
	public int ReadBase64(byte[] array, int offset, int len)
			{
				// TODO
				return 0;
			}

	// Read binhex data from the current element.
	[TODO]
	public int ReadBinHex(byte[] array, int offset, int len)
			{
				// TODO
				return 0;
			}

	// Read character data from the current element.
	[TODO]
	public int ReadChars(byte[] array, int index, int count)
			{
				// TODO
				return 0;
			}

	// Read the contents of the current node, including all markup.
	[TODO]
	public override String ReadInnerXml()
			{
				// TODO
				return null;
			}

	// Read the current node, including all markup.
	[TODO]
	public override String ReadOuterXml()
			{
				// TODO
				return null;
			}

	// Read the contents of an element or text node as a string.
	[TODO]
	public override String ReadString()
			{
				// TODO
				return null;
			}

	// Reset to the initial state.
	[TODO]
	public void ResetState()
			{
				// TODO
			}

	// Resolve an entity reference.
	[TODO]
	public override void ResolveEntity()
			{
				// TODO
			}

	// Get the number of attributes on the current node.
	public override int AttributeCount
			{
				get
				{
					if(attributes != null)
					{
						return attributes.Count;
					}
					else
					{
						return 0;
					}
				}
			}

	// Get the base URI for the current node.
	public override String BaseURI
			{
				get
				{
					return baseURI;
				}
			}

	// Get the depth of the current node.
	public override int Depth
			{
				get
				{
					if(attributeIndex == -1)
					{
						return depth;
					}
					else
					{
						return depth + 1;
					}
				}
			}

	// Determine if we have reached the end of the input stream.
	public override bool EOF
			{
				get
				{
					return (readState == ReadState.EndOfFile);
				}
			}

	// Get the encoding that is in use by the XML stream.
	public Encoding Encoding
			{
				get
				{
					if(readState == ReadState.Interactive)
					{
						return encoding;
					}
					else
					{
						return null;
					}
				}
			}

	// Determine if the current node can have an associated text value.
	public override bool HasValue
			{
				get
				{
					switch(NodeType)
					{
						case XmlNodeType.Attribute:
						case XmlNodeType.CDATA:
						case XmlNodeType.Comment:
						case XmlNodeType.DocumentType:
						case XmlNodeType.ProcessingInstruction:
						case XmlNodeType.SignificantWhitespace:
						case XmlNodeType.Text:
						case XmlNodeType.Whitespace:
						case XmlNodeType.XmlDeclaration:
							return true;
						default: break;
					}
					return false;
				}
			}

	// Get the current node as an attribute, if appropriate.
	private XmlAttribute GetAsAttribute()
			{
				if(attributes != null && attributeIndex != -1)
				{
					return attributes[attributeIndex];
				}
				else
				{
					return null;
				}
			}

	// Determine if the current node's value was generated from a DTD default.
	public override bool IsDefault
			{
				get
				{
					XmlAttribute attr = GetAsAttribute();
					if(attr != null)
					{
						return !(attr.Specified);
					}
					return false;
				}
			}

	// Determine if the current node is an empty element.
	public override bool IsEmptyElement
			{
				get
				{
					if(nodeType == XmlNodeType.Element)
					{
						return isEmpty;
					}
					else
					{
						return false;
					}
				}
			}

	// Retrieve an attribute value with a specified index.
	public override String this[int i]
			{
				get
				{
					return GetAttribute(i);
				}
			}

	// Retrieve an attribute value with a specified name.
	public override String this[String localname, String namespaceURI]
			{
				get
				{
					return GetAttribute(localname, namespaceURI);
				}
			}

	// Retrieve an attribute value with a specified qualified name.
	public override String this[String name]
			{
				get
				{
					return GetAttribute(name);
				}
			}

#if !ECMA_COMPAT
	// Determine if we have line information.
	bool IXmlLineInfo.HasLineInfo()
			{
				return true;
			}
#endif

	// Get the current line number.
	public int LineNumber
			{
				get
				{
					return lineNumber;
				}
			}

	// Get the current line position.
	public int LinePosition
			{
				get
				{
					if(linePosition > 0)
					{
						return linePosition;
					}
					else
					{
						return 0;
					}
				}
			}

	// Get the local name of the current node.
	public override String LocalName
			{
				get
				{
					XmlAttribute attr = GetAsAttribute();
					if(attr != null)
					{
						return attr.LocalName;
					}
					else
					{
						return localName;
					}
				}
			}

	// Get the fully-qualified name of the current node.
	public override String Name
			{
				get
				{
					XmlAttribute attr = GetAsAttribute();
					if(attr != null)
					{
						return attr.Name;
					}
					else if(prefix != String.Empty)
					{
						return nameTable.Add(prefix + ":" + localName);
					}
					else
					{
						return localName;
					}
				}
			}

	// Get the name that that is used to look up and resolve names.
	public override XmlNameTable NameTable
			{
				get
				{
					return nameTable;
				}
			}

	// Get the namespace URI associated with the current node.
	public override String NamespaceURI
			{
				get
				{
					XmlAttribute attr = GetAsAttribute();
					if(attr != null)
					{
						return attr.NamespaceURI;
					}
					else
					{
						return namespaceURI;
					}
				}
			}

	// Get or set the "namespace support" flag for this reader.
	public bool Namespaces
			{
				get
				{
					return namespaces;
				}
				set
				{
					if(readState != ReadState.Initial)
					{
						throw new InvalidOperationException
							(S._("Xml_InvalidReadState"));
					}
					namespaces = value;
				}
			}

	// Get or set the "normalize" flag for this reader.
	public bool Normalization
			{
				get
				{
					return normalize;
				}
				set
				{
					if(readState == ReadState.Closed)
					{
						throw new InvalidOperationException
							(S._("Xml_InvalidReadState"));
					}
					normalize = value;
				}
			}

	// Get the type of the current node.
	public override XmlNodeType NodeType
			{
				get
				{
					if(attributeIndex != -1)
					{
						return XmlNodeType.Attribute;
					}
					else
					{
						return nodeType;
					}
				}
			}

	// Get the namespace prefix associated with the current node.
	public override String Prefix
			{
				get
				{
					XmlAttribute attr = GetAsAttribute();
					if(attr != null)
					{
						return attr.Prefix;
					}
					else
					{
						return prefix;
					}
				}
			}

	// Get the quote character that was used to enclose an attribute value.
	public override char QuoteChar
			{
				get
				{
					return quoteChar;
				}
			}

	// Get the current read state of the reader.
	public override ReadState ReadState
			{
				get
				{
					return readState;
				}
			}

	// Get the text value of the current node.
	public override String Value
			{
				get
				{
					XmlAttribute attr = GetAsAttribute();
					if(attr != null)
					{
						return attr.Value;
					}
					else
					{
						return value;
					}
				}
			}

	// Get or set the whitespace handling flag.
	public WhitespaceHandling WhitespaceHandling
			{
				get
				{
					return whitespace;
				}
				set
				{
					if(value != WhitespaceHandling.All &&
					   value != WhitespaceHandling.Significant &&
					   value != WhitespaceHandling.None)
					{
						throw new ArgumentOutOfRangeException
							("value", S._("Xml_InvalidWhitespaceHandling"));
					}
					if(readState == ReadState.Closed)
					{
						throw new InvalidOperationException
							(S._("Xml_InvalidReadState"));
					}
					whitespace = value;
				}
			}

	// Get the current xml:lang scope.
	public override String XmlLang
			{
				get
				{
					return xmlLang;
				}
			}

	// Set the resolver to use to resolve DTD references.
	public XmlResolver XmlResolver
			{
				set
				{
					xmlResolver = value;
				}
			}

	// Get the current xml:space scope.
	public override XmlSpace XmlSpace
			{
				get
				{
					return xmlSpace;
				}
			}

}; // class XmlTextReader

}; // namespace System.Xml
