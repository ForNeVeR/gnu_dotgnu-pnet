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
	private TextWriter writer;
	private System.Xml.Formatting formatting;
	private int indentation;
	private char indentChar;
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
	private bool prevWasText;
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
					(new FileStream(filename, FileMode.Open, FileAccess.Read),
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
				xmlSpace = XmlSpace.Default;
				xmlLang = null;
				pseudoNSNumber = 1;
				prevWasText = false;
				special = Special.None;
			}

	// Push a new element scope.
	private void PushScope(String prefix, String localName, bool scopeShown)
			{
				scope = new ElementScope(scope);
				scope.prefix = nameTable.Add(prefix);
				scope.localName = nameTable.Add(localName);
				scope.scopeShown = scopeShown;
				if(formatting == System.Xml.Formatting.Indented)
				{
					scope.indentLevel = indentLevel + indentation;
				}
				else
				{
					scope.indentLevel = indentLevel;
				}
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
	private void DoIndent()
			{
				if(xmlSpace != System.Xml.XmlSpace.Preserve &&
				   formatting == System.Xml.Formatting.Indented)
				{
					int indent = indentLevel;
					while(indent > 0)
					{
						writer.Write(indentChar);
						--indent;
					}
				}
			}

	// Get the name of a pseudo-prefix for a namespace URI.
	private String GetPseudoPrefix(String ns)
			{
				String prefix;
				do
				{
					prefix = nameTable.Add("n" + pseudoNSNumber.ToString());
					++pseudoNSNumber;
				}
				while(namespaceManager.HasNamespace(prefix));
				namespaceManager.AddNamespace(prefix, ns);
				return prefix;
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
	private void Sync(WriteStateFlag flags)
			{
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
				if(writeState == System.Xml.WriteState.Content)
				{
					// Record that we wrote some text to a content field.
					prevWasText = true;
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
						PopScope();
						if(xmlSpace != System.Xml.XmlSpace.Preserve)
						{
							writer.WriteLine();
						}
					}
					else if(writeState == System.Xml.WriteState.Element)
					{
						// Terminate the element start.
						writer.Write(" />");
						PopScope();
					}
					while(scope != null)
					{
						DoIndent();
						writer.Write("</");
						writer.Write(scope.localName);
						writer.Write('>');
						PopScope();
						if(xmlSpace != System.Xml.XmlSpace.Preserve)
						{
							writer.WriteLine();
						}
					}

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
				return namespaceManager.LookupPrefix(ns);
			}

	// Encode an array as base64 and write it out as text.
	public override void WriteBase64(byte[] buffer, int index, int count)
			{
				// TODO
			}

	// Encode an array as BinHex and write it out as text.
	public override void WriteBinHex(byte[] buffer, int index, int count)
			{
				// TODO
			}

	// Write out a CDATA block.
	public override void WriteCData(String text)
			{
				if(text != null && text.IndexOf("]]>") != -1)
				{
					throw new ArgumentException
						(S._("Xml_InvalidXmlWritten"), "text");
				}
				Sync(WriteStateFlag.ContentFlag);
				if(text != null)
				{
					writer.Write(text);
				}
			}

	// Write a character entity.
	public override void WriteCharEntity(char ch)
			{
				Sync(WriteStateFlag.ContentFlag |
					 WriteStateFlag.AttributeFlag);
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
					 WriteStateFlag.AttributeFlag);

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
				// Bail out if the comment text contains "-->".
				if(text != null && text.IndexOf("-->") != -1)
				{
					throw new ArgumentException
						(S._("Xml_InvalidXmlWritten"), "text");
				}

				// Synchronize to an area that allows comments.
				Sync(WriteStateFlag.ContentFlag |
					 WriteStateFlag.PrologFlag);

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
				if(writeState != System.Xml.WriteState.Prolog)
				{
					throw new InvalidOperationException
						(S._("Xml_InvalidWriteState"));
				}

				// Validate the name.
				if(!XmlReader.IsName(name))
				{
					throw new ArgumentException
						(S._("Xml_InvalidName"), "name");
				}

				// Pretend that we are in attribute mode so that
				// "WriteString" will apply the correct quoting.
				writeState = System.Xml.WriteState.Attribute;

				// Write out the document type information.
				writer.Write("<!DOCTYPE ");
				writer.Write(name);
				writer.Write(" PUBLIC \"");
				WriteString(pubid);
				writer.Write("\" \"");
				WriteString(sysid);
				writer.Write('\"');
				if(subset != null && subset.Length != 0)
				{
					writer.Write(" \"");
					WriteString(subset);
					writer.Write('\"');
				}
				writer.WriteLine('>');

				// Return to the prolog.
				writeState = System.Xml.WriteState.Prolog;
				prevWasText = false;
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
					if(xmlSpace != System.Xml.XmlSpace.Preserve)
					{
						writer.WriteLine();
					}
				}
				else if(writeState == System.Xml.WriteState.Element)
				{
					// Terminate the element start.
					writer.Write(" />");
					PopScope();
				}
				while(scope != null)
				{
					DoIndent();
					writer.Write("</");
					writer.Write(scope.localName);
					writer.Write('>');
					PopScope();
					if(xmlSpace != System.Xml.XmlSpace.Preserve)
					{
						writer.WriteLine();
					}
				}
				writeState = System.Xml.WriteState.Start;
				prevWasText = false;
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
					if(xmlSpace != System.Xml.XmlSpace.Preserve && !prevWasText)
					{
						DoIndent();
					}
					writer.Write("</");
					if(scope.scopeShown)
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
				if(xmlSpace != System.Xml.XmlSpace.Preserve)
				{
					writer.WriteLine();
				}
				writeState = System.Xml.WriteState.Content;
				prevWasText = false;
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
					 WriteStateFlag.AttributeFlag);
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
					if(xmlSpace != System.Xml.XmlSpace.Preserve && !prevWasText)
					{
						DoIndent();
					}
					writer.Write("</");
					if(scope.scopeShown)
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
				if(xmlSpace != System.Xml.XmlSpace.Preserve)
				{
					writer.WriteLine();
				}
				writeState = System.Xml.WriteState.Content;
				prevWasText = false;
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
					 WriteStateFlag.AttributeFlag);
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
					 WriteStateFlag.AttributeFlag);
				writer.Write(name);
			}

	// Write a processing instruction.
	public override void WriteProcessingInstruction(String name, String text)
			{
				// TODO
			}

	// Write a qualified name.
	public override void WriteQualifiedName(String localName, String ns)
			{
				// TODO
			}

	// Write raw string data.
	public override void WriteRaw(String data)
			{
				// TODO
			}

	// Write raw data from an array.
	public override void WriteRaw(char[] buffer, int index, int count)
			{
				// TODO
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

				// We need to be in the "Element" state.
				if(writeState == System.Xml.WriteState.Attribute)
				{
					writer.Write(quoteChar);
				}
				else if(writeState != System.Xml.WriteState.Element)
				{
					throw new InvalidOperationException
						(S._("Xml_InvalidWriteState"));
				}

				// Output the name of the attribute, with appropriate prefixes.
				writer.Write(' ');
				if(((Object)prefix) != null && ((Object)ns) != null)
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
						prefix = GetPseudoPrefix(ns);
						writer.Write("xmlns:");
						writer.Write(prefix);
						writer.Write('=');
						writer.Write(quoteChar);
						WriteString(ns);
						writer.Write(quoteChar);
						writer.Write(' ');
						writer.Write(prefix);
						writer.Write(':');
					}
				}
				else if(((Object)prefix) != null)
				{
					// We were only given a prefix, so output it directly.
					if(prefix != scope.prefix)
					{
						writer.Write(prefix);
						writer.Write(':');
					}
				}
				else if(((Object)ns) != null)
				{
					// We were only given a namespace, so find the prefix.
					prefix = LookupPrefix(ns);
					if(((Object)prefix) == null || prefix.Length == 0)
					{
						// Create a new pseudo-prefix for the URI.
						prefix = GetPseudoPrefix(ns);
						writer.Write("xmlns:");
						writer.Write(prefix);
						writer.Write('=');
						writer.Write(quoteChar);
						WriteString(ns);
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
				writer.WriteLine
					("<?xml version=\"1.0\" encoding=\"{0}\" " +
					 "standalone=\"{1}\"?>",
					 writer.Encoding.WebName,
					 (standalone ? "yes" : "no"));
			#else
				writer.WriteLine
					("<?xml version=\"1.0\" "standalone=\"{0}\"?>",
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
				writer.WriteLine("<?xml version=\"1.0\" encoding=\"{0}\"?>",
					 			 writer.Encoding.WebName);
			#else
				writer.WriteLine("<?xml version=\"1.0\"?>");
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
				if(writeState == System.Xml.WriteState.Element)
				{
					writer.Write('>');
				}
				else if(writeState != System.Xml.WriteState.Content)
				{
					throw new InvalidOperationException
						(S._("Xml_InvalidWriteState"));
				}

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
				if(xmlSpace != System.Xml.XmlSpace.Preserve && !prevWasText)
				{
					DoIndent();
				}
				writer.Write('<');
				if(((Object)prefix) != null && ((Object)ns) != null)
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
						prefix = GetPseudoPrefix(ns);
						writer.Write(prefix);
						writer.Write(':');
						writer.Write(localName);
						writer.Write(' ');
						writer.Write("xmlns:");
						writer.Write(prefix);
						writer.Write('=');
						writer.Write(quoteChar);
						WriteString(ns);
						writer.Write(quoteChar);
						scopeShown = true;
					}
				}
				else if(((Object)prefix) != null)
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
				else if(((Object)ns) != null)
				{
					// We were only given a namespace, so find the prefix.
					prefix = LookupPrefix(ns);
					if(((Object)prefix) == null || prefix.Length == 0)
					{
						// Create a new pseudo-prefix for the URI.
						prefix = GetPseudoPrefix(ns);
						writer.Write(prefix);
						writer.Write(':');
						writer.Write(localName);
						writer.Write(' ');
						writer.Write("xmlns:");
						writer.Write(prefix);
						writer.Write('=');
						writer.Write(quoteChar);
						WriteString(ns);
						writer.Write(quoteChar);
						scopeShown = true;
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
				PushScope(prefix, localName, scopeShown);
				if(scopeShown)
				{
					namespaceManager.AddNamespace(prefix, ns);
				}

				// We are now in the attribute state.
				writeState = System.Xml.WriteState.Attribute;
				prevWasText = false;
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
					 WriteStateFlag.AttributeFlag);

				// Bail out if the text is empty.
				if(((Object)text) == null || text.Length == 0)
				{
					return;
				}

				// Quote the string and output it.
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
							if(ch < 0x20)
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
					 WriteStateFlag.AttributeFlag);
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
					 WriteStateFlag.PrologFlag);
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
					namespaces = true;
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

		public ElementScope(ElementScope next)
				{
					this.next = next;
				}

	}; // class ElementScope

}; // class XmlTextWriter

}; // namespace System.Xml
