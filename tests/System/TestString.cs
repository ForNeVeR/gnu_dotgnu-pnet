/*
 * TestBoolean.cs - Tests for the "Boolean" class.
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

public class TestString : TestCase
{
	// Constructor.
	public TestString(String name)
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

	public void TestIndexOfAcceptsLength()
	{
		try
			"x".IndexOf(a, 1);
		catch (ArgumentOutOfRangeException aoore)
			Fail("IndexOf(char, int) should not throw when passed Length as the int");
	}

}; // class TestBoolean
