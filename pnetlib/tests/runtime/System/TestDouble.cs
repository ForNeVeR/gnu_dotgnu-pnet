/*
 * TestDouble.cs - Test class for "System.Double" 
 *
 * Copyright (C) 2002  Southern Storm Software, Pty Ltd.
 * Copyright (C) 2002  FSF.
 * 
 * Authors : Autogenerated using csdoc2test 
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

public class TestDouble : TestCase
 {
	// Constructor.
	public TestDouble(String name)	: base(name)
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

#if CONFIG_EXTENDED_NUMERICS
	public void TestDoubleParse()
	{
		AssertEquals("Double.Parse(\"1.0\")",
				1.0d, Double.Parse("1.0"), 0.00000001d);
		AssertEquals("Double.Parse(\"-1.0\")",
				-1.0d, Double.Parse("-1.0"), 0.00000001d);
		AssertEquals("Double.Parse(\"1.0E+2\")",
				100d, Double.Parse("1.0E+2"), 0.00000001d);
		AssertEquals("Double.Parse(\"0.0\")",
				0.0d, Double.Parse("0.0"), 0.00000001d);
		AssertEquals("Double.Parse(\"0.5\")",
				0.5d, Double.Parse("0.5"), 0.00000001d);
		AssertEquals("Double.Parse(\"0.500000000000000\")",
				0.5d, Double.Parse("0.500000000000000"), 0.00000001d);
		AssertEquals("Double.Parse(\"1.13\")",
				1.13d, Double.Parse("1.13"), 0.00000001d);
		AssertEquals("Double.Parse(\"1.130000000000000\")",
				1.13d, Double.Parse("1.130000000000000"), 0.00000001d);
		AssertEquals("Double.Parse(\"1e+2\")",
				100d, Double.Parse("1e+2"), 0.00000001d);
		AssertEquals("Double.Parse(\"1e-2\")",
				0.01d, Double.Parse("1e-2"), 0.00000001d);
		AssertEquals("Double.Parse(\"1e2\")",
				100d, Double.Parse("1e2"), 0.00000001d);
	}

	public void TestDoubleToString()
	{
		double d;

		d = 0.0d;
		AssertEquals("0", "0", d.ToString());

		d = 1.0d;
		AssertEquals("1", "1", d.ToString());

		d = 1.13d;
		AssertEquals("1.13", "1.13", d.ToString());
		AssertEquals("d=1.13; d.ToString(\"R\",null)", "1.13", d.ToString("R", null));

		//d = -1.13d;
		//AssertEquals("-1.13", "-1.13", d.ToString());
	}

	public void TestDoubleIsNaN()
	{
		Assert("!IsNaN(1.0)", !Double.IsNaN(1.0));
		Assert("IsNaN(Double.NaN)", Double.IsNaN(Double.NaN));
	}

	public void TestDoubleIsNegativeInfinity()
	{
		Assert("!IsNegativeInfinity(1.0)", !Double.IsNegativeInfinity(1.0));
		Assert("!IsNegativeInfinity(Double.NaN)", !Double.IsNegativeInfinity(Double.NaN));
		Assert("!IsNegativeInfinity(Double.PositiveInfinity)", !Double.IsNegativeInfinity(Double.PositiveInfinity));
		Assert("IsNegativeInfinity(Double.NegativeInfinity)", Double.IsNegativeInfinity(Double.NegativeInfinity));
	}

	public void TestDoubleIsPositiveInfinity()
	{
		Assert("!IsPositiveInfinity(1.0)", !Double.IsPositiveInfinity(1.0));
		Assert("!IsPositiveInfinity(Double.NaN)", !Double.IsPositiveInfinity(Double.NaN));
		Assert("!IsPositiveInfinity(Double.NegativeInfinity)", !Double.IsPositiveInfinity(Double.NegativeInfinity));
		Assert("IsPositiveInfinity(Double.PositiveInfinity)", Double.IsPositiveInfinity(Double.PositiveInfinity));
	}

#endif // CONFIG_EXTENDED_NUMERICS
}
