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
	private XmlDTDReader dtdReader;
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
				dtdReader = new XmlDTDReader();
				info = new XmlElementInfo(parserContext);
				input = new XmlParserInput
					(null, nt, new EOFHandler(HandleEOF), new ErrorHandler(Error));
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
	[TODO] // ********************************************************* TODO
	public override void Close()
			{
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
	[TODO] // ********************************************************* TODO
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

	// Read the next node in the input stream.
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
	[TODO] // ********************************************************* TODO
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

				if(NodeType != XmlNodeType.Element)
				{
					return 0;
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
	[TODO] // ********************************************************* TODO
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
	[TODO] // ********************************************************* TODO
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
	[TODO] // ********************************************************* TODO
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
	private void Error(String messageTag, params Object[] args)
			{
				readState = ReadState.Error;
				ClearNodeInfo();
				input.Logger.Clear();
				throw new XmlException(String.Format(S._(messageTag), args));
			}

	// Callback for eof in the input handler.
	[TODO]
	private void HandleEOF()
			{
				readState = ReadState.EndOfFile;
			}

#if !ECMA_COMPAT
	private String ParseEntity(String entityref)
			{
				return parserContext.NameCollection.Get(entityref);
			}

	//internal SomeDTDRuleHandlingObjectGoesHere FooBar
	//		{
	//			get { return dtdReader.FooBar; }
	//			set { dtdReader.FooBar = value; }
	//		}
#endif

	// Read the attributes for an element start tag.
	//
	// Already read: ''
	[TODO]
	private void ReadAttributes(bool qmark)
			{
				// create our value log
				StringBuilder log = new StringBuilder();

				// create our list for reference positions
				ArrayList refs = new ArrayList();

				// read until we consume all of the attributes
				while(true)
				{
					// skip potentially optional whitespace
					bool hasWS = input.SkipWhitespace();

					// check for an end character
					if(!input.PeekChar()) { Error("Xml_UnexpectedEOF"); }
					if(qmark)
					{
						if(input.peekChar == '?') { return; }
					}
					else if(input.peekChar == '/' || input.peekChar == '>')
					{
						return;
					}

					// the attribute name must be preceded by whitespace
					if(!hasWS) { Error(/* TODO */); }

					// read the attribute name
					String name = input.ReadName();

					// skip optional whitespace and read the '=' character
					input.SkipWhitespace();
					input.Expect('=');
					input.SkipWhitespace();

					// scan for a valid quote character
					if(!input.NextChar()) { Error("Xml_UnexpectedEOF"); }
					char quoteChar = input.currChar;
					if(quoteChar != '"' && quoteChar != '\'')
					{
						Error(/* TODO */);
					}

					// push our log onto the logger's log stack
					input.Logger.Push(log);

					// read until we hit the quote character
					while(input.NextChar() && input.currChar != quoteChar)
					{
						// perform basic error checks and store ref positions
						if(input.currChar == '<')
						{
							Error(/* TODO */);
						}
						else if(input.currChar == '&')
						{
							// store the position of the reference
							refs.Add(log.Length);

							// read the reference
							ReadReference();
						}
					}

					// we hit eof, otherwise we'd have quoteChar, so give an error
					if(input.currChar != quoteChar) { Error("Xml_UnexpectedEOF"); }

					// erase the closing quote character from the log
					log.Length -= 1;

					// get the value from the log and pop it from the logger
					String value = input.Logger.Pop().ToString();

					// reset the log
					log.Length = 0;

					// add the attribute to the current node information
					info.AddAttribute
						(name, value, quoteChar, (int[])refs.ToArray(typeof(int)));

					// clear our list of reference positions
					refs.Clear();
				}
			}

	// Read a character data section.
	//
	// Already read: '<![CDATA['
	[TODO]
	private void ReadCDATA()
			{
				// TODO: state handling

				// clear the current node information
				ClearNodeInfo();

				// create our log and push it onto the logger's log stack
				StringBuilder log = new StringBuilder();
				input.Logger.Push(log);

				// read until we've consumed all of the character data content
				while(input.NextChar() && input.PeekChar())
				{
					// check for the ']]' sequence
					if(input.currChar == ']' && input.peekChar == ']')
					{
						input.NextChar();

						// check if we've got the ']]>' sequence
						if(!input.NextChar()) { Error("Xml_UnexpectedEOF"); }
						if(input.currChar == '>')
						{
							// erase the ']]>' sequence from the log
							log.Length -= 3;

							// get the cdata from the log and pop it from the logger
							String value = input.Logger.Pop().ToString();

							// set the current node information and return
							info.NodeType = XmlNodeType.CDATA;
							info.SetElementInfo("", value);
							return;
						}
					}
				}

				// if we make it here then we hit eof, so give an error
				Error("Xml_UnexpectedEOF");
			}

	// Read a comment.
	//
	// Already read: '<!--'
	[TODO]
	private void ReadComment()
			{
				// TODO: state handling

				// clear the current node information
				ClearNodeInfo();

				// create our log and push it onto the logger's log stack
				StringBuilder log = new StringBuilder();
				input.Logger.Push(log);

				// read until we consume all of the comment content
				while(input.NextChar() && input.PeekChar())
				{
					// check for the '--' sequence
					if(input.currChar == '-' && input.peekChar == '-')
					{
						input.NextChar();

						// erase the '--' sequence from the log
						log.Length -= 2;

						// get the comment from the log and pop it from the logger
						String value = input.Logger.Pop().ToString();

						// the comment must end with '>' at this point
						input.Expect('>');

						// set the current node information and return
						info.NodeType = XmlNodeType.Comment;
						info.SetElementInfo("", value);
						return;
					}
				}

				// if we make it here then we hit eof, so give an error
				Error("Xml_UnexpectedEOF");
			}

	// Read the xml document.
	//
	// Already read: ''
	[TODO]
	private bool ReadDocument()
			{
				// return false if there are no nodes left to read
				if(!input.PeekChar()) { return false; }

				// increase the depth if we last read an element start tag
				if(incDepth)
				{
					++depth;
					incDepth = false;
				}

				// handle all the possible node cases
				switch(input.peekChar)
				{
					// handle the tag node case
					case '<':
					{
						input.NextChar();

						// handle all the possible tag cases
						if(!input.PeekChar()) { Error(/* TODO */); }
						switch(input.peekChar)
						{
							// handle the qmark tag case
							case '?':
							{
								input.NextChar();
								ReadQMarkTag();
							}
							break;

							// handle the emark tag case
							case '!':
							{
								input.NextChar();
								ReadEMarkTag();
							}
							break;

							// handle the element end tag case
							case '/':
							{
								input.NextChar();
								ReadETag();
							}
							break;

							// handle the element start tag case
							default:
							{
								ReadSTag();
							}
							break;
						}
					}
					break;

					// handle the whitespace node case
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

					// handle the text node case
					default:
					{
						ReadText();
					}
					break;
				}

				// if we made it this far, we must've read a node, so return true
				return true;
			}

	// Read a doctype declaration.
	//
	// Already read: '<!DOCTYPE'
	private void ReadDoctypeDeclaration()
			{
				// TODO: state handling

				// clear the current node information
				ClearNodeInfo();

				// (re)initialize the dtd reader
				dtdReader.Init(input, xmlResolver, parserContext.NameTable);

				// read the dtd
				dtdReader.Read();

				// set the current node information
				info.NodeType = XmlNodeType.DocumentType;
				info.SetElementInfo(dtdReader.Name, dtdReader.Value);
			}

	// Read a '<!' tag.
	//
	// Already read: '<!'
	[TODO]
	private void ReadEMarkTag()
			{
				// handle all the possible emark tag cases
				if(!input.NextChar()) { Error("Xml_UnexpectedEOF"); }
				switch(input.currChar)
				{
					// handle the DOCTYPE case
					case 'D':
					{
						input.Expect("OCTYPE");
						ReadDoctypeDeclaration();
					}
					break;

					// handle the comment case
					case '-':
					{
						input.Expect('-');
						ReadComment();
					}
					break;

					// handle the CDATA case
					case '[':
					{
						input.Expect("CDATA[");
						ReadCDATA();
					}
					break;

					// handle the unknown case
					default:
					{
						Error(/* TODO */);
					}
					break;
				}
			}

	// Read an element end tag.
	//
	// Already read: '</'
	[TODO]
	private void ReadETag()
			{
				// TODO: state handling

				// clear the current node information
				ClearNodeInfo();

				// read the element name
				String name = input.ReadName();

				// skip optional whitespace
				input.SkipWhitespace();

				// the element end tag must end with '>' at this point
				input.Expect('>');

				// decrease the depth
				--depth;

				// this end tag must match the last start tag
				if(elementNames.Pop() != (Object)name) { Error(/* TODO */); }

				// set the current node information
				info.NodeType = XmlNodeType.EndElement;
				info.SetElementInfo(name);
			}

	// Read a processing instruction.
	//
	// Already read: '<?' Name
	[TODO]
	private void ReadProcessingInstruction(String target)
			{
				// TODO: state handling
				// TODO: check target for ('X'|'x')('M'|'m')('L'|'l')

				// clear the current node information
				ClearNodeInfo();

				// skip potentially optional whitespace
				bool hasWS = input.SkipWhitespace();

				// check for the closing characters
				if(!input.NextChar()) { Error("Xml_UnexpectedEOF"); }
				if(!input.PeekChar()) { Error("Xml_UnexpectedEOF"); }
				if(input.currChar == '?' && input.peekChar == '>')
				{
					input.NextChar();

					// set the current node information and return
					info.NodeType = XmlNodeType.ProcessingInstruction;
					info.SetElementInfo(target, String.Empty);
					return;
				}

				// pi content must be preceded by whitespace
				if(!hasWS) { Error(/* TODO */); }

				// create our log and push it onto the logger's log stack
				StringBuilder log = new StringBuilder();
				input.Logger.Push(log);

				// read until we consume all of the pi content
				while(input.NextChar() && input.PeekChar())
				{
					if(input.currChar == '?' && input.peekChar == '>')
					{
						input.NextChar();

						// erase the '?>' sequence from the log
						log.Length -= 2;

						// get the content from the log and pop it from the logger
						String value = input.Logger.Pop().ToString();

						// set the current node information and return
						info.NodeType = XmlNodeType.ProcessingInstruction;
						info.SetElementInfo(target, value);
						return;
					}
				}

				// if we make it here then we hit eof, so give an error
				Error("Xml_UnexpectedEOF");
			}

	// Read a '<?' tag.
	//
	// Already read: '<?'
	[TODO]
	private void ReadQMarkTag()
			{
				// read the pi target name
				String target = input.ReadName();

				// check if we have a pi or xml declaration
				if(target == "xml")
				{
					ReadXmlDeclaration();
				}
				else
				{
					ReadProcessingInstruction(target);
				}
			}

	// Read an entity reference.
	//
	// Already read: '&'
	private void ReadReference()
			{
				// check for an empty reference
				if(!input.PeekChar()) { Error("Xml_UnexpectedEOF"); }
				if(input.peekChar == ';') { Error(/* TODO */); }

				// handle character or general references
				if(input.peekChar == '#')
				{
					input.NextChar();

					// check for an empty character reference
					if(!input.PeekChar()) { Error("Xml_UnexpectedEOF"); }
					if(input.peekChar == ';') { Error(/* TODO */); }

					// handle a hex or decimal character reference
					if(input.peekChar == 'x')
					{
						input.NextChar();

						// check for an empty hex character reference
						if(!input.PeekChar()) { Error("Xml_UnexpectedEOF"); }
						if(input.peekChar == ';') { Error(/* TODO */); }

						// read until we consume all the digits
						while(input.NextChar() && input.currChar != ';')
						{
							// check for invalid characters
							if((input.currChar <= '0' || input.currChar >= '9') &&
							   (input.currChar <= 'A' || input.currChar >= 'F') &&
							   (input.currChar <= 'a' || input.currChar >= 'f'))
							{
								Error(/* TODO */);
							}
						}
					}
					else
					{
						// read until we consume all the digits
						while(input.NextChar() && input.currChar != ';')
						{
							// check for invalid characters
							if(input.currChar <= '0' || input.currChar >= '9')
							{
								Error(/* TODO */);
							}
						}
					}

					// we hit eof, otherwise we'd have ';', so give an error
					if(input.currChar != ';') { Error("Xml_UnexpectedEOF"); }
				}
				else
				{
					// read the reference name
					input.ReadName();

					// the reference must end with ';' at this point
					input.Expect(';');
				}
			}

	// Read an element start tag.
	//
	// Already read: '<'
	[TODO]
	private void ReadSTag()
			{
				// TODO: state handling

				// clear the current node information
				ClearNodeInfo();

				// read the element name
				String name = input.ReadName();

				// read the element's attributes
				ReadAttributes(false);

				// check if this is an empty element or not
				if(!input.NextChar()) { Error("Xml_UnexpectedEOF"); }
				bool empty = false;
				if(input.currChar == '/')
				{
					empty = true;
					input.Expect('>');
				}
				else
				{
					incDepth = true;
					elementNames.Push(name);
				}

				// set the current node information
				info.NodeType = XmlNodeType.Element;
				info.SetElementInfo(name);
				info.IsEmptyElement = empty;
			}

	// Read a text node.
	//
	// Already read: ''
	[TODO]
	private void ReadText()
			{
				/* TODO: Add Support for normalization */
				ClearNodeInfo();
				info.NodeType = XmlNodeType.Text;
				info.SetElementInfo("",input.ReadTo('<'));
			}

	// Read a whitespace node.
	//
	// Already read: ''
	[TODO]
	private void ReadWhitespace()
			{
				// TODO: state handling

				// create our log and push it onto the logger's log stack
				StringBuilder log = new StringBuilder();
				input.Logger.Push(log);

				// skip the required whitespace
				if(!input.SkipWhitespace()) { Error(/* TODO */); }

				// get the whitespace from the log and pop it from the logger
				String value = input.Logger.Pop().ToString();

				// clear the current node information
				ClearNodeInfo();

				// set the current node information
				info.SetElementInfo(String.Empty, value);
				info.NodeType = XmlNodeType.Whitespace; // ??
			}

	// Read an xml declaration.
	//
	// Already read: '<?xml'
	[TODO]
	private void ReadXmlDeclaration()
			{
				// TODO: state handling
				// TODO: encoding checks
				// TODO: ensure attributes are well-formed and in correct order

				// clear the current node information
				ClearNodeInfo();

				// create our log and push it onto the logger's log stack
				StringBuilder log = new StringBuilder();
				input.Logger.Push(log);

				// read the xml declaration's attributes
				ReadAttributes(true);

				// get attributes from the log and pop it from the logger
				String value = input.Logger.Pop().ToString();

				// set the current node information
				info.NodeType = XmlNodeType.XmlDeclaration;
				info.SetElementInfo("xml", value);

				// the xml declaration must end with '?>' at this point
				input.Expect("?>");
			}



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

						// add the new name value to the name table
						name = nt.Add(value);

						// find the namespace separator
						int index = name.LastIndexOf(':');

						// set the namespace information
						if(index >= 0)
						{
							XmlNamespaceManager nm = context.NamespaceManager;

							// add the prefix and local name to the name table
							prefix = nt.Add(name.Substring(0, index));
							localName = nt.Add(name.Substring(index + 1));

							// set the namespace uri based on the prefix
							namespaceURI = nm.LookupNamespace(prefix);
						}
						else
						{
							// no namespace information is available
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
					}
					else if(valuePosition == refs[refsPosition])
					{
						++refsPosition;
						int start = valuePosition + 1;
						valuePosition = value.IndexOf(';', start) + 1;
						int end = valuePosition - 2;
						other.SetNodeInfo(value.Substring(start, end));
						otherType = XmlNodeType.EntityReference;
					}
					else
					{
						int start = valuePosition;
						valuePosition = refs[refsPosition];
						int end = valuePosition - 1;
						other.SetNodeInfo("", value.Substring(start, end));
						otherType = XmlNodeType.Text;
					}
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
