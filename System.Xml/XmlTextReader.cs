/*
 * XmlTextReader.cs - Implementation of the "System.Xml.XmlTextReader" class.
 *
 * Copyright (C) 2002  Southern Storm Software, Pty Ltd.
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
 
namespace System.Xml
{

using System;
using System.IO;
using System.Text;
using System.Collections;
using System.Globalization;

public class XmlTextReader : XmlReader
#if !ECMA_COMPAT
	, IXmlLineInfo
#endif
{
	// Internal state.
	private bool contextSupport;
	private bool incDepth;
	private bool namespaces;
	private bool normalize;
	private int depth;
	private ReadState readState;
	private Stack elementNames;
	private WhitespaceHandling whitespace;
	private XmlElementInfo info;
	private XmlParserContext parserContext;
	private XmlParserInput input;
	private XmlResolver xmlResolver;
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
				depth = 0;
				readState = ReadState.Initial;
				whitespace = WhitespaceHandling.All;
				parserContext = new XmlParserContext
					(nt, new XmlNamespaceManager(nt), String.Empty, XmlSpace.None);
				xmlResolver = new XmlUrlResolver();

				contextSupport = false;
				incDepth = false;
				elementNames = new Stack();
				input = new XmlParserInput(new EOFHandler(HandleEOF), null);
				info = new XmlElementInfo(parserContext);
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
				if(url == null)
				{
					parserContext.BaseURI = String.Empty;
				}
				else
				{
					parserContext.BaseURI = nt.Add(url);
				}
				XmlStreamReader sr = new XmlStreamReader(input, true);
				this.input.Reader = sr;
				parserContext.Encoding = sr.CurrentEncoding;
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
				if(url == null)
				{
					parserContext.BaseURI = String.Empty;
				}
				else
				{
					parserContext.BaseURI = nt.Add(url);
				}
				this.input.Reader = input;
				if(input as StreamReader != null)
				{
					parserContext.Encoding = ((StreamReader)input).CurrentEncoding;
				}
			}
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

				if(context == null)
				{
					XmlStreamReader sr = new XmlStreamReader(xmlFragment, true);
					input.Reader = sr;
					parserContext.Encoding = sr.CurrentEncoding;
					contextSupport = false;
				}
				else
				{
					parserContext.BaseURI = context.BaseURI;
					parserContext.DocTypeName = context.DocTypeName;
					parserContext.InternalSubset = context.InternalSubset;
					parserContext.NamespaceManager = context.NamespaceManager;
					parserContext.PublicId = context.PublicId;
					parserContext.SystemId = context.SystemId;
					parserContext.XmlLang = context.XmlLang;
					parserContext.XmlSpace = context.XmlSpace;
					parserContext.NameCollection = context.NameCollection;
					contextSupport = true;
					XmlStreamReader sr = new XmlStreamReader(xmlFragment, true);
					if(context.Encoding == null)
					{
						parserContext.Encoding = sr.CurrentEncoding;
					}
					else
					{
						parserContext.Encoding = context.Encoding;
					}
					input.Reader = sr;
				}
				namespaces = false;
			}
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

				if(context == null)
				{
					parserContext.BaseURI = String.Empty;
					XmlStreamReader sr = new XmlStreamReader(new StringReader(xmlFragment));
					input.Reader = sr.TxtReader;
					parserContext.Encoding = sr.CurrentEncoding;
				}
				else
				{
					parserContext.BaseURI = context.BaseURI;
					parserContext.XmlLang = context.XmlLang;
					parserContext.XmlSpace = context.XmlSpace;
					contextSupport = true;
					XmlStreamReader sr = new XmlStreamReader(new StringReader(xmlFragment));
					if(context.Encoding == null)
					{
						parserContext.Encoding = sr.CurrentEncoding;
					}
					else
					{
						parserContext.Encoding = context.Encoding;
					}
					input.Reader = sr.TxtReader;
				}
				namespaces = false;
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
				if(url == null) { throw new XmlException("url"); }
				// TODO - handle non-file streams
				XmlStreamReader sr = new XmlStreamReader(url, true);
				input.Reader = sr;
				parserContext.BaseURI = nt.Add(url);
				parserContext.Encoding = sr.CurrentEncoding;
				parserContext.NameTable = nt;
			}

	// Get the number of attributes on the current node.
	public override int AttributeCount
			{
				get { return info.AttributeCount; }
			}

	// Get the base URI for the current node.
	public override String BaseURI
			{
				get { return parserContext.BaseURI; }
			}

	// Get the depth of the current node.
	public override int Depth
			{
				get { return depth + info.DepthOffset; }
			}

	// Determine if we have reached the end of the input stream.
	public override bool EOF
			{
				get { return (readState == ReadState.EndOfFile); }
			}

	// Get the encoding that is in use by the XML stream.
	public Encoding Encoding
			{
				get
				{
					if(readState == ReadState.Interactive)
					{
						return parserContext.Encoding;
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
						default:
							return false;
					}
				}
			}

	// Determine if the current node's value was generated from a DTD default.
	public override bool IsDefault
			{
				get { return false; }
			}

	// Determine if the current node is an empty element.
	public override bool IsEmptyElement
			{
				get { return info.IsEmptyElement; }
			}

	// Retrieve an attribute value with a specified index.
	public override String this[int i]
			{
				get { return GetAttribute(i); }
			}

	// Retrieve an attribute value with a specified name.
	public override String this[String localname, String namespaceURI]
			{
				get { return GetAttribute(localname, namespaceURI); }
			}

	// Retrieve an attribute value with a specified qualified name.
	public override String this[String name]
			{
				get { return GetAttribute(name); }
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
				get { return input.LineNumber; }
			}

	// Get the current line position.
	public int LinePosition
			{
				get { return input.LinePosition; }
			}

	// Get the local name of the current node.
	public override String LocalName
			{
				get { return info.Current.LocalName; }
			}

	// Get the fully-qualified name of the current node.
	public override String Name
			{
				get { return info.Current.Name; }
			}

	// Get the name table that is used to look up and resolve names.
	public override XmlNameTable NameTable
			{
				get { return parserContext.NameTable; }
			}

	// Get the namespace URI associated with the current node.
	public override String NamespaceURI
			{
				get { return info.Current.NamespaceURI; }
			}

	// Get or set the "namespace support" flag for this reader.
	public bool Namespaces
			{
				get { return namespaces; }
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
				get { return normalize; }
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
				get { return info.NodeType; }
			}

	// Get the namespace prefix associated with the current node.
	public override String Prefix
			{
				get { return info.Current.Prefix; }
			}

	// Get the quote character that was used to enclose an attribute value.
	public override char QuoteChar
			{
				get { return info.Current.QuoteChar; }
			}

	// Get the current read state of the reader.
	public override ReadState ReadState
			{
				get { return readState; }
			}

	// Get the text value of the current node.
	public override String Value
			{
				get { return info.Current.Value; }
			}

	// Get or set the whitespace handling flag.
	public WhitespaceHandling WhitespaceHandling
			{
				get { return whitespace; }
				set
				{
					if(!Enum.IsDefined(typeof(WhitespaceHandling), value))
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
				get { return parserContext.XmlLang; }
			}

	// Set the resolver to use to resolve DTD references.
	public XmlResolver XmlResolver
			{
				set { xmlResolver = value; }
			}

	// Get the current xml:space scope.
	public override XmlSpace XmlSpace
			{
				get { return parserContext.XmlSpace; }
			}

	internal XmlParserContext ParserContext 
			{
				get { return parserContext; }
			}

	// Clean up the resources that were used by this XML reader.
	[TODO] 	
	public override void Close()
			{
				/* TODO: Clean up resrouces used by reader */
				parserContext.SystemId = String.Empty;
				parserContext.PublicId = String.Empty;
				depth = 0;
				contextSupport = false;
				input.Close();
			}

	// Returns the value of an attribute with a specific index.
	public override String GetAttribute(int i)
			{
				XmlNodeInfo tmp = info.GetAttribute(i);
				if(tmp == null)
				{
					throw new ArgumentOutOfRangeException
						("i", S._("Xml_InvalidAttributeIndex"));
				}
				return tmp.Value;
			}

	// Returns the value of an attribute with a specific name.
	public override String GetAttribute(String localName, String namespaceURI)
			{
				XmlNodeInfo tmp = info.GetAttribute(localName, namespaceURI);
				if(tmp == null) { return null; }
				return tmp.Value;
			}

	// Returns the value of an attribute with a specific qualified name.
	public override String GetAttribute(String name)
			{
				XmlNodeInfo tmp = info.GetAttribute(name);
				if(tmp == null) { return null; }
				return tmp.Value;
			}

	// Get the remainder of the current XML stream.
	[TODO] 
	public TextReader GetRemainder()
			{
				String tmp = input.Reader.ReadToEnd();
				StringReader s = new StringReader(tmp);
				Close();
				readState = ReadState.EndOfFile;
				return s;
			}

	// Resolve a namespace in the scope of the current element.
	public override String LookupNamespace(String prefix)
			{
				if(!namespaces) { return String.Empty; }
				if(prefix == null) { throw new ArgumentNullException("prefix"); }
				return parserContext.NamespaceManager.LookupNamespace(prefix);
			}

	// Move the current position to a particular attribute.
	public override void MoveToAttribute(int i)
			{
				if(!info.MoveToAttribute(i))
				{
					throw new ArgumentOutOfRangeException
						("i", S._("Xml_InvalidAttributeIndex"));
				}
			}

	// Move the current position to an attribute with a particular name.
	public override bool MoveToAttribute(String localName, String namespaceURI)
			{
				return info.MoveToAttribute(localName, namespaceURI);
			}

	// Move the current position to an attribute with a qualified name.
	public override bool MoveToAttribute(String name)
			{
				return info.MoveToAttribute(name);
			}

	// Move to the element that owns the current attribute.
	public override bool MoveToElement()
			{
				return info.MoveToElement();
			}

	// Move to the first attribute owned by the current element.
	public override bool MoveToFirstAttribute()
			{
				return info.MoveToFirstAttribute();
			}

	// Move to the next attribute owned by the current element.
	public override bool MoveToNextAttribute()
			{
				return info.MoveToNextAttribute();
			}

	// Read the next node in the input stream. -- This should mimic mono's interpretation of Read()
	public override bool Read()
			{
				// Validate the current state of the stream.
				switch(readState)
				{
					case ReadState.EndOfFile:
					{
						return false;
					}
					// Not reached.

					case ReadState.Closed:
					{
						throw new XmlException(S._("Xml_ReaderClosed"));
					}
					// Not reached.

					case ReadState.Error:
					{
						throw new XmlException(S._("Xml_ReaderError"));
					}
					// Not reached.

					case ReadState.Initial:
					{
						info.ClearElementInfo();
						readState = ReadState.Interactive;
					}
					break;
				}
				return ReadDocument();
			}

	// Read the next attribute value in the input stream.
	public override bool ReadAttributeValue()
			{

				if(readState != ReadState.Interactive)
				{
					throw new XmlException
						(S._("Xml_WrongReadState"));
				}
				return info.ReadAttributeValue();
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
	public int ReadChars(char[] buffer, int index, int count)
			{
				if(count > buffer.Length - index)
				{
					throw new ArgumentException
						(S._("ArgumentExceedsLength"), "count");
				}
				if(buffer == null)
				{
					throw new ArgumentNullException
						(S._("ArgumentIsNull"), "buffer");
				}
				if(index < 0 || count < 0)
				{
					throw new ArgumentOutOfRangeException
						(S._("ArgumentOutOfRange"));
				}

				XmlNodeType nodeType = NodeType;
				if(nodeType != XmlNodeType.Text && nodeType != XmlNodeType.Element)
				{
					throw new XmlException
						(S._("Xml_WrongNodeType"));
				}

				/*
				if(nodeType != XmlNodeType.Text)
				{
					// if we are not at a text node, move to current element's content
					MoveToContent();
				}


				Read();

				int length = value.IndexOf("<");

				// go back to '<' position
				linePosition -= value.Length - length;

				String output = value.Substring(0, length);

				// read text now
				if(output.Length < count)
				{
					Array.Copy(output.ToCharArray(0, output.Length), 0, buffer, index, output.Length);
				}
				else
				{
					Array.Copy(output.ToCharArray(0, count), 0, buffer, index, count);
				}
				*/
				return 0;
			}

	// Read the contents of the current node, including all markup.
	[TODO] 
	public override String ReadInnerXml()
			{
				XmlNodeType nodeType = NodeType;
				if(nodeType != XmlNodeType.Element && nodeType != XmlNodeType.Attribute)
				{
					throw new XmlException
						(S._("Xml_WrongNodeType"));
				}

				/*
				if(nodeType == XmlNodeType.Element)
				{
					StringBuilder sb = new StringBuilder();
					char[] target = Name.ToCharArray();
					MoveToContent();
					while(input.NextChar() && input.PeekChar())
					{
						sb.Append(input.currChar);
						if(input.currChar == '<' && input.peekChar == '/')
						{
							input.NextChar();
							int i = 0;
							bool match = true;
							while(match && i < target.Length && input.NextChar())
							{
								sb.Append(input.currChar);
								if(input.currChar != target[i])
								{
									match = false;
								}
								++i;
							}
							if(match && input.NextChar())
							{
								if(input.currChar == '>')
								{
									int len = sb.Length - (target.Length + 2);
									return sb.ToString(0, len);
								}
								else
								{
									sb.Append(input.currChar);
								}
							}
						}
					}
					return String.Empty;
				}
				else if(nodeType == XmlNodeType.Attribute)
				{
					Read();
					return value;
				}
				*/
				return String.Empty;
			}

	// Read the current node, including all markup.
	[TODO] 	
	public override String ReadOuterXml()
			{
				StringBuilder sb = new StringBuilder();
				MoveToContent();
				if(NodeType == XmlNodeType.Element)
				{
					char[] target = Name.ToCharArray();
					sb.Append("<" + Name);
					int count = info.AttributeCount;
					XmlNodeInfo tmp;
					for (int i = 0; i < count; ++i)
					{
						tmp = info.GetAttribute(i);
						String name = tmp.Name;
						String value = tmp.Value;
						char quoteChar = tmp.QuoteChar;

						sb.Append(" ");
						sb.Append(name);
						sb.Append("=");
						sb.Append(quoteChar);
						sb.Append(value);
						sb.Append(quoteChar);
					}
					sb.Append(">");
					while(input.NextChar() && input.PeekChar())
					{
						sb.Append(input.currChar);
						if(input.currChar == '<' && input.peekChar == '/')
						{
							input.NextChar();
							sb.Append(input.currChar);
							int i = 0;
							bool match = true;
							while(match && i < target.Length && input.NextChar())
							{
								sb.Append(input.currChar);
								if(input.currChar != target[i])
								{
									match = false;
								}
								++i;
							}
							if(match && input.NextChar())
							{
								sb.Append(input.currChar);
								if(input.currChar == '>')
								{
									return sb.ToString();
								}
							}
						}
					}
				}
				else if(NodeType == XmlNodeType.Attribute)
				{
					Read();
					XmlNodeInfo node = info.Current;
					String name = node.Name;
					String value = node.Value;
					char quoteChar = node.QuoteChar;
					
					sb.Append(name);
					sb.Append("=");
					sb.Append(quoteChar);
					sb.Append(value);
					sb.Append(quoteChar);
					return sb.ToString();
				}
				return String.Empty;
			}

	// Read the contents of an element or text node as a string.
	[TODO]
	public override String ReadString()
			{
				// TODO
				return null;
			}

	// Reset to the initial state.
	public void ResetState()
			{
				if(contextSupport)
				{
					throw new InvalidOperationException
						(S._("Xml_ContextNotNull"));
				}

				readState = ReadState.Initial;
			}

	// Resolve an entity reference.
	public override void ResolveEntity()
			{
				throw new InvalidOperationException(
						S._("Xml_CannotResolveEntity"));
			}

	// Clear the node information.
	[TODO] 	
	private void ClearNodeInfo()
			{
				parserContext.SystemId = String.Empty;
				parserContext.PublicId = String.Empty;
				info.ClearElementInfo();
			}

	// Enter the error state, and throw an XmlException.
	private void Error()
			{
				Error("Xml_ReaderError");
			}
	private void Error(String messageTag)
			{
				readState = ReadState.Error;
				ClearNodeInfo();
				throw new XmlException(S._(messageTag));
			}
	// Expect the next character to match `expected'.
	private void Expect(char expected)
			{
				if(!input.NextChar()) { Error("Xml_UnexpectedEOF"); }
				if(input.currChar != expected) { Error(/* TODO */); }
			}
	// Expect the stream characters to match `expected'.
	private void Expect(String expected)
			{
				int len = expected.Length;
				char[] c = expected.ToCharArray();
				for(int i = 0; i < len; ++i)
				{
					if(!input.NextChar()) { Error("Xml_UnexpectedEOF"); }
					if(input.currChar != c[i]) { Error(/* TODO */); }
				}
			}

	// Get the current character.
	private char GetCurrentChar()
			{
				return input.currChar;
			}

	[TODO]
	private void HandleEOF()
			{
				readState = ReadState.EndOfFile;
			}

	private static bool IsChar(char c)
			{
				return c == '\t' ||
				       c == '\n' ||
				       c == '\r' ||
				       (c >= ' ' &&
				        (Char.GetUnicodeCategory(c) != UnicodeCategory.Surrogate));
			}
	private static bool IsNameChar(char c)
			{
				if(c == '.' || c == '-' || c == '_' || c == ':')
				{
					return true;
				}
				switch(Char.GetUnicodeCategory(c))
				{
					case UnicodeCategory.UppercaseLetter:
					case UnicodeCategory.LowercaseLetter:
					case UnicodeCategory.TitlecaseLetter:
					case UnicodeCategory.OtherLetter:
					case UnicodeCategory.LetterNumber:
					case UnicodeCategory.SpacingCombiningMark:
					case UnicodeCategory.EnclosingMark:
					case UnicodeCategory.NonSpacingMark:
					case UnicodeCategory.ModifierLetter:
					case UnicodeCategory.DecimalDigitNumber:
						return true;
					default:
						return false;
				}
			}
	private static bool IsNameInit(char c)
			{
				if(c == '_' || c == ':')
				{
					return true;
				}
				switch(Char.GetUnicodeCategory(c))
				{
					case UnicodeCategory.UppercaseLetter:
					case UnicodeCategory.LowercaseLetter:
					case UnicodeCategory.TitlecaseLetter:
					case UnicodeCategory.OtherLetter:
					case UnicodeCategory.LetterNumber:
						return true;
					default:
						return false;
				}
			}
	private static bool IsPublicId(char c)//PubidChar
			{
				return (c != '"' && c != '&' && c != '<' && c != '>') &&
				       (c == '\n' || c == '\r' || c == '_' ||
				        (c >= ' ' && c <= 'Z') || (c >= 'a' && c <= 'z'));
			}
	private static bool IsWhitespace(char c)
			{
				return (c == '\t' || c == '\n' || c == '\r' || c == ' ');
			}


#if !ECMA_COMPAT
	private String ParseEntity(String entityref)
			{
				return parserContext.NameCollection.Get(entityref);
			}
#endif
	[TODO]
	private void ReadAttributes(StringBuilder sb, bool qmark)
			{
				ArrayList refs = new ArrayList();
				while(true)
				{
					bool hasWS = false;
					while(input.PeekChar() && IsWhitespace(input.peekChar))
					{
						input.NextChar();
						sb.Append(input.currChar);
						hasWS = true;
					}
					if(!input.PeekChar()) { Error("Xml_UnexpectedEOF"); }
					if(qmark)
					{
						if(input.peekChar == '?') { return; }
					}
					else if(input.peekChar == '/' || input.peekChar == '>')
					{
						return;
					}
					if(!hasWS) { Error(/* TODO */); }
					String name = ReadName(sb);
					while(input.PeekChar() && IsWhitespace(input.peekChar))
					{
						input.NextChar();
						sb.Append(input.currChar);
					}
					Expect('=');
					sb.Append('=');
					while(input.PeekChar() && IsWhitespace(input.peekChar))
					{
						input.NextChar();
						sb.Append(input.currChar);
					}
					if(!input.NextChar()) { Error("Xml_UnexpectedEOF"); }
					char quoteChar = input.currChar;
					if(quoteChar != '"' && quoteChar != '\'') { Error(/* TODO */); }
					sb.Append(quoteChar);
					int valuePos = sb.Length;
					int start = 0;
					refs.Clear();
					while(input.NextChar() && input.currChar != quoteChar)
					{
						++start;
						switch(input.currChar)
						{
							case '<':
							{
								Error(/* TODO */);
								return; // keep the compiler happy
							}
							// Not reached.

							case '&':
							{
								if(ReadReference(sb)) { refs.Add(start); }
							}
							break;

							default:
							{
								sb.Append(input.currChar);
							}
							break;
						}
					}
					if(input.currChar != quoteChar)
					{
						Error("Xml_UnexpectedEOF");
					}
					String value = sb.ToString(valuePos, sb.Length - valuePos);
					sb.Append(quoteChar);
					info.AddAttribute(name, value, quoteChar, (int[])refs.ToArray(typeof(int)));
				}
			}

	private bool ReadDocument()
			{
				if(!input.PeekChar())
				{
					// TODO: EOF Handling?
					return false;
				}
				if(incDepth) { ++depth; }
    				switch(input.peekChar)
				{
					case '<':
					{
						input.NextChar();
						if(!input.PeekChar()) { Error(/* TODO */); }
						switch(input.peekChar)
						{
							case '?':
							{
								input.NextChar();
								ReadQMarkTag();
							}
							break;

							case '!':
							{
								input.NextChar();
								ReadEMarkTag();
							}
							break;

							case '/':
							{
								input.NextChar();
								ReadETag();
							}
							break;

							default:
							{
								ReadSTag();
							}
							break;
						}
					}
					break;

					case '\r':
					case '\n':
					case '\t':
					case ' ':
					{
						if(whitespace == WhitespaceHandling.None)
						{
							input.SkipWhitespace();
							return ReadDocument();
						}
						ReadWhitespace();
					}
					break;

					default:
					{
						ReadText();
					}
					break;
				}
				return true;
			}


	[TODO]
	private void ReadCDATA()
			{
				ClearNodeInfo();
				StringBuilder sb = new StringBuilder();
				int count = 0;
				while(input.NextChar())
				{
					if(input.currChar == ']')
					{
						++count;
					}
					else if(input.currChar == '>' && count >= 2)
					{
						sb.Length -= 2;
						info.NodeType = XmlNodeType.CDATA;
						info.SetElementInfo("", sb.ToString());
						return;
					}
					else
					{
						count = 0;
					}
					sb.Append(input.currChar);
				}
				Error("Xml_UnexpectedEOF");
			}

	[TODO]
	private void ReadComment()
			{
				ClearNodeInfo();
				StringBuilder sb = new StringBuilder();
				while(input.NextChar())
				{
					if(input.currChar == '-')
					{
						if(!input.NextChar()) { Error("Xml_UnexpectedEOF"); }
						if(input.currChar == '-')
						{
							Expect('>');
							info.NodeType = XmlNodeType.Comment;
							info.SetElementInfo("", sb.ToString());
							return;
						}
						sb.Append('-');
					}
					sb.Append(input.currChar);
				}
				Error("Xml_UnexpectedEOF");
			}

	[TODO]
	private void ReadDoctypeDeclaration()
			{
				if(!input.NextChar()) { Error("Xml_UnexpectedEOF"); }
				if(!IsWhitespace(input.currChar)) { Error(/* TODO */); }
				input.SkipWhitespace();
				String name = ReadName();
				
				readState = ReadState.Error;
				ClearNodeInfo();
				throw new XmlException("ReadDoctypeDeclaration");
			}

	[TODO] 
	private bool ReadElement() // garbage
			{
				while(input.NextChar())
				{
					switch(input.currChar)
					{
						case '<':
							ReadName();
							break;
						case '\t':
						case '\r':
						case '\n':
						case ' ':
							//ReadAttributes();
							break;
						case '>':
							return true;
					}
				}
				return false;
			}
	[TODO]
	private void ReadEMarkTag() 
			{
				if(!input.NextChar()) { Error("Xml_UnexpectedEOF"); }
				switch(input.currChar)
				{
					case 'D':
					{
						Expect("OCTYPE");
						ReadDoctypeDeclaration();
					}
					break;

					case '-':
					{
						Expect('-');
						ReadComment();
					}
					break;

					case '[':
					{
						Expect("CDATA[");
						ReadCDATA();
					}
					break;

					default:
					{
						Error(/* TODO */);
					}
					break;
				}
			}
	[TODO]
	private void ReadETag() 
			{
				// TODO: state handling
				ClearNodeInfo();
				String name = ReadName();
				input.SkipWhitespace();
				Expect('>');
				--depth;
				if(elementNames.Pop() != name) { Error(/* TODO */); }
				info.NodeType = XmlNodeType.EndElement;
				info.SetElementInfo(name);
			}
	private String ReadName()
			{
				return ReadName(new StringBuilder());
			}
	private String ReadName(StringBuilder sb)
			{
				int pos = sb.Length;
				if(!input.NextChar()) { Error("Xml_UnexpectedEOF"); }
				if(!IsNameInit(input.currChar)) { Error(/* TODO */); }
				sb.Append(input.currChar);
				while(input.PeekChar() && IsNameChar(input.peekChar))
				{
					input.NextChar();
					sb.Append(input.currChar);
				}
				int len = sb.Length - pos;
				return parserContext.NameTable.Add(sb.ToString(pos, len));
			}
	[TODO]
	private void ReadQMarkTag() 
			{	
				String target = ReadName();
				if(target == "xml")
				{
					ReadXmlOrTextDeclaration();
					return;
				}

				// Not XmlDeclaration, so must be ProcessingInstruction
				// TODO: check for ('X'|'x')('M'|'m')('L'|'l')
				// TODO: state handling

				input.SkipWhitespace();

				StringBuilder sb = new StringBuilder();
				while(input.NextChar())
				{
					if(input.currChar == '?' &&
					   input.PeekChar() &&
					   input.peekChar == '>')
					{
						input.NextChar();
						ClearNodeInfo();
						info.NodeType = XmlNodeType.ProcessingInstruction;
						info.SetElementInfo(target, sb.ToString());
						return;
					}
					sb.Append(input.currChar);
				}
			}

	// Read an entity reference. Expands built-in and character references.
	// Returns true if it's an unknown reference, false if expanded.
	private bool ReadReference(StringBuilder sb)
			{
				if(!input.PeekChar()) { Error("Xml_UnexpectedEOF"); }
				if(input.peekChar == ';') { Error(/* TODO */); }
				if(input.peekChar == '#')
				{
					input.NextChar();
					int value = 0;
					if(!input.PeekChar()) { Error("Xml_UnexpectedEOF"); }
					if(input.peekChar == ';') { Error(/* TODO */); }
					if(input.peekChar == 'x')
					{
						input.NextChar();
						if(!input.PeekChar()) { Error("Xml_UnexpectedEOF"); }
						if(input.peekChar == ';') { Error(/* TODO */); }
						while(input.NextChar() && input.currChar != ';')
						{
							value *= 0x10;
							if(input.currChar >= '0' && input.currChar <= '9')
							{
								value += input.currChar - '0';
							}
							else if(input.currChar >= 'A' && input.currChar <= 'F')
							{
								value += (input.currChar - 'A') + 10;
							}
							else if(input.currChar >= 'a' && input.currChar <= 'f')
							{
								value += (input.currChar - 'a') + 10;
							}
							else
							{
								Error(/* TODO */);
							}
						}
					}
					else
					{
						while(input.NextChar() && input.currChar != ';')
						{
							value *= 10;
							if(input.currChar >= '0' && input.currChar <= '9')
							{
								value += input.currChar - '0';
							}
							else
							{
								Error(/* TODO */);
							}
						}
					}
					if(input.currChar != ';') { Error("Xml_UnexpectedEOF"); }
					sb.Append((char)value);
					return false;
				}

				int undo = sb.Length;
				sb.Append('&');

				switch(ReadName(sb))
				{
					case "amp":
					{
						sb.Length = undo+1;
						return false;
					}
					// Not reached.

					case "apos":
					{
						sb.Length = undo;
						sb.Append('\'');
						return false;
					}
					// Not reached.

					case "gt":
					{
						sb.Length = undo;
						sb.Append('>');
						return false;
					}
					// Not reached.

					case "lt":
					{
						sb.Length = undo;
						sb.Append('<');
						return false;
					}
					// Not reached.

					case "quot":
					{
						sb.Length = undo;
						sb.Append('"');
						return false;
					}
					// Not reached.

					default:
					{
						sb.Append(';');
						return true;
					}
					// Not reached.
				}
			}

	[TODO]
	private void ReadSTag()
			{
				// TODO: state handling
				ClearNodeInfo();
				StringBuilder sb = new StringBuilder();
				String name = ReadName(sb);
				ReadAttributes(sb, false);
				input.NextChar();
				bool empty = false;
				if(input.currChar == '/')
				{
					empty = true;
					Expect('>');
				}
				else
				{
					incDepth = true;
					elementNames.Push(name);
				}
				info.NodeType = XmlNodeType.Element;
				info.SetElementInfo(name);
				info.IsEmptyElement = empty;
			}

	[TODO]
	private void ReadText()
			{
				/* TODO: Add Support for normalization */
				ClearNodeInfo();
				info.NodeType = XmlNodeType.Text;
				info.SetElementInfo("",ReadTo('<'));
			}

	// Read characters until target, calling Error on EOF.
	private String ReadTo(char target)
			{
				StringBuilder sb = new StringBuilder();
				if(!input.ReadTo(target, sb, false)) { Error(); }
				return sb.ToString();
			}
	// Read characters until target, calling Error on EOF.
	private String ReadTo(char target, bool includeTarget)
			{
				StringBuilder sb = new StringBuilder();
				if(!input.ReadTo(target, sb, includeTarget)) { Error(); }
				return sb.ToString();
			}

	[TODO]
	private void ReadWhitespace()
			{
				// TODO: state handling
				if(!input.NextChar()) { Error("Xml_UnexpectedEOF"); }
				if(!IsWhitespace(input.currChar)) { Error(/* TODO */); }
				StringBuilder sb = new StringBuilder();
				sb.Append(input.currChar);
				while(input.PeekChar() && IsWhitespace(input.peekChar))
				{
					input.NextChar();
					sb.Append(input.currChar);
				}
				ClearNodeInfo();
				info.SetElementInfo(String.Empty, sb.ToString());
				info.NodeType = XmlNodeType.Whitespace; // ??
			}

	[TODO]
	private void ReadXmlOrTextDeclaration()
			{
				// TODO: state handling
				// TODO: handle differences between xml and text declarations
				ClearNodeInfo();
				StringBuilder sb = new StringBuilder();
				ReadAttributes(sb, true);
				info.NodeType = XmlNodeType.XmlDeclaration;
				info.SetElementInfo("xml", sb.ToString());
				Expect("?>");
			}

	public delegate void EOFHandler();

	private sealed class XmlParserInput
	{
		// Internal state.
		private int lineNumber;
		private int linePosition;
		private int bufferPos;
		private int bufferLen;
		private char[] buffer;
		private TextReader reader;
		private EOFHandler eofHandler;

		private const int BUFSIZE = 1024;


		// Fields
		public char currChar;
		public char peekChar;


		// Constructor.
		public XmlParserInput(EOFHandler eofHandler, TextReader reader)
				{
					lineNumber = 1;
					linePosition = 1;
					bufferPos = 0;
					bufferLen = 0;
					buffer = new char[BUFSIZE];
					this.reader = reader;
					this.eofHandler = eofHandler;
				}

		// Move to the next character, returning false on EOF.
		public bool NextChar()
				{
					if(bufferPos == bufferLen)
					{
						bufferLen = reader.Read(buffer, 0, BUFSIZE);
						bufferPos = 0;
						if(bufferLen == 0)
						{
							eofHandler();
							return false;
						}
					}
					currChar = buffer[bufferPos++];
					if(currChar == '\n')
					{
						++lineNumber;
						linePosition = 1;
					}
					else
					{
						++linePosition;
					}
					return true;
				}

		// Peek at the next character, returning false on EOF.
		public bool PeekChar()
				{
					if(bufferPos == bufferLen)
					{
						bufferLen = reader.Read(buffer, 0, BUFSIZE);
						bufferPos = 0;
						if(bufferLen == 0)
						{
							eofHandler();
							return false;
						}
					}
					peekChar = buffer[bufferPos];
					return true;
				}

		// Read characters into result until target, returning false on EOF.
		public bool ReadTo(char target, StringBuilder result, bool includeTarget)
				{
					while(true)
					{
						while(bufferPos < bufferLen)
						{
							currChar = buffer[bufferPos++];
							if(currChar == '\n')
							{
								++lineNumber;
								linePosition = 1;
							}
							else
							{
								++linePosition;
							}
							if(currChar == target)
							{
								if(includeTarget)
								{
									result.Append(currChar);
								}
								return true;
							}
							else
							{
								result.Append(currChar);
							}
						}
						bufferLen = reader.Read(buffer, 0, BUFSIZE);
						bufferPos = 0;
						if(bufferLen == 0)
						{
							eofHandler();
							return false;
						}
					}
				}

		// Skip whitespace characters, returning false on EOF.
		public bool SkipWhitespace()
				{
					while(true)
					{
						while(bufferPos < bufferLen)
						{
							currChar = buffer[bufferPos++];
							switch(currChar)
							{
								case '\t':
								case '\r':
								case ' ':
								{
									++linePosition;
								}
								break;

								case '\n':
								{
									++lineNumber;
									linePosition = 1;
								}
								break;

								default:
								{
									--bufferPos;
									return true;
								}
								// Not reached.
							}
						}
						bufferLen = reader.Read(buffer, 0, BUFSIZE);
						bufferPos = 0;
						if(bufferLen == 0)
						{
							eofHandler();
							return false;
						}
					}
				}

		// Get the current line number.
		public int LineNumber
				{
					get { return lineNumber; }
				}

		// Get the current line position.
		public int LinePosition
				{
					get { return linePosition; }
				}

		// Get or set the reader.
		public TextReader Reader
				{
					get { return reader; }
					set { reader = value; }
				}

		// Close the reader.
		public void Close()
				{
					if(reader == null) { return; }
					reader.Close();
					reader = null;
				}

	}; // class XmlParserInput

	private sealed class XmlNodeInfo
	{
		// Internal state.
		private String localName;
		private String name;
		private String namespaceURI;
		private String prefix;
		private String value;
		private char quoteChar;
		private int[] refs;
		private XmlParserContext context;

		// Constructors.
		public XmlNodeInfo(XmlParserContext context, String name)
				{
					this.context = context;
					this.Name = name;
					this.value = String.Empty;
					this.quoteChar = '"';
					refs = null;
				}
		public XmlNodeInfo(XmlParserContext context,
		                   String name,
		                   String value,
		                   char quoteChar,
				   int[] refs)
				{
					this.context = context;
					this.Name = name;
					this.value = value;
					this.quoteChar = quoteChar;
					this.refs = refs;
				}

		// Get the local name.
		public String LocalName
				{
					get { return localName; }
				}

		// Get or set the fully-qualified name.
		public String Name
				{
					get { return name; }
					set
					{
						XmlNameTable nt = context.NameTable;

						name = nt.Add(value);
						int index = name.LastIndexOf(':');
						if(index >= 0)
						{
							XmlNamespaceManager nm = context.NamespaceManager;

							prefix = nt.Add(name.Substring(0, index));
							localName = nt.Add(name.Substring(index + 1));
							namespaceURI = nm.LookupNamespace(prefix);
						}
						else
						{
							prefix = String.Empty;
							localName = name;
							namespaceURI = String.Empty;
						}
					}
				}

		// Get the namespace URI.
		public String NamespaceURI
				{
					get { return namespaceURI; }
				}

		// Get the namespace prefix.
		public String Prefix
				{
					get { return prefix; }
				}

		// Get or set the text value.
		public String Value
				{
					get { return value; }
					set { this.value = value; }
				}

		// Get or set the quote character.
		public char QuoteChar
				{
					get { return quoteChar; }
					set { quoteChar = value; }
				}

		// Get or set the indices for entity references contained in the value.
		public int[] Refs
				{
					get { return refs; }
					set { refs = value; }
				}

		// Clear the node information.
		public void ClearNodeInfo()
				{
					localName = String.Empty;
					name = String.Empty;
					namespaceURI = String.Empty;
					prefix = String.Empty;
					value = String.Empty;
					quoteChar = '"';
					refs = null;
				}

		// Set the node information.
		public void SetNodeInfo(String name)
				{
					ClearNodeInfo();
					this.Name = name;
				}
		public void SetNodeInfo(String name, String value)
				{
					ClearNodeInfo();
					this.Name = name;
					this.value = value;
				}
		public void SetNodeInfo(String name, String value, char quoteChar, int[] refs)
				{
					ClearNodeInfo();
					this.Name = name;
					this.value = value;
					this.quoteChar = quoteChar;
					this.refs = refs;
				}

	}; // class XmlNodeInfo

	private sealed class XmlElementInfo
	{
		// Internal state.
		private bool empty;
		private int count;
		private int index;
		private int refsPosition;
		private int valuePosition;
		private int[] refs;
		private String value;
		private XmlNodeInfo element;
		private XmlNodeInfo other;
		private XmlNodeInfo[] attribs;
		private XmlNodeType elementType;
		private XmlNodeType otherType;
		private XmlParserContext context;

		private const int INITSIZE = 8;


		// Constructor.
		public XmlElementInfo(XmlParserContext parserContext)
				{
					empty = false;
					count = 0;
					index = -1;
					refs = null;
					value = null;
					refsPosition = 0;
					valuePosition = 0;
					element = new XmlNodeInfo(parserContext, String.Empty);
					other = new XmlNodeInfo(parserContext, String.Empty);
					attribs = new XmlNodeInfo[INITSIZE];
					elementType = XmlNodeType.None;
					otherType = XmlNodeType.None;
					context = parserContext;
				}
		public XmlElementInfo(XmlParserContext parserContext, XmlNodeInfo attribute)
				{
					empty = false;
					count = 0;
					index = -1;
					refs = null;
					value = null;
					refsPosition = 0;
					valuePosition = 0;
					element = attribute;
					other = new XmlNodeInfo(parserContext, String.Empty);
					attribs = null;
					elementType = XmlNodeType.Attribute;
					otherType = XmlNodeType.None;
					context = parserContext;
				}

		// Get the number of attributes.
		public int AttributeCount
				{
					get { return count; }
				}

		// Get the current node.
		public XmlNodeInfo Current
				{
					get
					{
						switch(otherType)
						{
							case XmlNodeType.None: return element;
							case XmlNodeType.Attribute: return attribs[index];
							default: return other;
						}
					}
				}

		// Get the amount to add to the depth.
		public int DepthOffset
				{
					get
					{
						if(elementType == XmlNodeType.Attribute)
						{
							switch(otherType)
							{
								case XmlNodeType.None: return 0;
								default: return 1;
							}
						}
						switch(otherType)
						{
							case XmlNodeType.None: return 0;
							case XmlNodeType.Attribute: return 1;
							default: return 2;
						}
					}
				}

		// Get or set if the current node is an empty element.
		public bool IsEmptyElement
				{
					get
					{
						if(index == -1) { return empty; }
						else { return false; }
					}
					set { empty = value; }
				}

		// Get or set the type of the current node.
		public XmlNodeType NodeType
				{
					get
					{
						if(otherType == XmlNodeType.None) { return elementType; }
						else { return otherType; }
					}
					set { elementType = value; }
				}


		// Add an attribute.
		public void AddAttribute(String name, String value, char quoteChar, int[] refs)
				{
					if(count == attribs.Length)
					{
						XmlNodeInfo[] tmp = new XmlNodeInfo[count*2];
						Array.Copy(attribs, tmp, count);
						attribs = tmp;
					}
					if(attribs[count] == null)
					{
						attribs[count++] = new XmlNodeInfo
							(context, name, value, quoteChar, refs);
					}
					else
					{
						attribs[count++].SetNodeInfo(name, value, quoteChar, refs);
					}
				}

		// Clear attribute set.
		public void ClearAttributes()
				{
					count = 0;
					index = -1;
					refs = null;
					value = null;
					refsPosition = 0;
					valuePosition = 0;
					other.ClearNodeInfo();
					otherType = XmlNodeType.None;
				}

		// Clear the element information.
		public void ClearElementInfo()
				{
					ClearAttributes();
					element.ClearNodeInfo();
					elementType = XmlNodeType.None;
					empty = false;
				}

		// Return an attribute with the given index.
		public XmlNodeInfo GetAttribute(int i)
				{
					if(i < 0 || i >= count) { return null; }
					return attribs[i];
				}
		// Return an attribute with the given name and namespace uri.
		public XmlNodeInfo GetAttribute(String localName, String namespaceURI)
				{
					XmlNodeInfo info;
					for(int i = 0; i < count; ++i)
					{
						info = attribs[i];
						if(info.LocalName == localName &&
						   info.NamespaceURI == namespaceURI)
						{
							return info;
						}
					}
					return null;
				}
		// Return an attribute with the given fully-qualified name.
		public XmlNodeInfo GetAttribute(String name)
				{
					XmlNodeInfo info;
					for(int i = 0; i < count; ++i)
					{
						info = attribs[i];
						if(info.Name == name)
						{
							return info;
						}
					}
					return null;
				}

		// Move the position to an attribute with the given index.
		public bool MoveToAttribute(int i)
				{
					if(elementType == XmlNodeType.Attribute) { return false; }
					if(i < 0 || i >= count) { return false; }
					index = i;
					refs = null;
					value = null;
					refsPosition = 0;
					valuePosition = 0;
					otherType = XmlNodeType.Attribute;
					return true;
				}
		// Move the position to an attribute with the given name and namespace uri.
		public bool MoveToAttribute(String localName, String namespaceURI)
				{
					XmlNodeInfo info;
					for(int i = 0; i < count; ++i)
					{
						info = attribs[i];
						if(info.LocalName == localName &&
						   info.NamespaceURI == namespaceURI)
						{
							return MoveToAttribute(i);
						}
					}
					return false;
				}
		// Move the position to an attribute with the given fully-qualified name.
		public bool MoveToAttribute(String name)
				{
					XmlNodeInfo info;
					for(int i = 0; i < count; ++i)
					{
						info = attribs[i];
						if(info.Name == name)
						{
							return MoveToAttribute(i);
						}
					}
					return false;
				}

		// Move the position to before the first attribute.
		public bool MoveToElement()
				{
					if(elementType == XmlNodeType.Attribute) { return false; }
					if(index == -1) { return false; }
					index = -1;
					refs = null;
					value = null;
					refsPosition = 0;
					valuePosition = 0;
					otherType = XmlNodeType.None;
					return true;
				}

		// Move the position to the first attribute.
		public bool MoveToFirstAttribute()
				{
					return MoveToAttribute(0);
				}

		// Move the position to the next attribute.
		public bool MoveToNextAttribute()
				{
					return MoveToAttribute(index+1);
				}

		// Read the next attribute value in the input stream.
		public bool ReadAttributeValue()
				{
					if(elementType != XmlNodeType.Attribute)
					{
						if(otherType == XmlNodeType.None)
						{
							return false;
						}
					}
					if(value == null)
					{
						switch(otherType)
						{
							case XmlNodeType.None:
							{
								value = element.Value;
								refs = element.Refs;
							}
							break;

							case XmlNodeType.Attribute:
							{
								value = attribs[index].Value;
								refs = attribs[index].Refs;
							}
							break;

							default: return false;
						}
					}
					if(valuePosition == value.Length) { return false; }
					if(refs == null || refsPosition == refs.Length)
					{
						other.SetNodeInfo("", value.Substring(valuePosition));
						valuePosition = value.Length;
						otherType = XmlNodeType.Text;
						return true;
					}
					if(valuePosition == refs[refsPosition])
					{
						++refsPosition;
						int start = valuePosition + 1;
						valuePosition = value.IndexOf(';', start) + 1;
						int end = valuePosition - 2;
						other.SetNodeInfo(value.Substring(start, end));
						otherType = XmlNodeType.EntityReference;
						return true;
					}
					int start1 = valuePosition;
					valuePosition = refs[refsPosition];
					int end1 = valuePosition - 1;
					other.SetNodeInfo("", value.Substring(start1, end1));
					otherType = XmlNodeType.Text;
					return true;
				}

		// Set the element information.
		public void SetElementInfo(String name)
				{
					element.SetNodeInfo(name);
				}
		public void SetElementInfo(String name, String value)
				{
					element.SetNodeInfo(name, value);
				}
		public void SetElementInfo(String name, String value, char quoteChar)
				{
					element.SetNodeInfo(name, value, quoteChar, null);
				}

	}; // class XmlElementInfo

}; // class XmlTextReader

}; // namespace System.Xml
