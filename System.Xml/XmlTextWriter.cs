/*
 * XmlTextWriter.cs - Implementation of the "System.Xml.XmlTextWriter" class.
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
using System.Globalization;

public class XmlTextWriter : XmlWriter
{
	// Internal state.
	internal TextWriter writer;
	private System.Xml.Formatting formatting;
	private int indentation;
	private char indentChar;
	private char[] indentChars;
	private bool namespaces;
	private char quoteChar;
	private System.Xml.WriteState writeState;
	private XmlNameTable nameTable;
	private XmlNamespaceManager namespaceManager;
	private ElementScope scope;
	private int indentLevel;
	private XmlSpace xmlSpace;
	private String xmlLang;
	private int pseudoNSNumber;
	internal bool autoShiftToContent;
	private enum Special
	{
		None  = 0,
		Lang  = 1,
		Space = 2
	}
	private Special special;

	// Constructors.
	public XmlTextWriter(Stream w, Encoding encoding)
			{
				writer = new StreamWriter
					(w, ((encoding != null) ? encoding : Encoding.UTF8));
				Initialize();
			}
	public XmlTextWriter(String filename, Encoding encoding)
			{
				writer = new StreamWriter
					(new FileStream(filename, FileMode.Create, FileAccess.Write),
					 ((encoding != null) ? encoding : Encoding.UTF8));
				Initialize();
			}
	public XmlTextWriter(TextWriter w)
			{
				if(w == null)
				{
					throw new ArgumentNullException("w");
				}
				writer = w;
				Initialize();
			}

	// Initialize the common writer fields.
	private void Initialize()
			{
				formatting = System.Xml.Formatting.None;
				indentation = 2;
				indentChar = ' ';
				namespaces = true;
				quoteChar = '"';
				writeState = System.Xml.WriteState.Start;
				nameTable = new NameTable();
				namespaceManager = new XmlNamespaceManager(nameTable);
				scope = null;
				indentLevel = 0;
				xmlSpace = XmlSpace.None;
				xmlLang = null;
				pseudoNSNumber = 1;
				special = Special.None;
				autoShiftToContent = false;
				indentChars = new char[indentation];
				for (int i = 0; i < indentation; i++)
				{
					indentChars[i] = indentChar;
				}
			}

	// Push a new element scope.
	private void PushScope(String prefix, String localName, bool scopeShown, String xmlns)
			{
				scope = new ElementScope(scope);
				scope.prefix = (prefix != null ? nameTable.Add(prefix) : null);
				scope.localName =
					(localName != null ? nameTable.Add(localName) : null);
				scope.xmlns = (xmlns != null ? nameTable.Add(xmlns) : null);
				scope.scopeShown = scopeShown;
				scope.indentLevel = indentLevel;
				if(formatting == System.Xml.Formatting.Indented)
				{
					indentLevel++;
				}
				scope.indent = false;
				scope.xmlSpace = xmlSpace;
				scope.xmlLang = xmlLang;
				namespaceManager.PushScope();
			}

	// Pop the current element scope.
	private void PopScope()
			{
				indentLevel = scope.indentLevel;
				xmlSpace = scope.xmlSpace;
				xmlLang = scope.xmlLang;
				scope = scope.next;
				namespaceManager.PopScope();
			}

	// Add indentation before the current element.
	private void DoIndent(bool useScope)
			{
				if(xmlSpace != System.Xml.XmlSpace.Preserve)
				{
					if(formatting == System.Xml.Formatting.Indented)
					{
						int indent = useScope ? scope.indentLevel : indentLevel;
						writer.Write(Environment.NewLine);
						while(indent > 0)
						{
							writer.Write(indentChars);
							--indent;
						}
						// flag that perent element needs DoIndent on WriteEndElement
						if (!useScope && (scope != null))
						{
							scope.indent = true;
						}
					}
				}
			}

	// State flags for "Sync".
	[Flags]
	private enum WriteStateFlag
	{
		StartFlag     = (1<<0),
		PrologFlag    = (1<<1),
		ElementFlag   = (1<<2),
		AttributeFlag = (1<<3),
		ContentFlag   = (1<<4),
		ClosedFlag    = (1<<5)

	}; // enum WriteStateFlag

	// Synchronize the output with a particular document area.
	private void Sync(WriteStateFlag flags, bool indent)
			{
				System.Xml.WriteState writeStateOnCalling = writeState;

				// Determine if the current write state is compatible
				// with the synchronisation flags, and shift to the
				// requested state if necessary.
				switch(writeState)
				{
					case WriteState.Element:
					{
						if((flags & WriteStateFlag.ContentFlag) != 0)
						{
							writer.Write(">");
							writeState = System.Xml.WriteState.Content;
						}
						else
						{
							throw new InvalidOperationException
								(S._("Xml_InvalidWriteState"));
						}
					}
					break;

					case WriteState.Attribute:
					{
						if((flags & WriteStateFlag.AttributeFlag) != 0)
						{
							// We can write directly to the attribute.
						}
						else if((flags & WriteStateFlag.ContentFlag) != 0)
						{
							// Terminate the attribute and switch to contents.
							writer.Write(quoteChar);
							writer.Write(">");
							writeState = System.Xml.WriteState.Content;
						}
						else
						{
							throw new InvalidOperationException
								(S._("Xml_InvalidWriteState"));
						}
					}
					break;

					case WriteState.Content:
					{
						if((flags & WriteStateFlag.ContentFlag) == 0)
						{
							throw new InvalidOperationException
								(S._("Xml_InvalidWriteState"));
						}
					}
					break;

					case WriteState.Start:
					{
						// Automatically shift to the content mode
						// if we are outputting a document fragment
						// from "XmlNode.InnerXml" or "XmlNode.OuterXml".
						if(autoShiftToContent)
						{
							if((flags & WriteStateFlag.ContentFlag) == 0)
							{
								throw new InvalidOperationException
									(S._("Xml_InvalidWriteState"));
							}
							writeState = WriteState.Content;
						}
						else
						{
							if(((1 << (int)writeState) & (int)flags) == 0)
							{
								throw new InvalidOperationException
									(S._("Xml_InvalidWriteState"));
							}
							writeState = System.Xml.WriteState.Prolog;
						}
					}
					break;

					case WriteState.Prolog:
					case WriteState.Closed:
					{
						if(((1 << (int)writeState) & (int)flags) == 0)
						{
							throw new InvalidOperationException
								(S._("Xml_InvalidWriteState"));
						}
					}
					break;
				}
				if(indent) // do indent if possible
				{
					bool needIndent;

					if(scope != null)
					{
						needIndent = scope.indent;
					}
					else
					{
						needIndent = false;
					}
					if((writeStateOnCalling == System.Xml.WriteState.Prolog) ||
						(writeStateOnCalling == System.Xml.WriteState.Element) ||
						(writeStateOnCalling == System.Xml.WriteState.Attribute) ||
						needIndent)
					{
						DoIndent(false);
					}
				}
			}


	private void WriteRawData(String value)
	{
		writer.Write(value);
	}

	// Write an xml declaration.
	private void WriteXmlDeclaration(String text)
			{
				// make sure we're in the start state
				if(writeState != System.Xml.WriteState.Start)
				{
					throw new InvalidOperationException
						(S._("Xml_InvalidWriteState"));
				}

				// create the reader
				XmlTextReader r = new XmlTextReader
					(new StringReader("<?xml "+text+"?>"), nameTable);

				// read the value
				r.Read();

				// read the version attribute
				if(r["version"] != "1.0")
				{
					throw new ArgumentException
						(S._("Xml_InvalidVersion"), "text");
				}

			#if !ECMA_COMPAT
				// make sure the encoding matches
				String encoding = r["encoding"];
				if(encoding != null && encoding.Length > 0 &&
				   writer.Encoding.WebName != encoding)
				{
					throw new ArgumentException
						(S._("Xml_InvalidEncoding"), "text");
				}
			#endif

				// make sure the standalone is valid
				String standalone = r["standalone"];
				bool haveStandalone = false;
				if(standalone != null && standalone.Length > 0)
				{
					if(standalone != "yes" && standalone != "no")
					{
						throw new ArgumentException
							(S._("Xml_InvalidEncoding"), "text");
					}
					haveStandalone = true;
				}

				// write the start of the document
				if(haveStandalone)
				{
					WriteStartDocument(standalone == "yes");
				}
				else
				{
					WriteStartDocument();
				}
			}

	// Close this writer and free all resources.
	public override void Close()
			{
				if(writeState != System.Xml.WriteState.Closed)
				{
					// Close all open element scopes.
					if(writeState == System.Xml.WriteState.Attribute)
					{
						// Terminate the attribute and the element start.
						writer.Write(quoteChar);
						writer.Write(" />");
						if(scope != null)
						{
							PopScope();
						}
					}
					else if(writeState == System.Xml.WriteState.Element)
					{
						// Terminate the element start.  We may need
						// to ignore this if writing in "auto-shift"
						// mode as we may have written a bare attribute.
						if(scope != null || !autoShiftToContent)
						{
							writer.Write(" />");
							PopScope();
						}
					}
					while(scope != null)
					{
						if (scope.indent)
						{
							DoIndent(true);
						}
						writer.Write("</");
						writer.Write(scope.localName);
						writer.Write('>');
						PopScope();
					}

					// write last newline
					DoIndent(false);
					// Flush and close the TextWriter stream.
					Flush();
					writer.Close();
					writeState = System.Xml.WriteState.Closed;
				}
			}

	// Flush the buffers that are used by this writer.
	public override void Flush()
			{
				writer.Flush();
			}

	// Look up the namespace URI for a specified namespace prefix.
	public override String LookupPrefix(String ns)
			{
				if(((Object)ns) == null || ns.Length == 0)
				{
					throw new ArgumentNullException("ns");
				}
				String prefix = namespaceManager.LookupPrefix(ns);
				return (prefix == String.Empty) ? null : prefix;
			}

	// Encode an array as base64 and write it out as text.
	public override void WriteBase64(byte[] buffer, int index, int count)
			{
			
				// Validate the parameters.
				if(buffer == null)
				{
					throw new ArgumentNullException("buffer");
				}
				else if(index < 0)
				{
					throw new ArgumentOutOfRangeException
						("index", S._("ArgRange_Array"));
				}
				else if(count < 0 || (buffer.Length - index) < count)
				{
					throw new ArgumentOutOfRangeException
						("count", S._("ArgRange_Array"));
				}
				else if(writeState == WriteState.Closed)
				{
					throw new InvalidOperationException
						(S._("Xml_InvalidWriteState"));
				}
		
				// Synchronize to the content or attribute area.
				Sync(WriteStateFlag.ContentFlag |
					 WriteStateFlag.AttributeFlag, false);

				WriteRawData(XmlConvert.ToBase64String(buffer, index, count));
			}
	
	// Encode an array as BinHex and write it out as text.
	public override void WriteBinHex(byte[] buffer, int index, int count)
			{
				// Validate the parameters.
				if(buffer == null)
				{
					throw new ArgumentNullException("buffer");
				}
				else if(index < 0)
				{
					throw new ArgumentOutOfRangeException
						("index", S._("ArgRange_Array"));
				}
				else if(count < 0)
				{
					throw new ArgumentOutOfRangeException
						("count", S._("ArgRange_Array"));
				}
				else if(buffer.Length - index < count)
				{
					throw new ArgumentException
						("index", S._("ArgRange_Array"));
				}
				else if(writeState == WriteState.Closed)
				{
					throw new InvalidOperationException
						(S._("Xml_InvalidWriteState"));
				}

				// Synchronize to the content or attribute area.
				Sync(WriteStateFlag.ContentFlag |
					 WriteStateFlag.AttributeFlag, false);

				WriteRawData(XmlConvert.ToHexString(buffer, index, count));	
			}

	// Write out a CDATA block.
	public override void WriteCData(String text)
			{
				if(text != null && text.IndexOf("]]>") != -1)
				{
					throw new ArgumentException
						(S._("Xml_InvalidXmlWritten"), "text");
				}
				Sync(WriteStateFlag.ContentFlag, false);
				writer.Write("<![CDATA[");

				if(text != null)
				{
					writer.Write(text);
				}
				writer.Write("]]>");
			}

	// Write a character entity.
	public override void WriteCharEntity(char ch)
			{
				Sync(WriteStateFlag.ContentFlag |
					 WriteStateFlag.AttributeFlag, false);
				writer.Write("&#x{0:X2};", (int)ch);
			}

	// Write a text buffer.
	public override void WriteChars(char[] buffer, int index, int count)
			{
				// Validate the parameters.
				if(buffer == null)
				{
					throw new ArgumentNullException("buffer");
				}
				else if(index < 0)
				{
					throw new ArgumentOutOfRangeException
						("index", S._("ArgRange_Array"));
				}
				else if(count < 0 || (buffer.Length - index) < count)
				{
					throw new ArgumentOutOfRangeException
						("count", S._("ArgRange_Array"));
				}

				// Synchronize to the content or attribute area.
				Sync(WriteStateFlag.ContentFlag |
					 WriteStateFlag.AttributeFlag, false);

				// The buffer must not end in a low surrogate.
				if(count > 0 &&
				   buffer[index + count - 1] >= 0xD800 &&
				   buffer[index + count - 1] <= 0xDBFF)
				{
					throw new ArgumentException
						(S._("Xml_InvalidSurrogate"), "buffer");
				}

				// Quote the character buffer and output it.
				int posn, prev;
				char ch;
				prev = 0;
				for(posn = 0; posn < count; ++posn)
				{
					ch = buffer[index + posn];
					switch(ch)
					{
						case '\x09': case '\x0A': case '\x0D': break;

						case '<':
						{
							if(prev < posn)
							{
								writer.Write
									(buffer, index + prev, posn - prev);
							}
							writer.Write("&lt;");
							prev = posn + 1;
						}
						break;

						case '>':
						{
							if(prev < posn)
							{
								writer.Write
									(buffer, index + prev, posn - prev);
							}
							writer.Write("&gt;");
							prev = posn + 1;
						}
						break;

						case '&':
						{
							if(prev < posn)
							{
								writer.Write
									(buffer, index + prev, posn - prev);
							}
							writer.Write("&amp;");
							prev = posn + 1;
						}
						break;

						case '"':
						{
							if(writeState != System.Xml.WriteState.Attribute)
							{
								break;
							}
							if(prev < posn)
							{
								writer.Write
									(buffer, index + prev, posn - prev);
							}
							writer.Write("&quot;");
							prev = posn + 1;
						}
						break;

						case '\'':
						{
							if(writeState != System.Xml.WriteState.Attribute)
							{
								break;
							}
							if(prev < posn)
							{
								writer.Write
									(buffer, index + prev, posn - prev);
							}
							writer.Write("&apos;");
							prev = posn + 1;
						}
						break;

						default:
						{
							if(ch < 0x20)
							{
								// Quote a low-ASCII control character.
								if(prev < posn)
								{
									writer.Write
										(buffer, index + prev, posn - prev);
								}
								prev = posn + 1;
								writer.Write("&#");
								writer.Write
									(((int)ch).ToString
									  (null, NumberFormatInfo.InvariantInfo));
								writer.Write(';');
							}
						}
						break;
					}
				}
				if(prev < count)
				{
					writer.Write(buffer, index + prev, posn - prev);
				}
			}

	// Write a comment.
	public override void WriteComment(String text)
			{
				// Bail out if the comment text contains "--".
				if (text !=  null && text.Length > 0)
				{
					if(text.IndexOf("--") != -1)
					{
						throw new ArgumentException
							(S._("Xml_InvalidXmlWritten"), "text");
					}
				}
				// Synchronize to an area that allows comments.
				Sync(WriteStateFlag.StartFlag |
				     WriteStateFlag.ContentFlag |
					 WriteStateFlag.PrologFlag, true);

				// Write out the comment.
				writer.Write("<!--");
				if(text != null)
				{
					writer.Write(text);
				}
				writer.Write("-->");
			}

	// Write a document type specification.
	public override void WriteDocType(String name, String pubid,
									  String sysid, String subset)
			{
				// We must be in the prolog.
				if(autoShiftToContent)
				{
					if(writeState == System.Xml.WriteState.Closed)
					{
						throw new InvalidOperationException
							(S._("Xml_InvalidWriteState"));
					}
				}
				else
				{
					if(writeState != System.Xml.WriteState.Prolog)
					{
						throw new InvalidOperationException
							(S._("Xml_InvalidWriteState"));
					}
				}

				// Validate the name.
				if(!XmlReader.IsName(name))
				{
					throw new ArgumentException
						(S._("Xml_InvalidName"), "name");
				}

				// Write out the document type information.
				DoIndent(false);
				writer.Write("<!DOCTYPE ");
				writer.Write(name);
				writeState = System.Xml.WriteState.Attribute;
				if(pubid != null)
				{
					writer.Write(" PUBLIC ");
					writer.Write(quoteChar);
					WriteQuotedString(pubid);
					writer.Write(quoteChar);
					writer.Write(' ');
					writer.Write(quoteChar);
					WriteQuotedString(sysid);
					writer.Write(quoteChar);
				}
				else if(sysid != null)
				{
					writer.Write(" SYSTEM ");
					writer.Write(quoteChar);
					WriteQuotedString(sysid);
					writer.Write(quoteChar);
				}
				if(subset != null)
				{
					writer.Write(' ');
					writer.Write('[');
					writer.Write(subset);
					writer.Write(']');
				}
				writer.Write('>');

				// Return to the prolog.
				writeState = System.Xml.WriteState.Prolog;
			}

	// Write the end of an attribute.
	public override void WriteEndAttribute()
			{
				// We must be in the Attribute state.
				if(writeState != System.Xml.WriteState.Attribute)
				{
					throw new InvalidOperationException
						(S._("Xml_InvalidWriteState"));
				}

				// Terminate the attribute and return to the element state.
				writer.Write(quoteChar);
				writeState = System.Xml.WriteState.Element;
				special = Special.None;
			}

	// Write the document end information and reset to the start state.
	public override void WriteEndDocument()
			{
				if(writeState == System.Xml.WriteState.Start ||
				   writeState == System.Xml.WriteState.Prolog ||
				   writeState == System.Xml.WriteState.Closed)
				{
					throw new ArgumentException
						(S._("Xml_InvalidWriteState"), "WriteState");
				}
				if(writeState == System.Xml.WriteState.Attribute)
				{
					// Terminate the attribute and the element start.
					writer.Write(quoteChar);
					writer.Write(" />");
					PopScope();
				}
				else if(writeState == System.Xml.WriteState.Element)
				{
					// Terminate the element start.
					writer.Write(" />");
					PopScope();
				}
				while(scope != null)
				{
					if (scope.indent)
					{
						DoIndent(true);
					}
					writer.Write("</");
					writer.Write(scope.localName);
					writer.Write('>');
					PopScope();
				}
				writeState = System.Xml.WriteState.Start;
			}

	// Write the end of an element and pop the namespace scope.
	public override void WriteEndElement()
			{
				// We must be in the Element or Content state.
				if(writeState == System.Xml.WriteState.Element)
				{
					// Terminate the element with " />".
					writer.Write(" />");
				}
				else if(writeState == System.Xml.WriteState.Content)
				{
					// Terminate the element with a full end tag.
					if(scope.indent)
					{
						DoIndent(true);
					}
					writer.Write("</");
					if(scope.scopeShown && scope.prefix != null && scope.prefix != String.Empty)
					{
						writer.Write(scope.prefix);
						writer.Write(':');
					}
					writer.Write(scope.localName);
					writer.Write(">");
				}
				else
				{
					throw new InvalidOperationException
						(S._("Xml_InvalidWriteState"));
				}

				// Pop the current scope.
				PopScope();
				writeState = System.Xml.WriteState.Content;
			}

	// Write an entity reference.
	public override void WriteEntityRef(String name)
			{
				if(!XmlReader.IsNameToken(name))
				{
					throw new ArgumentException
						(S._("Xml_InvalidEntityRef"), "name");
				}
				Sync(WriteStateFlag.ContentFlag |
					 WriteStateFlag.AttributeFlag, false);
				writer.Write("&{0};", name);
			}

	// Write a full end element tag, even if there is no content.
	public override void WriteFullEndElement()
			{
				// We must be in the Element or Content state.
				if(writeState == System.Xml.WriteState.Element)
				{
					// Terminate the start element.
					writer.Write('>');
				}
				if(writeState == System.Xml.WriteState.Element ||
				   writeState == System.Xml.WriteState.Content)
				{
					// Terminate the element with a full end tag.
					if(scope.indent)
					{
						DoIndent(true);
					}
					writer.Write("</");
					if(scope.scopeShown && scope.prefix != null && scope.prefix != String.Empty)
					{
						writer.Write(scope.prefix);
						writer.Write(':');
					}
					writer.Write(scope.localName);
					writer.Write(">");
				}
				else
				{
					throw new InvalidOperationException
						(S._("Xml_InvalidWriteState"));
				}

				// Pop the current scope.
				PopScope();
				writeState = System.Xml.WriteState.Content;
			}

	// Write a name, as long as it is XML-compliant.
	public override void WriteName(String name)
			{
				if(!XmlReader.IsName(name))
				{
					throw new ArgumentException
						(S._("Xml_InvalidName"), "name");
				}
				Sync(WriteStateFlag.ContentFlag |
					 WriteStateFlag.AttributeFlag, false);
				writer.Write(name);
			}

	// Write a name token, as long as it is XML-compliant.
	public override void WriteNmToken(String name)
			{
				if(!XmlReader.IsNameToken(name))
				{
					throw new ArgumentException
						(S._("Xml_InvalidName"), "name");
				}
				Sync(WriteStateFlag.ContentFlag |
					 WriteStateFlag.AttributeFlag, false);
				writer.Write(name);
			}

	// Write a processing instruction. <?name text?>
	public override void WriteProcessingInstruction(String name, String text)
			{
				// Make sure we have a name.
				if(name == null || name.Length == 0)
				{
					throw new ArgumentException
						(S._("Xml_ArgumentException"), "name");
				}

				// validate and write the xml declaration value
				if(name == "xml")
				{
					WriteXmlDeclaration(text);
					return;
				}

				// Make sure we have valid processing instruction text.
				if(text != null && text.IndexOf("?>") != -1)
				{
					throw new ArgumentException
						(S._("Xml_InvalidXmlWritten"), "text");
				}

				// Synchronize to an area that allows processing instructions.
				Sync(WriteStateFlag.StartFlag |
				     WriteStateFlag.PrologFlag |
				     WriteStateFlag.ContentFlag, true);

				// Write out the processing instruction.
				if(text != null && text.Length != 0)
				{
					writer.Write("<?{0} {1}?>", name, text);
				}
				else
				{
					writer.Write("<?{0}?>",name);
				}
			}

	// Write a qualified name.
	public override void WriteQualifiedName(String localName, String ns)
			{
				if ((localName == null) || (localName == String.Empty))
				{
					throw new ArgumentException
						(S._("Xml_ArgumentException"), "localName");
				}
				
				if  ((Namespaces == false) && (ns != null) 
						&& (ns != String.Empty))
				{
					throw new ArgumentException
						(S._("Xml_ArgumentException"), "ns");
				}
				
				if(!XmlReader.IsName(localName))
				{
					throw new XmlException
						(S._("Xml_XmlException"));
				}

							
				if (writeState == System.Xml.WriteState.Closed)
				{
					throw new InvalidOperationException
						(S._("Xml_InvalidOperation"));
				}

				if (writeState == System.Xml.WriteState.Closed)
				{
						throw new InvalidOperationException
							(S._("Xml_InvalidOperation"));
				}

				if ((writeState == System.Xml.WriteState.Element) || (writeState == System.Xml.WriteState.Attribute))
				{

						if ((ns == null) && (writeState == System.Xml.WriteState.Element))
						{
							throw new ArgumentException
								(S._("Xml_ArgumentException"), "ns");
						}
						
						if (Namespaces == false) 
						{
							if ((ns == null) && (writeState == System.Xml.WriteState.Attribute))
							{
								writer.Write(localName);
							}
						} 
						else if (Namespaces == true) 
						{
							
							try 
							{
								Uri uri = new System.Uri(ns);
								String prefix = LookupPrefix(ns);

								if((Object)prefix == null && prefix == String.Empty)
								{
									throw new ArgumentException
										(S._("Xml_PrefixNotFound"), "ns");
								}
								else
								{

									if (writeState == System.Xml.WriteState.Attribute)
									{
										if (prefix != scope.prefix && prefix != String.Empty)
										{
											writer.Write("{0}:{1}", prefix, localName);
										}
										else
										{
											writer.Write("{0}", localName);
										}
									} 
									else if (writeState == System.Xml.WriteState.Element)
									{
										if((Object)ns == null && ns == String.Empty)
										{
											throw new ArgumentException
												(S._("Xml_NamespaceValueNull"));
										}
										else if (prefix != scope.prefix && prefix != String.Empty)
										{
											writer.Write("{0}:{1}", prefix, localName);
										}
										else if (prefix == scope.prefix)
										{
											writer.Write("{0}", localName);
										}
									}
								}	
							}	
							catch (UriFormatException)
							{
								throw new ArgumentException
									(S._("Xml_InvalidUriFormat"), "ns");
							}
						}
						
				}					
			}

	// Write raw string data.
	public override void WriteRaw(String data)
			{
				if(writeState == System.Xml.WriteState.Closed)
				{
					throw new InvalidOperationException
						(S._("Xml_WriteStateClosed"));
				}
				else
				{
					char[] buffer = data.ToCharArray();
					WriteChars(buffer, 0 , buffer.Length);
				}
			}

	// Write raw data from an array.
	public override void WriteRaw(char[] buffer, int index, int count)
			{
				// Validate the parameters.
				if(buffer == null)
				{
					throw new ArgumentNullException("buffer");
				}
				else if(index < 0)
				{
					throw new ArgumentOutOfRangeException
						("index", S._("ArgRange_Array"));
				}
				else if(count < 0 || (buffer.Length - index) < count)
				{
					throw new ArgumentOutOfRangeException
						("count", S._("ArgRange_Array"));
				}
				else if(writeState == System.Xml.WriteState.Closed)
				{
					throw new InvalidOperationException
						(S._("Xml_InvalidWriteState"));
				}
				else 
				{
					WriteChars(buffer, index, count);
				}
			}

	// Write the start of an attribute with a full name.
	public override void WriteStartAttribute(String prefix, String localName,
										     String ns)
			{
				// Validate the parameters.
				if(!namespaces && (prefix != null || ns != null))
				{
					throw new ArgumentException
						(S._("Xml_NamespacesNotSupported"));
				}

				// Check the state and output delimiters.
				if(writeState == System.Xml.WriteState.Attribute)
				{
					writer.Write(quoteChar);
					writer.Write(' ');
				}
				else if(writeState == System.Xml.WriteState.Element)
				{
					writer.Write(' ');
				}
				else if(writeState == System.Xml.WriteState.Closed)
				{
					throw new InvalidOperationException
						(S._("Xml_InvalidWriteState"));
				}

				// Output the name of the attribute, with appropriate prefixes.
				if(((Object)prefix) != null && prefix != String.Empty &&
				   ((Object)ns) != null && ns != String.Empty)
				{
					// We need to associate a prefix with a namespace.
					String currMapping = LookupPrefix(ns);
					if(currMapping == prefix)
					{
						// The prefix is already mapped to this URI.
						if(prefix != scope.prefix)
						{
							writer.Write(prefix);
							writer.Write(':');
						}
					}
					else
					{
						writer.Write("xmlns");
						writer.Write('=');
						writer.Write(quoteChar);
						WriteQuotedString(ns);
						writer.Write(quoteChar);
						writer.Write(' ');
					}
				}
				else if(((Object)prefix) != null && prefix != String.Empty)
				{
					// We were only given a prefix, so output it directly.
					if(prefix != scope.prefix)
					{
						writer.Write(prefix);
						writer.Write(':');
					}
				}
				else if(((Object)ns) != null && ns != String.Empty)
				{
					// We were only given a namespace, so find the prefix.
					prefix = LookupPrefix(ns);
					if(((Object)prefix) == null || prefix.Length == 0)
					{
						writer.Write("xmlns");
						writer.Write('=');
						writer.Write(quoteChar);
						WriteQuotedString(ns);
						writer.Write(quoteChar);
						writer.Write(' ');
					}
				}
				writer.Write(localName);

				// Output the start of the attribute value.
				writer.Write('=');
				writer.Write(quoteChar);

				// We are now in the attribute state.
				writeState = System.Xml.WriteState.Attribute;

				// Recognise special attributes.
				if(prefix == "xml")
				{
					if(localName == "lang")
					{
						special = Special.Lang;
					}
					else if(localName == "space")
					{
						special = Special.Space;
					}
					else
					{
						special = Special.None;
					}
				}
				else
				{
					special = Special.None;
				}
			}

	// this is sorta a non-spec hack to get namespaces declared from within attributes
	// to be added to the namespaceManager
	internal override void WriteStartAttribute(String prefix, String localName,
										     String ns , String value, bool flagNS)
			{
				// heres the hack
				if((Object)value != null && value != String.Empty && flagNS == true)
				{
						// add the namespace and prefix to namespaceManager
						namespaceManager.AddNamespace(localName, value);
				}
				
				// Validate the parameters.
				if(!namespaces && (prefix != null || ns != null))
				{
					throw new ArgumentException
						(S._("Xml_NamespacesNotSupported"));
				}

				// Check the state and output delimiters.
				if(writeState == System.Xml.WriteState.Attribute)
				{
					writer.Write(quoteChar);
					writer.Write(' ');
				}
				else if(writeState == System.Xml.WriteState.Element)
				{
					writer.Write(' ');
				}
				else if(writeState == System.Xml.WriteState.Closed)
				{
					throw new InvalidOperationException
						(S._("Xml_InvalidWriteState"));
				}

				// Output the name of the attribute, with appropriate prefixes.
				if(((Object)prefix) != null && prefix != String.Empty &&
				   ((Object)ns) != null && ns != String.Empty)
				{
					// We need to associate a prefix with a namespace.
					String currMapping = LookupPrefix(ns);
					if(currMapping == prefix)
					{
						// The prefix is already mapped to this URI.
						if(prefix != scope.prefix)
						{
							writer.Write(prefix);
							writer.Write(':');
						}
					}
					else
					{
						// Create a new pseudo-prefix for the URI.
						writer.Write("xmlns");
						writer.Write('=');
						writer.Write(quoteChar);
						WriteQuotedString(ns);
						writer.Write(quoteChar);
						writer.Write(' ');
					}
				}
				else if(((Object)prefix) != null && prefix != String.Empty)
				{
					// We were only given a prefix, so output it directly.
					if(prefix != scope.prefix)
					{
						writer.Write(prefix);
						writer.Write(':');
					}
				}
				else if(((Object)ns) != null && ns != String.Empty)
				{
					// We were only given a namespace, so find the prefix.
					prefix = LookupPrefix(ns);
					if(((Object)prefix) == null || prefix.Length == 0)
					{
						// Create a new pseudo-prefix for the URI.
						writer.Write("xmlns");
						writer.Write('=');
						writer.Write(quoteChar);
						WriteQuotedString(ns);
						writer.Write(quoteChar);
						writer.Write(' ');
						writer.Write(prefix);
						writer.Write(':');
					}
				}
				writer.Write(localName);

				// Output the start of the attribute value.
				writer.Write('=');
				writer.Write(quoteChar);

				// We are now in the attribute state.
				writeState = System.Xml.WriteState.Attribute;

				// Recognise special attributes.
				if(prefix == "xml")
				{
					if(localName == "lang")
					{
						special = Special.Lang;
					}
					else if(localName == "space")
					{
						special = Special.Space;
					}
					else
					{
						special = Special.None;
					}
				}
				else
				{
					special = Special.None;
				}

			}

	
	// Write the start of an XML document.
	public override void WriteStartDocument(bool standalone)
			{
				if(writeState != System.Xml.WriteState.Start)
				{
					throw new InvalidOperationException
						(S._("Xml_InvalidWriteState"));
				}
			#if !ECMA_COMPAT
				writer.Write
					("<?xml version=\"1.0\" encoding=\"{0}\" " +
					 "standalone=\"{1}\"?>",
					 writer.Encoding.WebName,
					 (standalone ? "yes" : "no"));
			#else
				writer.Write
					("<?xml version=\"1.0\" standalone=\"{0}\"?>",
					 (standalone ? "yes" : "no"));
			#endif
				writeState = System.Xml.WriteState.Prolog;
			}

	// Write the start of an XML document with no standalone attribute.
	public override void WriteStartDocument()
			{
				if(writeState != System.Xml.WriteState.Start)
				{
					throw new InvalidOperationException
						(S._("Xml_InvalidWriteState"));
				}
			#if !ECMA_COMPAT
				writer.Write("<?xml version=\"1.0\" encoding=\"{0}\"?>",
					 			 writer.Encoding.WebName);
			#else
				writer.Write("<?xml version=\"1.0\"?>");
			#endif
				writeState = System.Xml.WriteState.Prolog;
			}

	// Write the start of an element with a full name.
	public override void WriteStartElement(String prefix, String localName,
										   String ns)
			{
				// Validate the parameters.
				if(!namespaces && (prefix != null || ns != null))
				{
					throw new ArgumentException
						(S._("Xml_NamespacesNotSupported"));
				}

				// We need to be in the Element or Content state.
				Sync(WriteStateFlag.ElementFlag |
					 WriteStateFlag.AttributeFlag |
					 WriteStateFlag.ContentFlag |
					 WriteStateFlag.StartFlag |
					 WriteStateFlag.PrologFlag, true);

				// Get the current scope prefix.
				String currPrefix;
				if(scope != null)
				{
					currPrefix = scope.prefix;
				}
				else
				{
					currPrefix = null;
				}

				// Output the name of the element, with appropriate prefixes.
				bool scopeShown = false;
				writer.Write('<');
				if(((Object)prefix) != null && prefix != String.Empty &&
				   ((Object)ns) != null && ns != String.Empty)
				{
					// We need to associate a prefix with a namespace.
					String currMapping = LookupPrefix(ns);
					if(currMapping == prefix)
					{
						// The prefix is already mapped to this URI.
						if(prefix != currPrefix)
						{
							writer.Write(prefix);
							writer.Write(':');
							scopeShown = true;
						}
						writer.Write(localName);
					}
					else
					{
						// Create a new pseudo-prefix for the URI.
						writer.Write(localName);
						writer.Write(' ');
						writer.Write("xmlns");
						writer.Write('=');
						writer.Write(quoteChar);
						WriteQuotedString(ns);
						writer.Write(quoteChar);
						scopeShown = true;
					}
				}
				else if(((Object)prefix) != null && prefix != String.Empty)
				{
					// We were only given a prefix, so output it directly.
					if(prefix != currPrefix)
					{
						writer.Write(prefix);
						writer.Write(':');
						scopeShown = true;
					}
					writer.Write(localName);
				}
				else if(((Object)ns) != null && ns != String.Empty)
				{
					// We were only given a namespace, so find the prefix.
					prefix = LookupPrefix(ns);
					if((((Object)prefix) == null || prefix.Length == 0) && scope == null)
					{
						writer.Write(localName);
						writer.Write(' ');
						writer.Write("xmlns");
						writer.Write('=');
						writer.Write(quoteChar);
						WriteQuotedString(ns);
						writer.Write(quoteChar);
						scopeShown = true;
					}
					else if(((Object)prefix) == null || prefix.Length == 0)
					{
						if(scope.scopeShown == false || scope.xmlns != ns )
						{
							writer.Write(localName);
							writer.Write(' ');	
							writer.Write("xmlns");
							writer.Write('=');
							writer.Write(quoteChar);
							WriteQuotedString(ns);
							writer.Write(quoteChar);
							scopeShown = true;
						}
						else if(scope.xmlns == ns)
						{
							writer.Write(localName);
							scopeShown = true;
						}
	
					}
					else if(prefix != currPrefix)
					{
						// We know the prefix, but need to specify it.
						writer.Write(prefix);
						writer.Write(':');
						writer.Write(localName);
						scopeShown = true;
					}
					else
					{
						// We have the same prefix as our parent element.
						writer.Write(localName);
					}
				}
				else
				{
					// We only have a name.
					writer.Write(localName);
				}

				// Push a new scope record.
				PushScope(prefix, localName, scopeShown, ns);
				if(scopeShown)
				{
					if(prefix != String.Empty && prefix != null)
						namespaceManager.AddNamespace(prefix, ns);
				}

				// We are now in the element state.
				writeState = System.Xml.WriteState.Element;
			}

	// Write a quoted string.
	private void WriteQuotedString(String text)
			{
				int posn, prev, len;
				char ch;
				prev = 0;
				len = text.Length;
				for(posn = 0; posn < len; ++posn)
				{
					ch = text[posn];
					switch(ch)
					{
						case '\x09': case '\x0A': case '\x0D': break;

						case '<':
						{
							if(prev < posn)
							{
								writer.Write(text.Substring(prev, posn - prev));
							}
							writer.Write("&lt;");
							prev = posn + 1;
						}
						break;

						case '>':
						{
							if(prev < posn)
							{
								writer.Write(text.Substring(prev, posn - prev));
							}
							writer.Write("&gt;");
							prev = posn + 1;
						}
						break;

						case '&':
						{
							if(prev < posn)
							{
								writer.Write(text.Substring(prev, posn - prev));
							}
							writer.Write("&amp;");
							prev = posn + 1;
						}
						break;

						case '"':
						{
							if(writeState != System.Xml.WriteState.Attribute)
							{
								break;
							}
							if(quoteChar == '\'')
							{
								// no need to escape this
								break;
							}
							if(prev < posn)
							{
								writer.Write(text.Substring(prev, posn - prev));
							}
							writer.Write("&quot;");
							prev = posn + 1;
						}
						break;

						case '\'':
						{
							if(writeState != System.Xml.WriteState.Attribute)
							{
								break;
							}
							if(quoteChar == '"')
							{
								// no need to escape this
								break;
							}
							if(prev < posn)
							{
								writer.Write(text.Substring(prev, posn - prev));
							}
							writer.Write("&apos;");
							prev = posn + 1;
						}
						break;

						default:
						{
							if(ch < 0x20 && !Char.IsWhiteSpace(ch))
							{
								// Quote a low-ASCII control character.
								if(prev < posn)
								{
									writer.Write
										(text.Substring(prev, posn - prev));
								}
								prev = posn + 1;
								writer.Write("&#");
								writer.Write
									(((int)ch).ToString
									  (null, NumberFormatInfo.InvariantInfo));
								writer.Write(';');
							}
						}
						break;
					}
				}
				if(prev < len)
				{
					writer.Write(text.Substring(prev, len - prev));
				}
			}

	// Write a string.
	public override void WriteString(String text)
			{
				// We must not be in the closed state.
				if(writeState == System.Xml.WriteState.Closed)
				{
					if(text != null && text.Length != 0)
					{
						throw new InvalidOperationException
							(S._("Xml_InvalidWriteState"));
					}
					return;
				}

				// If we are in the attribute state, then check
				// for the "xml:lang" and "xml:space" attributes.
				if(writeState == System.Xml.WriteState.Attribute)
				{
					if(special == Special.Lang)
					{
						xmlLang = text;
						special = Special.None;
					}
					else if(special == Special.Space)
					{
						if(text == "default")
						{
							xmlSpace = System.Xml.XmlSpace.Default;
						}
						else if(text == "preserve")
						{
							xmlSpace = System.Xml.XmlSpace.Preserve;
						}
						else
						{
							xmlSpace = System.Xml.XmlSpace.None;
						}
						special = Special.None;
					}
				}

				// If we are in the element state, then shift to content.
				Sync(WriteStateFlag.ContentFlag |
					 WriteStateFlag.AttributeFlag, false);

				// Bail out if the text is empty.
				if(((Object)text) == null || text.Length == 0)
				{
					return;
				}

				// Quote the string and output it.
				WriteQuotedString(text);
			}

	// Write a surrogate character entity.
	public override void WriteSurrogateCharEntity(char lowChar, char highChar)
			{
				if(lowChar < 0xDC00 || lowChar > 0xDFFF)
				{
					throw new ArgumentException
						(S._("Xml_InvalidSurrogate"), "lowChar");
				}
				if(highChar < 0xD800 || highChar > 0xDBFF)
				{
					throw new ArgumentException
						(S._("Xml_InvalidSurrogate"), "highChar");
				}
				Sync(WriteStateFlag.ContentFlag |
					 WriteStateFlag.AttributeFlag, false);
				int ch = 0x10000 + ((highChar - 0xD800) << 10) +
							(lowChar - 0xDC00);
				writer.Write("&#x{0:X5};", ch);
			}

	// Write a sequence of white space.
	public override void WriteWhitespace(String ws)
			{
				if(ws == null || ws == String.Empty)
				{
					throw new ArgumentException
						(S._("Xml_InvalidWhitespace"), "ws");
				}
				foreach(char ch in ws)
				{
					if(!Char.IsWhiteSpace(ch))
					{
						throw new ArgumentException
							(S._("Xml_InvalidWhitespace"), "ws");
					}
				}
				Sync(WriteStateFlag.ContentFlag |
					 WriteStateFlag.AttributeFlag |
					 WriteStateFlag.PrologFlag, false);
				writer.Write(ws);
			}

	// Get the base stream underlying the text writer.
	public Stream BaseStream
			{
				get
				{
					if(writer is StreamWriter)
					{
						return ((StreamWriter)writer).BaseStream;
					}
					else
					{
						return null;
					}
				}
			}

	// Get or set the output formatting mode.
	public Formatting Formatting
			{
				get
				{
					return formatting;
				}
				set
				{
					formatting = value;
				}
			}

	// Get or set the indent character.
	public char IndentChar
			{
				get
				{
					return indentChar;
				}
				set
				{
					indentChar = value;
					for (int i = 0; i < indentation; i++)
					{
						indentChars[i] = indentChar;
					}
				}
			}

	// Get or set the indentation level.
	public int Indentation
			{
				get
				{
					return indentation;
				}
				set
				{
					if(value < 0)
					{
						throw new ArgumentException
							(S._("ArgRange_NonNegative"), "value");
					}
					indentation = value;
					indentChars = new char[indentation];
					for (int i = 0; i < indentation; i++)
					{
						indentChars[i] = indentChar;
					}
				}
			}

	// Get or set the namespace support value.
	public bool Namespaces
			{
				get
				{
					return namespaces;
				}
				set
				{
					if(writeState != System.Xml.WriteState.Start)
					{
						throw new InvalidOperationException
							(S._("Xml_InvalidWriteState"));
					}
					namespaces = value;
				}
			}

	// Get or set the quote character.
	public char QuoteChar
			{
				get
				{
					return quoteChar;
				}
				set
				{
					if(value != '"' && value != '\'')
					{
						throw new ArgumentException
							(S._("Xml_InvalidQuoteChar"), "value");
					}
					quoteChar = value;
				}
			}

	// Get the current write state.
	public override WriteState WriteState
			{
				get
				{
					return writeState;
				}
			}

	// Get the xml:lang attribute.
	public override String XmlLang
			{
				get
				{
					return xmlLang;
				}
			}

	// Get the xml:space attribute.
	public override XmlSpace XmlSpace
			{
				get
				{
					return xmlSpace;
				}
			}

	// Class that keeps track of information for the open element scopes.
	private sealed class ElementScope
	{
		public String prefix;			// The prefix for the element.
		public String localName;		// The name of the element.
		public bool scopeShown;			// True if we output the prefix.
		public int indentLevel;			// Indent level for next higher scope.
		public XmlSpace xmlSpace;		// XmlSpace for next higher scope.
		public String xmlLang;			// XmlLang for next higher scope.
		public ElementScope next;		// The next higher scope.
		public String xmlns;			// Xmlnamespace in scope.
		public bool indent;				// flag that surrounding element needs DoIndent on end
		public ElementScope(ElementScope next)
				{
					this.next = next;
				}

	}; // class ElementScope

}; // class XmlTextWriter

}; // namespace System.Xml
