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
					"<soda><size>medium</size></soda>"};

	
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

	// Test OuterXml.
	public void TestXmlTextReaderReadOuterXml()
			{					
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

				
			}
				

}; // class TestXmlTextWriter
