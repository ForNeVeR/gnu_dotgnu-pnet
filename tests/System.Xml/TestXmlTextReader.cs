/*
 * TestXmlTextReader.cs - Tests for the
 *		"System.Xml.TestXmlTextReader" class.
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

public class TestXmlTextReader : TestCase
{
	private XmlTextReader xmlReader;
	private StringReader stringReader;
	
	enum ReturnType : int
			{
				Name = 0,
				Value = 1,
				Innertext = 2,
				InnerXml = 3,
				OuterXml = 4,
				LocalName = 5,
				Prefix = 6
			};
		
	private String[] xml = {"<soda caffeine=\"yes\">\n<size>medium</size>\n</soda>", 
					"<soda><size>medium</size></soda>",
					"<free>software's freedom</free>",
					"<?xml version=\"1.0\" ?><bookstore><book><title>Understanding The Linux Kernel</title>
					<author>Daniel P. Bovet and Marco Cesati</author></book><book><title>Learning Perl</title>
					<author>Randal L. Schwartz and Tom Christiansen</author></book></bookstore>"};

	
	// Constructor.
	public TestXmlTextReader(String name)
			: base(name)
			{
				// Nothing to do here.
			}

	// Set up for the tests.
	protected override void Setup()
			{
				stringReader = new StringReader(xml[0]);
				xmlReader = new XmlTextReader(stringReader);
			}

	// Clean up after the tests.
	protected override void Cleanup()
			{
				// Nothing to do here.
			}

	// Check that the current contents of the output is a particular string.
	private void Check(String msg, String expected, ReturnType retType)
			{
				String actual;
				switch (retType)
				{
					case ReturnType.Name:
						actual = xmlReader.Name;
						break;
					case ReturnType.Value:
						actual = xmlReader.Value;
						break;
					case ReturnType.InnerXml:
						actual = xmlReader.ReadInnerXml();
						break;
					case ReturnType.OuterXml:
						actual = xmlReader.ReadOuterXml();
						break;
					case ReturnType.LocalName:
						actual = xmlReader.LocalName;
						break;
					case ReturnType.Prefix:
						actual = xmlReader.Prefix;
						break;
					default:
						// do nothing
						break;
				}
				
				AssertEquals(msg, expected, actual);
			}

	// Clear the current output.
	private void Clear()
			{
				xmlReader = null;
			}

	// Reset the entire XML text reader.
	private void Reset()
			{
				xmlReader = new XmlTextReader(stringReader);
			}

	// Test the constructors
	public void TestXmlTextReaderCtor()
			{
				// TODO: Test each constructor
			}


	// Test OuterXml.
	public void TestXmlTextReaderReadOuterXml()
			{		
				Reset();
				xmlReader.Read();
				Check("ReadOuterXml (1)", xml[0], ReturnType.OuterXml);
			
				Reset();
				stringReader = new StringReader(xml[1]);
				xmlReader = new XmlTextReader(stringReader);
				
				xmlReader.Read();
				Check("ReadOuterXml (2)", xml[1], ReturnType.OuterXml);

				Clear();
					
			}

	public void TestXmlTextReaderRead()
			{
				bool ret = xmlReader.Read();
				AssertEquals("Read (1)", true, ret);
				
				Check("Read (2)", "soda", ReturnType.Name);
				Check("Read (3)", "soda", ReturnType.LocalName);
				
				ret = xmlReader.Read();
				AssertEquals("Read (4)", true, ret);
			
				Check("Read (5)", "size", ReturnType.Name);
				Check("Read (6)", "size", ReturnType.LocalName);

				ret = xmlReader.Read();
				AssertEquals("Read (7)", true, ret);

				Check("Read (8)", "medium", ReturnType.Value);
				
				Reset();
				stringReader = new StringReader(xml[2]);
				xmlReader = new XmlTextReader(stringReader);

				xmlReader.Read();
				Check("Read (9)", "free", ReturnType.Name);
				xmlReader.Read();
				Check("Read (9)", "software's freedom", ReturnType.Value);

				Clear();
			}

	public void TestXmlTextReaderReadAttributeValue()
			{
				stringReader = new StringReader(xml[0]);
				xmlReader = new XmlTextReader(stringReader);
				xmlReader.Read();
				xmlReader.MoveToFirstAttribute();
				xmlReader.ReadAttributeValue();
				AssertEquals("Read (1)", XmlNodeType.Text, xmlReader.NodeType);
			}	
	
	public void TestXmlTextReaderGetRemainder()
			{
				stringReader = new StringReader(xml[3]);
				xmlReader = new XmlTextReader(stringReader);
				bool ret = xmlReader.Read();
				AssertEquals("Read (1)", true, ret);
				xmlReader.Read();
				xmlReader.Read();
				Check("Read (2)", "book", ReturnType.Name);
				xmlReader.Read();
				Check("Read (3)", "title", ReturnType.Name);
				xmlReader.Read();
				Check("Read (4)", "Understanding The Linux Kernel", ReturnType.Value);
				xmlReader.Read();
				Check("Read (5)", "title", ReturnType.Name);
				xmlReader.Read();
				Check("Read (6)", "author", ReturnType.Name);
				xmlReader.Read();
				Check("Read (7)", "Daniel P. Bovet and Marco Cesati", ReturnType.Value);
				xmlReader.Read();
				Check("Read (8)", "author", ReturnType.Name);
				xmlReader.Read();
				Check("Read (9)", "book", ReturnType.Name);

				xmlReader = new XmlTextReader(xmlReader.GetRemainder());
				xmlReader.Read();
				Check("Read (2)", "book", ReturnType.Name);
				xmlReader.Read();
				Check("Read (3)", "title", ReturnType.Name);
				xmlReader.Read();
				Check("Read (4)", "Learning Perl", ReturnType.Value);
				xmlReader.Read();
				Check("Read (5)", "title", ReturnType.Name);
				xmlReader.Read();
				Check("Read (6)", "author", ReturnType.Name);
				xmlReader.Read();
				Check("Read (7)", "Randal L. Schwartz and Tom Christiansen", ReturnType.Value);
				xmlReader.Read();
				Check("Read (8)", "author", ReturnType.Name);
				xmlReader.Read();
				Check("Read (9)", "book", ReturnType.Name);
				xmlReader.Close();

				Clear();
			}

}; // class TestXmlTextWriter
