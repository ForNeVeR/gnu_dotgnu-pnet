/*
 * Testruntime.cs - Tests for the "System" namespace.
 *
 * Copyright (C) 2001  Southern Storm Software, Pty Ltd.
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

public class Testruntime
{

	public static TestSuite Suite()
			{
				TestSuite suite = new TestSuite("Basic Runtime Tests");
				suite.AddTests(typeof(TestBoolean));
				suite.AddTests(typeof(TestGuid));
				suite.AddTests(typeof(TestSByte));
				suite.AddTests(typeof(TestString));
				suite.AddTests(typeof(TestStringBuilder));
				suite.AddTests(typeof(TestArrayList));
				suite.AddTest(TestCryptography.Suite());
				return suite;
			}

}; // class Testruntime

