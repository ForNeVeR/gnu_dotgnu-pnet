/*
 * TestString.cs - Tests for the "System.String" class.
 *
 * Copyright (C) 2002  Free Software Foundation, Inc.
 *
 * Authors : Stephen Compall, Gopal.V, & Richard Baumann
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

using System.Reflection;
using System.Resources;
using System.Xml;

public class TestXmlConvert : TestCase
{
	// Constructor.
	public TestXmlConvert(String name)
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
	
	public void TestToSByte0()
	{
		sbyte x = XmlConvert.ToSByte("0");

		AssertEquals(x,(sbyte)0);
	}

	public void TestToSByte42()
	{
		sbyte x = XmlConvert.ToSByte("42");

		AssertEquals(x,(sbyte)42);
	}

	public void TestToSByteMinus42()
	{
		sbyte x = XmlConvert.ToSByte("-42");

		AssertEquals(x,(sbyte)-42);
	}

	public void TestToSByteOverflow()
	{
		try {
			sbyte x = XmlConvert.ToSByte("1024");

			Fail();
		} catch (OverflowException e) {
			// OK
		}
	}

	public void TestToSByteUnderflow()
	{
		try {
			sbyte x = XmlConvert.ToSByte("-1024");

			Fail();
		} catch (OverflowException e) {
			// OK
		}
	}

	public void TestToSByteJunk()
	{
		try {
			sbyte x = XmlConvert.ToSByte("Junk");

			Fail();
		} catch (FormatException e) {
			// OK
		}
	}

	public void TestToSByteNull()
	{
		try {
			String s = null;
			sbyte x = XmlConvert.ToSByte(s);

			Fail();
		} catch (ArgumentNullException e) {
			// OK
		}	
	}

}; // class TestString
