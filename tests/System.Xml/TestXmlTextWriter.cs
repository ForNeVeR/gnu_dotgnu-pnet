/*
 * TestXmlTextWriter.cs - Tests for the
 *		"System.Xml.TestXmlTextWriter" class.
 *
 * Copyright (C) 2002  Southern Storm Software, Pty Ltd.
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

using CSUnit;
using System;
using System.IO;
using System.Text;
using System.Xml;

public class TestXmlTextWriter : TestCase
{
	// Internal state.
	private StringWriter stringWriter;
	private XmlTextWriter xml;

	// Constructor.
	public TestXmlTextWriter(String name)
			: base(name)
			{
				// Nothing to do here.
			}

	// Set up for the tests.
	protected override void Setup()
			{
				stringWriter = new StringWriter();
				stringWriter.NewLine = "\n";
				xml = new XmlTextWriter(stringWriter);
			}

	// Clean up after the tests.
	protected override void Cleanup()
			{
				// Nothing to do here.
			}

	// Check that the current contents of the output is a particular string.
	private void Check(String msg, String expected)
			{
				xml.Flush();
				String actual = stringWriter.GetStringBuilder().ToString();
				AssertEquals(msg, expected, actual);
			}

	// Clear the current output.
	private void Clear()
			{
				xml.Flush();
				StringBuilder builder = stringWriter.GetStringBuilder();
				builder.Remove(0, builder.Length);
			}

	// Reset the entire XML text writer.
	private void Reset()
			{
				stringWriter = new StringWriter();
				stringWriter.NewLine = "\n";
				xml = new XmlTextWriter(stringWriter);
			}

	// Test the XmlTextWriter.WriteProcessingInstruction
	public void TestXmlTextWriterWriteProcessingInstruction()
			{
				Clear();
				String name = "xsl-stylesheet";
				String text = "href=\"test.xsl\"";
				xml.WriteStartDocument();
				xml.WriteProcessingInstruction(name, text);
#if !ECMA_COMPAT
				Check("ProcessingInstruction (1)",
						"<?xml version=\"1.0\" encoding=\"utf-16\"?>\n<?xsl-stylesheet href=\"test.xsl\"?>\n");
#else
				Check("ProcessingInstruction (1)",
						"<?xml version=\"1.0\"?>\n<?xsl-stylesheet href=\"test.xsl\"?>\n");
#endif
			}
	
	// Test the property values on the writer.
	public void TestXmlTextWriterProperties()
			{
				// Check the initial values.
				AssertNull("Properties (1)", xml.BaseStream);
				AssertEquals("Properties (2)", Formatting.None, xml.Formatting);
				AssertEquals("Properties (3)", ' ', xml.IndentChar);
				AssertEquals("Properties (4)", 2, xml.Indentation);
				Assert("Properties (5)", xml.Namespaces);
				AssertEquals("Properties (6)", '"', xml.QuoteChar);
				AssertEquals("Properties (7)",
							 WriteState.Start, xml.WriteState);
				AssertNull("Properties (8)", xml.XmlLang);
				AssertEquals("Properties (9)",
							 XmlSpace.None, xml.XmlSpace);

				// Check that we can modify the settable properties.
				xml.Formatting = Formatting.Indented;
				AssertEquals("Set Formatting (1)",
							 Formatting.Indented, xml.Formatting);
				xml.Formatting = Formatting.None;
				AssertEquals("Set Formatting (2)",
							 Formatting.None, xml.Formatting);

				xml.IndentChar = 'A';
				AssertEquals("Set IndentChar (1)", 'A', xml.IndentChar);
				xml.IndentChar = ' ';
				AssertEquals("Set IndentChar (2)", ' ', xml.IndentChar);

				xml.Indentation = 0;
				AssertEquals("Set Indentation (1)", 0, xml.Indentation);
				xml.Indentation = 1;
				AssertEquals("Set Indentation (2)", 1, xml.Indentation);
				xml.Indentation = 1234;
				AssertEquals("Set Indentation (3)", 1234, xml.Indentation);
				try
				{
					xml.Indentation = -1;
					Fail("Set Indentation (4)");
				}
				catch(ArgumentException)
				{
					// Success
				}

				xml.Namespaces = false;
				Assert("Set Namespaces (1)", !xml.Namespaces);
				xml.Namespaces = true;
				Assert("Set Namespaces (2)", xml.Namespaces);

				xml.QuoteChar = '\'';
				AssertEquals("Set QuoteChar (1)", '\'', xml.QuoteChar);
				xml.QuoteChar = '"';
				AssertEquals("Set QuoteChar (2)", '"', xml.QuoteChar);
				try
				{
					xml.QuoteChar = 'A';
					Fail("Set QuoteChar (3)");
				}
				catch(ArgumentException)
				{
					// Success
				}
			}

	// Test the "WriteStartDocument" method.
	public void TestXmlTextWriterStartDocument()
			{
				// Write a document start with no standalone indication.
				xml.WriteStartDocument();
			#if !ECMA_COMPAT
				Check("StartDocument (1)",
					  "<?xml version=\"1.0\" encoding=\"utf-16\"?>\n");
			#else
				Check("StartDocument (1)",
					  "<?xml version=\"1.0\"?>\n");
			#endif
				AssertEquals("StartDocument (2)",
							 WriteState.Prolog, xml.WriteState);
				Reset();

				// Write a document start with a true standalone indication.
				xml.WriteStartDocument(true);
			#if !ECMA_COMPAT
				Check("StartDocument (3)",
					  "<?xml version=\"1.0\" encoding=\"utf-16\" " +
					  "standalone=\"yes\"?>\n");
			#else
				Check("StartDocument (3)",
					  "<?xml version=\"1.0\" standalone=\"yes\"?>\n");
			#endif
				AssertEquals("StartDocument (4)",
							 WriteState.Prolog, xml.WriteState);
				Reset();

				// Write a document start with a false standalone indication.
				xml.WriteStartDocument(false);
			#if !ECMA_COMPAT
				Check("StartDocument (5)",
					  "<?xml version=\"1.0\" encoding=\"utf-16\" " +
					  "standalone=\"no\"?>\n");
			#else
				Check("StartDocument (5)",
					  "<?xml version=\"1.0\" standalone=\"no\"?>\n");
			#endif
				AssertEquals("StartDocument (6)",
							 WriteState.Prolog, xml.WriteState);

				// Cannot start a document during the prolog.
				try
				{
					xml.WriteStartDocument();
					Fail("StartDocument (7)");
				}
				catch(InvalidOperationException)
				{
					// Success
				}

				// End the document (not possible because no root element).
				try
				{
					xml.WriteEndDocument();
					Fail("StartDocument (8)");
				}
				catch(ArgumentException)
				{
					// Success
				}

				// Add a root element and retry.
				Clear();
				xml.WriteStartElement("foo");
				xml.WriteEndDocument();
				Check("StartDocument (9)", "<foo />\n");
				AssertEquals("StartDocument (10)",
							 WriteState.Start, xml.WriteState);
			}

}; // class TestXmlTextWriter
