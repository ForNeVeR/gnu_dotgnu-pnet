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

	// Test adding an XML declaration to the document.
	public void TestXmlDocumentAddXmlDeclaration()
			{
				XmlDocument doc = new XmlDocument();

				// Add the declaration.
				XmlDeclaration decl =
					doc.CreateXmlDeclaration("1.0", null, null);
				AssertNull("XmlDeclaration (1)", decl.ParentNode);
				AssertEquals("XmlDeclaration (2)", doc, decl.OwnerDocument);
				doc.AppendChild(decl);
				AssertEquals("XmlDeclaration (3)", doc, decl.ParentNode);
				AssertEquals("XmlDeclaration (4)", doc, decl.OwnerDocument);

				// Try to add it again, which should fail this time.
				try
				{
					doc.AppendChild(decl);
					Fail("adding XmlDeclaration node twice");
				}
				catch(InvalidOperationException)
				{
					// Success
				}
				try
				{
					doc.PrependChild(decl);
					Fail("prepending XmlDeclaration node twice");
				}
				catch(InvalidOperationException)
				{
					// Success
				}

				// Adding a document type before should fail.
				XmlDocumentType type =
					doc.CreateDocumentType("foo", null, null, null);
				try
				{
					doc.PrependChild(type);
					Fail("prepending XmlDocumentType");
				}
				catch(InvalidOperationException)
				{
					// Success
				}

				// Adding a document type after should succeed.
				doc.AppendChild(type);

				// Adding an element before should fail.
				XmlElement element = doc.CreateElement("foo");
				try
				{
					doc.PrependChild(element);
					Fail("prepending XmlElement");
				}
				catch(InvalidOperationException)
				{
					// Success
				}

				// Adding the element between decl and type should fail.
				try
				{
					doc.InsertAfter(element, decl);
					Fail("inserting XmlElement between XmlDeclaration " +
						 "and XmlDocumentType");
				}
				catch(InvalidOperationException)
				{
					// Success
				}
				try
				{
					doc.InsertBefore(element, type);
					Fail("inserting XmlElement between XmlDeclaration " +
						 "and XmlDocumentType (2)");
				}
				catch(InvalidOperationException)
				{
					// Success
				}

				// Adding an element after should succeed.
				doc.AppendChild(element);
			}

	// Test adding a document type to the document.
	public void TestXmlDocumentAddDocumentType()
			{
				XmlDocument doc = new XmlDocument();

				// Add the document type.
				XmlDocumentType type =
					doc.CreateDocumentType("foo", null, null, null);
				AssertNull("XmlDocumentType (1)", type.ParentNode);
				AssertEquals("XmlDocumentType (2)", doc, type.OwnerDocument);
				doc.AppendChild(type);
				AssertEquals("XmlDocumentType (3)", doc, type.ParentNode);
				AssertEquals("XmlDocumentType (4)", doc, type.OwnerDocument);

				// Try to add it again, which should fail this time.
				try
				{
					doc.AppendChild(type);
					Fail("adding XmlDocumentType node twice");
				}
				catch(InvalidOperationException)
				{
					// Success
				}
				try
				{
					doc.PrependChild(type);
					Fail("prepending XmlDocumentType node twice");
				}
				catch(InvalidOperationException)
				{
					// Success
				}

				// Adding an XmlDeclaration after should fail.
				XmlDeclaration decl =
					doc.CreateXmlDeclaration("1.0", null, null);
				try
				{
					doc.AppendChild(decl);
					Fail("appending XmlDeclaration after XmlDocumentType");
				}
				catch(InvalidOperationException)
				{
					// Success
				}

				// But adding XmlDeclaration before should succeed.
				doc.PrependChild(decl);
			}

	// Test adding an element to the document.
	public void TestXmlDocumentAddElement()
			{
				XmlDocument doc = new XmlDocument();

				// Add an element to the document.
				XmlElement element = doc.CreateElement("foo");
				AssertNull("XmlElement (1)", element.ParentNode);
				AssertEquals("XmlElement (2)", doc, element.OwnerDocument);
				doc.AppendChild(element);
				AssertEquals("XmlElement (3)", doc, element.ParentNode);
				AssertEquals("XmlElement (4)", doc, element.OwnerDocument);

				// Try to add it again, which should fail this time.
				try
				{
					doc.AppendChild(element);
					Fail("adding XmlElement node twice");
				}
				catch(InvalidOperationException)
				{
					// Success
				}
				try
				{
					doc.PrependChild(element);
					Fail("prepending XmlElement node twice");
				}
				catch(InvalidOperationException)
				{
					// Success
				}

				// Adding an XmlDeclaration after should fail.
				XmlDeclaration decl =
					doc.CreateXmlDeclaration("1.0", null, null);
				try
				{
					doc.AppendChild(decl);
					Fail("appending XmlDeclaration after XmlElement");
				}
				catch(InvalidOperationException)
				{
					// Success
				}

				// But adding XmlDeclaration before should succeed.
				doc.PrependChild(decl);

				// Adding a document type after should fail.
				XmlDocumentType type =
					doc.CreateDocumentType("foo", null, null, null);
				try
				{
					doc.AppendChild(type);
					Fail("appending XmlDocumentType");
				}
				catch(InvalidOperationException)
				{
					// Success
				}

				// Adding a document type before should succeed.
				doc.InsertBefore(type, element);
			}

}; // class TestXmlDocument

#endif // !ECMA_COMPAT
