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
				// Each namespace has a "SuiteXXX" class that defines
				// the tests in that namespace.  See the subdirectories
				// for these classes when adding new tests.
				TestSuite suite = new TestSuite("Runtime Tests");
				suite.AddTest(SuiteSystem.Suite());
				suite.AddTest(SuiteCollections.Suite());
				suite.AddTest(SuiteText.Suite());
				suite.AddTest(SuiteCryptography.Suite());
				return suite;
			}

}; // class Testruntime
