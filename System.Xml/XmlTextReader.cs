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
	private XmlAttribute attr;
	private XmlAttributeCollection attributes;
	private int attributeIndex;
	private int depth;
	private bool isEmpty;
	private bool readAttribute;
	private bool contextSupport;
	private StringBuilder builder;
	private String	name;
	private String systemId;
	private String publicId;
	private XmlParserContext parserContext;
	private String docTypeName;
	private String internalSubset;
	private bool isEntity = false;
	private int startEntity = 0;

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
				docTypeName = String.Empty;
				internalSubset = String.Empty;
				value = String.Empty;
				attr = new XmlAttributeToken(nameTable,null,null);
				attributes = new XmlAttributeCollection(attr); 
				attributeIndex = -1;
				depth = 0;
				isEmpty = false;
				contextSupport = false;
				name = String.Empty;
				readAttribute = false;
				systemId = String.Empty;
				publicId = String.Empty;
				parserContext = new XmlParserContext(nt, namespaceManager, xmlLang,
						xmlSpace );
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
				XmlStreamReader sr = new XmlStreamReader(input, true);
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
					baseURI = String.Empty;
					XmlStreamReader sr = new XmlStreamReader(xmlFragment, true);
					encoding = sr.CurrentEncoding;
				}
				else
				{
					baseURI = context.BaseURI;
					encoding = context.Encoding;
					xmlLang = context.XmlLang;
					xmlSpace = context.XmlSpace;
					contextSupport = true;
					parserContext = context;
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
					baseURI = String.Empty;
					XmlStreamReader sr = new XmlStreamReader(new StringReader(xmlFragment));
					
					encoding = sr.CurrentEncoding;
					reader = sr.TxtReader;
				}
				else
				{
					baseURI = context.BaseURI;
					xmlLang = context.XmlLang;
					xmlSpace = context.XmlSpace;
					contextSupport = true;
					XmlStreamReader sr = new XmlStreamReader(new StringReader(xmlFragment));
					if(context.Encoding == null)
					{
						encoding = sr.CurrentEncoding;
					}
					else
					{
						encoding = context.Encoding;
					}
					reader = sr.TxtReader;
					parserContext = context;
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
				XmlStreamReader sr = new XmlStreamReader(url, true);
				encoding = sr.CurrentEncoding;
				reader = sr;
				parserContext.NameTable = nt;
			}

	// Clean up the resources that were used by this XML reader.
	public override void Close()
			{
				prefix = String.Empty;
				localName = String.Empty;
				namespaceURI = String.Empty;
				value = String.Empty;
				systemId = String.Empty;
				publicId = String.Empty;
				attr = new XmlAttributeToken(nameTable,null,null);
				attributes = new XmlAttributeCollection(attr); 
				attributeIndex = -1;
				depth = 0;
				isEmpty = false;
				contextSupport = false;
				name = String.Empty;
				readAttribute = false;
				reader = null;
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
	public TextReader GetRemainder()
			{
				String tmp = reader.ReadToEnd();
				StringReader s = new StringReader(tmp);
				Close();
				readState = ReadState.EndOfFile;
				
				return s;
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
				systemId = String.Empty;
				publicId = String.Empty;
				attr = new XmlAttributeToken(nameTable,null,null);
				attributes = new XmlAttributeCollection(attr);
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
	private String ReadIdentifier(int ch)
			{
				StringBuilder builder = new StringBuilder();

				if(ch != -1)
				{
					builder.Append((char)ch);
				}
				
				while((ch = ReadChar()) != -1)
				{
					if((char)ch == '/')
					{
						SkipWhite();
						Expect('>');
						nodeType = XmlNodeType.EndElement;
						isEmpty = true;
						readAttribute = false;
						return builder.ToString();
					}
					else if((char)ch == '>')
					{
						nodeType = XmlNodeType.Element;
						readAttribute = false;
						return builder.ToString();
					}
					else if (Char.IsWhiteSpace((char)ch))
					{
						SkipWhite();
						// more error checking
						if((char)ch != '>' && (char)ch != '/')
						{
							nodeType = XmlNodeType.Element;
							readAttribute = true;
							
							return builder.ToString();
						}
						else if((char)ch == '>')
						{
							nodeType = XmlNodeType.Element;
							readAttribute = false;
							return builder.ToString();
						}
						else if((char)ch == '/')
						{
							SkipWhite();
							Expect('>');
							nodeType = XmlNodeType.EndElement;
							isEmpty = true;
							readAttribute = false;
							return builder.ToString();
						}
					}
					builder.Append((char)ch);
				}
				return null;
			}
#if !ECMA_COMPAT	
	private String ParseEntity(String entityref)
			{
				String value = parserContext.NameCollection.Get(entityref);
				return value;
			}
#endif
	
	
	// Set the name information from an identifier.
	private void SetName(String identifier)
			{
				int index = identifier.LastIndexOf(':');
				if(index >= 0)
				{
					prefix = nameTable.Add(identifier.Substring(0, index));
					localName = nameTable.Add(identifier.Substring(index + 1));
					namespaceURI = namespaceManager.LookupNamespace(prefix);
				}
				else
				{
					prefix = String.Empty;
					localName = nameTable.Add(identifier);
					namespaceURI = String.Empty;
				}
			}

	internal string ReadTo(char stop)
			{
				StringBuilder builder = new StringBuilder();
				int ch = 0;
				while((ch = ReadChar()) != -1 && (char)ch != stop)
				{
					builder.Append((char)ch);
				}
				return builder.ToString();
			}
	
#if !ECMA_COMPAT
	internal void ReadDTDInternalSubset(int ch)
			{
				StringBuilder builder = new StringBuilder();
				SkipWhite ();
				builder.Append((char)ch);
				switch((char)ch)
				{
					case ']':
						nodeType = XmlNodeType.None;
						break;
					case '%':
						string peName = ReadIdentifier(ch); 
						Expect (';');
						nodeType = XmlNodeType.EntityReference;	
						break;
					case '<':
						ch = ReadChar();
						builder.Append((char)ch);
						switch((char)ch)
						{
							case '!':
								ch = ReadChar();
								builder.Append((char)ch);
								switch((char)ch)
								{
									case '-':
										AnalyzeChar(ch, true);
										break;
									case 'E':
										ch = ReadChar();
										builder.Append((char)ch);
										switch((char)ch)
										{
											case 'N':
												while((ch = ReadChar()) != -1)
												{
													builder.Append((char)ch);
													if(builder.ToString() == "<!ENTITY")
													{
														SkipWhite();
														String name = ReadTo(' ');
														String space = ReadTo('\'');
														String value = ReadTo('\'');
														builder.Append(name + space + value + ReadTo('>'));
														parserContext.InternalSubset = builder.ToString();
														parserContext.NameCollection.Add(name, value);
														return;
													}
												}
												break;
											case 'L':
												while((ch = ReadChar()) != -1)
												{
													builder.Append((char)ch);
													if(builder.ToString() == "<!ELEMENT")
													{
														ch = ReadChar();
														if(!Char.IsWhiteSpace((char)ch))
															throw new XmlException(
																	S._("Xml_BadDTDDeclaration"));

														builder.Append((char)ch);
														builder.Append(ReadTo('>'));
														parserContext.InternalSubset = builder.ToString();
														return;
													}
												}
												break;
											default:
												throw new XmlException(S._("Syntax Error after '<!E' (ELEMENT or ENTITY must be found)"));
										}
										break;
									case 'A':
										//Expect ("TTLIST");
										//ReadAttListDecl ();
										break;
									case 'N':
										//Expect ("OTATION");
										//ReadNotationDecl ();
										break;
									default:
										throw new XmlException(S._("Syntax Error after '<!' characters."));
								}
								break;
							default:
								throw new XmlException(S._("Syntax Error after '<' character."));
						}
						break;
					default:
						throw new XmlException(S._("Syntax Error inside doctypedecl markup."));
				}

				return;
			}
#endif
	
	
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
					if((char)ch == '\n')
					{
						lineNumber++;
						linePosition = 0;
					}
					else if(Char.IsWhiteSpace((char)ch))
					{
						linePosition++;
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

	internal void AnalyzeChar(int ch, bool structFlag)
			{

				if(ch == -1)
				{
					if(linePosition > 1)
					{
						--linePosition;
					}
					readState = ReadState.EndOfFile;
					ClearNodeInfo();
					return;
				}
				
				switch((char)ch)
				{
					case '<':
						ch = ReadChar();
						
						// Set structFlag to true for specialchar
						// start sequences
						AnalyzeChar(ch, true);
						break;
					case '/':
						// Check for correct structure 
						if(structFlag == true)
						{
							ClearNodeInfo();
							SkipWhite();
							name = ReadIdentifier(-1);
							SetName(name);
							nodeType = XmlNodeType.EndElement;
							value = String.Empty;
							--depth;
						}
						else
						{
							// This will handle any '/' in Text
							// Nodes.
							goto default;
						}
						break;
					case '?':
						// Processing Instructions
						// <? PITarget 
						if(structFlag != true)
						{
							goto default;
						}

						/* else */

						ClearNodeInfo();
						builder = new StringBuilder();
						while((ch = ReadChar()) != -1)
						{
							builder.Append((char)ch);
							if((char)ch == '?' && (char)reader.Peek() == '>' || Char.IsWhiteSpace((char)ch))
							{
								if(builder.ToString(0, builder.Length -1) == "xml")
								{
									nodeType = XmlNodeType.XmlDeclaration;
								}
								else
								{
									nodeType = XmlNodeType.ProcessingInstruction;
								}
								break;
							}
						}
						
						
						while((ch = ReadChar()) != -1)
						{
							builder.Append((char)ch);
							if((char)ch == '?' && (char)reader.Peek() == '>')
							{
								value = builder.ToString(0, builder.Length -1);
								ReadChar();
								break;
							}
						}
						
						break;
					case '!':
						// Check for correct structure 
						if(structFlag != true)
						{
							goto default;
						}

						// Comment, CDATA, or document type information.
						ch = ReadChar();
						AnalyzeChar(ch, true);
						break;
					case '-':
						// Parse the "<!--" comment start sequence.
						if(structFlag != true)
						{
							goto default;
						}

						ch = ReadChar();
						if((char)ch != '-')
						{
							UngetAndThrow(ch);
						}

						// Search for the "-->" comment end sequence.
						int count = 0;
						while((ch = ReadChar()) != -1)
						{
							builder.Append((char)ch);
						
							if((char)ch == '-')
							{
									++count;
							}
							else if((char)ch == '>')
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
							if((char)ch == '\n')
							{
								++lineNumber;
								linePosition = 0;
							}
						}
						if((char)ch != '>')
						{
							UngetAndThrow(ch);
						}

						// Create a comment node and return.
						ClearNodeInfo();
						nodeType = XmlNodeType.Comment;
						value = builder.ToString();
						break;
						
					case '[':
						if(structFlag != true)
						{
							goto default;
						}
						builder = new StringBuilder();
						while((ch = ReadChar()) != -1)
						{
							builder.Append((char)ch);
							if((char)ch == '[')
							{
								// reset to grab all input between '[' ']'
								builder = new StringBuilder();
							}
							else if((char)ch == ']')
							{
								// for speed just check for first ] 
								// then we check for ]]> as a whole
								builder.Append((char)ReadChar());
								builder.Append((char)ReadChar());
								if(builder.ToString().Substring(builder.Length - 3, 3) == "]]>")
								{
									// break the read and state Value
									ClearNodeInfo();
									value = builder.ToString(0, builder.Length -3);
									break;
								}
						
							}
						}
						break;
							
					case 'D':
						if(structFlag != true)
						{
							goto default;
						}
						ClearNodeInfo();	
						// document type nodes
						// clean the buffer
						builder = new StringBuilder();
						builder.Append((char)ch);
						count = 0;
						while ((ch = ReadChar()) != -1)
						{
							if(Char.IsWhiteSpace((char)ch) && count == 0)
							{
								nodeType = XmlNodeType.DocumentType;
								if(builder.ToString() != "DOCTYPE")
								{
									throw new XmlException
										(S._("Xml_Malformed"));
								}
								SkipWhite();
								builder = new StringBuilder();
								while((ch = ReadChar()) != -1)
								{
									
									if(Char.IsWhiteSpace((char)ch))
									{
										SkipWhite();
										name = builder.ToString();
										break;
									}
									else if((char)ch == '>')
									{
										name = builder.ToString();
										return;
									}	
									else
									{
										builder.Append((char)ch);	
									}
								}
								SkipWhite();
								if((char)ch == '>')
								{
									return;
								}
								builder = new StringBuilder();
								while((ch = ReadChar()) != -1)
								{
									builder.Append((char)ch);
									if(Char.IsWhiteSpace((char)reader.Peek()))
									{
#if !ECMA_COMPAT
										if(builder.ToString() == "SYSTEM")
										{
											builder = new StringBuilder();
											SkipWhite();
											int qChar = ReadChar();
											ch = ReadChar();
											while(ch != qChar && ch != -1)
											{
												builder.Append((char)ch);
												ch = ReadChar();
											}
											systemId = builder.ToString();
											
											break;
										}
										else if(builder.ToString() == "PUBLIC")
										{
											builder = new StringBuilder();
											SkipWhite();
											int qChar = ReadChar();
											ch = ReadChar();
											while(ch != qChar && ch != -1)
											{
												builder.Append((char)ch);
												ch = ReadChar();
												// TODO: Check for non-pubid characters
											}
											publicId = builder.ToString();
											builder = new StringBuilder();
											SkipWhite();
											qChar = ReadChar();
											ch = ReadChar();
											while(ch != qChar && ch != -1)
											{
												builder.Append((char)ch);
												ch = ReadChar();
											}
											systemId = builder.ToString();
											break;
										}
										else if((char)ch == '[')
										{
											// seek to ']'
											SkipWhite();
											
											while((ch = ReadChar()) != -1)
											{
												builder.Append((char)ch);
												if((char)ch == ']' && (char)reader.Peek() == '>')
												{
													internalSubset = builder.ToString();
													ReadChar();
													return;
												}
											}
											
											break;
										}
										else
										{
											name = builder.ToString();
											SkipWhite();
											ch = ReadChar();
											if((char)ch == '[')
											{
												// seek to ']'
												SkipWhite();
												ch = ReadChar();
												builder.Append((char)ch);
												ReadDTDInternalSubset(ch);
												break;
											}

											break;
										}
#else
										if((char)ch == '[')
										{
											isEntity = true;
										}
										if((char)ch == ']' && (char)reader.Peek() == '>' 
												&& isEntity == true)
										{
											internalSubset = builder.ToString();
											ReadChar();
											return;
										}
#endif

									}
								}

								SkipWhite();
								builder = new StringBuilder();
								int level = 0;
								if((char)reader.Peek() == '[')
								{
									// seek to ']'
									while((ch = ReadChar()) != -1)
									{
										builder.Append((char)ch);
										if((char)reader.Peek() == ']' && level == 0)
										{
											ReadChar();
											break;
										}
										else if((char)reader.Peek() == '[')
										{
											// DTDInternalSubset
											ReadChar();
											while(reader.Peek() != ']')
											{
#if !ECMA_COMPAT
												ReadDeclaration();
#else
												builder.Append((char)ch);
#endif
											}
										}
											
									}
									
								}
								else if((char)reader.Peek() == '>')
								{
									ReadChar();
									isEmpty = false;
									value = parserContext.InternalSubset;
									return;
								}

								builder = new StringBuilder();
								count++;
							}
							else if(Char.IsWhiteSpace((char)ch) && count == 1)
							{
								name = builder.ToString();
							}
							else if((char)ch == '[')
							{
								builder = new StringBuilder();
								while((ch = ReadChar()) != -1)
								{
									builder.Append((char)ch);
									if((char)ch == ']' && (char)reader.Peek() == '>')
									{
										ReadChar();
										break;
									}
								}
								value = builder.ToString(0, builder.Length - 1);
								break;
							}
							builder.Append((char)ch);
						}
						break;
					case '=':
						name = builder.ToString();
						SetName(name);
						nodeType = XmlNodeType.Attribute;
						attr = new XmlAttributeToken(nameTable, name ,null);
						attributeIndex++;
						// reset buffer
						builder = new StringBuilder();
						break;
					case '\'':
					case '"':
						// get quote
						quoteChar = (char)ch;
						if(nodeType != XmlNodeType.Attribute)
						{
							builder.Append((char)ch);
							AnalyzeChar(ReadChar(), false);
							return;	
						}
						
						// Get Text value of an Attribute	
						while((ch = ReadChar()) != -1)
						{
							if((char)ch == QuoteChar)
							{
								attr.Value = builder.ToString();
								attributes.Append(attr);
								if(!isEntity)
								{
									nodeType = XmlNodeType.Text;
								}
								else
								{
									nodeType = XmlNodeType.EntityReference;
								}
								if(prefix == "xmlns")
								{
									// append namespace
									namespaceManager.AddNamespace(localName, builder.ToString());
								}
								
								
								// go to next char
								SkipWhite();
								
								// reset buffer
								builder = new StringBuilder();
								ch = ReadChar();
								if((char)ch == '/')
								{
									readAttribute = false;
									isEmpty = true;
									Expect('>');
									break;
								}
								else if ((char)ch == '>')
								{
									readAttribute = false;
									break;
								}
								ungetch = ch;
								break;
							}
#if !ECMA_COMPAT
							else if((char)ch == '&')
							{
								// found entity reference
								isEntity = true;
								startEntity = builder.Length;
							}
							else if((char)ch == ';' && isEntity == true)
							{
								String entityRef = builder.ToString(startEntity, builder.Length - startEntity);
								builder.Remove(startEntity, builder.Length - startEntity);
								builder.Append( ParseEntity(entityRef) );
								isEntity = false;
							}
#endif
							else
							{
								builder.Append((char)ch);
							}
						}
						break;
					default:
						// save any text
						builder.Append((char)ch);
						// if '<'
						if(structFlag == true)
						{
							// move back one space
							ClearNodeInfo();
							name = ReadIdentifier(ch);
							// checking for termination '/'
							SetName(name);
							if(nodeType == XmlNodeType.Element)
								++depth;
							// reset buffer
							builder = new StringBuilder();
							break;
						}
						else
						{
#if !ECMA_COMPAT
							if((char)ch == '&')
							{
								// found entity reference
								isEntity = true;
								startEntity = builder.Length -1;
								builder.Remove(builder.Length-1, 1);
							}
#endif
							ch = ReadChar(); 	
							if((char)ch != '<')
							{
#if !ECMA_COMPAT
								if((char)ch == ';' && isEntity == true)
								{
									String entityRef = builder.ToString(startEntity, builder.Length - startEntity);
									builder.Remove(startEntity, builder.Length - startEntity);
									builder.Append(ParseEntity(entityRef));
									isEntity = false;

									ch = ReadChar();
									if((char)ch != '<')
									{
										AnalyzeChar(ch, false);
									}
									else
									{
										ClearNodeInfo();
										nodeType = XmlNodeType.Text;
										ungetch = ch;
										value = builder.ToString();
									}
								}
								else
								{
									AnalyzeChar(ch, false);
								}
								break;
#else
								AnalyzeChar(ch, false);
#endif
							}
							else
							{
								ClearNodeInfo();
								nodeType = XmlNodeType.Text;
								ungetch = ch;
								value = builder.ToString();
							}							
						}
						break;
					
				}// end switch(ch)
			}// end AnalyzeChar
#if !ECMA_COMPAT		
	internal bool ReadDeclaration()
			{
				int ch;
				string tempName;
				builder = new StringBuilder();

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
				
				// Skip white space in the input stream.
				SkipWhite();

				ch = ReadChar();

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
				
				readState = ReadState.Interactive;	
				// Determine what to do based on the next character.
				
			
				
				// Handling, set flag to true if first char is <
				if ((char)ch == '<')
				{
					while(ch != -1)
					{
						ReadDTDInternalSubset(ch);
						ch = ReadChar();
					}
				}			
				return false;


			}
#endif	
	
	// Read the next node in the input stream. -- This should mimic mono's interpretation of Read()
	public override bool Read()
			{
				int ch;
				string tempName;
				builder = new StringBuilder();

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
				
				// Skip white space in the input stream.
				SkipWhite();

				ch = ReadChar();

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
				
				readState = ReadState.Interactive;	
				// Determine what to do based on the next character.
				
			
				
				// Handling, set flag to true if first char is <
				if ((char)ch == '<')
				{
					AnalyzeChar(ch, true);
					tempName = Name;
					while(readAttribute == true)
					{
						ch = ReadChar();
						// get AttributeName
						AnalyzeChar(ch, false);
						ch = ReadChar();
						// get AttributeValue
						AnalyzeChar(ch, true);
						// set back to Element node type
						// for proper implementation behavior
						attributeIndex = -1;	
						nodeType = XmlNodeType.Element;
						SetName(tempName);
						value = String.Empty;
					}
					return true;
				}
				else 
				{
					AnalyzeChar(ch, false);
					tempName = Name;
					while(readAttribute == true)
					{
						ch = ReadChar();
						// get AttributeName
						AnalyzeChar(ch, false);
						ch = ReadChar();
						// get AttributeValue
						AnalyzeChar(ch, true);
						// set back to Element node type
						// for proper implementation behavior
						attributeIndex = -1;
						nodeType = XmlNodeType.Element;
						SetName(tempName);
						value = String.Empty;
					}
					return true;
				}
			
				return false;

			}

	// Reads A Document Object Node at a time.  
	internal bool ReadNode()
			{
				int ch;
				builder = new StringBuilder();

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
				
				// Skip white space in the input stream. 
				SkipWhite();

				ch = ReadChar();

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
				
				readState = ReadState.Interactive;	
				// Determine what to do based on the next character.
				
			
				
				switch (nodeType)
				{
					case XmlNodeType.Attribute:
						// Set structFlag to true so quoteChar will
						// know it is the first quote
						AnalyzeChar(ch,true);
						return true;
					default:
						// Handling, set flag to true if first char is <
						if ((char)ch == '<')
						{
							AnalyzeChar(ch, true);
						}
						else 
						{
							AnalyzeChar(ch, false);
						}
						return true;
				}
				return false;
			}

	// Read the next attribute value in the input stream.
	public override bool ReadAttributeValue()
			{
				
				if(readState !=  ReadState.Interactive)
				{
					throw new XmlException
						(S._("Xml_WrongReadState"));
				}
				
				value = attributes[attributeIndex].Value;
				nodeType = XmlNodeType.Text;
				return true;	
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
	public int ReadChars(char[] buffer, int index, int count)
			{
				if(count > buffer.Length - index)
				{
					throw new ArgumentException
						(S._("ArgumentExceedsLength"), "count");
				}
				else if (buffer == null)
				{
					throw new ArgumentNullException
						(S._("ArgumentIsNull"), "buffer");
				}
				else if ( index < 0 || count < 0 )
				{
					throw new ArgumentOutOfRangeException
						(S._("ArgumentOutOfRange"));
				}
				else if (nodeType != XmlNodeType.Text && nodeType != XmlNodeType.Element)
				{
					throw new XmlException
						(S._("Xml_WrongNodeType"));
				}
				
				int length;
				String output;
				if(nodeType != XmlNodeType.Text)
				{
					// if we are not at a text node, move to current element's content
					
					MoveToContent();
				}
				
				
				Read();

				length = value.IndexOf("<");

				// go back to '<' position
				linePosition -= value.Length - length;

				output = value.Substring(0, length);
				
				// read text now 	
				if( output.Length < count)
				{
					Array.Copy(output.ToCharArray(0, output.Length), 0, buffer, index, output.Length);
				}
				else
				{
					Array.Copy(output.ToCharArray(0, count), 0, buffer, index, count);
				}
			
				return 0;
			}

	// Read the contents of the current node, including all markup.
	public override String ReadInnerXml()
			{
				if(nodeType != XmlNodeType.Element && nodeType != XmlNodeType.Attribute)
				{
					throw new XmlException
						(S._("Xml_WrongNodeType"));
				}
				
				int ch;
				StringBuilder buffer = new StringBuilder();
				builder = new StringBuilder();
				if(nodeType == XmlNodeType.Element)
				{
					MoveToContent();
					while((ch = ReadChar()) != -1)
					{
						builder.Append((char)ch);
						if((char)ch == '<' && (char)reader.Peek() == '/')
						{
							ReadChar();
							while((ch = ReadChar()) != -1)
							{
								builder.Append((char)ch);
								if((char)ch == '>')
								{
									break;
								}
								buffer.Append((char)ch);
							}							
							if(buffer.ToString() == Name)
							{
								return builder.ToString(0,builder.Length - buffer.Length-2);
							} 
							else
							{
								// reset buffer
								buffer = new StringBuilder();
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
			
				return String.Empty;
				
			}

	// Read the current node, including all markup.
	public override String ReadOuterXml()
			{
				builder = new StringBuilder();
				StringBuilder buffer = new StringBuilder();
				MoveToContent();
				int ch;
				if(NodeType == XmlNodeType.Element)
				{
					builder.Append("<"+Name);
					if(attributes.Count >= 0)
					{
						for (int a=0; a < attributes.Count; a++)
						{
							attr = attributes[a];
							builder.Append(" " + attr.Name + "=" 
									+ QuoteChar + attr.Value + QuoteChar);
						}
					}
					builder.Append(">");
						
				}
				else if(NodeType == XmlNodeType.Attribute)
				{
					Read();
					return Name + "=" + QuoteChar + Value + QuoteChar;
				}
				while((ch = ReadChar()) != -1)
				{
					builder.Append((char)ch);
					if((char)ch == '<' && (char)reader.Peek() == '/')
					{
						builder.Append((char)ReadChar());
						while((ch = ReadChar()) != -1)
						{
							builder.Append((char)ch);
							if((char)ch == '>')
							{
								break;
							}
							buffer.Append((char)ch);
						}							
						if(buffer.ToString() == Name)
						{
							return builder.ToString();
						} 
						else
						{
							// reset buffer
							buffer = new StringBuilder();
						}
					}	
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
				if(contextSupport != false)
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
					if(attributeIndex != -1 && nodeType != XmlNodeType.Text)
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

	internal XmlParserContext ParserContext 
			{
				get
				{
					return parserContext;
				}

			}


	// Note: this is a hack to implement XmlTextReader without
	// building an XmlDocument along with it.
	// Do *not* use this elsewhere as a substitute for XmlAttribute
	// Also, the use is restricted to a minimum here as well
	internal class XmlAttributeToken : XmlAttribute
	{ 
		private String value = null;
		
		public XmlAttributeToken(XmlNameTable nt,String name,String ns):
			base(null,GetNameInfo(nt,name,null,null))
		{
		}
		
		private static NameCache.NameInfo GetNameInfo(XmlNameTable nt,
				String localName,
				String prefix,
				String ns)
		{
			string name;

			if(localName == null)
			{
				localName = String.Empty;
			}
			else
			{
				localName = nt.Add(localName);
			}
			if(prefix == null)
			{
				prefix = String.Empty;
			}
			else
			{
				prefix = nt.Add(prefix);
			}
			if(ns == null)
			{
				prefix = String.Empty;
			}
			else
			{
				ns=nt.Add(ns);
			}
			if(prefix.Length > 0)
			{
				name = nt.Add(prefix + ":" + localName);
			}
			else
			{
				name = localName;
			}
			return new 
				NameCache.NameInfo(localName, prefix, name, ns, null);
		}
		public override String Value
		{
			get
			{
				return this.value;
			}
			set
			{
				this.value = value;
			}
		}
				
	}

}; // class XmlTextReader

}; // namespace System.Xml
