/*
 * XmlTextReader.cs - Implementation of the "System.Xml.XmlTextReader" class.
 *
 * Copyright (C) 2002  Southern Storm Software, Pty Ltd.
 * Copyright (C) 2003, 2004  Free Software Foundation, Inc.
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
using System.Xml.Private;

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
	private bool xmlPopScope;
	private bool xmlnsPopScope;
	private int depth;
	private NodeManager nodes;
	private ReadState readState;
	private Stack elementNames;
	private WhitespaceHandling whitespace;
	private XmlDTDReader dtdReader;
	private XmlParserContext context;
	private XmlParserInput input;
	private XmlResolver resolver;

	private readonly Object xmlBaseName;
	private readonly Object xmlLangName;
	private readonly Object xmlSpaceName;
	private readonly Object xmlNSPrefix;


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

				xmlBaseName = nt.Add("xml:base");
				xmlLangName = nt.Add("xml:lang");
				xmlSpaceName = nt.Add("xml:space");
				xmlNSPrefix = nt.Add("xmlns");

				contextSupport = false;
				xmlPopScope = false;
				xmlnsPopScope = false;
				incDepth = false;
				elementNames = new Stack();
				dtdReader = new XmlDTDReader(context);
				nodes = new NodeManager(nt, new ErrorHandler(Error));
				input = new XmlParserInput
					(null, nt, new EOFHandler(HandleEOF), new ErrorHandler(Error));
				context = new XmlParserContext
					(nt, new XmlNamespaceManager(nt), String.Empty, XmlSpace.None);
				resolver = new XmlUrlResolver();
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
					context.BaseURI = String.Empty;
				}
				else
				{
					context.BaseURI = nt.Add(url);
				}
				XmlStreamReader sr = new XmlStreamReader(input, true);
				this.input.Reader = sr;
				context.Encoding = sr.CurrentEncoding;
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
					context.BaseURI = String.Empty;
				}
				else
				{
					context.BaseURI = nt.Add(url);
				}
				this.input.Reader = input;
				if(input as StreamReader != null)
				{
					context.Encoding = ((StreamReader)input).CurrentEncoding;
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
					this.context.Encoding = sr.CurrentEncoding;
					contextSupport = false;
				}
				else
				{
					this.context.BaseURI = context.BaseURI;
					this.context.DocTypeName = context.DocTypeName;
					this.context.InternalSubset = context.InternalSubset;
					this.context.NamespaceManager = context.NamespaceManager;
					this.context.PublicId = context.PublicId;
					this.context.SystemId = context.SystemId;
					this.context.XmlLang = context.XmlLang;
					this.context.XmlSpace = context.XmlSpace;
					dtdReader.Context = this.context;
					contextSupport = true;
					XmlStreamReader sr = new XmlStreamReader(xmlFragment, true);
					if(context.Encoding == null)
					{
						this.context.Encoding = sr.CurrentEncoding;
					}
					else
					{
						this.context.Encoding = context.Encoding;
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
					this.context.BaseURI = String.Empty;
					XmlStreamReader sr = new XmlStreamReader(new StringReader(xmlFragment));
					input.Reader = sr.TxtReader;
					this.context.Encoding = sr.CurrentEncoding;
					contextSupport = false;
				}
				else
				{
					this.context.BaseURI = context.BaseURI;
					this.context.XmlLang = context.XmlLang;
					this.context.XmlSpace = context.XmlSpace;
					dtdReader.Context = this.context;
					contextSupport = true;
					XmlStreamReader sr = new XmlStreamReader(new StringReader(xmlFragment));
					if(context.Encoding == null)
					{
						this.context.Encoding = sr.CurrentEncoding;
					}
					else
					{
						this.context.Encoding = context.Encoding;
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
				context.BaseURI = nt.Add(url);
				context.Encoding = sr.CurrentEncoding;
				context.NameTable = nt;
			}

	// Get the number of attributes on the current node.
	public override int AttributeCount
			{
				get { return nodes.Current.AttributeCount; }
			}

	// Get the base URI for the current node.
	public override String BaseURI
			{
				get { return context.BaseURI; }
			}

	// Get the depth of the current node.
	public override int Depth
			{
				get { return depth + nodes.Current.DepthOffset; }
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
						return context.Encoding;
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
				get { return nodes.Current.IsEmptyElement; }
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
				get { return nodes.Current.LocalName; }
			}

	// Get the fully-qualified name of the current node.
	public override String Name
			{
				get { return nodes.Current.Name; }
			}

	// Get the name table that is used to look up and resolve names.
	public override XmlNameTable NameTable
			{
				get { return context.NameTable; }
			}

	// Get the namespace URI associated with the current node.
	public override String NamespaceURI
			{
				get { return nodes.Current.NamespaceURI; }
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
				get { return nodes.Current.NodeType; }
			}

	// Get the namespace prefix associated with the current node.
	public override String Prefix
			{
				get { return nodes.Current.Prefix; }
			}

	// Get the quote character that was used to enclose an attribute value.
	public override char QuoteChar
			{
				get { return nodes.Current.QuoteChar; }
			}

	// Get the current read state of the reader.
	public override ReadState ReadState
			{
				get { return readState; }
			}

	// Get the text value of the current node.
	public override String Value
			{
				get { return nodes.Current.Value; }
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
				get { return context.XmlLang; }
			}

	// Set the resolver to use to resolve DTD references.
	public XmlResolver XmlResolver
			{
				set { resolver = value; }
			}

	// Get the current xml:space scope.
	public override XmlSpace XmlSpace
			{
				get { return context.XmlSpace; }
			}

	internal XmlParserContext ParserContext 
			{
				get { return context; }
			}

	// Clean up the resources that were used by this XML reader.
	[TODO] // ********************************************************* TODO
	public override void Close()
			{
				context.SystemId = String.Empty;
				context.PublicId = String.Empty;
				depth = 0;
				contextSupport = false;
				input.Close();
			}

	// Returns the value of an attribute with a specific index.
	public override String GetAttribute(int i)
			{
				NodeInfo tmp = nodes.Current.GetAttribute(i);
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
				NodeInfo tmp = nodes.Current.GetAttribute(localName, namespaceURI);
				if(tmp == null) { return null; }
				return tmp.Value;
			}

	// Returns the value of an attribute with a specific qualified name.
	public override String GetAttribute(String name)
			{
				NodeInfo tmp = nodes.Current.GetAttribute(name);
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
				return context.NamespaceManager.LookupNamespace(prefix);
			}

	// Move the current position to a particular attribute.
	public override void MoveToAttribute(int i)
			{
				if(!nodes.Current.MoveToAttribute(i))
				{
					throw new ArgumentOutOfRangeException
						("i", S._("Xml_InvalidAttributeIndex"));
				}
			}

	// Move the current position to an attribute with a particular name.
	public override bool MoveToAttribute(String localName, String namespaceURI)
			{
				return nodes.Current.MoveToAttribute(localName, namespaceURI);
			}

	// Move the current position to an attribute with a qualified name.
	public override bool MoveToAttribute(String name)
			{
				return nodes.Current.MoveToAttribute(name);
			}

	// Move to the element that owns the current attribute.
	public override bool MoveToElement()
			{
				return nodes.Current.MoveToElement();
			}

	// Move to the first attribute owned by the current element.
	public override bool MoveToFirstAttribute()
			{
				return nodes.Current.MoveToFirstAttribute();
			}

	// Move to the next attribute owned by the current element.
	public override bool MoveToNextAttribute()
			{
				return nodes.Current.MoveToNextAttribute();
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
				return nodes.Current.ReadAttributeValue();
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
					int count = nodes.Current.AttributeCount;
					NodeInfo tmp;
					for (int i = 0; i < count; ++i)
					{
						tmp = nodes.Current.GetAttribute(i);
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
					NodeInfo node = nodes.Current;
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
				context.SystemId = String.Empty;
				context.PublicId = String.Empty;
				nodes.Reset();
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

	// Get the namespace information for a given name.
	private void GetNamespaceInfo
				(String name, out String localName, out String namespaceURI,
				 out String prefix)
			{
				XmlNameTable nt = context.NameTable;

				// set the defaults
				localName = name;
				prefix = String.Empty;
				namespaceURI = String.Empty;

				if(namespaces)
				{
					XmlNamespaceManager nm = context.NamespaceManager;

					// find the namespace separator
					int index = name.LastIndexOf(':');

					// give an error if there's no name before the separator
					if(index == 0)
					{
						Error(/* TODO */);
					}

					// set the namespace information
					if(index > 0)
					{
						// add the prefix and local name to the name table
						prefix = nt.Add(name.Substring(0, index));
						localName = nt.Add(name.Substring(index + 1));

						// check for a valid prefix
						if(prefix.IndexOf(':') != -1)
						{
							Error(/* TODO */);
						}
					}
					// set the namespace uri based on the prefix
					namespaceURI = nm.LookupNamespace(prefix);
				}
			}

	// Callback for eof in the input handler.
	[TODO]
	private void HandleEOF()
			{
				readState = ReadState.EndOfFile;
			}

	// Process 'xml:' and 'xmlns' information for a list of attributes.
	private void ProcessAttributeInfo(Attributes attributes)
			{
				AttributeInfo info;
				int count = attributes.Count;
				for(int i = 0; i < count; ++i)
				{
					info = attributes[i];
					String tmpName = info.Name;
					String tmpValue = info.Value;
					if(((Object)tmpName) == xmlBaseName)
					{
						context.BaseURI = tmpValue;
					}
					else if(((Object)tmpName) == xmlLangName)
					{
						context.XmlLang = tmpValue;
					}
					else if(((Object)tmpName) == xmlSpaceName)
					{
						if(tmpValue == "default")
						{
							context.XmlSpace = XmlSpace.Default;
						}
						else if(tmpValue == "preserve")
						{
							context.XmlSpace = XmlSpace.Preserve;
						}
						else
						{
							Error(/* TODO */);
						}
					}
					else if(namespaces)
					{
						XmlNamespaceManager ns = context.NamespaceManager;
						if(((Object)tmpName) == xmlNSPrefix)
						{
							ns.AddNamespace("", tmpValue);
						}
						else if(tmpName.StartsWith("xmlns:"))
						{
							String tmpLocalName = tmpName.Substring(6);
							ns.AddNamespace(tmpLocalName, tmpValue);
						}
					}
				}
			}

	// Read an attribute value.
	//
	// Already read: ('"' | "'")
	private void ReadAttributeValue
				(StringBuilder log, Segments segments, char quoteChar)
			{
				int segLen = 0;
				int textStart = 0;
				SegmentInfo seg;
				XmlNameTable nt = context.NameTable;

				// push the log onto the logger's log stack
				input.Logger.Push(log);

				// read until we consume all the text
				while(input.PeekChar() && input.peekChar != quoteChar)
				{
					// perform error checks and process segments
					if(input.peekChar == '<')
					{
						Error(/* TODO */);
					}
					else if(input.peekChar == '&')
					{
						// if we have text, add a new text segment to the list
						if(log.Length > textStart)
						{
							int textEnd = (log.Length - textStart);
							seg = segments[segLen++];
							seg.SetInfo(true, log.ToString(textStart, textEnd));
						}

						// move to the '&' character
						input.NextChar();

						// add a new reference segment to the list
						seg = segments[segLen++];
						seg.SetInfo(false, nt.Add(ReadReference()));

						// store the start index of the next segment
						textStart = log.Length;
					}
					else
					{
						input.NextChar();
					}
				}

				// if we have text, add a new text segment to the list
				if(log.Length > textStart)
				{
					int textEnd = (log.Length - textStart);
					seg = segments[segLen++];
					seg.SetInfo(true, log.ToString(textStart, textEnd));
				}

				// store the number of segments
				segments.Count = segLen;

				// pop the log from the logger's log stack
				input.Logger.Pop();
			}

	// Read and normalize an attribute value.
	//
	// Already read: ('"' | "'")
	private void ReadAttributeValueNormalize
				(StringBuilder log, Segments segments, char quoteChar)
			{
				int segLen = 0;
				int textStart = 0;
				SegmentInfo seg;
				XmlNameTable nt = context.NameTable;

				// push the log onto the logger's log stack
				input.Logger.Push(log);

				// read until we consume all the text
				while(input.PeekChar() && input.peekChar != quoteChar)
				{
					// perform error checks and process segments
					if(input.peekChar == '<')
					{
						Error(/* TODO */);
					}
					else if(input.peekChar == '&')
					{
						bool haveText = (log.Length > textStart);
						int textEnd = (log.Length - textStart);
						int position = log.Length;
						String s; char c;

						// move to the '&' character
						input.NextChar();

						// read the reference
						if(ReadReferenceNormalize(out s, out c))
						{
							log.Length = position;
							log.Append(c);
						}
						else
						{
							// if we have text, add a new text segment to the list
							if(haveText)
							{
								seg = segments[segLen++];
								seg.SetInfo(true, log.ToString(textStart, textEnd));
							}

							// add a new reference segment to the list
							seg = segments[segLen++];
							seg.SetInfo(false, nt.Add(s));

							// store the start index of the next segment
							textStart = log.Length;
						}
					}
					else if(XmlCharInfo.IsWhitespace(input.peekChar))
					{
						// skip whitespace without logging
						input.Logger.Pop();
						input.SkipWhitespace();
						input.Logger.Push(log);

						// set whitespace to single space character
						log.Append(' ');
					}
					else
					{
						input.NextChar();
					}
				}

				// if we have text, add a new text segment to the list
				if(log.Length > textStart)
				{
					int textEnd = (log.Length - textStart);
					seg = segments[segLen++];
					seg.SetInfo(true, log.ToString(textStart, textEnd));
				}

				// store the number of segments
				segments.Count = segLen;

				// pop the log from the logger's log stack
				input.Logger.Pop();
			}

	// Read the attributes for an element start tag or xml declaration tag.
	//
	// Already read: ''
	[TODO]
	private void ReadAttributes(bool qmark)
			{
				// create our value log
				StringBuilder log = new StringBuilder();

				// get our attribute list
				int attLen = 0;
				Attributes attributes = nodes.Attributes;

				// read until we consume all of the attributes
				while(true)
				{
					// skip potentially optional whitespace
					bool hasWS = input.SkipWhitespace();

					// check for an end character
					if(!input.PeekChar()) { Error("Xml_UnexpectedEOF"); }
					if(qmark)
					{
						if(input.peekChar == '?')
						{
							attributes.Count = attLen;
							return;
						}
					}
					else if(input.peekChar == '/' || input.peekChar == '>')
					{
						attributes.Count = attLen;
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

					// get the attribute and segment information nodes
					AttributeInfo att = attributes[attLen++];
					Segments segments = att.Segments;

					// read the attribute value
					if(normalize && !qmark)
					{
						ReadAttributeValueNormalize(log, segments, quoteChar);
					}
					else
					{
						ReadAttributeValue(log, segments, quoteChar);
					}

					// the attribute value must be properly terminated
					if(!input.NextChar()) { Error("Xml_UnexpectedEOF"); }
					if(input.currChar != quoteChar) { Error(/* TODO */); }

					// get the value from the log
					String value = log.ToString();

					// reset the log
					log.Length = 0;

					// set the attribute information
					att.SetInfo(segments, quoteChar, name, value);
				}
			}

	// Read a character data section.
	//
	// Already read: '<![CDATA['
	[TODO]
	private void ReadCDATA()
			{
				// TODO: state handling

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
							nodes.CDATA.SetInfo(value);
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
						nodes.Comment.SetInfo(value);
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
				// use the iterable node's next node if possible
				NodeInfo current = nodes.Current;
				if(current is IterableNodeInfo)
				{
					if(((IterableNodeInfo)current).Next())
					{
						return true;
					}
				}

				// return false if there are no nodes left to read
				if(!input.PeekChar()) { return false; }

				// increase the depth if we last read an element start tag
				if(incDepth)
				{
					++depth;
					incDepth = false;
				}

				// pop the base/lang/space scope if we last read an element end tag
				if(xmlPopScope)
				{
					context.PopScope();
					xmlPopScope = false;
				}

				// pop the namespace scope if we last read an element end tag
				if(xmlnsPopScope)
				{
					context.NamespaceManager.PopScope();
					xmlnsPopScope = false;
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

				// (re)initialize the dtd reader
				dtdReader.Init(input, resolver);

				// read the dtd
				dtdReader.Read();

				// set the current node information
				nodes.DoctypeDeclaration.SetInfo
					(context.DocTypeName, context.InternalSubset);
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

				// read the element name
				String name = input.ReadName();

				// skip optional whitespace
				input.SkipWhitespace();

				// the element end tag must end with '>' at this point
				input.Expect('>');

				// decrease the depth
				--depth;

				// set a flag to pop the base/lang/space scope on the next read
				xmlPopScope = true;

				// set a flag to pop the namespace scope on the next read
				xmlnsPopScope = namespaces;

				// this end tag must match the last start tag
				if(elementNames.Pop() != (Object)name) { Error(/* TODO */); }

				// add the name to the name table
				XmlNameTable nt = context.NameTable;
				name = nt.Add(name);

				// get the namespace information
				String localName;
				String namespaceURI;
				String prefix;
				GetNamespaceInfo
					(name, out localName, out namespaceURI, out prefix);

				// set the current node information
				nodes.EndElement.SetInfo(localName, name, namespaceURI, prefix);
			}

	// Read a processing instruction.
	//
	// Already read: '<?' Name
	[TODO]
	private void ReadProcessingInstruction(String target)
			{
				// TODO: state handling
				// TODO: check target for ('X'|'x')('M'|'m')('L'|'l')

				// skip potentially optional whitespace
				bool hasWS = input.SkipWhitespace();

				// check for the closing characters
				if(!input.NextChar()) { Error("Xml_UnexpectedEOF"); }
				if(!input.PeekChar()) { Error("Xml_UnexpectedEOF"); }
				if(input.currChar == '?' && input.peekChar == '>')
				{
					input.NextChar();

					// set the current node information and return
					nodes.ProcessingInstruction.SetInfo(target, String.Empty);
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
						nodes.ProcessingInstruction.SetInfo(target, value);
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
	private String ReadReference()
			{
				// check for an empty reference
				if(!input.PeekChar()) { Error(/* TODO */); }
				if(input.peekChar == ';') { Error(/* TODO */); }

				// create our log and push it onto the logger's log stack
				StringBuilder log = new StringBuilder();
				input.Logger.Push(log);

				// handle character or general references
				if(input.peekChar == '#')
				{
					input.NextChar();

					// check for an empty character reference
					if(!input.PeekChar()) { Error(/* TODO */); }
					if(input.peekChar == ';') { Error(/* TODO */); }

					// handle a hex or decimal character reference
					if(input.peekChar == 'x')
					{
						input.NextChar();

						// check for an empty hex character reference
						if(!input.PeekChar()) { Error(/* TODO */); }
						if(input.peekChar == ';') { Error(/* TODO */); }

						// read until we consume all the digits
						while(input.NextChar() && input.currChar != ';')
						{
							if((input.currChar < '0' || input.currChar > '9') &&
							   (input.currChar < 'A' || input.currChar > 'F') &&
							   (input.currChar < 'a' || input.currChar > 'f'))
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
							if(input.currChar < '0' || input.currChar > '9')
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
				// pop the log and return the reference name
				input.Logger.Pop();
				return context.NameTable.Add(log.ToString(0, log.Length-1));
			}

	// Read and normalize an entity reference, returning true if normalized.
	//
	// Already read: '&'
	private bool ReadReferenceNormalize(out String name, out char value)
			{
				// check for an empty reference
				if(!input.PeekChar()) { Error(/* TODO */); }
				if(input.peekChar == ';') { Error(/* TODO */); }

				// set the defaults
				name = null;
				value = (char)0;

				// handle character or general references
				if(input.peekChar == '#')
				{
					input.NextChar();

					// check for an empty character reference
					if(!input.PeekChar()) { Error(/* TODO */); }
					if(input.peekChar == ';') { Error(/* TODO */); }

					// handle a hex or decimal character reference
					if(input.peekChar == 'x')
					{
						input.NextChar();

						// check for an empty hex character reference
						if(!input.PeekChar()) { Error(/* TODO */); }
						if(input.peekChar == ';') { Error(/* TODO */); }

						// read until we consume all the digits
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
						// read until we consume all the digits
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

					// we hit eof, otherwise we'd have ';', so give an error
					if(input.currChar != ';') { Error("Xml_UnexpectedEOF"); }

					// check the range of the character
					if(!XmlCharInfo.IsChar(value))
					{
						Error(/* TODO */);
					}

					return true;
				}
				else
				{
					// read the reference name
					name = input.ReadName();

					// the reference must end with ';' at this point
					input.Expect(';');

					// check for builtins
					switch(name)
					{
						case "amp":
						{
							value = '&';
							return true;
						}
						// Not reached.

						case "apos":
						{
							value = '\'';
							return true;
						}
						// Not reached.

						case "gt":
						{
							value = '>';
							return true;
						}
						// Not reached.

						case "lt":
						{
							value = '<';
							return true;
						}
						// Not reached.

						case "quot":
						{
							value = '"';
							return true;
						}
						// Not reached.

						default:
						{
							return false;
						}
						// Not reached.
					}
				}
			}

	// Read an element start tag.
	//
	// Already read: '<'
	[TODO]
	private void ReadSTag()
			{
				// TODO: state handling

				// read the element name
				String name = input.ReadName();

				// read the element's attributes
				ReadAttributes(false);

				// check if this is an empty element or not
				if(!input.NextChar()) { Error("Xml_UnexpectedEOF"); }
				bool empty = false;
				if(input.currChar == '/')
				{
					input.Expect('>');
					empty = true;
					xmlPopScope = true;
					xmlnsPopScope = namespaces;
				}
				else
				{
					incDepth = true;
					elementNames.Push(name);
				}

				// push the "xml:" scope
				context.PushScope();

				// push the "xmlns" scope
				if(namespaces)
				{
					context.NamespaceManager.PushScope();
				}

				// process the attribute information
				Attributes attributes = nodes.Attributes;
				ProcessAttributeInfo(attributes);

				// get the namespace information
				String localName;
				String namespaceURI;
				String prefix;
				GetNamespaceInfo
					(name, out localName, out namespaceURI, out prefix);

				// update search information and check for duplicates
				if(namespaces)
				{
					attributes.UpdateInfo
						(context.NameTable, context.NamespaceManager, namespaceURI);
				}
				else
				{
					attributes.UpdateInfo(context.NameTable);
				}

				// set the current node information
				nodes.Element.SetInfo
					(empty, attributes, localName, name, namespaceURI, prefix);
			}

	// Read a text node.
	//
	// Already read: ''
	[TODO]
	private void ReadText()
			{
				// set things up
				int segLen = 0;
				SegmentInfo seg;
				TextInfo info = nodes.Text;
				Segments segments = info.Segments;
				XmlNameTable nt = context.NameTable;

				// create our log and push it onto the logger's log stack
				StringBuilder log = new StringBuilder();
				input.Logger.Push(log);

				// read until we consume all the text
				while(input.PeekChar() && input.peekChar != '<')
				{
					if(input.peekChar == '&')
					{
						String s; char c;

						// skip the '&' character
						input.Logger.Pop();
						input.NextChar();

						// read the reference
						if(ReadReferenceNormalize(out s, out c))
						{
							log.Append(c);
						}
						else
						{
							// if we have text, add a new text segment to the list
							if(log.Length > 0)
							{
								seg = segments[segLen++];
								seg.SetInfo(true, log.ToString());
								log.Length = 0;
							}

							// add a new reference segment to the list
							seg = segments[segLen++];
							seg.SetInfo(false, nt.Add(s));
						}

						// push the log back onto the log stack
						input.Logger.Push(log);
					}
					else
					{
						input.NextChar();
					}
				}

				// if we have text, add a new text segment to the list
				if(log.Length > 0)
				{
					seg = segments[segLen++];
					seg.SetInfo(true, log.ToString());
					log.Length = 0;
				}

				// store the number of segments
				segments.Count = segLen;

				// pop the log from the logger's log stack
				input.Logger.Pop();

				// set the current node information
				info.SetInfo(segments);
			}

	// Read a whitespace node.
	//
	// Already read: ''
	[TODO]
	private void ReadWhitespace()
			{
				// TODO: state handling
				// TODO: figure out when whitespace is significant

				// create our log and push it onto the logger's log stack
				StringBuilder log = new StringBuilder();
				input.Logger.Push(log);

				// skip the required whitespace
				if(!input.SkipWhitespace()) { Error(/* TODO */); }

				// get the whitespace from the log and pop it from the logger
				String value = input.Logger.Pop().ToString();

				// set the current node information
				nodes.Whitespace.SetInfo(false, value);
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

				// create our log and push it onto the logger's log stack
				StringBuilder log = new StringBuilder();
				input.Logger.Push(log);

				// read the xml declaration's attributes
				ReadAttributes(true);

				// the xml declaration must end with '?>' at this point
				input.Expect("?>");

				// get attributes from the log and pop it from the logger
				String value = input.Logger.Pop().ToString();

				// update search information and check for duplicates
				Attributes attributes = nodes.Attributes;
				attributes.UpdateInfo(context.NameTable);

				// set the current node information
				nodes.XmlDeclaration.SetInfo(attributes, value);
			}

}; // class XmlTextReader

}; // namespace System.Xml
