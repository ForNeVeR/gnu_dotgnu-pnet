/*
 * TestUri.cs - Tests for the "Uri" class.
 *
 * Copyright (C) 2002  Free Software Foundation, Inc.
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

public class TestUri : TestCase
{
	// Constructor.
	public TestUri(String name) : base(name)
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

	public void TestUriBasicParsing()
	{
		Uri basic = new Uri("ftp://rms@ftp.gnu.org:2538/pub/gnu/?freesoftware=good");
		AssertEquals("Scheme parsed", basic.Scheme, "ftp");
		AssertEquals("Host parsed", basic.Host, "ftp.gnu.org");
		AssertEquals("AbsolutePath parsed", basic.AbsolutePath, "/pub/gnu/");
		AssertEquals("Query parsed", basic.Query, "?freesoftware=good");
		AssertEquals("Port parsed", basic.Port, 2538);
		AssertEquals("User info parsed", basic.UserInfo, "rms");
		AssertEquals("Authority built correctly", basic.Authority, "rms@ftp.gnu.org:2538");
	}

}; // class TestUri
