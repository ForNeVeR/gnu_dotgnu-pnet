/*
 * TestXmlDocument.cs - Tests for the "System.Xml.XmlDocument" class.
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
using System.Xml;

#if !ECMA_COMPAT

public class TestXmlDocument : TestCase
{
	// Constructor.
	public TestXmlDocument(String name)
			: base(name)
			{
				// Nothing to do here.
			}

	// Set up for the tests.
	protected override void Setup()
			{
				// Nothing to do here.
			}

	// Clean up after the tests.
	protected override void Cleanup()
			{
				// Nothing to do here.
			}

	// Test document construction.
	public void TestXmlDocumentConstruct()
			{
				XmlDocument doc1, doc2;

				// Simple creation.
				doc1 = new XmlDocument();
				AssertNotNull("Construct (1)", doc1.Implementation);

				// Create a document from the same name table,
				// but a different implementation.
				doc2 = new XmlDocument(doc1.NameTable);
				AssertEquals("Construct (2)", doc1.NameTable,
							 doc2.NameTable);
			}

	// Test the properties of an XmlDocument node.
	public void TestXmlDocumentProperties()
			{
				XmlDocument doc = new XmlDocument();

				// Verify the initial conditions.
				AssertNull("Properties (1)", doc.Attributes);
				AssertEquals("Properties (2)", doc.BaseURI, String.Empty);
				AssertNotNull("Properties (3)", doc.ChildNodes);
				AssertNull("Properties (4)", doc.DocumentElement);
				AssertNull("Properties (5)", doc.DocumentType);
				AssertNull("Properties (6)", doc.FirstChild);
				Assert("Properties (7)", !doc.HasChildNodes);
				Assert("Properties (8)", !doc.IsReadOnly);
				AssertEquals("Properties (9)", doc.LocalName, "#document");
				AssertEquals("Properties (10)", doc.Name, "#document");
				AssertEquals("Properties (11)", doc.NamespaceURI,
							 String.Empty);
				AssertNull("Properties (12)", doc.NextSibling);
				AssertEquals("Properties (13)", doc.NodeType,
							 XmlNodeType.Document);
				AssertNull("Properties (13)", doc.OwnerDocument);
				AssertNull("Properties (14)", doc.ParentNode);
				AssertEquals("Properties (15)", doc.Prefix, String.Empty);
				AssertNull("Properties (16)", doc.PreviousSibling);
				AssertEquals("Properties (17)", doc.Value, String.Empty);
			}

}; // class TestXmlDocument

#endif // !ECMA_COMPAT
